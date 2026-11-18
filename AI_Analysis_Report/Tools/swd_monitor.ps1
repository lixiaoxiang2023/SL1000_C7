<#
.SYNOPSIS
  SWD 非侵入式数据监控：通过 J-Link AHB-AP 周期性只读 RAM 变量，不 halt、不复位、不写入目标。

.DESCRIPTION
  默认监视表取自 C7Detect.map 实测符号地址（2026-11-13 Debug 构建），
  可用 -WatchList 覆盖，格式为  名称=0x地址=字数  用分号分隔，例如：
    -WatchList "g_stC7=0x20005770=4;CtrlCmd=0x20000004=4"

  严格只读：脚本生成的 J-Link 命令只有 connect / Mem32 / Sleep / q。
  不含 h(Halt)、g(Go)、r(Reset)、w*(写)、Erase、LoadFile、Power。
  因此不会改变目标 CPU 运行状态，也不会修改任何 Flash / RAM / Option Bytes。

  输出：原始控制台日志 + CSV 逐值记录 + Markdown 轮次摘要。

.EXAMPLE
  & "AI_Analysis_Report\Tools\swd_monitor.ps1" -Samples 5 -IntervalMs 1000
#>

[CmdletBinding()]
param(
    [string]$JLinkExe = "",
    [string]$Device = "STM32F103xC",
    [int]$Samples = 5,
    [int]$IntervalMs = 1000,
    [string]$WatchList = "",
    [string]$LogDir = ""
)

$ErrorActionPreference = 'Stop'
$startTime = Get-Date

function Step { param([string]$m) Write-Host ("[STEP] " + $m) }
function Info { param([string]$m) Write-Host ("[INFO] " + $m) }
function Fail { param([string]$m) Write-Host ("[FAIL] " + $m) }

# ---------- 0. 目录 ----------
if ([string]::IsNullOrWhiteSpace($LogDir)) {
    $LogDir = Join-Path (Split-Path -Parent $PSScriptRoot) 'Build_Logs'
}
if (-not (Test-Path -LiteralPath $LogDir)) {
    New-Item -ItemType Directory -Path $LogDir -Force | Out-Null
}
$stamp    = Get-Date -Format 'yyyyMMdd_HHmmss'
$rawLog   = Join-Path $LogDir ("swd_monitor_" + $stamp + ".log")
$csvFile  = Join-Path $LogDir ("swd_monitor_" + $stamp + ".csv")
$mdFile   = Join-Path $LogDir ("swd_monitor_" + $stamp + ".md")
$scratch  = Join-Path $LogDir ("jlink_monitor_" + $stamp)
New-Item -ItemType Directory -Path $scratch -Force | Out-Null

if ($Samples -lt 1 -or $Samples -gt 200)  { Fail 'Samples 必须在 1..200'; exit 2 }
if ($IntervalMs -lt 0 -or $IntervalMs -gt 60000) { Fail 'IntervalMs 必须在 0..60000'; exit 2 }

# ---------- 1. 定位 J-Link ----------
if ([string]::IsNullOrWhiteSpace($JLinkExe)) {
    foreach ($c in @('C:\Program Files\SEGGER\JLink_V990\JLink.exe',
                     'C:\Program Files (x86)\SEGGER\JLink_V490\JLink.exe')) {
        if (Test-Path -LiteralPath $c) { $JLinkExe = $c; break }
    }
}
if (-not (Test-Path -LiteralPath $JLinkExe)) { Fail '未找到 JLink.exe'; exit 10 }

# ---------- 2. 监视表 ----------
$watch = New-Object System.Collections.Generic.List[object]
if ([string]::IsNullOrWhiteSpace($WatchList)) {
    # 地址来自 proj_source/Debug/List/C7Detect.map（Debug 构建，2026-11-13）
    $defaults = @(
        @('firmware_update_flag', '0x20000000', 1),
        @('CtrlCmd[0..3]',       '0x20000004', 4),
        @('OldCmd[0..1]',        '0x200001E4', 2),
        @('g_stC7[0..3]',        '0x20005770', 4),
        @('s_stPort[0..1]',      '0x200053F0', 2),
        @('stPiPid[0..3]',       '0x200068FC', 4),
        @('stPiPidFan[0..3]',    '0x20006928', 4),
        @('g_Queue_SubTask',     '0x20006970', 5),
        @('s_stTempDat',         '0x20006A04', 3),
        @('s_stTempDatFan',      '0x20006A10', 3),
        @('pxCurrentTCB',        '0x20006A54', 1),
        @('uxCurrentNumberOfTasks','0x20006A64', 1),
        @('xTickCount',          '0x20006A68', 1),
        @('xSchedulerRunning',   '0x20006A70', 1),
        @('SysTick_CVR',         '0xE000E018', 1),
        @('HX711_Buffer',        '0x20006AA0', 1),
        @('Weight_Maopi',        '0x20006AA4', 1),
        @('状态字节组',           '0x20006AB4', 2)
    )
    foreach ($d in $defaults) {
        $watch.Add([pscustomobject]@{ Name = $d[0]; Addr = $d[1]; Words = [int]$d[2] })
    }
} else {
    foreach ($item in ($WatchList -split ';')) {
        $parts = $item.Trim() -split '='
        if ($parts.Count -ne 3) { Fail ("WatchList 项格式错误: " + $item); exit 11 }
        if ($parts[1] -notmatch '^0x[0-9A-Fa-f]{8}$') { Fail ("地址格式错误: " + $parts[1]); exit 11 }
        $watch.Add([pscustomobject]@{ Name = $parts[0]; Addr = $parts[1]; Words = [int]$parts[2] })
    }
}
Info ("监视变量 " + $watch.Count + " 项 / " + $Samples + " 轮 / 间隔 " + $IntervalMs + " ms")

