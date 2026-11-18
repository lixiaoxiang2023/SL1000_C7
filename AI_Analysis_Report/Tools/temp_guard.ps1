<#
.SYNOPSIS
    烘干/固化温度链路只读守护监控（J-Link SWD，非侵入式）

.DESCRIPTION
    用途：在不改动固件、不写内存、不复位、不 halt CPU 的前提下，
          持续轮询温度闭环的全部关键变量，捕捉：
            - 加热被关闭事件（bHeatSta / bHeatFanSta 由 1 变 0）
            - 温度越限（主路 >900 = 90.0C，烘干路 >920 = 92.0C）
            - PWM 通道自停（blRunning 由 1 变 0，对应 pwm.c 的 dwStep 耗尽）
            - 调度器 tick 停滞（任务卡死）
            - 占空比突变

    安全约束：
          只发送 connect / Mem32 / Sleep / q；不发送 reset / write / erase / go。
          不修改任何目标内存。

.PARAMETER JLinkExe
    JLink.exe 完整路径

.PARAMETER Device
    J-Link 器件名，必须与工程一致

.PARAMETER DurationSec
    监控总时长（秒）

.PARAMETER IntervalMs
    采样间隔（毫秒）

.PARAMETER LogDir
    日志输出目录

.EXAMPLE
    .\temp_guard.ps1 -DurationSec 600 -IntervalMs 500
#>
[CmdletBinding()]
param(
    [string]$JLinkExe   = 'C:\Program Files\SEGGER\JLink_V990\JLink.exe',
    [string]$Device     = 'STM32F103xC',
    [int]   $DurationSec = 600,
    [int]   $IntervalMs  = 500,
    [string]$LogDir     = 'D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Build_Logs',
    [int]   $TempMainLimit = 900,
    [int]   $TempFanLimit  = 920
)

$ErrorActionPreference = 'Stop'
$script:exitCode = 0

function Info([string]$m) { Write-Host ('[INFO ] ' + $m) }
function Warn([string]$m) { Write-Host ('[WARN ] ' + $m) -ForegroundColor Yellow }
function Fail([string]$m) { Write-Host ('[FAIL ] ' + $m) -ForegroundColor Red }
function Step([string]$m) { Write-Host ('[STEP ] ' + $m) -ForegroundColor Cyan }

# ---------- 1. 参数校验 ----------
Step '校验参数与路径'
if (-not (Test-Path -LiteralPath $JLinkExe)) { Fail ('JLinkExe 不存在: ' + $JLinkExe); exit 2 }
if ($DurationSec -lt 5)   { Fail 'DurationSec 至少 5 秒'; exit 2 }
if ($IntervalMs  -lt 200) { Fail 'IntervalMs 至少 200ms（低于此值日志量过大）'; exit 2 }
if (-not (Test-Path -LiteralPath $LogDir)) { New-Item -ItemType Directory -Path $LogDir -Force | Out-Null }

$stamp     = Get-Date -Format 'yyyyMMdd_HHmmss'
$logFile   = Join-Path $LogDir ('temp_guard_' + $stamp + '.log')
$csvFile   = Join-Path $LogDir ('temp_guard_' + $stamp + '.csv')
$mdFile    = Join-Path $LogDir ('temp_guard_' + $stamp + '.md')
$jlinkFile = Join-Path $LogDir ('temp_guard_' + $stamp + '.jlink')

