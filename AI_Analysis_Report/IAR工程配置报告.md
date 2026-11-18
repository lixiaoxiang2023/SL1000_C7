# IAR 工程配置报告

- 生成时间：2026-11-13 16:58

## 1. 工程文件清单（全目录扫描结果）

| 类型 | 文件 | 说明 |
|---|---|---|
| `.eww` 工作空间 | `proj_source/C7Detect.eww` | 仅含一个工程引用 `$WS_DIR$\C7Detect.ewp`，无批构建定义 |
| `.ewp` 工程 | `proj_source/C7Detect.ewp` | **唯一主工程**（全目录仅此 1 个 .ewp，无歧义，无需用户确认） |
| `.ewd` 调试配置 | `proj_source/C7Detect.ewd` | 对应 Debug 配置的 C-SPY 调试设置（fileVersion 2，`<name>Debug</name>`） |
| `.icf` 链接脚本 | `proj_source/link_only_app.icf` | **Debug 配置实际使用**（App @0x08008000） |
| | `proj_source/link_auto_link.icf` | post-build 二次链接使用（Boot @0x08000000） |
| | `proj_source/link_all.icf` | 未被工程引用（历史遗留） |
| | `code/A2Detect.icf` | 未被工程引用（其他产品线遗留） |
| 后处理脚本 | `proj_source/build_post.bat` | `.ewp` Postbuild 命令：`$PROJ_DIR$\build_post.bat $PROJ_DIR$ $TARGET_FNAME$` |

**主 .ewp 确认**：`D:\workspace\SL-1000\CODE\DevBranche\C7\proj_source\C7Detect.ewp`

## 2. 编译配置（Configuration）名称

`.ewp` 内共 **2 个 configuration**（实测行号：5 行 Debug 起、979 行 Release 起）：

| 配置名 | 链接脚本 | 宏定义 | 状态 |
|---|---|---|---|
| **Debug** | `$PROJ_DIR$\link_only_app.icf` | `__UC_AUTOBUILD_DEFAULT` | ✅ 实际使用（`.eww` 批构建绑定项与 `.ewd` 均为 Debug；历史 `Debug/List/*.map` 亦由此产生） |
| Release | `$TOOLKIT_DIR$\CONFIG\generic.icf`（通用脚本，非定制） | `NDEBUG` | 未使用，脚本未定制，编译无法产出双镜像固件 |

**结论：应使用 `Debug` 配置编译，本次基线编译即用它。**
注意：Debug 名称下并非纯调试——它带 post-build 二次链接（`--semihosting` 参数来自工程链接设置，生产固件也从该配置产出）。

## 3. 编译器版本与语言选项（自编译命令行实测）

| 选项 | 值 | 说明 |
|---|---|---|
| 编译器 | IAR ANSI C/C++ Compiler V7.40.3.8902/W32 for ARM | 编译日志实测 |
| C 标准 | 未显式指定 `--c89/--c99/--std`，IAR 7.40 默认 **C89/90 + IAR 扩展**（`-e` 开启） | 日志命令行实测 |
| C++ 内联 | `--use_c++_inline` | 命令行实测 |
| 优化 | `-Ol`（低优化），且 `--no_cse --no_unroll --no_inline --no_code_motion --no_tbaa --no_clustering --no_scheduling`（全关） | Debug 配置 |
| 运行库 | DLIB Full（`--dlib_config ...\INC\c\DLib_Config_Full.h`，`--redirect _Printf=_PrintfFull _Scanf=_ScanfFull`） | 命令行实测 |
| 抑制的诊断 | `--diag_suppress Pa039,Pa082` | 命令行实测 |
| 链接 | `--entry __iar_program_start --vfe --semihosting --config link_only_app.icf` | 链接日志实测 |

## 4. 预定义宏（CCDefines）

- `__UC_AUTOBUILD_DEFAULT`（仅此 1 个，Debug 与 Release 各自不同：Release 为 `NDEBUG`）
- `STM32F10X_HD` 不在工程宏中，硬编码于 `platcode/devlib/inc/stm32f10x.h:46`

## 5. 头文件搜索路径（CCIncludePath2，共 20 条）

```
$PROJ_DIR$\platcode\devlib\inc
$PROJ_DIR$\platcode\devlib\std
$PROJ_DIR$\platcode\xcanlib
$PROJ_DIR$\platcode\xcanlib\include
$PROJ_DIR$\platcode\FreeRTOSv10\include
$PROJ_DIR$\platcode\FreeRTOSv10\portable\ARM_CM3
$PROJ_DIR$\platcode\StStdPeriphDriver\inc
$PROJ_DIR$\..\code\devinit
$PROJ_DIR$\..\code\dev\inc
$PROJ_DIR$\..\code\FreeRTOS
$PROJ_DIR$\..\code\iosrc
$PROJ_DIR$\..\code\xcan
$PROJ_DIR$\..\code\app\inc
$PROJ_DIR$\..\code\inc
$PROJ_DIR$\..\code\fat
$PROJ_DIR$\..\code\pid
$PROJ_DIR$\..\code\UpdataShare
$PROJ_DIR$\platcode
$PROJ_DIR$\
$PROJ_DIR$\..\code\version
```

## 6. 参与编译的源文件（57 条，链接日志确认 57 个 .o）

- 应用层：bootMain / FunCtrl / main / MotorCtrl / ProgramUpdata / QRCodeMod / SysData / TaskMove / TaskTempModule / TaskWasteDetect
- 设备层：FlashDrive / i2c_io / mpu6050 / piheat_pid / sensor / stm32_flash / stm32f10x_flash / tempfan / XH711
- 平台库：devinit / dev / devplat / io / tim / adc / bxcan / usart / pwm / pw / a3983 / spi / i2c / dac / dma / ds18b20 / eth / rtc / at45 / misc / core_cm3 / system_stm32f10x / ver_def
- FreeRTOS：tasks / queue / list / croutine / heap_2 / port / portasm
- xcanlib：xcan / xcan_config / xcan_in / xcan_out / xcan_sync
- 启动文件：startup_stm32f10x_hd.s

## 7. 工程状态检查（只读）

- **非 Git 仓库**（`git rev-parse` 失败：not a git repository），实际使用 **SVN**
- `svn status`（只读）发现 **2 个未提交的用户修改**，已保留、未覆盖：
  1. `M code/app/source/TaskTempModule.c`
  2. `M proj_source/version_json/version-sl1000-c7.json`
- 未版本化条目（`?`）：`proj_source/.dependlibs`、`C7Detect.dep`、`C7Detectbin`、`Debug` 等（均为构建产物/依赖库快照）
- 本阶段未执行任何 `svn update/commit/revert`，未执行任何清理命令
