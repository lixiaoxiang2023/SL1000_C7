/*******************************************************************************
文件名称：   pw.c
文件目录：   $PROJ_DIR$/../code/usr/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2017.10.28
程序用途：   基于stm32 pw电源控制模块的通用驱动

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "dev.h"

#ifdef PLAT_STM32_F1
#if(CONFIG_PW)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量定义
*******************************************************************************/
static struct {
    void (*pvHandler) (unsigned int);
}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

#if(CONFIG_PW_DEINIT)
/*******************************************************************************
*函数功能：
*    @ 复位
*参数描述：
*    无
*返回值描述：
*    无
*说明：
*    无
*******************************************************************************/
static void Deinit(void)
{
    RCC->BitAPB1RSTR.PWRRST = 1;
    RCC->BitAPB1RSTR.PWRRST = 0;
}
#endif

#if(CONFIG_PW_PVD_EVENT_UP)
/*******************************************************************************
函数功能：
*   @ 向电源接口增加监控事件
参数描述：
*   @ unsigned int dwLevel：监控的电平，22为2.2V，范围 22-29
*   @ void (*pvHandler) (int)：电源异常时事件通知，参数：0电源上升，1电源下降
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int PvdEventUp(unsigned int dwLevel, void (*pvHandler) (unsigned int))
{
    unsigned char byLevel = (dwLevel - 22);

    if(byLevel > 7) byLevel = 0;
    PWR->BitCR.PLS = byLevel;

    // 允许上下边缘出发
    EXTI->BitFTSR.FTSR16 = 1;
    EXTI->BitRTSR.RTSR16 = 1;

    DevIo->NvicUp(PVD_IRQn, 10, 0);
    s_stCtr.pvHandler = pvHandler;

    // 开启PVD监控
    PWR->BitCR.PVDE = 1;
    return 0;
}
#endif

#if(CONFIG_PW_PVD_EVENT_DOWN)
/*******************************************************************************
函数功能：
*   @ 关闭电源监控事件
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int PvdEventDown(void)
{
    // 关闭PVD监控
    PWR->BitCR.PVDE = 0;

    // 禁止上下边缘出发
    EXTI->BitFTSR.FTSR16 = 0;
    EXTI->BitRTSR.RTSR16 = 0;

    DevIo->NvicDown(PVD_IRQn);
    s_stCtr.pvHandler = NULL;

    return 0;
}
#endif

#if(CONFIG_PW_WAKEUP_PIN_ON)
/*******************************************************************************
函数功能：
*   @ 唤醒引脚使能
参数描述：
*   @ bool blIsOn： true 打开，false 关闭
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void WakeUpPinOn(bool blIsOn)
{
    PWR->BitCSR.EWUP = (blIsOn == true)?1:0;
}
#endif

#if(CONFIG_PW_SLEEP_NOW_WFI)
/*******************************************************************************
函数功能：
*   @ 进入sleep mode 等待中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SleepNowWFI(void)
{
    __WFI();
}
#endif

#if(CONFIG_PW_SLEEP_NOW_WFE)
/*******************************************************************************
函数功能：
*   @ 进入sleep mode 等待时间
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SleepNowWFE(void)
{
    __WFE();
}
#endif

#if(CONFIG_PW_SLEEP_ON_EXIT)
/*******************************************************************************
函数功能：
*   @ 进入sleep mode 等待中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SleepOnExit(void)
{
    SCB->BitSCR.SLEEPONEXIT = 1;
    SCB->BitSCR.SLEEPDEEP = 0;

    __WFI();
}
#endif

#if(CONFIG_PW_STOP_MODE)
/*******************************************************************************
函数功能：
*   @ CPU 进入停机模式
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void StopMode(void)
{
    PWR->BitCR.PDDS = 0;
    PWR->BitCR.LPDS = 0;
    SCB->BitSCR.SLEEPDEEP = 1;

    __WFI();
}
#endif

#if(CONFIG_PW_MODE_LOW_POWER)
/*******************************************************************************
函数功能：
*   @ CPU 进入停机模式
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void StopModeLowPower(void)
{
    PWR->BitCR.PDDS = 0;
    PWR->BitCR.LPDS = 1;
    SCB->BitSCR.SLEEPDEEP = 1;

    __WFI();
}
#endif

#if(CONFIG_PW_STANDBY_MODE)
/*******************************************************************************
函数功能：
*   @ CPU 进入待机模式
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void StandbyMode(void)
{
    PWR->BitCR.CWUF = 1;
    PWR->BitCR.PDDS = 1;

    SCB->BitSCR.SLEEPDEEP = 1;

    __WFI();
}
#endif

#if(CONFIG_PW_IS_FROM_WAKEUP)
/*******************************************************************************
函数功能：
*   @ 检查是否CPU从wakeup启动
参数描述：
*   @ 无
返回值描述：
*   @ true：从wakeup中启动
*   @ false：不是从wakeup中启动
说明：
*   @ 无
*******************************************************************************/
static bool IsFromWakeUp(void)
{
    bool blWake = (PWR->BitCSR.WUF == 0)? false:true;

    PWR->BitCSR.WUF = 0;
    return blWake;
}
#endif

