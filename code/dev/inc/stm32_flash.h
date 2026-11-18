/*
**file name :stm32_flash.h
**author xq
*/

#ifndef _STM32_FLASH
#define _STM32_FLASH

#include "stm32f10x.h"

#define    FLASH_SECTOR_SIZE        0x400          // 1K 一个扇区的大小
#define    SAVE_FLASH_SECTOR_NO     10             //存储区扇区数
#define    BACKUP_FLASH_SECTOR_NO   10             //备份区扇区数
#define    SAVE_FLASH_ADDR          0x08038000                                //存储Flash起始地址
#define    BACKUP_FLASH_ADDR        SAVE_FLASH_ADDR +   FLASH_SECTOR_SIZE * SAVE_FLASH_SECTOR_NO  //备份Flash起始地址




#define    SYS_DATA_OFFEST          0x00          //系统参数存储偏移地址0x00,最大存储长度 128字节


uint8_t UserDataToFlash(uint32_t dwOffsetAdrr,u8 *vpDataBuff,uint32_t dwLength);
void FlashToUserData(uint32_t dwOffsetAdrr,u8 *pvDataBuff, uint32_t dwLength);
uint8_t InitSysDataToFlash(uint8_t *pdyDataBuff, uint32_t dwLength);
uint8_t CheckSaveFlashData(uint32_t dwOffsetAdrr);
uint8_t EraseSaveFlash(void);
#endif