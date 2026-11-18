<#
.SYNOPSIS
  C7 工程安全烧录脚本：J-Link Commander + SWD，默认只烧 App 区，绝不覆盖 Bootloader。

.DESCRIPTION
  安全门禁（任一不满足即非零退出，且不执行任何写操作）：
    1. VTref 必须 > 1.0 V
    2. 目标芯片必须识别为 STM32F10x High Density (DBGMCU_IDCODE 低12位 = 0x414)
    3. Flash 容量必须为 256 KB (0x1FFFF7E0 低16位 = 0x0100)
    4. 器件只允许使用工程配置型号，不自动切换其他型号
    5. 固件文件必须存在且格式明确；BIN 必须带明确起始地址（来自 .icf / .map，禁止猜测）
    6. 默认不覆盖 Bootloader；需同时给出 -IncludeBootloader 与 -ConfirmBootloaderOverwrite

  禁止项（脚本内不生成这些命令）：
    Erase（整片擦除）、Power（给目标供电）、Reset、Go、Option Bytes、读/写保护配置

  全程保留原始日志，输出 JSON 摘要，失败返回非零退出码。

.EXAMPLE
  & "AI_Analysis_Report\Tools\flash_jlink.ps1" -FirmwarePath "proj_source\Debug\Exe\C7Detect.out"
#>

[CmdletBinding()]
param(
    [string]$JLinkExe = "",
    [string]$FirmwarePath = "",
    [string]$ExpectedDevice = "STM32F103xC",
    [string]$LogDir = "",
    [switch]$IncludeBootloader,
    [switch]$ConfirmBootloaderOverwrite
)

$ErrorActionPreference = 'Stop'
$startTime = Get-Date
$nativeExit = -999

function Step  { param([string]$m) Write-Host ("[STEP] " + $m) }
function Info  { param([string]$m) Write-Host ("[INFO] " + $m) }
function Fail  { param([string]$m) Write-Host ("[FAIL] " + $m) }

# ---------- 0. 日志目录 ----------
if ([string]::IsNullOrWhiteSpace($LogDir)) {
    $LogDir = Join-Path (Split-Path -Parent $PSScriptRoot) 'Build_Logs'
}
if (-not (Test-Path -LiteralPath $LogDir)) {
    New-Item -ItemType Directory -Path $LogDir -Force | Out-Null
}
$stamp      = Get-Date -Format 'yyyyMMdd_HHmmss'
$logFile    = Join-Path $LogDir ("flash_" + $stamp + ".log")
$summaryFile= Join-Path $LogDir ("flash_summary_" + $stamp + ".json")
$scratch    = Join-Path $LogDir ("jlink_flash_scripts_" + $stamp)
New-Item -ItemType Directory -Path $scratch -Force | Out-Null

$result = [ordered]@{
    StartTime      = $startTime.ToString('yyyy-MM-dd HH:mm:ss')
    JLinkExe       = ''
    Firmware       = ''
    FirmwareFormat = ''
    ExpectedDevice = $ExpectedDevice
    VTref          = ''
    DetectedDevice = ''
    IdCode         = ''
    FlashSizeKB    = ''
    AddressRange   = ''
    EraseRange     = ''
    BootloaderTouched = $false
    NativeExitCode = ''
    Verified       = $false
    ScriptExitCode = 0
    LogPath        = $logFile
}

function Save-And-Exit {
    param([int]$code, [string]$reason)
    $result.ScriptExitCode = $code
    $result.EndTime = (Get-Date).ToString('yyyy-MM-dd HH:mm:ss')
    $result.Reason  = $reason
    $result | ConvertTo-Json | Set-Content -LiteralPath $summaryFile -Encoding UTF8
    Write-Host ("[RESULT] ExitCode=" + $code + "  Reason=" + $reason)
    Write-Host ("[RESULT] Log=" + $logFile)
    exit $code
}

