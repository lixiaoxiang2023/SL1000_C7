/******************************************************************************
文件名称：sysdata.h
文件目录：$PROJ_DIR$\user\app\inc
项目名称：
创建者：黄鹏
创建日期：20150717
程序用途：系统参数头文件
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/
#ifndef __SYSDATA_H
#define __SYSDATA_H
/******************************************************************************
Include files头文件
******************************************************************************/
#include "stm32f10x.h"
//#include "process.h"
#include "stm32_flash.h"
/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/


/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

typedef struct __stC7SysPar stC7SysPareTypeDef;
struct __stC7SysPar
{
    uint8_t            byVer;                   //参数版本
    bool               bInitPar;                //初始参数标志，TRUE 初始参数，FALSE 非初始参数
    uint32_t           wM1ToDripStep;           //M1电机从外光耦位置到滴样位，电机所走的步数
    uint32_t           wM3UpStep;               //M3染剂上升的步数
    uint32_t           wM2_MoveOne;             //M2电机拨动玻片一次所走步数，电机所走的步数
    uint32_t           OilVolume;               //镜油量
    uint32_t           wM1RestToDripStep;       //
    uint32_t           wM3DownToWashStep;       //下降到冲洗位
    uint16_t           wMpuX;                   //X
    uint16_t           wMpuY;                   //Y
    uint32_t           wTareVal;                //去皮重量
    uint32_t           wM3DownToFanStep;        //下降到风干位
    uint16_t           wHeatTemp;               //固化温度
    uint16_t           wHeatFanTemp;            //烘干温度
    uint16_t           wM4Steps;                //旋转步数
    uint16_t           wM2MoveSteps;                //移动一小步
};

//typedef struct __stC7SysDebugPar stC7SysDebugPareTypeDef;
//struct __stC7SysDebugPar
//{
//    uint8_t byVer;                  // 参数版本
//    bool    bM2SpeedDebug;        // 试纸上皮带到CIS开灯的时间调试，TRUE 调试开启，FALSE 调试关闭
//    bool    bCISTimDebug;           // CIS扫描时间调试，TRUE 调试开启，FALSE 调试关闭
//    bool    bBurn_in_Mark;          //老练模式标志, TRUE 老练开启，FALSE 老练关闭
//    uint8_t  byTempErr[4];           //记录四个温度传感器的故障状态，0，正常，1故障
//};



/******************************************************************************
外部变量声明
******************************************************************************/
extern stC7SysPareTypeDef        *g_pstC7SysPare;
//extern stC2SysDebugPareTypeDef   *g_pstC2SysDebugPare;


/*****************************************************************************
宏定义函数申明
******************************************************************************/


/*****************************************************************************
输出函数申明
******************************************************************************/
uint8_t SysDatainit(void);
void SaveSysData(void);

#endif
