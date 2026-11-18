# 首次烧录与 SWD 数据监控结果报告

- 执行时间：2026-11-14 09:10 ~ 09:18
- 触发指令：用户「连接上了目标板，帮我烧写程序，并用SWD监控单片机数据」
- 结论：**烧录成功、Flash 读回校验通过、Bootloader 区逐字节未变、SWD 非侵入式监控已建立并捕获到目标运行中的动态数据**。

---

## 一、硬件复测与型号交叉核对

| 项 | 上一阶段（未接板） | 本次实测 |
|---|---|---|
| VTref | 0.000 V | **3.325 / 3.327 / 3.317 / 3.332 / 3.293 / 3.324 / 3.333 V**（多次读数稳定） |
| ITarget | 0 mA | 0 mA（J-Link 不供电，KS-Power Off） |
| SWD | 不具备条件 | `Found SW-DP with ID 0x1BA01477`，AHB-AP IDR `0x24770011`，连接成功 |
| 内核 | 未识别 | `CPUID 0x412FC231` → **Cortex-M3 r2p1, Little endian**，无 FPU |
| 器件 | 未识别 | J-Link 把工程型号归一化为 **`STM32F103RC`** |
| DBGMCU_IDCODE | 未读 | `0xE0042000 = 0x13090414`，低 12 位 **0x414 = High Density** |
| Flash 容量 | 未读 | `0x1FFFF7E0 = 0x00300100`，低 16 位 **0x0100 = 256 KB** |
| CPU 状态 | 未连接 | `CPU is not halted`（烧录后在运行） |

**交叉核对结论：一致**。工程侧依据 `C7Detect.ewp` `OGChipSelectEditMenu=STM32F103xC`、`C7Detect.ewd` `MemFile=...\ST\STM32F103xC.ddf`、`startup_stm32f10x_hd.s`、`STM32F10X_HD`、2 KB Flash 页；硬件侧实测 RC / High Density / 256 KB / Cortex-M3，全部吻合。未切换到其他型号尝试连接。

---

## 二、首次烧录单（执行前展示的 6 项）

| 项目 | 内容 |
|---|---|
| 实际识别芯片 | STM32F103RC / Cortex-M3 r2p1 / DEV_ID 0x414 / 256 KB Flash |
| 工程配置芯片 | STM32F103xC（High Density），**一致** |
| 拟烧录固件 | `proj_source\Debug\Exe\C7Detect.out`（IAR ELF，714 916 B，自带段地址） |
| 固件地址范围 | ELF 3 个 `PT_LOAD` 段：`0x08008000–0x0800812F`、`0x08008130–0x08008143`、`0x08008144–0x080145A7`，合计 **0xC5A8 = 50 600 B** |
| 预计擦除范围 | J-Link 按段自动擦除 App 区 2 KB 页（实测日志 `Bank 0 @ 0x08000000: 4 ranges affected (40960 bytes)`）；不含 `0x08000000–0x08007FFF`，不含参数区 `0x08038000` |
| 是否影响 Bootloader | **否**。选 ELF 而非 `C7Detect.bin`（合并镜像），正是为了避开上一阶段报告指出的 Bootloader 红线 |

> 更正：上一阶段报告中曾把 `0xC5A8` 换算为 50 568 B，正确值是 **50 600 B**（`0xC5A8 = 49152+1280+160+8`）。

---

## 三、烧录执行记录

工具：`AI_Analysis_Report\Tools\flash_jlink.ps1`（本次新建，UTF-8 BOM，语法 0 错误）

门禁全部通过：
- 门禁 A：`ShowHWStatus` VTref > 1.0 V
- 门禁 B：`connect` + `Mem32 0xE0042000` + `Mem32 0x1FFFF7E0`，DEV_ID=0x414 / 256 KB / Cortex-M3
- 器件固定 `STM32F103xC`，SWD，`speed auto`，`ExitOnError 1`
- 命令白名单只有 `LoadFile` + `VerifyBin`；**未生成** `Erase` / `Power` / `Reset` / `Go` / Option Bytes / 保护配置命令

