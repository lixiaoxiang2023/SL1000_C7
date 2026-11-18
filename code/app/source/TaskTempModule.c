/*****************************************************************************
文件名称：TaskTempModule.c
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
//#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include "SysData.h"
#include "TaskTempModule.h"
#include "devinit.h"
#include "temperature.h"
#include "piheat_pid.h"
//#include "adcf2_4.h"
#include "math.h"
#include "iosrc.h"
#include "thb6128.h"
#include "motorctrl.h"
#include "tempfan.h"
#include "clients/clients_def.h"

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
//uint16_t Backkup = 0;
//uint16_t Count = 0;
/******************************************************************************
外部全局变量申明
******************************************************************************/

//extern u32 Count_Temp;
/******************************************************************************
内部数据结构与数据类型定义
******************************************************************************/
static struct
{
    int wPwm ;
    uint16_t wPwm_Old;
    uint8_t bySetDerat;
    bool TempCheck;
    uint16_t TimeCount;
}s_stTempDat;

static struct
{
    int wPwm ;
    uint16_t wPwm_Old;
    uint8_t bySetDerat;
    bool TempCheck;
    uint16_t TimeCount;
}s_stTempDatFan;

stPidTdf stPiPid;
stPidTdf stPiPidFan;
/******************************************************************************
内部全局变量定义
******************************************************************************/
//const double Rp=10000.0;//10K
//const double T2=(273.15+25);
//const double B=3950.0;
//const double K=273.15;
//
//const double Rp1=100000.0;//100K
bool bHeatSta = false;
bool bHeatFanSta = false;
/******************************************************************************
Global Variables Definition全局变量定义
******************************************************************************/
//xTaskHandle               g_TempModuleTask;//检测任务句柄
//xTaskHandle               g_TempReadTask;//检测任务句柄
//xQueueHandle              g_Queue_TempCmd;//温控相关命令队列