/*************************************IWDG*************************************/

#if(CONFIG_PW_IWDG_CONFIG)
/*******************************************************************************
函数功能：
*   @ 检查是否CPU从wakeup启动
参数描述：
*   @ 无
返回值描述：
*   @ true：从wakeup中启动
*   @ false：不是从wakeup中启动
说明：
*   @ 无
*******************************************************************************/
static void IwdgConfig(unsigned char byIwdgPR, unsigned short int wIwdgRLR)
{
    // 调试时，关闭iwdg功能
    DBGMCU->bitCR.DBG_IWDG_STOP = 1;

    // 开启lsi时钟
    RCC->BitCSR.LSION = 1;
    while(RCC->BitCSR.LSIRDY == 0) {;}

    // 开始配置iwdg,允许读写iwdg寄存器
    IWDG->KR = 0x5555;
    while (1) {
        if((IWDG->SR & 0x01) == 0) {
            IWDG->PR = byIwdgPR;
            break;
        }
    }

    while (1) {
        if((IWDG->SR & 0x02) == 0) {
            IWDG->RLR = wIwdgRLR;
            break;
        }
    }

    IWDG->KR = 0xAAAA;
}
#endif

#if(CONFIG_PW_IWDG_UP)
/*******************************************************************************
函数功能：
*   @ 检查是否CPU从wakeup启动
参数描述：
*   @ 无
返回值描述：
*   @ true：从wakeup中启动
*   @ false：不是从wakeup中启动
说明：
*   @ 无
*******************************************************************************/
static void IwdgUp(void)
{
    IWDG->KR = 0xCCCC;
    IWDG->KR = 0xAAAA;
}
#endif

#if(CONFIG_PW_IWDG_DOWN)
/*******************************************************************************
函数功能：
*   @ 检查是否CPU从wakeup启动
参数描述：
*   @ 无
返回值描述：
*   @ true：从wakeup中启动
*   @ false：不是从wakeup中启动
说明：
*   @ 无
*******************************************************************************/
static void IwdgDown(void)
{
    IWDG->KR = 0xAAAA;

    // 关闭lsi时钟
    RCC->BitCSR.LSION = 0;
}
#endif

#if(CONFIG_PW_IWDG_FEED)
/*******************************************************************************
函数功能：
*   @ 检查是否CPU从wakeup启动
参数描述：
*   @ 无
返回值描述：
*   @ true：从wakeup中启动
*   @ false：不是从wakeup中启动
说明：
*   @ 无
*******************************************************************************/
static void IwdgFeed(void)
{
    IWDG->KR = 0xAAAA;
}
#endif


static const DevPwTdf s_Dev = {
#if(CONFIG_PW_DEINIT)
    Deinit,
#endif
#if(CONFIG_PW_PVD_EVENT_UP)
    PvdEventUp,
#endif
#if(CONFIG_PW_PVD_EVENT_DOWN)
    PvdEventDown,
#endif
#if(CONFIG_PW_WAKEUP_PIN_ON)
    WakeUpPinOn,
#endif
#if(CONFIG_PW_SLEEP_NOW_WFI)
    SleepNowWFI,
#endif
#if(CONFIG_PW_SLEEP_NOW_WFE)
    SleepNowWFE,
#endif
#if(CONFIG_PW_SLEEP_ON_EXIT)
    SleepOnExit,
#endif
#if(CONFIG_PW_STOP_MODE)
    StopMode,
#endif
#if(CONFIG_PW_MODE_LOW_POWER)
    StopModeLowPower,
#endif
#if(CONFIG_PW_STANDBY_MODE)
    StandbyMode,
#endif
#if(CONFIG_PW_IS_FROM_WAKEUP)
    IsFromWakeUp,
#endif
#if(CONFIG_PW_IWDG_CONFIG)
    IwdgConfig,
#endif
#if(CONFIG_PW_IWDG_UP)
    IwdgUp,
#endif
#if(CONFIG_PW_IWDG_DOWN)
    IwdgDown,
#endif
#if(CONFIG_PW_IWDG_FEED)
    IwdgFeed,
#endif
};

/*******************************************************************************
函数功能：
*   @ RTC实时时钟中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void PVD_IRQHandler(void)
{
    unsigned int dwOver = 0;

    if(EXTI->BitPR.PR16 == 1) {
        EXTI->BitPR.PR16 = 1;

        dwOver = (PWR->BitCSR.PVDO == 0)?1:0;
        if(s_stCtr.pvHandler) s_stCtr.pvHandler(dwOver);
    }
}

/*******************************************************************************
函数功能：
*   @ 初始化PW模块
参数描述：
*   @ 无
返回值描述：
*   @ ADC模块的操作方法集合
说明：
*   @ 无
*******************************************************************************/
const DevPwTdf *PwInit(void)
{
    RCC->BitAPB1ENR.PWREN = 1;
    return &s_Dev;
}
#endif
#endif