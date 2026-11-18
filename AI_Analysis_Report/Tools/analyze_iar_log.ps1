# =====================================================================
#  analyze_iar_log.ps1 - IAR 编译日志自动分析工具
#
#  用法:
#   & "...\AI_Analysis_Report\Tools\analyze_iar_log.ps1" -LogPath "...\Build_Logs\build_Debug_20261113_164840.log"
#   可选: -OutputDir 指定分析结果输出目录（默认与日志同目录）
#
#  功能:
#   自动统计 Error / Warning / Fatal Error 数量、编译失败文件清单、
#   错误代码与行号、链接阶段错误、Flash/RAM 段溢出、未定义符号、
#   重复定义、编译成功/失败判定。
#
#  原则: 只读分析。原始日志原样保留，分析结果写入独立的 .json / .md 文件。
#  支持 UTF-16LE（Tee-Object 默认）与 UTF-8 编码日志。
# =====================================================================
param(
    [Parameter(Mandatory = $true)]
    [string]$LogPath,          # 编译日志完整路径
    [Parameter(Mandatory = $false)]
    [string]$OutputDir = ""    # 分析结果输出目录（默认与日志同目录）
)

$ErrorActionPreference = 'Stop'

# ---------- 0. 参数检查与编码自适应读取 ----------
$LogPath = (Resolve-Path -LiteralPath $LogPath -ErrorAction Stop).Path
if (-not $OutputDir) { $OutputDir = Split-Path $LogPath -Parent }

$bytes = [IO.File]::ReadAllBytes($LogPath)
if ($bytes.Length -ge 2 -and $bytes[0] -eq 0xFF -and $bytes[1] -eq 0xFE) {
    $text = [Text.Encoding]::Unicode.GetString($bytes)          # UTF-16LE BOM
    $enc = 'UTF-16LE'
} elseif ($bytes.Length -ge 3 -and $bytes[0] -eq 0xEF -and $bytes[1] -eq 0xBB -and $bytes[2] -eq 0xBF) {
    $text = [Text.Encoding]::UTF8.GetString($bytes)             # UTF-8 BOM
    $enc = 'UTF-8'
} else {
    try   { $text = [Text.Encoding]::UTF8.GetString($bytes); $enc = 'UTF-8(no BOM)' }
    catch { $text = [Text.Encoding]::Default.GetString($bytes); $enc = 'ANSI' }
}

$timestamp = (Get-Item -LiteralPath $LogPath).LastWriteTime.ToString('yyyyMMdd_HHmmss')
$base      = [IO.Path]::GetFileNameWithoutExtension($LogPath)
$jsonOut   = Join-Path $OutputDir ("analysis_{0}_{1}.json" -f $base, $timestamp)
$mdOut     = Join-Path $OutputDir ("analysis_{0}_{1}.md"   -f $base, $timestamp)

# ---------- 1. 逐条诊断解析 ----------
# IAR 格式: "D:\path\File.c(123) : Error[PeXXX]: 说明"  /  "... : Warning[Pe177]: ..."
# 链接器:   "Error[Lt013]: ..." / "Error[Li001]: ..."
$diagRegex = '^\s*(?<file>[^(]+)\((?<line>\d+)\)\s*:\s*(?<level>Error|Warning)\[(?<code>\w+\d+)\]\s*:\s*(?<msg>.*)$'
$diagList  = @()
foreach ($line in ($text -split "`r?`n")) {
    $m = [regex]::Match($line, $diagRegex)
    if ($m.Success) {
        $diagList += [pscustomobject]@{
            File    = $m.Groups['file'].Value.Trim()
            Line    = [int]$m.Groups['line'].Value
            Level   = $m.Groups['level'].Value
            Code    = $m.Groups['code'].Value
            Message = $m.Groups['msg'].Value.Trim()
        }
    }
}

$errorList   = @($diagList | Where-Object { $_.Level -eq 'Error' })
$warningList = @($diagList | Where-Object { $_.Level -eq 'Warning' })

