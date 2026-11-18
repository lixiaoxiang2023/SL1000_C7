/*****************************************************************************
文件名称：  tim.c
文件目录：  $PROJ_DIR$/usr/dev
项目名称：  事业一部技术平台
创建者：    胥强
创建日期：  2016.03.10
程序用途：  基于stm32的定时器通用平台

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

#if(CONFIG_TIM1|CONFIG_TIM2|CONFIG_TIM3|CONFIG_TIM4|CONFIG_TIM5|CONFIG_TIM6|CONFIG_TIM8\
|CONFIG_TIM9|CONFIG_TIM10|CONFIG_TIM11|CONFIG_TIM12|CONFIG_TIM13|CONFIG_TIM14)

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量定义
*******************************************************************************/
static struct
{
    TIM_TypeDef *pstTim;

    void (*pvHandler)(unsigned int);
    unsigned int dwPare;
}s_stDev[TIM_DEV_NUM];

/******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 定时器时钟使能
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void RccInit(void)
{
#if(CONFIG_TIM1)
    RCC->BitAPB2ENR.TIM1EN = 1;
#endif
#if(CONFIG_TIM2)
    RCC->BitAPB1ENR.TIM2EN = 1;
#endif
#if(CONFIG_TIM3)
    RCC->BitAPB1ENR.TIM3EN = 1;
#endif
#if(CONFIG_TIM4)
    RCC->BitAPB1ENR.TIM4EN = 1;
#endif
#if(CONFIG_TIM5)
    RCC->BitAPB1ENR.TIM5EN = 1;
#endif
#if(CONFIG_TIM6)
    RCC->BitAPB1ENR.TIM6EN = 1;
#endif
#if(CONFIG_TIM7)
    RCC->BitAPB1ENR.TIM7EN = 1;
#endif
#if(CONFIG_TIM8)
    RCC->BitAPB2ENR.TIM8EN = 1;
#endif
#if(CONFIG_TIM9)
    RCC->BitAPB2ENR.TIM9EN = 1;
#endif
#if(CONFIG_TIM10)
    RCC->BitAPB2ENR.TIM10EN = 1;
#endif
#if(CONFIG_TIM11)
    RCC->BitAPB2ENR.TIM11EN = 1;
#endif
#if(CONFIG_TIM12)
    RCC->BitAPB1ENR.TIM12EN = 1;
#endif
#if(CONFIG_TIM13)
    RCC->BitAPB1ENR.TIM13EN = 1;
#endif
#if(CONFIG_TIM14)
    RCC->BitAPB1ENR.TIM14EN = 1;
#endif
}

/*******************************************************************************
函数功能：
*   @ 定时器3中断配置
参数描述:
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void NvicInit(void)
{
#if(CONFIG_TIM1)
    #if(PLAT_STM32_F1)
        DevIo->NvicUp(TIM1_UP_IRQn, 10, 0);
    #else
        DevIo->NvicUp(TIM1_UP_TIM10_IRQn, 10, 0);
    #endif
#endif

#if(CONFIG_TIM2)
    DevIo->NvicUp(TIM2_IRQn, 10, 0);
#endif
#if(CONFIG_TIM3)
    DevIo->NvicUp(TIM3_IRQn, 10, 0);
#endif
#if(CONFIG_TIM4)
    DevIo->NvicUp(TIM4_IRQn, 10, 0);
#endif
#if(CONFIG_TIM5)
    DevIo->NvicUp(TIM5_IRQn, 10, 0);
#endif

#if(CONFIG_TIM6)
#ifdef PLAT_STM32_F1
    DevIo->NvicUp(TIM6_IRQn, 10, 0);
#endif
#ifdef PLAT_STM32_F2
    DevIo->NvicUp(TIM6_DAC_IRQn, 10, 0);
#endif
#endif

#if(CONFIG_TIM7)
    DevIo->NvicUp(TIM7_IRQn, 10, 0);
#endif

#if(CONFIG_TIM8)
    #if(PLAT_STM32_F1)
        DevIo->NvicUp(TIM8_UP_IRQn, 10, 0);
    #else
        DevIo->NvicUp(TIM8_UP_TIM13_IRQn, 10, 0);
    #endif
#endif

#if(CONFIG_TIM9)
DevIo->NvicUp(TIM1_BRK_TIM9_IRQn, 10, 0);
#endif

#if(CONFIG_TIM10)
DevIo->NvicUp(TIM1_UP_TIM10_IRQn, 10, 0);
#endif

#if(CONFIG_TIM11)
DevIo->NvicUp(TIM1_TRG_COM_TIM11_IRQn, 10, 0);
#endif

#if(CONFIG_TIM12)
DevIo->NvicUp(TIM8_BRK_TIM12_IRQn, 10, 0);
#endif

#if(CONFIG_TIM13)
DevIo->NvicUp(TIM8_UP_TIM13_IRQn, 10, 0);
#endif

#if(CONFIG_TIM14)
DevIo->NvicUp(TIM8_TRG_COM_TIM14_IRQn, 10, 0);
#endif

#if(CONFIG_TIM1_CCMR)
    DevIo->NvicUp(TIM1_CC_IRQn, 10, 0);
#endif
#if(CONFIG_TIM8_CCMR)
    DevIo->NvicUp(TIM8_CC_IRQn, 10, 0);
#endif
}

/*******************************************************************************
函数功能：
*   @ 配置定时器基本的参数，包括预分频，重新加载周期参数
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPrescaler：定时器预分频
*   @ unsigned int dwPeriod：定时器重新加载参数
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 对单个定时器进行配置
*******************************************************************************/
static int Config(unsigned int dwDev, unsigned int dwPrescaler, unsigned int dwPeriod)
{
    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;

    s_stDev[dwDev].pstTim->CR1 = 0;
    s_stDev[dwDev].pstTim->ARR = dwPeriod;
    s_stDev[dwDev].pstTim->PSC = dwPrescaler;
    s_stDev[dwDev].pstTim->BitEGR.UG = 1;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设定定时器的ARR重装载寄存器
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int wArr：定时器重新加载参数
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 对单个定时器进行配置
*******************************************************************************/
static int SetArr(unsigned int dwDev, unsigned short int wArr)
{
    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    s_stDev[dwDev].pstTim->ARR = wArr;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 使用定时器进行延时
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int wNus：需要延时的值
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 延时的时间跟Config函数设定的值相关，在本驱动中默认设定值为1uS。
*******************************************************************************/
static int Delay(unsigned int dwDev, unsigned short int wNus)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

	pstTim->CNT = 0;
    pstTim->ARR = wNus;
    pstTim->BitCR1.OPM = 1;
	pstTim->BitCR1.CEN = 1;

	while(!pstTim->BitSR.UIF);
	pstTim->BitCR1.CEN = 0;
    pstTim->BitCR1.OPM = 0; //取消单脉冲模式

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 通过定时器产生一个周期性的中断事件
参数描述：
*   @ unsigned int dwDev：定时器设备号
*   @ unsigned short int wTim：事件的时间间隔
*   @ void (*pvHandler)(unsigned char)：时间到后的回调函数
*   @ unsigned char dwPare：回调函数使用的参数
返回值描述：
*   @ -1：设备忙，参数错误
*   @ 0：成功
说明：
*   @ 周期的时间跟Config函数设定的值相关，在本驱动中默认设定值为1uS。
*******************************************************************************/
static int TimEvent(unsigned int dwDev, unsigned short int wTim, void (*pvHandler)(unsigned int), unsigned int dwPare)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

	if(wTim == 0) return -1;
    s_stDev[dwDev].pvHandler = pvHandler;
    s_stDev[dwDev].dwPare = dwPare;

    pstTim->ARR = wTim;
    pstTim->CNT = 0;
    pstTim->BitDIER.UIE = 1;
    pstTim->BitCR1.CEN = 1;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 停止定时器计时功能
参数描述：
*   @ unsigned int dwDev：需要调用的设备
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 用于停止定时器定时功能，同时将清除相应的回调函数接口与参数,在周期性事件中使用
*******************************************************************************/
static int Stop(unsigned int dwDev)
{
    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    s_stDev[dwDev].pstTim->BitCR1.CEN = 0;

	s_stDev[dwDev].pvHandler = 0;
    s_stDev[dwDev].dwPare = 0;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置定时器的PWM输出
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 定时器将产生模式7的pwm
*******************************************************************************/
static int ConfigPwm(unsigned int dwDev, unsigned int dwPwmChn)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    switch (dwPwmChn) {
    case 1:
        pstTim->BitCCER.CC1E = 1;       //关闭互补极性
        pstTim->BitCCER.CC1NE = 0;      //高电平有效
        pstTim->BitCCER.CC1P = 1;       //关闭通道输出
        pstTim->BitCCER.CC1NP = 0;      //高电平有效

        pstTim->BitOcCCMR1.OC1S = 0;    //通道配置为输出
        pstTim->BitOcCCMR1.OC1FE = 1;   //输出比较1快速使能
        pstTim->BitOcCCMR1.OC1PE = 0;   //预装载关闭
        pstTim->BitOcCCMR1.OC1M = 7;    //PWM2模式
        pstTim->BitOcCCMR1.OC1CE = 0;   //
        break;
    case 2:
        pstTim->BitCCER.CC2E = 1;       //关闭互补极性
        pstTim->BitCCER.CC2NE = 0;      //高电平有效
        pstTim->BitCCER.CC2P = 1;       //关闭通道输出
        pstTim->BitCCER.CC2NP = 0;      //高电平有效

        pstTim->BitOcCCMR1.OC2S = 0;    //通道配置为输出
        pstTim->BitOcCCMR1.OC2FE = 1;   //输出比较1快速使能
        pstTim->BitOcCCMR1.OC2PE = 0;   //预装载关闭
        pstTim->BitOcCCMR1.OC2M = 7;    //PWM2模式
        pstTim->BitOcCCMR1.OC2CE = 0;   //
        break;
    case 3:
        pstTim->BitCCER.CC3E = 1;
        pstTim->BitCCER.CC3NE = 0;
        pstTim->BitCCER.CC3P = 1;
        pstTim->BitCCER.CC3NP = 0;

        pstTim->BitOcCCMR2.OC3S = 0;
        pstTim->BitOcCCMR2.OC3FE = 1;
        pstTim->BitOcCCMR2.OC3PE = 0;
        pstTim->BitOcCCMR2.OC3M = 7;
        pstTim->BitOcCCMR2.OC3CE = 0;
        break;
    case 4:
        pstTim->BitCCER.CC4E = 1;
        pstTim->BitCCER.CC4P = 1;

        pstTim->BitOcCCMR2.OC4S = 0;
        pstTim->BitOcCCMR2.OC4FE = 1;
        pstTim->BitOcCCMR2.OC4PE = 0;
        pstTim->BitOcCCMR2.OC4M = 7;
        pstTim->BitOcCCMR2.OC4CE = 0;
        break;
    default: return -1;
    }

    pstTim->BitEGR.UG = 1;
    pstTim->SR = 0;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置定时器的PWM输出时的ARR,与比较寄存器CCRx
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 对于产生不变的占空比与频率的PWM应用只需要设定一次，对于需要变动参数的需要实时更改此值
*******************************************************************************/
static int SetArrPwm(unsigned int dwDev, unsigned int dwPwmChn, unsigned short int wArr, unsigned short int wCcr)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    pstTim->ARR = wArr;
    switch(dwPwmChn) {
    case 1: pstTim->CCR1 = wCcr; break;
    case 2: pstTim->CCR2 = wCcr; break;
    case 3: pstTim->CCR3 = wCcr; break;
    case 4: pstTim->CCR4 = wCcr; break;
    default : break;
    }
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动pwm在定时器上的输出
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 调用此接口后将产生pwm。
*******************************************************************************/
static int StartPwm(unsigned int dwDev, unsigned int dwPwmChn, void (*pvHandler)(unsigned int), unsigned int dwPare)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    if(pvHandler) {
        s_stDev[dwDev].pvHandler = pvHandler;
        s_stDev[dwDev].dwPare = dwPare;

        pstTim->BitDIER.UIE = 1; //开启中断使能
    }
    else {
        pstTim->BitDIER.UIE = 0; //开启中断使能
    }

    switch (dwPwmChn) {
    case 1: pstTim->BitOcCCMR1.OC1M = 7; break;
    case 2: pstTim->BitOcCCMR1.OC2M = 7; break;
    case 3: pstTim->BitOcCCMR2.OC3M = 7; break;
    case 4: pstTim->BitOcCCMR2.OC4M = 7; break;
    default : return -1;
    }

    pstTim->CNT = 0;
    pstTim->BitCR1.CEN = 1;
    pstTim->BitBDTR.MOE = 1;        //对于TIM1,8使能主输出使能功能
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭pwm在定时器上的输出
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 调用此接口后将关闭pwm。
*******************************************************************************/
static int StopPwm(unsigned int dwDev, unsigned int dwPwmChn)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    pstTim->BitCR1.CEN = 0;
    pstTim->SR = 0;
    pstTim->BitBDTR.MOE = 0; //对于TIM1,8使能主输出使能功能
    pstTim->BitDIER.UIE = 0; //开启中断使能

    switch (dwPwmChn) {
    case 1: pstTim->BitOcCCMR1.OC1M = 0; break;
    case 2: pstTim->BitOcCCMR1.OC2M = 0; break;
    case 3: pstTim->BitOcCCMR2.OC3M = 0; break;
    case 4: pstTim->BitOcCCMR2.OC4M = 0; break;
    default: return -1;
    }

    return 0;
}

#if(CONFIG_TIM1_CCMR|CONFIG_TIM8_CCMR)
/*******************************************************************************
函数功能：
*   @ 配置定时器的捕获输入
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
*   @ bool blIsPolarityRising：触发电平极性，true 上升，false 下降
*   @ unsigned int dwICFilter：滤波器，小于16
*   @ unsigned int dwIcpscDiv：捕获分频
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 捕获将绑定到指定的通道
*******************************************************************************/
static int ConfigCcmr(unsigned int dwDev, unsigned int dwChn, bool blIsPolarityRising, unsigned int dwICFilter, unsigned int dwIcpscDiv)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    switch (dwChn) {
    case 1:
        pstTim->BitCCER.CC1P = (blIsPolarityRising)?0:1;
        pstTim->BitIcCCMR1.IC1S = 1;
        pstTim->BitIcCCMR1.IC1F = dwICFilter;
        pstTim->BitIcCCMR1.IC1PSC = dwIcpscDiv;
        break;
    case 2:
        pstTim->BitCCER.CC2P = (blIsPolarityRising)?0:1;
        pstTim->BitIcCCMR1.IC2S = 1;
        pstTim->BitIcCCMR1.IC2F = dwICFilter;
        pstTim->BitIcCCMR1.IC2PSC = dwIcpscDiv;
        break;
    case 3:
        pstTim->BitCCER.CC3P = (blIsPolarityRising)?0:1;
        pstTim->BitIcCCMR2.IC3S = 1;
        pstTim->BitIcCCMR2.IC3F = dwICFilter;
        pstTim->BitIcCCMR2.IC3PSC = dwIcpscDiv;
        break;
    case 4:
        pstTim->BitCCER.CC4P = (blIsPolarityRising)?0:1;
        pstTim->BitIcCCMR2.IC4S = 1;
        pstTim->BitIcCCMR2.IC4F = dwICFilter;
        pstTim->BitIcCCMR2.IC4PSC = dwIcpscDiv;
        break;
    default: return -1;
    }

    pstTim->BitEGR.UG = 1;
    if(dwDev == 0) DevIo->NvicUp(TIM1_CC_IRQn, 10, 0) ;
    if(dwDev == 7) DevIo->NvicUp(TIM8_CC_IRQn, 10, 0) ;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动pwm在定时器上捕获
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 调用此接口后将产生pwm。
*******************************************************************************/
static int StartCcmr(unsigned int dwDev, unsigned int dwChn, void (*pvHandler)(unsigned int), unsigned int dwPare)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    if(pvHandler) {
        s_stDev[dwDev].pvHandler = pvHandler;
        s_stDev[dwDev].dwPare = dwPare;
    }

    switch (dwChn) {
    case 1: pstTim->BitCCER.CC1E = 1; pstTim->BitDIER.CC1IE = 1; break;//开启捕获使能
    case 2: pstTim->BitCCER.CC2E = 1; pstTim->BitDIER.CC2IE = 1; break;
    case 3: pstTim->BitCCER.CC3E = 1; pstTim->BitDIER.CC3IE = 1; break;
    case 4: pstTim->BitCCER.CC4E = 1; pstTim->BitDIER.CC4IE = 1; break;
    default : return -1;
    }

    pstTim->CNT = 0;
    pstTim->BitCR1.CEN = 1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭pwm在定时器上的输出
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwPwmChn：定时器的pwm输出通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 调用此接口后将关闭pwm。
*******************************************************************************/
static int StopCcmr(unsigned int dwDev, unsigned int dwChn)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    pstTim->BitCR1.CEN = 0;
    pstTim->SR = 0;

    switch (dwChn) {
    case 1: pstTim->BitCCER.CC1E = 0; pstTim->BitDIER.CC1IE = 0; break;//开启捕获使能
    case 2: pstTim->BitCCER.CC2E = 0; pstTim->BitDIER.CC2IE = 0; break;
    case 3: pstTim->BitCCER.CC3E = 0; pstTim->BitDIER.CC3IE = 0; break;
    case 4: pstTim->BitCCER.CC4E = 0; pstTim->BitDIER.CC4IE = 0; break;
    default : return -1;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取捕获通道的捕获值
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwChn：定时器的捕获通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static unsigned short int ReadCcrm(unsigned int dwDev, unsigned int dwChn)
{
    TIM_TypeDef *pstTim;
    unsigned short int wCcmr = 0;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    switch (dwChn) {
    case 1: wCcmr = pstTim->CCR1; break;
    case 2: wCcmr = pstTim->CCR2; break;
    case 3: wCcmr = pstTim->CCR3; break;
    case 4: wCcmr = pstTim->CCR4; break;
    default: break;
    }

    return wCcmr;
}
#endif

#if(CONFIG_ENCODE)
/*******************************************************************************
函数功能：
*   @ 配置定时器3、4编码器接口模式。
参数描述：
*   @ unsigned int dwDev：需要调用的设备
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 配置成编码器接口
*******************************************************************************/
static int ConfigEcd(unsigned int dwDev, unsigned int dwMode, unsigned int IC1Polarity, unsigned int IC2Polarity)
{
	TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    pstTim->BitSR.UIF = 0;
    pstTim->BitCR1.CEN = 0;
    pstTim->BitCCER.CC1P = 0;
    pstTim->BitCCER.CC1E = 0;
    pstTim->BitSMCR.SMS = dwMode;
    pstTim->BitIcCCMR1.IC1S = 1;
    pstTim->BitIcCCMR1.IC2S = 1;
    pstTim->BitIcCCMR1.IC1F = 4;
    pstTim->BitIcCCMR1.IC2F = 4;
    pstTim->BitIcCCMR1.IC1PSC = 0;
    pstTim->BitCCER.CC1E = 1;

    pstTim->BitCCER.CC1P = IC1Polarity;
    pstTim->BitCCER.CC2P = IC2Polarity;
    //
    pstTim->CNT = 0;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取定时器counter
参数描述：
*   @ unsigned int dwDev：需要调用的设备
返回值描述：
*   @ 计数值
说明：
*   @ 无
*******************************************************************************/
static int EcdCnt(unsigned int dwDev)
{
    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;

    return (int)s_stDev[dwDev].pstTim->CNT;
}

/*******************************************************************************
函数功能：
*   @ 读取定时器counter
参数描述：
*   @ unsigned int dwDev：需要调用的设备
返回值描述：
*   @ 计数值
说明：
*   @ 无
*******************************************************************************/
static int EcdStart(unsigned int dwDev)
{
    TIM_TypeDef *pstTim;

    if(dwDev > TIM_DEV_NUM - 1) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    pstTim->CNT = 32768;
    pstTim->BitCR1.CEN = 1;
    return 0;
}
#endif


#if(CONFIG_TIM_TRGO)
/*******************************************************************************
函数功能：
*   @ 配置定时器触发输出模式。
参数描述：
*   @ unsigned int dwDev：需要调用的设备
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int ConfigTrgo(unsigned int dwDev, unsigned int dwChn)
{
    TIM_TypeDef *pstTim;

    if(dwDev >= TIM_DEV_NUM) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    switch (dwChn) {
        case 1:
            pstTim->BitCCER.CC1E    = 1;
            pstTim->BitCCER.CC1NE   = 0;                                        // 关闭互补极性
            pstTim->BitCCER.CC1P    = 0;                                        // 高电平有效
            pstTim->BitCCER.CC1NP   = 0;                                        // 高电平有效 
                                                                                                            
            pstTim->BitOcCCMR1.OC1S  = 0;                                       // 通道配置为输出
            pstTim->BitOcCCMR1.OC1FE = 1;                                       // 输出比较1快速使能
            pstTim->BitOcCCMR1.OC1PE = 0;                                       // 预装载关闭
            pstTim->BitOcCCMR1.OC1M  = 7;                                       // PWM2模式
            pstTim->BitOcCCMR1.OC1CE = 0;                                       // 
        break;
        
        case 2:
            pstTim->BitCCER.CC2E    = 1;                                        // 关闭互补极性
            pstTim->BitCCER.CC2NE   = 0;                                        // 高电平有效
            pstTim->BitCCER.CC2P    = 0;                                        // 关闭通道输出
            pstTim->BitCCER.CC2NP   = 0;                                        // 高电平有效
                                                                                                            
            pstTim->BitOcCCMR1.OC2S  = 0;                                       // 通道配置为输出
            pstTim->BitOcCCMR1.OC2FE = 1;                                       // 输出比较1快速使能
            pstTim->BitOcCCMR1.OC2PE = 0;                                       // 预装载关闭
            pstTim->BitOcCCMR1.OC2M  = 7;                                       // PWM2模式
            pstTim->BitOcCCMR1.OC2CE = 0;                                       // 
        break;
        
        case 3:
            pstTim->BitCCER.CC3E = 1;
            pstTim->BitCCER.CC3NE = 0;
            pstTim->BitCCER.CC3P = 1;
            pstTim->BitCCER.CC3NP = 0;
            
            pstTim->BitOcCCMR2.OC3S = 0;
            pstTim->BitOcCCMR2.OC3FE = 1;
            pstTim->BitOcCCMR2.OC3PE = 0;
            pstTim->BitOcCCMR2.OC3M = 7;
            pstTim->BitOcCCMR2.OC3CE = 0;
        break;
        
        case 4:
            pstTim->BitCCER.CC4E = 1;
            pstTim->BitCCER.CC4P = 0;                                           // 高电平有效
            
            pstTim->BitOcCCMR2.OC4S = 0;                                        // 通道配置为输出
            pstTim->BitOcCCMR2.OC4FE = 1;                                       // 输出比较1快速使能
            pstTim->BitOcCCMR2.OC4PE = 0;                                       // 预装载关闭
            pstTim->BitOcCCMR2.OC4M = 7;                                        // PWM2模式
            pstTim->BitOcCCMR2.OC4CE = 0;                                       // 
        break;
        
        default:
            return -1;
    }

    pstTim->BitCR2.MMS = 3 + dwChn;                                             // OCxRef做TRGO	
    
    pstTim->CNT = 0;
    pstTim->BitEGR.UG = 1;
    pstTim->SR = 0;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动TRGO在定时器上的输出
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwChn：定时器通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int StartTrgo(unsigned int dwDev, unsigned int dwChn)
{
    TIM_TypeDef *pstTim;

    if(dwDev >= TIM_DEV_NUM) return -1;
    if(!s_stDev[dwDev].pstTim) return -1;
    pstTim = s_stDev[dwDev].pstTim;

    switch (dwChn) {                                                            // PWM2模式,>CCR,输出有效电平
        case 1: pstTim->BitOcCCMR1.OC1M = 7; break;
        case 2: pstTim->BitOcCCMR1.OC2M = 7; break;
        case 3: pstTim->BitOcCCMR2.OC3M = 7; break;
        case 4: pstTim->BitOcCCMR2.OC4M = 7; break;
        default : return -1;
    }
    
    pstTim->CNT = 0;
    pstTim->BitCR1.CEN = 1;
    pstTim->BitBDTR.MOE = 1;                                                    // 对于TIM1,8使能主输出使能功能
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭Trgo在定时器上的输出
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned int dwChn：定时器通道
返回值描述：
*   @ -1：错误
*   @  0：成功
说明：
*   @ 调用此接口后将关闭TRGO。
*******************************************************************************/
static int StopTrgo(unsigned int dwDev, unsigned int dwChn)
{
    StopPwm(dwDev, dwChn);
    return 0;
}

#endif

/*******************************************************************************
函数功能：
*   @ 注册定时器在中断后需要回调的函数
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ void (*pvHandler)(unsigned char)： 带参数的函数地址
返回值描述：
*   @ 无
说明：
*   @ 在使用周期事件函数接口，与PWM函数时间接口时可以通过中断调用此注册的接口
*******************************************************************************/
static int RegHandler(unsigned int dwDev, void (*pvHandler)(unsigned int), unsigned int dwPare)
{
    if(dwDev > TIM_DEV_NUM - 1) return -1;
    s_stDev[dwDev].pvHandler = pvHandler;
    s_stDev[dwDev].dwPare = dwPare;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 中断函数接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 中断函数在接受到数据后将自动调用此注册的函数接口。
*******************************************************************************/
#if(PLAT_STM32_F2|PLAT_STM32_F4)
void TIM1_UP_TIM10_IRQHandler(void)
#else
void TIM1_UP_IRQHandler(void)
#endif
{
#if(CONFIG_TIM1)
    if (TIM1->BitSR.UIF == 1) {
        TIM1->BitSR.UIF = 0;
        if(s_stDev[0].pvHandler) s_stDev[0].pvHandler( s_stDev[0].dwPare );
    }
#endif

#if(CONFIG_TIM10)
    if (TIM10->BitSR.UIF == 1) {
        TIM10->BitSR.UIF = 0;
        if(s_stDev[9].pvHandler) s_stDev[9].pvHandler( s_stDev[9].dwPare );
    }
#endif
}

void TIM2_IRQHandler(void)
{
    if (TIM2->BitSR.UIF == 1) {
        TIM2->BitSR.UIF = 0;
        if(s_stDev[1].pvHandler) s_stDev[1].pvHandler( s_stDev[1].dwPare );
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->BitSR.UIF == 1) {
        TIM3->BitSR.UIF = 0;
        if(s_stDev[2].pvHandler) s_stDev[2].pvHandler( s_stDev[2].dwPare );
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM4->BitSR.UIF == 1) {
        TIM4->BitSR.UIF = 0;
        if(s_stDev[3].pvHandler) s_stDev[3].pvHandler( s_stDev[3].dwPare );
    }
}

void TIM5_IRQHandler(void)
{
    if (TIM5->BitSR.UIF == 1) {
        TIM5->BitSR.UIF = 0;
        if(s_stDev[4].pvHandler) s_stDev[4].pvHandler( s_stDev[4].dwPare );
    }
}
#if(PLAT_STM32_F2|PLAT_STM32_F4)
void TIM6_DAC_IRQHandler(void)
#else
void TIM6_IRQHandler(void)
#endif
{
    if (TIM6->BitSR.UIF == 1) {
        TIM6->BitSR.UIF = 0;
        if(s_stDev[5].pvHandler) s_stDev[5].pvHandler( s_stDev[5].dwPare );
    }
}

void TIM7_IRQHandler(void)
{
    if (TIM7->BitSR.UIF == 1) {
        TIM7->BitSR.UIF = 0;
        if(s_stDev[6].pvHandler) s_stDev[6].pvHandler( s_stDev[6].dwPare );
    }
}

#if(PLAT_STM32_F1)
    void TIM8_UP_IRQHandler(void)
#else
    void TIM8_UP_TIM13_IRQHandler(void)
#endif
{
#if(CONFIG_TIM8)
    if (TIM8->BitSR.UIF == 1) {
        TIM8->BitSR.UIF = 0;
        if(s_stDev[7].pvHandler) s_stDev[7].pvHandler( s_stDev[7].dwPare );
    }
#endif

#if(CONFIG_TIM13)
    if (TIM13->BitSR.UIF == 1) {
        TIM13->BitSR.UIF = 0;
        if(s_stDev[12].pvHandler) s_stDev[12].pvHandler( s_stDev[12].dwPare );
    }
#endif
}

#if(CONFIG_TIM9)
void TIM1_BRK_TIM9_IRQHandler(void)
{
    if (TIM9->BitSR.UIF == 1) {
        TIM9->BitSR.UIF = 0;
        if(s_stDev[8].pvHandler) s_stDev[8].pvHandler( s_stDev[8].dwPare );
    }
}
#endif

#if(CONFIG_TIM11)
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
    if (TIM11->BitSR.UIF == 1) {
        TIM11->BitSR.UIF = 0;
        if(s_stDev[10].pvHandler) s_stDev[10].pvHandler( s_stDev[10].dwPare );
    }
}
#endif

#if(CONFIG_TIM12)
void TIM8_BRK_TIM12_IRQHandler(void)
{
    if (TIM12->BitSR.UIF == 1) {
        TIM12->BitSR.UIF = 0;
        if(s_stDev[11].pvHandler) s_stDev[11].pvHandler( s_stDev[11].dwPare );
    }
}
#endif

#if(CONFIG_TIM14)
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    if (TIM14->BitSR.UIF == 1) {
        TIM14->BitSR.UIF = 0;
        if(s_stDev[13].pvHandler) s_stDev[13].pvHandler( s_stDev[13].dwPare );
    }
}
#endif

#if(CONFIG_TIM1_CCMR)
void TIM1_CC_IRQHandler(void)
{
    unsigned char byCcChn = 0;

    if(TIM1->BitSR.CC1IF) {
        TIM1->BitSR.CC1IF = 0;
        byCcChn = 0;
    }
    else if(TIM1->BitSR.CC2IF) {
        TIM1->BitSR.CC2IF = 0;
        byCcChn = 1;
    }
    else if(TIM1->BitSR.CC3IF) {
        TIM1->BitSR.CC3IF = 0;
        byCcChn = 2;
    }
    else if(TIM1->BitSR.CC4IF) {
        TIM1->BitSR.CC4IF = 0;
        byCcChn = 3;
    }

    if(s_stDev[0].pvHandler) s_stDev[0].pvHandler( byCcChn );
}
#endif
#if(CONFIG_TIM8_CCMR)
void TIM8_CC_IRQHandler(void)
{
    unsigned char byCcChn = 0;

    if(TIM8->BitSR.CC1IF) {
        TIM8->BitSR.CC1IF = 0;
        byCcChn = 0;
    }
    else if(TIM8->BitSR.CC2IF) {
        TIM8->BitSR.CC2IF = 0;
        byCcChn = 1;
    }
    else if(TIM8->BitSR.CC3IF) {
        TIM8->BitSR.CC3IF = 0;
        byCcChn = 2;
    }
    else if(TIM8->BitSR.CC4IF) {
        TIM8->BitSR.CC4IF = 0;
        byCcChn = 3;
    }

    if(s_stDev[7].pvHandler) s_stDev[7].pvHandler( byCcChn );
}
#endif

/*******************************************************************************
函数功能：
*   @ 通过设备号初始化相关的设备参数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 定义设备数据。
*******************************************************************************/
static void DevInit(void)
{
#if(CONFIG_TIM1)
    s_stDev[0].pstTim = TIM1;
    s_stDev[0].pvHandler = 0;
    s_stDev[0].dwPare = 0;
#endif
#if(CONFIG_TIM2)
    s_stDev[1].pstTim = TIM2;
    s_stDev[1].pvHandler = 0;
    s_stDev[1].dwPare = 0;
#endif
#if(CONFIG_TIM3)
    s_stDev[2].pstTim = TIM3;
    s_stDev[2].pvHandler = 0;
    s_stDev[2].dwPare = 0;
#endif
#if(CONFIG_TIM4)
    s_stDev[3].pstTim = TIM4;
    s_stDev[3].pvHandler = 0;
    s_stDev[3].dwPare = 0;
#endif
#if(CONFIG_TIM5)
    s_stDev[4].pstTim = TIM5;
    s_stDev[4].pvHandler = 0;
    s_stDev[4].dwPare = 0;
#endif
#if(CONFIG_TIM6)
    s_stDev[5].pstTim = TIM6;
    s_stDev[5].pvHandler = 0;
    s_stDev[5].dwPare = 0;
#endif
#if(CONFIG_TIM7)
    s_stDev[6].pstTim = TIM7;
    s_stDev[6].pvHandler = 0;
    s_stDev[6].dwPare = 0;
#endif
#if(CONFIG_TIM8)
    s_stDev[7].pstTim = TIM8;
    s_stDev[7].pvHandler = 0;
    s_stDev[7].dwPare = 0;
#endif

#if(CONFIG_TIM9)
    s_stDev[8].pstTim = TIM9;
    s_stDev[8].pvHandler = 0;
    s_stDev[8].dwPare = 0;
#endif
#if(CONFIG_TIM10)
    s_stDev[9].pstTim = TIM10;
    s_stDev[9].pvHandler = 0;
    s_stDev[9].dwPare = 0;
#endif
#if(CONFIG_TIM11)
    s_stDev[10].pstTim = TIM11;
    s_stDev[10].pvHandler = 0;
    s_stDev[10].dwPare = 0;
#endif
#if(CONFIG_TIM12)
    s_stDev[11].pstTim = TIM12;
    s_stDev[11].pvHandler = 0;
    s_stDev[11].dwPare = 0;
#endif
#if(CONFIG_TIM13)
    s_stDev[12].pstTim = TIM13;
    s_stDev[12].pvHandler = 0;
    s_stDev[12].dwPare = 0;
#endif
#if(CONFIG_TIM14)
    s_stDev[13].pstTim = TIM14;
    s_stDev[13].pvHandler = 0;
    s_stDev[13].dwPare = 0;
#endif
}

static const DevTimTdf s_Dev = {
    Config,
    SetArr,
    Delay,
    TimEvent,
    Stop,
    ConfigPwm,
    SetArrPwm,
    StartPwm,
    StopPwm,
#if(CONFIG_TIM1_CCMR|CONFIG_TIM8_CCMR)
    ConfigCcmr,
    StartCcmr,
    StopCcmr,
    ReadCcrm,
#endif
#if(CONFIG_ENCODE)
    ConfigEcd,
    EcdCnt,
    EcdStart,
#endif
    
    RegHandler,
#if(CONFIG_TIM_TRGO)
    ConfigTrgo,
    StartTrgo,
    StopTrgo,
#endif
};

/*******************************************************************************
函数功能：
*   @ 定时器模块初始化
参数描述：
*   @ 无
返回值描述：
*   @ 定时器设备提供的操作接口
说明：
*   @ 各个定时器的配置均为不分频，重载值为71，即72M时钟，1uS中断一次
*******************************************************************************/
const DevTimTdf *TimInit(void)
{
    RccInit();
    DevInit();

#if(CONFIG_TIM1)
    Config(0, 71, 3);
#endif
#if(CONFIG_TIM2)
    Config(1, 71, 3);
#endif
#if(CONFIG_TIM3)
    Config(2, 71, 3);
#endif
#if(CONFIG_TIM4)
    Config(3, 71, 3);
#endif
#if(CONFIG_TIM5)
    Config(4, 71, 3);
#endif
#if(CONFIG_TIM6)
    Config(5, 71, 3);
#endif
#if(CONFIG_TIM7)
    Config(6, 71, 3);
#endif
#if(CONFIG_TIM8)
    Config(7, 71, 3);
#endif

#if(CONFIG_TIM9)
    Config(8, 71, 3);
#endif
#if(CONFIG_TIM10)
    Config(9, 71, 3);
#endif
#if(CONFIG_TIM11)
    Config(10, 71, 3);
#endif
#if(CONFIG_TIM12)
    Config(11, 71, 3);
#endif
#if(CONFIG_TIM13)
    Config(12, 71, 3);
#endif
#if(CONFIG_TIM14)
    Config(13, 71, 3);
#endif

    NvicInit();
    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 定时器模块初始化
参数描述：
*   @ 无
返回值描述：
*   @ 定时器设备提供的操作接口
说明：
*   @ 各个定时器的配置均为不分频，重载值为71，即72M时钟，1uS中断一次
*******************************************************************************/
const DevTimTdf *Tim2Init(void)
{
    
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;
    
    RCC->BitAPB1ENR.TIM2EN = 1;
    s_stDev[1].pstTim = TIM2;
    s_stDev[1].pvHandler = 0;
    s_stDev[1].dwPare = 0;

    DevRccGetClocksFreq(&stRccFreq);

    dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000) - 1;

    Config(1, dwPre, 3);

    DevIo->NvicUp(TIM2_IRQn, 10, 0);
    return &s_Dev;
}

#endif
