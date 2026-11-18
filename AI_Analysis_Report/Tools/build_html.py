#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""build_html.py -- 把结论 + 两张 SVG 合成一份单文件 HTML 看板"""
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
SO = os.path.join(HERE, "sim_out")


def svg(name):
    d = open(os.path.join(SO, name), encoding="utf-8").read()
    return d.replace('<?xml version="1.0" encoding="utf-8"?>', "")


summ = json.load(open(os.path.join(SO, "summary.json"), encoding="utf-8"))
a = summ["A_cold_75"]
e = summ["E_tset7500"]

REACH = [(750, 75.0, "无"), (800, 79.6, "无"), (850, 84.3, "无"), (880, 87.2, "无"),
         (900, 89.1, "无"), (910, 90.0, "无"), (920, 91.0, "无"),
         (930, 91.9, "HARD_92C"), (940, 92.2, "HARD_92C"), (7500, 92.3, "HARD_92C")]

rows = "\n".join(
    "<tr%s><td>%d</td><td>%s</td><td>%.1f</td><td>%s</td></tr>" % (
        ' class="hot"' if p >= 90 else "", t, ("%.1f" % (t / 10.0)) + " ℃", p, ev)
    for t, p, ev in REACH)

html = """<!DOCTYPE html>
<html lang="zh-CN"><head><meta charset="utf-8">
<title>烘干 75℃ 冲到 90 多度 —— 仿真复核</title>
<style>
:root{--ink:#12202e;--sub:#5b6b7b;--line:#e3e9ef;--bg:#f6f8fa;--card:#fff;--hot:#b3261e;--cool:#1b6ca8}
*{box-sizing:border-box}
body{margin:0;padding:28px 20px 60px;background:var(--bg);color:var(--ink);
font-family:"Microsoft YaHei","PingFang SC",system-ui,Segoe UI,Arial;font-size:14px;line-height:1.65}
.wrap{max-width:1060px;margin:0 auto}
h1{font-size:22px;margin:0 0 6px;letter-spacing:.2px}
.meta{color:var(--sub);font-size:12.5px;margin-bottom:22px}
.card{background:var(--card);border:1px solid var(--line);border-radius:10px;padding:18px 20px;margin:16px 0}
.verdict{border-left:4px solid var(--hot)}
.k{font-size:15px;font-weight:600;margin:0 0 10px}
.big{font-size:26px;font-weight:700;color:var(--hot)}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:12px;margin-top:12px}
.tile{background:#fbfcfd;border:1px solid var(--line);border-radius:8px;padding:12px 14px}
.tile .n{font-size:20px;font-weight:700;margin-top:2px}
.tile .l{font-size:12px;color:var(--sub)}
table{width:100%;border-collapse:collapse;font-size:13px;margin-top:8px}
th,td{text-align:left;padding:7px 10px;border-bottom:1px solid var(--line)}
th{background:#f2f5f8;font-weight:600;color:#33414f}
tr.hot td{background:#fdf1f0;color:var(--hot);font-weight:600}
.svgbox{background:#fff;border:1px solid var(--line);border-radius:10px;padding:8px;overflow:auto}
.svgbox svg{display:block;width:100%;height:auto}
.note{font-size:12.5px;color:var(--sub);margin-top:8px}
code{background:#eef2f6;padding:1px 5px;border-radius:4px;font-size:12.5px;
font-family:Consolas,Menlo,monospace}
h2{font-size:16px;margin:30px 0 4px}
ul{margin:8px 0;padding-left:20px}
</style></head><body><div class="wrap">

<h1>烘干设定 75℃ 冲到 90 多度 —— 仿真复核</h1>
<div class="meta">SL-1000 C7 &nbsp;|&nbsp; STM32F103RC / FreeRTOS V10.1 &nbsp;|&nbsp;
把固件控制律逐行移植为可执行模型，限定参数下做确定性实验 &nbsp;|&nbsp; 未修改任何源代码</div>

<div class="card verdict">
  <div class="k">结论：设定值真的是 750 时，闭环在数学上到不了 90℃</div>
  <div>标称参数下把设定 750 跑到底，峰值 <span class="big">75.0℃</span>，稳定在 74.0℃，
  软限幅（80℃）一次都没触发。要稳定跑出"90 多度"，RAM 里的设定值必须落在
  <b>910~929</b>；≥930 会被 92℃ 分支切断。</div>
  <div class="note">也就是说：这不是控制参数过冲的问题，是设定值在 RAM 里已经不是 750 的问题。</div>
  <div class="grid">
    <div class="tile"><div class="l">设定 750 → 峰值</div><div class="n" style="color:var(--cool)">75.0 ℃</div></div>
    <div class="tile"><div class="l">设定 7500 → 峰值</div><div class="n" style="color:var(--hot)">92.2 ℃</div></div>
    <div class="tile"><div class="l">稳定在 90+ 需要的设定区间</div><div class="n">910 ~ 929</div></div>
    <div class="tile"><div class="l">260 组随机硬件参数里能到 90+ 的比例</div><div class="n">2.3 %</div></div>
  </div>
</div>

<h2>图 1 &nbsp;同一套固件，只改 RAM 里的 Temset</h2>
<div class="svgbox">@@TRACE@@</div>
<div class="note">蓝线 Temset=750：到 75.0 后稳在 74.0（P 控制无积分，固定 −1℃ 稳态误差）。
红线 Temset=7500：单调升到 92.2℃ 被 92℃ 分支切断，然后一路回落到室温且不再恢复。红线就是现场"90 多度报烘干温度过高"的签名。</div>

<h2>图 2 &nbsp;加热体↔风换热系数 × 传感链总滞后 → 峰值温度</h2>
<div class="svgbox">@@MAP@@</div>
<div class="note">正常风道（hA ≥ 5 W/K）+ 正常 NTC 惯性（滞后 ≤ 10 s）时峰值只有 74~77℃。
260 组随机参数里只有 6 组（2.3%）冲过 90℃，这 6 组 hA 从 0.82 到 7.21 都有，唯一公共因子是<b>传感链总滞后 ≥ 9.7 s</b>。</div>

<h2>表 &nbsp;设定值 → 峰值温度（反推用）</h2>
<table>
<tr><th>Temset (RAM, 0.1℃)</th><th>等于</th><th>仿真峰值</th><th>保护动作</th></tr>
@@ROWS@@
</table>
<div class="note">读数 ≈ 设定值 − 1.0℃（斜率 1:1）。现场读到 91.5℃，则 RAM 里的设定值约为 92.5℃（925）。</div>

<h2>现场 1 分钟判读（不改代码）</h2>
<div class="card">
<ul>
<li>读 <code>0x20000228</code> 取结构体指针，再读 <code>ptr+0x2A</code> = <b>wHeatFanTemp</b>。</li>
<li>读 <code>0x20006928</code> = <b>stPiPidFan.Temset</b>（float，PID 真正用的设定）。</li>
<li><b>≥910</b> → 设定值被写错（单位 ×10 的典型路径：<code>TaskMove.c:1038</code> 的 <code>wParm[1]*10</code>，无范围校验）。</li>
<li><b>=750</b> 且读数仍到 90+ → 走风机/滞后方向：读 <code>0x4001140C</code> bit9 看风机是否真的开着。</li>
</ul>
</div>

<div class="note" style="margin-top:26px">详细推导、代码行号、所有工况数据见同目录
《烘干75度过冲仿真分析报告.md》。所有数值由 <code>Tools/sim_*.py</code> 生成，可复算。</div>

</div></body></html>"""
html = (html.replace("@@TRACE@@", svg("trace.svg"))
            .replace("@@MAP@@", svg("map.svg"))
            .replace("@@ROWS@@", rows))

out = os.path.join(HERE, "..", "烘干75度过冲仿真看板.html")
with open(out, "w", encoding="utf-8") as f:
    f.write(html)
print("wrote", os.path.abspath(out))
