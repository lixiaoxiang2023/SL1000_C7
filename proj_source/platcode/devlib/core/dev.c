/*******************************************************************************
文件名称：   Dev.c
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

/*******************************************************************************
Constant Definition
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/
const DevIoTdf *DevIo;
#if(CONFIG_SPI1|CONFIG_SPI2|CONFIG_SPI3|CONFIG_SPI4|CONFIG_SPI5|CONFIG_SPI6)
const DevSpiTdf *DevSpi;
#endif
#if(CONFIG_DMA1|CONFIG_DMA2)
const DevDmaTdf *DevDma;
#endif
#if(CONFIG_TIM1|CONFIG_TIM2|CONFIG_TIM3|CONFIG_TIM4|CONFIG_TIM5|CONFIG_TIM8)
const DevTimTdf *DevTim;
#endif
#if(CONFIG_USART1|CONFIG_USART2|CONFIG_USART3|CONFIG_USART4|CONFIG_USART5|CONFIG_USART6)
const DevUartTdf *DevUart;
#endif
#if(CONFIG_I2C1|CONFIG_I2C2|CONFIG_I2C3)
const DevI2cTdf *DevI2c;
#endif
#if(CONFIG_RTC)
const DevRtcTdf *DevRtc;
#endif
#if(CONFIG_ADC1|CONFIG_ADC2|CONFIG_ADC3)
const DevAdcTdf *DevAdc;
#endif
#if(CONFIG_DAC)
const DevDacTdf *DevDac;
#endif
#if(CONFIG_PW)
const DevPwTdf *DevPw;
#endif

#if(CONFIG_CAN1|CONFIG_CAN2)
const DevCanTdf *DevCan;
#endif

#if(CONFIG_ETH)
const DevEthTdf *DevEth;
#endif

/*******************************************************************************
外部变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 根据配置初始化底层mcu硬件
参数描述：
*   @ 无
返回值描述：
*   @ IO口的电平
说明：
*******************************************************************************/
void DevCoreInit (void)
{
    SCB->AIRCR = ((unsigned int )0x05FA0000) | ((unsigned int )0x300); // NVIC_PriorityGroup_4

    DevIo = IoInit();
#if(CONFIG_DMA1|CONFIG_DMA2)
    DevDma = DmaInit();
#endif
#if(CONFIG_SPI1|CONFIG_SPI2|CONFIG_SPI3|CONFIG_SPI4)
    DevSpi = SpiInit();
#endif
#if(CONFIG_TIM1|CONFIG_TIM2|CONFIG_TIM3|CONFIG_TIM4|CONFIG_TIM5|CONFIG_TIM6|CONFIG_TIM7|CONFIG_TIM8|CONFIG_TIM9|CONFIG_TIM10|CONFIG_TIM11|CONFIG_TIM12|CONFIG_TIM13|CONFIG_TIM14)
    DevTim = TimInit();
#endif
#if(CONFIG_USART1|CONFIG_USART2|CONFIG_USART3|CONFIG_USART4|CONFIG_USART5|CONFIG_USART6)
    DevUart = UsartInit();
#endif
#if(CONFIG_I2C1|CONFIG_I2C2|CONFIG_I2C3)
    DevI2c = I2cInit();
#endif
#if(CONFIG_RTC)
    DevRtc = RtcInit();
#endif
#if(CONFIG_ADC1|CONFIG_ADC2|CONFIG_ADC3)
    DevAdc = AdcInit();
#endif
#if(CONFIG_DAC)
    DevDac = DacInit();
#endif
#if(CONFIG_PW)
    DevPw = PwInit();
#endif
#if(CONFIG_CAN1|CONFIG_CAN2)
    #ifdef CONFIG_CAN_BOUD_NEW_RATE
        DevCan = CanInit2(CONFIG_CAN_BOUD_NEW_RATE);
    #else
        DevCan = CanInit();
    #endif
#endif
#if(CONFIG_ETH)
    DevEth = EthInit();
#endif
}