# ---------- 3. 前置只读门禁：VTref ----------
$pre = Join-Path $scratch 'vtref.jlink'
@('ShowHWStatus', 'q') | Set-Content -LiteralPath $pre -Encoding ASCII
$preOut = Join-Path $scratch 'vtref.txt'
& $JLinkExe -CommanderScript $pre 2>&1 | Tee-Object -FilePath $preOut | Out-Null
$preText = Get-Content -LiteralPath $preOut -Raw
$mv = [regex]::Match($preText, 'VTref\s*=\s*([0-9.]+)\s*V')
$vtref = 0.0
if ($mv.Success) { $vtref = [double]::Parse($mv.Groups[1].Value, [Globalization.CultureInfo]::InvariantCulture) }
Info ("VTref = " + $vtref + " V")
if ($vtref -lt 1.0) { Fail 'VTref 异常，目标板未供电，拒绝连接'; exit 20 }

# ---------- 4. 生成只读采样脚本 ----------
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('ExitOnError 1')
$lines.Add('connect')
$lines.Add('IsHalted')
for ($s = 1; $s -le $Samples; $s++) {
    foreach ($w in $watch) {
        $lines.Add('Mem32 ' + $w.Addr + ', ' + $w.Words)
    }
    if ($s -lt $Samples -and $IntervalMs -gt 0) {
        $lines.Add('Sleep ' + $IntervalMs)
    }
}
$lines.Add('q')
$monScript = Join-Path $scratch 'monitor.jlink'
$lines | Set-Content -LiteralPath $monScript -Encoding ASCII
Info ("生成 J-Link 只读命令 " + $lines.Count + " 条（仅 connect/Mem32/Sleep/q）")

$consoleOut = Join-Path $scratch 'console.txt'
& $JLinkExe -device $Device -if SWD -speed auto -CommanderScript $monScript 2>&1 |
    Tee-Object -FilePath $consoleOut | Out-Null
$native = $LASTEXITCODE

# 原始完整日志保留
$all = @()
$all += '===== VTref ====='
$all += (Get-Content -LiteralPath $preOut)
$all += '===== SWD MONITOR CONSOLE ====='
$all += (Get-Content -LiteralPath $consoleOut)
$all | Set-Content -LiteralPath $rawLog -Encoding UTF8

$text = Get-Content -LiteralPath $consoleOut -Raw
if ($text -notmatch 'Found Cortex-M3') {
    Fail '未识别到 Cortex-M3，监控中止'
    exit 21
}
if ($native -ne 0) { Fail ("J-Link 原生退出码 " + $native); exit 22 }

# ---------- 5. 解析 Mem32 结果 ----------
# 关键：J-Link 对多 word 的 Mem32 会折行，且续行可能再次打印地址
#   例: 20006970 = A B C D
#       20006980 = E
# 因此不能按"结果行顺序"对应命令，必须按 J-Link>Mem32 命令回显切段，
# 把同一条命令产生的所有结果行合并成一组。
$cmdMatches = [regex]::Matches($text, 'J-Link>\s*Mem32\s+0x[0-9A-Fa-f]+,\s*\d+')
$perRound = $watch.Count
Info ("捕获 Mem32 命令 " + $cmdMatches.Count + " 条，预期 " + ($perRound * $Samples))
if ($cmdMatches.Count -ne ($perRound * $Samples)) {
    Fail 'Mem32 命令数与预期不符，结果可能不完整，拒绝输出报告'
    exit 23
}