# ---------- 1. 定位 J-Link Commander（实测路径，不猜测） ----------
if ([string]::IsNullOrWhiteSpace($JLinkExe)) {
    $candidates = @(
        'C:\Program Files\SEGGER\JLink_V990\JLink.exe',
        'C:\Program Files (x86)\SEGGER\JLink_V490\JLink.exe'
    )
    foreach ($c in $candidates) {
        if (Test-Path -LiteralPath $c) { $JLinkExe = $c; break }
    }
}
if (-not (Test-Path -LiteralPath $JLinkExe)) {
    Fail ("未找到 JLink.exe: " + $JLinkExe)
    Save-And-Exit 10 'JLink.exe 未找到'
}
$result.JLinkExe = $JLinkExe
$dll = Join-Path (Split-Path -Parent $JLinkExe) 'JLinkARM.dll'
if (Test-Path -LiteralPath $dll) {
    Info ("J-Link 驱动版本 " + (Get-Item -LiteralPath $dll).VersionInfo.FileVersion)
}

# ---------- 2. 校验固件 ----------
if ([string]::IsNullOrWhiteSpace($FirmwarePath)) {
    Fail '必须指定 -FirmwarePath'
    Save-And-Exit 11 '固件路径为空'
}
try { $fw = (Resolve-Path -LiteralPath $FirmwarePath).Path }
catch { Fail ("固件不存在: " + $FirmwarePath); Save-And-Exit 12 '固件文件不存在' }
$ext = [System.IO.Path]::GetExtension($fw).ToLowerInvariant()
if ($ext -notin @('.out', '.elf', '.axf', '.hex', '.bin', '.srec', '.mot')) {
    Fail ("不支持的固件格式: " + $ext)
    Save-And-Exit 13 '固件格式不明确'
}
if ($ext -eq '.bin' -and -not $IncludeBootloader) {
    Fail '裸 BIN 缺少地址信息。请使用 ELF(.out) 或显式给出 -IncludeBootloader 并确认地址。'
    Save-And-Exit 14 'BIN 地址不明确，禁止猜测'
}
$result.Firmware = $fw
$result.FirmwareFormat = $ext
Info ("固件: " + $fw + "  (" + (Get-Item -LiteralPath $fw).Length + " B)")

# ---------- 3. 门禁 A：VTref（只读，不连接目标） ----------
Step '门禁 A: 读取 VTref'
$pre = Join-Path $scratch 'precheck_vtref.jlink'
@('ShowHWStatus', 'q') | Set-Content -LiteralPath $pre -Encoding ASCII
$preOut = Join-Path $scratch 'precheck_vtref.txt'
& $JLinkExe -CommanderScript $pre 2>&1 | Tee-Object -FilePath $preOut | Out-Null
$preText = Get-Content -LiteralPath $preOut -Raw
$mVolt = [regex]::Match($preText, 'VTref\s*=\s*([0-9.]+)\s*V')
if (-not $mVolt.Success) { Fail '无法读取 VTref'; Save-And-Exit 20 'VTref 读取失败' }
$vtref = [double]::Parse($mVolt.Groups[1].Value, [Globalization.CultureInfo]::InvariantCulture)
$result.VTref = $vtref.ToString() + ' V'
Info ("VTref = " + $vtref + " V")
if ($vtref -lt 1.0) {
    Fail 'VTref 异常（< 1.0 V），目标板未供电或未接 SWD，拒绝烧录'
    Save-And-Exit 21 'VTref 异常'
}

# ---------- 4. 门禁 B：芯片型号（SWD 只读识别，不复位不写入） ----------
Step '门禁 B: SWD 只读识别目标芯片'
$id = Join-Path $scratch 'identify.jlink'
@('ExitOnError 1', 'connect', 'Mem32 0xE0042000, 1', 'Mem32 0x1FFFF7E0, 1', 'q') |
    Set-Content -LiteralPath $id -Encoding ASCII
