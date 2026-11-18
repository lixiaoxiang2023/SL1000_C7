#include "stm32f10x.h"
#include "stm32_flash.h"
#include "stm32f10x_flash.h"
//#include "flash_if.h"

uint8_t EraseSaveFlash(void);



/*****************************************************************************************
函数名：uint8_t UserDataToFlash(uint32_t dwOffsetAdrr,uint32_t *pdwDataBuff, uint32_t dwLength)
参数描述：
uint32_t dwOffsetAdrr：需要写入数据的偏移地址
uint32_t *pdwDataBuff：数据源的首地址
uint32_t dwLength:     数据长度（字节）
返回值描述：
写入失败 0
写入成功 1
说明：将需要保存的参数写入Flash中
特殊算法：无
注意事项：无
*****************************************************************************************/
uint8_t UserDataToFlash(uint32_t dwOffsetAdrr,u8 *vpDataBuff, uint32_t dwLength)
{
    uint16_t FlashData = 0xffff;
    uint32_t dwWriteAdrr = 0;
//    uint32_t SaveFlashAddress = SAVE_FLASH_ADDR;
//    uint32_t BackupFlashAddress = BACKUP_FLASH_ADDR;
//    uint8_t  *pdyDataBuff;


    __set_PRIMASK(1);

    FLASH_Unlock();//Flash解锁

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    EraseSaveFlash();

   // pdyDataBuff = vpDataBuff;

    dwWriteAdrr = dwOffsetAdrr+ SAVE_FLASH_ADDR;
    while(dwLength --)
    {
      FlashData=*(vpDataBuff++);
      FlashData|=*(vpDataBuff++)<<8;
      FLASH_ProgramHalfWord(dwWriteAdrr, FlashData);
      dwWriteAdrr += 2;
    }
     FLASH_Lock();//Flash解锁
    __set_PRIMASK(0);
    return 1;
}
/*****************************************************************************************
函数名：uint8_t FlashToUserData(uint32_t dwOffsetAdrr,uint32_t *pdwDataBuff, uint32_t dwLength)
参数描述：
uint32_t dwOffsetAdrr：需要写入数据的偏移地址
uint32_t *pdwDataBuff：数据源的首地址
uint32_t dwLength:     数据长度（字节）
返回值描述：
写入失败 0
写入成功 1
说明：Flash中存储的数据读出到系统参数
特殊算法：无
注意事项：无
*****************************************************************************************/
void FlashToUserData(uint32_t dwOffsetAdrr,u8 *pvDataBuff, uint32_t dwLength)
{
    uint32_t SaveFlashAddress = SAVE_FLASH_ADDR;
    uint8_t *pdyDataBuff;
    pdyDataBuff = (uint8_t*)pvDataBuff;
    for(uint32_t dwReadAdrr = 0;dwReadAdrr < dwLength;dwReadAdrr++)
    {
       *(pdyDataBuff + dwReadAdrr) = *(uint8_t*)(SaveFlashAddress + dwOffsetAdrr + dwReadAdrr);
    }

}


/*****************************************************************************************
函数名：uint8_t CheckSaveFlashData(void)
参数描述：

返回值描述：
数据保存区无数据 0
数据保存区有数据 1
说明：将初始化的参数写入Flash中
特殊算法：无
注意事项：无
*****************************************************************************************/
uint8_t CheckSaveFlashData(uint32_t dwOffsetAdrr)
{
    uint8_t  byFlashData = 0xff;
    uint32_t dwReadAdrr = 0;
    uint16_t wDataCounter = 0;
    uint32_t SaveFlashAddress = SAVE_FLASH_ADDR;
    for(dwReadAdrr = dwOffsetAdrr;dwReadAdrr < dwOffsetAdrr + 100;dwReadAdrr++)
    {
        byFlashData = *(uint8_t*)(SaveFlashAddress + dwReadAdrr);//数去数据
        if(byFlashData != 0xff)
        {
            wDataCounter ++;
            if((wDataCounter) > 4)//存储区超过4个字节有数据认为存储有数据
                return 1;
        }
    }
    return 0;
}


/*****************************************************************************************
函数名：uint8_t EraseSaveFlash(void)
参数描述：

返回值描述：
擦除成功 1
擦除失败 0
说明：将存储数据的Flash擦除
特殊算法：无
注意事项：无
*****************************************************************************************/
uint8_t EraseSaveFlash(void)
{
    for(u8 Count =0 ;Count < SAVE_FLASH_SECTOR_NO; Count++)
    {
        if(FLASH_ErasePage(SAVE_FLASH_ADDR + (Count * FLASH_SECTOR_SIZE)) != FLASH_COMPLETE)//清空备份数据区
        {
            return 0;
        }
    }
    return 1;
}