//void FanTempTask(void *pvParameters);
/******************************************************************************
内部函数申明
******************************************************************************/
/*****************************************************************************
函数名：void TempModuleTask(void *pvParameters)
函数功能：温控任务执行函数
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
//void TempReadTask(void *pvParameters)
//{
//    uint16_t wTempRead = 0;
//    while(1)
//    {
//        vTaskDelay(100);
//       // wTempRead = Temperature.ReadNtcTem(0);
//        g_stC7.stOut.byTemp[0] = wTempRead;
//    }
//
//}

/*****************************************************************************
函数名：void TempModuleTask(void *pvParameters)
函数功能：温控任务执行函数
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
void TempModuleTask(void *pvParameters)
{
    static  short int wTemp1 = 0;
    uint16_t k = 0;
    short int TempData1 = 0,TempData2 = 0;
    memset(&s_stTempDat, 0, sizeof(s_stTempDat));
    s_stTempDat.TempCheck = true;
    while(1)
    {
        if(bHeatSta == true)
        {
            k++;
            if(k == 5)
            {
                k = 0;
                
                wTemp1 = Temperature.ReadNtcTem();
                
                g_stC7.stOut.byTemp[0] = wTemp1;
                
                if(s_stTempDat.TimeCount == 0 ) 
                {
                    TempData1 = wTemp1;
                }
                else if((s_stTempDat.TimeCount == 3000) && (s_stTempDat.TempCheck == true))//记录5分钟后温度对比差小于10度，则出现故障
                {
                   // s_stTempDat.TimeCount = 0;
                    s_stTempDat.TempCheck = false;
                    TempData2 = wTemp1;
                    if(((TempData2 - TempData1)<100) && (TempData1 < (stPiPid.Temset-120)) && (s_stTempDat.bySetDerat == 0))
                    {
                        s_stTempDat.TimeCount = 0;
                        CloseTemp();
                        MotorCtrl.C7SetErr(ERR_C7_TEMP);
                    }
                }
                else if(wTemp1 == 3000)
                {
                    MotorCtrl.C7SetErr(ERR_C7_TEMP_CURING_ERR);
                }
                if(s_stTempDat.TempCheck == true) s_stTempDat.TimeCount++;
            }

            stPiPid.Temin = wTemp1;
            s_stTempDat.wPwm = (int)PidLoc(&stPiPid);
            if(s_stTempDat.wPwm > 100) s_stTempDat.wPwm = 100;
            else if(s_stTempDat.wPwm < 0)s_stTempDat.wPwm = 0;

            if(s_stTempDat.wPwm_Old != s_stTempDat.wPwm)
            {
                DevPwm->Write(0, s_stTempDat.wPwm*2, 50000);
                s_stTempDat.wPwm_Old = s_stTempDat.wPwm;
                //if(s_stTempDat.wPwm == 0) DevPwm->Stop(0);
            }
            if((wTemp1 > (g_pstC7SysPare->wHeatTemp + 50)) && (s_stTempDat.bySetDerat == 0))
            {
                DevPwm->Write(0, 0, 50000);
                DevPwm->Stop(0);
            }
            else if((wTemp1 > 370) && (s_stTempDat.bySetDerat == 1))
            {
                DevPwm->Write(0, 0, 50000);
                DevPwm->Stop(0);
            }
            else if(wTemp1 > 900)
            {
                CloseTemp();
            }
        }
        else
        {
            vTaskDelay(500);
            wTemp1 = Temperature.ReadNtcTem();
            g_stC7.stOut.byTemp[0] = wTemp1;
        }
        vTaskDelay(20);
       // Count++;
    }
}

/*****************************************************************************
函数名：void FanTempTask(void *pvParameters)
函数功能：温控任务执行函数
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
void FanTempTask(void *pvParameters)
{
    static  short int wTempFan = 0;
    uint16_t k1 = 0;
    uint16_t TempFanData1 = 0,TempFanData2 = 0;
    memset(&s_stTempDatFan, 0, sizeof(s_stTempDatFan));
    s_stTempDatFan.TempCheck = true;
    while(1)
    {
        if( bHeatFanSta == true)
        {
            k1++;
            if(k1 == 5)
            {
                k1 = 0;
                wTempFan = Temperature.ReadNtcFan();
                g_stC7.stOut.byTemp[1] = wTempFan;
                if(s_stTempDatFan.TimeCount == 0) TempFanData1 = wTempFan;
                else if((s_stTempDatFan.TimeCount == 1200) && (s_stTempDatFan.TempCheck == true))//记录2分钟后温度对比差小于5度，则出现故障
                {
                   // s_stTempDatFan.TimeCount = 0;
                    s_stTempDatFan.TempCheck = false;
                    TempFanData2 = wTempFan;
                    if(((TempFanData2 - TempFanData1) < 50) && (TempFanData1 < 650) && (s_stTempDat.bySetDerat == 0))
                    {
                        s_stTempDatFan.TimeCount = 0;
                        CloseHeatFan();
                        MotorCtrl.C7SetErr(ERR_C7_TEMPFAN);
                    }
                }                
                else if(wTempFan == 3000)
                {
                    MotorCtrl.C7SetErr(ERR_C7_TEMP_DRYING_ERR);
                }
                if(s_stTempDatFan.TempCheck == true) s_stTempDatFan.TimeCount++;
            }

            stPiPidFan.Temin = wTempFan;
            s_stTempDatFan.wPwm = (int)PidLocFan(&stPiPidFan)*2;
            if(s_stTempDatFan.wPwm > 100) s_stTempDatFan.wPwm = 100;
            else if(s_stTempDatFan.wPwm < 0)s_stTempDatFan.wPwm = 0;

            if((wTempFan > (g_pstC7SysPare->wHeatFanTemp + 50)) && (s_stTempDat.bySetDerat == 0))
            {
                s_stTempDatFan.wPwm_Old = s_stTempDatFan.wPwm = 0;
              //  DevPwm->Write(1, 0, 50000);
                DevPwm->Stop(1);
            }
            else if((wTempFan > 370) && (s_stTempDat.bySetDerat == 1))
            {
                s_stTempDatFan.wPwm_Old = s_stTempDatFan.wPwm = 0;
              //  DevPwm->Write(1, 0, 50000);
                DevPwm->Stop(1);
            }
            else if(wTempFan > 920)
            {
                CloseHeatFan();
            }
            else if(s_stTempDatFan.wPwm_Old != s_stTempDatFan.wPwm)
            {
                s_stTempDatFan.wPwm_Old = s_stTempDatFan.wPwm;
                
                if(s_stTempDatFan.wPwm == 0)
                {
                     DevPwm->Stop(1);
                }
                else
                {
                    DevPwm->Write(1,s_stTempDatFan.wPwm, 50000);
                }
            }
        }
        else
        {
            vTaskDelay(500);
            wTempFan = Temperature.ReadNtcFan();
            g_stC7.stOut.byTemp[1] = wTempFan;
        }
        vTaskDelay(20);  
    }
}

/*******************************************************************
功能：
参数：无
返回值：无
其它说明：
*******************************************************************/
void DeratTemp(void)
{
    //    vTaskSuspend(g_TempReadTask);
    //    vTaskResume(g_TempModuleTask);
    stPiPid.Temset = 400;
    s_stTempDat.bySetDerat = 1;
    s_stTempDat.TimeCount = 0;
    s_stTempDat.wPwm_Old = 0;
    // s_stTempDat.TempCheck = true;

    stPiPidFan.Temset = 400;
    s_stTempDatFan.bySetDerat = 1;
    s_stTempDatFan.TimeCount = 0;
    s_stTempDatFan.wPwm_Old = 0;
}

