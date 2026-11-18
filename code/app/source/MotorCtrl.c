/******************************************************************************
文件名称：  MotorCtrl.c
目录：$PROJ_DIR$/usr/app
项目名称：
创建者：GBS
创建日期：2020/12/8
程序用途：


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
#include "MotorCtrl.h"
#include "TaskMoveStrip.h"
#include "devinit.h"
#include "devplat.h"
#include "iosrc.h"
#include "a3983.h"
#include "sensor.h"
#include "SysData.h"
#include "clients/clients_def.h"
/******************************************************************************
Constant and Macro Definition 常量以及宏定义
******************************************************************************/
#define M1             1
#define M2             2
#define M3             3
#define M4             4
#define M5             5
/******************************************************************************
全局变量定义
******************************************************************************/

/******************************************************************************
外部变量定义
******************************************************************************/
bool Ageingrunning = false;
bool bFrist = 0;
#define  bOldVer     0 //用于新老镜油检测方式开启 1老版本 0新版本
/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

/******************************************************************************
内部全局变量申明
******************************************************************************/
//int wOpValCnt = 0;
//int ValCount = 0;

static struct {
    int wOpValCnt;
    int wValCount;
    int wOilValCount;
    bool bNoOilSta;
}CtrlVarTdf;

//const DevA3983Tdf *DevA3983;
/******************************************************************************
外部函数申明
******************************************************************************/

/******************************************************************************
内部函数申明
******************************************************************************/
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
    stA3983InitTdf A3983InitTdf;
    DevIo->Config(PB, 5, 1, Mode_IPU);          // op2 新电路板 J2
    // M1
    DevIo->Config(PB, 11, 1, Mode_AF_PP);        // clk
    DevIo->RemapConfig(GPIO_FullRemap_TIM2,ENABLE);
    DevIo->Config(PB, 10, 1, Mode_Out_PP);       // half
    DevIo->Config(PE, 15, 1, Mode_Out_PP);       // dir
    DevIo->Config(PB, 7, 1,  Mode_IPU);          // op1 新电路板  J1
    memset(&A3983InitTdf, 0, sizeof(stA3983InitTdf));
    A3983InitTdf.dwTimNo = 1;
    A3983InitTdf.dwTimChnNo = 4 ;
    A3983InitTdf.byAtOptLv = 1;
    A3983InitTdf.pwdOpGpio = &BIT_GPIOB_IDR_IDR7;//新电路板
    A3983InitTdf.pwdDirGpio = &BIT_GPIOE_ODR_ODR15;
    A3983InitTdf.pwdHalfGpio = &BIT_GPIOB_ODR_ODR10;
    A3983InitTdf.byHwnd = M1;
    DevA3983 = A3983Init(&A3983InitTdf);

    // M2
    DevIo->Config(PE, 14, 1, Mode_AF_PP);        // clk
    DevIo->RemapConfig(GPIO_FullRemap_TIM1,ENABLE);
    DevIo->Config(PE, 13, 1, Mode_Out_PP);       // half
    DevIo->Config(PE, 12, 1, Mode_Out_PP);       // dir
    DevIo->Config(PB, 6, 1, Mode_IPU);          // op1 新电路板 J7
    memset(&A3983InitTdf, 0, sizeof(stA3983InitTdf));
    A3983InitTdf.dwTimNo = 0;
    A3983InitTdf.dwTimChnNo = 4 ;
    A3983InitTdf.byAtOptLv = 0;
    A3983InitTdf.pwdOpGpio = &BIT_GPIOB_IDR_IDR6;//新电路板
    A3983InitTdf.pwdDirGpio = &BIT_GPIOE_ODR_ODR12;
    A3983InitTdf.pwdHalfGpio = &BIT_GPIOE_ODR_ODR13;
    A3983InitTdf.byHwnd = M2;
    DevA3983 = A3983Init(&A3983InitTdf);

    // M3
    DevIo->Config(PC, 6, 1, Mode_AF_PP);         // clk 新电路板
    DevIo->Config(PB, 0, 1, Mode_Out_PP);        // half
    DevIo->Config(PA, 7, 1, Mode_Out_PP);        // ir
   // DevIo->Config(PD, 5, 1, Mode_IPD);           // op 新电路板 行程开关 J19
    DevIo->Config(PB, 4, 1, Mode_IPU);          // op1 光耦
    memset(&A3983InitTdf, 0, sizeof(stA3983InitTdf));
    A3983InitTdf.dwTimNo = 7;
    A3983InitTdf.dwTimChnNo = 1 ;// 新电路板
    A3983InitTdf.byAtOptLv = 1;
    A3983InitTdf.pwdOpGpio = &BIT_GPIOB_IDR_IDR4; // op 新电路板
    A3983InitTdf.pwdDirGpio =  &BIT_GPIOA_ODR_ODR7;
    A3983InitTdf.pwdHalfGpio = &BIT_GPIOB_ODR_ODR0;
    A3983InitTdf.byHwnd = M3;
    DevA3983 = A3983Init(&A3983InitTdf);

    // M4
    DevIo->Config(PA, 6, 1, Mode_AF_PP);         // clk
    DevIo->Config(PA, 3, 1, Mode_Out_PP);        // half
    DevIo->Config(PA, 2, 1, Mode_Out_PP);        // ir
    DevIo->Config(PD, 7, 1, Mode_IPD);//行程开关，用于检测C7是否转到C6模块J30
    DevIo->Config(PB, 3, 1, Mode_IPU);           // op 新电路板
    memset(&A3983InitTdf, 0, sizeof(stA3983InitTdf));
    A3983InitTdf.dwTimNo = 2;
    A3983InitTdf.dwTimChnNo = 1 ;
    A3983InitTdf.byAtOptLv = 0;
    A3983InitTdf.pwdOpGpio = &BIT_GPIOB_IDR_IDR3;//新电路板 J24
    A3983InitTdf.pwdDirGpio = &BIT_GPIOA_ODR_ODR2;
    A3983InitTdf.pwdHalfGpio = &BIT_GPIOA_ODR_ODR3;
    A3983InitTdf.byHwnd = M4;
    DevA3983 = A3983Init(&A3983InitTdf);

    // M5
    DevIo->Config(PA, 1, 1, Mode_AF_PP);         // clk
    DevIo->Config(PE, 5, 1, Mode_Out_PP);        // Dir
    DevIo->Config(PE, 6, 1, Mode_Out_PP);        //  half
    DevIo->Config(PE, 4, 1, Mode_IN_FLOATING);           // op J22
    memset(&A3983InitTdf, 0, sizeof(stA3983InitTdf));
    A3983InitTdf.dwTimNo = 4;
    A3983InitTdf.dwTimChnNo =2 ;
    A3983InitTdf.byAtOptLv = 0;
    A3983InitTdf.pwdOpGpio = &BIT_GPIOE_IDR_IDR4;
    A3983InitTdf.pwdDirGpio = &BIT_GPIOE_ODR_ODR5;
    A3983InitTdf.pwdHalfGpio = &BIT_GPIOE_ODR_ODR6;
    A3983InitTdf.byHwnd = M5;
    DevA3983 = A3983Init(&A3983InitTdf);

    DevA3983->Stop(M1);
    DevA3983->Stop(M2);
    DevA3983->Stop(M3);
    DevA3983->Stop(M4);
    DevA3983->Stop(M5);
}

