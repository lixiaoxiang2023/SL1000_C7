/*****************************************************************************
文件名称：sensor.c
文件目录：$PROJ_DIR$/usr/dev/source
项目名称：
创建者：
创建日期：
程序用途：
版权声明：Copyright urit Co.,Ltd.All Right Reserved

log:

******************************************************************************/

/******************************************************************************
Include files头文件
******************************************************************************/
#include "devinit.h"
#include "tempfan.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
const double Rp=10000.0;//10K
const double T2=(273.15+25);
const double B=3950.0;
const double K=273.15;

const double Rp1=10000.0;//10K
/******************************************************************************
Global Variables Definition全局变量定义
******************************************************************************/

/******************************************************************************
外部全局变量申明
******************************************************************************/

/******************************************************************************
内部数据结构与数据类型定义
******************************************************************************/


/******************************************************************************
内部全局变量定义
******************************************************************************/
bool bADCbusy = false;

/******************************************************************************
内部函数申明
******************************************************************************/

/*****************************************************************************
函数名：void Init(void)
函数功能：光耦硬件初始化
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void Init(void)
{
    DevIo->Config(PC, 4, 1, Mode_AIN);//热风温度接口
    DevIo->Config(PC, 5, 1, Mode_AIN);//制片温度接口
    DevAdc->AddChn(0,14,SampleTime_239Cycles5);
//    DevAdc->Config(0, Mode_Independent,1);
    DevAdc->AddChn(0,15,SampleTime_239Cycles5);//制片温度接口
    DevAdc->Config(0,Mode_Independent,1);

//    DevAdc->ConfigDma(0,0,1);
//    DevAdc->ConfigDma(0,1,2);

}

/*******************************************************************************
函数功能：
*   @ 用于读取温度
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static short int ReadNtcTem(void)
{
    /* AI-FIX H-04 2026-11-14
       原实现: if(bADCbusy==false) break;  然后无条件 bADCbusy=true;
       缺陷1 : 判断与置位不是原子操作，两次任务切换即可让 ReadNtcTem 与
               ReadNtcFan 同时进入临界区，共用同一个 ADC1，互相改写
               CHN 配置，可能把主路通道(15)读到风路通道(14)的电压上。
       缺陷2 : 等待 2000ms 超时后仍然强行 bADCbusy=true 进入，
               等于锁彻底失效，两路必然并发。
       修正  : 判定+置位放入 taskENTER_CRITICAL 临界区保证原子；
               取不到锁不再抢入，直接返回哨兵 3000(300.0C)，
               由温控环的 wTemp>900 分支执行停机保护。 */
    {
        bool blAdcGot = false;

        for(int i = 0; i < 2000; i++)
        {
            taskENTER_CRITICAL();
            if(bADCbusy == false)
            {
                bADCbusy = true;
                blAdcGot = true;
            }
            taskEXIT_CRITICAL();

            if(blAdcGot == true) break;
            vTaskDelay(1);
        }

        if(blAdcGot == false) return 3000;
    }

    unsigned short int wAdc[3] = {0};
    unsigned short int wAdcMid = 0;

    double Rt = 0;
    double Vo = 0;
    double Tem = 0;
    signed short int temp = 0;

    /* 连续读取3次 */
    for(int i = 0; i < 3; i++)
    {
        wAdc[i] = DevAdc->ReadOne(0, 15, SampleTime_239Cycles5, 8);
        vTaskDelay(1);
    }

    bADCbusy = false;

    /* 三点排序取中值 */
    if(wAdc[0] > wAdc[1])
    {
        unsigned short int t = wAdc[0];
        wAdc[0] = wAdc[1];
        wAdc[1] = t;
    }

    if(wAdc[1] > wAdc[2])
    {
        unsigned short int t = wAdc[1];
        wAdc[1] = wAdc[2];
        wAdc[2] = t;
    }

    if(wAdc[0] > wAdc[1])
    {
        unsigned short int t = wAdc[0];
        wAdc[0] = wAdc[1];
        wAdc[1] = t;
    }

    wAdcMid = wAdc[1];   // 中值

    /* 电压计算 */
    Vo = (double)wAdcMid / 4095 * 3.3;

    /* 阻值计算 */
    Rt = (10000 * Vo) / (3.3 - Vo);

    /* 温度计算 */
    Tem = ((1 / (log(Rt/Rp)/B + (1/T2))) - K + 0.5) * 10;

    temp = (short int)Tem;

    //温度传感器异常  给订制3000 温度传感器故障，为了兼容之前逻辑 300度实际温度 超过设定温度会停止加热 2026.8.19 lixx
    if((wAdcMid<=OPEN_VALUE) || (wAdcMid>=SHORT_VALUE))
    {
        temp = 3000;
    }
    return temp;
}

