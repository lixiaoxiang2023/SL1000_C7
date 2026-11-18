/*****************************************************************************
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
******************************************************************************/
#ifndef __IO_H
#define __IO_H

/******************************************************************************
Include files头文件
******************************************************************************/
#include <stdbool.h>

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
#define PA      0
#define PB      1
#define PC      2
#define PD      3
#define PE      4
#define PF      5
#define PG      6
#define PH      7
#define PI      8
#define PJ      9
#define PK      10

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

#define GPIO_PinSource0            ((unsigned char)0x00)
#define GPIO_PinSource1            ((unsigned char)0x01)
#define GPIO_PinSource2            ((unsigned char)0x02)
#define GPIO_PinSource3            ((unsigned char)0x03)
#define GPIO_PinSource4            ((unsigned char)0x04)
#define GPIO_PinSource5            ((unsigned char)0x05)
#define GPIO_PinSource6            ((unsigned char)0x06)
#define GPIO_PinSource7            ((unsigned char)0x07)
#define GPIO_PinSource8            ((unsigned char)0x08)
#define GPIO_PinSource9            ((unsigned char)0x09)
#define GPIO_PinSource10           ((unsigned char)0x0A)
#define GPIO_PinSource11           ((unsigned char)0x0B)
#define GPIO_PinSource12           ((unsigned char)0x0C)
#define GPIO_PinSource13           ((unsigned char)0x0D)
#define GPIO_PinSource14           ((unsigned char)0x0E)
#define GPIO_PinSource15           ((unsigned char)0x0F)

#define GPIO_AF_RTC_50Hz            ((unsigned char)0x00)  /* RTC_50Hz Alternate Function mapping */
#define GPIO_AF_MCO                 ((unsigned char)0x00)  /* MCO (MCO1 and MCO2) Alternate Function mapping */
#define GPIO_AF_TAMPER              ((unsigned char)0x00)  /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
#define GPIO_AF_SWJ                 ((unsigned char)0x00)  /* SWJ (SWD and JTAG) Alternate Function mapping */
#define GPIO_AF_TRACE               ((unsigned char)0x00)  /* TRACE Alternate Function mapping */
#define GPIO_AF0_TIM2               ((unsigned char)0x00)  /* TIM2 Alternate Function mapping STM32F446xx*/

#define GPIO_AF_TIM1                ((unsigned char)0x01)  /* TIM1 Alternate Function mapping */
#define GPIO_AF_TIM2                ((unsigned char)0x01)  /* TIM2 Alternate Function mapping */
#define GPIO_AF_LPTIM               ((unsigned char)0x01)  /* LPTIM Alternate Function mapping STM32F410xx STM32F413_423xx*/


#define GPIO_AF_TIM3                ((unsigned char)0x02)  /* TIM3 Alternate Function mapping */
#define GPIO_AF_TIM4                ((unsigned char)0x02)  /* TIM4 Alternate Function mapping */
#define GPIO_AF_TIM5                ((unsigned char)0x02)  /* TIM5 Alternate Function mapping */

#define GPIO_AF_TIM8                ((unsigned char)0x03)  /* TIM8 Alternate Function mapping */
#define GPIO_AF_TIM9                ((unsigned char)0x03)  /* TIM9 Alternate Function mapping */
#define GPIO_AF_TIM10               ((unsigned char)0x03)  /* TIM10 Alternate Function mapping */
#define GPIO_AF_TIM11               ((unsigned char)0x03)  /* TIM11 Alternate Function mapping */
#define GPIO_AF3_CEC                ((unsigned char)0x03)  /* CEC Alternate Function mapping STM32F446xx*/
#define GPIO_AF3_DFSDM2             ((unsigned char)0x03)  /* DFSDM2 Alternate Function mapping STM32F413_423xx*/

#define GPIO_AF_I2C1                ((unsigned char)0x04)  /* I2C1 Alternate Function mapping */
#define GPIO_AF_I2C2                ((unsigned char)0x04)  /* I2C2 Alternate Function mapping */
#define GPIO_AF_I2C3                ((unsigned char)0x04)  /* I2C3 Alternate Function mapping */
#define GPIO_AF4_CEC                ((unsigned char)0x04)  /* CEC Alternate Function mapping STM32F446xx*/
#define GPIO_AF_FMPI2C              ((unsigned char)0x04)  /* FMPI2C Alternate Function mapping STM32F410xx STM32F412xG STM32F413_423xx STM32F446xx*/

