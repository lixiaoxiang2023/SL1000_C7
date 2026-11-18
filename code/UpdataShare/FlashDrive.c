/*******************************************************************************
文件名称：   FlashDrive.c
项目名称：   事业一部技术平台
创建者：     lixx
创建日期：   2020.10.7
程序用途：MCU内部FLASH读写驱动

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "FlashDrive.h"

/*******************************************************************************
Constant Definition
*******************************************************************************/
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

#if PLAT_STM32_F1
#define FLASH_PAGE_SIZE   0x800 //connectivity line devices flash page size is 2KByte
#endif
/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
外部变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
/**
* @brief  addr chage to Sector
* @param  None
* @retval None
*/
#if PLAT_STM32_F4

static unsigned char GetSector(unsigned int  Address)
{
    unsigned char sector = 0;

    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = 0;
    }
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = 1;
    }
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = 2;
    }
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = 3;
    }
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = 4;
    }
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = 5;
    }
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = 6;
    }
    else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = 7;
    }
    else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = 8;
    }
    else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = 9;
    }
    else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = 10;
    }
    else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = 11;
    }

    return sector;
}
#endif
/**
* @brief  Unlocks Flash for write access
* @param  None
* @retval None
*/
void FLASH_If_Init(void)
{
    FLASH_Unlock();
}

/**
* @brief  This function does an erase of all user flash area
* @param  StartSector: start of user flash area,Sectors: sector num to erase
* @retval 0 if success, -1 if error
*/
#if PLAT_STM32_F4
char FLASH_If_Erase(unsigned int  StartSector,unsigned char Sectors)
{
    const unsigned char strSector[]={
        FLASH_Sector_0,FLASH_Sector_1,FLASH_Sector_2,FLASH_Sector_3,
        FLASH_Sector_4,FLASH_Sector_5,FLASH_Sector_6,FLASH_Sector_7,
        FLASH_Sector_8,FLASH_Sector_9,FLASH_Sector_10,FLASH_Sector_11,
    };
    unsigned char ucSectorStart;

    ucSectorStart  = GetSector(StartSector);

    while (Sectors)
    {
        if (FLASH_EraseSector(strSector[ucSectorStart+Sectors-1],VoltageRange_3) == FLASH_COMPLETE)
        {
            Sectors--;
        }
        else
        {
            return -1;
        }
    }
    return 0;
}
#elif PLAT_STM32_F1
char FLASH_If_Erase(unsigned int  StartSector,unsigned char Sectors)
{
    unsigned int FlashAddress;

    FlashAddress = StartSector;
    Sectors = Sectors -  (( StartSector - 0x08000000)/FLASH_PAGE_SIZE);
    while (Sectors)
    {
        if (FLASH_ErasePage(FlashAddress) == FLASH_COMPLETE)
        {
            FlashAddress += FLASH_PAGE_SIZE;
            Sectors--;
        }
        else
        {
            return -1;
        }
    }
    return 0;
}

#endif
/**
* @brief  This function writes a data buffer in flash (data are 32-bit aligned).
* @note   After writing data buffer, the flash content is checked.
* @param  FlashAddress: start address for writing data buffer
* @param  Data: pointer on data buffer
* @param  DataLength: length of data buffer (unit is 32-bit word)
* @retval 0: Data successfully written to Flash memory
*         1: Error occurred while writing data in Flash memory
*         2: Written Data in flash memory is different from expected one
*/

unsigned int FLASH_If_Write(__IO unsigned int* FlashAddress, unsigned int* Data ,unsigned short DataLength)
{
    unsigned int i = 0;

    for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS)); i++)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
        be done by word */
        if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
        {
            /* Check the written value */
            if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
            {
                /* Flash content doesn't match SRAM content */
                return(2);
            }
            /* Increment FLASH destination address */
            *FlashAddress += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return (1);
        }
    }

    return (0);
}

//ulAddress和ul-Count必须4整除
FLASH_Status FlashProgram (unsigned long* pulData, unsigned long ulAddress, unsigned long ulCount)
{
	unsigned long Address,*pData,i;
	FLASH_Status  FLASHStatus = FLASH_BUSY;
	Address = ulAddress;
	pData = pulData;
	for(i = 0;i < (ulCount>>2);i++)
	{
		FLASHStatus = FLASH_ProgramWord(Address, *pData);
		pData++;//next 4 bytes
		Address += 4;
	}
	return FLASHStatus;
}

u16 FLASH_If_Read( __IO unsigned int* Addr, u32 *Readbuff, u16 uslen)
{
    for (u16 i = 0; i < uslen; i += 4)
    {
        Readbuff[i >> 2] = Addr[i >> 2];
    }
    return 0;
}