/*******************************************************************************
函数功能：
*   @ 用于读取温度
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static short int ReadNtcFan(void)
{
    /* AI-FIX H-04 2026-11-14
       原实现: if(bADCbusy==false) break;  然后无条件 bADCbusy=true;
       缺陷1 : 判断与置位不是原子操作，两次任务切换即可让 ReadNtcTem 与
               ReadNtcFan 同时进入临界区，共用同一个 ADC1，互相改写
               CHN 配置，可能把主路通道(15)读到风路通道(14)的电压上。
       缺陷2 : 等待 2000ms 超时后仍然强行 bADCbusy=true 进入，
               等于锁彻底失效，两路必然并发。
       修正  : 判定+置位放入 taskENTER_CRITICAL 临界区保证原子；
               取不到锁不再抢入，直接返回哨兵 3000(300.0C)，
               由温控环的 wTemp>900 分支执行停机保护。 */
    {
        bool blAdcGot = false;

        for(int i = 0; i < 2000; i++)
        {
            taskENTER_CRITICAL();
            if(bADCbusy == false)
            {
                bADCbusy = true;
                blAdcGot = true;
            }
            taskEXIT_CRITICAL();

            if(blAdcGot == true) break;
            vTaskDelay(1);
        }

        if(blAdcGot == false) return 3000;
    }

    unsigned short int wAdc[3] = {0};
    unsigned short int wAdcMid = 0;

    double Rt = 0;
    double Vo = 0;
    double Tem = 0;
    signed short int temp = 0;

    /* 连续读取3次 */
    for(int i = 0; i < 3; i++)
    {
        wAdc[i] = DevAdc->ReadOne(0, 14, SampleTime_239Cycles5, 8);
        vTaskDelay(1);
    }

    bADCbusy = false;

    /* 三点排序取中值 */
    if(wAdc[0] > wAdc[1])
    {
        unsigned short int t = wAdc[0];
        wAdc[0] = wAdc[1];
        wAdc[1] = t;
    }

    if(wAdc[1] > wAdc[2])
    {
        unsigned short int t = wAdc[1];
        wAdc[1] = wAdc[2];
        wAdc[2] = t;
    }

    if(wAdc[0] > wAdc[1])
    {
        unsigned short int t = wAdc[0];
        wAdc[0] = wAdc[1];
        wAdc[1] = t;
    }

    wAdcMid = wAdc[1];   // 中值

    /* 电压计算 */
    Vo = (double)wAdcMid / 4095 * 3.3;

    /* 阻值计算 */
    Rt = (10000 * Vo) / (3.3 - Vo);

    /* 温度计算 */
    Tem = ((1 / (log(Rt/Rp)/B + (1/T2))) - K + 0.5) * 10;

    temp = (short int)Tem;

    //温度传感器异常  给订制3000 温度传感器故障，为了兼容之前逻辑 300度实际温度 超过设定温度会停止加热 2026.8.19 lixx
    if((wAdcMid<=OPEN_VALUE) || (wAdcMid>=SHORT_VALUE))
    {
        temp = 3000;
    }
    return temp;
}

const TempTdf Temperature = {
    Init,
    ReadNtcTem,
    ReadNtcFan,
};