/*******************************************************************
功能：
参数：无
返回值：无
其它说明：
*******************************************************************/
void RecoverTemp(void)
{
    //    vTaskSuspend(g_TempReadTask);
    //    vTaskResume(g_TempModuleTask);
    bHeatSta = true;
    stPiPid.Temset = g_pstC7SysPare->wHeatTemp;//650;
    s_stTempDat.bySetDerat = 0;
    s_stTempDat.TimeCount = 0;
    s_stTempDat.wPwm_Old = 0;
    s_stTempDat.TempCheck = true;
    s_stTempDat.TimeCount = 0;

    bHeatFanSta = true;
    stPiPidFan.Temset = g_pstC7SysPare->wHeatFanTemp;//750;
    s_stTempDatFan.bySetDerat = 0;
    s_stTempDatFan.TimeCount = 0;
    s_stTempDatFan.wPwm_Old = 0;
    s_stTempDatFan.TempCheck = true;
    s_stTempDatFan.TimeCount = 0;
}

/*******************************************************************
功能：打开风扇加热
参数：无
返回值：无
其它说明：
*******************************************************************/
void OpenHeatFan(void)
{
    bHeatFanSta = true;
    stPiPidFan.Temset = g_pstC7SysPare->wHeatFanTemp;//750;
    s_stTempDatFan.bySetDerat = 0;
    s_stTempDatFan.TimeCount = 0;
    s_stTempDatFan.wPwm_Old = 0;
    s_stTempDatFan.TempCheck = true;
    s_stTempDatFan.TimeCount = 0;
}

/*******************************************************************
功能：创建温控任务
参数：无
返回值：无
其它说明：
*******************************************************************/
void CreateTempModuleTask(void)
{
    /* AI-FIX H-06 2026-11-14
       原实现: tskIDLE_PRIORITY(0)。全机 11 个任务里 0 优先级占 10 个，
               仅 XcanTask 为 1，温控环(含全部过温保护判断)是整机最低
               调度等级，与 9 个业务任务同池做时间片轮转，重负载时
               可能被连续推迟多个 tick，保护动作的实时性无法保证。
       修正  : 提升到 2(高于 XcanTask 的 1)。本任务每拍末尾都有
               vTaskDelay(20)，绝大部分时间处于阻塞态，提升优先级
               不会饿死低优先级任务；单次占用 CPU 约 100~200us
               (ADC 的 vTaskDelay 期间主动让出)，对 5ms 的 xcan 同步
               周期无实质影响。若现场发现通信异常，可回退为 1。 */
    xTaskCreate( TempModuleTask ,( char const * )"TempModuleTask", 256, NULL, 2, NULL);//AI-FIX H-06: 原 tskIDLE_PRIORITY
}