#define GPIO_AF_SPI1                ((unsigned char)0x05)  /* SPI1 Alternate Function mapping */
#define GPIO_AF_SPI2                ((unsigned char)0x05)  /* SPI2/I2S2 Alternate Function mapping */
#define GPIO_AF5_SPI3               ((unsigned char)0x05)  /* SPI3/I2S3 Alternate Function mapping (Only for STM32F411xE and STM32F413_423xx Devices) */
#define GPIO_AF_SPI4                ((unsigned char)0x05)  /* SPI4/I2S4 Alternate Function mapping for STMF4xxxxx*/
#define GPIO_AF_SPI5                ((unsigned char)0x05)  /* SPI5 Alternate Function mapping      STMF4xxxxx*/
#define GPIO_AF_SPI6                ((unsigned char)0x05)  /* SPI6 Alternate Function mapping      STMF4xxxxx*/

#define GPIO_AF_SPI3                ((unsigned char)0x06)  /* SPI3/I2S3 Alternate Function mapping */
#define GPIO_AF6_SPI1               ((unsigned char)0x06)  /* SPI1 Alternate Function mapping (Only for STM32F410xx Devices) */
#define GPIO_AF6_SPI2               ((unsigned char)0x06)  /* SPI2 Alternate Function mapping (Only for STM32F410xx/STM32F411xE Devices) */
#define GPIO_AF6_SPI4               ((unsigned char)0x06)  /* SPI4 Alternate Function mapping (Only for STM32F411xE Devices) */
#define GPIO_AF6_SPI5               ((unsigned char)0x06)  /* SPI5 Alternate Function mapping (Only for STM32F410xx/STM32F411xE Devices) */
#define GPIO_AF_SAI1                ((unsigned char)0x06)  /* SAI1 Alternate Function mapping      STMF4xxxxx*/
#define GPIO_AF_I2S2ext             ((unsigned char)0x06)  /* I2S2ext_SD Alternate Function mapping (only for STM32F412xG and STM32F413_423xx Devices) */
#define GPIO_AF6_DFSDM1             ((unsigned char)0x06)  /* DFSDM1 Alternate Function mapping STM32F413_423xx STM32F412xG*/
#define GPIO_AF6_DFSDM2             ((unsigned char)0x06)  /* DFSDM2 Alternate Function mapping STM32F413_423xx*/

#define GPIO_AF_USART1              ((unsigned char)0x07)  /* USART1 Alternate Function mapping */
#define GPIO_AF_USART2              ((unsigned char)0x07)  /* USART2 Alternate Function mapping */
#define GPIO_AF_USART3              ((unsigned char)0x07)  /* USART3 Alternate Function mapping */
#define GPIO_AF7_SPI3               ((unsigned char)0x07)  /* SPI3/I2S3ext Alternate Function mapping STMF4xxxxx*/
#define GPIO_AF7_DFSDM2             ((unsigned char)0x07)  /* DFSDM2 Alternate Function mapping     STM32F413_423xx*/
#define GPIO_AF7_SAI1               ((unsigned char)0x07)  /* SAI1 Alternate Function mapping       STM32F413_423xx*/

#define GPIO_AF_UART4               ((unsigned char)0x08)  /* UART4 Alternate Function mapping */
#define GPIO_AF_UART5               ((unsigned char)0x08)  /* UART5 Alternate Function mapping */
#define GPIO_AF_USART6              ((unsigned char)0x08)  /* USART6 Alternate Function mapping */
#define GPIO_AF_UART7               ((unsigned char)0x08)  /* UART7 Alternate Function mapping  STM32F413_423xx*/
#define GPIO_AF_UART8               ((unsigned char)0x08)  /* UART8 Alternate Function mapping  STM32F413_423xx*/
#define GPIO_AF8_USART3             ((unsigned char)0x08)  /* USART3 Alternate Function mapping STM32F413_423xx STM32F412xG*/
#define GPIO_AF8_DFSDM1             ((unsigned char)0x08)  /* DFSDM Alternate Function mapping  STM32F413_423xx STM32F412xG*/
#define GPIO_AF8_CAN1               ((unsigned char)0x08)  /* CAN1 Alternate Function mapping   STM32F413_423xx STM32F412xG*/
#define GPIO_AF8_SAI2               ((unsigned char)0x08)  /* SAI2 Alternate Function mapping STM32F412xG STM32F413_423xx*/
#define GPIO_AF_SPDIF               ((unsigned char)0x08)   /* SPDIF Alternate Function mapping STM32F412xG STM32F413_423xx*/

