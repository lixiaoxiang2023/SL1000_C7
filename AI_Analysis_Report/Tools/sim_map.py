#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
sim_map.py -- 过冲的二维判别图 (hA x tau_sensor) + 敏感度

要回答: 设定 75℃ 时, 到底什么硬件条件会让测量值冲过 90℃?
结论形式: 一条判别曲线, 而不是一句"可能会过冲"。
"""
import json
import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sim_temp_control import Plant, run   # noqa: E402

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sim_out")
os.makedirs(OUT, exist_ok=True)

HA = (0.8, 1.2, 1.8, 2.5, 3.5, 5.0, 7.0, 10.0, 14.0, 20.0)
TAUS = (2.0, 5.0, 8.0, 12.0, 18.0, 25.0, 35.0, 50.0)


def build_map():
    grid = []
    for hA in HA:
        row = []
        for ts in TAUS:
            # 把总滞后 ts 拆成 输运延迟40% + 传感器惯性60%
            pl = Plant(hA=hA, tau_s=ts * 0.6, tau_d=ts * 0.4)
            r = run(tset_ram=750, plant=pl, duration_s=900, dt=0.01)
            row.append(dict(peak=r["peak_sensor"], cut=r["cut_meas"],
                            t_peak=r["t_peak_s"], err=r["err"]))
        grid.append(row)
    return grid


def no_clamp_compare():
    """把 SP+5 限幅拿掉(等价于 wHeatFanTemp 被抬高到很大), 看限幅值多少度"""
    out = {}
    for hA, ts in ((3.0, 5.0), (1.8, 8.0), (10.0, 2.0)):
        pl = Plant(hA=hA, tau_s=ts, tau_d=1.0)
        a = run(tset_ram=750, w_heat_fan_temp=750, plant=pl, duration_s=900, dt=0.01)
        pl2 = Plant(hA=hA, tau_s=ts, tau_d=1.0)
        b = run(tset_ram=750, w_heat_fan_temp=3000, plant=pl2, duration_s=900, dt=0.01)
        out["hA%.1f_tau%.1f" % (hA, ts)] = dict(
            with_clamp=a["peak_sensor"], without_clamp=b["peak_sensor"],
            saved=round(b["peak_sensor"] - a["peak_sensor"], 2))
    return out


def full_power_curve():
    """100% 占空比阶跃: 真实机器做一次这个实验就能辨识 tau 与 T_inf"""
    pl = Plant()
    rows = []
    dt = 0.05
    for i in range(int(1200 / dt)):
        pl.advance(dt, 1.0)
        if i % int(10 / dt) == 0:
            rows.append((round(i * dt, 1), round(pl.Ta, 2), round(pl.Ts, 2)))
    return rows


def svg_map(grid):
    """手写 SVG 热力图, 无第三方依赖"""
    W, H = 680, 380
    ml, mt, mr, mb = 78, 46, 150, 58
    pw, ph = W - ml - mr, H - mt - mb
    cw, ch = pw / len(TAUS), ph / len(HA)

    def color(v):
        # 40 -> 120 映射 深蓝->红
        x = max(0.0, min(1.0, (v - 74.0) / 24.0))
        r = int(30 + 215 * x)
        g = int(110 - 80 * x)
        b = int(200 - 180 * x)
        return "rgb(%d,%d,%d)" % (r, g, b)

    L = []
    L.append('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 %d %d" font-family="system-ui,Segoe UI,Arial" font-size="11">' % (W, H))
    L.append('<rect width="%d" height="%d" fill="#ffffff"/>' % (W, H))
    for i, hA in enumerate(HA):
        for j, ts in enumerate(TAUS):
            v = grid[i][j]["peak"]
            x, y = ml + j * cw, mt + (len(HA) - 1 - i) * ch
            L.append('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" fill="%s" stroke="#fff" stroke-width="1"/>'
                     % (x, y, cw, ch, color(v)))
            L.append('<text x="%.1f" y="%.1f" text-anchor="middle" fill="%s">%.0f</text>'
                     % (x + cw / 2, y + ch / 2 + 4,
                        "#fff" if v > 88 else "#10243a", v))
    for j, ts in enumerate(TAUS):
        L.append('<text x="%.1f" y="%d" text-anchor="middle" fill="#334">%g</text>'
                 % (ml + j * cw + cw / 2, mt + ph + 16, ts))
    for i, hA in enumerate(HA):
        L.append('<text x="%d" y="%.1f" text-anchor="end" fill="#334">%g</text>'
                 % (ml - 8, mt + (len(HA) - 1 - i) * ch + ch / 2 + 4, hA))
    L.append('<text x="%.1f" y="%d" text-anchor="middle" fill="#111" font-size="12">传感链总滞后 tau_d + tau_s (s)</text>' % (ml + pw / 2, H - 18))
    L.append('<text x="18" y="%.1f" transform="rotate(-90 18 %.1f)" text-anchor="middle" fill="#111" font-size="12">加热体-风 换热系数 hA (W/K)</text>' % (mt + ph / 2, mt + ph / 2))
    L.append('<text x="%d" y="22" fill="#111" font-size="13" font-weight="600">设定 75℃ 时测量峰值 (℃) — 白色数字为仿真结果</text>' % ml)
    # 图例
    for k in range(60):
        v = 74 + k * 0.4
        L.append('<rect x="%d" y="%d" width="3" height="14" fill="%s"/>' % (W - 132 + k * 2, 70, color(v)))
    L.append('<text x="%d" y="64" fill="#111">74℃</text>' % (W - 132))
    L.append('<text x="%d" y="64" fill="#111">98℃</text>' % (W - 30))
    L.append('<text x="%d" y="112" fill="#b3261e" font-size="11">红区 &gt;90℃</text>' % (W - 132))
    L.append('<text x="%d" y="130" fill="#1b4f72" font-size="11">蓝区 &lt;=80℃</text>' % (W - 132))
    L.append('</svg>')
    return "\n".join(L)


if __name__ == "__main__":
    g = build_map()
    print("hA \\ tau_s   " + "  ".join("%5g" % t for t in TAUS))
    for i, hA in enumerate(HA):
        print("%8.1f     " % hA + "  ".join("%5.0f" % g[i][j]["peak"] for j in range(len(TAUS))))

    nc = no_clamp_compare()
    print("\n== SP+5 限幅的作用 ==")
    for k, v in nc.items():
        print("  %-16s 有限幅=%6.2f  无限幅=%6.2f  限幅贡献=%5.2f C"
              % (k, v["with_clamp"], v["without_clamp"], v["saved"]))

    fp = full_power_curve()
    print("\n== 100%% 占空比阶跃(用于现场辨识): t=60s %.1f, t=120s %.1f, t=300s %.1f, t=600s %.1f"
          % (fp[6][1], fp[12][1], fp[30][1], fp[60][1]))

    with open(os.path.join(OUT, "map.json"), "w", encoding="utf-8") as f:
        json.dump(dict(HA=HA, TAUS=TAUS, grid=g, clamp_cmp=nc, full_power=fp),
                  f, ensure_ascii=False, indent=2)
    with open(os.path.join(OUT, "map.svg"), "w", encoding="utf-8") as f:
        f.write(svg_map(g))
    print("\nwrote sim_out/map.svg")
