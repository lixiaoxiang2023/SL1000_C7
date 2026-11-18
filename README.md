# SL-1000 C7 嵌入式项目

从内部 SVN 仓库 `DevBranche/C7` 迁移到 GitHub 的项目快照。

## 项目构成

- `code/` — 应用源码(app / dev / devinit / FreeRTOS / iosrc / UpdataShare / version / xcan)
- `proj_source/platcode/` — 平台代码(STM32 标准外设库、FreeRTOS v10、FAT 文件系统、xcanlib、各型号 client)
- `proj_source/version_json/`、`settings/` — 版本与配置
- `AI_Analysis_Report/` — 环境识别、编译、烧录与问题分析报告

## 工具链与环境

- 编译器:IAR Embedded Workbench(ARM)
- 调试:J-Link(SWD)
- MCU:STM32 系列(详见 `AI_Analysis_Report/STM32芯片识别报告.md`)
- 原始工作区路径:`D:\workspace\SL-1000\CODE\DevBranche\C7`

## 说明

本仓库为源码归档快照。以下文件在迁移时未纳入版本控制:

- SVN 元数据(`.svn/`、`*.svn-base`)
- 编译中间产物(`*.o`、`*.pbi`、`*.cout`、`*.lst`、`Debug/` 等)
- 第三方工具可执行文件(`*.exe`、`*.zip` 等)
- 依赖库历史备份快照(`.dependlibs/repo_bkp/`)
- 含本机绝对路径的 J-Link 脚本(`*.jlink`)

完整规则见 [.gitignore](./.gitignore)。

## 导入方式

```bash
git clone <本仓库地址>
```

用 IAR 打开 `proj_source/` 下的工程文件即可构建;具体环境配置见 `AI_Analysis_Report/` 下的报告。