| 轮次 | 时间戳 | J-Link 结果 | 脚本退出码 |
|---|---|---|---|
| 1 | 09:10:25 | `4 ranges affected (40960 bytes)`，Program & Verify 2.053 s，`O.K.`；`Verify successful.` | 40（脚本误判，见下） |
| 2 | 09:11:22 | `Skipped. Contents already match`；`Verify successful.` | 40（脚本误判，见下） |
| 3 | 09:11:48 | `Skipped. Contents already match`；`Verify successful.` | **0 ✅** |

三次 J-Link 实际均成功；后两次因 Flash 内容已一致被跳过写入。脚本前两次退出码 40 是**判定逻辑误报**（把 `ExitOnError` 里的 "Error"、`Not supported by this model` 当成错误关键字），已改为纯正向成功判定（原生退出码 0 + `LoadFile` 段出现 `O.K.` + `VerifyBin` 段出现 `Verify successful`），第三次验证 exit 0。

校验镜像：从 `C7Detect.bin` 偏移 `0x8000` 精确截取 `0xC5A8` 字节生成 `app_only_0x08008000.bin`（只读派生，未改工程文件），用 `VerifyBin ..., 0x08008000` 读回 50 600 B 比对，结果 `Verify successful.`

### ⚠️ 如实说明：J-Link 的隐式复位

日志明确写有：
```
'loadfile': Performing implicit reset & halt of MCU.
Reset: Halt core after reset via DEMCR.VC_CORERESET.
Reset: Reset device via AIRCR.SYSRESETREQ.
```
这是 **J-Link `LoadFile` 的内建行为**，不是脚本下发的 `Reset` 命令。即本次烧录过程中目标 MCU 确实被复位过并短暂 halt，下载完成后恢复运行（后续探测 `CPU is not halted`、SysTick 在跑）。脚本末尾原先写的"未复位"措辞已修正。

---

## 四、Bootloader 完整性验证（事后只读）

用 `SaveBin ..., 0x08000000, 0x8000` 只读回读 Bootloader 区 32 768 B，与 `C7Detect.bin` 前 32 768 B 比对：

| 项 | 值 |
|---|---|
| 合并镜像 Boot 区 SHA256(前20) | `7139b8e9e2377adb37d2` |
| 目标板读回 Boot 区 SHA256(前20) | `7139b8e9e2377adb37d2` |
| 差异字节数 | **0** |
| 读回区非 `0xFF` 字节 | 32 462（未被擦空） |
| 前 16 字节 | `88180020113600086f3400086f340008`（镜像与读回相同，栈指针 `0x20001888`、复位向量 `0x08001136`） |

**结论：Bootloader 区逐字节完全一致，本次烧录未触碰、未破坏 Bootloader。**

---

## 五、SWD 数据监控

工具：`AI_Analysis_Report\Tools\swd_monitor.ps1`（本次新建）

- 模式：**非侵入式只读**。生成的 J-Link 命令只有 `connect` / `Mem32` / `Sleep` / `IsHalted` / `q`，不含 halt / reset / go / 任何写命令，因此不改变 CPU 运行状态、不写 Flash 或 RAM。
- 监视表 18 项，地址全部来自 `proj_source\Debug\List\C7Detect.map` 实测符号；支持 `-WatchList "名称=0x地址=字数;..."` 自定义。
- 可配置 `-Samples`（1..200）、`-IntervalMs`（0..60000）；输出原始日志 + 逐值 CSV + Markdown 摘要。

### 最终有效结果（`swd_monitor_20261114_091739.*`）

