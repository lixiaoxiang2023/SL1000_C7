/*******************************************************************************
文件名称：   io.c
目录：       $PROJ_DIR$/usr/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.01.21
程序用途：   IO口的通用配置平台、应用平台

版权声明：
Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
2016.01.21:
由胥强创建
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "dev.h"

#if(PLAT_STM32_F1|PLAT_GD32_F30X)
/*******************************************************************************
Constant Definition
*******************************************************************************/
#define LSB_MASK                    ((unsigned short int)0xFFFF)
#define DBGAFR_POSITION_MASK        ((unsigned int)0x000F0000)
#define DBGAFR_SWJCFG_MASK          ((unsigned int)0xF0FFFFFF)
#define DBGAFR_LOCATION_MASK        ((unsigned int)0x00200000)
#define DBGAFR_NUMBITS_MASK         ((unsigned int)0x00100000)
#define IO_MAX_DEV                  5

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
外部变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static struct {
    GPIO_TypeDef *pstGpio;

    struct {
        unsigned char byAdcNo:4;
        unsigned char byAdcChn:4;
    }stAdc[16];

    struct {
        unsigned char byTimNo:4;
        unsigned char byTimChn:4;
    }stTim[16];
}s_staDev[5];

#if(CONFIG_IO_IRP)
static void (*s_pfHandler[16]) (unsigned int dwPare);
#endif

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 对带入的参数进行原子操作,对标记数据进行锁定
参数描述：
*   @ unsigned int *pwLock： 需要进行操作的数据地址
返回值描述：
*   @ true：数据没有被占用，可以进行使用
*   @ false：数据被占用，不能使用
说明：
*******************************************************************************/
static bool CommLock(unsigned int *pwLock, bool blWaitLock)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwDelay = 0;
    bool ret = false;

    if(!pwLock) return false;
    DevRccGetClocksFreq(&stRccFreq);

loopLock:
    while(dwDelay) {
#if(CONFIG_FREERTOS)
        SysDelay(1);
        break;
#else
        dwDelay --;
#endif
    }

    __ASM("cpsid i");// 关中断
    __ASM("cpsid f");// 关异常

    if(*pwLock == 0) {
        *pwLock = 1;
        ret = true;
    }
    else if(blWaitLock == true) {
        __ASM("cpsie i"); // 开
        __ASM("cpsie f");

        // ~1ms
        dwDelay = stRccFreq.SYSCLK / 1000;
        goto loopLock;
    }

    __ASM("cpsie i"); // 开
    __ASM("cpsie f");

    return ret;
}

/*******************************************************************************
函数功能：
*   @ 对标定的数据进行解锁
参数描述：
*   @ unsigned int *pwLock： 需要进行操作的数据地址
返回值描述：
*   @ true：数据没有被占用，可以进行使用
*   @ false：数据被占用，不能使用
说明：
*******************************************************************************/
static void CommUnLock(unsigned int *pwLock)
{
    __ASM("cpsid i");// 关中断
    __ASM("cpsid f");// 关异常

    *pwLock = 0;

    __ASM("cpsie i");//开
    __ASM("cpsie f");
}