# ---------- 2. 采样表（全部地址取自 C7Detect.map，禁止臆造） ----------
# 布局依据：
#   stPiPid      @0x200068FC  Temset/Temin/dev/dev_next/dev_last/dev_sum/Kp/Ki/Kd/integral/fout
#   stPiPidFan   @0x20006928  同上
#   s_stTempDat  @0x20006A04  int wPwm; u16 wPwm_Old; u8 bySetDerat; bool TempCheck; u16 TimeCount
#   s_stTempDatFan @0x20006A10 同上
#   bHeatSta     @0x20006AB9  bHeatFanSta @0x20006ABA
#   DevCtr       @0x200067CC  chn0 @+0x04  stRun { dwStep@+0x08, dwStpCnt@+0x0C, blRunning@+0x10, bySpd@+0x11, bySpdCnt@+0x12 }
#                             chn1 @+0x18  stRun { dwStep@+0x20, dwStpCnt@+0x24, blRunning@+0x28, bySpd@+0x29, bySpdCnt@+0x2A }
$watch = @(
    @{ n = 'tick';   a = '0x20006A68'; w = 1; d = 'FreeRTOS xTickCount' },
    @{ n = 'temp';   a = '0x20005788'; w = 1; d = 'byTemp[0]主 / byTemp[1]扇' },
    @{ n = 'main';   a = '0x20006A04'; w = 3; d = '主路 wPwm/wPwmOld/derat/chk/timecnt' },
    @{ n = 'fan';    a = '0x20006A10'; w = 3; d = '风扇路 wPwm/wPwmOld/derat/chk/timecnt' },
    @{ n = 'pidm';   a = '0x200068FC'; w = 2; d = '主路 PID Temset/Temin' },
    @{ n = 'foutm';  a = '0x20006924'; w = 1; d = '主路 PID fout' },
    @{ n = 'dsm';    a = '0x20006910'; w = 1; d = '主路 PID dev_sum' },
    @{ n = 'pidf';   a = '0x20006928'; w = 2; d = '风扇路 PID Temset/Temin' },
    @{ n = 'foutf';  a = '0x20006950'; w = 1; d = '风扇路 PID fout' },
    @{ n = 'dsf';    a = '0x2000693C'; w = 1; d = '风扇路 PID dev_sum' },
    @{ n = 'heat';   a = '0x20006AB8'; w = 1; d = 'bHeatSta/bHeatFanSta' },
    @{ n = 'pwm0';   a = '0x200067D8'; w = 3; d = 'PWM chn0 dwStep/dwStpCnt/run|spd|cnt' },
    @{ n = 'pwm1';   a = '0x200067EC'; w = 3; d = 'PWM chn1 dwStep/dwStpCnt/run|spd|cnt' }
)

$samples = [int][math]::Floor(($DurationSec * 1000) / $IntervalMs)
if ($samples -lt 1) { $samples = 1 }

Info ('监控时长 ' + $DurationSec + 's，间隔 ' + $IntervalMs + 'ms，样本数 ' + $samples + '，监控项 ' + $watch.Count)

# ---------- 3. 生成 J-Link 脚本（只读命令） ----------
Step '生成 J-Link Commander 脚本（仅 connect/Mem32/Sleep/q）'
$l = New-Object System.Collections.Generic.List[string]
$l.Add('ExitOnError 0')
$l.Add('connect')
for ($s = 1; $s -le $samples; $s++) {
    foreach ($w in $watch) {
        $l.Add('Mem32 ' + $w.a + ', ' + $w.w)
    }
    if ($s -lt $samples) { $l.Add('Sleep ' + $IntervalMs) }
}
$l.Add('q')
$l | Set-Content -LiteralPath $jlinkFile -Encoding ASCII

# ---------- 4. 执行 ----------
Step '执行 J-Link 只读采样（不改内存 / 不复位 / 不 halt）'
$startTime = Get-Date
& $JLinkExe -device $Device -if SWD -speed auto -CommanderScript $jlinkFile *> $logFile
$nativeExit = $LASTEXITCODE
$endTime = Get-Date
$duration = $endTime - $startTime
Info ('J-Link 原生退出码: ' + $nativeExit + '，耗时 ' + [math]::Round($duration.TotalSeconds,1) + ' s')

