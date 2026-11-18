# STM32 芯片识别报告

- 生成时间：2026-11-13 16:58
- 识别方式：多文件交叉验证，不依据文件夹名称

## 1. 芯片型号结论

**STM32F103xC**（ST 高密度系列，LQFP100 封装类别，待硬件确认封装）

| 证据项 | 来源文件 | 内容 |
|---|---|---|
| IAR 器件选择 | `proj_source/C7Detect.ewp` → `OGChipSelectEditMenu` 选项 | `STM32F103xC	ST STM32F103xC`（编译日志 `-cpu=Cortex-M3` 佐证） |
| 内核 | `C7Detect.ewp` 编译命令行 | `--cpu=Cortex-M3 --fpu=None`（无 FPU，与 CM3 一致） |
| 器件宏 | `platcode/devlib/inc/stm32f10x.h:46` | `#define STM32F10X_HD`（高密度设备，硬编码于平台库头文件） |
| 启动文件 | `code/devinit/startup_stm32f10x_hd.s`，且 `Debug/Obj/startup_stm32f10x_hd.o` 存在 | `_hd` = High Density，链接日志确认参与链接 |
| Flash 页大小 | `code/UpdataShare/FlashDrive.h` | `FLASH_PAGE_SIZE 0x800`（2 KB 页 = F103xC 特征；小容量为 1 KB） |

**冲突检查**：所有证据一致指向 STM32F103xC（高密度），无冲突。
注：`STM32F10X_HD` 是硬编码在平台库 `stm32f10x.h` 中而非工程宏定义（`CCDefines` 仅含 `__UC_AUTOBUILD_DEFAULT`），这属于平台库设计，不算证据冲突，但换芯片时需要修改平台库头文件。

## 2. 内核与资源

| 项目 | 值 | 依据 |
|---|---|---|
| 内核 | ARM Cortex-M3，最高 72 MHz | `--cpu=Cortex-M3`；`FreeRTOSConfig.h: configCPU_CLOCK_HZ 72000000` |
| FPU | 无 | `--fpu=None` |
| Flash 容量 | 256 KB（0x08000000 – 0x0803FFFF） | 见下节地址布局 |
| Flash 页大小 | 2 KB | `FLASH_PAGE_SIZE 0x800` |
| SRAM 容量 | 48 KB（0x20000000 – 0x2000BFFF） | 链接脚本 |
| PSRAM/外部存储 | 无（无 FSMC/SDRAM 配置证据） | 未发现任何外存初始化代码 |

## 3. Flash 地址布局（双镜像 IAP，实测自链接脚本与 map）

| 区域 | 地址范围 | 大小 | 链接脚本 |
|---|---|---|---|
| Bootloader | 0x08000000 – 0x08007FFF | 32 KB | `link_auto_link.icf`（`.intvec_boot` @0x08000000） |
| App | 0x08008000 – 0x0803FFFF | 192 KB | `link_only_app.icf`（`__ICFEDIT_intvec_start__ = 0x08008000`） |
| 参数存储区 | 0x08038000（Flash 第 64 页，属于 App 区内约定） | — | `code/dev/inc/stm32_flash.h: SAVE_FLASH_ADDR` |
| 升级完成标志 | 0x0803FFF0 | 4 B | `code/app/inc/bootMain_Cfg.h` |

RAM 布局：CSTACK 0x400 @0x20006AC0、FreeRTOS 堆 17 432 B（heap_2.o .bss @0x2000 首段）、
App 中断向量 `firmware_update_flag` 定位 @0x20000000（`__no_init`）。

## 4. 其他硬件证据（非本次重点，已核实存在）

- CAN 1（`CONFIG_CAN1 1`，250 kbps）、ADC1、TIM1/2/3/4/5/8、A3983 步进驱动（`MAX_MO 6`）
- 详见前期分析报告《硬件外设驱动说明.md》