/*****************************************************************************
函数名：void Get_OpVal(void)
函数功能：判断是否有载玻片
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
void Get_OpVal(void)
{
    if(DevIo->Read(PD,2) == 0)//J25
    {
        CtrlVarTdf.wOpValCnt++;
    }
}

/*****************************************************************************
函数名：void Get_OpVal(void)
函数功能：
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static uint8_t Get_ErrLevel(uint16_t byErr)
{
    uint16_t byErrLevel = 0;

    switch(byErr)
    {
    case ERR_C7_OK      : byErrLevel = 0;break;
    case ERR_C7_M1MOTOR : byErrLevel = 1;break;
    case ERR_C7_M2MOTOR : byErrLevel = 2;break;
    case ERR_C7_M3MOTOR : byErrLevel = 2;break;
    case ERR_C7_M4MOTOR : byErrLevel = 2;break;
    case ERR_C7_TEMPFAN : byErrLevel = 2;break;
    case ERR_C7_NOOIL   : byErrLevel = 1;break;
    case ERR_C7_ADDOIL  : byErrLevel = 1;break;
    case ERR_C7_TEMP    : byErrLevel = 1;break;
    case ERR_C7_BOX     : byErrLevel = 1;break;
    case ERR_C7_M1OPC2  : byErrLevel = 1;break;
    case ERR_C7_SLIDECAR: byErrLevel = 1;break;
    case ERR_C7_TEMP_CURING_ERR    : byErrLevel = 1;break;
    case ERR_C7_TEMP_DRYING_ERR    : byErrLevel = 1;break;
    default:
         byErrLevel = 0;
        break;

    }
    return byErrLevel;
}

/*****************************************************************************
函数名：int C7SetErr(uint16_t byErr)
参数描述：报错函数
返回值描述：-1
特殊算法：无
注意事项：报错时停用所有电机、泵、阀
******************************************************************************/
int C7SetErr(uint16_t byErr)
{
    uint8_t byErrLevel = 0;
    g_stC7.stOut.stoutCmd.byErr = byErr;
    g_stC7.stOut.stoutCmd.byErrCnt++;
    byErrLevel = Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel == 2)
    {
        DevA3983->Stop(M1);
        DevA3983->Stop(M2);
        DevA3983->Stop(M3);
        DevA3983->Stop(M4);
        DevA3983->Stop(M5);
    }
    return 1;
}

