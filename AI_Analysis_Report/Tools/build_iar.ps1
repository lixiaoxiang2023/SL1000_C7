# =====================================================================
#  build_iar.ps1 - IAR 命令行自动编译脚本（第二阶段完善版）
#
#  用法示例（路径含空格已支持，全部使用绝对路径）:
#   & "D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Tools\build_iar.ps1" `
#     -IarBuildPath  "C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.2\common\bin\IarBuild.exe" `
#     -ProjectPath   "D:\workspace\SL-1000\CODE\DevBranche\C7\proj_source\C7Detect.ewp" `
#     -Configuration "Debug" `
#     -LogDir        "D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Build_Logs"
#
#  行为约定:
#   - 只使用 "-build"，绝不使用 "-clean"，不删除任何中间文件与固件
#   - Tee-Object 同时输出到控制台与时间戳日志，保留完整原始输出（含命令行回显）
#   - 原生 IarBuild 退出码与脚本退出码分开记录
#   - 脚本退出码: 0 = 编译成功; 非 0 = 编译失败（含参数错误）
#   - 已知问题: IarBuild V7.2.4.3784 在 post-build 输出阶段会以 0xC0000409 异常退出，
#     但 build_post.bat 实际执行完毕、固件正常产出，故以"错误数 + 产物新鲜度"判定成功
#   - 不修改任何 IAR 工程设置和源代码
# =====================================================================
param(
    [Parameter(Mandatory = $true)]
    [string]$IarBuildPath,      # IarBuild.exe 完整路径（绝对路径）
    [Parameter(Mandatory = $true)]
    [string]$ProjectPath,       # .ewp 工程文件完整路径（绝对路径）
    [Parameter(Mandatory = $true)]
    [string]$Configuration,     # 编译配置名称，例如 Debug
    [Parameter(Mandatory = $true)]
    [string]$LogDir             # 日志输出目录（绝对路径）
)

$ErrorActionPreference = 'Continue'

# IarBuild 7.2.4 已知崩溃码 (STATUS_STACK_BUFFER_OVERRUN)
$KNOWN_QUIRK_EXITCODE = -1073740791   # 0xC0000409

function Write-Log-Line {
    param([string]$Text, [string]$Path)
    Write-Host $Text
    # 与 Tee-Object 保持一致的 Unicode(UTF-16LE) 编码, 避免同一文件混合编码
    Out-File -FilePath $Path -Append -Encoding unicode -InputObject $Text
}

# ---------- 1. 路径规范化（绝对路径 + 空格安全） ----------
$startTime = Get-Date
$timestamp = $startTime.ToString('yyyyMMdd_HHmmss')

try {
    $IarBuildPath = (Resolve-Path -LiteralPath $IarBuildPath -ErrorAction Stop).Path
    $ProjectPath  = (Resolve-Path -LiteralPath $ProjectPath  -ErrorAction Stop).Path
} catch {
    Write-Host "[ERROR] 参数路径不存在: $($_.Exception.Message)"
    exit -1
}
if (-not (Test-Path -LiteralPath $LogDir)) {
    New-Item -ItemType Directory -Path $LogDir -Force | Out-Null
}
$LogDir = (Resolve-Path -LiteralPath $LogDir).Path

# ---------- 2. 配置名预校验（防止写错配置名导致空跑） ----------
$ewpText = Get-Content -LiteralPath $ProjectPath -Raw -Encoding Default
if ($ewpText -notmatch ('<configuration>\s*<name>' + [regex]::Escape($Configuration) + '</name>')) {
    Write-Host "[ERROR] 配置 '$Configuration' 在工程中不存在。工程内可用配置:"
    ([regex]::Matches($ewpText, '<configuration>\s*<name>([^<]+)</name>') | ForEach-Object { "  - " + $_.Groups[1].Value })
    exit -2
}

# ---------- 3. 日志文件（时间戳命名） ----------
$productName = [IO.Path]::GetFileNameWithoutExtension($ProjectPath)
$logFile     = Join-Path $LogDir ("build_{0}_{1}.log"   -f $Configuration, $timestamp)
$summaryFile = Join-Path $LogDir ("build_summary_{0}_{1}.json" -f $Configuration, $timestamp)
Remove-Item -LiteralPath $logFile -ErrorAction SilentlyContinue   # 确保从零开始（仅本次时间戳日志自身）