#define GPIO_AF_CAN1                ((unsigned char)0x09)  /* CAN1 Alternate Function mapping */
#define GPIO_AF_CAN2                ((unsigned char)0x09)  /* CAN2 Alternate Function mapping */
#define GPIO_AF_TIM12               ((unsigned char)0x09)  /* TIM12 Alternate Function mapping */
#define GPIO_AF_TIM13               ((unsigned char)0x09)  /* TIM13 Alternate Function mapping */
#define GPIO_AF_TIM14               ((unsigned char)0x09)  /* TIM14 Alternate Function mapping */
#define GPIO_AF9_I2C2               ((unsigned char)0x09)  /* I2C2 Alternate Function mapping (Only for STM32F401xx/STM32F410xx/STM32F411xE/STM32F412xG/STM32F413_423xx Devices) */
#define GPIO_AF9_I2C3               ((unsigned char)0x09)  /* I2C3 Alternate Function mapping (Only for STM32F401xx/STM32F411xE/STM32F412xG and STM32F413_423xx Devices) */
#define GPIO_AF9_SAI2               ((unsigned char)0x09)  /* SAI2 Alternate Function mapping STM32F446xx*/
#define GPIO_AF9_LTDC               ((unsigned char)0x09)  /* LTDC Alternate Function mapping STMF4xxxxx*/
#define GPIO_AF9_QUADSPI            ((unsigned char)0x09)  /* QuadSPI Alternate Function mapping STM32F412xG || STM32F413_423xx || STM32F446xx || STM32F469_479xx */
#define GPIO_AF9_FMPI2C             ((unsigned char)0x09)  /* FMPI2C Alternate Function mapping (Only for STM32F410xx Devices) */

#define GPIO_AF_OTG_FS              ((unsigned char)0xA)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF_OTG_HS              ((unsigned char)0xA)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF10_SAI2              ((unsigned char)0x0A)  /* SAI2 Alternate Function mapping STM32F446xx*/
#define GPIO_AF10_QUADSPI           ((unsigned char)0x0A)  /* QuadSPI Alternate Function mapping STM32F412xG || STM32F413_423xx || STM32F446xx || STM32F469_479xx*/
#define GPIO_AF10_FMC               ((unsigned char)0xA)  /* FMC Alternate Function mapping   STM32F412xG || STM32F413_423xx*/
#define GPIO_AF10_DFSDM1            ((unsigned char)0xA) /* DFSDM Alternate Function mapping  STM32F412xG || STM32F413_423xx*/
#define GPIO_AF10_DFSDM2            ((unsigned char)0x0A)  /* DFSDM2 Alternate Function mapping */
#define GPIO_AF10_SAI1              ((unsigned char)0x0A)  /* SAI1 Alternate Function mapping   */

#define GPIO_AF_ETH                 ((unsigned char)0x0B)  /* ETHERNET Alternate Function mapping */
#define GPIO_AF11_UART4             ((unsigned char)0x0B)  /* UART4 Alternate Function mapping  STM32F413_423xx*/
#define GPIO_AF11_UART5             ((unsigned char)0x0B)  /* UART5 Alternate Function mapping  STM32F413_423xx*/
#define GPIO_AF11_UART9             ((unsigned char)0x0B)  /* UART9 Alternate Function mapping  STM32F413_423xx*/
#define GPIO_AF11_UART10            ((unsigned char)0x0B)  /* UART10 Alternate Function mapping STM32F413_423xx*/
#define GPIO_AF11_CAN3              ((unsigned char)0x0B)  /* CAN3 Alternate Function mapping   STM32F413_423xx*/

#define GPIO_AF_FSMC                ((unsigned char)0xC)  /* FSMC Alternate Function mapping */
#define GPIO_AF_OTG_HS_FS           ((unsigned char)0xC)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF_SDIO                ((unsigned char)0xC)  /* SDIO Alternate Function mapping */
#define GPIO_AF_FMC                 ((unsigned char)0xC)  /* FMC Alternate Function mapping                      */

#define GPIO_AF_DCMI                ((unsigned char)0x0D)  /* DCMI Alternate Function mapping */
#define GPIO_AF_DSI                 ((unsigned char)0x0D)  /* DSI Alternate Function mapping */

#define GPIO_AF_LTDC                ((unsigned char)0x0E)  /* LCD-TFT Alternate Function mapping */
#define GPIO_AF14_RNG               ((unsigned char)0x0E)  /* RNG Alternate Function mapping  */

