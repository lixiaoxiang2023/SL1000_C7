/*******************************************************************************
文件名称：   Temperature.c
文件目录：
项目名称：
创建者：     廖江伟
创建日期：   2020.6.4
程序用途：   温控模块控制任务

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "stm32Bit.h"
#include "temperature.h"

#include "devplat.h"
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define Pcircle     20

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
//extern stCoreParTypeTdf                s_stCorePar;

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
struct
{
    stTempInit stInit;

    short signed int wTargetTemp;    //目标温度
    short signed int wCurTemp;       //当前温度
    short signed int wPtcTemp;
    SwType  eSw;        //开关状态
//    CtrlStaType eCtrlSta;//工作状态：制冷或加热
    stPID pstPid;//PID控制参数
    char DutyCir;
    int liPeridCnt;

}TempCtrl;


/*******************************************************************************
宏定义函数声明
*******************************************************************************/


/*******************************************************************************
内部函数声明
*******************************************************************************/
uint16_t PidOut(int TempDiff);
//static void CtrlParaInit(void);
static int GetTempDiff(void);
static void CalcDuty(void);
/*******************************************************************************
输出函数声明
*******************************************************************************/


/*****************************************************************************
函数名：static uint16_t PidOut(int TempDiff)
函数功能：根据PID值计算占空比
参数描述：
返回值描述：占空比，百分制
特殊算法：…
注意事项：
******************************************************************************/
//static uint16_t PidOut(int TempDiff)
//{
//	int iErr = 0;
//	float fPidOut = 0;
//
//	iErr = TempDiff;
//	TempCtrl.pstPid.iSumError += iErr;
//	TempCtrl.pstPid.iPresentError = iErr;
//
//	fPidOut = TempCtrl.pstPid.wKp * TempCtrl.pstPid.iPresentError +\
//        TempCtrl.pstPid.wKi * TempCtrl.pstPid.iSumError +\
//            TempCtrl.pstPid.wKd *(TempCtrl.pstPid.iPresentError - TempCtrl.pstPid.iLastError);
//
//	TempCtrl.pstPid.iLastError = TempCtrl.pstPid.iPresentError;
//
//	fPidOut = (fPidOut*10);
//	fPidOut = (fPidOut>0)?(fPidOut):(-fPidOut);
//	if(fPidOut > 100)
//        fPidOut = 100;
//	return (uint16_t)fPidOut;
//}


/*****************************************************************************
函数名：static void SetTargetTemp(short signed int Temp)
函数功能：写入目标温度
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void SetTargetTemp(short signed int Temp)
{
    TempCtrl.wTargetTemp = Temp;
}


/*****************************************************************************
函数名：static void SetCurTemp(short signed int Temp)
函数功能：写入当前温度
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void SetCurTemp(short signed int Temp)
{
    TempCtrl.wCurTemp = Temp;
}

/*****************************************************************************
函数名：static void SetPtcTemp(short signed int Temp)
函数功能：写入当前温度
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void SetPtcTemp(short signed int Temp)
{
    TempCtrl.wPtcTemp = Temp;
}


/*****************************************************************************
函数名：static void SetCurTemp(short signed int Temp)
函数功能：写入当前温度
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static short signed int GetTargetTemp(void)
{
    return TempCtrl.wTargetTemp ;
}

/*****************************************************************************
函数名：static short signed int GetCurTemp(void)
函数功能：获取当前温度
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static short signed int GetCurTemp(void)
{
    return TempCtrl.wCurTemp;
}

/*****************************************************************************
函数名：static short signed int GetPtcTemp(void)
函数功能：获取PTC温度
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static short signed int GetPtcTemp(void)
{
    return TempCtrl.wPtcTemp;
}


/*****************************************************************************
函数名：static int GetPrd(void)
函数功能：获取当前周期值
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static int GetPrd(void)
{
    return TempCtrl.liPeridCnt;
}

/*****************************************************************************
函数名：static void Setkp(float kp)
函数功能：写入PID比例参数
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Setkp(float kp)
{
    TempCtrl.pstPid.wKp = kp;
}

/*****************************************************************************
函数名：static void Setki(float ki)
函数功能：写入PID积分参数
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Setki(float ki)
{
        TempCtrl.pstPid.wKi = ki;
}

/*****************************************************************************
函数名：static void Setkd(float kd)
函数功能：写入PID微分参数
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Setkd(float kd)
{
        TempCtrl.pstPid.wKd = kd;
}


/*****************************************************************************
函数名：static void Heating(void)
函数功能：开启加热
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Heating(void)
{
    if(TempCtrl.liPeridCnt <= TempCtrl.DutyCir)
    {
    *TempCtrl.stInit.Ptc = 0;
    *TempCtrl.stInit.Ptc = 1;
    }
    else
    {
        *TempCtrl.stInit.Ptc = 0;
    }

}

/*****************************************************************************
函数名：static void Cooling(void)
函数功能：开启降温
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Cooling(void)
{
    *TempCtrl.stInit.Ptc = 0;
}


/*****************************************************************************
函数名：static void Start(void)
函数功能：开启温控
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Start(void)
{
    TempCtrl.eSw = eOn;
}

/*****************************************************************************
函数名：static void Stop(void)
函数功能：关闭温控
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void Stop(void)
{
    TempCtrl.eSw = eOff;
    Cooling();
}



/*****************************************************************************
函数名：static int GetTempDiff(void)
函数功能：获取温差
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static int GetTempDiff(void)
{
    int lTempDiff;

    lTempDiff = TempCtrl.wTargetTemp - TempCtrl.wCurTemp;
    return lTempDiff;
}


static void CalcDuty(void)
{
    if(TempCtrl.wCurTemp < 340)      TempCtrl.DutyCir = 10;
    else                             TempCtrl.DutyCir = 7;
}

static void SensorFaultCheck(void)
{
    static int byCnt = 0;
    if(TempCtrl.wCurTemp == 0)
    {
        byCnt++;
    }
	else
	{
		byCnt = 0;
	}
    if(byCnt > 500)  TempCtrl.eSw = eOff;
}

static void OutputDebugInfo(void)
{
//    char str[200];
//    int len;
//
//    sprintf(str,"Setting :%d Current :%d Duty:%d\r\n",TempCtrl.wTargetTemp,TempCtrl.wCurTemp,TempCtrl.DutyCir,TempCtrl.wPtcTemp);
//    len = strlen(str);
//    DevUart->SendString(0,(u8*)str,len);
}


/*****************************************************************************
函数名：static void CtrlProc(void)
函数功能：温度控制程序
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void CtrlProc(void)
{
    int liTempDiff; //温差

    if( TempCtrl.eSw == eOff )
    {
         return; //检查开关状态
    }

    liTempDiff = GetTempDiff();       //计算温差
    CalcDuty();

    if(liTempDiff > 0)   //加热
    {
        {
            Heating();
            SensorFaultCheck();
        }
    }
    else//散热
    {
        Cooling();
    }

    TempCtrl.liPeridCnt++;
    if(TempCtrl.liPeridCnt == 10) TempCtrl.liPeridCnt = 0;

}



DevTempType DevTemp =
{
    SetTargetTemp,
    SetCurTemp,
    SetPtcTemp,

    Setkp,
    Setki,
    Setkd,

    GetTargetTemp,
    GetCurTemp,
    GetPtcTemp,
    GetPrd,

    Start,
    Stop,

    CtrlProc,
    Cooling,
    OutputDebugInfo,
};



DevTempType *TempCtrlDevInit(stTempInit pstInit)
{
    TempCtrl.stInit = pstInit;

    return &DevTemp;
}

