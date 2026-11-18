/*******************************************************************************
文件名称： configinit.h
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
#ifndef __CONFIGINIT_H
#define __CONFIGINIT_H

#ifndef CONFIG_FREERTOS
#define CONFIG_FREERTOS                                 0  //使用FreeRTOS编译项
#endif

//core 设备定义
#ifndef CONFIG_GPIOX_NUM
    #define CONFIG_GPIOX_NUM                            5
#endif

#ifndef CONFIG_DMA1
    #define CONFIG_DMA1                                 0
#endif
#ifndef CONFIG_DMA2
    #define CONFIG_DMA2                                 0
#endif

#ifndef CONFIG_USART1
    #define CONFIG_USART1                               0
#endif
#ifndef CONFIG_USART2
    #define CONFIG_USART2                               0
#endif
#ifndef CONFIG_USART3
    #define CONFIG_USART3                               0
#endif
#ifndef CONFIG_USART4
    #define CONFIG_USART4                               0
#endif
#ifndef CONFIG_USART5
    #define CONFIG_USART5                               0
#endif
#ifndef CONFIG_USART6
    #define CONFIG_USART6                               0
#endif
#ifndef CONFIG_USART7
    #define CONFIG_USART7                               0
#endif
#ifndef CONFIG_USART8
    #define CONFIG_USART8                               0
#endif
#ifndef CONFIG_USART_DMA
    #define CONFIG_USART_DMA                            0
#endif
#ifndef CONFIG_USART_IT
    #define CONFIG_USART_IT                             0
#endif

#ifndef CONFIG_SPI1
    #define CONFIG_SPI1                                 0
#endif
#ifndef CONFIG_SPI2
    #define CONFIG_SPI2                                 0
#endif
#ifndef CONFIG_SPI3
    #define CONFIG_SPI3                                 0
#endif
#ifndef CONFIG_SPI4
    #define CONFIG_SPI4                                 0
#endif
#ifndef CONFIG_SPI_DMA
    #define CONFIG_SPI_DMA                              0
#endif
#ifndef CONFIG_SPI_DMA_RETURN
    #define CONFIG_SPI_DMA                              0
#endif
#ifndef CONFIG_ADC1
    #define CONFIG_ADC1                                 0
#endif
#ifndef CONFIG_ADC2
    #define CONFIG_ADC2                                 0
#endif
#ifndef CONFIG_ADC3
    #define CONFIG_ADC3                                 0
#endif
#ifndef CONFIG_ADC_DMA
    #define CONFIG_ADC_DMA                              0
#endif
#ifndef CONFIG_ADC_EXT_TRIG
    #define CONFIG_ADC_EXT_TRIG                         0
#endif
#ifndef CONFIG_ADC_SET_PRE
    #define CONFIG_ADC_SET_PRE                          0
#endif

#ifndef CONFIG_TIM1
    #define CONFIG_TIM1                                 0
#endif
#ifndef CONFIG_TIM2
    #define CONFIG_TIM2                                 0
#endif
#ifndef CONFIG_TIM3
    #define CONFIG_TIM3                                 0
#endif
#ifndef CONFIG_TIM4
    #define CONFIG_TIM4                                 0
#endif
#ifndef CONFIG_TIM5
    #define CONFIG_TIM5                                 0
#endif
#ifndef CONFIG_TIM6
    #define CONFIG_TIM6                                 0
#endif
#ifndef CONFIG_TIM7
    #define CONFIG_TIM7                                 0
#endif
#ifndef CONFIG_TIM8
    #define CONFIG_TIM8                                 0
#endif
#ifndef CONFIG_TIM9
    #define CONFIG_TIM9                                 0
#endif
#ifndef CONFIG_TIM10
    #define CONFIG_TIM10                                0
#endif
#ifndef CONFIG_TIM11
    #define CONFIG_TIM11                                0
#endif
#ifndef CONFIG_TIM12
    #define CONFIG_TIM12                                0
#endif
#ifndef CONFIG_TIM13
    #define CONFIG_TIM13                                0
#endif
#ifndef CONFIG_TIM14
    #define CONFIG_TIM14                                0
#endif

#ifndef CONFIG_TIM1_CCMR
    #define CONFIG_TIM1_CCMR                            0
#endif
#ifndef CONFIG_TIM8_CCMR
    #define CONFIG_TIM8_CCMR                            0
#endif

#ifndef CONFIG_TIM_TRGO
    #define CONFIG_TIM_TRGO                            (0)
#endif

#ifndef CONFIG_IO_IRP
    #define CONFIG_IO_IRP                               0
#endif
#ifndef CONFIG_IO_ADC
    #define CONFIG_IO_ADC                               0
#endif
#ifndef CONFIG_IO_PWM
    #define CONFIG_IO_PWM                               0
#endif

#ifndef CONFIG_I2C1
    #define CONFIG_I2C1                                 0
#endif
#ifndef CONFIG_I2C2
    #define CONFIG_I2C2                                 0
#endif
#ifndef CONFIG_I2C3
    #define CONFIG_I2C3                                 0
#endif

#ifndef CONFIG_I2C_IO
    #define CONFIG_I2C_IO                               0
#endif
#ifndef CONFIG_I2C_DMA
    #define CONFIG_I2C_DMA                              0
#endif
#ifndef CONFIG_I2C_RX_DMA
    #define CONFIG_I2C_RX_DMA                           0
#endif
#ifndef CONFIG_I2C_IRP
    #define CONFIG_I2C_IRP                              0
#endif
#ifndef CONFIG_I2C_READ
    #define CONFIG_I2C_READ                             0
#endif

#ifndef CONFIG_RTC
    #define CONFIG_RTC                                  0
#endif


#ifndef CONFIG_DAC
    #define CONFIG_DAC                                  0
#endif
#ifndef CONFIG_DAC_DMA
    #define CONFIG_DAC_DMA                              0
#endif

#ifndef CONFIG_CAN1
    #define CONFIG_CAN1                                 0
#endif

#ifndef CONFIG_CAN2
    #define CONFIG_CAN2                                 0
#endif

#ifndef CONFIG_CAN_BOUD
    #define CONFIG_CAN_BOUD                             500000
#endif

#ifndef CONFIG_ETH
    #define CONFIG_ETH                                  0
#endif

#ifndef CONFIG_FLASH
    #define CONFIG_FLASH                                  0
#endif

#ifndef CONFIG_AT45_FLSH
    #define CONFIG_AT45_FLSH                                  0
#endif

/*************************************-- PW --*********************************/
#ifndef CONFIG_PW
    #define CONFIG_PW                                   0