/*******************************************************************************
函数功能：
*   @ 重映射管教功能
参数描述：
    * @arg GPIO_Remap_SPI1
    * @arg GPIO_Remap_I2C1
    * @arg GPIO_Remap_USART1
    * @arg GPIO_Remap_USART2
    * @arg GPIO_PartialRemap_USART3
    * @arg GPIO_FullRemap_USART3
    * @arg GPIO_PartialRemap_TIM1
    * @arg GPIO_FullRemap_TIM1
    * @arg GPIO_PartialRemap1_TIM2
    * @arg GPIO_PartialRemap2_TIM2
    * @arg GPIO_FullRemap_TIM2
    * @arg GPIO_PartialRemap_TIM3
    * @arg GPIO_FullRemap_TIM3
    * @arg GPIO_Remap_TIM4
    * @arg GPIO_Remap1_CAN1
    * @arg GPIO_Remap2_CAN1
    * @arg GPIO_Remap_PD01
    * @arg GPIO_Remap_TIM5CH4_LSI
    * @arg GPIO_Remap_ADC1_ETRGINJ
    * @arg GPIO_Remap_ADC1_ETRGREG
    * @arg GPIO_Remap_ADC2_ETRGINJ
    * @arg GPIO_Remap_ADC2_ETRGREG
    * @arg GPIO_Remap_SWJ_NoJTRST
    * @arg GPIO_Remap_SWJ_JTAGDisable
    * @arg GPIO_Remap_SWJ_Disable
返回值描述：
*   @ 无
说明：
*******************************************************************************/
static void RemapConfig(unsigned int GPIO_Remap, FunctionalState NewState)
{
    unsigned int tmp = 0x00, tmp1 = 0x00, tmpreg = 0x00, tmpmask = 0x00;

    tmpreg = AFIO->MAPR;
    tmpmask = (GPIO_Remap & DBGAFR_POSITION_MASK) >> 0x10;
    tmp = GPIO_Remap & LSB_MASK;

    if ((GPIO_Remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) {
        tmpreg &= DBGAFR_SWJCFG_MASK;
        AFIO->MAPR &= DBGAFR_SWJCFG_MASK;
    }
    else if ((GPIO_Remap & DBGAFR_NUMBITS_MASK) == DBGAFR_NUMBITS_MASK) {
        tmp1 = ((unsigned int)0x03) << tmpmask;
        tmpreg &= ~tmp1;
        tmpreg |= ~DBGAFR_SWJCFG_MASK;
    }
    else {
        tmpreg &= ~(tmp << ((GPIO_Remap >> 0x15)*0x10));
        tmpreg |= ~DBGAFR_SWJCFG_MASK;
    }

    if (NewState != DISABLE) {
        tmpreg |= (tmp << ((GPIO_Remap >> 0x15)*0x10));
    }

    AFIO->MAPR = tmpreg;
}

/*******************************************************************************
函数功能：
*   @ 使能中断NVIC
参数描述：
*   @ unsigned int dwTrq：   中断号
*   @ unsigned int dwPrePri：主优先级
*   @ unsigned int dwSubPri：次优先级
返回值描述：
*   @ 无
说明：
*******************************************************************************/
static int NvicUp(unsigned int dwTrq, unsigned int dwPrePri, unsigned int dwSubPri)
{
    unsigned int  tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;

    /* Compute the Corresponding IRQ Priority --------------------------------*/
    tmppriority = (0x700 - ((SCB->AIRCR) & (unsigned int)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (unsigned int)dwPrePri << tmppre;
    tmppriority |=  dwSubPri & tmpsub;
    tmppriority = tmppriority << 0x04;

    NVIC->IP[dwTrq] = tmppriority;

    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[dwTrq >> 0x05] = (unsigned int)0x01 << (dwTrq & (unsigned char)0x1F);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*******************************************************************************/
static int NvicDown(unsigned int dwTrq)
{
    NVIC->ICER[RTC_IRQn >> 0x05] = (unsigned int)0x01 << (RTC_IRQn & (unsigned char)0x1F);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取IO口的电平状态
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ IO口的电平
说明：
*******************************************************************************/
static int Read(unsigned int dwDev, unsigned int byChn)
{
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    return ((pstGpio->IDR >> byChn) & 0x01);
}

/*******************************************************************************
函数功能：
*   @ 向IO口写入电平
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ -1：失败
说明：
*******************************************************************************/
static int Write(unsigned int dwDev, unsigned int byChn, unsigned int byIn)
{
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    if(byIn) pstGpio->ODR |= (1 << byChn);
    else pstGpio->ODR &= (~(1 << byChn));

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 向IO口写入电平
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ -1：失败
说明：
*******************************************************************************/
static int WriteGPIO(unsigned int dwDev, unsigned int byIn)
{
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    pstGpio->ODR = byIn;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取IO空数据
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ -1：失败
说明：
*******************************************************************************/
static short int ReadGPIO(unsigned int dwDev)
{
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    return pstGpio->ODR;
}

/*******************************************************************************
函数功能：
*   @ 读取IO空数据
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ -1：失败
说明：
*******************************************************************************/
static short int ReadGpioTrue(unsigned int dwDev)
{
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    return pstGpio->IDR;
}

/*******************************************************************************
函数功能：
*   @ 配置GPIO口
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned int dwChnNum ： 需要同时配置的通道个数
*   @ IOMode_TypeDef：模式
返回值描述：
*   @ -1：失败
说明：
*   @ 输出的模式都被配置成50Mhz。
*******************************************************************************/
static int Config(unsigned int dwDev, unsigned int dwChn, unsigned int dwChnNum, IOMode_TypeDef Mode)
{
    unsigned int dwCR = 0;
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    for(int pos = 0; pos < dwChnNum ; pos ++, dwChn ++) {
        if(dwChn < 8) {
            dwCR = pstGpio->CRL;
            dwCR &= (~((unsigned int)0x0f << (dwChn * 4)));
            dwCR |= (((unsigned int)(Mode & 0x0f)) << (dwChn * 4));
            pstGpio->CRL = dwCR;
        }
        else {
            dwCR = pstGpio->CRH;
            dwCR &= (~((unsigned int)0x0f << ((dwChn - 8) * 4)));
            dwCR |= (((unsigned int)(Mode & 0x0f)) << ((dwChn - 8) * 4));
            pstGpio->CRH = dwCR;
        }

        if(Mode == Mode_IPU) pstGpio->BSRR = (((unsigned int)0x01) << dwChn);
        if(Mode == Mode_IPD) pstGpio->BRR = (((unsigned int)0x01) << dwChn);
    }

    return 0;
}

#if(CONFIG_IO_IRP)
/*******************************************************************************
函数功能：
*   @ 向IO口增加中断功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ IoEXTITriggerTdf Trigger ： IO中断触发方式
*   @ void (*pfHandler)(void) ： 触发时的回调函数接口。
返回值描述：
*   @ -1：失败
说明：
*   @ 向IO口增加回调函数接口
*******************************************************************************/
static int AddIrp(unsigned int dwDev, unsigned int byChn, IoEXTITriggerTdf Trigger, void (*pfHandler)(unsigned int dwPare))
{
    unsigned int tmp = 0x00;
    unsigned int dwIrq = 0;
    unsigned int dwMaskAnd = (((unsigned int) 0x01) << byChn);
    unsigned int dwMaskNo = ~(((unsigned int) 0x01) << byChn);

    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    // Config(dwDev, byChn, 1, Mode_IPU);
    tmp = ((unsigned int)0x0F) << (0x04 * (byChn & (unsigned char)0x03));
    AFIO->EXTICR[byChn >> 0x02] &= ~tmp;
    AFIO->EXTICR[byChn >> 0x02] |= (((unsigned int)dwDev) << (0x04 * (byChn % 4)));

    EXTI->IMR |= dwMaskAnd;
    EXTI->EMR &= dwMaskNo;

    EXTI->RTSR &= dwMaskNo;
    EXTI->FTSR &= dwMaskNo;

    if(Trigger == Trigger_Rising) EXTI->RTSR |= dwMaskAnd;
    else if(Trigger == Trigger_Falling) EXTI->FTSR |= dwMaskAnd;
    else {
        EXTI->RTSR |= dwMaskAnd;
        EXTI->FTSR |= dwMaskAnd;
    }

    if(byChn == 0) dwIrq = EXTI0_IRQn;
    else if(byChn == 1) dwIrq = EXTI1_IRQn;
    else if(byChn == 2) dwIrq = EXTI2_IRQn;
    else if(byChn == 3) dwIrq = EXTI3_IRQn;
    else if(byChn == 4) dwIrq = EXTI4_IRQn;
    else if(byChn <= 9) dwIrq = EXTI9_5_IRQn;
    else dwIrq = EXTI15_10_IRQn;
    NvicUp(dwIrq, 10, 0);

    s_pfHandler[byChn] = pfHandler;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭中断功能
参数描述：
*   @ unsigned int dwChnStart  ：外部中断开始端口
*   @ unsigned int dwConNum    ：需要连续操作的端口数
返回值描述：
*   @ 0：成功
说明：
*   @ 关闭IO口的中断功能
*******************************************************************************/
static int IrpDown(unsigned int dwChnStart, unsigned int dwConNum)
{
    unsigned int dwImr = EXTI->IMR;
    unsigned int dwMaskBits = 0;
    unsigned int dwMaskNo;

    for(int i = 0; i < dwConNum; i++) {
        dwMaskBits <<= 1;
        dwMaskBits |= 1;
    }

    dwMaskNo = ~(dwMaskBits << dwChnStart);
    dwImr &= dwMaskNo;
    EXTI->IMR = dwImr;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 打开中断功能
参数描述：
*   @ unsigned int dwChnStart  ：外部中断开始端口
*   @ unsigned int dwConNum    ：需要连续操作的端口数
返回值描述：
*   @ 0：成功
说明：
*   @ 打开IO口的中断功能
*******************************************************************************/
static int IrpUp(unsigned int dwChnStart, unsigned int dwConNum)
{
    unsigned int dwMaskBits = 0;

    for(int i = 0; i < dwConNum; i++) {
        dwMaskBits <<= 1;
        dwMaskBits |= 1;
    }

    EXTI->PR  |= (dwMaskBits << dwChnStart);    // 清除挂起中断标志，防止不必要的中断
    EXTI->IMR |= (dwMaskBits << dwChnStart);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置IO的中断函数
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ void (*pfHandler)(void) ： 触发时的回调函数接口。
返回值描述：
*   @ -1：失败
说明：
*   @ 需要使用AddIrp增加Irp功能。
*******************************************************************************/
static int SetIrp(unsigned int dwDev, unsigned int byChn, void (*pfHandler)(unsigned int dwPare))
{
    if(dwDev >= IO_MAX_DEV) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    s_pfHandler[byChn] = pfHandler;
    return 0;
}
#endif

#if(CONFIG_IO_ADC)
/*******************************************************************************
函数功能：
*   @ 在IO的ADC电压读取功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned char byAdcNo ：使用的ADC模块
*   @ unsigned char byAdcCh　：ADC通道
返回值描述：
*   @ -1：失败
说明：
*   @ 在IO模块直接使用ADC，只能在不同的ADC上面使用1路ADC，这是绝对限制
*******************************************************************************/
static int AddAdc(unsigned int dwDev, unsigned int byChn, unsigned char byAdcNo, unsigned char byAdcChn)
{
    if(dwDev > 5) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    Config(dwDev, byChn, 1, Mode_AIN);
    DevAdc->AddChn(byAdcNo, byAdcChn, 1);
    s_staDev[dwDev].stAdc[byChn].byAdcNo = byAdcNo;
    s_staDev[dwDev].stAdc[byChn].byAdcChn = byAdcChn;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取IO的ADC
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ adc转换值
说明：
*   @ 必须通过AddAdc增加adc功能
*******************************************************************************/
static unsigned short int GetAdc(unsigned int dwDev, unsigned int byChn)
{
    if(dwDev > 5) return 0;
    if(!s_staDev[dwDev].pstGpio) return 0;

    return DevAdc->ReadOne(s_staDev[dwDev].stAdc[byChn].byAdcNo, s_staDev[dwDev].stAdc[byChn].byAdcChn, SampleTime_1Cycles5, 1);
}
#endif

#if(CONFIG_IO_PWM)
/*******************************************************************************
函数功能：
*   @ 向IO口增加PWM功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned char byTimNo ：使用的tim模块
*   @ unsigned char byTimChn ：tim通道
返回值描述：
*   @ -1：失败
说明：
*   @ 向IO口增加PWM功能，使用此方法后，可以在单独的IO输出一定频率的PWM波形
*******************************************************************************/
static int AddPwm(unsigned int dwDev, unsigned int byChn, unsigned char byTimNo, unsigned char byTimChn)
{
    if(dwDev > 5) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    Config(dwDev, byChn, GPIO_Mode_AF_PP);
    DevTim->ConfigPwm(byTimNo, byTimChn);
    s_staDev[dwDev].stTim[byChn].byTimNo = byTimNo;
    s_staDev[dwDev].stTim[byChn].byTimChn = byTimChn;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 在IO的ADC电压读取功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned int byAdcNo ：使用的ADC模块
*   @ unsigned int byAdcCh：ADC通道
返回值描述：
*   @ -1：失败
说明：
*   @ 在IO模块直接使用ADC，只能在不同的ADC上面使用1路ADC，这是绝对限制
*******************************************************************************/
static int OutPwm(unsigned int dwDev, unsigned int byChn, unsigned short int wPwm, unsigned short int wPwmUp)
{
    unsigned char byTimNo;
    unsigned char byTimChn;
    if(dwDev > 5) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    byTimNo = s_staDev[dwDev].stTim[byChn].byTimNo;
    byTimChn = s_staDev[dwDev].stTim[byChn].byTimChn;
    DevTim->SetArrPwm(byTimNo, byTimChn, wPwm, wPwmUp);
    DevTim->StartPwm(byTimNo, byTimChn, 0, 0);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 停止IO口的PWM输出
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ -1：失败
说明：
*   @ 停止IO口的PWM输出
*******************************************************************************/
static int StopPwm(unsigned int dwDev, unsigned int byChn)
{
    unsigned char byTimNo;
    unsigned char byTimChn;
    if(dwDev > 5) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    byTimNo = s_staDev[dwDev].stTim[byChn].byTimNo;
    byTimChn = s_staDev[dwDev].stTim[byChn].byTimChn;
    DevTim->StopPwm(byTimNo, byTimChn);
    return 0;
}
#endif

#if(CONFIG_IO_IRP)
/*******************************************************************************
函数功能：
*   @ 外部中断引脚的中断函数入口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*******************************************************************************/
void EXTI0_IRQHandler (void)
{
    EXTI->BitPR.PR0 = 1;
    if(s_pfHandler[0]) s_pfHandler[0](0);
}

void EXTI1_IRQHandler (void)
{
    EXTI->BitPR.PR1 = 1;
    if(s_pfHandler[1]) s_pfHandler[1](1);
}

void EXTI2_IRQHandler (void)
{
    EXTI->BitPR.PR2 = 1;
    if(s_pfHandler[2]) s_pfHandler[2](2);
}

void EXTI3_IRQHandler (void)
{
    EXTI->BitPR.PR3 = 1;
    if(s_pfHandler[3]) s_pfHandler[3](3);
}

void EXTI4_IRQHandler (void)
{
    EXTI->BitPR.PR4 = 1;
    if(s_pfHandler[4]) s_pfHandler[4](4);
}

void EXTI9_5_IRQHandler (void)
{
    if(EXTI->BitPR.PR5 == 1) {
        EXTI->BitPR.PR5 = 1;
        if(s_pfHandler[5]) s_pfHandler[5] (5);
    }
    if(EXTI->BitPR.PR6 == 1) {
        EXTI->BitPR.PR6 = 1;
        if(s_pfHandler[6]) s_pfHandler[6] (6);
    }
    if(EXTI->BitPR.PR7 == 1) {
        EXTI->BitPR.PR7 = 1;
        if(s_pfHandler[7]) s_pfHandler[7] (7);
    }
    if(EXTI->BitPR.PR8 == 1) {
        EXTI->BitPR.PR8 = 1;
        if(s_pfHandler[8]) s_pfHandler[8] (8);
    }
    if(EXTI->BitPR.PR9 == 1) {
        EXTI->BitPR.PR9 = 1;
        if(s_pfHandler[9]) s_pfHandler[9] (9);
    }
}

void EXTI15_10_IRQHandler (void)
{
	if(EXTI->BitPR.PR10 == 1) {
        EXTI->BitPR.PR10 = 1;
        if(s_pfHandler[10]) s_pfHandler[10] (10);
    }
    if(EXTI->BitPR.PR11 == 1) {
        EXTI->BitPR.PR11 = 1;
        if(s_pfHandler[11]) s_pfHandler[11] (11);
    }
    if(EXTI->BitPR.PR12 == 1) {
        EXTI->BitPR.PR12 = 1;
        if(s_pfHandler[12]) s_pfHandler[12] (12);
    }
    if(EXTI->BitPR.PR13 == 1) {
        EXTI->BitPR.PR13 = 1;
        if(s_pfHandler[13]) s_pfHandler[13] (13);
    }
    if(EXTI->BitPR.PR14 == 1) {
        EXTI->BitPR.PR14 = 1;
        if(s_pfHandler[14]) s_pfHandler[14] (14);
    }
    if(EXTI->BitPR.PR15 == 1) {
        EXTI->BitPR.PR15 = 1;
        if(s_pfHandler[15]) s_pfHandler[15] (15);
    }
}
#endif

static const DevIoTdf s_stIo = {
    CommLock,
    CommUnLock,
    RemapConfig,
    NvicUp,
    NvicDown,
    Read,
    Write,
    WriteGPIO,
    ReadGPIO,
    ReadGpioTrue,
    Config,
#if(CONFIG_IO_IRP)
    AddIrp,
    IrpDown,
    IrpUp,
    SetIrp,
#endif
#if(CONFIG_IO_ADC)
    AddAdc,
    GetAdc,
#endif
#if(CONFIG_IO_PWM)
    AddPwm,
    OutPwm,
    StopPwm,
#endif
};

/*******************************************************************************
函数功能：
*   @ 初始化IO模块
参数描述：
*   @ 无
返回值描述：
*   @ 模块操作方法
说明：
*******************************************************************************/
const DevIoTdf *IoInit(void)
{
    RCC->BitAPB2ENR.IOPAEN = 1;        //开启IO口时钟
    RCC->BitAPB2ENR.IOPBEN = 1;        //开启IO口时钟
    RCC->BitAPB2ENR.IOPCEN = 1;        //开启IO口时钟
    RCC->BitAPB2ENR.IOPDEN = 1;        //开启IO口时钟
    RCC->BitAPB2ENR.IOPEEN = 1;        //开启IO口时钟
    RCC->BitAPB2ENR.AFIOEN = 1;
#if(CONFIG_IO_IRP)
    for(int i = 0 ; i < 16; i ++) {
        s_pfHandler[i] = 0;
    }
#endif
    s_staDev[0].pstGpio = GPIOA;
    s_staDev[1].pstGpio = GPIOB;
    s_staDev[2].pstGpio = GPIOC;
    s_staDev[3].pstGpio = GPIOD;
    s_staDev[4].pstGpio = GPIOE;

    return &s_stIo;
}
#endif