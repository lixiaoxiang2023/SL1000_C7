# 自动修复记录（Auto Repair History）

本文件按轮次记录每次自动修复的关键信息，追加式写入，不覆写历史记录。

## 执行规则（每轮适用）

1. 每轮只修改 1 个目标，修改前先备份涉及文件到 `AI_Analysis_Report/Backups/<轮次>/`；
2. 修改后展示差异，再调用 `build_iar.ps1` 完整编译；
3. 编译失败 → 调用 `analyze_iar_log.ps1` → 定位根因 → 最小化修复 → 再次构建验证；
4. **最多 5 轮**；成功后立即停止，不做无意义修改；
5. 触发人工介入的停止条件：
   - 连续 2 轮无改善；
   - 需要修改受保护配置（`.ewp` / `.ewd` / `.icf` / `startup_stm32f10x_hd.s` / `FreeRTOSConfig.h` / 编译器版本 / 芯片型号）；
   - IAR License 或工具链本身故障；
6. 红线：不 IAR Clean、不 git/svn reset/clean、不删工程文件、不烧录、不覆盖用户未提交修改。

---

## Round 0 — 基线（2026-11-13）

| 项目 | 内容 |
|---|---|
| 轮次 | Round 0（基线，无代码修改） |
| 编译错误数量 | 0 |
| 警告数量 | 7（完整编译时）/ 0（增量编译时） |
| 错误原因 | 无错误 |
| 修改文件 | 无 |
| 修改内容 | 无 |
| 编译结果 | SUCCESS，`C7Detect.bin` 83 368 B，49 182 B ro code / 1 398 B ro data / 28 341 B rw data |
| 剩余问题 | ① 7 条警告（MotorCtrl.c:1186/1187/1377、ver_def.h:35、TaskMove.c:57/58/64）；② 前期分析报告中的 30 条缺陷（P-01~P-30）待按《后续自动修复流程说明.md》批次处理 |

基线日志：
- `AI_Analysis_Report/Build_Logs/build_Debug_20261113_164840.log`（完整编译，7 警告）
- `AI_Analysis_Report/Build_Logs/build_Debug_20261113_172101.log`（完善版脚本验证，0 警告/增量）

基线判定方式：`日志无 Error[...]` + `C7Detect.bin 时间戳新鲜` 双重标准
（IarBuild 7.2.4 原生退出码 0xC0000409 为已知 quirk，不作失败依据）。

---

## Round 1 — 温度链路程序隐患修复 H-01/H-02/H-04/H-05/H-06（2026-11-14）

| 项目 | 内容 |
|---|---|
| 轮次 | Round 1 |
| 触发 | 用户要求优化《烘干90度隐患_实测与全路径排查报告.md》中的 H-01/H-02/H-04/H-05/H-06 |
| 编译错误数量 | **0** |
| 警告数量 | **0**（完整编译） |
| Fatal | **0** |
| 编译结果 | **SUCCESS**，`C7Detect.bin` 83 580 B（基线 83 368，+212 B） |
| 判定方式 | 日志 `Errors: none / Warnings: none` + bin 时间戳新鲜（NativeExitCode 0xC0000409 仍为已知 quirk） |
| 备份 | `AI_Analysis_Report/backup/patch_20261114/`（4 个原件 + SHA256_before.txt + ai_fix.diff） |

### 修改文件与内容

| 文件 | 改动量 | 缺陷 | 修改内容 |
|---|---|---|---|
| `proj_source/platcode/devlib/plat/pwm.c` | +29 / −4 | H-01 | `Write()` 遇 `bySpd==0` 由 `return -1`（空操作）改为**等价于 `Stop()` 的真关断**；`blUsing` 检查前置 |
| `proj_source/platcode/devlib/plat/pwm.c` | 同上 | H-02 | 删除 `Write()` 中的 `bySpdCnt = 0`（保留相位）；`KernelProg()` 比较由 `== bySpd` 改 **`>= bySpd`** |
| `proj_source/platcode/devlib/core/adc.c` | +44 / −8 | H-05 | 新增 `ADC_WAIT_MAX_LOOP 50000`；四处 `while(...){;}` 裸死等改为**有界等待，超时关 ADON 并返回 0**；补 `dwDoTims==0` 除零保护 |
| `code/dev/source/tempfan.c` | +58 / −12 | H-04 | `ReadNtcTem`/`ReadNtcFan` 两处的 `bADCbusy` 判定+置位改入 **`taskENTER_CRITICAL` 临界区**；取锁失败不再强抢，**返回哨兵 3000** |
| `code/app/source/TaskTempModule.c` | +12 / −2 | H-06 | 两个温控任务优先级 `tskIDLE_PRIORITY`(0) → **2** |

