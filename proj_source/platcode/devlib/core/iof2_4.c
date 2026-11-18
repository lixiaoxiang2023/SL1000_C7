/*******************************************************************************
文件名称：   iof2.c
目录：       $PROJ_DIR$/usr/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.01.21
程序用途：   stmf2x IO口的通用配置平台、应用平台

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

#if(PLAT_STM32_F2|PLAT_STM32_F4)
/*******************************************************************************
Constant Definition
*******************************************************************************/
#define LSB_MASK                    ((unsigned short int )0xFFFF)
#define DBGAFR_POSITION_MASK        ((unsigned int)0x000F0000)
#define DBGAFR_SWJCFG_MASK          ((unsigned int)0xF0FFFFFF)
#define DBGAFR_LOCATION_MASK        ((unsigned int)0x00200000)
#define DBGAFR_NUMBITS_MASK         ((unsigned int)0x00100000)

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
}s_staDev[CONFIG_GPIOX_NUM];

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
    if(!pwLock) return;

    __ASM("cpsid i");// 关中断
    __ASM("cpsid f");// 关异常

    *pwLock = 0;

    __ASM("cpsie i");//开
    __ASM("cpsie f");
}

/*******************************************************************************
*函数功能：
*   @ 重映射管教功能
*参数描述：
*        *   @arg GPIO_AF_RTC_50Hz: Connect RTC_50Hz pin to AF0 (default after reset)
*        *   @arg GPIO_AF_MCO: Connect MCO pin (MCO1 and MCO2) to AF0 (default after reset)
*        *   @arg GPIO_AF_TAMPER: Connect TAMPER pins (TAMPER_1 and TAMPER_2) to AF0 (default after reset)
*        *   @arg GPIO_AF_SWJ: Connect SWJ pins (SWD and JTAG)to AF0 (default after reset)
*        *   @arg GPIO_AF_TRACE: Connect TRACE pins to AF0 (default after reset)
*        *   @arg GPIO_AF_TIM1: Connect TIM1 pins to AF1
*        *   @arg GPIO_AF_TIM2: Connect TIM2 pins to AF1
*        *   @arg GPIO_AF_TIM3: Connect TIM3 pins to AF2
*        *   @arg GPIO_AF_TIM4: Connect TIM4 pins to AF2
*        *   @arg GPIO_AF_TIM5: Connect TIM5 pins to AF2
*        *   @arg GPIO_AF_TIM8: Connect TIM8 pins to AF3
*        *   @arg GPIO_AF_TIM9: Connect TIM9 pins to AF3
*        *   @arg GPIO_AF_TIM10: Connect TIM10 pins to AF3
*        *   @arg GPIO_AF_TIM11: Connect TIM11 pins to AF3
*        *   @arg GPIO_AF_I2C1: Connect I2C1 pins to AF4
*        *   @arg GPIO_AF_I2C2: Connect I2C2 pins to AF4
*        *   @arg GPIO_AF_I2C3: Connect I2C3 pins to AF4
*        *   @arg GPIO_AF_SPI1: Connect SPI1 pins to AF5
*        *   @arg GPIO_AF_SPI2: Connect SPI2/I2S2 pins to AF5
*        *   @arg GPIO_AF_SPI3: Connect SPI3/I2S3 pins to AF6
*        *   @arg GPIO_AF_USART1: Connect USART1 pins to AF7
*        *   @arg GPIO_AF_USART2: Connect USART2 pins to AF7
*        *   @arg GPIO_AF_USART3: Connect USART3 pins to AF7
*        *   @arg GPIO_AF_UART4: Connect UART4 pins to AF8
*        *   @arg GPIO_AF_UART5: Connect UART5 pins to AF8
*        *   @arg GPIO_AF_USART6: Connect USART6 pins to AF8
*        *   @arg GPIO_AF_CAN1: Connect CAN1 pins to AF9
*        *   @arg GPIO_AF_CAN2: Connect CAN2 pins to AF9
*        *   @arg GPIO_AF_TIM12: Connect TIM12 pins to AF9
*        *   @arg GPIO_AF_TIM13: Connect TIM13 pins to AF9
*        *   @arg GPIO_AF_TIM14: Connect TIM14 pins to AF9
*        *   @arg GPIO_AF_OTG_FS: Connect OTG_FS pins to AF10
*        *   @arg GPIO_AF_OTG_HS: Connect OTG_HS pins to AF10
*        *   @arg GPIO_AF_ETH: Connect ETHERNET pins to AF11
*        *   @arg GPIO_AF_FSMC: Connect FSMC pins to AF12
*        *   @arg GPIO_AF_OTG_HS_FS: Connect OTG HS (configured in FS) pins to AF12
*        *   @arg GPIO_AF_SDIO: Connect SDIO pins to AF12
*        *   @arg GPIO_AF_DCMI: Connect DCMI pins to AF13
*        *   @arg GPIO_AF_EVENTOUT: Connect EVENTOUT pins to AF15
*返回值描述：
*   @ 无
*说明：
*******************************************************************************/
void PinAFConfig(GPIO_TypeDef* GPIOx, unsigned short int  GPIO_PinSource, uint8_t GPIO_AF)
{
    unsigned int temp = 0x00;
    unsigned int temp_2 = 0x00;

    temp = ((unsigned int)(GPIO_AF) << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4)) ;
    GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((unsigned int)0xF << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4)) ;
    temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
    GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
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
    NVIC->ICER[dwTrq >> 0x05] = (unsigned int)0x01 << (dwTrq & (unsigned char)0x1F);

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

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
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

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
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

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
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

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
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

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
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
static int Config(unsigned int dwDev, unsigned int dwChn, unsigned int dwChnNum, IOMode_TypeDef Mode, IOOType_TypeDef OType, \
    IOSpeed_TypeDef Speed, IOPuPd_TypeDef PuPd)
{
    unsigned int dwReg = 0;
    GPIO_TypeDef *pstGpio = 0;

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;
    pstGpio = s_staDev[dwDev].pstGpio;

    for(int pos = 0; pos < dwChnNum ; pos ++, dwChn ++) {
        dwReg = pstGpio->MODER;
        dwReg &= (~((unsigned int)0x03 << (dwChn * 2)));
        dwReg |= (((unsigned int)(Mode & 0x03)) << (dwChn * 2));
        pstGpio->MODER = dwReg;

        if(Mode == Mode_OUT || Mode == Mode_AF) {
            dwReg = pstGpio->OSPEEDR;
            dwReg &= (~((unsigned int)0x01 << (dwChn * 2)));
            dwReg |= (((unsigned int)(Speed & 0x03)) << (dwChn * 2));
            pstGpio->OSPEEDR = dwReg;

            dwReg = pstGpio->OTYPER;
            dwReg &= (~((unsigned int)0x01 << dwChn));
            dwReg |= (((unsigned int)(OType & 0x01)) << dwChn);
            pstGpio->OTYPER = dwReg;
        }

        dwReg = pstGpio->PUPDR;
        dwReg &= (~((unsigned int)0x03 << (dwChn * 2)));
        dwReg |= (((unsigned int)(PuPd & 0x03)) << (dwChn * 2));
        pstGpio->PUPDR = dwReg;
    }

    return 0;
}