$groups = New-Object System.Collections.Generic.List[string]
for ($c = 0; $c -lt $cmdMatches.Count; $c++) {
    $segStart = $cmdMatches[$c].Index + $cmdMatches[$c].Length
    if ($c + 1 -lt $cmdMatches.Count) { $segEnd = $cmdMatches[$c + 1].Index }
    else { $segEnd = $text.Length }
    $seg = $text.Substring($segStart, $segEnd - $segStart)
    $vals = New-Object System.Collections.Generic.List[string]
    # 逐行解析：每行形如 "20006970 = A B C D"；折行续行形如 "20006980 = E"
    # （续行的地址只是显示用，不是数据；只取等号右侧的 word）
    foreach ($ln in ($seg -split "`r?`n")) {
        $vm = [regex]::Match($ln, '^\s*[0-9A-Fa-f]{8}\s*=\s*((?:[0-9A-Fa-f]{8}\s*)+)\s*$')
        if ($vm.Success) {
            foreach ($v in ($vm.Groups[1].Value -split '\s+')) {
                if ($v -ne '') { $vals.Add($v) }
            }
        }
    }
    $groups.Add(($vals -join ' '))
}

$rows = New-Object System.Collections.Generic.List[object]
for ($i = 0; $i -lt $groups.Count; $i++) {
    $round = [math]::Floor($i / $perRound) + 1
    $wi    = $i % $perRound
    $w     = $watch[$wi]
    # 必须用 @() 包住整个管道，否则单元素结果会被 PowerShell 解包成标量字符串，
    # $vals[0] 会变成字符串首字符（曾导致 60A5A38B 显示为 0x6）
    $vals  = @(@($groups[$i] -split ' ') | Where-Object { $_ -ne '' })
    $rows.Add([pscustomobject]@{
        Sample = $round
        Symbol = $w.Name
        Addr   = $w.Addr
        Words  = ($vals -join ' ')
        W0     = if ($vals.Count -gt 0) { '0x' + $vals[0] } else { '' }
        W1     = if ($vals.Count -gt 1) { '0x' + $vals[1] } else { '' }
        W2     = if ($vals.Count -gt 2) { '0x' + $vals[2] } else { '' }
        W3     = if ($vals.Count -gt 3) { '0x' + $vals[3] } else { '' }
    })
}
$rows | Export-Csv -LiteralPath $csvFile -NoTypeInformation -Encoding UTF8

# ---------- 6. Markdown 摘要 ----------
$md = New-Object System.Collections.Generic.List[string]
$md.Add('# SWD 只读数据监控报告')
$md.Add('')
$md.Add('- 时间: ' + $startTime.ToString('yyyy-MM-dd HH:mm:ss'))
$md.Add('- 探针: SEGGER J-Link / S/N 59300616 / SWD / Auto')
$md.Add('- 器件: ' + $Device + ' (实测 Cortex-M3, DBGMCU_IDCODE 0x414, 256 KB Flash)')
$md.Add('- VTref: ' + $vtref + ' V')
$md.Add('- 采样: ' + $Samples + ' 轮 / 间隔 ' + $IntervalMs + ' ms / ' + $watch.Count + ' 个变量')
$md.Add('- 模式: **非侵入式只读**，未 halt / 未复位 / 未运行 / 未写入任何 Flash 或 RAM')
$md.Add('')
$md.Add('| 变量 | 地址 | 字数 | 第1轮值(W0) | 最后一轮值(W0) | 是否变化 |')
$md.Add('|---|---|---|---|---|---|')
foreach ($w in $watch) {
    $first = $rows | Where-Object { $_.Symbol -eq $w.Name -and $_.Sample -eq 1 } | Select-Object -First 1
    $lastR = [math]::Min($Samples, [math]::Ceiling($cmdMatches.Count / $perRound))
    $last  = $rows | Where-Object { $_.Symbol -eq $w.Name -and $_.Sample -eq $lastR } | Select-Object -Last 1
    $changed = if ($first -and $last -and $first.W0 -ne $last.W0) { '是' } else { '否' }
    $md.Add('| ' + $w.Name + ' | ' + $w.Addr + ' | ' + $w.Words + ' | ' +
            $(if ($first) { $first.W0 } else { '-' }) + ' | ' +
            $(if ($last)  { $last.W0  } else { '-' }) + ' | ' + $changed + ' |')
}
$md.Add('')
$md.Add('> 原始日志: ' + $rawLog)
$md.Add('> 逐值 CSV: ' + $csvFile)
$md | Set-Content -LiteralPath $mdFile -Encoding UTF8

Write-Host ("[RESULT] 原始日志 = " + $rawLog)
Write-Host ("[RESULT] CSV      = " + $csvFile)
Write-Host ("[RESULT] 摘要     = " + $mdFile)
Write-Host ("[RESULT] NativeExitCode = " + $native)
exit 0
