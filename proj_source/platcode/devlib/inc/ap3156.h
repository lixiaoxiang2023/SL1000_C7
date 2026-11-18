/*******************************************************************************
文件名称：   ap3156.h
文件目录：   $PROJ_DIR$/.../code/dev/plat/src
项目名称：   事业一部技术平台
创建者：     陆作沛
创建日期：   20140520
程序用途：   ap3156头文件

版权声明：   Copyright urit Co.,Ltd.All Right Reserved

日期：20140522
第一次修改：完成编程格式规范//为解决什么问题
修改人：陆作沛
修改内容：格式规范

日期：20160421 胥强
加入驱动内容进技术平台
*******************************************************************************/
#ifndef __AP3156_H
#define __AP3156_H

#include <stdbool.h>
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef enum
{
	ApLedChAll  = 10,
    ApLedCh123  = 11,
    ApLedCh456  = 12,
    ApLedCh1234 = 13,
    ApLedCh56   = 14,
    ApLedCh12345 = 15,
    ApLedCh6    = 16,
    ApLedCh5    = 17,
    ApLedCh4    = 18,
    ApLedCh3    = 19,
    ApLedCh2    = 20,
	ApLedCh1    = 21
}emLedChTdf;

typedef enum
{
	ApCurLev0 = 36,//0ma
	ApCurLev1 = 35,//0.05
	ApCurLev2 = 34,//0.5
	ApCurLev3 = 33,//1m
	ApCurLev4 = 32,//2ma
	ApCurLev5 = 31,//3.6ma
	ApCurLev6 = 30,
	ApCurLev7 = 29,
	ApCurLev8 = 28,
	ApCurLev9 = 27,
	ApCurLev10 = 26,
	ApCurLev11 = 25,
	ApCurLev12 = 24,
	ApCurLev13 = 23,
	ApCurLev14 = 22,
	ApCurLev15 = 21,
	ApCurLev16 = 20,
	ApCurLev17 = 19,
	ApCurLev18 = 18,
	ApCurLev19 = 17,
	ApCurLev20 = 16,
	ApCurLev21 = 15,
	ApCurLev22 = 14,
	ApCurLev23 = 13,
	ApCurLev24 = 12,
	ApCurLev25 = 11,
	ApCurLev26 = 10,
	ApCurLev27 = 9,
	ApCurLev28 = 8,
	ApCurLev29 = 7,
	ApCurLev30 = 6,
	ApCurLev31 = 5,
	ApCurLev32 = 4,
	ApCurLev33 = 3,
	ApCurLev34 = 2,
	ApCurLev35 = 1 //20ma
}emCurLevTdf;

typedef struct {
    unsigned int *pwdSda;   //驱动器的控制引脚
    unsigned char byTimNo;
}Ap3156InitTdf;

typedef struct {
    int (*Write) (emLedChTdf emLedCh, emCurLevTdf emCurLev, bool blIsRetNow);
    int (*GetCurrent) (emCurLevTdf emCurLev);
}DevAp3156Tdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
外部全局函数声明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevAp3156Tdf *Ap3156Init(Ap3156InitTdf *pstInit);

#endif