/*****************************************************************************
函数名：void M1_To_Drop(void)
函数功能：光耦检测位置到滴样位置
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M1_To_Drop(uint32_t Steps, uint8_t byDebug)
{
    CtrlVarTdf.wOpValCnt = 0;
    DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR5, 1);//外侧光耦
    DevA3983->Write(M1, 2000, 3200, 500, 1000, 10, 0, (M_SEN | M_CCW | M_DIV8));
    DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(!DevA3983->GetOp(M1))
    {
        vTaskDelay(5);
        if(!DevA3983->GetOp(M1))
        {
            C7SetErr(ERR_C7_M1MOTOR);
            return 1;
        }
    }
    vTaskDelay(100);
    DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//内侧光耦
    DevA3983->Write(M1, 1400, 3200, 800, 1000, 10, 0, (M_SEN | M_CW | M_DIV8));
    DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CW | M_DIV8));

    vTaskDelay(100);
    if(!DevA3983->GetOp(M1))
    {
        C7SetErr(ERR_C7_M1OPC2);
        return 1;
    }

    DevA3983->Write(M1, 2000, 2000, 800, 1000, 10, 0, (M_STP | M_CW | M_DIV8));
    vTaskDelay(100);
    DevA3983->Write(M1, 2000, 3200, 800, 1000, 10, 0, (M_SEN | M_CCW | M_DIV8));
    DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
    vTaskDelay(100);
//    if(!DevA3983->GetOp(M1))
//    {
//        C7SetErr(ERR_C7_M1OPC2);
//        return 1;
//    }
    DevA3983->Write(M1, Steps, Steps, 800, 1000, 10, 0, (M_STP | M_CW | M_DIV8));
    if(byDebug==0){
        DevA3983->Write(M1, 7300, 10000, 1000, 3500, 10, 0, (M_SEN | M_CCW | M_DIV8));
        DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
        if(!DevA3983->GetOp(M1))
        {
            vTaskDelay(5);
            if(!DevA3983->GetOp(M1))
            {
                C7SetErr(ERR_C7_M1MOTOR);
                return 1;
            }
        }
    }
    return 0;
}

/*****************************************************************************
函数名：void M2_MoveStep(void)
函数功能：M2电机走一小步
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M2_MoveStep(uint32_t Steps)
{
  
    DevA3983->Write(M2, Steps, Steps, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
    return 0;
}


/*****************************************************************************
函数名：void M1_To_Drop(void)
函数功能：光耦检测位置到滴样位置
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M1_Opc_To_Drop(uint32_t Steps)
{
    static uint16_t MotoStepcount = 0;
    DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//新电路板
    DevA3983->Write(M1, 3800, 3800, 600, 1000, 10, 0, (M_STP | M_CW | M_DIV8));
    vTaskDelay(300);
    DevA3983->Write(M1, 7300, 10000, 1000, 3800, 10, 0, (M_SEN | M_CCW | M_DIV8));
    MotoStepcount = DevA3983->GetSteps(M1);
    DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(!DevA3983->GetOp(M1) || (MotoStepcount < (Steps - 400)))
    {
        vTaskDelay(5);
        if(!DevA3983->GetOp(M1) || (MotoStepcount < (Steps - 400)))
        {
            C7SetErr(ERR_C7_M1MOTOR);
            return 1;
        }
    }

    return 0;
}



/*****************************************************************************
函数名：void M1_To_Drop(void)
函数功能：拨片盒位置到滴样光耦检测位置
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M1_To_Opc(uint32_t Steps,uint8_t byModle)
{
    uint16_t MotoStepcount = 0;
    if(Steps<3600) return 0;
    CtrlVarTdf.wOpValCnt = 0;
    DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR5, 1);//到最外侧光耦
    DevA3983->Write(M1, 2000, 5000, 1000, 2000, 10, 0, (M_SEN | M_CCW | M_DIV8));
    DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(!DevA3983->GetOp(M1))
    {
        C7SetErr(ERR_C7_M1MOTOR);
        //C7SetErr(0x77);
        return 1;
    }
    vTaskDelay(1000);
    DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);
    DevA3983->Write(M1, 1400, 5000, 800, 800, 10, 0, (M_SEN | M_CW | M_DIV8));
    DevA3983->Write(M1, 30, 30, 800, 800, 10, 0, (M_STP | M_CW | M_DIV8));
    if(!DevA3983->GetOp(M1))
    {
        C7SetErr(ERR_C7_M1OPC2);
        return 1;
    }
    vTaskDelay(100);
    DevA3983->Write(M1, (Steps - 3800), (Steps - 3800), 800, 1000, 8, Get_OpVal, (M_STP | M_CW | M_DIV8));//1405
    if(byModle == 1) CtrlVarTdf.wOpValCnt = 100;//强制复制，默认有载玻片
    if(CtrlVarTdf.wOpValCnt > 60)
    {
        g_stC7.stOut.byDropOpc ++ ;

        DevA3983->Write(M1, (Steps - 3800), (Steps - 2000), 800, 2000, 10, 0, (M_SEN | M_CCW | M_DIV8));
        MotoStepcount = DevA3983->GetSteps(M1);
        DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
//        if(!DevA3983->GetOp(M1))
//        {
//            C7SetErr(ERR_C7_M1OPC2);
//            return 1;
//        }
        DevA3983->Write(M1, (Steps - 3800), (Steps - 3800), 1000, 1000, 10, 0, (M_STP | M_CW | M_DIV8));//1405
    }
    else
    {
        DevA3983->Write(M1, (Steps - 3800), (Steps - 2000), 1000, 3500, 10, 0, (M_SEN | M_CCW | M_DIV8));
        MotoStepcount = DevA3983->GetSteps(M1);
        DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
//        if(!DevA3983->GetOp(M1))
//        {
//            C7SetErr(ERR_C7_M1OPC2);
//            return 1;
//        }
        if(MotoStepcount < Steps-3800-400)//用于判断是否拨载玻片卡
        {
            C7SetErr(ERR_C7_SLIDECAR);
            return 1;
        }
    }
return 0;
}

/*****************************************************************************
函数名：void M1_Rest_To_Drop(void)
函数功能：复位位置到滴样位置
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M1_Rest_To_Drop(uint32_t Steps,uint8_t byDebug)
{
    if (Steps < 1) return 0;
    DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//新电路板
    if(DevA3983->GetOp(M1))
    {
        DevA3983->Write(M1, Steps, Steps, 1000, 1000, 10, 0, (M_STP | M_CW | M_DIV8));
        if(DevA3983->GetOp(M1))
        {
            C7SetErr(ERR_C7_M1OPC2);
            return 1;
        }
        if(byDebug == 0){
            DevA3983->Write(M1, 7300, 9000, 1000, 3500, 10, 0, (M_SEN | M_CCW | M_DIV8));
            DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
            if(!DevA3983->GetOp(M1))
            {
                C7SetErr(ERR_C7_M1OPC2);
                return 1;
            }
        }
    }
    return 0;
}

/*****************************************************************************
函数名：void M1_GetIoVal(void)
函数功能：拨片盒位置到滴样位置
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void M1_GetIoVal(void)
{
    if(DevIo->Read(PB,5)) CtrlVarTdf.wValCount++;//新电路板
    if(CtrlVarTdf.wValCount > 6)
    {
        DevA3983->Stop(M1);
    }
}

/*****************************************************************************
函数名：void M1_Rest(void)
函数功能：M1复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M1_Rest(uint32_t Steps)
{
    //DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR5, 1);//新电路板 设置最后一个光耦
   // if(DevA3983->GetOp(M1))
   // {
   //     DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//新电路板 设置复位光耦
  //  }
   // else
    {
        DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//新电路板 设置复位光耦
        if(DevA3983->GetOp(M1))
        {
            DevA3983->Write(M1, 500, 500, 500, 500, 10, 0, (M_STP | M_CW | M_DIV8));
            if(DevA3983->GetOp(M1))
            {
               vTaskDelay(5); 
                if(DevA3983->GetOp(M1))
                {
                    C7SetErr(ERR_C7_M1MOTOR);
                    return 1;
                }
            }
            DevA3983->Write(M1, 500, 1000, 500, 500, 10, 0, (M_SEN | M_CCW | M_DIV8));
            DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
//            if(!DevA3983->GetOp(M1))
//            {
//                vTaskDelay(5); 
//                if(!DevA3983->GetOp(M1))
//                {
//                    C7SetErr(ERR_C7_M1MOTOR);
//                    return 1;
//                }
//            }
        }
        else
        {
            DevA3983->Write(M1, 1850, 3200, 500, 500, 10, M1_GetIoVal, (M_SEN | M_CCW | M_DIV8));
            DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
            if(CtrlVarTdf.wValCount > 6)
            {
                vTaskDelay(100);
                CtrlVarTdf.wValCount = 0;//找到最后一个光耦，停在该位置
                DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);
                DevA3983->Write(M1, 1400, 3500, 800, 800, 10, 0, (M_SEN | M_CW | M_DIV8));
                DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CW | M_DIV8));
                DevA3983->Write(M1, 1000, 1000, 800, 800, 10, 0, (M_STP | M_CW | M_DIV8));
                DevA3983->Write(M1, 1000, 1500, 800, 800, 10, 0, (M_SEN | M_CCW | M_DIV8));
                DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));   

            }
            else
            {
                CtrlVarTdf.wValCount = 0;
                DevA3983->Write(M1, 1850, 9000, 500, 1000, 10, 0, (M_SEN | M_CCW | M_DIV8));
                DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
                if(!DevA3983->GetOp(M1))
                {
                    vTaskDelay(5);  
                    if(!DevA3983->GetOp(M1))
                    {
                        DevA3983->Write(M1, 1400, 3500, 800, 800, 10, 0, (M_SEN | M_CW | M_DIV8));
                        DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CW | M_DIV8));
                        DevA3983->Write(M1, 1000, 1000, 800, 800, 10, 0, (M_STP | M_CW | M_DIV8));
                        DevA3983->Write(M1, 1000, 1500, 800, 800, 10, 0, (M_SEN | M_CCW | M_DIV8));
                        DevA3983->Write(M1, 30, 30, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8)); 
                        if(!DevA3983->GetOp(M1))
                        {

                            C7SetErr(ERR_C7_M1MOTOR);
                        }
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/*****************************************************************************
函数名：void M1_Rest(void)
函数功能：M1复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M1_DebugRest(void)
{
  //  DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR5, 1);//新电路板 设置最后一个光耦
   // if(DevA3983->GetOp(M1))
  //  {
   //     DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//新电路板 设置复位光耦
  //  }
  //  else
    {
        DevA3983->SetOp(M1, &BIT_GPIOB_IDR_IDR7, 1);//新电路板 设置复位光耦
        if(DevA3983->GetOp(M1))
        {
            DevA3983->Write(M1, 500, 500, 500, 500, 10, 0, (M_STP | M_CW | M_DIV8));
            if(DevA3983->GetOp(M1))
            {
                vTaskDelay(5);  
                if(DevA3983->GetOp(M1))
                {
                    C7SetErr(ERR_C7_M1MOTOR);
                    return 1;
                }
            }
            DevA3983->Write(M1, 500, 1000, 500, 500, 10, 0, (M_SEN | M_CCW | M_DIV8));
            DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
//            if(!DevA3983->GetOp(M1))
//            {
//                vTaskDelay(5);  
//                if(!DevA3983->GetOp(M1))
//                {
//                    C7SetErr(ERR_C7_M1MOTOR);
//                    return 1;
//                }
//            }
        }
        else
        {
            DevA3983->Write(M1, 1850, 3200, 500, 500, 10, M1_GetIoVal, (M_SEN | M_CCW | M_DIV8));
            DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
            if(CtrlVarTdf.wValCount > 6)  CtrlVarTdf.wValCount = 0;//找到最后一个光耦，停在该位置
            else
            {
                CtrlVarTdf.wValCount = 0;
                DevA3983->Write(M1, 1850, 9000, 500, 1000, 10, 0, (M_SEN | M_CCW | M_DIV8));
                DevA3983->Write(M1, 30, 30, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
                if(!DevA3983->GetOp(M1))
                {
                    vTaskDelay(5);  
                    if(!DevA3983->GetOp(M1))
                    {
                        C7SetErr(ERR_C7_M1MOTOR);
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/*****************************************************************************
函数名：void M2_MoveOneStep(void)
函数功能：M2前进一格
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M2_MoveOneStep(uint32_t Steps)
{
#if OLD_BOARD
    DevA3983->Write(M2, 300, 300, 2000, 2000, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(!DevA3983->GetOp(M2))
    {
        C7SetErr(C7_M2MotorErr);
        return 1;
    }
    DevA3983->Write(M2, 7000, 7000, 2000, 3800, 10, 0, (M_SEN | M_CCW | M_DIV8));
    DevA3983->Write(M2, 20, 20, 2000, 2000, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(DevA3983->GetOp(M2))
    {
        C7SetErr(C7_M2MotorErr);
        return 1;
    }
    DevA3983->Write(M2, Steps, Steps, 2000, 2000, 10, 0, (M_STP | M_CCW | M_DIV8));
#else
    DevA3983->Write(M2, 400, 400, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(!DevA3983->GetOp(M2))
    {
        vTaskDelay(5);  
        if(!DevA3983->GetOp(M2))
        {
            C7SetErr(ERR_C7_M2MOTOR);
            return 1;
        }
    }
    DevA3983->Write(M2, 3250, 5000, 1000, 3200, 10, 0, (M_SEN | M_CCW | M_DIV8));
    DevA3983->Write(M2, 20, 20, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
    if(DevA3983->GetOp(M2))
    {
        vTaskDelay(5);  
        if(DevA3983->GetOp(M2))
        {
            C7SetErr(ERR_C7_M2MOTOR);
            return 1;
        }
    }
    if(Steps < 2) Steps = 2;
    DevA3983->Write(M2, (Steps/2), (Steps/2), 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
#endif
    return 0;
}

/*****************************************************************************
函数名：void M2_Rest(void)
函数功能：M2复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M2_Rest(void)
{
    if(!DevA3983->GetOp(M2)) {
        return 0;
    }
    else{
#if OLD_BOARD
        DevA3983->Write(M2, 1000, 7000, 2000, 4000, 10, 0, (M_SEN | M_CW | M_DIV8));
#else
        DevA3983->Write(M2, 500, 5500, 1000, 2000, 10, 0, (M_SEN | M_CW | M_DIV8));
#endif

        if(DevA3983->GetOp(M2))
        {
            vTaskDelay(5);    
            if(DevA3983->GetOp(M2))
            {
                C7SetErr(ERR_C7_M2MOTOR);
                return 1;
            }
        }
        return 0;
    }
}

/*****************************************************************************
函数名：void M3_Up(void)
函数功能：M3电机向上
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M3_Up(uint32_t Steps)
{
    if (Steps < 1) return 0;
    if(!DevA3983->GetOp(M3))
    {
        DevA3983->Write(M3, 8300, 15000, 2000, 4000, 10, 0, (M_SEN | M_CW | M_DIV8));//2000/5000
        DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
        if(!DevA3983->GetOp(M3))
        {
            vTaskDelay(10);
            if(!DevA3983->GetOp(M3))
            {
                DevA3983->Write(M3, 1500, 1500,1000, 1000, 10, 0,(M_STP | M_CCW | M_DIV8));
                DevA3983->Write(M3, 1500, 3000, 2000, 4000, 10, 0, (M_SEN | M_CW | M_DIV8));//2000/5000
                DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
//                if(!DevA3983->GetOp(M3))
//                {
//                    C7SetErr(ERR_C7_M3MOTOR);
//                    return 1;
//                }
            }
        }
    }
    DevA3983->Write(M3, Steps, Steps, 2000, 4000, 10, 0, (M_STP | M_CCW | M_DIV8));//3000/6000
//    if(DevA3983->GetOp(M3))
//    {
//        vTaskDelay(5);
//        if(DevA3983->GetOp(M3))
//        {
//            C7SetErr(ERR_C7_M3MOTOR);
//            return 1;
//        }
//    }

    return 0;
}

/*****************************************************************************
函数名：void M3_Down(void)
函数功能：M3电机向下
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M3_Down(uint32_t Steps)
{
    if (Steps < 1) return 0;
    if(!DevA3983->GetOp(M3))
    {
        DevA3983->Write(M3, Steps, 15000, 3500, 6000, 10, 0, (M_SEN | M_CW | M_DIV8));//3000/6000
        DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
        if(!DevA3983->GetOp(M3))
        {
            vTaskDelay(10);
            if(!DevA3983->GetOp(M3))
            {
                DevA3983->Write(M3, 1500, 1500,1000, 1000, 10, 0,(M_STP | M_CCW | M_DIV8));
                DevA3983->Write(M3, 1500, 3000, 2000, 4000, 10, 0, (M_SEN | M_CW | M_DIV8));//2000/5000
                DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
//                if(!DevA3983->GetOp(M3))
//                {
//                    C7SetErr(ERR_C7_M3MOTOR);
//                    return 1;
//                }
            }
        }
    }

    return 0;
}

/*****************************************************************************
函数名：void M3_Down(void)
函数功能：M3电机向下到冲洗位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M3_DownToWash(uint32_t Steps)
{
    if (Steps < 1) return 0;
    if(DevA3983->GetOp(M3))
    {
        return 0;
    }
    else
    {
        DevA3983->Write(M3, Steps, Steps, 2000, 2000, 10, 0, (M_CW | M_DIV8));//2000/5000
    }
    return 0;
}

/*****************************************************************************
函数名：void M3_Down(void)
函数功能：M3电机向下到冲洗位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M3_UpTOWash(uint32_t Steps)
{
    if (Steps < 1) return 0;
    if(DevA3983->GetOp(M3))
    {
        return 0;
    }
    else
    {
        DevA3983->Write(M3, Steps, Steps, 2000, 2000, 10, 0, (M_CCW | M_DIV8));//2000/5000
    }
    return 0;
}

/*****************************************************************************
函数名：void M3_Down(void)
函数功能：M3电机向下到吹风位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M3_DownToFan(uint32_t Steps)
{
    if (Steps < 1) return 0;
    if(DevA3983->GetOp(M3))
    {
        return 0;
    }
    else
    {
        DevA3983->Write(M3, Steps, Steps, 2000, 2000, 10, 0, (M_CW | M_DIV8));//2000/5000
    }
    return 0;
}

/*****************************************************************************
函数名：void M3_Rest(void)
函数功能：M3电机复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M3_Rest(void)
{
    if(DevA3983->GetOp(M3))
    {
        DevA3983->Write(M3, 2500, 2500, 1000, 1000, 10, 0, (M_STP | M_CCW | M_DIV8));
        if(DevA3983->GetOp(M3))
        {
            vTaskDelay(10);
            if(DevA3983->GetOp(M3))
            {
                C7SetErr(ERR_C7_M3MOTOR);
                return 1;
            }
        }
        DevA3983->Write(M3, 2500, 4000,1000, 1000, 10, 0, (M_SEN | M_CW | M_DIV8));
        DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
        if(!DevA3983->GetOp(M3))
        {
            vTaskDelay(10);
            if(!DevA3983->GetOp(M3))
            {
                DevA3983->Write(M3, 2500, 2500,1000, 1000, 10, 0,(M_STP | M_CCW | M_DIV8));
                DevA3983->Write(M3, 2500, 4000, 2000, 4000, 10, 0, (M_SEN | M_CW | M_DIV8));//2000/5000
                DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
//                if(!DevA3983->GetOp(M3))
//                {
//                    C7SetErr(ERR_C7_M3MOTOR);
//                    return 1;
//                }
            }
        }
    }
    else
    {
        DevA3983->Write(M3, 2000, 15000, 1000, 3000, 10, 0, (M_SEN | M_CW | M_DIV8));//3000
        DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
        if(!DevA3983->GetOp(M3))
        {
            vTaskDelay(10);
            if(!DevA3983->GetOp(M3))
            {
                DevA3983->Write(M3, 2500, 2500,1000, 1000, 10, 0,(M_STP | M_CCW | M_DIV8));
                DevA3983->Write(M3, 2500, 4000, 2000, 4000, 10, 0, (M_SEN | M_CW | M_DIV8));//2000/5000
                DevA3983->Write(M3, 550, 550,1000, 1000, 10, 0,(M_STP | M_CW | M_DIV8));
                if(!DevA3983->GetOp(M3))
                {
                    C7SetErr(ERR_C7_M3MOTOR);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*****************************************************************************
函数名：int ReadM4OpSta(void)
函数功能：用于过滤光耦判断
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
int ReadM4OpSta(void)
{
    u8 bCount = 0;
    for(int i = 0;i < 5;i++)
    {
        if(DevA3983->GetOp(M4)) bCount++;  
        vTaskDelay(2);
    }
    if(bCount > 3) return 1;
    return 0;
}


/*****************************************************************************
函数名：void M4_Rest(void)
函数功能：M4复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M4_Rest(uint32_t Steps)
{
    if(Steps < 1) return 0;
    DevA3983->SetOp(M4, &BIT_GPIOD_IDR_IDR7, 1);//设置找C6光耦，判断压住行程开关
    vTaskDelay(5);
//    if(DevA3983->GetOp(M4))//压住行程光耦
    if(ReadM4OpSta())//压住行程光耦
    {
        DevA3983->SetOp(M4, &BIT_GPIOB_IDR_IDR3, 0);//设置为C7光耦
        DevA3983->Write(M4, 1600, 1600, 2000, 2000, 10, 0, (M_STP |  M_CW | M_DIV8));
        DevA3983->Write(M4, 4900, 12000, 2000, 3000, 10, 0, (M_SEN |  M_CW | M_DIV8));
#if 0
	    if(ReadM4OpSta()==1)        
        {
            vTaskDelay(5);
            if(DevA3983->GetOp(M4)==1)
            {
                C7SetErr(ERR_C7_M4MOTOR_ERR4);
              //  return 1;
            }
        }
#endif
        
        DevA3983->Write(M4, Steps, Steps, 2000, 2000, 10, 0, (M_STP |  M_CW | M_DIV8));
//        if(DevA3983->GetOp(M4))
        if(ReadM4OpSta())        
        {
            vTaskDelay(5);
            if(DevA3983->GetOp(M4))
            {
                C7SetErr(ERR_C7_M4MOTOR);
                return 1;
            }
        }
    }
    else//不压住行程开关，光耦挡片处于光耦中间挡住状态
    {
        DevA3983->SetOp(M4, &BIT_GPIOB_IDR_IDR3, 0);//设置找C7光耦
//        if(DevA3983->GetOp(M4))//如果在中间档住光耦
        if(ReadM4OpSta())  
        {
            DevA3983->Write(M4, 3510, 12000, 2000, 3000, 10, 0, (M_SEN | M_CW | M_DIV8));//直接复位
            DevA3983->Write(M4, Steps, Steps, 2000, 2000, 10, 0,(M_STP | M_CW | M_DIV8));//复位完继续走固定步数
//            if(DevA3983->GetOp(M4))//出错。光耦还被挡住
           if(ReadM4OpSta())  
            {
                vTaskDelay(5);
                DevA3983->Write(M4, 1500 + Steps, 1500 + Steps, 2000, 2000, 10, 0, (M_STP | M_CCW | M_DIV8));//移出
                DevA3983->Write(M4, 1500 + Steps, 5000, 2000, 2000, 10, 0, (M_SEN | M_CW | M_DIV8));//重新找光耦
#if 0
			    if(ReadM4OpSta()==1)        
                {
                    vTaskDelay(5);
                    if(DevA3983->GetOp(M4)==1)
                    {
                        C7SetErr(ERR_C7_M4MOTOR_ERR4);
                      //  return 1;
                    }
                }
#endif
                DevA3983->Write(M4, Steps, Steps, 2000, 2000, 10, 0,(M_STP | M_CW | M_DIV8));
                if(DevA3983->GetOp(M4))
                {
                    C7SetErr(ERR_C7_M4MOTOR_ERR2);
                    return 1;
                }
            }
        }
        else//如果为脱离光耦，分两种情况脱离，一个是C7位置，一个是C6位置脱离
        {
            DevA3983->Write(M4, 1500 + Steps, 1500 + Steps, 2000, 2000, 10, 0, (M_STP|M_CCW | M_DIV8));//往外1500步脱离光耦
//            if(DevA3983->GetOp(M4))//档住光耦,判断位靠近C7端
            if(ReadM4OpSta())  
            {
                DevA3983->Write(M4, 1500 + Steps, 12000, 2000, 3000, 10, 0, (M_SEN | M_CW | M_DIV8));//直接复位
#if 0
			    if(ReadM4OpSta()==1)        
                {
                    vTaskDelay(5);
                    if(DevA3983->GetOp(M4)==1)
                    {
                        C7SetErr(ERR_C7_M4MOTOR_ERR4);
                      //  return 1;
                    }
                }
#endif
                DevA3983->Write(M4, Steps, Steps, 2000, 2000, 10, 0, (M_STP | M_CW | M_DIV8));//复位完继续走固定步数
            }
//            if(DevA3983->GetOp(M4))
            if(ReadM4OpSta())  
            {
                vTaskDelay(5);
                DevA3983->Write(M4, 1500 + Steps, 1500 + Steps, 2000, 2000, 10, 0, (M_STP | M_CCW | M_DIV8));
                DevA3983->Write(M4, 1500 + Steps, 6000, 2000, 2000, 10, 0, (M_SEN | M_CW | M_DIV8));
#if 0
			    if(ReadM4OpSta()==1)        
                {
                    vTaskDelay(5);
                    if(DevA3983->GetOp(M4)==1)
                    {
                        C7SetErr(ERR_C7_M4MOTOR_ERR4);
                      //  return 1;
                    }
                }
#endif
                DevA3983->Write(M4, Steps, Steps, 2000, 2000, 10, 0,(M_STP | M_CW | M_DIV8));
                if(DevA3983->GetOp(M4))
                {
                    C7SetErr(ERR_C7_M4MOTOR_ERR3);
                    return 1;
                }
            }
        }
    }

    return 0;
}

/*****************************************************************************
函数名：void M4_Motion(u16 Steps)
函数功能：M4复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M4_Motion(void )
{
    DevA3983->SetOp(M4, &BIT_GPIOB_IDR_IDR3, 0);//新电路板
    if(DevA3983->GetOp(M4))//在中间档住光耦
    {
        DevA3983->Write(M4, 4500, 12000, 2000, 2000, 10, 0, (M_SEN | M_CW | M_DIV8));
        DevA3983->Write(M4, 120, 120, 2000, 2000, 10, 0, (M_STP | M_CW | M_DIV8));
    }
    DevA3983->SetOp(M4, &BIT_GPIOD_IDR_IDR7, 1);//新电路板

    DevA3983->Write(M4, 6000, 12000, 2000, 2000, 10, 0, (M_SEN | M_CCW| M_DIV8));
    


    DevA3983->Write(M4, 120, 120, 2000, 2000, 10, 0, (M_STP | M_CCW | M_DIV8));
   // if(!DevA3983->GetOp(M4))
   // {
    //    vTaskDelay(5);  
    //    if(!DevA3983->GetOp(M4))
    //    {
   //         C7SetErr(ERR_C7_M4MOTOR);
   //         return 1;
    //    }
    //}

    return 0;
}


/*****************************************************************************
函数名：void M5_Rest(void)
函数功能：M5复位
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M5_Rest(void)
{
//    if(DevA3983->GetOp(M5))
//    {
//        DevA3983->Write(M5, 1400, 3000, 400, 1000, 4, 0, (M_SEN | M_CCW | M_DIV8));
//        if(DevA3983->GetOp(M5))
//        {
//            vTaskDelay(1000);
//            DevA3983->Write(M5, 1400, 2600, 400, 1000, 4, 0, (M_SEN | M_CCW | M_DIV8));
//            if(DevA3983->GetOp(M5))
//            {
//                C7SetErr(ERR_C7_M5MOTOR);
//            }
//            return 1;
//        }
//    }
    return 0;
}

/*****************************************************************************
函数名：void M5_GetIoVal(void)
函数功能：检测滴镜油过程中是否有气泡
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void M5_GetIoVal(void)
{
    if(OpSensor.CheckOilSta() == 0) CtrlVarTdf.wOilValCount++;//新电路板
//    if(CtrlVarTdf.wOilValCount>0 && CtrlVarTdf.bNoOilSta == false)
//    {
//        CtrlVarTdf.bNoOilSta = true;
//        C7SetErr(C7_NoOilErr);
//    }
}

/*****************************************************************************
函数名：static int M5_DropStart()
函数功能：滴镜油
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M5_DropStart(unsigned int CountIn)
{
    if(CountIn > 3) CountIn = 3;
    for(int i = 0; i < CountIn; i++)
    {
        vTaskDelay(50);
        CtrlVarTdf.wOilValCount = 0;
        DevA3983->Write(M5, 1600, 1600, 500, 500, 4, M5_GetIoVal, (M_STP | M_CCW | M_DIV8));//M5_GetIoVal
    }
#if(bOldVer)
    {
        if((CtrlVarTdf.wOilValCount > 500) && (CtrlVarTdf.bNoOilSta == false))
        {
            CtrlVarTdf.bNoOilSta = true;
            C7SetErr(ERR_C7_NOOIL);
        }
    }
#endif
   return 0;
}

/*****************************************************************************
函数名：static void M5_FullOil()
函数功能：充满镜油
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int M5_FullOil(void)
{
    int wRunCount = 0;
    int wCheckCount = 0;
    vTaskDelay(20);
//    for(int i = 0; i < 10; i++)
//    {
//        vTaskDelay(100);
//        DevA3983->Write(M5, 1600, 1600, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
//        if((OpSensor.CheckOilSta() == 0)) wRunCount = i;
//    }
    for(int i = 0;i < 45; i++)
    {
        DevA3983->Write(M5, 1600, 1600, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
        vTaskDelay(100);
#if(bOldVer)
        {
            if(OpSensor.CheckOilSta() == 1) wCheckCount ++;
            else wCheckCount = 0;
            if(wCheckCount > 3)
            {
                wCheckCount = 0;
                CtrlVarTdf.bNoOilSta = false;
                break; 
            }
        }
#endif
    }
//        if(OpSensor.CheckOilSta() == 1)
//        {
//            wCheckCount ++;
//        }
//        else
//        {
//            wCheckCount = 0;
//        }
//
//        if((wCheckCount == 3) && (i < 4)) //可能误检
//        {
//            wRunCount = wRunCount + 1;
//            break;
//        }
//        else if((wCheckCount == 3) && (i > 4))
//        {
//#if OLD_BOARD
//            wRunCount = 60;
//#else
//            wRunCount = 10;
//#endif
//            break;
//        }
//    }
//    for(int i = 0; i < wRunCount; i++)
//    {
//        DevA3983->Write(M5, 1600, 1600, 500, 500, 10, 0, (M_STP | M_CCW | M_DIV8));
//        vTaskDelay(100);
//    }
    
#if(bOldVer)
    {
        if(OpSensor.CheckOilSta() == 0)
        {
            CtrlVarTdf.bNoOilSta = false;
            C7SetErr(ERR_C7_ADDOIL);
        }
    }
#endif

    return 0;

}

/*****************************************************************************
函数名：static void ResetAll()
函数功能：复位所有电机
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int ResetAll(uint32_t Steps)
{
    memset(&CtrlVarTdf, 0, sizeof(CtrlVarTdf));
    if(M3_Rest())  return 1;
    if(M2_Rest())  return 1;
    if(M1_Rest(Steps))  return 1;
    if(M4_Rest(g_pstC7SysPare->wM4Steps))  return 1;
    if(M5_Rest())  return 1;
#if(bOldVer)
    if(OpSensor.CheckOilSta() == 0)
    {
        CtrlVarTdf.bNoOilSta = false;
        C7SetErr(ERR_C7_NOOIL);
    }
#endif
    return 0;
}

/*****************************************************************************
函数名：static void MotorMove(uint8_t dwHwnd，uint8_t byCtr, uint32_t Steps)
函数功能：电机运行函数
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void MotorMove(uint8_t dwHwnd, uint8_t byCtr, uint32_t Steps)
{
    switch(dwHwnd)
    {
    case 0:
        if(byCtr) DevA3983->Write(M1, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CW | M_DIV8));
        else DevA3983->Write(M1, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CCW | M_DIV8));
        break;
    case 1:
        if(byCtr) DevA3983->Write(M2, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CCW | M_DIV8));
        else DevA3983->Write(M2, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CW | M_DIV8));
        break;
    case 2:
        if(byCtr) DevA3983->Write(M3, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CCW | M_DIV8));
        else DevA3983->Write(M3, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CW | M_DIV8));
        break;
    case 3:
        if(byCtr) DevA3983->Write(M4, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CCW | M_DIV8));
        else DevA3983->Write(M4, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CW | M_DIV8));
        break;
    case 4:
        if(byCtr) DevA3983->Write(M5, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CCW | M_DIV8));
        else DevA3983->Write(M5, Steps, Steps, 1000, 1000, 1, 0, (M_STP | M_CW | M_DIV8));
        break;
    default:break;
    }
}

/*****************************************************************************
函数名：static void C7TransferToC6()
函数功能：复位所有电机
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static int C7TransferToC6(void)
{
    
    ResetAll(g_pstC7SysPare->wM1RestToDripStep);
    unsigned char bCount = 0; 
    bCount = g_stC7.stOut.byDropOpc;
    M1_To_Opc(g_pstC7SysPare->wM1ToDripStep, 0);
    if(bCount != g_stC7.stOut.byDropOpc)
    {
        M1_Opc_To_Drop(g_pstC7SysPare->wM1ToDripStep);
        for(int i = 0; i < 9; i++)
        {
            M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
        }
        M5_DropStart(g_pstC7SysPare->OilVolume);
        M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
        M4_Motion();
    }
    return 0;
}

/*****************************************************************************
函数名：static void GetOp()
函数功能：获得光耦值
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void GetOp(uint16_t *DataOut)
{
    g_stC7.stOut.bySwapOutLen = 12;
    DataOut[0] = (u16)DevA3983->OnSensor(M1);//M1复位光耦
    DataOut[1] =  DevIo->Read(PB,5);//M1外复位光耦
    DataOut[2] = (u16)DevA3983->OnSensor(M2);//M2复位光耦
    DataOut[3] = (u16)DevA3983->OnSensor(M3);//M3复位光耦
    DataOut[4] = (u16)DevA3983->OnSensor(M4);//M4复位光耦
    DataOut[5] = DevIo->Read(PD,6);//试纸盒光耦
    DataOut[6] = DevIo->Read(PD,2)? 0:1;//滴样位光耦
    DataOut[7] = DevIo->Read(PD,3)? 0:1;//染剂位光耦
    DataOut[8] = DevIo->Read(PD,4)? 0:1;//滴镜油位光耦
    DataOut[9] = (u16)DevA3983->OnSensor(M5);//M5复位光耦
//    DataOut[11] = DevIo->Read(PB,1);//检测镜油有无光耦
    DataOut[10] = DevIo->Read(PD,8);//检测镜油有无光耦
    DataOut[11] = DevIo->Read(PD,7);//M4外侧光耦
}


static int C7Ageing(void)
{
    int ret = 0;
    int bCount = 0;
    if(Ageingrunning == true) return 0 ;
    if((Ageing == 1) && (Ageingrunning == false))
    {
        Ageingrunning = true;
        if(bFrist == 0)
        {
            ret = ResetAll(g_pstC7SysPare->wM1RestToDripStep);
            bFrist = 1;
            if(ret){
                Ageingrunning = false;
                Ageing = 0;
                return 1;
            }
        }
        bCount = g_stC7.stOut.byDropOpc;
        ret = M1_To_Opc(g_pstC7SysPare->wM1ToDripStep, 1);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret =M1_Opc_To_Drop(g_pstC7SysPare->wM1ToDripStep);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret = M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret = M3_Up(g_pstC7SysPare->wM3UpStep);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret = M3_Down(g_pstC7SysPare->wM3UpStep);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret = M5_DropStart(g_pstC7SysPare->OilVolume);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret = M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
        ret = M4_Motion();
        if(ret){
            Ageingrunning = false;
            Ageing = 0;
            return 1;
        }
    }
    Ageingrunning = false;
    return 0;
}

const MotorCtrlTdf MotorCtrl = {
    Init,
    M1_To_Drop,
    M1_Opc_To_Drop,
    M1_To_Opc,
    M1_Rest_To_Drop,
    //M1_To_Heating,
    M1_Rest,
    M1_DebugRest,
    M2_Rest,
    M2_MoveOneStep,
    M3_Up,
    M3_Down,
    M3_DownToWash,
    M3_UpTOWash,
    M3_Rest,
    ResetAll,
    GetOp,
    M4_Rest,
    M4_Motion,
    M5_Rest,
    M5_DropStart,
    M5_FullOil,
    C7SetErr,
    Get_ErrLevel,
    MotorMove,
    C7TransferToC6,
    M3_DownToFan,
    C7Ageing,
    M2_MoveStep,
};
