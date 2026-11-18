/*******************************************************************************
文件名称： dev.h
文件目录： $PROJ_DIR$/usr/Drive/inc
项目名称： 事业一部通用技术平台
创建者：   胥强
创建日期：
程序用途： 用于包含所有的设备,输出设备方法端口，配置设备使用

版权声明： Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __DEV_H
#define __DEV_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dev_config.h"
#include "configinit.h"

#include "devarch.h"

#if(CONFIG_FREERTOS)
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define SysDelay(a)   vTaskDelay(a)
#endif

#include "stm32Bit.h"
#if(PLAT_STM32_F1|PLAT_GD32_F30X)
    #include "io.h"
#endif

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    #include "iof2_4.h"
#endif


#if(CONFIG_DMA1|CONFIG_DMA2)
#if(PLAT_STM32_F1|PLAT_GD32_F30X)
    #include "dma.h"
#endif
#if(PLAT_STM32_F2|PLAT_STM32_F4)
    #include "dmaf2_4.h"
#endif
#endif

#if(CONFIG_ADC1|CONFIG_ADC2|CONFIG_ADC3)
#if(PLAT_STM32_F1|PLAT_GD32_F30X)
    #include "adc.h"
#endif
#if(PLAT_STM32_F2|PLAT_STM32_F4)
    #include "adcf2_4.h"
#endif
#endif

#if(CONFIG_USART1|CONFIG_USART2|CONFIG_USART3|CONFIG_USART4|CONFIG_USART5|CONFIG_USART6)
    #include "usart.h"
#endif

#if(CONFIG_SPI1|CONFIG_SPI2|CONFIG_SPI3|CONFIG_SPI4|CONFIG_SPI5|CONFIG_SPI6)
    #include "spi.h"
#endif
#if(CONFIG_TIM1|CONFIG_TIM2|CONFIG_TIM3|CONFIG_TIM4|CONFIG_TIM5|CONFIG_TIM6|CONFIG_TIM8)
    #include "tim.h"
#endif

#if(CONFIG_I2C1|CONFIG_I2C2|CONFIG_I2C3)
    #include "i2c.h"
#endif

#if(CONFIG_RTC)
    #if(PLAT_STM32_F1|PLAT_GD32_F30X)
        #include "rtc.h"
    #endif
    #if(PLAT_STM32_F2|PLAT_STM32_F4)
        #include "rtcf2.h"
    #endif
#endif

#if(CONFIG_DAC)
    #include "dac.h"
#endif

#if(CONFIG_PW)
    #include "pw.h"
#endif

#if(CONFIG_CAN1|CONFIG_CAN2)
    #include "bxcan.h"
#endif

#if(CONFIG_ETH)
    #include "eth.h"
#endif

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    unsigned int SYSCLK; /*!<  SYSCLK clock frequency expressed in Hz */
    unsigned int HCLK;   /*!<  HCLK clock frequency expressed in Hz   */
    unsigned int PCLK1;  /*!<  PCLK1 clock frequency expressed in Hz  */
    unsigned int PCLK2;  /*!<  PCLK2 clock frequency expressed in Hz  */
    unsigned int ADCCLK;
}ClocksTypeDef;

/*******************************************************************************
外部参数
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
void DevCoreInit (void);
void DevRccGetClocksFreq(ClocksTypeDef* RCC_Clocks);



#endif
