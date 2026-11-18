/*****************************************************************************************
文件名称：tlc5922.h
目录：$PROJ_DIR$/usr/dev
项目名称：urit-1600尿液分析仪
创建者：胥强
创建日期：20120816
程序用途：
	LED驱动器底层驱动程序

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
2012/8/114:由胥强创建

*****************************************************************************************/

#ifndef __TLC5922_H_
#define __TLC5922_H_

/*****************************************************************************************
Include file
*****************************************************************************************/
#include "stm32f2xx.h"

/*****************************************************************************************
Constant Definition宏定义
*****************************************************************************************/
#define LED_TLC5922_MAX_CUR                     80

#define WR_CUR_HEAD                             LED_5922_modeH
#define WR_ON_OFF_HEAD                          LED_5922_modeL

#define TLC5922_LED_ON                          1
#define TLC5922_LED_OFF                         0

#define LED_5922_CH0                            (uint16_t)0x0001
#define LED_5922_CH1                            (uint16_t)0x0002
#define LED_5922_CH2                            (uint16_t)0x0004
#define LED_5922_CH3                            (uint16_t)0x0008
#define LED_5922_CH4                            (uint16_t)0x0010
#define LED_5922_CH5                            (uint16_t)0x0020
#define LED_5922_CH6                            (uint16_t)0x0040
#define LED_5922_CH7                            (uint16_t)0x0080
#define LED_5922_CH8                            (uint16_t)0x0100
#define LED_5922_CH9                            (uint16_t)0x0200
#define LED_5922_CH10                           (uint16_t)0x0400
#define LED_5922_CH11                           (uint16_t)0x0800
#define LED_5922_CH12                           (uint16_t)0x1000
#define LED_5922_CH13                           (uint16_t)0x2000
#define LED_5922_CH14                           (uint16_t)0x4000
#define LED_5922_CH15                           (uint16_t)0x8000

#define LED_5922_CH_ALL                         (uint16_t)(LED_5922_CH0 | \
                                                LED_5922_CH1 | LED_5922_CH2 |\
                                                LED_5922_CH3 | LED_5922_CH4 |\
                                                LED_5922_CH5 | LED_5922_CH6 |\
                                                LED_5922_CH7 | LED_5922_CH8 |\
                                                LED_5922_CH9 | LED_5922_CH10 |\
                                                LED_5922_CH11 | LED_5922_CH12 |\
                                                LED_5922_CH13 | LED_5922_CH14 |\
                                                LED_5922_CH15)
/*****************************************************************************************
数据结构与数据类型定义
*****************************************************************************************/
typedef struct
{
        uint8_t byCurrent;
        uint8_t byLedOnOff;
}LedSetDef;

typedef struct
{
        LedSetDef Chanl_0;
        LedSetDef Chanl_1;
        LedSetDef Chanl_2;
        LedSetDef Chanl_3;
        LedSetDef Chanl_4;
        LedSetDef Chanl_5;
        LedSetDef Chanl_6;
        LedSetDef Chanl_7;
        LedSetDef Chanl_8;
        LedSetDef Chanl_9;
        LedSetDef Chanl_10;
        LedSetDef Chanl_11;
        LedSetDef Chanl_12;
        LedSetDef Chanl_13;
        LedSetDef Chanl_14;
        LedSetDef Chanl_15;
}LedChanelTypeDef;

/*****************************************************************************************
输出全局变量申明
*****************************************************************************************/

/*****************************************************************************************
输出函数申明
*****************************************************************************************/
void Tlc5922LedDataDeinit(LedChanelTypeDef *pstLedChanl);
uint32_t Tlc5922LedWrite(LedChanelTypeDef *pstLedChanl,uint16_t wUsedChanl);
void Tlc5922LedRead(LedChanelTypeDef *pstLedChanl);
void Tlc5922LedWriteOnOff(LedChanelTypeDef *pstLedChanl,uint16_t wUsedChanl);
void Tlc5922LedCmd(FunctionalState emCmd);
void Tlc5922LedInit(void);


#define Tlc5922LedOpen(x)                                      Tlc5922LedInit(x)
#endif