/******************************************************************************
文件名称：FunCtrl.c
目录：$PROJ_DIR$/../code/app
项目名称：
创建者：GBS
创建日期：2020/12/8
程序用途：控制电机
风扇控制

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息



******************************************************************************/

/******************************************************************************
Include file头文件
******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FunCtrl.h"
#include "dev.h"
#include "devinit.h"
#include "tempfan.h"
#include "iosrc.h"
/******************************************************************************
Constant and Macro Definition 常量以及宏定义
******************************************************************************/

/******************************************************************************
全局变量定义
******************************************************************************/

/******************************************************************************
外部变量定义
******************************************************************************/
/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

/******************************************************************************
内部全局变量申明
******************************************************************************/
//bool bHeatFanSta = false;
/******************************************************************************
外部函数申明
******************************************************************************/

/******************************************************************************
内部函数申明
******************************************************************************/
xTaskHandle               g_FanTempTask;//检测任务句柄
xTaskHandle               g_FanTempReadTask;//检测任务句柄

/*****************************************************************************
函数名：void SetFunPWM(u8 DutyCycle)
函数功能：
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void SetFunPWM(uint8_t DutyCycle)
{
    if(DutyCycle == 0)
    {
#if OLD_BOARD
        DevIo->Write(PD, 15, 1);
        DevIo->Write(PD, 12, 0);
#else
        DevIo->Write(PD, 9, 0);
#endif
        //    DevPwm->Write(1,0,50000);
        //    DevPwm->Stop(1);
    }
    else
    {
#if OLD_BOARD
        DevIo->Write(PD, 15, 0);
        DevIo->Write(PD, 12, 1);
#else
        DevIo->Write(PD, 9, 1);
#endif
        //    DevPwm->Write(1,DutyCycle,50000);
    }
}

/*****************************************************************************
uint16_t GetFanTemp()
函数功能：
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static uint16_t GetFanTemp(void)
{
    return g_stC7.stOut.byTemp[1];//Temperature.ReadNtcTem(1);
}

/*****************************************************************************
函数名：static void Init(void)
函数功能：初始化
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void Init(void)
{
#if OLD_BOARD
    DevIo->Config(PD, 15, 1, Mode_Out_PP); //PWM控制口 风扇
    DevIo->Write(PD, 15, 1);

    DevIo->Config(PD, 12, 1, Mode_Out_PP); //零时
    DevIo->Write(PD, 12, 0);
#else
    DevIo->Config(PD, 9, 1, Mode_Out_PP); //PWM控制口 风扇
    DevIo->Write(PD, 9, 0);
#endif
//    DevIo->Config(PB, 14, 1, Mode_Out_PP); //加热控制口
//    DevIo->Write(PB,14,0);
    DevIo->Config(PC, 10, 1, Mode_Out_PP); //
    DevIo->Write(PC,10,0);
}

/*****************************************************************************
函数名：void GetSpeed(u8 DutyCycle)
函数功能：
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static uint32_t GetSpeed(void)
{
    return 0;
}

///*****************************************************************************
//函数名：void TempModuleTask(void *pvParameters)
//函数功能：温控任务执行函数
//参数描述:
//返回值描述：
//特殊算法：
//注意事项：
//******************************************************************************/
//void FanTempTask(void *pvParameters)
//{
//    static  short int wTemp = 0;
//    uint16_t TempErrCunt = 0;
//    bool  bOpenSta = true;
//    while(1)
//    {
//        vTaskDelay(500);
//        wTemp = Temperature.ReadNtcTem(1);
//        g_stC7.stOut.byTemp[1] = wTemp;
//        if(bHeatFanSta == true)
//        {
//            if(wTemp>2000)
//            {//临时出错判断ADC异常
//                TempErrCunt++;
//                if(TempErrCunt==10){
//                    TempErrCunt = 0;
//                    DevIo->Write(PB,14,0);
//                    //              vTaskSuspend(g_FanTempTask);
//                    //              vTaskResume(g_FanTempReadTask);
//                }
//            }
//            else
//            {
//                TempErrCunt = 0;
//              //  g_stC7.stOut.byTemp[1] = wTemp;
//                if(wTemp > 1000){
//                    DevIo->Write(PB,14,0);
//                    bOpenSta = false;
//                }
//                if(bOpenSta == false && wTemp < 950){
//                    DevIo->Write(PB,14,1);
//                    bOpenSta = true;
//                }
//            }
//        }
//    }
//}
void OpendyeFan(uint8_t byMode)
{
    if(byMode == 1) 
    {
        DevIo->Write(PC,10,1);
    }
    else
    {
        
        DevIo->Write(PC,10,0);
    }
}

const FunCtrlTdf FunCtrl = {
    Init,
    SetFunPWM,
    GetFanTemp,
    GetSpeed,
    OpendyeFan,
};


