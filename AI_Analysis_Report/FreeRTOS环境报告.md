# FreeRTOS 环境报告

- 生成时间：2026-11-13 16:58

## 1. 内核版本

**FreeRTOS V10.1.0**（证据：`proj_source/platcode/FreeRTOSv10/include/task.h:46`
`#define tskKERNEL_VERSION_NUMBER "V10.1.0"`，并经编译链接验证 tasks.o/queue.o/list.o/croutine.o 参与链接）

⚠️ **证据冲突（已标记）**：`code/FreeRTOS/FreeRTOSConfig.h:2` 的许可声明注释写的是
"FreeRTOS V8.0.1 - Copyright (C) 2014"。该注释为陈旧遗留文本，**实际编译进固件的内核源码是 V10.1.0**
（task.h 宏定义是编译期事实，注释不参与语义）。建议后续更正注释，避免误导。

## 2. 堆实现

| 项目 | 值 | 证据 |
|---|---|---|
| heap 方案 | **heap_2**（不合并空闲块，定长分配） | `platcode/FreeRTOSv10/heap_2.c` 参与编译（Debug/Obj/heap_2.o + 链接日志）；heap_1/3/4/5 源码在库中但未编译 |
| 总堆大小 | `configTOTAL_HEAP_SIZE (17*1024)` = 17 432 B | `code/FreeRTOS/FreeRTOSConfig.h`；`C7Detect.map` heap_2.o .bss 实测 0x4400 |
| 分配方式 | 全部任务通过 `xTaskCreateStatic`? 否 —— 动态创建（pvPortMalloc） | main.c 中各 `CreateXxxTask()` 调用 `xTaskCreate` |

## 3. Cortex-M 移植层

| 项目 | 值 | 证据 |
|---|---|---|
| 移植层 | **ARM_CM3**（GCC/IAR 版 `portable/ARM_CM3/port.c` + portasm.s） | 编译路径 `platcode/FreeRTOSv10\portable\ARM_CM3`；port.o / portasm.o 参与链接 |
| 备选移植层 | ARM_CM4F 源码存在但未编译 | 目录中 port.c(CM4F) 无对应 .o |
| SysTick | `xPortSysTickHandler` 经向量表桥接 | `startup_stm32f10x_hd.s` 尾部 `#define vPortSVCHandler SVC_Handler` 等 3 条宏 |
| 中断阈值 | `configMAX_SYSCALL_INTERRUPT_PRIORITY 191`（0xB0 ≈ 抢占优先级 11） | FreeRTOSConfig.h |

## 4. 关键调度配置（FreeRTOSConfig.h 实测）

| 配置项 | 值 |
|---|---|
| configCPU_CLOCK_HZ | 72 000 000 |
| configTICK_RATE_HZ | 1000（1 ms tick） |
| configUSE_PREEMPTION | 1（抢占式） |
| configMAX_PRIORITIES | 5（实际仅用 0 与 1 两级） |
| configMINIMAL_STACK_SIZE | 128（字，= 512 B） |
| configKERNEL_INTERRUPT_PRIORITY | 255 |
| INCLUDE_uxTaskGetStackHighWaterMark | 1（已使能但全工程未调用） |

## 5. 任务清单（11 个，均动态创建于 main.c 调用的各 Create 函数）

MoveTask / TempModuleTask / FanTempTask / SendDataTask / XcanTask / LedCheckTask
等，创建顺序见 `main()`；详细优先级/栈/队列分析见前期《任务与中断关系说明.md》。

## 6. 与编译环境的关联事实

- FreeRTOS 头文件搜索路径已正确配置（见《IAR工程配置报告.md》§5）
- ⚠️ 已知历史问题（前期分析 P-01）：平台库 `platcode/devlib/inc/configinit.h` 将
  `CONFIG_FREERTOS` 默认置 0，导致 devlib 内 `SysDelay→vTaskDelay` 映射失效。
  该宏只影响平台库内部延时行为，不影响 FreeRTOS 内核本身的编译。