# Fatal Error（编译器/链接器致命错误）
$fatalList = @()
foreach ($line in ($text -split "`r?`n")) {
    if ($line -match '(?i)Fatal\s+Error\[(?<code>\w+\d+)\]\s*:\s*(?<msg>.*)') {
        $fatalList += [pscustomobject]@{ Code = $Matches['code']; Message = $Matches['msg'].Trim() }
    }
    elseif ($line -match '(?i)fatal error(?!\[)') {
        $fatalList += [pscustomobject]@{ Code = '(none)'; Message = $line.Trim() }
    }
}

# ---------- 2. 编译失败文件清单（出现 Error 的源文件去重） ----------
$failedFiles = @($errorList | Group-Object File | ForEach-Object {
    [pscustomobject]@{ File = $_.Name; ErrorCount = $_.Count }
})

# ---------- 3. 链接阶段错误（ilinkarm / Lt / Li 错误码 / "Error while linking"） ----------
$linkErrors = @()
foreach ($line in ($text -split "`r?`n")) {
    if ($line -match 'Error\[(?<code>(Lt|Li)\d+)\]\s*:\s*(?<msg>.*)') {
        $linkErrors += [pscustomobject]@{ Code = $Matches['code']; Message = $Matches['msg'].Trim() }
    }
}
if ($text -match '(?im)^\s*Error while running Linker.*$|(?im)Linking\s*$.*?Error') { }
$hasLinkStage = ($text -match '(?m)^Linking\s*$')
$linkDiag = @($diagList | Where-Object { $_.Code -match '^(Lt|Li)' })

# ---------- 4. 未定义符号 ----------
$undefinedSymbols = @()
foreach ($line in ($text -split "`r?`n")) {
    if ($line -match 'undefined symbol|unresolved external') {
        $undefinedSymbols += $line.Trim()
    }
}
$undefinedSymbols = @($undefinedSymbols | Select-Object -Unique)

# ---------- 5. 重复定义 ----------
$duplicateDefs = @()
foreach ($line in ($text -split "`r?`n")) {
    if ($line -match 'multiply defined|already defined|duplicate symbol|redefinition[^(]*Error') {
        $duplicateDefs += $line.Trim()
    }
}
$duplicateDefs = @($duplicateDefs | Select-Object -Unique)

# ---------- 6. Flash/RAM 段溢出 ----------
# 仅匹配真实溢出/放置失败关键字; 正常的 "bytes of ... memory" 摘要行归入 MemorySummary, 不算溢出
$overflow = @()
foreach ($line in ($text -split "`r?`n")) {
    if ($line -match '(?i)too (long|large)|overflow|does not fit|no range|out of (memory|range)|placed?(ment)? (error|fail)|section.*failed') {
        $overflow += $line.Trim()
    }
}
# 段占用摘要（链接成功的尺寸输出行）
$memLines = @(($text -split "`r?`n") | Where-Object { $_ -match 'bytes of (readonly|readwrite)' } | ForEach-Object { $_.Trim() } | Select-Object -Unique)

# ---------- 7. 成功/失败判定 ----------
$phaseCompile = ($errorList.Count -gt 0)
$symbolSummary = [regex]::Matches($text, '(?m)^\s*(?<n>\d[\d\s]*)\s*bytes of readonly\s+code memory').Count
$buildSuccess = ($errorList.Count -eq 0) -and ($fatalList.Count -eq 0) -and ($linkErrors.Count -eq 0) -and ($undefinedSymbols.Count -eq 0) -and ($memLines.Count -gt 0)

# ---------- 8. 输出 JSON ----------
$report = [ordered]@{
    LogPath          = $LogPath
    LogEncoding      = $enc
    AnalyzedAt       = (Get-Date).ToString('yyyy-MM-dd HH:mm:ss')
    Summary = [ordered]@{
        BuildSuccess      = $buildSuccess
        ErrorCount        = $errorList.Count
        WarningCount      = $warningList.Count
        FatalErrorCount   = $fatalList.Count
        FailedFileCount   = $failedFiles.Count
        LinkErrorCount    = ($linkErrors.Count + $linkDiag.Count)
        UndefinedSymbols  = $undefinedSymbols.Count
        DuplicateDefs     = $duplicateDefs.Count
        OverflowHints     = $overflow.Count
        HasMemorySummary  = ($memLines.Count -gt 0)
    }
    Errors           = $errorList
    Warnings         = $warningList
    FatalErrors      = $fatalList
    FailedFiles      = $failedFiles
    LinkErrors       = $linkErrors
    LinkStageErrors  = $linkDiag
    UndefinedSymbols = $undefinedSymbols
    DuplicateDefs    = $duplicateDefs
    OverflowHints    = $overflow
    MemorySummary    = $memLines
    ErrorCodes       = @($errorList | Group-Object Code | ForEach-Object { [pscustomobject]@{ Code = $_.Name; Count = $_.Count; Sample = $_.Group[0].Message } })
}
$report | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath $jsonOut -Encoding UTF8

