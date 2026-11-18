/******************************************************************************
文件名称：cis.h
文件目录：$PROJ_DIR$\user\Drive\inc
项目名称：
创建者：胥强
创建日期：2013、01、25
程序用途：
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/
#ifndef __CIS_H
#define __CIS_H

/******************************************************************************
Include files头文件
******************************************************************************/
#include "stm32f2xx.h"


/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
/**************************常量宏定义**************************************/
#define MAX_CIS_LED_ON_TIM                                      77
#define MIN_CIS_LED_ON_TIM                                      7

#define CIS_DATA_WIDTH                                          40
#define CIS_DATA_LENTH                                          1216

/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

typedef struct
{
        uint16_t wRcal;
        uint16_t wGcal;
        uint16_t wBcal;
        uint16_t wIRcal;
        uint16_t wYGcal;
}stCisPareTypeDef;

/*****************************************************************************
输出函数申明
******************************************************************************/
void CisClean(void);
void CisScanOneTim(void);
int  CisGetBusyState(void);
int  CisInit(void);
void CisGetLightBanlanceWhite(void);
void CisGetLightBanlanceBlack(void);
int  CisGetLightBanlance(void);
int  CisCheakStart(void);
void CisCalcLightBanlance(uint8_t byLineProc);
void CisGetCal(stCisPareTypeDef *pstPare,uint8_t byLine);
void cisHwInit(void);
int CisLightAdj(uint16_t wLightVal);
void CisGetCalRgb(stCisPareTypeDef *pstPare,uint8_t byLine);

#endif