#if(CONFIG_IO_IRP)
/*******************************************************************************
函数功能：
*   向IO口增加中断功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ IoEXTITriggerTdf Trigger ： IO中断触发方式
*   @ void (*pfHandler)(void) ： 触发时的回调函数接口。
返回值描述：
*   @ -1：失败
说明：
*   向IO口增加回调函数接口
*******************************************************************************/
static int IrpPri(unsigned int dwChn, unsigned int dwPri)
{
    unsigned int dwIrq = 0;

    if(dwChn == 0) dwIrq = EXTI0_IRQn;
    else if(dwChn == 1) dwIrq = EXTI1_IRQn;
    else if(dwChn == 2) dwIrq = EXTI2_IRQn;
    else if(dwChn == 3) dwIrq = EXTI3_IRQn;
    else if(dwChn == 4) dwIrq = EXTI4_IRQn;
    else if(dwChn <= 9) dwIrq = EXTI9_5_IRQn;
    else if(dwIrq < 16) dwIrq = EXTI15_10_IRQn;
    else return -1;

    NvicUp(dwIrq, dwPri, 0);

    return 0;
}

/*******************************************************************************
函数功能：
*   向IO口增加中断功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ IoEXTITriggerTdf Trigger ： IO中断触发方式
*   @ void (*pfHandler)(void) ： 触发时的回调函数接口。
返回值描述：
*   @ -1：失败
说明：
*   向IO口增加回调函数接口
*******************************************************************************/
static int AddIrp(unsigned int dwDev, unsigned int byChn, IoEXTITriggerTdf Trigger, void (*pfHandler)(unsigned int dwPare))
{
    unsigned int tmp = 0x00;
    unsigned int dwIrq = 0;
    unsigned int dwMaskAnd = (((unsigned int) 0x01) << byChn);
    unsigned int dwMaskNo = ~(((unsigned int) 0x01) << byChn);

    if(dwDev > CONFIG_GPIOX_NUM) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    if(byChn < 16) {
        // Config(dwDev, byChn, 1, Mode_IN, OType_PP, Speed_2MHz, PuPd_NOPULL);
        tmp = ((unsigned int)0x0F) << (0x04 * (byChn & (unsigned char)0x03));
        SYSCFG->EXTICR[byChn >> 0x02] &= ~tmp;
        tmp = (((unsigned int)dwDev) << (0x04 * (byChn % 4)));
        SYSCFG->EXTICR[byChn >> 0x02] |= tmp;
    }

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
    else if(dwIrq < 16) dwIrq = EXTI15_10_IRQn;
    else return 0;

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
    设置IO的中断函数
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ void (*pfHandler)(void) ： 触发时的回调函数接口。
返回值描述：
*   @ -1：失败
说明：
    需要使用AddIrp增加Irp功能。
*******************************************************************************/
static int SetIrp(unsigned int dwDev, unsigned int byChn, void (*pfHandler)(unsigned int dwPare))
{
    if(dwDev > CONFIG_GPIOX_NUM) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    s_pfHandler[byChn] = pfHandler;
    return 0;
}
#endif

#if(CONFIG_IO_ADC)
/*******************************************************************************
函数功能：
*   在IO的ADC电压读取功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned char byAdcNo ：使用的ADC模块
*   @ unsigned char byAdcCh　：ADC通道
返回值描述：
*   @ -1：失败
说明：
*   在IO模块直接使用ADC，只能在不同的ADC上面使用1路ADC，这是绝对限制
*******************************************************************************/
static int AddAdc(unsigned int dwDev, unsigned int byChn, unsigned char byAdcNo, unsigned char byAdcChn)
{
    if(dwDev > CONFIG_GPIOX_NUM) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    Config(dwDev, byChn, 1, Mode_AN, OType_PP, Speed_2MHz, PuPd_NOPULL);
    DevAdc->AddChn(byAdcNo, byAdcChn, 1);
    s_staDev[dwDev].stAdc[byChn].byAdcNo = byAdcNo;
    s_staDev[dwDev].stAdc[byChn].byAdcChn = byAdcChn;

    return 0;
}

/*******************************************************************************
函数功能：
    读取IO的ADC
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ adc转换值
说明：
    必须通过AddAdc增加adc功能
*******************************************************************************/
static unsigned short int GetAdc(unsigned int dwDev, unsigned int byChn)
{
    if(dwDev > CONFIG_GPIOX_NUM) return 0;
    if(!s_staDev[dwDev].pstGpio) return 0;

    return DevAdc->ReadOne(s_staDev[dwDev].stAdc[byChn].byAdcNo, s_staDev[dwDev].stAdc[byChn].byAdcChn, SampleTime_1Cycles5, 2);
}
#endif

#if(CONFIG_IO_PWM)
/*******************************************************************************
函数功能：
*   向IO口增加PWM功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned char byTimNo ：使用的tim模块
*   @ unsigned char byTimChn ：tim通道
返回值描述：
*   @ -1：失败
说明：
*   向IO口增加PWM功能，使用此方法后，可以在单独的IO输出一定频率的PWM波形
*******************************************************************************/
static int AddPwm(unsigned int dwDev, unsigned int byChn, unsigned char byTimNo, unsigned char byTimChn)
{
    if(dwDev > CONFIG_GPIOX_NUM) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    Config(dwDev, byChn, 1, Mode_AF, OType_PP, Speed_50MHz, PuPd_NOPULL);
    DevTim->ConfigPwm(byTimNo, byTimChn);
    s_staDev[dwDev].stTim[byChn].byTimNo = byTimNo;
    s_staDev[dwDev].stTim[byChn].byTimChn = byTimChn;

    return 0;
}

/*******************************************************************************
函数功能：
*   在IO的ADC电压读取功能
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
*   @ unsigned int byAdcNo ：使用的ADC模块
*   @ unsigned int byAdcCh：ADC通道
返回值描述：
*   @ -1：失败
说明：
*   在IO模块直接使用ADC，只能在不同的ADC上面使用1路ADC，这是绝对限制
*******************************************************************************/
static int OutPwm(unsigned int dwDev, unsigned int byChn, unsigned short int wPwm, unsigned short int wPwmUp)
{
    unsigned char byTimNo;
    unsigned char byTimChn;
    if(dwDev > CONFIG_GPIOX_NUM) return -1;
    if(!s_staDev[dwDev].pstGpio) return -1;

    byTimNo = s_staDev[dwDev].stTim[byChn].byTimNo;
    byTimChn = s_staDev[dwDev].stTim[byChn].byTimChn;
    DevTim->SetArrPwm(byTimNo, byTimChn, wPwm, wPwmUp);
    DevTim->StartPwm(byTimNo, byTimChn, 0, 0);
    return 0;
}

/*******************************************************************************
函数功能：
*   停止IO口的PWM输出
参数描述：
*   @ unsigned int byPort：GPIO口 0：GPIOA，1：GPIOB...
*   @ unsigned int byChn：GPIO通道 0：ch0; 1:ch1...
返回值描述：
*   @ -1：失败
说明：
*   停止IO口的PWM输出
*******************************************************************************/
static int StopPwm(unsigned int dwDev, unsigned int byChn)
{
    unsigned char byTimNo;
    unsigned char byTimChn;
    if(dwDev > CONFIG_GPIOX_NUM) return -1;
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
*   @ 无
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
    PinAFConfig,
    NvicUp,
    NvicDown,
    Read,
    Write,
    WriteGPIO,
    ReadGPIO,
    ReadGpioTrue,
    Config,
#if(CONFIG_IO_IRP)
    IrpPri,
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
    for(int i = 0; i < CONFIG_GPIOX_NUM ; i ++ ) {
        // 使用cortex-M3/M4的bit-band功能对GPIO进行时钟开启
        // CONFIG_GPIOX_NUM 为定义的从 GPIOA - GPIOx的顺序，在RCC->AHB1ENR, APIOA 到GPIOK是依次排列
        // 使用这个技巧进行所需要的最多GPIO时钟开启
        // 若需要使用到 ABCDE CONFIG_GPIOX_NUM = 5，依次开启GPIOA,B,C,D,E,需要根据不同的F2/F4芯片配置
        // 不同的使用数目
        // 此段代码与芯片的特异性相关, 此部分代码既有一定的技巧，适用用stm32f2/f4系列ＭＣＵ.
        *( unsigned int *)(PERIPH_BB_BASE + (((( unsigned int)&(RCC->AHB1ENR)) & 0xfffff) << 5) + (i << 2)) = 1;

        // 在F2/F4的芯片上面GPIO存器的地址在GPIO的基础上进行0x0400的递增，一次到GPIOK
        s_staDev[i].pstGpio = ((GPIO_TypeDef *) (GPIOA_BASE + (0x0400 * i)));
    }

    RCC->BitAPB2ENR.SYSCFGEN = 1;

#if(CONFIG_IO_IRP)
    for(int i = 0 ; i < 16; i ++) {
        s_pfHandler[i] = 0;
    }
#endif

    return &s_stIo;
}
#endif