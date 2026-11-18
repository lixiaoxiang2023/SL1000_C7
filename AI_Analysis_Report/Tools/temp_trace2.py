# -*- coding: utf-8 -*-
"""
SL-1000 C7 温控真实曲线采样器 v2

设计要点（针对 v1 的两个问题）：
  1. v1 用单个 JLink 进程跑几十分钟，中途 SWD 掉线后剩余全部丢数
     → v2 改为「分段会话」，每 CHUNK_SEC 秒重启一个 JLink 进程；
       某段成功率过低时自动重试并记录 GAP。
  2. v1 只在结束时一次性解析
     → v2 每段结束立即增量追加 CSV + fsync，任何时刻中断都不丢已采数据。

只读：脚本只含 connect / Mem32 / Sleep / q，不含 reset / erase / write / go。
"""
import csv
import datetime
import os
import re
import subprocess
import sys
import time

JLINK = r"C:\Program Files\SEGGER\JLink_V990\JLink.exe"
DEVICE = "STM32F103xC"

# (名字, 地址, 字数)
CMDS = [
    ("temp",    0x20005788, 1),   # byTemp[0]固化 低16 / byTemp[1]烘干 高16  (0.1℃)
    ("sp",      0x200068C4, 1),   # wHeatTemp 低16 / wHeatFanTemp 高16        (0.1℃)
    ("heat",    0x20006AB8, 1),   # byte1=bHeatSta  byte2=bHeatFanSta
    ("fan",     0x20006A10, 3),   # s_stTempDatFan: wPwm(int32) | wPwm_Old|derat|chk | TimeCount
    ("pwm",     0x200067E4, 5),   # DevCtr.staChn[1]: pwdPort | level|using | dwStep | dwStpCnt | run|spd|cnt
    ("pidfan",  0x20006928, 11),  # stPiPidFan: Temset Temin dev dev_next dev_last dev_sum Kp Ki Kd integral fout
    ("pidmain", 0x200068FC, 11),  # stPiPid
    ("tick",    0x20006A68, 1),   # xTickCount
    ("tcb",     0x20006A54, 1),   # pxCurrentTCB
]
PER = len(CMDS)

COLS = [
    "chunk", "seq", "wall", "epoch", "ok",
    "T_main", "T_fan", "SP_main", "SP_fan",
    "bHeat", "bHeatFan", "derat", "temchk", "timecnt",
    "wPwm", "wPwmOld",
    "dwStep", "dwStpCnt", "blRun", "bySpd",
    "pid_set", "pid_meas", "pid_dev", "pid_out", "pid_sum",
    "tick", "tcb",
]


def f32(w):
    import struct
    return struct.unpack("<f", struct.pack("<I", w & 0xFFFFFFFF))[0]


def build_script(n_samples, interval_ms, path):
    L = ["ExitOnError 0", "connect"]
    for s in range(n_samples):
        for _, a, w in CMDS:
            L.append("Mem32 0x%08X, %d" % (a, w))
        if s < n_samples - 1:
            L.append("Sleep %d" % interval_ms)
    L.append("q")
    with open(path, "w", encoding="ascii", newline="\r\n") as f:
        f.write("\n".join(L))
    return L


SEG_RE = re.compile(r"J-Link>Mem32\s+0x([0-9A-Fa-f]{8}),\s*(\d+)\s*\r?\n([\s\S]*?)(?=J-Link>|$)")
WORD_RE = re.compile(r"\b([0-9A-Fa-f]{8})\b")


def parse_segments(txt):
    """返回 [(addr, count, [words...]), ...]，按出现顺序"""
    out = []
    for m in SEG_RE.finditer(txt):
        addr = int(m.group(1), 16)
        cnt = int(m.group(2))
        body = m.group(3)
        if "Could not read memory" in body:
            out.append((addr, cnt, []))
            continue
        words = []
        for line in body.splitlines():
            mm = re.match(r"^\s*[0-9A-Fa-f]{8}\s*=\s*(.+)$", line)
            if mm:
                for t in WORD_RE.findall(mm.group(1)):
                    words.append(int(t, 16))
        out.append((addr, cnt, words[:cnt]))
    return out


