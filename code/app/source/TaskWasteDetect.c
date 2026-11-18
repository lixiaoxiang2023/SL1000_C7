/*****************************************************************************
文件名称：TaskWasteDetect.c
文件目录：$PROJ_DIR$/usr/app/source
项目名称：
创建者：
创建日期：
程序用途：
温度控制任务

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息

******************************************************************************/

/******************************************************************************
Include files头文件
******************************************************************************/
#include "devinit.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <math.h>
#include "sensor.h"
#include "TaskWasteDetect.h"
#include "TaskMoveStrip.h"
#include "iosrc.h"
#include "MotorCtrl.h"
#include "SysData.h"
#include "clients/clients_def.h"
#include "TaskTempModule.h"
/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/

/******************************************************************************
Global Variables Definition全局变量定义
******************************************************************************/

#define LED_ON                    BIT_GPIOE_ODR_ODR1
//#define HEAT                      BIT_GPIOE_ODR_ODR5
/******************************************************************************
外部全局变量申明
******************************************************************************/
//uint16_t wX_Offset = 0;
//uint16_t wY_Offset = 0;
/******************************************************************************
内部数据结构与数据类型定义
******************************************************************************/
//#if(CONFIG_MPU6050)
//    const DevMpu6050Tdf *DevMpu6050;
//#endif
/******************************************************************************
内部全局变量定义
******************************************************************************/

/******************************************************************************
内部函数申明
******************************************************************************/

/*******************************************************************************
*函数功能：
*   @ LED状态
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
void LedCheckTask(void *pvPare)
{
    bool bBoxSta = false;
//    static uint16_t wMpuData_X = 0;
//    static uint16_t wMpuData_Y = 0;
//    MPU6050_Angle DataSum;
    while (1) {
//        DataSum.X_Angle = 0;
//        DataSum.Y_Angle = 0;
//        for(int i=0;i<5;i++)
//        {
//            vTaskDelay(5);
//            DevMpu6050->MPU6050_Get_Angle(&data);
//            DataSum.X_Angle += data.X_Angle;
//            DataSum.Y_Angle += data.Y_Angle;
//        }
//        g_stC7.stOut.wMpuSwap[0]= (uint16_t)(DataSum.X_Angle/5*100);
//        g_stC7.stOut.wMpuSwap[1]= (uint16_t)(DataSum.Y_Angle/5*100);

//        DevMpu6050->MPU6050_Get_Angle(&data);
//
//        if(g_pstC7SysPare->wMpuX >9000) wMpuData_X = (uint16_t)(data.X_Angle*100) - (g_pstC7SysPare->wMpuX - 9000);//data.X_Angle>>8
//        else wMpuData_X = (uint16_t)(data.X_Angle*100) + (9000 - g_pstC7SysPare->wMpuX);
//        if((wMpuData_X > 0) && (wMpuData_X<18100)){
//            g_stC7.stOut.wMpuSwap[0] = wMpuData_X;
//        }
//
//        if(g_pstC7SysPare->wMpuY >9000) wMpuData_Y = (uint16_t)(data.Y_Angle*100) - (g_pstC7SysPare->wMpuY - 9000);
//        else wMpuData_Y = (uint16_t)(data.Y_Angle*100) + (9000 - g_pstC7SysPare->wMpuY);
//        if((wMpuData_Y > 0) && (wMpuData_Y<18100)){
//            g_stC7.stOut.wMpuSwap[1] = wMpuData_Y;
//        }
        LED_ON = 0;
        vTaskDelay(500);
        LED_ON = 1;
        vTaskDelay(375);
        if((DevIo->Read(PD,6) == 0) && (bBoxSta == false))
        {
            vTaskDelay(200);
            if(DevIo->Read(PD,6) == 0)
            {
                bBoxSta = true;
                MotorCtrl.C7SetErr(ERR_C7_BOX);
                g_stC7.stOut.byCardBox = 0;//无盒子
            }
        }
        else if( DevIo->Read(PD,6) == 1)
        {
            vTaskDelay(200);
            if( DevIo->Read(PD,6) == 1)
            {
                if(g_stC7.stOut.stoutCmd.byErr == ERR_C7_BOX)
                {
                    g_stC7.stOut.stoutCmd.byErr = 0;//自动清错误
                }
                bBoxSta = false;
                g_stC7.stOut.byCardBox = 1;//有盒子
            }
        }
    }
}

/*******************************************************************
功能：LED初始化
参数：无
返回值：无
其它说明：2013.12.18
*******************************************************************/
void LEDInit(void)
{
    DevIo->Config(PE, 1, 1, Mode_Out_OD);
}

/*******************************************************************
功能：创建LED任务
参数：无
返回值：无
其它说明：2013.12.18
*******************************************************************/
void CreateLedCheckTask(void)
{
    LEDInit();
    xTaskCreate( LedCheckTask , ( char const * ) "LedCheckTask", 128, NULL, tskIDLE_PRIORITY, NULL );//tskIDLE_PRIORITY
}
