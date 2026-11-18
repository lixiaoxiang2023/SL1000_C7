/*****************************************************************************
文件名称：ram.c
文件目录：$PROJ_DIR$/usr/app
项目名称：
创建者：胥强
创建日期：2012、10、10
程序用途：用于U560CPLD部分数据接口的初始化
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/

/******************************************************************************
Include files头文件
******************************************************************************/
#include "stm32f2xx.h"
#include "dev.h"
#include "stm32f2xx_fsmc.h"
#include "stm32f2xx_rcc.h"
//#include "stm32f2xx_gpio.h"
#include "ram.h"
#include "IOF2_4.h"

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/

/******************************************************************************
Global Variables Definition全局变量定义
******************************************************************************/
//显示内存区/----4M
#pragma location=".lcdram"
uint16_t g_wLcdMem[480][800];//750KB
#pragma location=".lcdram"
uint16_t g_wLcdMemCopy[480][800];//750KB
#pragma location=".lcdram"
uint16_t g_wLcdBackdrop[480][800];//750KB
#pragma location=".lcdram"
uint16_t g_w24X24Code[8178*36];//575KB
#pragma location=".lcdram"
uint16_t g_w16X16Code[8178*16];//255KB



//CIS检测数据内存区----2M
#pragma location=".cisram"
uint16_t g_wCisMem[40][5][1216];//475KB
//#pragma location=".cisram"
//uint16_t g_wCisMem_16bmp[1216][40];

//#pragma location=".cisram"
//uint16_t g_wCisMem_24bmp[63600];//1060*40*3/2

//用户数据存储区 2M
#pragma location=".usrram"
uint16_t g_wCisBlan[10][1216];//24KB
#pragma location=".usrram"
uint16_t g_waCalcBuff[40*40];//3.1KB
#pragma location=".usrram"
uint16_t g_waPicDataBuff[300*1024];//600KB
#pragma location=".usrram"
uint16_t g_wStripBin[10*1024];//20KB
#pragma location=".usrram"
uint16_t g_wPrintBin[10*1024];//20KB
#pragma location=".usrram"
uint16_t g_wW25BlockBuf[64*1024];//128KB

/******************************************************************************
外部全局变量申明
******************************************************************************/
extern uint8_t  g_SYSTEST;//系统整机测试，0 不测试；1 测试
/******************************************************************************
内部数据结构与数据类型定义
******************************************************************************/

/******************************************************************************
内部全局变量定义
******************************************************************************/

/******************************************************************************
内部函数申明
******************************************************************************/


/*****************************************************************************
函数名：void SramInit(void)
函数功能：初始化存储器，同时初始化CPLD以及SDRAM模块
参数描述: 无
返回值描述： 无
特殊算法：无
注意事项：如果要使用CPLD部分电路必须在系统时钟设置完成后，初始化此函数。
******************************************************************************/
//int RamInit(void)
//{
//        FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
//        FSMC_NORSRAMTimingInitTypeDef  p;
//        GPIO_InitTypeDef GPIO_InitStructure;
//
//        /* Enable GPIOs clock */
//        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |
//                               RCC_AHB1Periph_GPIOB, ENABLE);
//
//        /* Enable FSMC clock */
//        RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
//
//        /* GPIOD configuration */
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
//        //GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//
//        GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3 | GPIO_Pin_4  | GPIO_Pin_5  |
//                GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
//                        GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//
//        GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//        //GPIOB configuration
//        GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_FSMC);
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//
//        GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//        /* GPIOE configuration */
//        //GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
//        //GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
//        //GPIO_PinAFConfig(GPIOE, GPIO_PinSource2 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource6 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
//        GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);
//
//        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |
//                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
//                        GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
//                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//
//        GPIO_Init(GPIOE, &GPIO_InitStructure);
//
//        /*-- FSMC Configuration ------------------------------------------------------*/
//        p.FSMC_AddressSetupTime = 0;
//        p.FSMC_AddressHoldTime = 0;
//        p.FSMC_DataSetupTime = 0;
//        p.FSMC_BusTurnAroundDuration = 15;
//        p.FSMC_CLKDivision = 7;
//        p.FSMC_DataLatency = 1;
//        p.FSMC_AccessMode = FSMC_AccessMode_D;
//
//        FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
//        FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
//        FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
//        FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//        FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Enable;
//        FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Enable;
//        FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_High;
//        FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
//        FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//        FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//        FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Enable;
//        FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//        FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Enable;
//        FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
//        FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
//
//        FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
//
//        /*!< Enable FSMC Bank1_SRAM2 Bank */
//        FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
//
//        //开启LCD背光
//        REG_CPLD_LCD_PWR = 1;
//        //REG_CPLD_CIS_EN = 1;//使能CIS时钟
//
//        for(int delay = 0;delay < 0xffffff; delay++);
//        //CPLD复位
//        REG_CPLD_RST = 0;
//        for(int delay = 0;delay < 0xffffff;delay ++);
//        REG_CPLD_RST = 1;
//
//        //delay
//        for(int delay = 0;delay < 0xffffff; delay++);
//        REG_CPLD_CIS_CLR = 1;
//        for(int delay = 0;delay < 0x0a; delay++);
//        REG_CPLD_CIS_CLR = 0;
//
//        //REG_CPLD_CIS_EN = 1;
//        for(int delay = 0;delay < 0xffffff; delay++);
//        //REG_CPLD_CIS_EN = 0;
//
//        REG_CPLD_BEEP = 0;
////        if(g_SYSTEST == 0) {
////                for(int dwTestCut = 0; dwTestCut < 800*480*2; dwTestCut += 2) {
////                        *(uint16_t*)(EXT_MEMERY_START + dwTestCut) = (uint16_t)dwTestCut;
////
////                        if(*(uint16_t*)(EXT_MEMERY_START + dwTestCut) != (uint16_t)dwTestCut) {
////                                while(1) {
////                                        REG_CPLD_BEEP = 0;
////                                        for(int delay = 0;delay < 0xffffff; delay++);
////                                        REG_CPLD_BEEP = 1;
////                                        for(int delay = 0;delay < 0xffffff; delay++);
////                                }
////                        }
////                }
////        }
//
//        return 0;
//}

