# -*- coding: utf-8 -*-
"""
读取 temp_trace2.py 的 trace.csv，判定「常温 -> 目标温度」这段升温是否过冲。

输出：
  - 控制台摘要
  - <name>.md   结论 + 分段表
  - <name>.svg  温度/占空比/设定值曲线

判定依据（全部来自实测采样，不做假设）：
  1. 升温段 = bHeatFan==1 且 derat==0 且 SP_fan>=600 的连续区间
  2. 过冲量 = 段内 T_fan 最大值 - SP_fan
  3. 保护动作签名 = 段内曾出现 T_fan > SP_fan+40 且 wPwm==0（G1 限幅已动作）
  4. 满功率占比 = wPwm>=95 的样本占比（判断是否长时间饱和）
  5. 若 T_fan 曾 >=900（90.0℃）单独标记
"""
import csv
import datetime
import os
import sys


def load(path):
    rows = []
    with open(path, encoding="utf-8-sig") as f:
        for r in csv.DictReader(f):
            if r["ok"] != "1":
                continue
            try:
                tk = int(r["tick"])          # CSV 里 tick 是十进制毫秒
                rows.append(dict(
                    t=tk / 1000.0,           # 以 xTickCount 为唯一时基，避免 J-Link 卡顿导致时间漂移
                    wall=r["wall"],
                    Tmain=int(r["T_main"]),
                    Tfan=int(r["T_fan"]),
                    SPmain=int(r["SP_main"]),
                    SPfan=int(r["SP_fan"]),
                    bHeat=int(r["bHeat"]),
                    bHeatFan=int(r["bHeatFan"]),
                    derat=int(r["derat"]),
                    wPwm=int(r["wPwm"]),
                    wPwmOld=int(r["wPwmOld"]),
                    blRun=int(r["blRun"]),
                    tick=tk,
                    pid_set=float(r["pid_set"] or 0),
                    pid_meas=float(r["pid_meas"] or 0),
                    pid_out=float(r["pid_out"] or 0),
                ))
            except (ValueError, KeyError):
                continue
    rows.sort(key=lambda x: x["t"])
    # 去重（分块边界可能重复）
    out = []
    for r in rows:
        if out and abs(r["t"] - out[-1]["t"]) < 0.05:
            continue
        out.append(r)
    return out


def seg_heating(rows):
    """返回升温段列表 [(i0,i1)]，条件：加热使能 + 非降额 + 设定>=60℃"""
    segs = []
    i = 0
    n = len(rows)
    while i < n:
        r = rows[i]
        if r["bHeatFan"] == 1 and r["derat"] == 0 and r["SPfan"] >= 600:
            j = i
            while j + 1 < n and rows[j + 1]["bHeatFan"] == 1 and rows[j + 1]["derat"] == 0:
                j += 1
            if j - i >= 10:
                segs.append((i, j))
            i = j + 1
        else:
            i += 1
    return segs


def analyze(rows):
    res = {"rows": len(rows)}
    if not rows:
        return res
    res["span_s"] = rows[-1]["t"] - rows[0]["t"]
    res["Tfan_min"] = min(r["Tfan"] for r in rows) / 10.0
    res["Tfan_max"] = max(r["Tfan"] for r in rows) / 10.0
    res["over90"] = [r for r in rows if r["Tfan"] >= 900]
    res["segs"] = []
    for (i0, i1) in seg_heating(rows):
        seg = rows[i0:i1 + 1]
        sp = seg[-1]["SPfan"]
        t0 = seg[0]["t"]
        tmax = max(s["Tfan"] for s in seg) / 10.0
        sat = sum(1 for s in seg if s["wPwm"] >= 95)
        cut = [s for s in seg if s["wPwm"] == 0 and s["Tfan"] > sp + 40]
        # 首次到达 90% 目标的时刻
        lo = seg[0]["Tfan"]
        target90 = lo + int(0.9 * (sp - lo))
        t_reach = None
        for s in seg:
            if s["Tfan"] >= target90:
                t_reach = s["t"] - t0
                break
        res["segs"].append(dict(
            i0=i0, i1=i1, t0=t0, dur=seg[-1]["t"] - t0,
            SP=sp / 10.0, Tstart=lo / 10.0, Tmax=tmax,
            overshoot=tmax - sp / 10.0,
            sat_pct=100.0 * sat / len(seg),
            cut_n=len(cut),
            t_reach=t_reach,
            wall0=seg[0]["wall"], wall1=seg[-1]["wall"],
            n=len(seg),
        ))
    return res


