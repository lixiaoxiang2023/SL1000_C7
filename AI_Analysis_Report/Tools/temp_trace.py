# -*- coding: utf-8 -*-
"""
temp_trace.py —— 生成高频只读 J-Link 采样脚本并解析
只发送 connect / Mem32 / Sleep / q，不写内存、不复位、不 halt。
"""
import os, re, sys, csv, datetime, subprocess

JLINK = r"C:\Program Files\SEGGER\JLink_V990\JLink.exe"
DEVICE = "STM32F103xC"
OUT = r"D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Build_Logs"

# 精简监控项(3条命令/周期) -> 高采样率
WATCH = [
    ("temp",   0x20005788, 1),   # byTemp[0]主 低16位 / byTemp[1]风 高16位 (0.1℃)
    ("fan",    0x20006A10, 2),   # fan wPwm / wPwm_Old|derat|chk
    ("pwm1",   0x200067EC, 3),   # chn1 dwStep / dwStpCnt / run|spd|cnt   ← 必须3字
    ("sp",     0x200068C4, 1),   # wHeatTemp 低16 / wHeatFanTemp 高16
    ("tcb",    0x20006A54, 1),   # pxCurrentTCB  (任务存活判据)
    ("tick",   0x20006A68, 1),   # xTickCount
    ("heat",   0x20006AB8, 1),   # byte1=bHeatSta  byte2=bHeatFanSta  ← 保护代码使能位
]


def build(sec, interval_ms, path):
    n = max(1, int(sec * 1000 / interval_ms))
    L = ["ExitOnError 0", "connect"]
    for s in range(n):
        for _, a, w in WATCH:
            L.append("Mem32 0x%08X, %d" % (a, w))
        if s < n - 1:
            L.append("Sleep %d" % interval_ms)
    L.append("q")
    with open(path, "w", encoding="ascii", newline="\r\n") as f:
        f.write("\n".join(L))
    return n


def parse(log, csv_path, t0):
    raw = open(log, "rb").read()
    for enc in ("utf-8", "gbk", "utf-16"):
        try:
            txt = raw.decode(enc); break
        except Exception:
            txt = None
    txt = raw.decode("utf-8", errors="replace")
    blocks = re.findall(r"J-Link>Mem32\s+(0x[0-9A-Fa-f]+),\s*(\d+)\s*\r?\n([\s\S]*?)(?=J-Link>|$)", txt)
    per = len(WATCH)
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
    complete = len(vals) // per
    rows = []
    for k in range(complete):
        g = vals[k * per:(k + 1) * per]
        if any(len(x) == 0 for x in g):
            continue
        tm = g[0][0]
        fan = g[1]
        p1 = g[2]
        sp = g[3][0]
        tcb = g[4][0] if len(g) > 4 else 0
        tick = g[5][0] if len(g) > 5 else 0
        heat = g[6][0] if len(g) > 6 else 0
        st = p1[2] if len(p1) > 2 else 0
        run = st & 0xFF
        spd = (st >> 8) & 0xFF
        rows.append({
            "seq": k,
            "t_s": round(k * (t0[1] / 1000.0), 2),
            "T_main": tm & 0xFFFF,
            "T_fan": (tm >> 16) & 0xFFFF,
            "Fan_wPwm": fan[0],
            "Fan_derat": (fan[1] >> 16) & 0xFF,
            "PWM1_dwStep": p1[0],
            "PWM1_cnt": p1[1] & 0xFFFF,
            "PWM1_run": run,
            "PWM1_spd": spd,
            "SP_main": sp & 0xFFFF,
            "SP_fan": (sp >> 16) & 0xFFFF,
            "TCB": tcb,
            "Tick": tick,
            "bHeatSta": (heat >> 8) & 0xFF,
            "bHeatFanSta": (heat >> 16) & 0xFF,
        })
    if rows:
        with open(csv_path, "w", newline="", encoding="utf-8-sig") as f:
            w = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
            w.writeheader(); w.writerows(rows)
    return rows


def main():
    sec = int(sys.argv[1]) if len(sys.argv) > 1 else 1200
    iv = int(sys.argv[2]) if len(sys.argv) > 2 else 200
    stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    jl = os.path.join(OUT, "trace_%s.jlink" % stamp)
    log = os.path.join(OUT, "trace_%s.log" % stamp)
    csvp = os.path.join(OUT, "trace_%s.csv" % stamp)
    n = build(sec, iv, jl)
    print("[gen] %d samples, interval %dms -> %s" % (n, iv, jl))
    t0 = datetime.datetime.now()
    with open(log, "wb") as fo:
        p = subprocess.run([JLINK, "-device", DEVICE, "-if", "SWD", "-speed", "auto",
                            "-CommanderScript", jl], stdout=fo, stderr=subprocess.STDOUT)
    dur = (datetime.datetime.now() - t0).total_seconds()
    print("[run] native exit=%s elapsed=%.1fs" % (p.returncode, dur))
    rows = parse(log, csvp, (sec, iv * (dur / max(1, n))))
    print("[parse] rows=%d -> %s" % (len(rows), csvp))
    if rows:
        tf = [r["T_fan"] for r in rows]
        print("[stat] T_fan min=%.1f max=%.1f first=%.1f last=%.1f" %
              (min(tf) / 10.0, max(tf) / 10.0, tf[0] / 10.0, tf[-1] / 10.0))
        print("[stat] SP_fan=%d  Fan_derat set=%s" %
              (rows[0]["SP_fan"], sorted({r["Fan_derat"] for r in rows})))
    print("CSV=" + csvp)


if __name__ == "__main__":
    main()
