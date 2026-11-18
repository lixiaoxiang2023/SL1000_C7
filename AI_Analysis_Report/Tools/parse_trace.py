# -*- coding: utf-8 -*-
"""解析 temp_trace.py 产生的 J-Link 日志，输出温度/占空比/保护位时间序列"""
import re
import sys

WATCH_N = 7
FIELDS = ["temp", "fan", "pwm1", "sp", "tcb", "tick", "heat"]


def parse(path, interval_ms=600):
    txt = open(path, "rb").read().decode("utf-8", errors="replace")
    blocks = re.findall(
        r"J-Link>Mem32\s+(0x[0-9A-Fa-f]+),\s*(\d+)\s*\r?\n([\s\S]*?)(?=J-Link>|$)", txt)
    vals = []
    for _, _, body in blocks:
        v = []
        for line in body.splitlines():
            m = re.match(r"^\s*[0-9A-Fa-f]{8}\s*=\s*(.+)$", line)
            if m:
                for t in m.group(1).split():
                    if re.fullmatch(r"[0-9A-Fa-f]{8}", t):
                        v.append(int(t, 16))
        vals.append(v)
    n = len(vals) // WATCH_N
    rows = []
    for k in range(n):
        g = vals[k * WATCH_N:(k + 1) * WATCH_N]
        if any(len(x) == 0 for x in g):
            continue
        tm = g[0][0]
        fan = g[1]
        p1 = g[2]
        sp = g[3][0]
        tcb = g[4][0]
        tick = g[5][0]
        hb = g[6][0]
        rows.append(dict(
            k=k, t=round(k * interval_ms / 1000.0, 2),
            Tmain=tm & 0xFFFF, Tfan=(tm >> 16) & 0xFFFF,
            wPwm=fan[0], wPwmOld=(fan[1] >> 16) & 0xFFFF, derat=(fan[1] >> 24) & 0xFF,
            dwStep=p1[0], cnt=p1[1] & 0xFFFF,
            run=(p1[1] >> 16) & 0xFF, spd=(p1[1] >> 24) & 0xFF,
            SPmain=sp & 0xFFFF, SPfan=(sp >> 16) & 0xFFFF,
            TCB=tcb, Tick=tick,
            bHeat=(hb >> 8) & 0xFF, bHeatFan=(hb >> 16) & 0xFF,
        ))
    return rows


def summary(rows):
    if not rows:
        print("no rows")
        return
    tf = [r["Tfan"] for r in rows]
    print("samples %d  span %.0fs" % (len(rows), rows[-1]["t"] - rows[0]["t"]))
    print("Tfan   min=%.1f max=%.1f first=%.1f last=%.1f" % (
        min(tf) / 10, max(tf) / 10, tf[0] / 10, tf[-1] / 10))
    tm = [r["Tmain"] for r in rows]
    print("Tmain  min=%.1f max=%.1f" % (min(tm) / 10, max(tm) / 10))
    print("SPfan  ", sorted({r["SPfan"] for r in rows}))
    print("SPmain ", sorted({r["SPmain"] for r in rows}))
    print("bHeat  ", sorted({r["bHeat"] for r in rows}),
          " bHeatFan ", sorted({r["bHeatFan"] for r in rows}))
    print("derat  ", sorted({r["derat"] for r in rows}))
    print("wPwm   ", sorted({r["wPwm"] for r in rows}))
    print("PWM1 run", sorted({r["run"] for r in rows}),
          " spd", sorted({r["spd"] for r in rows}))
    print("dwStep ", sorted({r["dwStep"] for r in rows}))
    d = [abs(tf[i] - tf[i - 1]) for i in range(1, len(tf))]
    if d:
        print("单步最大变化 %.1fC  平均 %.3fC" % (max(d) / 10, sum(d) / len(d) / 10))


if __name__ == "__main__":
    p = sys.argv[1]
    iv = int(sys.argv[2]) if len(sys.argv) > 2 else 600
    rs = parse(p, iv)
    summary(rs)
    print("---- 每60s ----")
    for r in rs:
        if abs(r["t"] % 60.0) < 0.5:
            print("t=%5.0fs Tfan=%5.1f Tmain=%5.1f wPwm=%3d old=%3d derat=%d hFan=%d SPf=%4d SPm=%4d cnt=%4d" % (
                r["t"], r["Tfan"] / 10, r["Tmain"] / 10, r["wPwm"], r["wPwmOld"],
                r["derat"], r["bHeatFan"], r["SPfan"], r["SPmain"], r["cnt"]))