if (-not (Test-Path -LiteralPath $logFile)) { Fail '未生成日志'; exit 4 }
$rawText = Get-Content -LiteralPath $logFile -Raw
if ($rawText -notmatch 'CPU is not halted|Cortex-M3 identified|Connect') {
    Warn '未捕获到连接成功标记，可能未接目标板'
}

# ---------- 5. 解析（按命令回显切段，规避 J-Link 折行重复打印地址的问题） ----------
Step '解析采样数据'
$blocks = [regex]::Matches($rawText, 'J-Link>Mem32\s+(0x[0-9A-Fa-f]+),\s*(\d+)\s*\r?\n([\s\S]*?)(?=J-Link>|$)')
Info ('捕获 Mem32 命令回显 ' + $blocks.Count + ' 条，预期 ' + ($samples * $watch.Count))

$perSample = $watch.Count
$valsByCmd = New-Object System.Collections.Generic.List[object]
foreach ($b in $blocks) {
    $vals = New-Object System.Collections.Generic.List[string]
    foreach ($line in ($b.Groups[3].Value -split "`r?`n")) {
        $m = [regex]::Match($line, '^\s*[0-9A-Fa-f]{8}\s*=\s*(.+)$')
        if ($m.Success) {
            foreach ($t in ($m.Groups[1].Value -split '\s+')) {
                if ($t -match '^[0-9A-Fa-f]{8}$') { $vals.Add($t) }
            }
        }
    }
    $valsByCmd.Add($vals)
}

$complete = [int][math]::Floor($valsByCmd.Count / $perSample)
Info ('完整样本数 ' + $complete)
if ($complete -lt 1) { Fail '未解析到任何完整样本'; exit 3 }

