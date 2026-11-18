#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""plot_trace.py -- 把仿真曲线画成 SVG (无第三方依赖)"""
import csv
import os

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sim_out")


def load(name, step=1):
    rows = list(csv.DictReader(open(os.path.join(OUT, name + ".csv"), encoding="utf-8")))
    return [(float(r["t_s"]), float(r["meas_0.1C"]) / 10.0, float(r["duty_pct"]))
            for r in rows[::step]]


def main():
    a = load("A_cold_75", 3)
    e = load("E_tset7500", 3)

    W, H = 680, 400
    ml, mt, mr, mb = 62, 48, 130, 52
    pw, ph = W - ml - mr, H - mt - mb
    TMAX, YMIN, YMAX = 960.0, 20.0, 100.0

    def X(t):
        return ml + t / TMAX * pw

    def Y(v):
        return mt + ph - (v - YMIN) / (YMAX - YMIN) * ph

    L = ['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 %d %d" font-family="system-ui,Segoe UI,Arial" font-size="11">' % (W, H),
         '<rect width="%d" height="%d" fill="#ffffff"/>' % (W, H)]

    # 网格
    v = 20
    while v <= 100:
        L.append('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="#e6ecf2"/>' % (ml, Y(v), ml + pw, Y(v)))
        L.append('<text x="%d" y="%.1f" text-anchor="end" fill="#5a6b7b">%d</text>' % (ml - 8, Y(v) + 4, v))
        v += 10
    for t in range(0, 961, 120):
        L.append('<line x1="%.1f" y1="%d" x2="%.1f" y2="%d" stroke="#e6ecf2"/>' % (X(t), mt, X(t), mt + ph))
        L.append('<text x="%.1f" y="%d" text-anchor="middle" fill="#5a6b7b">%d</text>' % (X(t), mt + ph + 16, t))

    # 参考线
    for val, lab, col in ((75, "设定 75℃", "#1b6ca8"), (80, "软限幅 设定+5℃ = 80", "#c98a00"),
                          (90, "上位机判过高 ≈90", "#b3261e"), (92, "硬保护 92 (死代码)", "#7a1a12")):
        L.append('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="%s" stroke-dasharray="5 4" stroke-width="1"/>'
                 % (ml, Y(val), ml + pw, Y(val), col))
        L.append('<text x="%d" y="%.1f" fill="%s">%s</text>' % (ml + 4, Y(val) - 5, col, lab))

    # 曲线
    for data, col, wd in ((a, "#1b6ca8", 2.0), (e, "#b3261e", 2.0)):
        pts = " ".join("%.1f,%.1f" % (X(t), Y(val)) for t, val, _ in data if t <= TMAX)
        L.append('<polyline points="%s" fill="none" stroke="%s" stroke-width="%.1f"/>' % (pts, col, wd))

    # 标注
    L.append('<circle cx="%.1f" cy="%.1f" r="3.5" fill="#b3261e"/>' % (X(253.5), Y(92)))
    L.append('<text x="%.1f" y="%.1f" fill="#b3261e" font-weight="600">t=254s 92.2℃ 切断</text>'
             % (X(253.5) + 8, Y(92) + 14))
    L.append('<circle cx="%.1f" cy="%.1f" r="3.5" fill="#1b6ca8"/>' % (X(164.6), Y(75)))
    L.append('<text x="%.1f" y="%.1f" fill="#1b6ca8" font-weight="600">t=165s 到 75.0 后稳在 74.0</text>'
             % (X(164.6) + 8, Y(75) - 10))

    L.append('<text x="%d" y="24" font-size="13" font-weight="600" fill="#111">同一套固件、同一台机器, 只改 RAM 里的 Temset: 75℃ vs 750℃(即 7500)</text>' % ml)
    L.append('<text x="%.1f" y="%d" text-anchor="middle" fill="#111" font-size="12">时间 (s)</text>' % (ml + pw / 2, H - 16))
    L.append('<text x="16" y="%.1f" transform="rotate(-90 16 %.1f)" text-anchor="middle" fill="#111" font-size="12">byTemp[1] 读到的温度 (℃)</text>' % (mt + ph / 2, mt + ph / 2))
    L.append('<text x="%d" y="%d" fill="#1b6ca8" font-size="12">■ Temset=750 (设定正确)</text>' % (W - mr + 8, mt + 8))
    L.append('<text x="%d" y="%d" fill="#b3261e" font-size="12">■ Temset=7500 (参数被×10)</text>' % (W - mr + 8, mt + 28))
    L.append('</svg>')

    with open(os.path.join(OUT, "trace.svg"), "w", encoding="utf-8") as f:
        f.write("\n".join(L))
    print("wrote sim_out/trace.svg")


if __name__ == "__main__":
    main()
