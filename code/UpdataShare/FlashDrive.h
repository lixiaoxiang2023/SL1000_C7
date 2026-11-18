/*******************************************************************************
文件名称：FlashDrive.h
版本：        1.00
文件目录：    $PROJ_DIR$/../
项目名称：    事业一部技术平台
创建者：       lixx
创建日期：     2020.10.10
程序用途：内部FLASH驱动
版权声明：
Copyright urit Co.,Ltd.All Right Reserved

第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __FLASHDRIVE_H
#define __FLASHDRIVE_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "dev.h"
#include "StDriverCfg.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS   0x0803FFF0
#define USER_FLASH_END_ADDRESS     (USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS)
#define FLASH_PAGE_SIZE   0x800 //connectivity line devices flash page size is 2KByte
/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/


/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出变量申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
unsigned int FLASH_If_Write(__IO unsigned int* FlashAddress, unsigned int* Data ,unsigned short DataLength);
char FLASH_If_Erase(unsigned int  StartSector,unsigned char Sectors);
void FLASH_If_Init(void);
FLASH_Status FlashProgram (unsigned long* pulData, unsigned long ulAddress, unsigned long ulCount);

u16 FLASH_If_Read( __IO unsigned int* Addr, u32 *Readbuff, u16 uslen);

void FLASH_If_Init(void);

#endif