$rows = New-Object System.Collections.Generic.List[object]
for ($k = 0; $k -lt $complete; $k++) {
    $o = [ordered]@{
        Sample = $k + 1
        Tick = ''
        T_Main = ''; T_Fan = ''
        Main_wPwm = ''; Main_wPwmOld = ''; Main_Derat = ''; Main_Chk = ''; Main_TimeCnt = ''
        Fan_wPwm = '';  Fan_wPwmOld = '';  Fan_Derat = '';  Fan_Chk = '';  Fan_TimeCnt = ''
        Main_Temset = ''; Main_Temin = ''; Main_fout = ''; Main_devsum = ''
        Fan_Temset = '';  Fan_Temin = '';  Fan_fout = '';  Fan_devsum = ''
        bHeatSta = ''; bHeatFanSta = ''
        PWM0_dwStep = ''; PWM0_dwStpCnt = ''; PWM0_Run = ''; PWM0_Spd = ''; PWM0_SpdCnt = ''
        PWM1_dwStep = ''; PWM1_dwStpCnt = ''; PWM1_Run = ''; PWM1_Spd = ''; PWM1_SpdCnt = ''
    }
    for ($j = 0; $j -lt $perSample; $j++) {
        $w = $watch[$j]
        $v = $valsByCmd[$k * $perSample + $j]
        if ($v.Count -eq 0) { continue }
        switch ($w.n) {
            'tick'  { $o.Tick = [Convert]::ToUInt32($v[0], 16) }
            'temp'  {
                $t = [Convert]::ToUInt32($v[0], 16)
                $o.T_Main = [int]($t -band 0xFFFF)
                $o.T_Fan  = [int](($t -shr 16) -band 0xFFFF)
            }
            'main'  {
                $o.Main_wPwm = [Convert]::ToInt32($v[0], 16)
                if ($v.Count -ge 2) {
                    $x = [Convert]::ToUInt32($v[1], 16)
                    $o.Main_wPwmOld = [int]($x -band 0xFFFF)
                    $o.Main_Derat   = [int](($x -shr 16) -band 0xFF)
                    $o.Main_Chk     = [int](($x -shr 24) -band 0xFF)
                }
                if ($v.Count -ge 3) { $o.Main_TimeCnt = [Convert]::ToInt32($v[2], 16) }
            }
            'fan'   {
                $o.Fan_wPwm = [Convert]::ToInt32($v[0], 16)
                if ($v.Count -ge 2) {
                    $x = [Convert]::ToUInt32($v[1], 16)
                    $o.Fan_wPwmOld = [int]($x -band 0xFFFF)
                    $o.Fan_Derat   = [int](($x -shr 16) -band 0xFF)
                    $o.Fan_Chk     = [int](($x -shr 24) -band 0xFF)
                }
                if ($v.Count -ge 3) { $o.Fan_TimeCnt = [Convert]::ToInt32($v[2], 16) }
            }
            'pidm'  { if ($v.Count -ge 2) { $o.Main_Temset = $v[0]; $o.Main_Temin = $v[1] } }
            'foutm' { $o.Main_fout = $v[0] }
            'dsm'   { $o.Main_devsum = $v[0] }
            'pidf'  { if ($v.Count -ge 2) { $o.Fan_Temset = $v[0]; $o.Fan_Temin = $v[1] } }
            'foutf' { $o.Fan_fout = $v[0] }
            'dsf'   { $o.Fan_devsum = $v[0] }
            'heat'  {
                $x = [Convert]::ToUInt32($v[0], 16)
                $o.bHeatSta    = [int](($x -shr 8)  -band 0xFF)
                $o.bHeatFanSta = [int](($x -shr 16) -band 0xFF)
            }
            'pwm0'  {
                if ($v.Count -ge 3) {
                    $o.PWM0_dwStep   = [Convert]::ToInt32($v[0], 16)
                    $o.PWM0_dwStpCnt = [Convert]::ToInt32($v[1], 16)
                    $x = [Convert]::ToUInt32($v[2], 16)
                    $o.PWM0_Run    = [int]($x -band 0xFF)
                    $o.PWM0_Spd    = [int](($x -shr 8)  -band 0xFF)
                    $o.PWM0_SpdCnt = [int](($x -shr 16) -band 0xFF)
                }
            }
            'pwm1'  {
                if ($v.Count -ge 3) {
                    $o.PWM1_dwStep   = [Convert]::ToInt32($v[0], 16)
                    $o.PWM1_dwStpCnt = [Convert]::ToInt32($v[1], 16)
                    $x = [Convert]::ToUInt32($v[2], 16)
                    $o.PWM1_Run    = [int]($x -band 0xFF)
                    $o.PWM1_Spd    = [int](($x -shr 8)  -band 0xFF)
                    $o.PWM1_SpdCnt = [int](($x -shr 16) -band 0xFF)
                }
            }
        }
    }
    $rows.Add([pscustomobject]$o)
}
$rows | Export-Csv -LiteralPath $csvFile -NoTypeInformation -Encoding UTF8

