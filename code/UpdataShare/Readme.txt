文件说明：
1、FlashDrive.c 为STM32F4、STM32F1内部FLASH读写驱动程序，与FlashDrive.h直接移植可用；
2、sys_updata.c 为外置FLASH，带文件系统，固件升级解析封装程序，默认1M FLASH. 选择外置FLASH固件升级需移植；

固件升级模块移植说明：
1、移植bootMain.c文件，根据自己芯片内核更改对应芯片中断向量表，根据自己升级方式初始化外设（1、外置FLASH方式需要初始化SPI ，2、bootloader Can升级需要初始化can及timer2定时器）可参照DEMO程序修改文件

2、bootMain_cfg.h，为固件升级参数配置文件：
#include "iosrc_b3.h"                                        //协议资源
#define  APP_START_ADDR                    0x08008000            //应用程序起始地址
#define  USER_APP_MAX_SIZE                 ((1024-32)*1024) //用户应用程序最大  空间
#define  USER_APP_MIN_SIZE                 (32*1024)             //用户应用程序最小32KB
#define  DEV_NAME                                 g_stB3                 //CAN 通信结构体资源
#define USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS   0x080FFFF0               //固件下载完成标志 一般放在 FLASH 末端 最后4字节

3、dev_config.h  配置 CONFIG_FLASH 1 、CONFIG_TIM2 1   SYS_FILE （选择升级方式 0 为bootloader直接升级 1通过SPI FLASH 升级）