def report(res, rows, md_path):
    L = []
    A = L.append
    A("# 常温 → 目标温度 升温段实测判定\n")
    A("数据源：`trace.csv`，有效样本 %d，时间跨度 %.0f 秒（%.1f 分钟）。\n"
      % (res.get("rows", 0), res.get("span_s", 0), res.get("span_s", 0) / 60.0))
    A("")
    if rows:
        A("| 项 | 值 |")
        A("|---|---|")
        A("| 起始时间 | %s |" % rows[0]["wall"])
        A("| 结束时间 | %s |" % rows[-1]["wall"])
        A("| 烘干温度范围 | %.1f ~ %.1f ℃ |" % (res["Tfan_min"], res["Tfan_max"]))
        A("| 固化温度范围 | %.1f ~ %.1f ℃ |" % (
            min(r["Tmain"] for r in rows) / 10.0, max(r["Tmain"] for r in rows) / 10.0))
        A("| 曾 ≥90.0℃ 的样本 | %d 个 |" % len(res["over90"]))
        A("")
    if not res["segs"]:
        A("## 结论：本次采样内**没有捕获到升温段**\n")
        A("判据：需要同时满足 `bHeatFan==1`、`derat==0`、`SP_fan>=600` 且持续 ≥10 个样本。")
        A("")
        A("说明采样期间机器一直处于「未加热」或「Derat 降额」状态，因此无法用这段数据判定过冲。")
        A("")
    for k, s in enumerate(res["segs"], 1):
        A("## 升温段 %d：%.1f℃ → 目标 %.1f℃\n" % (k, s["Tstart"], s["SP"]))
        A("| 指标 | 实测 |")
        A("|---|---|")
        A("| 区间 | %s ~ %s（%.1f 分钟） |" % (s["wall0"], s["wall1"], s["dur"] / 60.0))
        A("| 起始温度 | %.1f ℃ |" % s["Tstart"])
        A("| 峰值温度 | **%.1f ℃** |" % s["Tmax"])
        A("| 设定值 | %.1f ℃ |" % s["SP"])
        A("| **过冲量** | **%+.1f ℃** |" % s["overshoot"])
        A("| 满功率(wPwm≥95)占比 | %.1f %% |" % s["sat_pct"])
        A("| 限幅动作样本数 | %d |" % s["cut_n"])
        A("| 到达 90%% 目标耗时 | %s |" % (
            "%.0f s" % s["t_reach"] if s["t_reach"] is not None else "未到达"))
        A("")
        if s["overshoot"] >= 5:
            A("> **判定：过冲显著**。峰值超出设定 %.1f℃，超过 92℃ 硬保护门限的余量为 %.1f℃。"
              % (s["overshoot"], 92.0 - s["Tmax"]))
        elif s["overshoot"] >= 1:
            A("> **判定：存在小幅过冲**（%.1f℃），未触及 90℃ 保护带。" % s["overshoot"])
        else:
            A("> **判定：无过冲**。峰值 %.1f℃ 未超过设定值 %.1f℃。"
              % (s["Tmax"], s["SP"]))
        A("")
    with open(md_path, "w", encoding="utf-8") as f:
        f.write("\n".join(L))
    return "\n".join(L)