# ---------- 6. 事件检测 ----------
Step '检测异常事件'
$events = New-Object System.Collections.Generic.List[string]
$prev = $null
foreach ($r in $rows) {
    if ($null -ne $prev) {
        if ($prev.bHeatSta -eq 1 -and $r.bHeatSta -eq 0) {
            $events.Add('样本 ' + $r.Sample + '：**主加热 bHeatSta 1→0**（CloseTemp 生效），T主=' + [math]::Round($r.T_Main/10.0,1) + 'C')
        }
        if ($prev.bHeatFanSta -eq 1 -and $r.bHeatFanSta -eq 0) {
            $events.Add('样本 ' + $r.Sample + '：**风扇加热 bHeatFanSta 1→0**（CloseHeatFan/CloseTemp），T扇=' + [math]::Round($r.T_Fan/10.0,1) + 'C')
        }
        if ($null -ne $prev.PWM0_Run -and $prev.PWM0_Run -eq 1 -and $r.PWM0_Run -eq 0) {
            $events.Add('样本 ' + $r.Sample + '：PWM chn0 由运行转停止（dwStpCnt=' + $r.PWM0_dwStpCnt + '/' + $r.PWM0_dwStep + '）')
        }
        if ($null -ne $prev.PWM1_Run -and $prev.PWM1_Run -eq 1 -and $r.PWM1_Run -eq 0) {
            $events.Add('样本 ' + $r.Sample + '：PWM chn1 由运行转停止（dwStpCnt=' + $r.PWM1_dwStpCnt + '/' + $r.PWM1_dwStep + '）')
        }
        if ($null -ne $prev.Tick -and $r.Tick -eq $prev.Tick) {
            $events.Add('样本 ' + $r.Sample + '：xTickCount 未推进（上一值 ' + $prev.Tick + '），调度器或 SWD 读取异常')
        }
        if ($r.T_Main -gt $TempMainLimit) {
            $events.Add('样本 ' + $r.Sample + '：**主路温度越限** ' + [math]::Round($r.T_Main/10.0,1) + 'C > ' + [math]::Round($TempMainLimit/10.0,1) + 'C')
        }
        if ($r.T_Fan -gt $TempFanLimit) {
            $events.Add('样本 ' + $r.Sample + '：**风扇路温度越限** ' + [math]::Round($r.T_Fan/10.0,1) + 'C > ' + [math]::Round($TempFanLimit/10.0,1) + 'C')
        }
        if ($null -ne $prev.Main_wPwm -and [math]::Abs($r.Main_wPwm - $prev.Main_wPwm) -ge 30) {
            $events.Add('样本 ' + $r.Sample + '：主路占空比突变 ' + $prev.Main_wPwm + ' -> ' + $r.Main_wPwm)
        }
    }
    $prev = $r
}
if ($events.Count -eq 0) { Info '未检测到异常事件' } else { Info ('检测到 ' + $events.Count + ' 条事件') }

# ---------- 7. 统计摘要 ----------
$tMainVals = @($rows | Where-Object { $_.T_Main -ne '' } | ForEach-Object { $_.T_Main })
$tFanVals  = @($rows | Where-Object { $_.T_Fan  -ne '' } | ForEach-Object { $_.T_Fan })
$wPwmVals  = @($rows | Where-Object { $_.Main_wPwm -ne '' } | ForEach-Object { $_.Main_wPwm })

function Stat($arr, $scale) {
    if ($arr.Count -eq 0) { return 'n/a' }
    $min = ($arr | Measure-Object -Minimum).Minimum
    $max = ($arr | Measure-Object -Maximum).Maximum
    $avg = ($arr | Measure-Object -Average).Average
    return ('min ' + [math]::Round($min/$scale,1) + ' / avg ' + [math]::Round($avg/$scale,1) + ' / max ' + [math]::Round($max/$scale,1))
}

$tickFirst = $rows[0].Tick
$tickLast  = $rows[$rows.Count-1].Tick
$tickRate  = 'n/a'
if ($tickFirst -ne '' -and $tickLast -ne '' -and $tickLast -gt $tickFirst) {
    $elapsed = ($rows.Count - 1) * $IntervalMs / 1000.0
    if ($elapsed -gt 0) { $tickRate = [math]::Round(($tickLast - $tickFirst) / $elapsed, 1) }
}

