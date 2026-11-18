/******************************************************************************
文件名称：FlashDist.h
文件目录：$PROJ_DIR$\code\user\inc
项目名称：全自动1800
创建者：  许昌德
创建日期：20150811
程序用途：
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/
#ifndef __FLASHDIST_H
#define __FLASHDIST_H
/******************************************************************************
Include files头文件
******************************************************************************/
//#include "stm32f10x.h"
//#include "stm32Bit.h"

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
#define FLASH_PAGE_SIZE         (uint16_t) 0x1FFFF
#define Write_start_addr        ((uint32_t)0x08020000)//扇区6 共128K空间
#define Updata_Flash_Flag        0x0803FFF0          //升级标志位存放地址
#define Updat_Len_addr           0x0803FFF4         //升级标志位存放地址
#define UP_FLASH_WAIT_TIMEOUT    100000
//#define Updata_Flash_Flag        0x0803C000          //升级标志位存放地址--240K
//#define SaveSetModeStateAddr     0x0803D000         //模式保存地址
//#define SaveSampPositionAddr     0x0803E000         //采样针位置数据保存
//#define SaveDropSizeAddr         0x0803F000         //滴样量保存






#endif