# ---------- 9. 输出 Markdown（可读版） ----------
$md = @()
$md += "# IAR 日志分析报告"
$md += ""
$md += ("- 日志: " + $LogPath)
$md += ("- 编码: " + $enc)
$md += ("- 分析时间: " + $report.AnalyzedAt)
$md += ""
$md += "| 指标 | 数值 |"
$md += "|---|---|"
$md += ("| 编译成功 | {0} |" -f $(if ($buildSuccess) {'是'} else {'否'}))
$md += "| Error | $($errorList.Count) |"
$md += "| Warning | $($warningList.Count) |"
$md += "| Fatal Error | $($fatalList.Count) |"
$md += "| 编译失败文件 | $($failedFiles.Count) |"
$md += "| 链接阶段错误 | $($linkErrors.Count + $linkDiag.Count) |"
$md += "| 未定义符号 | $($undefinedSymbols.Count) |"
$md += "| 重复定义 | $($duplicateDefs.Count) |"
$md += "| 溢出提示 | $($overflow.Count) |"
$md += ""
if ($errorList.Count -gt 0) {
    $md += "## 错误明细"
    $md += ""
    $md += "| 文件 | 行号 | 错误码 | 说明 |"
    $md += "|---|---|---|---|"
    foreach ($e in $errorList) { $md += ("| {0} | {1} | {2} | {3} |" -f $e.File, $e.Line, $e.Code, $e.Message) }
    $md += ""
}
if ($fatalList.Count -gt 0) {
    $md += "## Fatal Error"
    $md += ""
    foreach ($f in $fatalList) { $md += ("- [{0}] {1}" -f $f.Code, $f.Message) }
    $md += ""
}
if ($undefinedSymbols.Count -gt 0) {
    $md += "## 未定义符号"; $md += ""
    $md += ($undefinedSymbols | ForEach-Object { "- $_" }); $md += ""
}
if ($duplicateDefs.Count -gt 0) {
    $md += "## 重复定义"; $md += ""
    $md += ($duplicateDefs | ForEach-Object { "- $_" }); $md += ""
}
if ($memLines.Count -gt 0) {
    $md += "## 存储占用摘要（链接器输出）"; $md += ""
    $md += ($memLines | ForEach-Object { "- $_" }); $md += ""
}
$md += ""
$md += ("> 原始日志已完整保留: " + $LogPath + " （本文件仅为过滤视图）")
$md | Set-Content -LiteralPath $mdOut -Encoding UTF8

# ---------- 10. 控制台摘要 ----------
Write-Host "==============================================================="
Write-Host " IAR 日志分析完成"
Write-Host "==============================================================="
Write-Host (" 编译判定    : {0}" -f $(if ($buildSuccess) {'SUCCESS'} else {'FAILED'}))
Write-Host (" Error       : {0}" -f $errorList.Count)
Write-Host (" Warning     : {0}" -f $warningList.Count)
Write-Host (" Fatal Error : {0}" -f $fatalList.Count)
Write-Host (" 失败文件    : {0}" -f $failedFiles.Count)
Write-Host (" 链接错误    : {0}" -f ($linkErrors.Count + $linkDiag.Count))
Write-Host (" 未定义符号  : {0}" -f $undefinedSymbols.Count)
Write-Host (" 重复定义    : {0}" -f $duplicateDefs.Count)
Write-Host (" 溢出提示    : {0}" -f $overflow.Count)
Write-Host (" JSON 报告   : $jsonOut")
Write-Host (" MD 报告     : $mdOut")
Write-Host (" 原始日志保留: $LogPath")
Write-Host "==============================================================="

# 退出码: 分析到错误返回 1（供自动修复流程判断），否则 0
if ($buildSuccess) { exit 0 } else { exit 1 }