# ---------- 8. Markdown 报告 ----------
$md = New-Object System.Collections.Generic.List[string]
$md.Add('# 温度链路只读守护监控报告')
$md.Add('')
$md.Add('| 项目 | 值 |')
$md.Add('|---|---|')
$md.Add('| 日志时间 | ' + $stamp + ' |')
$md.Add('| 监控时长 | ' + $DurationSec + ' s |')
$md.Add('| 采样间隔 | ' + $IntervalMs + ' ms |')
$md.Add('| 完整样本 | ' + $complete + ' |')
$md.Add('| 器件 | ' + $Device + '（J-Link 连接名） |')
$md.Add('| 原生退出码 | ' + $nativeExit + ' |')
$md.Add('| 原始日志 | temp_guard_' + $stamp + '.log |')
$md.Add('')
$md.Add('## 温度统计')
$md.Add('')
$md.Add('| 通道 | 最小 / 平均 / 最大（C） |')
$md.Add('|---|---|')
$md.Add('| 主路 NTC(CH15) | ' + (Stat $tMainVals 10) + ' |')
$md.Add('| 烘干路 NTC(CH14) | ' + (Stat $tFanVals 10) + ' |')
$md.Add('| 主路 PID 输出 wPwm | ' + (Stat $wPwmVals 1) + ' |')
$md.Add('')
$md.Add('## 调度活性')
$md.Add('')
$md.Add('- xTickCount：' + $tickFirst + ' → ' + $tickLast)
$md.Add('- 实测 tick 频率：**' + $tickRate + ' Hz**（工程配置 configTICK_RATE_HZ = 1000）')
$md.Add('')
$md.Add('## 事件时间线')
$md.Add('')
if ($events.Count -eq 0) {
    $md.Add('未检测到异常事件。')
} else {
    foreach ($e in $events) { $md.Add('- ' + $e) }
}
$md.Add('')
$md.Add('## 采样首末对比')
$md.Add('')
if ($rows.Count -ge 2) {
    $a = $rows[0]; $b = $rows[$rows.Count-1]
    $md.Add('| 变量 | 首样本 | 末样本 |')
    $md.Add('|---|---|---|')
    $md.Add('| T主 (C) | ' + [math]::Round($a.T_Main/10.0,1) + ' | ' + [math]::Round($b.T_Main/10.0,1) + ' |')
    $md.Add('| T扇 (C) | ' + [math]::Round($a.T_Fan/10.0,1) + ' | ' + [math]::Round($b.T_Fan/10.0,1) + ' |')
    $md.Add('| 主 wPwm | ' + $a.Main_wPwm + ' | ' + $b.Main_wPwm + ' |')
    $md.Add('| 主 wPwmOld | ' + $a.Main_wPwmOld + ' | ' + $b.Main_wPwmOld + ' |')
    $md.Add('| 主 derat | ' + $a.Main_Derat + ' | ' + $b.Main_Derat + ' |')
    $md.Add('| 主 TimeCnt | ' + $a.Main_TimeCnt + ' | ' + $b.Main_TimeCnt + ' |')
    $md.Add('| 扇 wPwm | ' + $a.Fan_wPwm + ' | ' + $b.Fan_wPwm + ' |')
    $md.Add('| 扇 derat | ' + $a.Fan_Derat + ' | ' + $b.Fan_Derat + ' |')
    $md.Add('| bHeatSta | ' + $a.bHeatSta + ' | ' + $b.bHeatSta + ' |')
    $md.Add('| bHeatFanSta | ' + $a.bHeatFanSta + ' | ' + $b.bHeatFanSta + ' |')
    $md.Add('| PWM0 dwStep | ' + $a.PWM0_dwStep + ' | ' + $b.PWM0_dwStep + ' |')
    $md.Add('| PWM0 dwStpCnt | ' + $a.PWM0_dwStpCnt + ' | ' + $b.PWM0_dwStpCnt + ' |')
    $md.Add('| PWM0 Run/Spd | ' + $a.PWM0_Run + '/' + $a.PWM0_Spd + ' | ' + $b.PWM0_Run + '/' + $b.PWM0_Spd + ' |')
}
$md.Add('')
$md.Add('> 原始日志已完整保留：temp_guard_' + $stamp + '.log；本文件仅为摘要视图。')
$md | Set-Content -LiteralPath $mdFile -Encoding UTF8

Write-Host ''
Info ('日志: ' + $logFile)
Info ('CSV : ' + $csvFile)
Info ('摘要: ' + $mdFile)

if ($complete -lt 1) { $script:exitCode = 3 }
exit $script:exitCode
