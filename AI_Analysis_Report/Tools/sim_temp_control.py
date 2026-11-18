#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
sim_temp_control.py  --  SL-1000 C7 烘干(通道1)温度链路 纯软件仿真 / 参数辨识

把 firmware 逐行搬进模型, 用确定性数值实验代替"可能/大概":

  Q1  设定 75℃ 时, 传感器读数最高能到多少?        -> 由控制律决定, 可精确算
  Q2  真实温度冲到 90+ 需要什么硬件条件?           -> 解析边界 + 扫描验证
  Q3  有哪些代码路径能让 80/92℃ 保护完全失效?       -> 穷举 + 概率

源码依据
  TaskTempModule.c L211-291  FanTempTask
  piheat_pid.c     L92-107   PidLocFan
  pwm.c            L76-166   KernelProg / Write / Stop
  tempfan.c        L165-232  ReadNtcFan (NTC->ADC->0.1℃)
  devinit.c        L136-155  PWMInit (dev1 = PB14, active high)
  SysData.c        L48-65    con_stC7SysPareInit (650 / 750)
"""
import json
import math
import os

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sim_out")
os.makedirs(OUT, exist_ok=True)

RP, T2, BETA, KELV = 10000.0, 273.15 + 25.0, 3950.0, 273.15
VREF, ADC_FULL = 3.3, 4095.0
OPEN_VALUE, SHORT_VALUE = 10, 4000


# ---------------------------------------------------------------- NTC 链路
def t_to_adc(t_c):
    rt = RP * math.exp(BETA * (1.0 / (t_c + KELV) - 1.0 / T2))
    vo = VREF * rt / (10000.0 + rt)
    return max(0, min(4095, int(round(vo / VREF * ADC_FULL))))


def adc_to_temp(adc):
    if adc <= OPEN_VALUE or adc >= SHORT_VALUE:
        return 3000
    vo = adc / ADC_FULL * VREF
    if vo >= VREF:
        return 3000
    rt = 10000.0 * vo / (VREF - vo)
    return int(((1.0 / (math.log(rt / RP) / BETA + 1.0 / T2)) - KELV + 0.5) * 10.0)


def t_to_meas(t_c):
    return adc_to_temp(t_to_adc(t_c))


# ---------------------------------------------------------------- PWM 通道
class PwmChannel:
    def __init__(self, dev=1, active_level=1):
        self.dev, self.active = dev, active_level
        self.running = False
        self.spd = 0
        self.port = 1 - active_level

    def write(self, spd, tim=50000):
        if spd == 0:
            return -1                    # pwm.c L129
        self.spd = 100 if spd > 100 else int(spd)
        self.running = True
        return 0

    def stop(self):
        self.running = False
        self.port = 1 - self.active

    @property
    def duty(self):
        return 0.0 if not self.running else self.spd / 100.0


# ---------------------------------------------------------------- 任务状态机
class FanTempTask:
    """TaskTempModule.c L211-291 的等价实现"""
    def __init__(self, pwm, tset_ram=750, w_heat_fan_temp=750, by_set_derat=0):
        self.pwm = pwm
        self.Temset = float(tset_ram)
        self.wHeatFanTemp = w_heat_fan_temp      # L253 比较用的全局参数
        self.bySetDerat = by_set_derat           # L234/253/259 读的是 s_stTempDat
        self.wPwm = self.wPwmOld = 0
        self.TempCheck = True
        self.TimeCount = 0
        self.wTempFan = 0
        self.k1 = 0
        self.Temin = 0.0
        self.dev_sum = self.dev_last = 0.0
        self.bHeatFanSta = True
        self.err = 0
        self.ev = []
        self.data1 = 0

    def _pid_loc_fan(self):
        if self.Temset == 0:
            return 0.0
        dev = (self.Temset - self.Temin) / 10.0          # 单位 摄氏度
        self.dev_sum += dev
        fout = 20.0 * dev + 0.0 * self.dev_sum + 0.0 * (self.dev_last - dev) + 5.0
        self.dev_last = dev
        if fout > 300:
            self.dev_sum = 300.0
        return fout

    def step(self, t_ms, meas):
        if not self.bHeatFanSta:
            self.wTempFan = meas
            return
        self.k1 += 1
        if self.k1 == 5:                      # 100ms 采样一次
            self.k1 = 0
            self.wTempFan = meas
            if self.TimeCount == 0:
                self.data1 = meas
            elif self.TimeCount == 1200 and self.TempCheck:
                self.TempCheck = False
                data2 = meas
                if (data2 - self.data1) < 50 and self.data1 < 650 and self.bySetDerat == 0:
                    self.TimeCount = 0
                    self.pwm.stop()
                    self.bHeatFanSta = False
                    self.err = 0x05
                    self.ev.append((t_ms, "ERR_C7_TEMPFAN"))
            elif meas == 3000:
                self.err = 0x19
                self.ev.append((t_ms, "ERR_C7_TEMP_DRYING_ERR"))
            if self.TempCheck:
                self.TimeCount += 1

        self.Temin = self.wTempFan
        self.wPwm = int(self._pid_loc_fan()) * 2
        if self.wPwm > 100:
            self.wPwm = 100
        elif self.wPwm < 0:
            self.wPwm = 0

        if self.wTempFan > (self.wHeatFanTemp + 50) and self.bySetDerat == 0:
            self.pwm.stop()
            self.ev.append((t_ms, "CLAMP_SP+5"))
        elif self.wTempFan > 370 and self.bySetDerat == 1:
            self.pwm.stop()
            self.ev.append((t_ms, "CLAMP_37C"))
        elif self.wTempFan > 920:
            self.pwm.stop()
            self.bHeatFanSta = False
            self.ev.append((t_ms, "HARD_92C"))
        elif self.wPwmOld != self.wPwm:
            self.wPwmOld = self.wPwm
            if self.wPwm == 0:
                self.pwm.stop()
            else:
                self.pwm.write(self.wPwm, 50000)


# ---------------------------------------------------------------- 热学对象
class Plant:
    """两节点: 加热体 Th --(hA)--> 风道 Ta --(k_l)--> 环境; 传感器=Ta 延迟+一阶滞后"""
    def __init__(self, Pmax=300.0, C_h=60.0, C_a=600.0, hA=6.0, k_l=3.0,
                 tau_s=2.0, tau_d=0.5, Tamb=25.0, load=0.0):
        self.Pmax, self.C_h, self.C_a = Pmax, C_h, C_a
        self.hA, self.k_l = hA, k_l
        self.tau_s, self.tau_d = tau_s, tau_d
        self.Tamb, self.load = Tamb, load
        self.Th = self.Ta = self.Ts = Tamb
        self.nbuf = max(1, int(self.tau_d / 0.005) + 1)
        self.buf = [Tamb] * self.nbuf
        self.bidx = 0

    def soak(self, t_air, duty=0.15):
        """热机启动: 风道已在 t_air, 加热体处于该占空比下的平衡温度"""
        self.Th = t_air + duty * self.Pmax / self.hA
        self.Ta = self.Ts = t_air
        self.buf = [t_air] * self.nbuf

    def advance(self, dt, duty):
        ta_d = self.buf[self.bidx]
        self.buf[self.bidx] = self.Ta
        self.bidx = (self.bidx + 1) % self.nbuf
        p = self.Pmax * duty
        self.Th += dt * (p - self.hA * (self.Th - self.Ta)) / self.C_h
        self.Ta += dt * (self.hA * (self.Th - self.Ta)
                         - self.k_l * (self.Ta - self.Tamb) - self.load) / self.C_a
        self.Ts += dt * (ta_d - self.Ts) / self.tau_s

    def stored(self):
        return self.C_h * (self.Th - self.Ta)


# ---------------------------------------------------------------- 闭环
def run(tset_ram=750, w_heat_fan_temp=None, plant=None, duration_s=900,
        dt=0.005, start_air=None, soak_duty=0.15, trace=None,
        by_set_derat=0):
    if w_heat_fan_temp is None:
        w_heat_fan_temp = tset_ram
    pwm = PwmChannel()
    task = FanTempTask(pwm, tset_ram, w_heat_fan_temp, by_set_derat)
    plant = plant or Plant()
    if start_air is not None:
        plant.soak(start_air, soak_duty)

    meas = t_to_meas(plant.Ts)
    n = int(duration_s / dt)
    task_dt, next_task = 0.020, 0.0
    rows = []
    peak_s = peak_a = plant.Ts
    t_peak = 0.0
    cut_e = 0.0
    cut_meas = None
    first_cut = True
    sample_every = max(1, int(0.1 / dt))
    prev_ev = 0

    for i in range(n):
        t = i * dt
        if t >= next_task:
            next_task += task_dt
            task.step(t * 1000.0, meas)
            if first_cut and len(task.ev) > prev_ev:
                for _, k in task.ev[prev_ev:]:
                    if k in ("CLAMP_SP+5", "HARD_92C", "CLAMP_37C"):
                        cut_e, cut_meas, first_cut = plant.stored(), task.wTempFan, False
                        break
            prev_ev = len(task.ev)
        plant.advance(dt, pwm.duty)
        meas = t_to_meas(plant.Ts)
        peak_s = max(peak_s, plant.Ts)
        if plant.Ta > peak_a:
            peak_a, t_peak = plant.Ta, t
        if i % sample_every == 0:
            rows.append((round(t, 2), round(plant.Th, 2), round(plant.Ta, 2),
                         round(plant.Ts, 2), meas, round(pwm.duty * 100, 1),
                         task.wPwm, int(task.bHeatFanSta)))

    res = dict(name=trace or "run", tset_ram=tset_ram,
               peak_sensor=round(peak_s, 2), peak_true=round(peak_a, 2),
               t_peak_s=round(t_peak, 1), cut_meas=cut_meas,
               cut_energy_J=round(cut_e, 1), err=task.err,
               events=sorted({k for _, k in task.ev}))
    if trace:
        with open(os.path.join(OUT, trace + ".csv"), "w", encoding="utf-8") as f:
            f.write("t_s,Th,Ta,Ts,meas_0.1C,duty_pct,wPwm,bHeatFanOn\n")
            for r in rows:
                f.write(",".join(str(x) for x in r) + "\n")
    return res


def sweep_hA():
    out = []
    for hA in (1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 5.0, 6.0, 8.0, 10.0, 14.0):
        pl = Plant(hA=hA)
        r = run(tset_ram=750, plant=pl, duration_s=600)
        r["hA"] = hA
        r["theory_dT"] = round(pl.C_h * (pl.Pmax / hA) / pl.C_a, 2)
        out.append(r)
    return out


def sweep_Ca():
    out = []
    for Ca in (300.0, 400.0, 600.0, 900.0, 1200.0):
        for hA in (2.0, 4.0, 8.0):
            pl = Plant(C_a=Ca, hA=hA)
            r = run(tset_ram=750, plant=pl, duration_s=600)
            r.update(C_a=Ca, hA=hA,
                     theory_dT=round(pl.C_h * (pl.Pmax / hA) / pl.C_a, 2))
            out.append(r)
    return out


def main():
    S = {}

    # A. 冷启动 75℃
    S["A_cold_75"] = run(tset_ram=750, plant=Plant(), duration_s=900,
                         trace="A_cold_75")
    # B. 热机启动 (风道已 70℃) 设定改 75
    S["B_warm_75"] = run(tset_ram=750, plant=Plant(), duration_s=600,
                         start_air=70.0, trace="B_warm_75")
    # C. hA 扫描
    S["C_sweep_hA"] = sweep_hA()
    # D. C_a 扫描
    S["D_sweep_Ca"] = sweep_Ca()
    # E. RAM 里 Temset=7500 (750℃) -- 上位机按 0.1℃ 下发
    S["E_tset7500"] = run(tset_ram=7500, plant=Plant(), duration_s=1800,
                          trace="E_tset7500")
    # F. Derat 耦合
    S["F_derat"] = run(tset_ram=750, plant=Plant(), by_set_derat=1,
                       duration_s=600, start_air=70.0, trace="F_derat")
    # G. 设定 85℃ 对照
    S["G_85"] = run(tset_ram=850, plant=Plant(), duration_s=900,
                    start_air=70.0, trace="G_85")

    thr = None
    for r in sorted(S["C_sweep_hA"], key=lambda x: x["hA"]):
        if r["peak_true"] >= 90.0:
            thr = r["hA"]
    S["hA_threshold_for_90"] = thr

    with open(os.path.join(OUT, "summary.json"), "w", encoding="utf-8") as f:
        json.dump(S, f, ensure_ascii=False, indent=2)

    def show(k, r):
        print("  %-12s peak_sensor=%6.2f peak_true=%6.2f cut@%s E=%6.0fJ ev=%s err=0x%02x"
              % (k, r["peak_sensor"], r["peak_true"], r["cut_meas"],
                 r["cut_energy_J"], ",".join(r["events"]) or "-", r["err"]))

    print("== A/B/E/F/G ==")
    for k in ("A_cold_75", "B_warm_75", "E_tset7500", "F_derat", "G_85"):
        show(k, S[k])
    print("== C: hA 扫描 (Pmax=300 C_h=60 C_a=600) ==")
    for r in S["C_sweep_hA"]:
        print("  hA=%5.1f  theory_dT=%5.2f  peak_true=%6.2f  cut@%s"
              % (r["hA"], r["theory_dT"], r["peak_true"], r["cut_meas"]))
    print("== D: C_a 扫描 ==")
    for r in S["D_sweep_Ca"]:
        print("  C_a=%6.0f hA=%4.1f theory_dT=%5.2f peak_true=%6.2f"
              % (r["C_a"], r["hA"], r["theory_dT"], r["peak_true"]))
    print("== hA threshold for peak>=90: %s ==" % S["hA_threshold_for_90"])


if __name__ == "__main__":
    main()