int RamInit(void)
{
        FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
        FSMC_NORSRAMTimingInitTypeDef  p;

        /* Enable GPIOs clock */

        /* Enable FSMC clock */
    //    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

        /* GPIOD configuration */
        DevRccAHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

        DevIo->PinAFConfig(GPIOD, 0 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 1 ,GPIO_AF_FSMC);
       // DevIo->PinAFConfig(GPIOD, 2 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 3 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 4 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 5 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 6 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 7 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 8 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 9 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 10 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 11 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 12 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 13 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 14 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOD, 15 ,GPIO_AF_FSMC);

        DevIo->Config(PD, 0, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 1, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
//        DevIo->Config(PD, 2, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 3, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 4, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 5, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 6, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 7, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 8, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 9, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 10, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 11, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 12, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 13, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 14, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PD, 15, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);

        DevIo->PinAFConfig(GPIOB, 7 ,GPIO_AF_FSMC);
        DevIo->Config(PB, 7, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);





//        DevIo->PinAFConfig(GPIOE, 0 ,GPIO_AF_FSMC);
//        DevIo->PinAFConfig(GPIOE, 1 ,GPIO_AF_FSMC);
//        DevIo->PinAFConfig(GPIOE, 2 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 3 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 4 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 5 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 6 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 7 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 8 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 9 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 10 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 11 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 12 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 13 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 14 ,GPIO_AF_FSMC);
        DevIo->PinAFConfig(GPIOE, 15 ,GPIO_AF_FSMC);

//        DevIo->Config(PE, 0, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
//        DevIo->Config(PE, 1, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
//        DevIo->Config(PE, 2, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 3, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 4, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 5, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 6, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 7, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 8, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 9, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 10, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 11, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 12, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 13, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 14, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);
        DevIo->Config(PE, 15, 1, Mode_AF, OType_PP, Speed_100MHz, PuPd_NOPULL);


        /*-- FSMC Configuration ------------------------------------------------------*/
        p.FSMC_AddressSetupTime = 0;
        p.FSMC_AddressHoldTime = 0;
        p.FSMC_DataSetupTime = 0;
        p.FSMC_BusTurnAroundDuration = 15;
        p.FSMC_CLKDivision = 7;
        p.FSMC_DataLatency = 1;
        p.FSMC_AccessMode = FSMC_AccessMode_D;

        FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
        FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
        FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
        FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
        FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Enable;
        FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Enable;
        FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_High;
        FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
        FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
        FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
        FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Enable;
        FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
        FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Enable;
        FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
        FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

        FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

        /*!< Enable FSMC Bank1_SRAM2 Bank */
        FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

        //开启LCD背光
        //REG_CPLD_LCD_PWR = 1;
        //REG_CPLD_CIS_EN = 1;//使能CIS时钟

        for(int delay = 0;delay < 0xffffff; delay++);
        //CPLD复位
        REG_CPLD_RST = 0;
        for(int delay = 0;delay < 0xffffff;delay ++);
        REG_CPLD_RST = 1;

        //delay
        for(int delay = 0;delay < 0xffffff; delay++);
        REG_CPLD_CIS_CLR = 1;
        for(int delay = 0;delay < 0x0a; delay++);
        REG_CPLD_CIS_CLR = 0;

        //REG_CPLD_CIS_EN = 1;
        for(int delay = 0;delay < 0xffffff; delay++);
        //REG_CPLD_CIS_EN = 0;

        //REG_CPLD_BEEP = 0;
//        if(g_SYSTEST == 0) {
//                for(int dwTestCut = 0; dwTestCut < 800*480*2; dwTestCut += 2) {
//                        *(uint16_t*)(EXT_MEMERY_START + dwTestCut) = (uint16_t)dwTestCut;
//
//                        if(*(uint16_t*)(EXT_MEMERY_START + dwTestCut) != (uint16_t)dwTestCut) {
//                                while(1) {
//                                        REG_CPLD_BEEP = 0;
//                                        for(int delay = 0;delay < 0xffffff; delay++);
//                                        REG_CPLD_BEEP = 1;
//                                        for(int delay = 0;delay < 0xffffff; delay++);
//                                }
//                        }
//                }
//        }

        return 0;
}

////////////////////////////////////////////////////////////一下函数为SRAM测试所用/////////////////////////////////////////////////////////////////////
/*****************************************************************************
int SramTest(void)
函数功能：SRAM测试
参数描述: 无
返回值描述： 无
特殊算法：无
注意事项：
******************************************************************************/
int SramTest(void)
{
        int wErrCount = 0;
        for(int dwTestCut = 0; dwTestCut < 0x800000; dwTestCut += 2) {
                *(uint16_t*)(EXT_MEMERY_START + dwTestCut) = (uint16_t)dwTestCut;

                if(*(uint16_t*)(EXT_MEMERY_START + dwTestCut) != (uint16_t)dwTestCut) {
                        wErrCount ++;
                }
        }

        return wErrCount;
}