#define GPIO_AF_EVENTOUT            ((unsigned char)0x0F)  /* EVENTOUT Alternate Function mapping */

#define GPIO_Mode_AIN               GPIO_Mode_AN

#define GPIO_AF_OTG1_FS             GPIO_AF_OTG_FS
#define GPIO_AF_OTG2_HS             GPIO_AF_OTG_HS
#define GPIO_AF_OTG2_FS             GPIO_AF_OTG_HS_FS

/******************************************************************************
数据结构与数据类型定义
******************************************************************************/
typedef enum
{
    Trigger_Rising = 0x08,
    Trigger_Falling = 0x0C,
    Trigger_Rising_Falling = 0x10
}IoEXTITriggerTdf;

typedef enum
{
    Mode_IN   = 0x00, /*!< GPIO Input Mode */
    Mode_OUT  = 0x01, /*!< GPIO Output Mode */
    Mode_AF   = 0x02, /*!< GPIO Alternate function Mode */
    Mode_AN   = 0x03  /*!< GPIO Analog Mode */
}IOMode_TypeDef;

typedef enum
{
    OType_PP = 0x00,
    OType_OD = 0x01
}IOOType_TypeDef;

typedef enum
{
    Speed_2MHz   = 0x00, /*!< Low speed */
    Speed_25MHz  = 0x01, /*!< Medium speed */
    Speed_50MHz  = 0x02, /*!< Fast speed */
    Speed_100MHz = 0x03  /*!< High speed on 30 pF (80 MHz Output max speed on 15 pF) */
}IOSpeed_TypeDef;

typedef enum
{
    PuPd_NOPULL = 0x00,
    PuPd_UP     = 0x01,
    PuPd_DOWN   = 0x02
}IOPuPd_TypeDef;

typedef struct {
    bool (*CommLock) (unsigned int *pwLock, bool blWaitLock);
    void (*CommUnLock) (unsigned int *pwLock);
    void (*PinAFConfig) (GPIO_TypeDef* GPIOx, unsigned short int GPIO_PinSource, unsigned char GPIO_AF);
    int (*NvicUp) (unsigned int dwTrq, unsigned int dwPrePri, unsigned int dwSubPri);
    int (*NvicDown) (unsigned int dwTrq);
    int (*Read) (unsigned int byPort, unsigned int byChn);
    int (*Write) (unsigned int byPort, unsigned int byChn, unsigned int byIn);
    int (*WriteGPIO) (unsigned int byPort, unsigned int byIn);
    short int (*ReadGPIO) (unsigned int dwDev);
    short int (*ReadGpioTrue) (unsigned int dwDev);
    int (*Config) (unsigned int dwDev, unsigned int dwChn, unsigned int dwChnNum, IOMode_TypeDef Mode, IOOType_TypeDef OType, \
                IOSpeed_TypeDef Speed, IOPuPd_TypeDef PuPd);

#if(CONFIG_IO_IRP)
    int (*IrpPri) (unsigned int dwChn, unsigned int dwPri);
    int (*AddIrp) (unsigned int dwDev, unsigned int byChn, IoEXTITriggerTdf Trigger, void (*pfHandler)(unsigned int dwPare));
    int (*IrpDown) (unsigned int dwChnStart, unsigned int dwConNum);
    int (*IrpUp) (unsigned int dwChnStart, unsigned int dwConNum);
    int (*SetIrp) (unsigned int dwDev, unsigned int byChn, void (*pfHandler)(unsigned int dwPare));
#endif

#if(CONFIG_IO_ADC)
    int (*AddAdc) (unsigned int dwDev, unsigned int byChn, unsigned char byAdcNo, unsigned char byAdcChn);
    unsigned short int (*GetAdc) (unsigned int dwDev, unsigned int byChn);
#endif

#if(CONFIG_IO_PWM)
    int (*AddPwm) (unsigned int dwDev, unsigned int byChn, unsigned char byTimNo, unsigned char byTimChn);
    int (*OutPwm) (unsigned int dwDev, unsigned int byChn, unsigned short int wPwm, unsigned short int wPwmUp);
    int (*StopPwm) (unsigned int dwDev, unsigned int byChn);
#endif
}DevIoTdf;

/***************************************************************************
宏定义函数申明
******************************************************************************/
extern const DevIoTdf *DevIo;

/*****************************************************************************
输出函数申明
******************************************************************************/
const DevIoTdf *IoInit(void);

#endif