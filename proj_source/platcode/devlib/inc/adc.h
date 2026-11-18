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
#define PRE_PCLK2_DIV2                         0
#define PRE_PCLK2_DIV4                         1
#define PRE_PCLK2_DIV6                         2
#define PRE_PCLK2_DIV8                         3

#define Mode_Independent                       0
#define Mode_RegInjecSimult                    1
#define Mode_RegSimult_AlterTrig               2
#define Mode_InjecSimult_FastInterl            3
#define Mode_InjecSimult_SlowInterl            4
#define Mode_InjecSimult                       5
#define Mode_RegSimult                         6
#define Mode_FastInterl                        7
#define Mode_SlowInterl                        8
#define Mode_AlterTrig                         9

#define SampleTime_1Cycles5                    0
#define SampleTime_7Cycles5                    1
#define SampleTime_13Cycles5                   2
#define SampleTime_28Cycles5                   3
#define SampleTime_41Cycles5                   4
#define SampleTime_55Cycles5                   5
#define SampleTime_71Cycles5                   6
#define SampleTime_239Cycles5                  7

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    void (*Config) (unsigned int dwDev, unsigned char byDualMode, bool blScanMode);
    int (*ClrChn) (unsigned int dwDev);
    int (*AddChn) (unsigned int dwDev, unsigned char byChn, unsigned char bySmpTim);
    void (*ContinuousAdOn) (unsigned int dwDev);
    void (*ContinuousAdOff) (unsigned int dwDev);
    bool (*IsBusy) (unsigned int dwDev);
#if(CONFIG_ADC_DMA)
    int (*ConfigDma) (unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn);
    int (*ReadMultiple) (unsigned int dwDev, unsigned short int *pwOut);
#endif
    unsigned short int (*ReadOne) (unsigned int dwDev, unsigned int dwChn, unsigned char bySmpTim, unsigned int dwDoTims);

#if(CONFIG_ADC_SET_PRE)
    void (*SetPRE) (unsigned int dwPRE);
#endif
} DevAdcTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevAdcTdf* DevAdc;
/*******************************************************************************
输出函数申明
*******************************************************************************/
extern const DevAdcTdf *AdcInit(void);

#endif