# -*- coding: utf-8 -*-
"""快速查看采样目录的最新状态（不占用 J-Link）"""
import csv
import glob
import os
import sys


def latest_dir():
    dirs = sorted(glob.glob(os.path.join(os.path.dirname(__file__), "..", "Build_Logs", "trace2_*")))
    return dirs[-1] if dirs else None


def main():
    d = sys.argv[1] if len(sys.argv) > 1 else latest_dir()
    if not d:
        print("no trace2 dir")
        return
    p = os.path.join(d, "trace.csv")
    rows = [r for r in csv.DictReader(open(p, encoding="utf-8-sig")) if r["ok"] == "1"]
    print("dir  :", os.path.basename(d))
    print("valid:", len(rows))
    if not rows:
        return
    first, last = rows[0], rows[-1]
    print("span : %s -> %s" % (first["wall"], last["wall"]))
    tf = [int(r["T_fan"]) for r in rows]
    tm = [int(r["T_main"]) for r in rows]
    print("Tfan : %.1f -> %.1f  (min %.1f max %.1f)" % (
        tf[0] / 10, tf[-1] / 10, min(tf) / 10, max(tf) / 10))
    print("Tmain: %.1f -> %.1f" % (tm[0] / 10, tm[-1] / 10))
    print("SP   : %s  derat=%s  bHeatFan=%s  wPwm=%s  blRun=%s" % (
        sorted({r["SP_fan"] for r in rows}), sorted({r["derat"] for r in rows}),
        sorted({r["bHeatFan"] for r in rows}), sorted({r["wPwm"] for r in rows})[:8],
        sorted({r["blRun"] for r in rows})))
    # 近 60s 速率
    if len(rows) > 150:
        a, b = rows[-150], rows[-1]
        import datetime
        ta = datetime.datetime.strptime(a["wall"], "%Y-%m-%d %H:%M:%S.%f")
        tb = datetime.datetime.strptime(b["wall"], "%Y-%m-%d %H:%M:%S.%f")
        dt = (tb - ta).total_seconds()
        if dt > 0:
            print("rate : %+.2f C/min (近 %.0fs)" % (
                (int(b["T_fan"]) - int(a["T_fan"])) / 10.0 / dt * 60, dt))
    print("sup90: %d samples" % sum(1 for x in tf if x >= 900))


if __name__ == "__main__":
    main()