def svg(rows, path, title="Temp"):
    if len(rows) < 2:
        return
    W, H = 1120, 520
    ml, mr, mt, mb = 62, 150, 46, 52
    pw, ph = W - ml - mr, H - mt - mb
    t0 = rows[0]["t"]
    t1 = rows[-1]["t"]
    span = max(1e-6, t1 - t0)
    tmin = min(r["Tfan"] for r in rows) / 10.0
    tmax = max(r["Tfan"] for r in rows) / 10.0
    smin = min(r["SPfan"] for r in rows) / 10.0
    ymin = min(tmin, smin) - 4
    ymax = max(tmax, smin) + 4

    def X(t):
        return ml + (t - t0) / span * pw

    def Y(v):
        return mt + (ymax - v) / max(1e-6, ymax - ymin) * ph

    L = ['<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d">' % (W, H, W, H),
         '<rect width="%d" height="%d" fill="#ffffff"/>' % (W, H),
         '<text x="%d" y="26" font-size="15" font-weight="600" fill="#111">%s</text>' % (ml, title)]
    # 网格
    step = 10 if (ymax - ymin) > 25 else 5
    v = int(ymin // step * step)
    while v <= ymax:
        y = Y(v)
        L.append('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="#e6e6e6"/>' % (ml, y, ml + pw, y))
        L.append('<text x="%d" y="%.1f" font-size="11" fill="#666" text-anchor="end">%d</text>' % (ml - 8, y + 4, v))
        v += step
    # 90℃ 线
    if ymin < 90 < ymax:
        y = Y(90)
        L.append('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="#d33" stroke-dasharray="6,4"/>' % (ml, y, ml + pw, y))
        L.append('<text x="%d" y="%.1f" font-size="11" fill="#d33">90℃</text>' % (ml + pw + 6, y + 4))

    # 设定值阶梯
    pts = []
    for r in rows:
        pts.append("%.1f,%.1f" % (X(r["t"]), Y(r["SPfan"] / 10.0)))
    L.append('<polyline fill="none" stroke="#2a7" stroke-width="1.6" points="%s"/>' % " ".join(pts))
    # 温度
    pts = []
    for r in rows:
        pts.append("%.1f,%.1f" % (X(r["t"]), Y(r["Tfan"] / 10.0)))
    L.append('<polyline fill="none" stroke="#1f6fd0" stroke-width="1.9" points="%s"/>' % " ".join(pts))

    # 占空比条
    for r in rows:
        h = r["wPwm"] / 100.0 * (ph * 0.22)
        if h <= 0:
            continue
        L.append('<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" fill="#f0a500" opacity="0.55"/>'
                 % (X(r["t"]), mt + ph - h, max(1.0, pw / max(1, len(rows))), h))

    L.append('<text x="%d" y="%d" font-size="11" fill="#1f6fd0">烘干温度</text>' % (ml + pw + 6, mt + 14))
    L.append('<text x="%d" y="%d" font-size="11" fill="#2a7">设定值</text>' % (ml + pw + 6, mt + 32))
    L.append('<text x="%d" y="%d" font-size="11" fill="#b07c00">占空比(0-100%%)</text>' % (ml + pw + 6, mt + 50))
    L.append('<text x="%d" y="%d" font-size="11" fill="#666" text-anchor="middle">时间 (s)</text>'
             % (ml + pw / 2, H - 16))
    for k in range(6):
        t = t0 + span * k / 5.0
        L.append('<text x="%.1f" y="%d" font-size="10" fill="#666" text-anchor="middle">%.0f</text>'
                 % (X(t), mt + ph + 16, t - t0))
    L.append("</svg>")
    open(path, "w", encoding="utf-8").write("\n".join(L))


if __name__ == "__main__":
    p = sys.argv[1]
    allrows = load(p)
    if not allrows:
        print("没有有效样本")
        sys.exit(1)
    # 只画与升温相关的区间：从首个 derat==0 样本前 60s 开始
    idx = next((i for i, r in enumerate(allrows)
                if r["derat"] == 0 and r["SPfan"] >= 600), 0)
    t_start = allrows[idx]["t"] - 60
    rows = [r for r in allrows if r["t"] >= t_start]
    res = analyze(allrows)
    base = os.path.splitext(p)[0]
    txt = report(res, allrows, base + "_判定.md")
    print(txt[:4000])
    svg(rows, base + "_曲线.svg", title="烘干升温曲线（时基=xTickCount）")
    print("\n已生成: %s_判定.md  %s_曲线.svg" % (base, base))
