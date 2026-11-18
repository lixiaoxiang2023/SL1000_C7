# -*- coding: utf-8 -*-
"""plot_real.py —— 把真实采样 CSV 画成单文件 SVG（无外部依赖）"""
import csv, sys, os


def load(p):
    R = []
    with open(p, encoding="utf-8-sig") as f:
        for r in csv.DictReader(f):
            R.append({k: (float(v) if k == "t_s" else int(v)) for k, v in r.items()})
    return R


def main(src, dst):
    R = load(src)
    W, H = 900, 430
    ml, mr, mt, mb = 60, 30, 46, 46
    pw, ph = W - ml - mr, H - mt - mb
    t0 = R[0]["t_s"]
    t1 = R[-1]["t_s"]
    lo, hi = 70.0, 82.0
    L = ['<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 %d %d" width="%d" height="%d" font-family="Segoe UI,Microsoft YaHei,sans-serif">' % (W, H, W, H)]
    L.append('<rect width="%d" height="%d" fill="#ffffff"/>' % (W, H))
    L.append('<text x="%d" y="24" font-size="15" font-weight="600" fill="#111">实测曲线：烘干设定 75℃，稳态段（SWD 只读采样，%d 样本 / %.0f s）</text>' % (ml, len(R), t1 - t0))

    def X(t):
        return ml + (t - t0) / max(0.001, t1 - t0) * pw

    def Y(v):
        return mt + ph - (v - lo) / (hi - lo) * ph

    # 网格
    v = lo
    while v <= hi + 1e-9:
        y = Y(v)
        L.append('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="#e8e8e8"/>' % (ml, y, ml + pw, y))
        L.append('<text x="%d" y="%.1f" text-anchor="end" font-size="11" fill="#444">%.0f</text>' % (ml - 8, y + 4, v))
        v += 2
    # 设定线与软限幅
    for val, col, txt, dash in ((75.0, "#c0392b", "设定 75.0℃", ""), (80.0, "#e67e22", "软限幅 80.0℃ (设定+5)", "6 4"),
                                (92.0, "#7f8c8d", "92℃ 分支（设定≤870 时不可达）", "2 6")):
        y = Y(val)
        d = ' stroke-dasharray="%s"' % dash if dash else ""
        L.append('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="%s" stroke-width="1.3"%s/>' % (ml, y, ml + pw, y, col, d))
        L.append('<text x="%d" y="%.1f" font-size="10.5" fill="%s">%s</text>' % (ml + pw - 4, y - 4, col, txt))
    # 温度折线
    pts = " ".join("%.1f,%.1f" % (X(r["t_s"]), Y(r["T_fan"] / 10.0)) for r in R)
    L.append('<polyline points="%s" fill="none" stroke="#1f6feb" stroke-width="1.6"/>' % pts)
    pts2 = " ".join("%.1f,%.1f" % (X(r["t_s"]), Y(r["T_main"] / 10.0)) for r in R)
    L.append('<polyline points="%s" fill="none" stroke="#16a34a" stroke-width="1.1" opacity="0.85"/>' % pts2)

    # 占空比副轴（底部条带）
    by = mt + ph + 18
    L.append('<text x="%d" y="%d" font-size="10.5" fill="#555">占空比 wPwm（%% ，副标尺 0~40）</text>' % (ml, by + 10))
    for r in R:
        h = min(24, r["Fan_wPwm"] / 40.0 * 24)
        L.append('<line x1="%.1f" y1="%d" x2="%.1f" y2="%.1f" stroke="#8b5cf6" stroke-width="1.2" opacity="0.55"/>'
                 % (X(r["t_s"]), by + 24, X(r["t_s"]), by + 24 - h))

    # 轴
    L.append('<line x1="%d" y1="%d" x2="%d" y2="%d" stroke="#999"/>' % (ml, mt + ph, ml + pw, mt + ph))
    L.append('<text x="%d" y="%d" text-anchor="middle" font-size="11" fill="#333">时间 (s)</text>' % (ml + pw // 2, H - 6))
    for k in range(6):
        t = t0 + (t1 - t0) * k / 5.0
        L.append('<text x="%.1f" y="%d" text-anchor="middle" font-size="10" fill="#666">%.0f</text>' % (X(t), mt + ph + 14, t))
    # 图例
    L.append('<rect x="%d" y="%d" width="12" height="3" fill="#1f6feb"/>' % (ml + 8, mt + 10))
    L.append('<text x="%d" y="%d" font-size="11" fill="#333">烘干 T_fan</text>' % (ml + 26, mt + 15))
    L.append('<rect x="%d" y="%d" width="12" height="3" fill="#16a34a"/>' % (ml + 118, mt + 10))
    L.append('<text x="%d" y="%d" font-size="11" fill="#333">固化 T_main</text>' % (ml + 136, mt + 15))
    L.append('</svg>')
    open(dst, "w", encoding="utf-8").write("\n".join(L))
    print("SVG ->", dst, len("\n".join(L)), "bytes")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
