# -*- coding: utf-8 -*-
"""
judge_trace.py —— 对真实温度曲线做判定
判定目标：区分「真过冲」与「假高温读数」

判据:
  A. 单步跳变: |ΔT| >= 30 (0.1℃) 且下一拍回落 >= 50% -> 采样/读数异常, 非真实热惯性
  B. 平滑爬升: 连续 N 拍单调上升且累计 >= 30 -> 真实升温
  C. 突破软限幅: T_fan > (SP_fan + 50)  -> 保护未动作
  D. 失控加热: T_fan > 760 且 Fan_wPwm == 100
"""
import csv, sys, os

def load(p):
    rows = []
    with open(p, encoding="utf-8-sig") as f:
        for r in csv.DictReader(f):
            try:
                rows.append({k: (float(v) if k in ("t_s",) else int(v)) for k, v in r.items()})
            except ValueError:
                continue
    return rows


def main(path):
    R = load(path)
    if not R:
        print("NO DATA"); return
    n = len(R)
    print("样本数 %d  时长 %.1f s  平均采样周期 %.0f ms" %
          (n, R[-1]["t_s"] - R[0]["t_s"], 1000.0 * (R[-1]["t_s"] - R[0]["t_s"]) / max(1, n - 1)))

    tf = [r["T_fan"] for r in R]
    tm = [r["T_main"] for r in R]
    sp = [r["SP_fan"] for r in R]
    spm = [r["SP_main"] for r in R]
    pw = [r["Fan_wPwm"] for r in R]
    duty = [r["PWM1_spd"] for r in R]

    print("\n=== 设定值 ===")
    print("SP_fan  集合: %s" % sorted(set(sp)))
    print("SP_main 集合: %s" % sorted(set(spm)))

    print("\n=== 温度统计 (0.1℃) ===")
    print("T_fan : min=%d max=%d 首=%d 末=%d 极差=%d" % (min(tf), max(tf), tf[0], tf[-1], max(tf) - min(tf)))
    print("T_main: min=%d max=%d 首=%d 末=%d" % (min(tm), max(tm), tm[0], tm[-1]))

    # 稳态段(后半程)纹波
    half = tf[n // 2:]
    print("后半程 T_fan: min=%d max=%d 峰峰=%d" % (min(half), max(half), max(half) - min(half)))

    print("\n=== 占空比 ===")
    print("Fan_wPwm 集合: %s" % sorted(set(pw)))
    print("PWM1_spd 集合: %s" % sorted(set(duty)))
    print("run 状态: %s" % sorted({r["PWM1_run"] for r in R}))
    print("derat  : %s" % sorted({r["Fan_derat"] for r in R}))

    # A. 单步跳变
    print("\n=== A. 单步跳变检测 (|ΔT|>=30 且 1~2 拍内回落) ===")
    jumps = []
    for i in range(1, n):
        d = tf[i] - tf[i - 1]
        if abs(d) >= 30:
            back = tf[i + 1] - tf[i] if i + 1 < n else 0
            rebounds = (d > 0 and back <= -15) or (d < 0 and back >= 15)
            jumps.append((R[i]["t_s"], tf[i - 1], tf[i], d, back, rebounds))
    if not jumps:
        print("无 |ΔT|>=3.0℃ 的单步跳变 -> 读数是连续的, 不存在假高温毛刺")
    for j in jumps[:20]:
        print("  t=%7.1fs %d -> %d (Δ%+d) 下一拍%+d  %s" %
              (j[0], j[1], j[2], j[3], j[4], "假读数" if j[5] else "持续变化"))

    # B. 平滑爬升
    print("\n=== B. 最长连续升温段 ===")
    best = (0, 0, 0)
    s = 0
    for i in range(1, n):
        if tf[i] >= tf[i - 1]:
            s += 1
            if s > best[0]:
                best = (s, i - s, i)
        else:
            s = 0
    if best[0]:
        i0, i1 = best[1], best[2]
        print("  连续 %d 拍上升, t=%0.1f..%0.1f s, %d -> %d (Δ%+d), 速率 %.3f ℃/s" %
              (best[0], R[i0]["t_s"], R[i1]["t_s"], tf[i0], tf[i1], tf[i1] - tf[i0],
               (tf[i1] - tf[i0]) / 10.0 / max(0.001, R[i1]["t_s"] - R[i0]["t_s"])))

    # C. 软限幅
    print("\n=== C. 软限幅动作 (T_fan > SP_fan+50) ===")
    over = [(R[i]["t_s"], tf[i], sp[i]) for i in range(n) if tf[i] > sp[i] + 50]
    print("  越限样本: %d 个" % len(over))
    for o in over[:10]:
        print("    t=%0.1fs T_fan=%d SP=%d" % o)

    # D. 失控加热
    print("\n=== D. 失控加热 (T_fan>760 且 duty=100) ===")
    bad = [(R[i]["t_s"], tf[i], duty[i]) for i in range(n) if tf[i] > 760 and duty[i] >= 99]
    print("  样本: %d 个" % len(bad))
    for b in bad[:10]:
        print("    t=%0.1fs T_fan=%d duty=%d" % b)

    # 温度-占空比相关性(1拍滞后)
    print("\n=== E. 控制方向校验 ===")
    if n > 20:
        rise = sum(1 for i in range(1, n) if tf[i] < sp[i] and duty[i] > 50)
        fall = sum(1 for i in range(1, n) if tf[i] >= sp[i] and duty[i] == 0)
        print("  低于设定且占空比>50%% 的样本: %d (应占多数)" % rise)
        print("  达到/超过设定且占空比=0 的样本: %d" % fall)

    print("\n=== 结论 ===")
    mx = max(tf)
    if mx > 800:
        print("  !! 实测最高 %0.1f℃ 超过软限幅 80.0℃ -> 保护未动作, 需查任务存活" % (mx / 10.0))
    else:
        print("  实测最高 %0.1f℃, 未触及软限幅 80.0℃" % (mx / 10.0))
    if jumps and any(j[5] for j in jumps):
        print("  存在假读数毛刺 -> 指向 ADC 并发读" )
    else:
        print("  无假读数毛刺")


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "sim_out/latest.csv")
