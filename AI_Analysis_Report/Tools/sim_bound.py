#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
sim_bound.py -- 上界证明 + 可达性穷举

结论要用"能/不能"表述, 不能用"可能":
  P1  只要 (Temset=750, bySetDerat=0, bHeatFanSta=1), 送进上位机的 byTemp[1]
      物理上存在上界。用 800 组随机硬件参数验证该上界。
  P2  要突破该上界, RAM 中必须满足什么条件? 反解出来。
  P3  92℃ 保护分支的"可达条件"是什么? 是否真的可达?
"""
import json
import math
import os
import random
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sim_temp_control import Plant, run, t_to_meas   # noqa: E402

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sim_out")
os.makedirs(OUT, exist_ok=True)


def bound_sweep(n=260, seed=20261114):
    """随机撒参数, 看 SP=750 时测量峰值的最大值"""
    rnd = random.Random(seed)
    best = []
    for i in range(n):
        Pmax = rnd.uniform(120, 450)
        C_h = math.exp(rnd.uniform(math.log(8), math.log(500)))
        C_a = math.exp(rnd.uniform(math.log(200), math.log(2500)))
        hA = math.exp(rnd.uniform(math.log(0.4), math.log(25)))
        k_l = rnd.uniform(1.0, 6.0)
        tau_s = rnd.uniform(0.5, 25.0)
        tau_d = rnd.uniform(0.0, 8.0)
        pl = Plant(Pmax=Pmax, C_h=C_h, C_a=C_a, hA=hA, k_l=k_l,
                   tau_s=tau_s, tau_d=tau_d)
        r = run(tset_ram=750, plant=pl, duration_s=420, dt=0.01)
        best.append(dict(peak=r["peak_sensor"], Pmax=Pmax, C_h=C_h, C_a=C_a,
                         hA=hA, k_l=k_l, tau_s=tau_s, tau_d=tau_d,
                         cut=r["cut_meas"], err=r["err"]))
    best.sort(key=lambda x: -x["peak"])
    return best


def reachability():
    """反解: Temset / wHeatFanTemp 各取什么值, 峰值能到多少"""
    rows = []
    for tset in (750, 800, 850, 870, 880, 900, 1000, 2000, 7500):
        pl = Plant()
        r = run(tset_ram=tset, plant=pl, duration_s=1500, dt=0.01)
        rows.append(dict(tset=tset, peak=r["peak_sensor"], err=r["err"],
                         events=r["events"]))
    return rows


def clamp_sweep():
    """Temset 固定 750, 只抬高 wHeatFanTemp(全局参数) -> 软限幅失效但闭环仍指向 750"""
    rows = []
    for w in (750, 800, 850, 900, 950):
        pl = Plant()
        r = run(tset_ram=750, w_heat_fan_temp=w, plant=pl,
                duration_s=900, dt=0.01)
        rows.append(dict(wHeatFanTemp=w, clamp=w + 50,
                         peak=r["peak_sensor"], cut=r["cut_meas"]))
    return rows


if __name__ == "__main__":
    bs = bound_sweep()
    print("== P1: 260 组随机硬件参数, Temset=750 时的测量峰值 ==")
    print("   max peak = %.2f C   (case: %s)" % (bs[0]["peak"],
          {k: round(v, 1) for k, v in bs[0].items() if k != "peak"}))
    print("   min peak = %.2f C" % bs[-1]["peak"])
    over81 = [b for b in bs if b["peak"] > 81.0]
    over90 = [b for b in bs if b["peak"] > 90.0]
    print("   >81C : %d / %d" % (len(over81), len(bs)))
    print("   >90C : %d / %d" % (len(over90), len(bs)))
    print("   top5:")
    for b in bs[:5]:
        print("     peak=%6.2f  Pmax=%5.0f C_h=%5.1f C_a=%6.0f hA=%5.2f "
              "tau_s=%4.1f tau_d=%3.1f cut=%s" %
              (b["peak"], b["Pmax"], b["C_h"], b["C_a"], b["hA"],
               b["tau_s"], b["tau_d"], b["cut"]))

    print("== P2: Temset 抬高 -> 峰值/事件 ==")
    for r in reachability():
        print("   Temset=%6d (%.0fC)  peak=%6.2f  ev=%s" %
              (r["tset"], r["tset"] / 10.0, r["peak"], r["events"]))

    print("== P3: Temset=750 但 wHeatFanTemp 抬高 ==")
    for r in clamp_sweep():
        print("   wHeatFanTemp=%4d clamp=%4d  peak=%6.2f" %
              (r["wHeatFanTemp"], r["clamp"], r["peak"]))

    with open(os.path.join(OUT, "bound.json"), "w", encoding="utf-8") as f:
        json.dump(dict(bound=bs, reach=reachability(), clamp=clamp_sweep()),
                  f, ensure_ascii=False, indent=2)