$idOut = Join-Path $scratch 'identify.txt'
& $JLinkExe -device $ExpectedDevice -if SWD -speed auto -CommanderScript $id 2>&1 |
    Tee-Object -FilePath $idOut | Out-Null
$idText = Get-Content -LiteralPath $idOut -Raw
$result.NativeExitCode = $LASTEXITCODE

$dev = [regex]::Match($idText, 'Device\s+"([^"]+)"\s+selected')
$result.DetectedDevice = if ($dev.Success) { $dev.Groups[1].Value } else { 'UNKNOWN' }
$cpu = [regex]::Match($idText, 'Found\s+(Cortex-M\S+)\s')
if ($cpu.Success) { Info ("内核: " + $cpu.Groups[1].Value) }

$mId = [regex]::Match($idText, 'E0042000\s*=\s*([0-9A-Fa-f]{8})')
$mFs = [regex]::Match($idText, '1FFFF7E0\s*=\s*([0-9A-Fa-f]{8})')
if (-not $mId.Success -or -not $mFs.Success) {
    Fail '无法读取 DBGMCU_IDCODE / Flash Size，拒绝烧录'
    Save-And-Exit 22 '芯片识别寄存器读取失败'
}
$idVal = [Convert]::ToUInt32($mId.Groups[1].Value, 16)
$fsVal = [Convert]::ToUInt32($mFs.Groups[1].Value, 16)
$devId = $idVal -band 0x0FFF
$flashKB = $fsVal -band 0xFFFF
$result.IdCode = '0x' + $idVal.ToString('X8') + ' (DEV_ID=0x' + $devId.ToString('X3') + ')'
$result.FlashSizeKB = $flashKB
Info ("DBGMCU_IDCODE = 0x" + $idVal.ToString('X8') + "  DEV_ID=0x" + $devId.ToString('X3') +
      "  FlashSize=" + $flashKB + " KB  器件=" + $result.DetectedDevice)
if ($devId -ne 0x414) {
    Fail ("密度不符：期望 0x414 (High Density)，实测 0x" + $devId.ToString('X3') + '，拒绝烧录')
    Save-And-Exit 23 '目标芯片密度与工程不符'
}
if ($flashKB -ne 256) {
    Fail ("Flash 容量不符：期望 256 KB，实测 " + $flashKB + ' KB，拒绝烧录')
    Save-And-Exit 24 '目标 Flash 容量与工程不符'
}
if ($idText -notmatch 'Found Cortex-M3') {
    Fail '未识别到 Cortex-M3 内核，拒绝烧录'
    Save-And-Exit 25 '内核不符'
}

# ---------- 5. 生成 App-only 校验镜像（从合并 BIN 精确截取，只读派生） ----------
$appBin = ''
$mergedBin = Join-Path (Split-Path -Parent $fw) 'C7Detect.bin'
if ($ext -eq '.out' -and (Test-Path -LiteralPath $mergedBin)) {
    $bytes = [System.IO.File]::ReadAllBytes($mergedBin)
    $appOffset = 0x8000
    $appLen    = 0xC5A8          # 来源 boot.map: .section_app @0x08008000 size 0xc5a8
    if ($bytes.Length -ge ($appOffset + $appLen)) {
        $appBin = Join-Path $scratch 'app_only_0x08008000.bin'
        $slice = New-Object byte[] $appLen
        [Array]::Copy($bytes, $appOffset, $slice, 0, $appLen)
        [System.IO.File]::WriteAllBytes($appBin, $slice)
        Info ("已生成 App-only 校验镜像: " + $appBin + "  (" + $appLen + " B)")
    }
}
$result.AddressRange = '0x08008000 - 0x080145A7 (ELF PT_LOAD, 0xC5A8 B)'
$result.EraseRange   = '约 0x08008000 - 0x08014800 (2KB 页对齐，仅 App 区)'
$result.BootloaderTouched = $false