| 变量 | 地址 | 第 1 轮 | 第 5 轮 | 变化 |
|---|---|---|---|---|
| `pxCurrentTCB` | `0x20006A54` | `0x20002028` | `0x20001DB8` | **是**（任务切换） |
| `xTickCount` | `0x20006A68` | `0x000552BC` | `0x00055AB2` | **是**（+2038 / 2 s ≈ 1 kHz） |
| `SysTick_CVR` | `0xE000E018` | `0x00005DF6` | `0x00008F0A` | **是** |
| `xSchedulerRunning` | `0x20006A70` | `1` | `1` | 否（调度器已启动） |
| `uxCurrentNumberOfTasks` | `0x20006A64` | `12` | `12` | 否 |
| `firmware_update_flag` | `0x20000000` | `0x60A5A38B` | 同 | 否 |
| `CtrlCmd[0]` | `0x20000004` | `0x000000ED` | 同 | 否 |
| `g_stC7[0]` | `0x20005770` | `0x00000116` | 同 | 否 |
| `stPiPid[0]` / `stPiPidFan[0]` | `0x200068FC` / `0x20006928` | `0x43C80000`（400.0f） | 同 | 否 |
| `s_stTempDat` / `s_stTempDatFan` | `0x20006A04` / `0x20006A10` | 全 0 | 同 | 否 |
| `g_Queue_SubTask` | `0x20006970` | 5 个队列句柄 `0x20000B30..0x20000CB0` | 同 | 否 |
| `HX711_Buffer` / `Weight_Maopi` | `0x20006AA0` / `0x20006AA4` | 0 | 同 | 否 |

**解读**：tick 以 1000 Hz 推进（`configTICK_RATE_HZ=1000` 吻合）、调度器运行、12 个任务、`pxCurrentTCB` 在两个 TCB 间切换 → **目标在正常运行 FreeRTOS**。业务变量稳定是因为当前没有上位机 CAN 命令输入，属正常空闲态，不是卡死。

补充只读探测：
- `SysTick CTRL = 0x00010007`（使能 + tick 中断），CVR 三次 `0xCBB5 → 0xF95C → 0xA21E` 持续变化
- `ICSR = 0x00000000`（采样瞬间无活动异常）
- 从当前 TCB `0x20001DB8` 读回 96 B，在偏移 `0x34` 解析出 `pcTaskName = "SendDataTask"` → 某次采样时正在运行数据上报任务；`xStateListItem.pvContainer = 0x20006768` 即 `pxReadyTasksLists`
- `Regs` 在 CPU 运行时不可读（J-Link 返回 `CPU is not halted !`），符合非侵入式约束，未强行 halt

### ⚠️ 两次解析缺陷与作废文件（主动披露）

前两份监控报告的数值**不可信，已作废**，原因是工具自身的解析 bug，不是硬件问题：

1. `swd_monitor_20261114_091310.*` —— J-Link 对 ≥5 words 的 `Mem32` 会折行，且续行**再次打印地址**（`20006970 = A B C D` 后又 `20006980 = E`）；旧解析器按结果行顺序对应命令，一条命令被算成两条，后续全部串位（`pxCurrentTCB` 被写成 `0x2`）。
   **修正**：改为按 `J-Link>Mem32 ...` 命令回显切段，段内逐行合并。
2. `swd_monitor_20261114_091645.*` —— 命令对齐已修好（`Words` 列全对），但单元素数组被 PowerShell 解包成标量字符串，`$vals[0]` 取到首字符（`60A5A38B → 0x6`、`20001DB8 → 0x2`）。
   **修正**：`@()` 包住整个管道强制数组。
3. `flash_20261114_091025.*` / `flash_20261114_091122.*` —— 烧录本身成功，但脚本失败判定误报（详见第三节），退出码 40。

以上作废文件**按要求全部保留**，未删除；有效结果以 `flash_20261114_091148.*` 和 `swd_monitor_20261114_091739.*` 为准。

---

## 六、本次新增文件与变更留痕

