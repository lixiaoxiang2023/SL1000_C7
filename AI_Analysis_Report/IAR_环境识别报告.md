# IAR 环境识别报告

- 生成时间：2026-11-13 16:58
- 识别方式：直接扫描安装目录 + 读取 exe 文件版本资源，未做任何猜测

## 1. 安装位置

| 项目 | 结果 |
|---|---|
| `C:\Program Files\IAR Systems` | **不存在** |
| `C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.2` | 存在 |
| `C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.3` | 存在 |

## 2. IarBuild.exe 版本信息

| 安装 | IarBuild.exe 完整路径 | FileVersion | ProductVersion |
|---|---|---|---|
| EW 7.2 | `C:\Program Files (x86)\IAR Systems\Embedded Workbench 7.2\common\bin\IarBuild.exe` | **7.2.4.3784** | 7.2.4.0 |
| EW 8.3 | `C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.3\common\bin\IarBuild.exe` | 8.3.2.5988 | 8.3.2.0 |

## 3. ARM 编译器（iccarm.exe）版本

| 安装 | iccarm.exe FileVersion | 实际编译器字符串 |
|---|---|---|
| EW 7.2 目录 | **7.40.3.8902** | "IAR ANSI C/C++ Compiler V7.40.3.8902/W32 for ARM"（本次编译日志中实测） |
| EW 8.3 目录 | 8.40.1.212 | — |

**重要说明**：目录名 "Embedded Workbench 7.2" 是产品系列名，不代表编译器版本。
该安装内的 iccarm 实际是 **7.40.3.8902**，与工程 `.ewp` 历史元数据记录的 7.40.3.8937 同属
7.40.3 补丁族（仅内部 build 号差 35），可直接用于本工程。本次基线编译即使用此安装。

## 4. 命令行工具可用性（实测验证）

以下工具均通过实际调用验证可用：

| 工具 | 路径（EW 7.2 安装内） | 验证方式 |
|---|---|---|
| IarBuild.exe | `...\Embedded Workbench 7.2\common\bin\IarBuild.exe` | 完整编译调用 3 次 |
| iccarm.exe | `...\Embedded Workbench 7.2\arm\bin\iccarm.exe` | 编译日志中全部源文件编译成功 |
| ilinkarm.exe | `...\Embedded Workbench 7.2\arm\bin\ilinkarm.exe` | 链接成功（含 post-build 二次链接） |
| ielftool.exe | `...\Embedded Workbench 7.2\arm\bin\ielftool.exe` | bin 转换成功（V9.18.7.137） |

## 5. 选型结论

- **本工程使用 EW 7.2 安装（iccarm 7.40.3.8902）**，理由：
  1. 与 `.ewp` 历史构建元数据（7.40.3）匹配；
  2. 编译选项（`-e` IAR 扩展、`--use_c++_inline`、DLIB Full）在 7.40 语义下与既有 `.o/.map` 一致；
  3. 用 8.40.1 编译器编译 2015 年代工程存在 C 标准默认值变化、警告集变化等风险，不适合作为基线。
- EW 8.3（iccarm 8.40.1.212）保留不用，仅在将来迁移工程时评估。
