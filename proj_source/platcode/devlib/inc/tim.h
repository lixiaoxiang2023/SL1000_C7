/*******************************************************************************
文件名称：   tim.h
文件目录：   $PROJ_DIR$/usr/app/inc
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.10
程序用途：   定时器头文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：

*******************************************************************************/
#ifndef __TIM_H
#define __TIM_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define TIM_DEV_1                                   (0)
#define TIM_DEV_2                                   (1)
#define TIM_DEV_3                                   (2)
#define TIM_DEV_4                                   (3)
#define TIM_DEV_5                                   (4)
#define TIM_DEV_6                                   (5)
#define TIM_DEV_7                                   (6)
#define TIM_DEV_8                                   (7)
#define TIM_DEV_9                                   (8)
#define TIM_DEV_10                                  (9)
#define TIM_DEV_11                                  (10)
#define TIM_DEV_12                                  (11)
#define TIM_DEV_13                                  (12)
#define TIM_DEV_14                                  (13)
#define TIM_DEV_NUM                                 (14)

#define TIM_CHN_1                                   (1)
#define TIM_CHN_2                                   (2)
#define TIM_CHN_3                                   (3)
#define TIM_CHN_4                                   (4)

#define ICPSC_DIV1                                  (0)
#define ICPSC_DIV2                                  (1)
#define ICPSC_DIV3                                  (2)

#define EncoderMode_TI1                             (1)
#define EncoderMode_TI2                             (2)
#define EncoderMode_TI12                            (3)

#define ICPolarity_Rising                           (0)
#define ICPolarity_Falling                          (1)
    
/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    int (*Config) (unsigned int dwDev, unsigned int dwPrescaler, unsigned int dwPeriod);
    int (*SetArr) (unsigned int dwDev, unsigned short int wArr);
    int (*Delay) (unsigned int dwDev, unsigned short int wNus);
    int (*TimEvent) (unsigned int dwDev, unsigned short int wTim, void (*pvHandler)(unsigned int), unsigned int byPare);
    int (*Stop) (unsigned int dwDev);
    int (*ConfigPwm) (unsigned int dwDev, unsigned int dwPwmChn);
    int (*SetArrPwm) (unsigned int dwDev, unsigned int dwPwmChn, unsigned short int wArr, unsigned short int wCcr);
    int (*StartPwm) (unsigned int dwDev, unsigned int dwPwmChn, void (*pvHandler)(unsigned int), unsigned int byDat);
    int (*StopPwm) (unsigned int dwDev, unsigned int dwPwmChn);
#if(CONFIG_TIM1_CCMR|CONFIG_TIM8_CCMR)
    int (*ConfigCcmr) (unsigned int dwDev, unsigned int dwPwmChn, bool blIsPolarityRising, unsigned int dwICFilter, unsigned int dwIcpscDiv);
    int (*StartCcmr) (unsigned int dwDev, unsigned int dwPwmChn, void (*pvHandler)(unsigned int), unsigned int dwPare);
    int (*StopCcmr) (unsigned int dwDev, unsigned int dwPwmChn);
    unsigned short int (*ReadCcrm) (unsigned int dwDev, unsigned int dwChn);
#endif
#if(CONFIG_ENCODE)
    int (*ConfigEcd) (unsigned int dwDev, unsigned int dwMode, unsigned int IC1Polarity, unsigned int IC2Polarity);
    int (*EcdCnt) (unsigned int dwDev);
    int (*EcdStart) (unsigned int dwDev);
#endif
    int (*RegHandler) (unsigned int dwDev, void (*pvHandler)(unsigned int), unsigned int dwDat);
#if(CONFIG_TIM_TRGO)
    int (*ConfigTrgo)(unsigned int dwDev, unsigned int dwChn);
    int (*StartTrgo) (unsigned int dwDev, unsigned int dwChn);
    int (*StopTrgo)  (unsigned int dwDev, unsigned int dwChn);
#endif
}DevTimTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevTimTdf *DevTim;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevTimTdf *TimInit(void);

const DevTimTdf *Tim2Init(void);
#endif
