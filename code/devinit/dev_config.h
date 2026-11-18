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
#ifndef __DEV_CONFIG_H
#define __DEV_CONFIG_H

#define PLAT_STM32_F1                               1
//#define PLAT_STM32_F2                               1

//core 设备定义
#define CONFIG_DMA1                                 0
#define CONFIG_DMA2                                 0

//#define CONFIG_RTC                                  1

#define CONFIG_USART1                               1
#define CONFIG_USART2                               1
//#define CONFIG_USART3                               1
//#define CONFIG_USART4                               1
//#define CONFIG_USART5                               1

//#define CONFIG_SPI1                                 1
//#define CONFIG_SPI2                                 1
//#define CONFIG_SPI3                                 1
//#define CONFIG_SPI_DMA                              1
//#define CONFIG_SPI_DMA_RETURN                       1

#define CONFIG_ADC1                                 1
#define CONFIG_ADC2                                 0
#define CONFIG_ADC_DMA                              0
//#define CONFIG_ADC_SET_PRE                          1

//#define CONFIG_DAC                                  1
#define CONFIG_IO_ADC                               1
#define CONFIG_CAN1                                 1

#define CONFIG_TIM1                                 1
#define CONFIG_TIM2                                 1
#define CONFIG_TIM3                                 1
#define CONFIG_TIM4                                 1
#define CONFIG_TIM5                                 1
#define CONFIG_TIM6                                 0
#define CONFIG_TIM7                                 0
#define CONFIG_TIM8                                 1
#define CONFIG_TIM13                                0
#define CONFIG_TIM14                                0
#define CONFIG_IO_IRP                               1

//plat 定义
#define CONFIG_COM                                  1
#define CONFIG_AD9822                               0
#define CONFIG_DS18B20                              0
#define CONFIG_XCAN                                 1
#define CONFIG_PWM                                  1
#define CONFIG_MPU6050                              1
//#define CONFIG_AT45                               1
//ulnar2003 电机个数
#define CONFIG_A3983                                1
#define MAX_MO                                      6
#define MAX_MPU                                     3
#define MAX_PWM                                     3

#define CONFIG_CAN_BOUD_NEW_RATE    250000
#define CONFIG_CAN_TS1_TQ           2
#define CONFIG_CAN_TS2_TQ           5

//#define USE_SIN_UP_SPEED            1
/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
外部参数
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/

#endif