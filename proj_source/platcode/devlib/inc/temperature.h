/******************************************************************************
文件名称：Temperature.h
文件目录：
项目名称：UC58
创建者：廖江伟
创建日期：2019.12.20
程序用途：温控模块控制组件
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/
#ifndef __TEMPERATURE__H
#define __TEMPERATURE__H


/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "stdbool.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct //控制引脚
{
    unsigned int *Ptc;
    unsigned int *Fan1;
    unsigned int *Fan2;
}stTempInit;

typedef enum     //开关状态
{
    eOff,
    eOn
}SwType;

typedef enum
{
    eHeat,
    eCool
}CtrlStaType;

typedef struct
{
	float    wKp;           // 比例常数
	float    wKi;           // 积分常数
	float    wKd;           // 微分常数
	int      iLastError;    // 前一次误差
	int      iPresentError; // 当前误差
	int      iSumError;     // 累积误差
}stPID;

//typedef struct
//{
//    int  TargetTemp; //目标温度
//    stPID stpPID;
//
//}stTempCtrl;






typedef struct
{
    void (*SetTargetTemp)(short signed int Temp);
    void (*SetCurTemp)(short signed int Temp);
    void (*SetPtcTemp)(short signed int Temp);

    void (*Setkp)(float kp);
    void (*Setki)(float ki);
    void (*Setkd)(float kd);

    short signed int (*GetTargetTemp)(void);
    short signed int (*GetCurTemp)(void);
    short signed int (*GetPtcTemp)(void);
    int  (*GetPrd)(void);

    void (*Start)(void);
    void (*Stop)(void);

    void (*CtrlProc)(void);
    void (*Cooling)(void);
    void (*OutputDebugInfo)(void);
}DevTempType;



/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
DevTempType *TempCtrlDevInit(stTempInit pstInit);
#endif