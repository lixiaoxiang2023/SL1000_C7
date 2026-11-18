/*******************************************************************************
文件名称：   io.h
文件目录：   $PROJ_DIR$/usr/Drive/inc
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.12
程序用途：   用于通用的IO口驱动


版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __IO_H
#define __IO_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define PA      0
#define PB      1
#define PC      2
#define PD      3
#define PE      4

///
#define GPIO_Pin_0                 ((unsigned short int)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((unsigned short int)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((unsigned short int)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((unsigned short int)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((unsigned short int)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((unsigned short int)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((unsigned short int)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((unsigned short int)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((unsigned short int)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((unsigned short int)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((unsigned short int)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((unsigned short int)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((unsigned short int)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((unsigned short int)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((unsigned short int)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((unsigned short int)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((unsigned short int)0xFFFF)  /* All pins selected */

//
#define GPIO_Remap_SPI1            ((unsigned int)0x00000001)  /* SPI1 Alternate Function mapping */
#define GPIO_Remap_I2C1            ((unsigned int)0x00000002)  /* I2C1 Alternate Function mapping */
#define GPIO_Remap_USART1          ((unsigned int)0x00000004)  /* USART1 Alternate Function mapping */
#define GPIO_Remap_USART2          ((unsigned int)0x00000008)  /* USART2 Alternate Function mapping */
#define GPIO_PartialRemap_USART3   ((unsigned int)0x00140010)  /* USART3 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART3      ((unsigned int)0x00140030)  /* USART3 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM1     ((unsigned int)0x00160040)  /* TIM1 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM1        ((unsigned int)0x001600C0)  /* TIM1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM2    ((unsigned int)0x00180100)  /* TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2    ((unsigned int)0x00180200)  /* TIM2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM2        ((unsigned int)0x00180300)  /* TIM2 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM3     ((unsigned int)0x001A0800)  /* TIM3 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM3        ((unsigned int)0x001A0C00)  /* TIM3 Full Alternate Function mapping */
#define GPIO_Remap_TIM4            ((unsigned int)0x00001000)  /* TIM4 Alternate Function mapping */
#define GPIO_Remap1_CAN1           ((unsigned int)0x001D4000)  /* CAN Alternate Function mapping */
#define GPIO_Remap2_CAN1           ((unsigned int)0x001D6000)  /* CAN Alternate Function mapping */
#define GPIO_Remap_PD01            ((unsigned int)0x00008000)  /* PD01 Alternate Function mapping */
#define GPIO_Remap_TIM5CH4_LSI     ((unsigned int)0x00200001)  /* LSI connected to TIM5 Channel4 input capture for calibration */
#define GPIO_Remap_ADC1_ETRGINJ    ((unsigned int)0x00200002)  /* ADC1 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC1_ETRGREG    ((unsigned int)0x00200004)  /* ADC1 External Trigger Regular Conversion remapping */
#define GPIO_Remap_ADC2_ETRGINJ    ((unsigned int)0x00200008)  /* ADC2 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC2_ETRGREG    ((unsigned int)0x00200010)  /* ADC2 External Trigger Regular Conversion remapping */
#define GPIO_Remap_SWJ_NoJTRST     ((unsigned int)0x00300100)  /* Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST */
#define GPIO_Remap_SWJ_JTAGDisable ((unsigned int)0x00300200)  /* JTAG-DP Disabled and SW-DP Enabled */
#define GPIO_Remap_SWJ_Disable     ((unsigned int)0x00300400)  /* Full SWJ Disabled (JTAG-DP + SW-DP) */

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef enum
{
    Trigger_Rising = 0x08,
    Trigger_Falling = 0x0C,
    Trigger_Rising_Falling = 0x10
}IoEXTITriggerTdf;

typedef enum
{
    Mode_AIN = 0x00,
    Mode_IN_FLOATING = 0x04,
    Mode_IPD = 0x08,
    Mode_IPU = 0x18,
    Mode_Out_OD = 0x07,
    Mode_Out_PP = 0x03,
    Mode_AF_OD = 0x0f,
    Mode_AF_PP = 0x0b
}IOMode_TypeDef;

typedef struct {
    bool (*CommLock) (unsigned int *pwLock, bool blWaitLock);
    void (*CommUnLock) (unsigned int *pwLock);
    void (*RemapConfig) (unsigned int GPIO_Remap, FunctionalState NewState);
    int (*NvicUp) (unsigned int dwTrq, unsigned int dwPrePri, unsigned int dwSubPri);
    int (*NvicDown) (unsigned int dwTrq);
    int (*Read) (unsigned int byPort, unsigned int byChn);
    int (*Write) (unsigned int byPort, unsigned int byChn, unsigned int byIn);
    int (*WriteGPIO) (unsigned int byPort, unsigned int byIn);
    short int (*ReadGPIO) (unsigned int dwDev);
    short int (*ReadGpioTrue) (unsigned int dwDev);
    int (*Config) (unsigned int dwDev, unsigned int dwChn, unsigned int dwChnNum, IOMode_TypeDef Mode);
    int (*AddIrp) (unsigned int dwDev, unsigned int byChn, IoEXTITriggerTdf Trigger, void (*pfHandler)(unsigned int dwPare));
    int (*IrpDown) (unsigned int dwChnStart, unsigned int dwConNum);
    int (*IrpUp) (unsigned int dwChnStart, unsigned int dwConNum);
    int (*SetIrp) (unsigned int dwDev, unsigned int byChn, void (*pfHandler)(unsigned int dwPare));
    int (*AddAdc) (unsigned int dwDev, unsigned int byChn, unsigned char byAdcNo, unsigned char byAdcChn);
    unsigned short int (*GetAdc) (unsigned int dwDev, unsigned int byChn);
    int (*AddPwm) (unsigned int dwDev, unsigned int byChn, unsigned char byTimNo, unsigned char byTimChn);
    int (*OutPwm) (unsigned int dwDev, unsigned int byChn, unsigned short int wPwm, unsigned short int wPwmUp);
    int (*StopPwm) (unsigned int dwDev, unsigned int byChn);
}DevIoTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevIoTdf *DevIo;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevIoTdf *IoInit(void);

#endif