def decode(segs, t0, chunk_id, base_seq):
    rows = []
    n = len(segs) // PER
    for s in range(n):
        g = segs[s * PER:(s + 1) * PER]
        # 地址必须与 CMDS 一致，否则说明错位
        if any(g[i][0] != CMDS[i][1] for i in range(PER)):
            continue
        ok = all(len(x[2]) == x[1] for x in g)
        row = {c: "" for c in COLS}
        row["chunk"] = chunk_id
        row["seq"] = base_seq + s
        row["ok"] = 1 if ok else 0
        if ok:
            tm = g[0][2][0]
            sp = g[1][2][0]
            hb = g[2][2][0]
            fan = g[3][2]
            pwm = g[4][2]
            pf = g[5][2]
            pm = g[6][2]
            row.update(
                T_main=tm & 0xFFFF, T_fan=(tm >> 16) & 0xFFFF,
                SP_main=sp & 0xFFFF, SP_fan=(sp >> 16) & 0xFFFF,
                bHeat=(hb >> 8) & 0xFF, bHeatFan=(hb >> 16) & 0xFF,
                wPwm=fan[0] & 0xFFFFFFFF,
                wPwmOld=fan[1] & 0xFFFF,
                derat=(fan[1] >> 16) & 0xFF,
                temchk=(fan[1] >> 24) & 0xFF,
                timecnt=fan[2] & 0xFFFF,
                dwStep=pwm[2] & 0xFFFFFFFF,
                dwStpCnt=pwm[3] & 0xFFFFFFFF,
                blRun=pwm[4] & 0xFF,
                bySpd=(pwm[4] >> 8) & 0xFF,
                pid_set=round(f32(pf[0]), 2),
                pid_meas=round(f32(pf[1]), 2),
                pid_dev=round(f32(pf[2]), 2),
                pid_out=round(f32(pf[10]), 2),
                pid_sum=round(f32(pf[5]), 1),
                tick=g[7][2][0],
                tcb=g[8][2][0],
            )
        rows.append(row)
    return rows, n


def run_chunk(chunk_id, n_samples, interval_ms, workdir):
    jl = os.path.join(workdir, "c%02d.jlink" % chunk_id)
    lg = os.path.join(workdir, "c%02d.log" % chunk_id)
    build_script(n_samples, interval_ms, jl)
    try:
        p = subprocess.run(
            [JLINK, "-device", DEVICE, "-if", "SWD", "-speed", "auto",
             "-CommanderScript", jl],
            capture_output=True, timeout=n_samples * interval_ms / 1000.0 + 120)
        txt = (p.stdout or b"").decode("utf-8", errors="replace")
    except subprocess.TimeoutExpired:
        txt = ""
    with open(lg, "w", encoding="utf-8") as f:
        f.write(txt)
    return parse_segments(txt)


def main():
    total_sec = int(sys.argv[1]) if len(sys.argv) > 1 else 3600
    interval_ms = int(sys.argv[2]) if len(sys.argv) > 2 else 400
    chunk_sec = int(sys.argv[3]) if len(sys.argv) > 3 else 120

    stamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    base = os.path.dirname(os.path.abspath(__file__))
    workdir = os.path.join(base, "..", "Build_Logs", "trace2_" + stamp)
    os.makedirs(workdir, exist_ok=True)
    csv_path = os.path.join(workdir, "trace.csv")

    n_chunk = max(1, total_sec // chunk_sec)
    samples_per_chunk = int(chunk_sec * 1000 / interval_ms)

    print("workdir :", workdir)
    print("plan    : %d chunks x %ds, %d samples/chunk @ %dms, total %ds"
          % (n_chunk, chunk_sec, samples_per_chunk, interval_ms, total_sec))

    with open(csv_path, "w", newline="", encoding="utf-8-sig") as f:
        csv.writer(f).writerow(COLS)
        f.flush()

    seq = 0
    t_start = time.time()
    for c in range(n_chunk):
        t0 = time.time()
        segs = run_chunk(c, samples_per_chunk, interval_ms, workdir)
        rows, n = decode(segs, t0, c, seq)
        seq += n
        good = sum(r["ok"] for r in rows)
        rate = (good / n * 100) if n else 0.0

        now = datetime.datetime.now()
        for i, r in enumerate(rows):
            el = t0 + i * interval_ms / 1000.0
            r["wall"] = datetime.datetime.fromtimestamp(el).strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            r["epoch"] = round(el, 3)

        with open(csv_path, "a", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=COLS)
            for r in rows:
                w.writerow(r)
            f.flush()
            os.fsync(f.fileno())

        msg = "[%s] chunk %2d/%d  samples=%d ok=%d (%.1f%%)  seq=%d" % (
            datetime.datetime.now().strftime("%H:%M:%S"), c + 1, n_chunk, n, good, rate, seq)
        if rate < 60:
            msg += "  <-- GAP/弱连接"
        print(msg, flush=True)

        # 目标总时长控制
        if time.time() - t_start + chunk_sec > total_sec:
            # 用短块补足剩余
            remain = total_sec - (time.time() - t_start)
            if remain < 30:
                break

    print("DONE csv=%s rows=%d" % (csv_path, seq), flush=True)


if __name__ == "__main__":
    main()