#endif

#ifndef CONFIG_PW_DEINIT
    #define CONFIG_PW_DEINIT                            0
#endif
#ifndef CONFIG_PW_PVD_EVENT_UP
    #define CONFIG_PW_PVD_EVENT_UP                      0
#endif
#ifndef CONFIG_PW_PVD_EVENT_DOWN
    #define CONFIG_PW_PVD_EVENT_DOWN                    0
#endif
#ifndef CONFIG_PW_WAKEUP_PIN_ON
    #define CONFIG_PW_WAKEUP_PIN_ON                     0
#endif
#ifndef CONFIG_PW_SLEEP_NOW_WFI
    #define CONFIG_PW_SLEEP_NOW_WFI                     0
#endif
#ifndef CONFIG_PW_SLEEP_NOW_WFE
    #define CONFIG_PW_SLEEP_NOW_WFE                     0
#endif
#ifndef CONFIG_PW_SLEEP_ON_EXIT
    #define CONFIG_PW_SLEEP_ON_EXIT                     0
#endif
#ifndef CONFIG_PW_STOP_MODE
    #define CONFIG_PW_STOP_MODE                         0
#endif
#ifndef CONFIG_PW_MODE_LOW_POWER
    #define CONFIG_PW_MODE_LOW_POWER                    0
#endif
#ifndef CONFIG_PW_STANDBY_MODE
    #define CONFIG_PW_STANDBY_MODE                      0
#endif
#ifndef CONFIG_PW_IS_FROM_WAKEUP
    #define CONFIG_PW_IS_FROM_WAKEUP                    0
#endif
#ifndef CONFIG_PW_IWDG_CONFIG
    #define CONFIG_PW_IWDG_CONFIG                       0
#endif
#ifndef CONFIG_PW_IWDG_UP
    #define CONFIG_PW_IWDG_UP                           0
#endif
#ifndef CONFIG_PW_IWDG_DOWN
    #define CONFIG_PW_IWDG_DOWN                         0
#endif
#ifndef CONFIG_PW_IWDG_FEED
    #define CONFIG_PW_IWDG_FEED                         0
#endif

/******************************************************************************/
/*******************@@@@@@@@@@@@@@@@@@- PLAT -@@@@@@@@@@@@@@@@@@@@@@@@@********/
/******************************************************************************/
/******************************-- 基于串口的通信协议 --************************/
#ifndef CONFIG_COM_PROTOCOL
    #define CONFIG_COM_PROTOCOL                         0
#endif
#ifndef CONFIG_COM_PROTOCOL_CHN_NUM
    #define CONFIG_COM_PROTOCOL_CHN_NUM                 1
#endif
#ifndef CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE
    #define CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE          0
#endif

/************************************-- 3983电机 --****************************/
#ifndef CONFIG_A3983
    #define CONFIG_A3983                                0
#endif
#ifdef CONFIG_A3983
#ifndef MAX_MO
    #define MAX_MO                                      1
#endif
#endif

