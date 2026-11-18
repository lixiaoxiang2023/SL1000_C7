/*******************************************************************************
文件名称：  usart.h
文件目录：  $PROJ_DIR$/usr/Drive/inc
项目名称：  事业一部技术平台
创建者：    胥强
创建日期：  2016.03.08
程序用途：  串口驱动文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __ADC_H
#define __ADC_H

#include <stdbool.h>

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define ADC_DEV_1                                (0)
#define ADC_DEV_2                                (1)
#define ADC_DEV_3                                (2)
#define ADC_DEV_NUM                              (3)

#define ADC_Channel_0                           ((uint8_t)0x00)
#define ADC_Channel_1                           ((uint8_t)0x01)
#define ADC_Channel_2                           ((uint8_t)0x02)
#define ADC_Channel_3                           ((uint8_t)0x03)
#define ADC_Channel_4                           ((uint8_t)0x04)
#define ADC_Channel_5                           ((uint8_t)0x05)
#define ADC_Channel_6                           ((uint8_t)0x06)
#define ADC_Channel_7                           ((uint8_t)0x07)
#define ADC_Channel_8                           ((uint8_t)0x08)
#define ADC_Channel_9                           ((uint8_t)0x09)
#define ADC_Channel_10                          ((uint8_t)0x0A)
#define ADC_Channel_11                          ((uint8_t)0x0B)
#define ADC_Channel_12                          ((uint8_t)0x0C)
#define ADC_Channel_13                          ((uint8_t)0x0D)
#define ADC_Channel_14                          ((uint8_t)0x0E)
#define ADC_Channel_15                          ((uint8_t)0x0F)
#define ADC_Channel_16                          ((uint8_t)0x10)
#define ADC_Channel_17                          ((uint8_t)0x11)

#define Prescaler_Div2                          (0)
#define Prescaler_Div4                          (1)
#define Prescaler_Div6                          (2)
#define Prescaler_Div8                          (3)

#define SampleTime_1Cycles5                     (0)
#define SampleTime_7Cycles5                     (1)
#define SampleTime_13Cycles5                    (2)
#define SampleTime_28Cycles5                    (3)
#define SampleTime_41Cycles5                    (4)
#define SampleTime_55Cycles5                    (5)
#define SampleTime_71Cycles5                    (6)
#define SampleTime_239Cycles5                   (7)

#define Mode_Independent                        (0)
#define DualMode_RegSimult_InjecSimult          (1)
#define DualMode_RegSimult_AlterTrig            (2)
#define DualMode_InjecSimult                    (5)
#define DualMode_RegSimult                      (6)
#define DualMode_Interl                         (7)
#define DualMode_AlterTrig                      (9)
#define TripleMode_RegSimult_InjecSimult        (11)
#define TripleMode_RegSimult_AlterTrig          (12)
#define TripleMode_InjecSimult                  (15)
#define TripleMode_RegSimult                    (16)
#define TripleMode_Interl                       (17)
#define TripleMode_AlterTrig                    (19)

#define DMAAccessMode_Disabled                  (0)
#define DMAAccessMode_1                         (1)
#define DMAAccessMode_2                         (2)
#define DMAAccessMode_3                         (3)

#define TwoSamplingDelay_5Cycles                (0)
#define TwoSamplingDelay_6Cycles                (1)
#define TwoSamplingDelay_7Cycles                (2)
#define TwoSamplingDelay_8Cycles                (3)
#define TwoSamplingDelay_9Cycles                (4)
#define TwoSamplingDelay_10Cycles               (5)
#define TwoSamplingDelay_11Cycles               (6)
#define TwoSamplingDelay_12Cycles               (7)
#define TwoSamplingDelay_13Cycles               (8)
#define TwoSamplingDelay_14Cycles               (9)
#define TwoSamplingDelay_15Cycles               (10)
#define TwoSamplingDelay_16Cycles               (11)
#define TwoSamplingDelay_17Cycles               (12)
#define TwoSamplingDelay_18Cycles               (13)
#define TwoSamplingDelay_19Cycles               (14)
#define TwoSamplingDelay_20Cycles               (15)

#define ADC_ExtTrigConv_T1_CC1                  (0)
#define ADC_ExtTrigConv_T1_CC2                  (1)
#define ADC_ExtTrigConv_T1_CC3                  (2)
#define ADC_ExtTrigConv_T2_CC2                  (3)
#define ADC_ExtTrigConv_T2_CC3                  (4)
#define ADC_ExtTrigConv_T2_CC4                  (5)
#define ADC_ExtTrigConv_T2_TRGO                 (6)
#define ADC_ExtTrigConv_T3_CC1                  (7)
#define ADC_ExtTrigConv_T3_TRGO                 (8)
#define ADC_ExtTrigConv_T4_CC4                  (9)
#define ADC_ExtTrigConv_T5_CC1                  (10)
#define ADC_ExtTrigConv_T5_CC2                  (11)
#define ADC_ExtTrigConv_T5_CC3                  (12)
#define ADC_ExtTrigConv_T8_CC1                  (13)
#define ADC_ExtTrigConv_T8_TRGO                 (14)
#define ADC_ExtTrigConv_Exti_Line11             (15)

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    void (*Config)      (unsigned int dwDev, bool blScanMode);
    void (*ConfigComm)  ( unsigned char byMode, unsigned char byPrescaler, unsigned char byDmaMode, unsigned char byTwoSamplingDelay);
    int (*ClrChn)       (unsigned int dwDev);
    int (*AddChn)       (unsigned int dwDev, unsigned char byChn, unsigned char bySmpTim);
    void (*ContinuousAdOn) (unsigned int dwDev);
    void (*ContinuousAdOff)(unsigned int dwDev);
    bool (*IsBusy)      (unsigned int dwDev);
#if(CONFIG_ADC_DMA)
    int (*ConfigDma)    (unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaStream, unsigned int dwChn, unsigned int dwMode);
    int (*ReadMultiple) (unsigned int dwDev, unsigned short int *pwOut);
#if(CONFIG_ADC_EXT_TRIG)
    int (*ConfigExtTrig)(unsigned int dwDev, unsigned int dwExtTrig, unsigned short int *pwOut, \
                         unsigned int len, void (*Handler)(unsigned int dwIn));
    int (*StartExtTrig) (unsigned int dwDev);
    int (*StopExtTrig)  (unsigned int dwDev);
#endif
#endif
    unsigned short int (*ReadOne) (unsigned int dwDev, unsigned int dwChn, unsigned char bySmpTim, unsigned int dwDoTims);
}DevAdcTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevAdcTdf* DevAdc;
/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevAdcTdf *AdcInit(void);

#endif