### 资源占用

| 项 | Round 0 | Round 1 | 变化 |
|---|---|---|---|
| readonly code | 49 182 B | 49 394 B | +212 B |
| readonly data | 1 398 B | 1 398 B | 0 |
| readwrite data | 28 341 B | 28 341 B | **0** |
| `C7Detect.bin` | 83 368 B | 83 580 B | +212 B |

RAM 零增长 —— 补丁未引入任何静态变量或缓冲区。

### 关键分析结论（修复依据）

- **H-01 的真实入口只有一个**：全工程 `Write(...,0,...)` 共 5 处，
  只有 `TaskTempModule.c:173`（`wPwm*2` 变 0）**没有 `Stop()` 兜底**，
  其余 4 处（`:179/:184/:412/:416/:429`）后面紧跟 `Stop()`，幂等无害。
- 固化通道软限幅为 `wHeatTemp+50` = **118℃**，92℃ 分支被 `else if` 遮蔽不可达 —— 
  所以 H-01 的锁死后果是"1~2% 占空比持续加热到 118℃"。
- **H-02 的 `>=` 改动是必要配套**：仅删除 `bySpdCnt=0` 而不改比较符，
  当新 `bySpd` 小于当前相位时会出现"整周期 ON"，方向错误。
- **H-05 超时值与硬件时序的余量**：一次转换 ≈18 µs、校准 ≈6 µs；
  50 000 次空转 ≈5~7 ms，余量 >250 倍，正常运行永不触发。
- **返回 0 的安全性**：`tempfan.c` 中 `wAdcMid <= OPEN_VALUE(10)` 会判开路，
  转 3000 哨兵触发停机，不会出现"读 0 就按 0℃ 满功率"。

### 未做的事（明确边界）

- **未烧录、未在线验证**（需用户授权）
- 未改任何控制参数：`Kp=20`、`Ki=0`、`Tset+50`、92℃ 分支、`wPwm` 限幅公式
- 未动 H-03 / H-07 / H-08 / H-09 / H-10（理由见报告 §6.8）

### 版本库状态

```
M  code\app\source\TaskTempModule.c          ← 原有用户修改 + 本轮 H-06
M  code\dev\source\tempfan.c                 ← 本轮 H-04 新增
M  proj_source\version_json\version-sl1000-c7.json   ← 原有用户修改，未触碰
?  proj_source\platcode                       ← pwm.c/adc.c 所在目录本就未纳管
```

用户原有 2 个修改完整保留。`TaskTempModule.c` 为叠加态，可用 `ai_fix.diff` 剥离。

### 回滚（四条，逐条对应，勿合并）

在工程根目录 `D:/workspace/SL-1000/CODE/DevBranche/C7` 下执行：

```
cp AI_Analysis_Report/backup/patch_20261114/pwm.c             proj_source/platcode/devlib/plat/pwm.c
cp AI_Analysis_Report/backup/patch_20261114/adc.c             proj_source/platcode/devlib/core/adc.c
cp AI_Analysis_Report/backup/patch_20261114/tempfan.c         code/dev/source/tempfan.c
cp AI_Analysis_Report/backup/patch_20261114/TaskTempModule.c  code/app/source/TaskTempModule.c
```

四个原件均为改前版本（GBK 编码 + CRLF 换行，覆盖后编码不变）。
回滚后用 `SHA256_before.txt` 校验即知是否还原干净。

### 遗留问题

- ① 真机验证三项：H-01 看 `PWM0.blRunning` 是否真归零；H-02 用示波器量 PB14/PB15 实际占空比；H-06 看 `pxCurrentTCB` 是否出现温控任务
- ② H-06 优先级若影响 xcan 通信，回退为 1 即可
- ③ 报告 §6 中 H-03/H-07~H-10 仍未处理