/*******************************************************************************
函数功能：
*   @ 根据RCC的时钟配置，取得各个时钟的具体值
参数描述：
*   @ 无
返回值描述：
*   @ 时钟值
说明：
*******************************************************************************/
void DevRccGetClocksFreq(ClocksTypeDef* RCC_Clocks)
{
    unsigned int  tmp = 0, presc = 0, pllsource = 0, pllm = 2;

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    unsigned int  pllvco = 0, pllp = 2;
    unsigned int  pllr = 2;
#endif

    tmp = RCC->BitCFGR.SWS;

    switch (tmp) {
    case 0:  /* HSI used as system clock source */
        RCC_Clocks->SYSCLK = HSI_VALUE;
        break;
    case 1:  /* HSE used as system clock  source */
        RCC_Clocks->SYSCLK = HSE_VALUE;
        break;
    case 2:  /* PLL P used as system clock  source */

#if(PLAT_STM32_F2|PLAT_STM32_F4)
        /*
        *   PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
        *   SYSCLK = PLL_VCO / PLLP
        */
        pllsource = RCC->BitPLLCFGR.PLLSRC; // 0:HSI 1:HSE
        pllm = RCC->BitPLLCFGR.PLLM;

        if (pllsource != 0){
            /* HSE used as PLL clock source */
            pllvco = (HSE_VALUE / pllm) * (RCC->BitPLLCFGR.PLLN);
        }
        else {
            /* HSI used as PLL clock source */
            pllvco = (HSI_VALUE / pllm) * (RCC->BitPLLCFGR.PLLN);
        }

        pllp = (RCC->BitPLLCFGR.PLLP + 1 ) * 2;
        RCC_Clocks->SYSCLK = pllvco/pllp;
        break;
#endif

#if(PLAT_STM32_F1)
        pllm = RCC->BitCFGR.PLLMULL + 2;
        pllsource = RCC->BitCFGR.PLLSRC;

        if (pllsource == 0x00) {
            /* HSI oscillator clock divided by 2 selected as PLL clock entry */
            RCC_Clocks->SYSCLK = (HSI_VALUE >> 1) * pllm;
        }
        else {
            /* HSE selected as PLL clock entry */
            if (RCC->BitCFGR.PLLXTPRE != 0) RCC_Clocks->SYSCLK = (HSE_VALUE >> 1) * pllm;
            else RCC_Clocks->SYSCLK = HSE_VALUE * pllm;
        }

        break;
#endif

    case 3:
#if(PLAT_STM32_F2|PLAT_STM32_F4)
        pllsource = RCC->BitPLLCFGR.PLLSRC; // 0:HSI 1:HSE
        pllm = RCC->BitPLLCFGR.PLLM;

        if (pllsource != 0) {
            /* HSE used as PLL clock source */
            pllvco = (HSE_VALUE / pllm) * (RCC->BitPLLCFGR.PLLN);
        }
        else {
            /* HSI used as PLL clock source */
            pllvco = (HSI_VALUE / pllm) * (RCC->BitPLLCFGR.PLLN);
        }

        pllr = (RCC->BitPLLCFGR.PLLR + 1 ) *2;
        RCC_Clocks->SYSCLK = pllvco/pllr;
        break;
#endif

    default:
        RCC_Clocks->SYSCLK = HSI_VALUE;
        break;
    }

    /* Compute HCLK, PCLK1 and PCLK2 clocks frequencies ------------------------*/
    static const uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

    tmp = RCC->BitCFGR.HPRE;
    presc = APBAHBPrescTable[tmp];
    RCC_Clocks->HCLK = RCC_Clocks->SYSCLK >> presc;

    tmp = RCC->BitCFGR.PPRE1;
    presc = APBAHBPrescTable[tmp];
    RCC_Clocks->PCLK1 = RCC_Clocks->HCLK >> presc;

    tmp = RCC->BitCFGR.PPRE2;
    presc = APBAHBPrescTable[tmp];
    RCC_Clocks->PCLK2 = RCC_Clocks->HCLK >> presc;

#if(PLAT_STM32_F1)
    static const uint8_t ADCPrescTable[4] = {2, 4, 6, 8};

    tmp = RCC->BitCFGR.ADCPRE;
    presc = ADCPrescTable[tmp];
#endif

    RCC_Clocks->ADCCLK = RCC_Clocks->PCLK2 / presc;
}