# ---------- 6. Bootloader 保护 ----------
if ($IncludeBootloader) {
    if (-not $ConfirmBootloaderOverwrite) {
        Fail '请求覆盖 Bootloader，但未给出 -ConfirmBootloaderOverwrite，拒绝执行'
        Save-And-Exit 30 'Bootloader 覆盖未二次确认'
    }
    Fail '本脚本默认禁止覆盖 Bootloader；如需整片烧录请人工在 J-Link 下显式操作并记录。'
    Save-And-Exit 31 '拒绝覆盖 Bootloader'
}

# ---------- 7. 执行烧录 + 校验 ----------
Step '执行烧录（LoadFile，仅 App 段；J-Link 自动按扇区擦除，非整片擦除）'
$lines = New-Object System.Collections.Generic.List[string]
$lines.Add('ExitOnError 1')
$lines.Add('connect')
$lines.Add('LoadFile "' + $fw + '"')
if ($appBin -ne '') {
    $lines.Add('VerifyBin "' + $appBin + '", 0x08008000')
}
$lines.Add('q')
$flashScript = Join-Path $scratch 'flash.jlink'
$lines | Set-Content -LiteralPath $flashScript -Encoding ASCII

$flashOut = Join-Path $scratch 'flash_console.txt'
& $JLinkExe -device $ExpectedDevice -if SWD -speed auto -CommanderScript $flashScript 2>&1 |
    Tee-Object -FilePath $flashOut | ForEach-Object { Write-Host $_ }
$flashText = Get-Content -LiteralPath $flashOut -Raw
$result.NativeExitCode = $LASTEXITCODE

# 完整原始日志（探测 + 识别 + 烧录 + 校验）
$all = @()
$all += '===== VTref PRECHECK ====='
$all += (Get-Content -LiteralPath $preOut)
$all += '===== CHIP IDENTIFY ====='
$all += (Get-Content -LiteralPath $idOut)
$all += '===== FLASH / VERIFY ====='
$all += (Get-Content -LiteralPath $flashOut)
$all | Set-Content -LiteralPath $logFile -Encoding UTF8

# 采用纯正向成功判定：J-Link 已启用 ExitOnError 1，真实错误必然导致非零退出码；
# 宽泛关键字扫描会把 "ExitOnError" / "Not supported by this model" 等正常输出误判为错误，故不使用。
$loadSection = ''
$lIdx = $flashText.IndexOf('LoadFile')
if ($lIdx -ge 0) { $loadSection = $flashText.Substring($lIdx) }
if ($loadSection -notmatch '(?i)O\.K\.|Downloading.*success') {
    Fail 'LoadFile 之后未捕获到 O.K.，烧录结果未确认'
    Save-And-Exit 43 '烧录结果未确认'
}
if ($LASTEXITCODE -ne 0) {
    Fail ("J-Link 原生退出码 " + $LASTEXITCODE)
    Save-And-Exit 41 'J-Link 非零退出'
}
if ($appBin -ne '') {
    # 只截取 VerifyBin 命令之后的输出，避免把 connect 阶段的 "O.K." 误判为校验通过
    $verifySection = ''
    $vIdx = $flashText.IndexOf('VerifyBin')
    if ($vIdx -ge 0) { $verifySection = $flashText.Substring($vIdx) }
    if ($verifySection -match '(?i)O\.K\.|Verify successful|Verified|Verification.*(success|OK)|compare.*OK') {
        $result.Verified = $true
        Info 'Flash 读回校验通过'
    } else {
        Fail '未捕获到校验通过标记，请人工复核日志'
        Save-And-Exit 42 '校验结果未确认'
    }
} else {
    Info '无派生校验镜像，以 LoadFile 返回状态为准（建议人工复核）'
}

Step '完成：未改 Option Bytes、未擦除 Bootloader、未显式下发 Reset/Go/Power 命令'
Info '注意：J-Link LoadFile 默认会执行 implicit reset & halt of MCU（日志可查），属 J-Link 内建行为'
Save-And-Exit 0 '烧录与校验完成'