Write-Log-Line "===============================================================" $logFile
Write-Log-Line ("IarBuild     : {0}" -f $IarBuildPath) $logFile
Write-Log-Line ("Project      : {0}" -f $ProjectPath) $logFile
Write-Log-Line ("Configuration: {0}" -f $Configuration) $logFile
Write-Log-Line ("Start        : {0}" -f $startTime.ToString('yyyy-MM-dd HH:mm:ss')) $logFile
Write-Log-Line ("Command      : IarBuild.exe `"$ProjectPath`" -build `"$Configuration`" -log all") $logFile
Write-Log-Line "===============================================================" $logFile

# ---------- 4. 执行编译：完整原始输出同时显示与落盘 ----------
& $IarBuildPath "$ProjectPath" -build "$Configuration" -log all 2>&1 |
    Tee-Object -FilePath $logFile -Append
$nativeExitCode = $LASTEXITCODE     # 原生 IarBuild 进程退出码（如实记录，不加工）

# ---------- 5. 结束时间与产物校验 ----------
$endTime  = Get-Date
$duration = $endTime - $startTime

$productDir = Join-Path (Split-Path $ProjectPath -Parent) "Debug\Exe"
$productBin = Join-Path $productDir ("{0}.bin" -f $productName)
$productOut = Join-Path $productDir ("{0}.out" -f $productName)
$productFreshBin = $false
$productFreshOut = $false
if (Test-Path -LiteralPath $productBin) {
    if ((Get-Item -LiteralPath $productBin).LastWriteTime -ge $startTime.AddSeconds(-5)) { $productFreshBin = $true }
}
if (Test-Path -LiteralPath $productOut) {
    if ((Get-Item -LiteralPath $productOut).LastWriteTime -ge $startTime.AddSeconds(-5)) { $productFreshOut = $true }
}
$productSize = if (Test-Path -LiteralPath $productBin) { (Get-Item -LiteralPath $productBin).Length } else { 0 }

# ---------- 6. 诊断统计（逐条格式，避免与汇总行重复计数） ----------
$logText = Get-Content -LiteralPath $logFile -Raw
$errCount  = ([regex]::Matches($logText, '(?m)^\s*Error\[\w+\d+\]')).Count
$warnCount = ([regex]::Matches($logText, '(?m)^\s*Warning\[\w+\d+\]')).Count
$fatalCount = ([regex]::Matches($logText, '(?i)Fatal Error')).Count

$isQuirkExit = ($nativeExitCode -eq $KNOWN_QUIRK_EXITCODE)
$buildSuccess = ($errCount -eq 0) -and ($fatalCount -eq 0) -and $productFreshBin -and $productFreshOut

Write-Log-Line "===============================================================" $logFile
Write-Log-Line ("End          : {0}" -f $endTime.ToString('yyyy-MM-dd HH:mm:ss')) $logFile
Write-Log-Line ("Duration     : {0}" -f $duration.ToString()) $logFile
Write-Log-Line ("NativeExitCode: {0}  (0xC0000409 quirk: {1})" -f $nativeExitCode, $isQuirkExit) $logFile
Write-Log-Line ("Errors: {0}  Warnings: {1}  Fatal: {2}" -f $errCount, $warnCount, $fatalCount) $logFile
Write-Log-Line ("ProductBin   : {0}  fresh={1}  size={2}" -f $productBin, $productFreshBin, $productSize) $logFile
Write-Log-Line ("BuildVerdict : {0}" -f $(if ($buildSuccess) {'SUCCESS'} else {'FAILED'})) $logFile
Write-Log-Line "===============================================================" $logFile

# ---------- 7. 摘要 JSON ----------
$summary = [ordered]@{
    IarBuildPath    = $IarBuildPath
    ProjectPath     = $ProjectPath
    Configuration   = $Configuration
    StartTime       = $startTime.ToString('yyyy-MM-dd HH:mm:ss')
    EndTime         = $endTime.ToString('yyyy-MM-dd HH:mm:ss')
    DurationSec     = [math]::Round($duration.TotalSeconds, 1)
    NativeExitCode  = $nativeExitCode
    KnownQuirkHit   = $isQuirkExit
    Errors          = $errCount
    Warnings        = $warnCount
    FatalErrors     = $fatalCount
    ProductBin      = $productBin
    ProductFreshBin = $productFreshBin
    ProductFreshOut = $productFreshOut
    ProductSize     = $productSize
    BuildVerdict    = $(if ($buildSuccess) {'SUCCESS'} else {'FAILED'})
    LogPath         = $logFile
}
$summary | ConvertTo-Json | Set-Content -LiteralPath $summaryFile -Encoding UTF8

Write-Host "[RESULT] BuildVerdict=$(if ($buildSuccess) {'SUCCESS'} else {'FAILED'})  NativeExitCode=$nativeExitCode  Errors=$errCount  Warnings=$warnCount  ProductFresh=$productFreshBin"
Write-Host "[RESULT] 日志: $logFile"
Write-Host "[RESULT] 摘要: $summaryFile"

# ---------- 8. 脚本退出码：成功=0，失败=非 0（要求 7） ----------
if ($buildSuccess) { exit 0 }
elseif ($nativeExitCode -ne 0) { exit $nativeExitCode }
else { exit 3 }   # 原生退出码为 0 但判定失败（如产物未生成），用 3 区分