/************************************-- ULN2003 --*****************************/
#ifndef CONFIG_ULN2003
    #define CONFIG_ULN2003                              0
#endif
#ifdef CONFIG_ULN2003
#ifndef MAX_MO1
    #define MAX_MO1                                     1
#endif
#endif
#ifndef MO_ULN2003_OP_CONFIRM_STEPS
    #define MO_ULN2003_OP_CONFIRM_STEPS                 4
#endif


#ifndef ULN2003_4_CW1
    #define ULN2003_4_CW1                               0x06
#endif
#ifndef ULN2003_4_CW2
    #define ULN2003_4_CW2                               0x03
#endif
#ifndef ULN2003_4_CW3
    #define ULN2003_4_CW3                               0x09
#endif
#ifndef ULN2003_4_CW4
    #define ULN2003_4_CW4                               0x0c
#endif


#ifndef ULN2003_8_CW1
    #define ULN2003_8_CW1                               0x08
#endif
#ifndef ULN2003_8_CW2
    #define ULN2003_8_CW2                               0x0c
#endif
#ifndef ULN2003_8_CW3
    #define ULN2003_8_CW3                               0x04
#endif
#ifndef ULN2003_8_CW4
    #define ULN2003_8_CW4                               0x06
#endif
#ifndef ULN2003_8_CW5
    #define ULN2003_8_CW5                               0x02
#endif
#ifndef ULN2003_8_CW6
    #define ULN2003_8_CW6                               0x03
#endif
#ifndef ULN2003_8_CW7
    #define ULN2003_8_CW7                               0x01
#endif
#ifndef ULN2003_8_CW8
    #define ULN2003_8_CW8                               0x09
#endif

/************************************-- AT45存储器 --**************************/
#ifndef CONFIG_AT45
    #define CONFIG_AT45                                 0
#endif
#ifndef CONFIG_AT45_SPI_DMA
    #define CONFIG_AT45_SPI_DMA                         0
#endif
#ifndef CONFIG_AT45_ALL_FUNC
    #define CONFIG_AT45_ALL_FUNC                        0
#endif
#ifndef AT45_PAGE_512
    #define AT45_PAGE_512                               0
#endif

/************************************-- CAT3626驱动器 --***********************/
#ifndef CONFIG_CAT3626
    #define CONFIG_CAT3626                              0
#endif
#ifndef CONFIG_CAT3626_IO
    #define CONFIG_CAT3626_IO                           0
#endif
#ifndef CONFIG_CAT3626_HW
    #define CONFIG_CAT3626_HW                           0
#endif
#ifndef CONFIG_CAT3626_DMA
    #define CONFIG_CAT3626_DMA                          0
#endif
#ifndef CONFIG_CAT3626_IRQ
    #define CONFIG_CAT3626_IRQ                          0
#endif

/************************************-- MCP3202 --*****************************/
#ifndef CONFIG_MCP3202
    #define CONFIG_MCP3202                              0
#endif

/************************************-- XCAN --********************************/
#ifndef CONFIG_XCAN
    #define CONFIG_XCAN                                 0
#endif

/************************************-- COM --*********************************/
#ifndef CONFIG_COM
    #define CONFIG_COM                                  0
#endif

/************************************-- AD0822 --******************************/
#ifndef CONFIG_AD9822
    #define CONFIG_AD9822                               0
#endif

/************************************-- AP3156 --******************************/
#ifndef CONFIG_AP3156
    #define CONFIG_AP3156                               0
#endif

/************************************-- PARA --********************************/
#ifndef CONFIG_PARA
    #define CONFIG_PARA                                 0
#endif

/************************************-- BEEP --********************************/
#ifndef CONFIG_BEEP
    #define CONFIG_BEEP                                 0
#endif
#ifndef CONFIG_BEEP_WRITE_BIT
    #define CONFIG_BEEP_WRITE_BIT                       0
#endif
#ifndef CONFIG_BEEP_CLK_DRIVE
    #define CONFIG_BEEP_CLK_DRIVE                       0
#endif
#ifndef CONFIG_BEEP_CLK_DRIVE_SET_SPD
    #define CONFIG_BEEP_CLK_DRIVE_SET_SPD               0
#endif

/************************************-- PS2 --*********************************/
#ifndef CONFIG_PS2
    #define CONFIG_PS2                                  0
#endif
#ifndef CONFIG_PS2_CTR
    #define CONFIG_PS2_CTR                              0
#endif

/************************************-- PRINTER --*****************************/
#ifndef CONFIG_PRINTER
    #define CONFIG_PRINTER                              0
#endif
#ifndef CONFIG_PRINTER_COM_DMA
    #define CONFIG_PRINTER_COM_DMA                      0
#endif

/************************************-- 18B20 --*******************************/
#ifndef CONFIG_DS18B20
    #define CONFIG_DS18B20                              0