/*******************************************************************
功能：创建温控任务
参数：无
返回值：无
其它说明：
*******************************************************************/
void CreateFanTempTask(void)
{
    xTaskCreate( FanTempTask ,( char const * )"FanTempTask", 256, NULL, 2, NULL);//AI-FIX H-06: 原 tskIDLE_PRIORITY
}

/*******************************************************************
功能：创建讀取温控任务
参数：无
返回值：无
其它说明：
*******************************************************************/
//void CreateTempReadTask(void)
//{
//    //xTaskCreate( TempReadTask ,( char const * )"TempReadTask", 128, NULL, tskIDLE_PRIORITY, &g_TempReadTask );//tskIDLE_PRIORITY
//}
/*******************************************************************
功能：关闭加热
参数：无
返回值：无
其它说明：
*******************************************************************/
void CloseTemp(void)
{
    //    vTaskSuspend(g_TempModuleTask);
    //    vTaskResume(g_TempReadTask);
    bHeatSta = false;
    DevPwm->Write(0, 0, 50000);
    DevPwm->Stop(0);

    bHeatFanSta = false;
    DevPwm->Write(1, 0, 50000);
    DevPwm->Stop(1);
}

/*******************************************************************
功能：关闭风扇加热
参数：无
返回值：无
其它说明：
*******************************************************************/
void CloseHeatFan(void)
{
    bHeatFanSta = false;
    DevPwm->Write(1, 0, 50000);
    DevPwm->Stop(1);
}

/*******************************************************************
功能：
参数：无
返回值：无
其它说明：设置固化温度
*******************************************************************/
void SetTemp(u16 wTempData)
{
//    g_pstC7SysPare->wHeatTemp = wTempData;
//    if((wTempData > 600) && (wTempData < 800))
//    {
//        SaveSysData();
//    }
}

/*******************************************************************
功能：
参数：无
返回值：无
其它说明：设置烘干温度
*******************************************************************/
void SetFanTemp(u16 wTempData)
{
//   g_pstC7SysPare->wHeatFanTemp = wTempData;
//    if((wTempData > 700) && (wTempData < 850))
//    {
//        SaveSysData();
//    }
}

/*******************************************************************************
函数功能：
*   @ 电加热PID参数初始化
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void PidInit(void)
{
    s_stTempDat.wPwm    = 0;
    stPiPid.Temset      = g_pstC7SysPare->wHeatTemp;//650;
    stPiPid.Temin       = 0;
    stPiPid.dev         = 0.0;
    stPiPid.dev_last    = 0.0;
    stPiPid.dev_next    = 0.0;
    stPiPid.Kp          = 20;//14 18
    stPiPid.Ki          = 0.00;
    stPiPid.Kd          = 0.00;
    stPiPid.fout        = 0.0;


    s_stTempDatFan.wPwm    = 0;
    stPiPidFan.Temset      = g_pstC7SysPare->wHeatFanTemp;//750;
    stPiPidFan.Temin       = 0;
    stPiPidFan.dev         = 0.0;
    stPiPidFan.dev_last    = 0.0;
    stPiPidFan.dev_next    = 0.0;
    stPiPidFan.Kp          = 20;//14 18
    stPiPidFan.Ki          = 0.00;
    stPiPidFan.Kd          = 0.00;
    stPiPidFan.fout        = 0.0;
}

/*****************************************************************************
函数名：void TempModuleInit(void)ca
函数功能：
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
uint8_t TempModuleInit(void)
{
    //    AdInit();
    PWMInit();
    PidInit();

    return 0;
}