| 文件 | 说明 |
|---|---|
| `AI_Analysis_Report\Tools\flash_jlink.ps1` | 新建，安全烧录（6 道门禁 + 完整日志 + 非零退出码） |
| `AI_Analysis_Report\Tools\swd_monitor.ps1` | 新建，SWD 非侵入式只读轮询监控 |
| `AI_Analysis_Report\环境检测报告.md` | 上一阶段产物，未修改 |
| `AI_Analysis_Report\烧录与SWD监控结果报告.md` | 本报告 |
| `AI_Analysis_Report\Build_Logs\*` | 本次全部烧录/校验/回读/监控日志、CSV、摘要、派生校验镜像 |

**未修改任何既有文件**（两个已验证脚本 `build_iar.ps1` / `analyze_iar_log.ps1` 保持 11-13 17:17/17:20 时间戳不变），因此没有 before/after diff；新增文件以 SHA256 留痕：

```
d328cfd16032638ac2f7b76f32f7c9fa108251b7423e74bfede15fc878ef4d12  flash_jlink.ps1 (创建时)
510eba3642dc15b88e3d3075cebf48da338ec135b0665720071c27776c89fabe  swd_monitor.ps1 (创建时)
```
（后续对两个脚本做过判定逻辑修正，最终版本以磁盘文件为准。）

**合规复核**：`svn status` 仍为且仅为 2 个用户未提交修改 —— `code\app\source\TaskTempModule.c`、`proj_source\version_json\version-sl1000-c7.json`，完整保留；未执行 `svn revert` / `cleanup` / `update` / `commit`。未改业务源码、`.ewp` / `.ewd` / `.icf` / 启动文件 / Option Bytes / Bootloader；未整片擦除；未自动切换芯片型号；未改芯片安全配置。

---

## 七、可直接复制的命令

```powershell
# 安全烧录（默认仅 App 区，绝不覆盖 Bootloader）
& "D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Tools\flash_jlink.ps1" `
  -JLinkExe "C:\Program Files\SEGGER\JLink_V990\JLink.exe" `
  -FirmwarePath "D:\workspace\SL-1000\CODE\DevBranche\C7\proj_source\Debug\Exe\C7Detect.out" `
  -ExpectedDevice "STM32F103xC" `
  -LogDir "D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Build_Logs"

# SWD 只读监控（例如 200 轮 / 1 秒）
& "D:\workspace\SL-1000\CODE\DevBranche\C7\AI_Analysis_Report\Tools\swd_monitor.ps1" `
  -Samples 200 -IntervalMs 1000 -LogDir "...\AI_Analysis_Report\Build_Logs"

# 自定义监视变量
& "...\swd_monitor.ps1" -Samples 10 -IntervalMs 200 `
  -WatchList "g_stC7=0x20005770=4;xTickCount=0x20006A68=1;SysTick_CVR=0xE000E018=1"
```

---

## 八、待确认 / 下一步

1. **长期监控目标**：请指定要盯的具体变量与采样周期（当前默认 18 项包含通用状态；若要看 CAN 命令响应、温度闭环、电机步数，需要把对应符号加入监视表）。
2. **业务功能验证**：当前无上位机 CAN 命令，`CtrlCmd` / `g_stC7` / 温度量静止。若要在真实动作下观察数据，需要上位机发命令或触发测试流程。
3. **C-SPY 断点调试尚未启动**：本机 `armjlink.dll` 是 2015 年 7.40.3.8937，而 `JLinkARM.dll` 被替换成 9.90，兼容性未实测；`.ewd` 里 `CCJLinkInterfaceRadio=1`（SWD 待 GUI 复核）、速度固定 32 kHz、`OCDownloadVerifyAll=0`。这些属于受保护工程配置，需你授权后才改。
4. **FreeRTOS RTOS-aware**：IAR 7.40.3 无 FreeRTOS 插件，当前靠 SWD 直读 `pxCurrentTCB` / `xTickCount` / TCB 任务名实现等效观测（已验证可用）。
5. 若你本意是烧录**含 Bootloader 的整片合并镜像** `C7Detect.bin`（0x08000000 起），请明确告知 —— 本次刻意没有这样做。