#endif
#ifndef CONFIG_TCS230
    #define CONFIG_TCS230                               0
#endif
#ifndef CONFIG_ADS7843
    #define CONFIG_ADS7843                              0
#endif
#ifndef CONFIG_ENC28
    #define CONFIG_ENC28                                0
#endif

/************************************-- 18B20 --*******************************/
#ifndef CONFIG_WIFI
    #define CONFIG_WIFI                                 0
#endif

/************************************-- MPR03X --******************************/
#ifndef CONFIG_MPR03X
    #define CONFIG_MPR03X                               0
#endif

/************************************-- BUTTONS --*****************************/
#ifndef CONFIG_BUTTONS
    #define CONFIG_BUTTONS                              0
#endif
#ifdef  CONFIG_BUTTONS
#ifndef CONFIG_BUTTONS_NUM
    #define CONFIG_BUTTONS_NUM                          1
#endif
#endif
#ifndef CONFIG_BUTTONS_IS_PRESSED
    #define CONFIG_BUTTONS_IS_PRESSED                   0
#endif
#ifndef CONFIG_BUTTONS_PRESSED
    #define CONFIG_BUTTONS_PRESSED                      0
#endif
#ifndef CONFIG_BUTTONS_LONG_PRESSED
    #define CONFIG_BUTTONS_LONG_PRESSED                 0
#endif

/************************************-- BlueTooth --***************************/
#ifndef CONFIG_BT_HW6
    #define CONFIG_BT_HW6                               0
#endif
#ifndef CONFIG_BT_I412
    #define CONFIG_BT_I412                              0
#endif
#ifndef CONFIG_BT_DEBUG
    #define CONFIG_BT_DEBUG                             0
#endif
#ifndef CONFIG_BT_RX_BUFF_SIZE
    #define CONFIG_BT_RX_BUFF_SIZE                      26
#endif
#ifndef CONFIG_BT_UNLINK
    #define CONFIG_BT_UNLINK                            0
#endif
#ifndef CONFIG_BT_SET_ROLE
    #define CONFIG_BT_SET_ROLE                          0
#endif
#ifndef CONFIG_BT_CHEAK_BAUD
    #define CONFIG_BT_CHEAK_BAUD                        0
#endif
#ifndef CONFIG_BT_SET_NAME
    #define CONFIG_BT_SET_NAME                          0
#endif
#ifndef CONFIG_BT_GET_NAME
    #define CONFIG_BT_GET_NAME                          0
#endif
#ifndef CONFIG_BT_SET_PIN
    #define CONFIG_BT_SET_PIN                           0
#endif
#ifndef CONFIG_BT_CLEAR
    #define CONFIG_BT_CLEAR                             0
#endif
#ifndef CONFIG_BT_SEARCH
    #define CONFIG_BT_SEARCH                            0
#endif
#ifndef CONFIG_BT_GET_MAC
    #define CONFIG_BT_GET_MAC                           0
#endif
#ifndef CONFIG_BT_SET_BAUD
    #define CONFIG_BT_SET_BAUD                          0
#endif
#ifndef CONFIG_BT_IS_CONNECTED
    #define CONFIG_BT_IS_CONNECTED                      0
#endif
#ifndef CONFIG_BT_IS_DEV_EXIST
    #define CONFIG_BT_IS_DEV_EXIST                      0
#endif
#ifndef CONFIG_BT_NITOCE
    #define CONFIG_BT_NITOCE                            0
#endif
#ifndef CONFIG_BT_SET_DEFAULT
    #define CONFIG_BT_SET_DEFAULT                       0
#endif
#ifndef CONFIG_BT_SET_NITOCE_HANLDER
    #define CONFIG_BT_SET_NITOCE_HANLDER                0
#endif
#ifndef CONFIG_BT_AUTO_LINK
    #define CONFIG_BT_AUTO_LINK                         0
#endif
#ifndef CONFIG_BT_SET_RESET
    #define CONFIG_BT_SET_RESET                         0
#endif
#ifndef CONFIG_BT_SET_ADV
    #define CONFIG_BT_SET_ADV                           0
#endif
#ifndef CONFIG_BT_GET_CONN_INFO
    #define CONFIG_BT_GET_CONN_INFO                     0
#endif

#ifndef CONFIG_LAN8720
    #define CONFIG_LAN8720                              0
#endif

#ifndef CONFIG_PWM
    #define CONFIG_PWM                                  0
#endif
#ifndef CONFIG_PWM_DEV_NUM
    #define CONFIG_PWM_DEV_NUM                          4
#endif

#ifndef CONFIG_KB1281
    #define CONFIG_KB1281                               0
#endif

#ifndef CONFIG_THERMISTOR
    #define CONFIG_THERMISTOR                           0
#endif

#ifndef CONFIG_SNAPSW
    #define CONFIG_SNAPSW                               0
#endif

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