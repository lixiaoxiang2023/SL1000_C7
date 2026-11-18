/**
******************************************************************************
* @file    stm32f2xx.h
* @author  MCD Application Team
* @version V1.0.0
* @date    18-April-2011
* @brief   CMSIS Cortex-M3 Device Peripheral Access Layer Header File.
*          This file contains all the peripheral register's definitions, bits
*          definitions and memory mapping for STM32F2xx devices.
*
*          The file is the unique include file that the application programmer
*          is using in the C source code, usually in main.c. This file contains:
*           - Configuration section that allows to select:
*              - The device used in the target application
*              - To use or not the peripheral’s drivers in application code(i.e.
*                code will be based on direct access to peripheral’s registers
*                rather than drivers API), this option is controlled by
*                "#define USE_STDPERIPH_DRIVER"
*              - To change few application-specific parameters such as the HSE
*                crystal frequency
*           - Data structures and the address mapping for all peripherals
*           - Peripheral's registers declarations and bits definition
*           - Macros to access peripheral’s registers hardware
*
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
******************************************************************************
*/

/** @addtogroup CMSIS
* @{
*/

/** @addtogroup stm32f2xx
* @{
*/

#ifndef __STM32F2xx_H
#define __STM32F2xx_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    /** @addtogroup Library_configuration_section
    * @{
    */

    /* Uncomment the line below according to the target STM32 device used in your
    application
    */

#if !defined (STM32F2XX)
#define STM32F2XX
#endif

    /*  Tip: To avoid modifying this file each time you need to switch between these
    devices, you can define the device in your toolchain compiler preprocessor.
    */

#if !defined (STM32F2XX)
#error "Please select first the target STM32F2XX device used in your application (in stm32f2xx.h file)"
#endif

#if !defined  (USE_STDPERIPH_DRIVER)
    /**
    * @brief Comment the line below if you will not use the peripherals drivers.
    In this case, these drivers will not be included and the application code will
    be based on direct access to peripherals registers
    */
#define USE_STDPERIPH_DRIVER
#endif /* USE_STDPERIPH_DRIVER */

    /**
    * @brief In the following line adjust the value of External High Speed oscillator (HSE)
    used in your application

    Tip: To avoid modifying this file each time you need to use different HSE, you
    can define the HSE value in your toolchain compiler preprocessor.
    */
#define HSE_VALUE            ((uint32_t)8000000) /*!< Value of the External oscillator in Hz */

    /**
    * @brief In the following line adjust the External High Speed oscillator (HSE) Startup
    Timeout value
    */
#define HSE_STARTUP_TIMEOUT  ((uint16_t)0x0500)   /*!< Time out for HSE start up */
#define HSI_VALUE            ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/

    /**
    * @brief STM32F2Xxx Standard Peripherals Library version number V1.0.0
    */
#define __STM32F2XX_STDPERIPH_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F2XX_STDPERIPH_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32F2XX_STDPERIPH_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32F2XX_STDPERIPH_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32F2XX_STDPERIPH_VERSION        ((__STM32F2XX_STDPERIPH_VERSION_MAIN << 24)\
    |(__STM32F2XX_STDPERIPH_VERSION_SUB1 << 16)\
        |(__STM32F2XX_STDPERIPH_VERSION_SUB2 << 8)\
            |(__STM32F2XX_STDPERIPH_VERSION_RC))

/**
* @}
*/

/** @addtogroup Configuration_section_for_CMSIS
* @{
*/

/**
* @brief Configuration of the Cortex-M3 Processor and Core Peripherals
*/
#define __MPU_PRESENT             1 /*!< STM32F2XX provide an MPU */
#define __NVIC_PRIO_BITS          4 /*!< STM32F2XX uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used */

/**
* @brief STM32F2XX Interrupt Number Definition, according to the selected device
*        in @ref Library_configuration_section
*/
typedef enum IRQn
{
    /******  Cortex-M3 Processor Exceptions Numbers ****************************************************************/
    NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                                          */
    MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt                           */
    BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                                   */
    UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                                 */
    SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                                    */
    DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                              */
    PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                                    */
    SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                                */
    /******  STM32 specific Interrupt Numbers **********************************************************************/
    WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                                         */
    PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt                         */
    TAMP_STAMP_IRQn             = 2,      /*!< Tamper and TimeStamp interrupts through the EXTI line             */
    RTC_WKUP_IRQn               = 3,      /*!< RTC Wakeup interrupt through the EXTI line                        */
    FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                                            */
    RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                              */
    EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                              */
    EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                              */
    EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                              */
    EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                              */
    EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                              */
    DMA1_Stream0_IRQn           = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
    DMA1_Stream1_IRQn           = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
    DMA1_Stream2_IRQn           = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
    DMA1_Stream3_IRQn           = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
    DMA1_Stream4_IRQn           = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
    DMA1_Stream5_IRQn           = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
    DMA1_Stream6_IRQn           = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
    ADC_IRQn                    = 18,     /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
    CAN1_TX_IRQn                = 19,     /*!< CAN1 TX Interrupt                                                 */
    CAN1_RX0_IRQn               = 20,     /*!< CAN1 RX0 Interrupt                                                */
    CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                                */
    CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                                */
    EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                                     */
    TIM1_BRK_TIM9_IRQn          = 24,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
    TIM1_UP_TIM10_IRQn          = 25,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
    TIM1_TRG_COM_TIM11_IRQn     = 26,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
    TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                                    */
    TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                             */
    TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                             */
    TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                             */
    I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                              */
    I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                              */
    I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                              */
    I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                              */
    SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                             */
    SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                             */
    USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                                           */
    USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                                           */
    USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                                           */
    EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                                   */
    RTC_Alarm_IRQn              = 41,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
    OTG_FS_WKUP_IRQn            = 42,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
    TIM8_BRK_TIM12_IRQn         = 43,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
    TIM8_UP_TIM13_IRQn          = 44,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
    TIM8_TRG_COM_TIM14_IRQn     = 45,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
    TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                                    */
    DMA1_Stream7_IRQn           = 47,     /*!< DMA1 Stream7 Interrupt                                            */
    FSMC_IRQn                   = 48,     /*!< FSMC global Interrupt                                             */
    SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                             */
    TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                             */
    SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                             */
    UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                                            */
    UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                                            */
    TIM6_DAC_IRQn               = 54,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
    TIM7_IRQn                   = 55,     /*!< TIM7 global interrupt                                             */
    DMA2_Stream0_IRQn           = 56,     /*!< DMA2 Stream 0 global Interrupt                                    */
    DMA2_Stream1_IRQn           = 57,     /*!< DMA2 Stream 1 global Interrupt                                    */
    DMA2_Stream2_IRQn           = 58,     /*!< DMA2 Stream 2 global Interrupt                                    */
    DMA2_Stream3_IRQn           = 59,     /*!< DMA2 Stream 3 global Interrupt                                    */
    DMA2_Stream4_IRQn           = 60,     /*!< DMA2 Stream 4 global Interrupt                                    */
    ETH_IRQn                    = 61,     /*!< Ethernet global Interrupt                                         */
    ETH_WKUP_IRQn               = 62,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
    CAN2_TX_IRQn                = 63,     /*!< CAN2 TX Interrupt                                                 */
    CAN2_RX0_IRQn               = 64,     /*!< CAN2 RX0 Interrupt                                                */
    CAN2_RX1_IRQn               = 65,     /*!< CAN2 RX1 Interrupt                                                */
    CAN2_SCE_IRQn               = 66,     /*!< CAN2 SCE Interrupt                                                */
    OTG_FS_IRQn                 = 67,     /*!< USB OTG FS global Interrupt                                       */
    DMA2_Stream5_IRQn           = 68,     /*!< DMA2 Stream 5 global interrupt                                    */
    DMA2_Stream6_IRQn           = 69,     /*!< DMA2 Stream 6 global interrupt                                    */
    DMA2_Stream7_IRQn           = 70,     /*!< DMA2 Stream 7 global interrupt                                    */
    USART6_IRQn                 = 71,     /*!< USART6 global interrupt                                           */
    I2C3_EV_IRQn                = 72,     /*!< I2C3 event interrupt                                              */
    I2C3_ER_IRQn                = 73,     /*!< I2C3 error interrupt                                              */
    OTG_HS_EP1_OUT_IRQn         = 74,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
    OTG_HS_EP1_IN_IRQn          = 75,     /*!< USB OTG HS End Point 1 In global interrupt                        */
    OTG_HS_WKUP_IRQn            = 76,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
    OTG_HS_IRQn                 = 77,     /*!< USB OTG HS global interrupt                                       */
    DCMI_IRQn                   = 78,     /*!< DCMI global interrupt                                             */
    CRYP_IRQn                   = 79,     /*!< CRYP crypto global interrupt                                      */
    HASH_RNG_IRQn               = 80      /*!< Hash and Rng global interrupt                                     */
} IRQn_Type;

/**
* @}
*/

#include "core_cm3.h"
#include "system_stm32f2xx.h"
#include <stdint.h>

/** @addtogroup Exported_types
* @{
*/
/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy purpose) */
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
#include <stdbool.h>
#define FALSE false
#define TRUE  true
//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

/**
* @}
*/

/** @addtogroup Peripheral_registers_structures
* @{
*/

/**
* @brief Analog to Digital Converter
*/

typedef struct
{
    //__IO uint32_t SR;     /*!< ADC status register,                         Address offset: 0x00 */
    union {
        __IO uint32_t SR;
        struct {
            __IO uint32_t AWD:1;
            __IO uint32_t EOC:1;
            __IO uint32_t JEOC:1;
            __IO uint32_t JSTRT:1;
            __IO uint32_t STRT:1;
            __IO uint32_t OVR:1;
            __IO uint32_t Reserved31_6:26;
        }BitSR;
    };

    //__IO uint32_t CR1;    /*!< ADC control register 1,                      Address offset: 0x04 */
    union {
        __IO uint32_t CR1;
        struct {
            __IO uint32_t AWDCH:5;
            __IO uint32_t EOCIE:1;
            __IO uint32_t AWDIE:1;
            __IO uint32_t JEOCIE:1;
            __IO uint32_t SCAN:1;
            __IO uint32_t AWDSGL:1;
            __IO uint32_t JAUTO:1;
            __IO uint32_t DISCEN:1;
            __IO uint32_t JDISCEN:1;
            __IO uint32_t DISCNUM:3;
            //__IO uint32_t DUALMOD:4;
            __IO uint32_t Reserved21_16:6;
            __IO uint32_t JAWDEN:1;
            __IO uint32_t AWDEN:1;
            __IO uint32_t RES:2;
            __IO uint32_t OVRIE:1;
            __IO uint32_t Reserved31_27:5;
        }BitCR1;
    };

    //__IO uint32_t CR2;    /*!< ADC control register 2,                      Address offset: 0x08 */
    union {
        __IO uint32_t CR2;
        struct {
            __IO uint32_t ADON:1;
            __IO uint32_t CONT:1;
            __IO uint32_t Reserved7_2:6;
            __IO uint32_t DMA:1;
            __IO uint32_t DDS:1;
            __IO uint32_t EOCS:1;
            __IO uint32_t ALIGN:1;
            __IO uint32_t Reserved15_12:4;
            __IO uint32_t JEXTSEL:4;
            __IO uint32_t JEXTEN:2;
            __IO uint32_t JSWSTART:1;
            __IO uint32_t Reserved23:1;
            __IO uint32_t EXTSEL:4;
            __IO uint32_t EXTEN:2;
            __IO uint32_t SWSTART:1;
            __IO uint32_t Reserved31:1;
        }BitCR2;
    };

    __IO uint32_t SMPR1;  /*!< ADC sample time register 1,                  Address offset: 0x0C */
    __IO uint32_t SMPR2;  /*!< ADC sample time register 2,                  Address offset: 0x10 */
    __IO uint32_t JOFR1;  /*!< ADC injected channel data offset register 1, Address offset: 0x14 */
    __IO uint32_t JOFR2;  /*!< ADC injected channel data offset register 2, Address offset: 0x18 */
    __IO uint32_t JOFR3;  /*!< ADC injected channel data offset register 3, Address offset: 0x1C */
    __IO uint32_t JOFR4;  /*!< ADC injected channel data offset register 4, Address offset: 0x20 */
    __IO uint32_t HTR;    /*!< ADC watchdog higher threshold register,      Address offset: 0x24 */
    __IO uint32_t LTR;    /*!< ADC watchdog lower threshold register,       Address offset: 0x28 */
    //__IO uint32_t SQR1;   /*!< ADC regular sequence register 1,             Address offset: 0x2C */
        union {
        __IO uint32_t SQR1;
        struct {
            __IO uint32_t SQ13:5;
            __IO uint32_t SQ14:5;
            __IO uint32_t SQ15:5;
            __IO uint32_t SQ16:5;
            __IO uint32_t L:4;
            __IO uint32_t Reserved31_24:8;
        }BitSQR1;
    };

    //__IO uint32_t SQR2;   /*!< ADC regular sequence register 2,             Address offset: 0x30 */
    union {
        __IO uint32_t SQR2;
        struct {
            __IO uint32_t SQ7:5;
            __IO uint32_t SQ8:5;
            __IO uint32_t SQ9:5;
            __IO uint32_t SQ10:5;
            __IO uint32_t SQ11:5;
            __IO uint32_t SQ12:5;
            __IO uint32_t Reserved31_30:2;
        }BitSQR2;
    };

    //__IO uint32_t SQR3;   /*!< ADC regular sequence register 3,             Address offset: 0x34 */
    union {
        __IO uint32_t SQR3;
        struct {
            __IO uint32_t SQ1:5;
            __IO uint32_t SQ2:5;
            __IO uint32_t SQ3:5;
            __IO uint32_t SQ4:5;
            __IO uint32_t SQ5:5;
            __IO uint32_t SQ6:5;
            __IO uint32_t Reserved31_30:2;
        }BitSQR3;
    };

    //__IO uint32_t JSQR;   /*!< ADC injected sequence register,              Address offset: 0x38*/
    union {
        __IO uint32_t JSQR;
        struct {
            __IO uint32_t JSQ1:5;
            __IO uint32_t JSQ2:5;
            __IO uint32_t JSQ3:5;
            __IO uint32_t JSQ4:5;
            __IO uint32_t JL:2;
            __IO uint32_t Reserved31_22:10;
        }BitJSQR;
    };

    __IO uint32_t JDR1;   /*!< ADC injected data register 1,                Address offset: 0x3C */
    __IO uint32_t JDR2;   /*!< ADC injected data register 2,                Address offset: 0x40 */
    __IO uint32_t JDR3;   /*!< ADC injected data register 3,                Address offset: 0x44 */
    __IO uint32_t JDR4;   /*!< ADC injected data register 4,                Address offset: 0x48 */
    __IO uint32_t DR;     /*!< ADC regular data register,                   Address offset: 0x4C */
} ADC_TypeDef;

typedef struct
{
    //__IO uint32_t CSR;    /*!< ADC Common status register,                  Address offset: ADC1 base address + 0x300 */
    union {
        __IO uint32_t CSR;
        struct {
            __IO uint32_t OVR1:1;
            __IO uint32_t STRT1:1;
            __IO uint32_t JSTRT1:1;
            __IO uint32_t JEOC1:1;
            __IO uint32_t EOC1:1;
            __IO uint32_t AWD1:1;
            __IO uint32_t Reserved7_6:2;
            __IO uint32_t OVR2:1;
            __IO uint32_t STRT2:1;
            __IO uint32_t JSTRT2:1;
            __IO uint32_t JEOC2:1;
            __IO uint32_t EOC2:1;
            __IO uint32_t AWD2:1;
            __IO uint32_t Reserved15_14:2;
            __IO uint32_t OVR3:1;
            __IO uint32_t STRT3:1;
            __IO uint32_t JSTRT3:1;
            __IO uint32_t JEOC3:1;
            __IO uint32_t EOC3:1;
            __IO uint32_t AWD3:1;
            __IO uint32_t Reserved31_22:10;
        }BitCSR;
    };
    //__IO uint32_t CCR;    /*!< ADC common control register,                 Address offset: ADC1 base address + 0x304 */
    union {
        __IO uint32_t CCR;
        struct {
            __IO uint32_t MULTI:5;
            __IO uint32_t Reserved7_5:3;
            __IO uint32_t DELAY:4;
            __IO uint32_t Reserved12:1;
            __IO uint32_t DDS:1;
            __IO uint32_t DMA:2;
            __IO uint32_t ADCPRE:2;
            __IO uint32_t Reserved21_18:4;
            __IO uint32_t VBATE:1;
            __IO uint32_t TSVREFE:1;
            __IO uint32_t Reserved31_24:8;
        }BitCCR;
    };

    //__IO uint32_t CDR;    /*!< ADC common regular data register for dual
    //AND triple modes,                            Address offset: ADC1 base address + 0x308 */
    union {
        __IO uint32_t CDR;
        struct {
            __IO uint32_t DATA1:16;
            __IO uint32_t DATA2:16;
        }BitCDR;
    };

} ADC_Common_TypeDef;

/**
* @brief Controller Area Network TxMailBox
*/

typedef struct
{
    union{
        __IO uint32_t TIR;
        struct{
            __IO uint32_t TXRQ:1;//                       ((uint32_t)0x00000001)        /*!<Transmit Mailbox Request */
            __IO uint32_t RTR:1;//                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request */
            __IO uint32_t IDE:1;//                        ((uint32_t)0x00000004)        /*!<Identifier Extension */
            __IO uint32_t EXID:18;//                       ((uint32_t)0x001FFFF8)        /*!<Extended identifier */
            __IO uint32_t STID:11;//                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */
        }BitTIR;
    };
    union{
        __IO uint32_t TDTR;
        struct{
            __IO uint32_t DLC:4;//                       ((uint32_t)0x0000000F)        /*!<Data Length Code */
            __IO uint32_t Reserved7_4:4;
            __IO uint32_t TGT:1;//                       ((uint32_t)0x00000100)        /*!<Transmit Global Time */
            __IO uint32_t Reserved15_9:7;
            __IO uint32_t TIME:16;//                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp */
        }BitTDTR;
    };
    union{
        __IO uint32_t TDLR;
        struct{
            __IO uint32_t DATA0:8;//                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
            __IO uint32_t DATA1:8;//                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
            __IO uint32_t DATA2:8;//                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
            __IO uint32_t DATA3:8;//                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */
        }BitTDLR;
    };
    union{
        __IO uint32_t TDHR;
        struct{
            __IO uint32_t DATA4:8;//                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
            __IO uint32_t DATA5:8;//                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
            __IO uint32_t DATA6:8;//                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
            __IO uint32_t DATA7:8;//                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */
        }BitTDHR;
    };
} CAN_TxMailBox_TypeDef;

/**
* @brief Controller Area Network FIFOMailBox
*/

typedef struct
{
    union{
        __IO uint32_t RIR;
        struct{
            __IO uint32_t Reserved0:1;
            __IO uint32_t RTR:1;//                        ((uint32_t)0x00000002)        /*!<Remote Transmission Request */
            __IO uint32_t IDE:1;//                        ((uint32_t)0x00000004)        /*!<Identifier Extension */
            __IO uint32_t EXID:18;//                       ((uint32_t)0x001FFFF8)        /*!<Extended Identifier */
            __IO uint32_t STID:11;//                       ((uint32_t)0xFFE00000)        /*!<Standard Identifier or Extended Identifier */
        }BitRIR;
    };
    union{
        __IO uint32_t RDTR;
        struct{
            __IO uint32_t DLC:4;//                       ((uint32_t)0x0000000F)        /*!<Data Length Code */
            __IO uint32_t Reserved7_4:4;
            __IO uint32_t FMI:8;//                       ((uint32_t)0x0000FF00)        /*!<Filter Match Index */
            __IO uint32_t TIME:16;//                      ((uint32_t)0xFFFF0000)        /*!<Message Time Stamp */
        }BitRDTR;
    };
    union{
        __IO uint32_t RDLR;
        struct{
            __IO uint32_t DATA0:8;//                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
            __IO uint32_t DATA1:8;//                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
            __IO uint32_t DATA2:8;//                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
            __IO uint32_t DATA3:8;//                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */
        }BitRDLR;
    };
    union{
        __IO uint32_t RDHR;
        struct{
            __IO uint32_t DATA4:8;//                     ((uint32_t)0x000000FF)        /*!<Data byte 0 */
            __IO uint32_t DATA5:8;//                     ((uint32_t)0x0000FF00)        /*!<Data byte 1 */
            __IO uint32_t DATA6:8;//                     ((uint32_t)0x00FF0000)        /*!<Data byte 2 */
            __IO uint32_t DATA7:8;//                     ((uint32_t)0xFF000000)        /*!<Data byte 3 */
        }BitRDHR;
    };
} CAN_FIFOMailBox_TypeDef;

/**
* @brief Controller Area Network FilterRegister
*/

typedef struct
{
    __IO uint32_t FR1; /*!< CAN Filter bank register 1 */
    __IO uint32_t FR2; /*!< CAN Filter bank register 1 */
} CAN_FilterRegister_TypeDef;

/**
* @brief Controller Area Network
*/

typedef struct
{
    union{
        __IO uint32_t MCR;
        struct{
            __IO uint32_t INRQ:1;//                        ((uint16_t)0x0001)            /*!<Initialization Request */
            __IO uint32_t SLEEP:1;//                       ((uint16_t)0x0002)            /*!<Sleep Mode Request */
            __IO uint32_t TXFP:1;//                        ((uint16_t)0x0004)            /*!<Transmit FIFO Priority */
            __IO uint32_t RFLM:1;//                        ((uint16_t)0x0008)            /*!<Receive FIFO Locked Mode */
            __IO uint32_t NART:1;//                        ((uint16_t)0x0010)            /*!<No Automatic Retransmission */
            __IO uint32_t AWUM:1;//                        ((uint16_t)0x0020)            /*!<Automatic Wakeup Mode */
            __IO uint32_t ABOM:1;//                        ((uint16_t)0x0040)            /*!<Automatic Bus-Off Management */
            __IO uint32_t TTCM:1;//                        ((uint16_t)0x0080)            /*!<Time Triggered Communication Mode */
            __IO uint32_t Reserved14_8:7;
            __IO uint32_t RESET:1;//                       ((uint16_t)0x8000)            /*!<bxCAN software master reset */
            __IO uint32_t DBF:1;//
            __IO uint32_t Reserved31_17:15;
        }BitMCR;
    };
    union{
        __IO uint32_t MSR;
        struct{
            __IO uint32_t INAK:1;//                        ((uint16_t)0x0001)            /*!<Initialization Acknowledge */
            __IO uint32_t SLAK:1;//                        ((uint16_t)0x0002)            /*!<Sleep Acknowledge */
            __IO uint32_t ERRI:1;//                        ((uint16_t)0x0004)            /*!<Error Interrupt */
            __IO uint32_t WKUI:1;//                        ((uint16_t)0x0008)            /*!<Wakeup Interrupt */
            __IO uint32_t SLAKI:1;//                       ((uint16_t)0x0010)            /*!<Sleep Acknowledge Interrupt */
            __IO uint32_t Reserved7_5:3;
            __IO uint32_t TXM:1;//                         ((uint16_t)0x0100)            /*!<Transmit Mode */
            __IO uint32_t RXM:1;//                         ((uint16_t)0x0200)            /*!<Receive Mode */
            __IO uint32_t SAMP:1;//                        ((uint16_t)0x0400)            /*!<Last Sample Point */
            __IO uint32_t RX:1;//                          ((uint16_t)0x0800)            /*!<CAN Rx Signal */
            __IO uint32_t Reserved31_12:20;
        }BitMSR;
    };
    union{
        __IO uint32_t TSR;
        struct{
            __IO uint32_t RQCP0:1;//                       ((uint32_t)0x00000001)        /*!<Request Completed Mailbox0 */
            __IO uint32_t TXOK0:1;//                       ((uint32_t)0x00000002)        /*!<Transmission OK of Mailbox0 */
            __IO uint32_t ALST0:1;//                       ((uint32_t)0x00000004)        /*!<Arbitration Lost for Mailbox0 */
            __IO uint32_t TERR0:1;//                       ((uint32_t)0x00000008)        /*!<Transmission Error of Mailbox0 */
            __IO uint32_t Reserved6_4:3;
            __IO uint32_t ABRQ0:1;//                       ((uint32_t)0x00000080)        /*!<Abort Request for Mailbox0 */
            __IO uint32_t RQCP1:1;//                       ((uint32_t)0x00000100)        /*!<Request Completed Mailbox1 */
            __IO uint32_t TXOK1:1;//                       ((uint32_t)0x00000200)        /*!<Transmission OK of Mailbox1 */
            __IO uint32_t ALST1:1;//                       ((uint32_t)0x00000400)        /*!<Arbitration Lost for Mailbox1 */
            __IO uint32_t TERR1:1;//                       ((uint32_t)0x00000800)        /*!<Transmission Error of Mailbox1 */
            __IO uint32_t Reserved14_12:3;
            __IO uint32_t ABRQ1:1;//                       ((uint32_t)0x00008000)        /*!<Abort Request for Mailbox 1 */
            __IO uint32_t RQCP2:1;//                       ((uint32_t)0x00010000)        /*!<Request Completed Mailbox2 */
            __IO uint32_t TXOK2:1;//                       ((uint32_t)0x00020000)        /*!<Transmission OK of Mailbox 2 */
            __IO uint32_t ALST2:1;//                       ((uint32_t)0x00040000)        /*!<Arbitration Lost for mailbox 2 */
            __IO uint32_t TERR2:1;//                       ((uint32_t)0x00080000)        /*!<Transmission Error of Mailbox 2 */
            __IO uint32_t Reserved22_20:3;
            __IO uint32_t ABRQ2:1;//                       ((uint32_t)0x00800000)        /*!<Abort Request for Mailbox 2 */
            __IO uint32_t CODE:2;//                        ((uint32_t)0x03000000)        /*!<Mailbox Code */
            __IO uint32_t TME0:1;//                         ((uint32_t)0x1C000000)        /*!<TME[2:0] bits */
            __IO uint32_t TME1:1;//                         ((uint32_t)0x1C000000)        /*!<TME[2:0] bits */
            __IO uint32_t TME2:1;//                         ((uint32_t)0x1C000000)        /*!<TME[2:0] bits */
            __IO uint32_t LOW0:1;//                         ((uint32_t)0xE0000000)        /*!<LOW[2:0] bits */
            __IO uint32_t LOW1:1;//                         ((uint32_t)0xE0000000)        /*!<LOW[2:0] bits */
            __IO uint32_t LOW2:1;//                         ((uint32_t)0xE0000000)        /*!<LOW[2:0] bits */
        }BitTSR;
    };
    union{
        __IO uint32_t RF0R;
        struct{
            __IO uint32_t FMP0:2;//                       ((uint8_t)0x03)               /*!<FIFO 0 Message Pending */
            __IO uint32_t Reserved2:1;
            __IO uint32_t FULL0:1;//                      ((uint8_t)0x08)               /*!<FIFO 0 Full */
            __IO uint32_t FOVR0:1;//                      ((uint8_t)0x10)               /*!<FIFO 0 Overrun */
            __IO uint32_t RFOM0:1;//                      ((uint8_t)0x20)               /*!<Release FIFO 0 Output Mailbox */
            __IO uint32_t Reserved31_6:26;
        }BitRF0R;
    };
    union{
        __IO uint32_t RF1R;
        struct{
            __IO uint32_t FMP1:2;//                       ((uint8_t)0x03)               /*!<FIFO 0 Message Pending */
            __IO uint32_t Reserved2:1;
            __IO uint32_t FULL1:1;//                      ((uint8_t)0x08)               /*!<FIFO 0 Full */
            __IO uint32_t FOVR1:1;//                      ((uint8_t)0x10)               /*!<FIFO 0 Overrun */
            __IO uint32_t RFOM1:1;//                      ((uint8_t)0x20)               /*!<Release FIFO 0 Output Mailbox */
            __IO uint32_t Reserved31_6:26;
        }BitRF1R;
    };
    union{
        __IO uint32_t IER;
        struct{
            __IO uint32_t TMEIE:1;//                       ((uint32_t)0x00000001)        /*!<Transmit Mailbox Empty Interrupt Enable */
            __IO uint32_t FMPIE0:1;//                      ((uint32_t)0x00000002)        /*!<FIFO Message Pending Interrupt Enable */
            __IO uint32_t FFIE0:1;//                       ((uint32_t)0x00000004)        /*!<FIFO Full Interrupt Enable */
            __IO uint32_t FOVIE0:1;//                      ((uint32_t)0x00000008)        /*!<FIFO Overrun Interrupt Enable */
            __IO uint32_t FMPIE1:1;//                      ((uint32_t)0x00000010)        /*!<FIFO Message Pending Interrupt Enable */
            __IO uint32_t FFIE1:1;//                       ((uint32_t)0x00000020)        /*!<FIFO Full Interrupt Enable */
            __IO uint32_t FOVIE1:1;//                      ((uint32_t)0x00000040)        /*!<FIFO Overrun Interrupt Enable */
            __IO uint32_t Reserved7:1;
            __IO uint32_t EWGIE:1;//                       ((uint32_t)0x00000100)        /*!<Error Warning Interrupt Enable */
            __IO uint32_t EPVIE:1;//                       ((uint32_t)0x00000200)        /*!<Error Passive Interrupt Enable */
            __IO uint32_t BOFIE:1;//                       ((uint32_t)0x00000400)        /*!<Bus-Off Interrupt Enable */
            __IO uint32_t LECIE:1;//                       ((uint32_t)0x00000800)        /*!<Last Error Code Interrupt Enable */
            __IO uint32_t Reserved14_12:3;
            __IO uint32_t ERRIE:1;//                       ((uint32_t)0x00008000)        /*!<Error Interrupt Enable */
            __IO uint32_t WKUIE:1;//                       ((uint32_t)0x00010000)        /*!<Wakeup Interrupt Enable */
            __IO uint32_t SLKIE:1;//                       ((uint32_t)0x00020000)        /*!<Sleep Interrupt Enable */
            __IO uint32_t Reserved31_18:14;
        }BitIER;
    };
    union{
        __IO uint32_t ESR;
        struct{
            __IO uint32_t EWGF:1;//                        ((uint32_t)0x00000001)        /*!<Error Warning Flag */
            __IO uint32_t EPVF:1;//                        ((uint32_t)0x00000002)        /*!<Error Passive Flag */
            __IO uint32_t BOFF:1;//                        ((uint32_t)0x00000004)        /*!<Bus-Off Flag */
            __IO uint32_t Reserved3:1;
            __IO uint32_t LEC:3;//                         ((uint32_t)0x00000070)        /*!<LEC[2:0] bits (Last Error Code) */
            __IO uint32_t Reserved15_7:9;
            __IO uint32_t TEC:8;//                         ((uint32_t)0x00FF0000)        /*!<Least significant byte of the 9-bit Transmit Error Counter */
            __IO uint32_t REC:8;//                         ((uint32_t)0xFF000000)        /*!<Receive Error Counter */
        }BitESR;
    };
    union{
        __IO uint32_t BTR;
        struct{
            __IO uint32_t BRP:10;//                         ((uint32_t)0x000003FF)        /*!<Baud Rate Prescaler */
            __IO uint32_t Reserved15_10:6;
            __IO uint32_t TS1:4;//                         ((uint32_t)0x000F0000)        /*!<Time Segment 1 */
            __IO uint32_t TS2:3;//                         ((uint32_t)0x00700000)        /*!<Time Segment 2 */
            __IO uint32_t Reserved23:1;
            __IO uint32_t SJW:2;//                         ((uint32_t)0x03000000)        /*!<Resynchronization Jump Width */
            __IO uint32_t Reserved29_26:4;
            __IO uint32_t LBKM:1;//                        ((uint32_t)0x40000000)        /*!<Loop Back Mode (Debug) */
            __IO uint32_t SILM:1;//                        ((uint32_t)0x80000000)        /*!<Silent Mode */
        }BitBTR;
    };
    uint32_t  RESERVED0[88];
    CAN_TxMailBox_TypeDef sTxMailBox[3];
    CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
    uint32_t  RESERVED1[12];
    union{
        __IO uint32_t FMR;
        struct{
            __IO uint32_t FINIT:1;//                        ((uint16_t)0x3FFF)            /*!<Filter Mode */
            __IO uint32_t Reserved7_1:7;
            __IO uint32_t CAN2SB:6;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t Reserved31_14:18;
        }BitFMR;
    };
    union{
        __IO uint32_t FM1R;
        struct{
            __IO uint32_t FBM0:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FBM1:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FBM2:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FBM3:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FBM4:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FBM5:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FBM6:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FBM7:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FBM8:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FBM9:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FBM10:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FBM11:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FBM12:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FBM13:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t FBM14:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FBM15:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FBM16:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FBM17:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FBM18:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FBM19:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FBM20:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FBM21:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FBM22:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FBM23:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FBM24:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FBM25:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FBM26:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FBM27:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t Reserved31_28:4;
        }BitFM1R;
    };
    uint32_t  RESERVED2;
    union{
        __IO uint32_t FS1R;
        struct{
            __IO uint32_t FSC0:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FSC1:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FSC2:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FSC3:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FSC4:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FSC5:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FSC6:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FSC7:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FSC8:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FSC9:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FSC10:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FSC11:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FSC12:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FSC13:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t FSC14:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FSC15:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FSC16:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FSC17:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FSC18:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FSC19:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FSC20:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FSC21:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FSC22:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FSC23:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FSC24:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FSC25:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FSC26:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FSC27:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t Reserved31_28:4;
        }BitFS1R;
    };
    uint32_t  RESERVED3;
    union{
        __IO uint32_t FFA1R;
        struct{
            __IO uint32_t FFA0:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FFA1:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FFA2:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FFA3:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FFA4:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FFA5:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FFA6:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FFA7:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FFA8:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FFA9:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FFA10:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FFA11:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FFA12:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FFA13:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t FFA14:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FFA15:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FFA16:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FFA17:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FFA18:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FFA19:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FFA20:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FFA21:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FFA22:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FFA23:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FFA24:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FFA25:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FFA26:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FFA27:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t Reserved31_28:4;
        }BitFFA1R;
    };
    uint32_t  RESERVED4;
    union{
        __IO uint32_t FA1R;
        struct{
            __IO uint32_t FACT0:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FACT1:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FACT2:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FACT3:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FACT4:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FACT5:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FACT6:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FACT7:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FACT8:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FACT9:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FACT10:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FACT11:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FACT12:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FACT13:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t FACT14:1;//                       ((uint16_t)0x0001)            /*!<Filter Init Mode bit 0 */
            __IO uint32_t FACT15:1;//                       ((uint16_t)0x0002)            /*!<Filter Init Mode bit 1 */
            __IO uint32_t FACT16:1;//                       ((uint16_t)0x0004)            /*!<Filter Init Mode bit 2 */
            __IO uint32_t FACT17:1;//                       ((uint16_t)0x0008)            /*!<Filter Init Mode bit 3 */
            __IO uint32_t FACT18:1;//                       ((uint16_t)0x0010)            /*!<Filter Init Mode bit 4 */
            __IO uint32_t FACT19:1;//                       ((uint16_t)0x0020)            /*!<Filter Init Mode bit 5 */
            __IO uint32_t FACT20:1;//                       ((uint16_t)0x0040)            /*!<Filter Init Mode bit 6 */
            __IO uint32_t FACT21:1;//                       ((uint16_t)0x0080)            /*!<Filter Init Mode bit 7 */
            __IO uint32_t FACT22:1;//                       ((uint16_t)0x0100)            /*!<Filter Init Mode bit 8 */
            __IO uint32_t FACT23:1;//                       ((uint16_t)0x0200)            /*!<Filter Init Mode bit 9 */
            __IO uint32_t FACT24:1;//                      ((uint16_t)0x0400)            /*!<Filter Init Mode bit 10 */
            __IO uint32_t FACT25:1;//                      ((uint16_t)0x0800)            /*!<Filter Init Mode bit 11 */
            __IO uint32_t FACT26:1;//                      ((uint16_t)0x1000)            /*!<Filter Init Mode bit 12 */
            __IO uint32_t FACT27:1;//                      ((uint16_t)0x2000)            /*!<Filter Init Mode bit 13 */
            __IO uint32_t Reserved31_28:4;
        }BitFA1R;
    };
    uint32_t  RESERVED5[8];
    CAN_FilterRegister_TypeDef sFilterRegister[28];
} CAN_TypeDef;

/**
* @brief CRC calculation unit
*/

typedef struct
{
    __IO uint32_t DR;         /*!< CRC Data register,             Address offset: 0x00 */
    __IO uint8_t  IDR;        /*!< CRC Independent data register, Address offset: 0x04 */
    uint8_t       RESERVED0;  /*!< Reserved, 0x05                                      */
    uint16_t      RESERVED1;  /*!< Reserved, 0x06                                      */
    __IO uint32_t CR;         /*!< CRC Control register,          Address offset: 0x08 */
} CRC_TypeDef;

/**
* @brief Digital to Analog Converter
*/

typedef struct
{
    //__IO uint32_t CR;       /*!< DAC control register,                                    Address offset: 0x00 */
    union {
        __IO uint32_t CR;
        struct {
            __IO uint32_t EN1:1;
            __IO uint32_t BOFF1:1;
            __IO uint32_t TEN1:1;
            __IO uint32_t TSEL1:3;
            __IO uint32_t WAVE1:2;
            __IO uint32_t MAMP1:4;
            __IO uint32_t DMAEN1:1;
            __IO uint32_t DMAUDRIE1:1;
            __IO uint32_t Reserved15_14:2;
            __IO uint32_t EN2:1;
            __IO uint32_t BOFF2:1;
            __IO uint32_t TEN2:1;
            __IO uint32_t TSEL2:3;
            __IO uint32_t WAVE2:2;
            __IO uint32_t MAMP2:4;
            __IO uint32_t DMAEN2:1;
            __IO uint32_t DMAUDRIE2:1;
            __IO uint32_t Reserved31_30:2;
        }BitCR;
    };

    //__IO uint32_t SWTRIGR;  /*!< DAC software trigger register,                           Address offset: 0x04 */
    union {
        __IO uint32_t SWTRIGR;
        struct {
            __IO uint32_t SWTRIG1:1;
            __IO uint32_t SWTRIG2:1;
            __IO uint32_t Reserved31_2:30;
        }BitSWTRIGR;
    };
    __IO uint32_t DHR12R1;  /*!< DAC channel1 12-bit right-aligned data holding register, Address offset: 0x08 */
    __IO uint32_t DHR12L1;  /*!< DAC channel1 12-bit left aligned data holding register,  Address offset: 0x0C */
    __IO uint32_t DHR8R1;   /*!< DAC channel1 8-bit right aligned data holding register,  Address offset: 0x10 */
    __IO uint32_t DHR12R2;  /*!< DAC channel2 12-bit right aligned data holding register, Address offset: 0x14 */
    __IO uint32_t DHR12L2;  /*!< DAC channel2 12-bit left aligned data holding register,  Address offset: 0x18 */
    __IO uint32_t DHR8R2;   /*!< DAC channel2 8-bit right-aligned data holding register,  Address offset: 0x1C */
    __IO uint32_t DHR12RD;  /*!< Dual DAC 12-bit right-aligned data holding register,     Address offset: 0x20 */
    __IO uint32_t DHR12LD;  /*!< DUAL DAC 12-bit left aligned data holding register,      Address offset: 0x24 */
    __IO uint32_t DHR8RD;   /*!< DUAL DAC 8-bit right aligned data holding register,      Address offset: 0x28 */
    __IO uint32_t DOR1;     /*!< DAC channel1 data output register,                       Address offset: 0x2C */
    __IO uint32_t DOR2;     /*!< DAC channel2 data output register,                       Address offset: 0x30 */
    __IO uint32_t SR;       /*!< DAC status register,                                     Address offset: 0x34 */
} DAC_TypeDef;

/**
* @brief Debug MCU
*/

typedef struct
{
    __IO uint32_t IDCODE;  /*!< MCU device ID code,               Address offset: 0x00 */
    __IO uint32_t CR;      /*!< Debug MCU configuration register, Address offset: 0x04 */
    __IO uint32_t APB1FZ;  /*!< Debug MCU APB1 freeze register,   Address offset: 0x08 */
    __IO uint32_t APB2FZ;  /*!< Debug MCU APB2 freeze register,   Address offset: 0x0C */
}DBGMCU_TypeDef;

/**
* @brief DCMI
*/

typedef struct
{
    __IO uint32_t CR;       /*!< DCMI control register 1,                       Address offset: 0x00 */
    __IO uint32_t SR;       /*!< DCMI status register,                          Address offset: 0x04 */
    __IO uint32_t RISR;     /*!< DCMI raw interrupt status register,            Address offset: 0x08 */
    __IO uint32_t IER;      /*!< DCMI interrupt enable register,                Address offset: 0x0C */
    __IO uint32_t MISR;     /*!< DCMI masked interrupt status register,         Address offset: 0x10 */
    __IO uint32_t ICR;      /*!< DCMI interrupt clear register,                 Address offset: 0x14 */
    __IO uint32_t ESCR;     /*!< DCMI embedded synchronization code register,   Address offset: 0x18 */
    __IO uint32_t ESUR;     /*!< DCMI embedded synchronization unmask register, Address offset: 0x1C */
    __IO uint32_t CWSTRTR;  /*!< DCMI crop window start,                        Address offset: 0x20 */
    __IO uint32_t CWSIZER;  /*!< DCMI crop window size,                         Address offset: 0x24 */
    __IO uint32_t DR;       /*!< DCMI data register,                            Address offset: 0x28 */
} DCMI_TypeDef;

/**
* @brief DMA Controller
*/

typedef struct
{
    //__IO uint32_t CR;     /*!< DMA stream x configuration register      */
    union{
        __IO uint32_t CR;
        struct{
            __IO uint32_t EN:1;
            __IO uint32_t DMEIE:1;
            __IO uint32_t TEIE:1;
            __IO uint32_t HTIE:1;
            __IO uint32_t TCIE:1;
            __IO uint32_t PFCTRL:1;
            __IO uint32_t DIR:2;
            __IO uint32_t CIRC:1;
            __IO uint32_t PINC:1;
            __IO uint32_t MINC:1;
            __IO uint32_t PSIZE:2;
            __IO uint32_t MSIZE:2;
            __IO uint32_t PINCOS:1;
            __IO uint32_t PL:1;
            __IO uint32_t DBM:1;
            __IO uint32_t CT:1;
            __IO uint32_t Reserved20:1;
            __IO uint32_t PBURST:2;
            __IO uint32_t MBURST:2;
            __IO uint32_t CHSEL:4;
            __IO uint32_t Reserved31_28:4;
        }BitCR;
    };
    __IO uint32_t NDTR;   /*!< DMA stream x number of data register     */
    __IO uint32_t PAR;    /*!< DMA stream x peripheral address register */
    __IO uint32_t M0AR;   /*!< DMA stream x memory 0 address register   */
    __IO uint32_t M1AR;   /*!< DMA stream x memory 1 address register   */
    //__IO uint32_t FCR;    /*!< DMA stream x FIFO control register       */
    union{
        __IO uint32_t FCR;
        struct{
            __IO uint32_t FTH:2;
            __IO uint32_t DMDIS:1;
            __IO uint32_t FS:3;
            __IO uint32_t Reserved6:1;
            __IO uint32_t FEIE:1;
            __IO uint32_t Reserved31_8:24;
        }BitFCR;
    };
} DMA_Stream_TypeDef;

typedef struct
{
    //__IO uint32_t LISR;   /*!< DMA low interrupt status register,      Address offset: 0x00 */
    union{
        __IO uint32_t LISR;
        struct{
            __IO uint32_t FEIF0:1;
            __IO uint32_t Reserved1:1;
            __IO uint32_t DMEIF0:1;
            __IO uint32_t TEIF0:1;
            __IO uint32_t HTIF0:1;
            __IO uint32_t TCIF0:1;
            __IO uint32_t FEIF1:1;
            __IO uint32_t Reserved7:1;
            __IO uint32_t DMEIF1:1;
            __IO uint32_t TEIF1:1;
            __IO uint32_t HTIF1:1;
            __IO uint32_t TCIF1:1;
            __IO uint32_t Reserved15_12:4;
            __IO uint32_t FEIF2:1;
            __IO uint32_t Reserved17:1;
            __IO uint32_t DMEIF2:1;
            __IO uint32_t TEIF2:1;
            __IO uint32_t HTIF2:1;
            __IO uint32_t TCIF2:1;
            __IO uint32_t FEIF3:1;
            __IO uint32_t Reserved23:1;
            __IO uint32_t DMEIF3:1;
            __IO uint32_t TEIF3:1;
            __IO uint32_t HTIF3:1;
            __IO uint32_t TCIF3:1;
            __IO uint32_t Reserved31_28:4;
        }BitLISR;
    };
    //__IO uint32_t HISR;   /*!< DMA high interrupt status register,     Address offset: 0x04 */
    union{
        __IO uint32_t HISR;
        struct{
            __IO uint32_t FEIF4:1;
            __IO uint32_t Reserved1:1;
            __IO uint32_t DMEIF4:1;
            __IO uint32_t TEIF4:1;
            __IO uint32_t HTIF4:1;
            __IO uint32_t TCIF4:1;
            __IO uint32_t FEIF5:1;
            __IO uint32_t Reserved7:1;
            __IO uint32_t DMEIF5:1;
            __IO uint32_t TEIF5:1;
            __IO uint32_t HTIF5:1;
            __IO uint32_t TCIF5:1;
            __IO uint32_t Reserved15_12:4;
            __IO uint32_t FEIF6:1;
            __IO uint32_t Reserved17:1;
            __IO uint32_t DMEIF6:1;
            __IO uint32_t TEIF6:1;
            __IO uint32_t HTIF6:1;
            __IO uint32_t TCIF6:1;
            __IO uint32_t FEIF7:1;
            __IO uint32_t Reserved23:1;
            __IO uint32_t DMEIF7:1;
            __IO uint32_t TEIF7:1;
            __IO uint32_t HTIF7:1;
            __IO uint32_t TCIF7:1;
            __IO uint32_t Reserved31_28:4;
        }BitHISR;
    };

    //__IO uint32_t LIFCR;  /*!< DMA low interrupt flag clear register,  Address offset: 0x08 */
    union{
        __IO uint32_t LIFCR;
        struct{
            __IO uint32_t CFEIF0:1;
            __IO uint32_t Reserved1:1;
            __IO uint32_t CDMEIF0:1;
            __IO uint32_t CTEIF0:1;
            __IO uint32_t CHTIF0:1;
            __IO uint32_t CTCIF0:1;
            __IO uint32_t CFEIF1:1;
            __IO uint32_t Reserved7:1;
            __IO uint32_t CDMEIF1:1;
            __IO uint32_t CTEIF1:1;
            __IO uint32_t CHTIF1:1;
            __IO uint32_t CTCIF1:1;
            __IO uint32_t Reserved15_12:4;
            __IO uint32_t CFEIF2:1;
            __IO uint32_t Reserved17:1;
            __IO uint32_t CDMEIF2:1;
            __IO uint32_t CTEIF2:1;
            __IO uint32_t CHTIF2:1;
            __IO uint32_t CTCIF2:1;
            __IO uint32_t CFEIF3:1;
            __IO uint32_t Reserved23:1;
            __IO uint32_t CDMEIF3:1;
            __IO uint32_t CTEIF3:1;
            __IO uint32_t CHTIF3:1;
            __IO uint32_t CTCIF3:1;
            __IO uint32_t Reserved31_28:4;
        }BitLIFCR;
    };
    //__IO uint32_t HIFCR;  /*!< DMA high interrupt flag clear register, Address offset: 0x0C */
    union{
        __IO uint32_t HIFCR;
        struct{
            __IO uint32_t CFEIF4:1;
            __IO uint32_t Reserved1:1;
            __IO uint32_t CDMEIF4:1;
            __IO uint32_t CTEIF4:1;
            __IO uint32_t CHTIF4:1;
            __IO uint32_t CTCIF4:1;
            __IO uint32_t CFEIF5:1;
            __IO uint32_t Reserved7:1;
            __IO uint32_t CDMEIF5:1;
            __IO uint32_t CTEIF5:1;
            __IO uint32_t CHTIF5:1;
            __IO uint32_t CTCIF5:1;
            __IO uint32_t Reserved15_12:4;
            __IO uint32_t CFEIF6:1;
            __IO uint32_t Reserved17:1;
            __IO uint32_t CDMEIF6:1;
            __IO uint32_t CTEIF6:1;
            __IO uint32_t CHTIF6:1;
            __IO uint32_t CTCIF6:1;
            __IO uint32_t CFEIF7:1;
            __IO uint32_t Reserved23:1;
            __IO uint32_t CDMEIF7:1;
            __IO uint32_t CTEIF7:1;
            __IO uint32_t CHTIF7:1;
            __IO uint32_t CTCIF7:1;
            __IO uint32_t Reserved31_28:4;
        }BitHIFCR;
    };

    DMA_Stream_TypeDef Stream[8];
} DMA_TypeDef;

/**
* @brief Ethernet MAC
*/

typedef struct
{
    __IO uint32_t MACCR;
    __IO uint32_t MACFFR;
    __IO uint32_t MACHTHR;
    __IO uint32_t MACHTLR;
    __IO uint32_t MACMIIAR;
    __IO uint32_t MACMIIDR;
    __IO uint32_t MACFCR;
    __IO uint32_t MACVLANTR;             /*    8 */
    uint32_t      RESERVED0[2];
    __IO uint32_t MACRWUFFR;             /*   11 */
    __IO uint32_t MACPMTCSR;
    uint32_t      RESERVED1[2];
    __IO uint32_t MACSR;                 /*   15 */
    __IO uint32_t MACIMR;
    __IO uint32_t MACA0HR;
    __IO uint32_t MACA0LR;
    __IO uint32_t MACA1HR;
    __IO uint32_t MACA1LR;
    __IO uint32_t MACA2HR;
    __IO uint32_t MACA2LR;
    __IO uint32_t MACA3HR;
    __IO uint32_t MACA3LR;               /*   24 */
    uint32_t      RESERVED2[40];
    __IO uint32_t MMCCR;                 /*   65 */
    __IO uint32_t MMCRIR;
    __IO uint32_t MMCTIR;
    __IO uint32_t MMCRIMR;
    __IO uint32_t MMCTIMR;               /*   69 */
    uint32_t      RESERVED3[14];
    __IO uint32_t MMCTGFSCCR;            /*   84 */
    __IO uint32_t MMCTGFMSCCR;
    uint32_t      RESERVED4[5];
    __IO uint32_t MMCTGFCR;
    uint32_t      RESERVED5[10];
    __IO uint32_t MMCRFCECR;
    __IO uint32_t MMCRFAECR;
    uint32_t      RESERVED6[10];
    __IO uint32_t MMCRGUFCR;
    uint32_t      RESERVED7[334];
    __IO uint32_t PTPTSCR;
    __IO uint32_t PTPSSIR;
    __IO uint32_t PTPTSHR;
    __IO uint32_t PTPTSLR;
    __IO uint32_t PTPTSHUR;
    __IO uint32_t PTPTSLUR;
    __IO uint32_t PTPTSAR;
    __IO uint32_t PTPTTHR;
    __IO uint32_t PTPTTLR;
    __IO uint32_t RESERVED8;
    __IO uint32_t PTPTSSR;  /* added for STM32F2xx */
    uint32_t      RESERVED9[565];
    __IO uint32_t DMABMR;
    __IO uint32_t DMATPDR;
    __IO uint32_t DMARPDR;
    __IO uint32_t DMARDLAR;
    __IO uint32_t DMATDLAR;
    __IO uint32_t DMASR;
    __IO uint32_t DMAOMR;
    __IO uint32_t DMAIER;
    __IO uint32_t DMAMFBOCR;
    __IO uint32_t DMARSWTR;  /* added for STM32F2xx */
    uint32_t      RESERVED10[8];
    __IO uint32_t DMACHTDR;
    __IO uint32_t DMACHRDR;
    __IO uint32_t DMACHTBAR;
    __IO uint32_t DMACHRBAR;
} ETH_TypeDef;

/**
* @brief External Interrupt/Event Controller
*/

typedef struct
{
    union{
        __IO uint32_t IMR;
        struct{
            __IO uint32_t IMR0:1;//                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
            __IO uint32_t IMR1:1;//                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
            __IO uint32_t IMR2:1;//                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
            __IO uint32_t IMR3:1;//                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
            __IO uint32_t IMR4:1;//                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
            __IO uint32_t IMR5:1;//                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
            __IO uint32_t IMR6:1;//                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
            __IO uint32_t IMR7:1;//                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
            __IO uint32_t IMR8:1;//                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
            __IO uint32_t IMR9:1;//                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
            __IO uint32_t IMR10:1;//                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
            __IO uint32_t IMR11:1;//                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
            __IO uint32_t IMR12:1;//                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
            __IO uint32_t IMR13:1;//                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
            __IO uint32_t IMR14:1;//                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
            __IO uint32_t IMR15:1;//                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
            __IO uint32_t IMR16:1;//                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
            __IO uint32_t IMR17:1;//                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
            __IO uint32_t IMR18:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t IMR19:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t IMR20:1;
            __IO uint32_t IMR21:1;
            __IO uint32_t IMR22:1;
            __IO uint32_t Reserved31_23:9;
        }BitIMR;
    };
    union{
        __IO uint32_t EMR;
        struct{
            __IO uint32_t EMR0:1;//                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
            __IO uint32_t EMR1:1;//                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
            __IO uint32_t EMR2:1;//                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
            __IO uint32_t EMR3:1;//                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
            __IO uint32_t EMR4:1;//                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
            __IO uint32_t EMR5:1;//                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
            __IO uint32_t EMR6:1;//                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
            __IO uint32_t EMR7:1;//                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
            __IO uint32_t EMR8:1;//                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
            __IO uint32_t EMR9:1;//                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
            __IO uint32_t EMR10:1;//                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
            __IO uint32_t EMR11:1;//                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
            __IO uint32_t EMR12:1;//                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
            __IO uint32_t EMR13:1;//                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
            __IO uint32_t EMR14:1;//                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
            __IO uint32_t EMR15:1;//                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
            __IO uint32_t EMR16:1;//                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
            __IO uint32_t EMR17:1;//                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
            __IO uint32_t EMR18:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t EMR19:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t EMR20:1;
            __IO uint32_t EMR21:1;
            __IO uint32_t EMR22:1;
            __IO uint32_t Reserved31_23:9;
        }BitEMR;
    };
    union{
        __IO uint32_t RTSR;
        struct{
            __IO uint32_t RTSR0:1;//                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
            __IO uint32_t RTSR1:1;//                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
            __IO uint32_t RTSR2:1;//                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
            __IO uint32_t RTSR3:1;//                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
            __IO uint32_t RTSR4:1;//                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
            __IO uint32_t RTSR5:1;//                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
            __IO uint32_t RTSR6:1;//                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
            __IO uint32_t RTSR7:1;//                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
            __IO uint32_t RTSR8:1;//                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
            __IO uint32_t RTSR9:1;//                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
            __IO uint32_t RTSR10:1;//                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
            __IO uint32_t RTSR11:1;//                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
            __IO uint32_t RTSR12:1;//                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
            __IO uint32_t RTSR13:1;//                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
            __IO uint32_t RTSR14:1;//                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
            __IO uint32_t RTSR15:1;//                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
            __IO uint32_t RTSR16:1;//                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
            __IO uint32_t RTSR17:1;//                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
            __IO uint32_t RTSR18:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t RTSR19:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t RTSR20:1;
            __IO uint32_t RTSR21:1;
            __IO uint32_t RTSR22:1;
            __IO uint32_t Reserved31_23:9;
        }BitRTSR;
    };
    union{
        __IO uint32_t FTSR;
        struct{
            __IO uint32_t FTSR0:1;//                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
            __IO uint32_t FTSR1:1;//                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
            __IO uint32_t FTSR2:1;//                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
            __IO uint32_t FTSR3:1;//                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
            __IO uint32_t FTSR4:1;//                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
            __IO uint32_t FTSR5:1;//                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
            __IO uint32_t FTSR6:1;//                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
            __IO uint32_t FTSR7:1;//                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
            __IO uint32_t FTSR8:1;//                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
            __IO uint32_t FTSR9:1;//                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
            __IO uint32_t FTSR10:1;//                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
            __IO uint32_t FTSR11:1;//                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
            __IO uint32_t FTSR12:1;//                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
            __IO uint32_t FTSR13:1;//                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
            __IO uint32_t FTSR14:1;//                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
            __IO uint32_t FTSR15:1;//                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
            __IO uint32_t FTSR16:1;//                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
            __IO uint32_t FTSR17:1;//                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
            __IO uint32_t FTSR18:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t FTSR19:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t FTSR20:1;
            __IO uint32_t FTSR21:1;
            __IO uint32_t FTSR22:1;
            __IO uint32_t Reserved31_23:9;
        }BitFTSR;
    };
    union{
        __IO uint32_t SWIER;
        struct{
            __IO uint32_t SWIER0:1;//                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
            __IO uint32_t SWIER1:1;//                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
            __IO uint32_t SWIER2:1;//                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
            __IO uint32_t SWIER3:1;//                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
            __IO uint32_t SWIER4:1;//                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
            __IO uint32_t SWIER5:1;//                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
            __IO uint32_t SWIER6:1;//                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
            __IO uint32_t SWIER7:1;//                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
            __IO uint32_t SWIER8:1;//                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
            __IO uint32_t SWIER9:1;//                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
            __IO uint32_t SWIER10:1;//                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
            __IO uint32_t SWIER11:1;//                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
            __IO uint32_t SWIER12:1;//                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
            __IO uint32_t SWIER13:1;//                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
            __IO uint32_t SWIER14:1;//                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
            __IO uint32_t SWIER15:1;//                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
            __IO uint32_t SWIER16:1;//                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
            __IO uint32_t SWIER17:1;//                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
            __IO uint32_t SWIER18:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t SWIER19:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t SWIER20:1;
            __IO uint32_t SWIER21:1;
            __IO uint32_t SWIER22:1;
            __IO uint32_t Reserved31_23:9;
        }BitSWIER;
    };
    union{
        __IO uint32_t PR;
        struct{
            __IO uint32_t PR0:1;//                        ((uint32_t)0x00000001)        /*!< Interrupt Mask on line 0 */
            __IO uint32_t PR1:1;//                        ((uint32_t)0x00000002)        /*!< Interrupt Mask on line 1 */
            __IO uint32_t PR2:1;//                        ((uint32_t)0x00000004)        /*!< Interrupt Mask on line 2 */
            __IO uint32_t PR3:1;//                        ((uint32_t)0x00000008)        /*!< Interrupt Mask on line 3 */
            __IO uint32_t PR4:1;//                        ((uint32_t)0x00000010)        /*!< Interrupt Mask on line 4 */
            __IO uint32_t PR5:1;//                        ((uint32_t)0x00000020)        /*!< Interrupt Mask on line 5 */
            __IO uint32_t PR6:1;//                        ((uint32_t)0x00000040)        /*!< Interrupt Mask on line 6 */
            __IO uint32_t PR7:1;//                        ((uint32_t)0x00000080)        /*!< Interrupt Mask on line 7 */
            __IO uint32_t PR8:1;//                        ((uint32_t)0x00000100)        /*!< Interrupt Mask on line 8 */
            __IO uint32_t PR9:1;//                        ((uint32_t)0x00000200)        /*!< Interrupt Mask on line 9 */
            __IO uint32_t PR10:1;//                       ((uint32_t)0x00000400)        /*!< Interrupt Mask on line 10 */
            __IO uint32_t PR11:1;//                       ((uint32_t)0x00000800)        /*!< Interrupt Mask on line 11 */
            __IO uint32_t PR12:1;//                       ((uint32_t)0x00001000)        /*!< Interrupt Mask on line 12 */
            __IO uint32_t PR13:1;//                       ((uint32_t)0x00002000)        /*!< Interrupt Mask on line 13 */
            __IO uint32_t PR14:1;//                       ((uint32_t)0x00004000)        /*!< Interrupt Mask on line 14 */
            __IO uint32_t PR15:1;//                       ((uint32_t)0x00008000)        /*!< Interrupt Mask on line 15 */
            __IO uint32_t PR16:1;//                       ((uint32_t)0x00010000)        /*!< Interrupt Mask on line 16 */
            __IO uint32_t PR17:1;//                       ((uint32_t)0x00020000)        /*!< Interrupt Mask on line 17 */
            __IO uint32_t PR18:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t PR19:1;//                       ((uint32_t)0x00040000)        /*!< Interrupt Mask on line 18 */
            __IO uint32_t PR20:1;
            __IO uint32_t PR21:1;
            __IO uint32_t PR22:1;
            __IO uint32_t Reserved31_23:9;
        }BitPR;
    };
} EXTI_TypeDef;

/**
* @brief FLASH Registers
*/

typedef struct
{
    __IO uint32_t ACR;      /*!< FLASH access control register, Address offset: 0x00 */
    __IO uint32_t KEYR;     /*!< FLASH key register,            Address offset: 0x04 */
    __IO uint32_t OPTKEYR;  /*!< FLASH option key register,     Address offset: 0x08 */
    __IO uint32_t SR;       /*!< FLASH status register,         Address offset: 0x0C */
    __IO uint32_t CR;       /*!< FLASH control register,        Address offset: 0x10 */
    __IO uint32_t OPTCR;    /*!< FLASH option control register, Address offset: 0x14 */
} FLASH_TypeDef;

/**
* @brief Flexible Static Memory Controller
*/

typedef struct
{
    __IO uint32_t BTCR[8];    /*!< NOR/PSRAM chip-select control register(BCR) and chip-select timing register(BTR), Address offset: 0x00-1C */
} FSMC_Bank1_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank1E
*/

typedef struct
{
    __IO uint32_t BWTR[7];    /*!< NOR/PSRAM write timing registers, Address offset: 0x104-0x11C */
} FSMC_Bank1E_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank2
*/

typedef struct
{
    __IO uint32_t PCR2;       /*!< NAND Flash control register 2,                       Address offset: 0x60 */
    __IO uint32_t SR2;        /*!< NAND Flash FIFO status and interrupt register 2,     Address offset: 0x64 */
    __IO uint32_t PMEM2;      /*!< NAND Flash Common memory space timing register 2,    Address offset: 0x68 */
    __IO uint32_t PATT2;      /*!< NAND Flash Attribute memory space timing register 2, Address offset: 0x6C */
    uint32_t      RESERVED0;  /*!< Reserved, 0x70                                                            */
    __IO uint32_t ECCR2;      /*!< NAND Flash ECC result registers 2,                   Address offset: 0x74 */
} FSMC_Bank2_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank3
*/

typedef struct
{
    __IO uint32_t PCR3;       /*!< NAND Flash control register 3,                       Address offset: 0x80 */
    __IO uint32_t SR3;        /*!< NAND Flash FIFO status and interrupt register 3,     Address offset: 0x84 */
    __IO uint32_t PMEM3;      /*!< NAND Flash Common memory space timing register 3,    Address offset: 0x88 */
    __IO uint32_t PATT3;      /*!< NAND Flash Attribute memory space timing register 3, Address offset: 0x8C */
    uint32_t      RESERVED0;  /*!< Reserved, 0x90                                                            */
    __IO uint32_t ECCR3;      /*!< NAND Flash ECC result registers 3,                   Address offset: 0x94 */
} FSMC_Bank3_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank4
*/

typedef struct
{
    __IO uint32_t PCR4;       /*!< PC Card  control register 4,                       Address offset: 0xA0 */
    __IO uint32_t SR4;        /*!< PC Card  FIFO status and interrupt register 4,     Address offset: 0xA4 */
    __IO uint32_t PMEM4;      /*!< PC Card  Common memory space timing register 4,    Address offset: 0xA8 */
    __IO uint32_t PATT4;      /*!< PC Card  Attribute memory space timing register 4, Address offset: 0xAC */
    __IO uint32_t PIO4;       /*!< PC Card  I/O space timing register 4,              Address offset: 0xB0 */
} FSMC_Bank4_TypeDef;

/**
* @brief General Purpose I/O
*/

typedef struct
{
    //__IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
    union {
        __IO uint32_t MODER;
        struct {
            __IO uint32_t MODER0:2;
            __IO uint32_t MODER1:2;
            __IO uint32_t MODER2:2;
            __IO uint32_t MODER3:2;
            __IO uint32_t MODER4:2;
            __IO uint32_t MODER5:2;
            __IO uint32_t MODER6:2;
            __IO uint32_t MODER7:2;
            __IO uint32_t MODER8:2;
            __IO uint32_t MODER9:2;
            __IO uint32_t MODER10:2;
            __IO uint32_t MODER11:2;
            __IO uint32_t MODER12:2;
            __IO uint32_t MODER13:2;
            __IO uint32_t MODER14:2;
            __IO uint32_t MODER15:2;
        }BitMODER;
    };

    //__IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
    union {
        __IO uint32_t OTYPER;
        struct {
            __IO uint32_t OT0:1;
            __IO uint32_t OT1:1;
            __IO uint32_t OT2:1;
            __IO uint32_t OT3:1;
            __IO uint32_t OT4:1;
            __IO uint32_t OT5:1;
            __IO uint32_t OT6:1;
            __IO uint32_t OT7:1;
            __IO uint32_t OT8:1;
            __IO uint32_t OT9:1;
            __IO uint32_t OT10:1;
            __IO uint32_t OT11:1;
            __IO uint32_t OT12:1;
            __IO uint32_t OT13:1;
            __IO uint32_t OT14:1;
            __IO uint32_t OT15:1;
            __IO uint32_t Reserved31_16:16;
        }BitOTYPER;
    };

    //__IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
    union {
        __IO uint32_t OSPEEDR;
        struct {
            __IO uint32_t OSPEEDR0:2;
            __IO uint32_t OSPEEDR1:2;
            __IO uint32_t OSPEEDR2:2;
            __IO uint32_t OSPEEDR3:2;
            __IO uint32_t OSPEEDR4:2;
            __IO uint32_t OSPEEDR5:2;
            __IO uint32_t OSPEEDR6:2;
            __IO uint32_t OSPEEDR7:2;
            __IO uint32_t OSPEEDR8:2;
            __IO uint32_t OSPEEDR9:2;
            __IO uint32_t OSPEEDR10:2;
            __IO uint32_t OSPEEDR11:2;
            __IO uint32_t OSPEEDR12:2;
            __IO uint32_t OSPEEDR13:2;
            __IO uint32_t OSPEEDR14:2;
            __IO uint32_t OSPEEDR15:2;
        }BitOSPEEDR;
    };

    //__IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    union {
        __IO uint32_t PUPDR;
        struct {
            __IO uint32_t PUPDR0:2;
            __IO uint32_t PUPDR1:2;
            __IO uint32_t PUPDR2:2;
            __IO uint32_t PUPDR3:2;
            __IO uint32_t PUPDR4:2;
            __IO uint32_t PUPDR5:2;
            __IO uint32_t PUPDR6:2;
            __IO uint32_t PUPDR7:2;
            __IO uint32_t PUPDR8:2;
            __IO uint32_t PUPDR9:2;
            __IO uint32_t PUPDR10:2;
            __IO uint32_t PUPDR11:2;
            __IO uint32_t PUPDR12:2;
            __IO uint32_t PUPDR13:2;
            __IO uint32_t PUPDR14:2;
            __IO uint32_t PUPDR15:2;
        }BitPUPDR;
    };

    //__IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
    union{
        __IO uint32_t IDR;
        struct{
            __IO uint32_t IDR0:1;//                        ((uint16_t)0x0001)            /*!< Port input data, bit 0 */
            __IO uint32_t IDR1:1;//                        ((uint16_t)0x0002)            /*!< Port input data, bit 1 */
            __IO uint32_t IDR2:1;//                        ((uint16_t)0x0004)            /*!< Port input data, bit 2 */
            __IO uint32_t IDR3:1;//                        ((uint16_t)0x0008)            /*!< Port input data, bit 3 */
            __IO uint32_t IDR4:1;//                        ((uint16_t)0x0010)            /*!< Port input data, bit 4 */
            __IO uint32_t IDR5:1;//                        ((uint16_t)0x0020)            /*!< Port input data, bit 5 */
            __IO uint32_t IDR6:1;//                        ((uint16_t)0x0040)            /*!< Port input data, bit 6 */
            __IO uint32_t IDR7:1;//                        ((uint16_t)0x0080)            /*!< Port input data, bit 7 */
            __IO uint32_t IDR8:1;//                        ((uint16_t)0x0100)            /*!< Port input data, bit 8 */
            __IO uint32_t IDR9:1;//                        ((uint16_t)0x0200)            /*!< Port input data, bit 9 */
            __IO uint32_t IDR10:1;//                       ((uint16_t)0x0400)            /*!< Port input data, bit 10 */
            __IO uint32_t IDR11:1;//                       ((uint16_t)0x0800)            /*!< Port input data, bit 11 */
            __IO uint32_t IDR12:1;//                       ((uint16_t)0x1000)            /*!< Port input data, bit 12 */
            __IO uint32_t IDR13:1;//                       ((uint16_t)0x2000)            /*!< Port input data, bit 13 */
            __IO uint32_t IDR14:1;//                       ((uint16_t)0x4000)            /*!< Port input data, bit 14 */
            __IO uint32_t IDR15:1;//                       ((uint16_t)0x8000)            /*!< Port input data, bit 15 */
            __IO uint32_t Reserved16_31:16;
        }BitIDR;
    };

    //__IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
    union
    {
        __IO uint32_t ODR;
        struct
        {
            __IO uint32_t ODR0:1;//                        ((uint16_t)0x0001)            /*!< Port input data, bit 0 */
            __IO uint32_t ODR1:1;//                        ((uint16_t)0x0002)            /*!< Port input data, bit 1 */
            __IO uint32_t ODR2:1;//                        ((uint16_t)0x0004)            /*!< Port input data, bit 2 */
            __IO uint32_t ODR3:1;//                        ((uint16_t)0x0008)            /*!< Port input data, bit 3 */
            __IO uint32_t ODR4:1;//                        ((uint16_t)0x0010)            /*!< Port input data, bit 4 */
            __IO uint32_t ODR5:1;//                        ((uint16_t)0x0020)            /*!< Port input data, bit 5 */
            __IO uint32_t ODR6:1;//                        ((uint16_t)0x0040)            /*!< Port input data, bit 6 */
            __IO uint32_t ODR7:1;//                        ((uint16_t)0x0080)            /*!< Port input data, bit 7 */
            __IO uint32_t ODR8:1;//                        ((uint16_t)0x0100)            /*!< Port input data, bit 8 */
            __IO uint32_t ODR9:1;//                        ((uint16_t)0x0200)            /*!< Port input data, bit 9 */
            __IO uint32_t ODR10:1;//                       ((uint16_t)0x0400)            /*!< Port input data, bit 10 */
            __IO uint32_t ODR11:1;//                       ((uint16_t)0x0800)            /*!< Port input data, bit 11 */
            __IO uint32_t ODR12:1;//                       ((uint16_t)0x1000)            /*!< Port input data, bit 12 */
            __IO uint32_t ODR13:1;//                       ((uint16_t)0x2000)            /*!< Port input data, bit 13 */
            __IO uint32_t ODR14:1;//                       ((uint16_t)0x4000)            /*!< Port input data, bit 14 */
            __IO uint32_t ODR15:1;//                       ((uint16_t)0x8000)            /*!< Port input data, bit 15 */
            __IO uint32_t Reserved16_31:16;
        }BitODR;
    };

    //__IO uint16_t BSRRL;    /*!< GPIO port bit set/reset low register,  Address offset: 0x18      */
    union {
        __IO uint16_t BSRRL;
        struct {
            __IO uint16_t BS0:1;//                        ((uint32_t)0x00000001)        /*!< Port x Set bit 0 */
            __IO uint16_t BS1:1;//                        ((uint32_t)0x00000002)        /*!< Port x Set bit 1 */
            __IO uint16_t BS2:1;//                        ((uint32_t)0x00000004)        /*!< Port x Set bit 2 */
            __IO uint16_t BS3:1;//                        ((uint32_t)0x00000008)        /*!< Port x Set bit 3 */
            __IO uint16_t BS4:1;//                        ((uint32_t)0x00000010)        /*!< Port x Set bit 4 */
            __IO uint16_t BS5:1;//                        ((uint32_t)0x00000020)        /*!< Port x Set bit 5 */
            __IO uint16_t BS6:1;//                        ((uint32_t)0x00000040)        /*!< Port x Set bit 6 */
            __IO uint16_t BS7:1;//                        ((uint32_t)0x00000080)        /*!< Port x Set bit 7 */
            __IO uint16_t BS8:1;//                        ((uint32_t)0x00000100)        /*!< Port x Set bit 8 */
            __IO uint16_t BS9:1;//                        ((uint32_t)0x00000200)        /*!< Port x Set bit 9 */
            __IO uint16_t BS10:1;//                       ((uint32_t)0x00000400)        /*!< Port x Set bit 10 */
            __IO uint16_t BS11:1;//                       ((uint32_t)0x00000800)        /*!< Port x Set bit 11 */
            __IO uint16_t BS12:1;//                       ((uint32_t)0x00001000)        /*!< Port x Set bit 12 */
            __IO uint16_t BS13:1;//                       ((uint32_t)0x00002000)        /*!< Port x Set bit 13 */
            __IO uint16_t BS14:1;//                       ((uint32_t)0x00004000)        /*!< Port x Set bit 14 */
            __IO uint16_t BS15:1;//                       ((uint32_t)0x00008000)        /*!< Port x Set bit 15 */
        }BitBSRRL;
    };

    //__IO uint16_t BSRRH;    /*!< GPIO port bit set/reset high register, Address offset: 0x1A      */
    union {
        __IO uint16_t BSRRH;
        struct {
            __IO uint16_t BR0:1;//                        ((uint32_t)0x00010000)        /*!< Port x Reset bit 0 */
            __IO uint16_t BR1:1;//                        ((uint32_t)0x00020000)        /*!< Port x Reset bit 1 */
            __IO uint16_t BR2:1;//                        ((uint32_t)0x00040000)        /*!< Port x Reset bit 2 */
            __IO uint16_t BR3:1;//                        ((uint32_t)0x00080000)        /*!< Port x Reset bit 3 */
            __IO uint16_t BR4:1;//                        ((uint32_t)0x00100000)        /*!< Port x Reset bit 4 */
            __IO uint16_t BR5:1;//                        ((uint32_t)0x00200000)        /*!< Port x Reset bit 5 */
            __IO uint16_t BR6:1;//                        ((uint32_t)0x00400000)        /*!< Port x Reset bit 6 */
            __IO uint16_t BR7:1;//                        ((uint32_t)0x00800000)        /*!< Port x Reset bit 7 */
            __IO uint16_t BR8:1;//                        ((uint32_t)0x01000000)        /*!< Port x Reset bit 8 */
            __IO uint16_t BR9:1;//                        ((uint32_t)0x02000000)        /*!< Port x Reset bit 9 */
            __IO uint16_t BR10:1;//                       ((uint32_t)0x04000000)        /*!< Port x Reset bit 10 */
            __IO uint16_t BR11:1;//                       ((uint32_t)0x08000000)        /*!< Port x Reset bit 11 */
            __IO uint16_t BR12:1;//                       ((uint32_t)0x10000000)        /*!< Port x Reset bit 12 */
            __IO uint16_t BR13:1;//                       ((uint32_t)0x20000000)        /*!< Port x Reset bit 13 */
            __IO uint16_t BR14:1;//                       ((uint32_t)0x40000000)        /*!< Port x Reset bit 14 */
            __IO uint16_t BR15:1;//                       ((uint32_t)0x80000000)        /*!< Port x Reset bit 15 */
        }BitBSRRH;
    };

    //__IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    union {
        __IO uint32_t LCKR;
        struct {
            __IO uint32_t LCK0:1;//                        ((uint16_t)0x0001)            /*!< Port input data, bit 0 */
            __IO uint32_t LCK1:1;//                        ((uint16_t)0x0002)            /*!< Port input data, bit 1 */
            __IO uint32_t LCK2:1;//                        ((uint16_t)0x0004)            /*!< Port input data, bit 2 */
            __IO uint32_t LCK3:1;//                        ((uint16_t)0x0008)            /*!< Port input data, bit 3 */
            __IO uint32_t LCK4:1;//                        ((uint16_t)0x0010)            /*!< Port input data, bit 4 */
            __IO uint32_t LCK5:1;//                        ((uint16_t)0x0020)            /*!< Port input data, bit 5 */
            __IO uint32_t LCK6:1;//                        ((uint16_t)0x0040)            /*!< Port input data, bit 6 */
            __IO uint32_t LCK7:1;//                        ((uint16_t)0x0080)            /*!< Port input data, bit 7 */
            __IO uint32_t LCK8:1;//                        ((uint16_t)0x0100)            /*!< Port input data, bit 8 */
            __IO uint32_t LCK9:1;//                        ((uint16_t)0x0200)            /*!< Port input data, bit 9 */
            __IO uint32_t LCK10:1;//                       ((uint16_t)0x0400)            /*!< Port input data, bit 10 */
            __IO uint32_t LCK11:1;//                       ((uint16_t)0x0800)            /*!< Port input data, bit 11 */
            __IO uint32_t LCK12:1;//                       ((uint16_t)0x1000)            /*!< Port input data, bit 12 */
            __IO uint32_t LCK13:1;//                       ((uint16_t)0x2000)            /*!< Port input data, bit 13 */
            __IO uint32_t LCK14:1;//                       ((uint16_t)0x4000)            /*!< Port input data, bit 14 */
            __IO uint32_t LCK15:1;//                       ((uint16_t)0x8000)            /*!< Port input data, bit 15 */
            __IO uint32_t LCKK:1;
            __IO uint32_t Reserved31_17:15;
        }BitLCKR;
    };

    union {
        __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x24-0x28 */
        struct {
            struct {
                __IO uint32_t AFRL0:4;
                __IO uint32_t AFRL1:4;
                __IO uint32_t AFRL2:4;
                __IO uint32_t AFRL3:4;
                __IO uint32_t AFRL4:4;
                __IO uint32_t AFRL5:4;
                __IO uint32_t AFRL6:4;
                __IO uint32_t AFRL7:4;
            }BitAFRL;
            struct {
                __IO uint32_t AFRL8:4;
                __IO uint32_t AFRL9:4;
                __IO uint32_t AFRL10:4;
                __IO uint32_t AFRL11:4;
                __IO uint32_t AFRL12:4;
                __IO uint32_t AFRL13:4;
                __IO uint32_t AFRL14:4;
                __IO uint32_t AFRL15:4;
            }BitAFRH;
        }BitAFR;
    };
} GPIO_TypeDef;

/**
* @brief System configuration controller
*/

typedef struct
{
    //__IO uint32_t MEMRMP;       /*!< SYSCFG memory remap register,                      Address offset: 0x00      */
    union {
        __IO uint32_t MEMRMP;
        struct {
            __IO uint32_t MEM_MODE:2;
           __IO uint32_t Reserved31_2:30;
        }BitMEMRMP;
    };

    __IO uint32_t PMC;          /*!< SYSCFG peripheral mode configuration register,     Address offset: 0x04      */
    //__IO uint32_t EXTICR[4];    /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
    union{
        __IO uint32_t EXTICR[4];    /*!< SYSCFG external interrupt configuration registers, Address offset: 0x08-0x14 */
        struct {
            struct {
                __IO uint32_t EXTI0:4;
                __IO uint32_t EXTI1:4;
                __IO uint32_t EXTI2:4;
                __IO uint32_t EXTI3:4;
                __IO uint32_t Reserved31_16:16;
            }BitEXTICR1;
            struct {
                __IO uint32_t EXTI4:4;
                __IO uint32_t EXTI5:4;
                __IO uint32_t EXTI6:4;
                __IO uint32_t EXTI7:4;
                __IO uint32_t Reserved31_16:16;
            }BitEXTICR2;
            struct {
                __IO uint32_t EXTI8:4;
                __IO uint32_t EXTI9:4;
                __IO uint32_t EXTI10:4;
                __IO uint32_t EXTI11:4;
                __IO uint32_t Reserved31_16:16;
            }BitEXTICR3;
            struct {
                __IO uint32_t EXTI12:4;
                __IO uint32_t EXTI13:4;
                __IO uint32_t EXTI14:4;
                __IO uint32_t EXTI15:4;
                __IO uint32_t Reserved31_16:16;
            }BitEXTICR4;
        }BitEXTICR;
    };

    uint32_t      RESERVED[2];  /*!< Reserved, 0x18-0x1C                                                          */
    __IO uint32_t CMPCR;        /*!< SYSCFG Compensation cell control register,         Address offset: 0x20      */
} SYSCFG_TypeDef;

/**
* @brief Inter-integrated Circuit Interface
*/

typedef struct
{
    //__IO uint16_t CR1;        /*!< I2C Control register 1,     Address offset: 0x00 */
    union {
        __IO uint16_t CR1;
        struct {
            __IO uint16_t PE1:1;//                          ((uint16_t)0x0001)            /*!<Peripheral Enable */
            __IO uint16_t SMBUS:1;//                       ((uint16_t)0x0002)            /*!<SMBus Mode */
            __IO uint16_t Reserved2:1;
            __IO uint16_t SMBTYPE:1;//                     ((uint16_t)0x0008)            /*!<SMBus Type */
            __IO uint16_t ENARP:1;//                       ((uint16_t)0x0010)            /*!<ARP Enable */
            __IO uint16_t ENPEC:1;//                       ((uint16_t)0x0020)            /*!<PEC Enable */
            __IO uint16_t ENGC:1;//                        ((uint16_t)0x0040)            /*!<General Call Enable */
            __IO uint16_t NOSTRETCH:1;//                   ((uint16_t)0x0080)            /*!<Clock Stretching Disable (Slave mode) */
            __IO uint16_t START:1;//                       ((uint16_t)0x0100)            /*!<Start Generation */
            __IO uint16_t STOP:1;//                        ((uint16_t)0x0200)            /*!<Stop Generation */
            __IO uint16_t ACK:1;//                         ((uint16_t)0x0400)            /*!<Acknowledge Enable */
            __IO uint16_t POS:1;//                         ((uint16_t)0x0800)            /*!<Acknowledge/PEC Position (for data reception) */
            __IO uint16_t PEC:1;//                         ((uint16_t)0x1000)            /*!<Packet Error Checking */
            __IO uint16_t ALERT:1;//                       ((uint16_t)0x2000)            /*!<SMBus Alert */
            __IO uint16_t Reserved14:1;
            __IO uint16_t SWRST:1;//                       ((uint16_t)0x8000)            /*!<Software Reset */
        }BitCR1;
    };
    uint16_t      RESERVED0;  /*!< Reserved, 0x02                                   */
    //__IO uint16_t CR2;        /*!< I2C Control register 2,     Address offset: 0x04 */
    union {
        __IO uint16_t CR2;
        struct {
            __IO uint16_t FREQ:6;//                        ((uint16_t)0x003F)            /*!<FREQ[5:0] bits (Peripheral Clock Frequency) */
            __IO uint16_t Reserved7_6:2;
            __IO uint16_t ITERREN:1;//                     ((uint16_t)0x0100)            /*!<Error Interrupt Enable */
            __IO uint16_t ITEVTEN:1;//                     ((uint16_t)0x0200)            /*!<Event Interrupt Enable */
            __IO uint16_t ITBUFEN:1;//                     ((uint16_t)0x0400)            /*!<Buffer Interrupt Enable */
            __IO uint16_t DMAEN:1;//                       ((uint16_t)0x0800)            /*!<DMA Requests Enable */
            __IO uint16_t LAST:1;//                        ((uint16_t)0x1000)            /*!<DMA Last Transfer */
            __IO uint16_t Reserved15_13:3;
        }BitCR2;
    };

    uint16_t      RESERVED1;  /*!< Reserved, 0x06                                   */
    //__IO uint16_t OAR1;       /*!< I2C Own address register 1, Address offset: 0x08 */
    union {
        __IO uint16_t OAR1;
        struct {
            __IO uint16_t ADD0:1;//                       ((uint16_t)0x0001)            /*!<Bit 0 */
            __IO uint16_t ADD1_7:7;//                     ((uint16_t)0x00FE)            /*!<Interface Address */
            __IO uint16_t ADD8_9:2;//                     ((uint16_t)0x0300)            /*!<Interface Address */
            __IO uint16_t Reserved13_10:4;
            __IO uint16_t Reserved14:1;
            __IO uint16_t ADDMODE:1;//                    ((uint16_t)0x8000)            /*!<Addressing Mode (Slave mode) */
        }BitOAR1;
    };

    uint16_t      RESERVED2;  /*!< Reserved, 0x0A                                   */
    //__IO uint16_t OAR2;       /*!< I2C Own address register 2, Address offset: 0x0C */
    union {
        __IO uint16_t OAR2;
        struct {
            __IO uint16_t ENDUAL:1;//
            __IO uint16_t ADD2_7_1:7;//                       ((uint16_t)0x0001)            /*!<Bit 0 */
            __IO uint16_t Reserved15_8:8;
        }BitOAR2;
    };

    uint16_t      RESERVED3;  /*!< Reserved, 0x0E                                   */
    __IO uint16_t DR;         /*!< I2C Data register,          Address offset: 0x10 */
    uint16_t      RESERVED4;  /*!< Reserved, 0x12                                   */
    //__IO uint16_t SR1;        /*!< I2C Status register 1,      Address offset: 0x14 */
    union {
        __IO uint16_t SR1;
        struct {
            __IO uint16_t SB:1;//                          ((uint16_t)0x0001)            /*!<Start Bit (Master mode) */
            __IO uint16_t ADDR:1;//                        ((uint16_t)0x0002)            /*!<Address sent (master mode)/matched (slave mode) */
            __IO uint16_t BTF:1;//                         ((uint16_t)0x0004)            /*!<Byte Transfer Finished */
            __IO uint16_t ADD10:1;//                       ((uint16_t)0x0008)            /*!<10-bit header sent (Master mode) */
            __IO uint16_t STOPF:1;//                       ((uint16_t)0x0010)            /*!<Stop detection (Slave mode) */
            __IO uint16_t Reserved5:1;
            __IO uint16_t RXNE:1;//                        ((uint16_t)0x0040)            /*!<Data Register not Empty (receivers) */
            __IO uint16_t TXE:1;//                         ((uint16_t)0x0080)            /*!<Data Register Empty (transmitters) */
            __IO uint16_t BERR:1;//                        ((uint16_t)0x0100)            /*!<Bus Error */
            __IO uint16_t ARLO:1;//                        ((uint16_t)0x0200)            /*!<Arbitration Lost (master mode) */
            __IO uint16_t AF:1;//                          ((uint16_t)0x0400)            /*!<Acknowledge Failure */
            __IO uint16_t OVR:1;//                         ((uint16_t)0x0800)            /*!<Overrun/Underrun */
            __IO uint16_t PECERR:1;//                      ((uint16_t)0x1000)            /*!<PEC Error in reception */
            __IO uint16_t Reserved13:1;
            __IO uint16_t TIMEOUT:1;//                     ((uint16_t)0x4000)            /*!<Timeout or Tlow Error */
            __IO uint16_t SMBALERT:1;//                    ((uint16_t)0x8000)            /*!<SMBus Alert */
        }BitSR1;
    };
    uint16_t      RESERVED5;  /*!< Reserved, 0x16                                   */
    //__IO uint16_t SR2;        /*!< I2C Status register 2,      Address offset: 0x18 */
    union {
        __IO uint16_t SR2;
        struct {
            __IO uint16_t MSL:1;//                         ((uint16_t)0x0001)            /*!<Master/Slave */
            __IO uint16_t BUSY:1;//                        ((uint16_t)0x0002)            /*!<Bus Busy */
            __IO uint16_t TRA:1;//                         ((uint16_t)0x0004)            /*!<Transmitter/Receiver */
            __IO uint16_t Reserved3:1;
            __IO uint16_t GENCALL:1;//                     ((uint16_t)0x0010)            /*!<General Call Address (Slave mode) */
            __IO uint16_t SMBDEFAULT:1;//                  ((uint16_t)0x0020)            /*!<SMBus Device Default Address (Slave mode) */
            __IO uint16_t SMBHOST:1;//                     ((uint16_t)0x0040)            /*!<SMBus Host Header (Slave mode) */
            __IO uint16_t DUALF:1;//                       ((uint16_t)0x0080)            /*!<Dual Flag (Slave mode) */
            __IO uint16_t PEC:8;//                         ((uint16_t)0xFF00)            /*!<Packet Error Checking Register */
        }BitSR2;
    };

    uint16_t      RESERVED6;  /*!< Reserved, 0x1A                                   */
    __IO uint16_t CCR;        /*!< I2C Clock control register, Address offset: 0x1C */
    uint16_t      RESERVED7;  /*!< Reserved, 0x1E                                   */
    __IO uint16_t TRISE;      /*!< I2C TRISE register,         Address offset: 0x20 */
    uint16_t      RESERVED8;  /*!< Reserved, 0x22                                   */
} I2C_TypeDef;

/**
* @brief Independent WATCHDOG
*/

typedef struct
{
    __IO uint32_t KR;   /*!< IWDG Key register,       Address offset: 0x00 */
    __IO uint32_t PR;   /*!< IWDG Prescaler register, Address offset: 0x04 */
    __IO uint32_t RLR;  /*!< IWDG Reload register,    Address offset: 0x08 */
    __IO uint32_t SR;   /*!< IWDG Status register,    Address offset: 0x0C */
} IWDG_TypeDef;

/**
* @brief Power Control
*/

typedef struct
{
    //__IO uint32_t CR;   /*!< PWR power control register,        Address offset: 0x00 */
    union {
        __IO uint32_t CR;
        struct {
            __IO uint32_t LPDS:1;
            __IO uint32_t PDDS:1;
            __IO uint32_t CWUF:1;
            __IO uint32_t CSBF:1;
            __IO uint32_t PVDE:1;
            __IO uint32_t PLS:3;
            __IO uint32_t DBP:1;
            __IO uint32_t FPDS:1;
            __IO uint32_t Reserved31_10:22;
        }BitCR;
    };

    //__IO uint32_t CSR;  /*!< PWR power control/status register, Address offset: 0x04 */
    union {
        __IO uint32_t CSR;
        struct {
            __IO uint32_t WUF:1;
            __IO uint32_t SBF:1;
            __IO uint32_t PVDO:1;
            __IO uint32_t BRR:1;
            __IO uint32_t Reserved7_4:4;
            __IO uint32_t EWUP:1;
            __IO uint32_t BRE:1;
            __IO uint32_t Reserved31_10:22;
        }BitCSR;
    };
} PWR_TypeDef;

/**
* @brief Reset and Clock Control
*/

typedef struct
{
    union {
        __IO uint32_t CR;
        struct {
            __IO uint32_t HSION:1;//                        ((uint32_t)0x00000001)        /*!< Internal High Speed clock enable */
            __IO uint32_t HSIRDY:1;//                       ((uint32_t)0x00000002)        /*!< Internal High Speed clock ready flag */
            __IO uint32_t Reserved2:1;
            __IO uint32_t HSITRIM:5;//                      ((uint32_t)0x000000F8)        /*!< Internal High Speed clock trimming */
            __IO uint32_t HSICAL:8;//                       ((uint32_t)0x0000FF00)        /*!< Internal High Speed clock Calibration */
            __IO uint32_t HSEON:1;//                        ((uint32_t)0x00010000)        /*!< External High Speed clock enable */
            __IO uint32_t HSERDY:1;//                       ((uint32_t)0x00020000)        /*!< External High Speed clock ready flag */
            __IO uint32_t HSEBYP:1;//                       ((uint32_t)0x00040000)        /*!< External High Speed clock Bypass */
            __IO uint32_t CSSON:1;//                        ((uint32_t)0x00080000)        /*!< Clock Security System enable */
            __IO uint32_t Reserved23_20:4;
            __IO uint32_t PLLON:1;//                        ((uint32_t)0x01000000)        /*!< PLL enable */
            __IO uint32_t PLLRDY:1;//                       ((uint32_t)0x02000000)        /*!< PLL clock ready flag */
            __IO uint32_t PLLI2SON:1;
            __IO uint32_t PLLI2SRDY:1;
            __IO uint32_t Reserved31_28:4;
        }BitCR;
    };

        //__IO uint32_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
    union {
        __IO uint32_t PLLCFGR;
        struct {
            __IO uint32_t PLLM:6;
            __IO uint32_t PLLN:9;
            __IO uint32_t Reserved15:1;
            __IO uint32_t PLLP:2;
            __IO uint32_t Reserved21_18:4;
            __IO uint32_t PLLSRC:1;
            __IO uint32_t Reserved23:1;
            __IO uint32_t PLLQ:4;
            __IO uint32_t PLLR:4;
        }BitPLLCFGR;
    };
    //__IO uint32_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
    union {
        __IO uint32_t CFGR;
        struct {
            __IO uint32_t SW:2;//                         ((uint32_t)0x00000003)        /*!< SW[1:0] bits (System clock Switch) */
            __IO uint32_t SWS:2;//                        ((uint32_t)0x0000000C)        /*!< SWS[1:0] bits (System Clock Switch Status) */
            __IO uint32_t HPRE:4;//                       ((uint32_t)0x000000F0)        /*!< HPRE[3:0] bits (AHB prescaler) */
            __IO uint32_t Reserved9_8:2;
            __IO uint32_t PPRE1:3;//                      ((uint32_t)0x00000700)        /*!< PRE1[2:0] bits (APB1 prescaler) */
            __IO uint32_t PPRE2:3;//                      ((uint32_t)0x00003800)        /*!< PRE2[2:0] bits (APB2 prescaler) */
            __IO uint32_t RTCPRE:5;
            __IO uint32_t MCO1:2;
            __IO uint32_t I2SSCR:1;
            __IO uint32_t MCO1PRE:3;
            __IO uint32_t MCO2PRE:3;
            __IO uint32_t MCO2:2;
        }BitCFGR;
    };
    __IO uint32_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
    __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
    __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
    __IO uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
    uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                                                    */
    __IO uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
    __IO uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
    uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
    union{
        __IO uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
        struct{
            __IO uint32_t GPIOAEN:1;//                 ((uint32_t)0x00000001)
            __IO uint32_t GPIOBEN:1;//                 ((uint32_t)0x00000002)
            __IO uint32_t GPIOCEN:1;//                 ((uint32_t)0x00000004)
            __IO uint32_t GPIODEN:1;//                 ((uint32_t)0x00000008)
            __IO uint32_t GPIOEEN:1;//                 ((uint32_t)0x00000010)
            __IO uint32_t GPIOFEN:1;//                 ((uint32_t)0x00000020)
            __IO uint32_t GPIOGEN:1;//                 ((uint32_t)0x00000040)
            __IO uint32_t GPIOHEN:1;//                 ((uint32_t)0x00000080)
            __IO uint32_t GPIOIEN:1;//                 ((uint32_t)0x00000100)
            __IO uint32_t Reserved11_9:3;
            __IO uint32_t CRCEN:1;//                   ((uint32_t)0x00001000)
            __IO uint32_t Reserved17_13:5;
            __IO uint32_t BKPSRAMEN:1;//               ((uint32_t)0x00040000)
            __IO uint32_t Reserved20_19:2;
            __IO uint32_t DMA1EN:1;//                  ((uint32_t)0x00200000)
            __IO uint32_t DMA2EN:1;//                  ((uint32_t)0x00400000)
            __IO uint32_t Reserved24_23:2;
            __IO uint32_t ETHMACEN:1;//                ((uint32_t)0x02000000)
            __IO uint32_t ETHMACTXEN:1;//              ((uint32_t)0x04000000)
            __IO uint32_t ETHMACRXEN:1;//              ((uint32_t)0x08000000)
            __IO uint32_t ETHMACPTPEN:1;//             ((uint32_t)0x10000000)
            __IO uint32_t OTGHSEN:1;//                 ((uint32_t)0x20000000)
            __IO uint32_t OTGHSULPIEN:1;//             ((uint32_t)0x40000000)
            __IO uint32_t Reserved31:1;
        }BitAHB1ENR;
    };
    union{
        __IO uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
        struct{
            __IO uint32_t DCMIEN:1;//                  ((uint32_t)0x00000001)
            __IO uint32_t Reserved3_1:3;
            __IO uint32_t CRYPEN:1;//                  ((uint32_t)0x00000010)
            __IO uint32_t HASHEN:1;//                  ((uint32_t)0x00000020)
            __IO uint32_t RNGEN:1;//                   ((uint32_t)0x00000040)
            __IO uint32_t OTGFSEN:1;//                 ((uint32_t)0x00000080)
            __IO uint32_t Reserved31_8:24;
        }BitAHB2ENR;
    };

    union{
        __IO uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
        struct{
            __IO uint32_t FSMCEN:1;
            __IO uint32_t Reserved31_1:31;
        }BitAHB3ENR;
    };
    uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                                                    */
    union{
        __IO uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
        struct{
            __IO uint32_t TIM2EN:1;//                  ((uint32_t)0x00000001)
            __IO uint32_t TIM3EN:1;//                  ((uint32_t)0x00000002)
            __IO uint32_t TIM4EN:1;//                  ((uint32_t)0x00000004)
            __IO uint32_t TIM5EN:1;//                  ((uint32_t)0x00000008)
            __IO uint32_t TIM6EN:1;//                  ((uint32_t)0x00000010)
            __IO uint32_t TIM7EN:1;//                  ((uint32_t)0x00000020)
            __IO uint32_t TIM12EN:1;//                 ((uint32_t)0x00000040)
            __IO uint32_t TIM13EN:1;//                 ((uint32_t)0x00000080)
            __IO uint32_t TIM14EN:1;//                 ((uint32_t)0x00000100)
            __IO uint32_t Reserved10_9:2;
            __IO uint32_t WWDGEN:1;//                  ((uint32_t)0x00000800)
            __IO uint32_t Reserved13_12:2;
            __IO uint32_t SPI2EN:1;//                  ((uint32_t)0x00004000)
            __IO uint32_t SPI3EN:1;//                  ((uint32_t)0x00008000)
            __IO uint32_t Reserved16:1;
            __IO uint32_t USART2EN:1;//                ((uint32_t)0x00020000)
            __IO uint32_t USART3EN:1;//                ((uint32_t)0x00040000)
            __IO uint32_t UART4EN:1;//                 ((uint32_t)0x00080000)
            __IO uint32_t UART5EN:1;//                 ((uint32_t)0x00100000)
            __IO uint32_t I2C1EN:1;//                  ((uint32_t)0x00200000)
            __IO uint32_t I2C2EN:1;//                  ((uint32_t)0x00400000)
            __IO uint32_t I2C3EN:1;//                  ((uint32_t)0x00800000)
            __IO uint32_t Reserved24:1;
            __IO uint32_t CAN1EN:1;//                  ((uint32_t)0x02000000)
            __IO uint32_t CAN2EN:1;//                  ((uint32_t)0x04000000)
            __IO uint32_t Reserved27:1;
            __IO uint32_t PWREN:1;//                   ((uint32_t)0x10000000)
            __IO uint32_t DACEN:1;//                   ((uint32_t)0x20000000)
            __IO uint32_t Reserved31_30:2;
        }BitAPB1ENR;
    };
    union{
        __IO uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
        struct{
            __IO uint32_t TIM1EN:1;//                  ((uint32_t)0x00000001)
            __IO uint32_t TIM8EN:1;//                  ((uint32_t)0x00000002)
            __IO uint32_t Reserved3_2:2;
            __IO uint32_t USART1EN:1;//                ((uint32_t)0x00000010)
            __IO uint32_t USART6EN:1;//                ((uint32_t)0x00000020)
            __IO uint32_t Reserved7_6:2;
            __IO uint32_t ADC1EN:1;//                  ((uint32_t)0x00000100)
            __IO uint32_t ADC2EN:1;//                  ((uint32_t)0x00000200)
            __IO uint32_t ADC3EN:1;//                  ((uint32_t)0x00000400)
            __IO uint32_t SDIOEN:1;//                  ((uint32_t)0x00000800)
            __IO uint32_t SPI1EN:1;//                  ((uint32_t)0x00001000)
            __IO uint32_t Reserved13:1;
            __IO uint32_t SYSCFGEN:1;//                ((uint32_t)0x00004000)
            __IO uint32_t Reserved15:1;
            __IO uint32_t TIM9EN:1;//                  ((uint32_t)0x00010000)
            __IO uint32_t TIM10EN:1;//                 ((uint32_t)0x00020000)
            __IO uint32_t TIM11EN:1;//                 ((uint32_t)0x00040000)
            __IO uint32_t Reserved31_19:13;
        }BitAPB2ENR;
    };
    uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
    __IO uint32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
    __IO uint32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
    __IO uint32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
    uint32_t      RESERVED4;     /*!< Reserved, 0x5C                                                                    */
    __IO uint32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
    __IO uint32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
    uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
    //__IO uint32_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
    union {
        __IO uint32_t BDCR;
        struct {
            __IO uint32_t LSEON:1;
            __IO uint32_t LSERDY:1;
            __IO uint32_t LSEBYP:1;
            __IO uint32_t Reserved7_3:5;
            __IO uint32_t TRCSEL:2;
            __IO uint32_t Reserved14_10:5;
            __IO uint32_t RTCEN:1;
            __IO uint32_t DBRST:1;
            __IO uint32_t Reserved31_17:15;
        }BitBDCR;
    };
    __IO uint32_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
    uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
    __IO uint32_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
    __IO uint32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
} RCC_TypeDef;

/**
* @brief Real-Time Clock
*/

typedef struct
{
    //__IO uint32_t TR;      /*!< RTC time register,                                        Address offset: 0x00 */
    union {
        __IO uint32_t TR;
        struct {
            __IO uint32_t S:7;
            __IO uint32_t Reserved7:1;
            __IO uint32_t MN:7;
            __IO uint32_t Reserved15:1;
            __IO uint32_t H:6;
            __IO uint32_t PM:1;
            __IO uint32_t Reserved31_23:9;
        }BitTR;
    };
    //__IO uint32_t DR;      /*!< RTC date register,                                        Address offset: 0x04 */
    union {
        __IO uint32_t DR;
        struct {
            __IO uint32_t D:6;
            __IO uint32_t Reserved7_6:2;
            __IO uint32_t M:5;
            __IO uint32_t WDU:3;
            __IO uint32_t Y:8;
            __IO uint32_t Reserved31_24:8;
        }BitDR;
    };
    //__IO uint32_t CR;      /*!< RTC control register,                                     Address offset: 0x08 */
    union {
        __IO uint32_t CR;
        struct {

            __IO uint32_t WUCKSEL:3;
            __IO uint32_t TSEDGE:1;
            __IO uint32_t REFCKON:1;
            __IO uint32_t Reserved5:1;
            __IO uint32_t FMT:1;
            __IO uint32_t DCE:1;
            __IO uint32_t ALRAE:1;
            __IO uint32_t ALRBE:1;
            __IO uint32_t WUTE:1;
            __IO uint32_t TSE:1;
            __IO uint32_t ALRAIE:1;
            __IO uint32_t ALRBIE:1;
            __IO uint32_t WUTIE:1;
            __IO uint32_t TSIE:1;
            __IO uint32_t ADD1H:1;
            __IO uint32_t SUB1H:1;
            __IO uint32_t BKP:1;
            __IO uint32_t Reserved19:1;
            __IO uint32_t POL:1;
            __IO uint32_t OSEL:2;
            __IO uint32_t COE:1;
            __IO uint32_t Reserved31_24:8;
        }BitCR;
    };

    //__IO uint32_t ISR;     /*!< RTC initialization and status register,                   Address offset: 0x0C */
    union {
        __IO uint32_t ISR;
        struct {
            __IO uint32_t ALRAWF:1;
            __IO uint32_t ALRBWF:1;
            __IO uint32_t WUTWF:1;
            __IO uint32_t Reserved3:1;
            __IO uint32_t INITS:1;
            __IO uint32_t RSF:1;
            __IO uint32_t INITF:1;
            __IO uint32_t INIT:1;
            __IO uint32_t ALRAF:1;
            __IO uint32_t ALRBF:1;
            __IO uint32_t WUTF:1;
            __IO uint32_t TSF:1;
            __IO uint32_t TSOVF:1;
            __IO uint32_t TAMP1F:1;
            __IO uint32_t Reserved31_14:18;
        }BitISR;
    };

    //__IO uint32_t PRER;    /*!< RTC prescaler register,                                   Address offset: 0x10 */
    union {
        __IO uint32_t PRER;
        struct {
            __IO uint32_t PREDIV_S:13;
            __IO uint32_t Reserved15_13:3;
            __IO uint32_t PREDIV_A:7;
            __IO uint32_t Reserved31_23:9;
        }BitPRER;
    };

    __IO uint32_t WUTR;    /*!< RTC wakeup timer register,                                Address offset: 0x14 */
    //__IO uint32_t CALIBR;  /*!< RTC calibration register,                                 Address offset: 0x18 */
    union {
        __IO uint32_t CALIBR;
        struct {
            __IO uint32_t DC:5;
            __IO uint32_t Reserved6_5:2;
            __IO uint32_t DCS:1;
            __IO uint32_t Reserved31_8:24;
        }BitCALIBR;
    };

    //__IO uint32_t ALRMAR;  /*!< RTC alarm A register,                                     Address offset: 0x1C */
    union {
        __IO uint32_t ALRMAR;
        struct {
            __IO uint32_t SU:4;
            __IO uint32_t ST:3;
            __IO uint32_t MSK1:1;
            __IO uint32_t MNU:4;
            __IO uint32_t MNT:3;
            __IO uint32_t MSK2:1;
            __IO uint32_t HU:4;
            __IO uint32_t HT:2;
            __IO uint32_t PM:1;
            __IO uint32_t MSK3:1;
            __IO uint32_t DU:4;
            __IO uint32_t DT:2;
            __IO uint32_t WDSEL:1;
            __IO uint32_t MSK4:1;
        }BitALRMAR;
    };

    //__IO uint32_t ALRMBR;  /*!< RTC alarm B register,                                     Address offset: 0x20 */
    union {
        __IO uint32_t ALRMBR;
        struct {
            __IO uint32_t SU:4;
            __IO uint32_t ST:3;
            __IO uint32_t MSK1:1;
            __IO uint32_t MNU:4;
            __IO uint32_t MNT:3;
            __IO uint32_t MSK2:1;
            __IO uint32_t HU:4;
            __IO uint32_t HT:2;
            __IO uint32_t PM:1;
            __IO uint32_t MSK3:1;
            __IO uint32_t DU:4;
            __IO uint32_t DT:2;
            __IO uint32_t WDSEL:1;
            __IO uint32_t MSK4:1;
        }BitALRMBR;
    };

    __IO uint32_t WPR;     /*!< RTC write protection register,                            Address offset: 0x24 */
    uint32_t RESERVED1;    /*!< Reserved, 0x28                                                                 */
    uint32_t RESERVED2;    /*!< Reserved, 0x2C                                                                 */
    //__IO uint32_t TSTR;    /*!< RTC time stamp time register,                             Address offset: 0x30 */
    union {
        __IO uint32_t TSTR;
        struct {
            __IO uint32_t SU:4;
            __IO uint32_t ST:3;
            __IO uint32_t Reserved7:1;
            __IO uint32_t MNU:4;
            __IO uint32_t MNT:3;
            __IO uint32_t Reserved15:1;
            __IO uint32_t HU:4;
            __IO uint32_t HT:2;
            __IO uint32_t PM:1;
            __IO uint32_t Reserved31_23:9;
        }BitTSTR;
    };

    //__IO uint32_t TSDR;    /*!< RTC time stamp date register,                             Address offset: 0x34 */
    union {
        __IO uint32_t TSDR;
        struct {
            __IO uint32_t DU:4;
            __IO uint32_t DT:2;
            __IO uint32_t Reserved7_6:2;
            __IO uint32_t MU:4;
            __IO uint32_t MT:1;
            __IO uint32_t WDU:3;
            __IO uint32_t Reserved31_16:16;
        }BitTSDR;
    };

    uint32_t RESERVED3;    /*!< Reserved, 0x38                                                                 */
    uint32_t RESERVED4;    /*!< Reserved, 0x3C                                                                 */
    //__IO uint32_t TAFCR;   /*!< RTC tamper and alternate function configuration register, Address offset: 0x40 */
    union {
        __IO uint32_t TAFCR;
        struct {
            __IO uint32_t TAMP1E:1;
            __IO uint32_t TAMP1TRG:1;
            __IO uint32_t TAMPIE:1;
            __IO uint32_t Reserved15_3:13;
            __IO uint32_t TAMP1INSEL:1;
            __IO uint32_t TSINSEL:1;
            __IO uint32_t ALARMOUTTYPE:1;
            __IO uint32_t Reserved31_19:13;
        }BitTAFCR;
    };

    uint32_t RESERVED5;    /*!< Reserved, 0x44                                                                 */
    uint32_t RESERVED6;    /*!< Reserved, 0x48                                                                 */
    uint32_t RESERVED7;    /*!< Reserved, 0x4C                                                                 */
    __IO uint32_t BKP0R;   /*!< RTC backup register 1,                                    Address offset: 0x50 */
    __IO uint32_t BKP1R;   /*!< RTC backup register 1,                                    Address offset: 0x54 */
    __IO uint32_t BKP2R;   /*!< RTC backup register 2,                                    Address offset: 0x58 */
    __IO uint32_t BKP3R;   /*!< RTC backup register 3,                                    Address offset: 0x5C */
    __IO uint32_t BKP4R;   /*!< RTC backup register 4,                                    Address offset: 0x60 */
    __IO uint32_t BKP5R;   /*!< RTC backup register 5,                                    Address offset: 0x64 */
    __IO uint32_t BKP6R;   /*!< RTC backup register 6,                                    Address offset: 0x68 */
    __IO uint32_t BKP7R;   /*!< RTC backup register 7,                                    Address offset: 0x6C */
    __IO uint32_t BKP8R;   /*!< RTC backup register 8,                                    Address offset: 0x70 */
    __IO uint32_t BKP9R;   /*!< RTC backup register 9,                                    Address offset: 0x74 */
    __IO uint32_t BKP10R;  /*!< RTC backup register 10,                                   Address offset: 0x78 */
    __IO uint32_t BKP11R;  /*!< RTC backup register 11,                                   Address offset: 0x7C */
    __IO uint32_t BKP12R;  /*!< RTC backup register 12,                                   Address offset: 0x80 */
    __IO uint32_t BKP13R;  /*!< RTC backup register 13,                                   Address offset: 0x84 */
    __IO uint32_t BKP14R;  /*!< RTC backup register 14,                                   Address offset: 0x88 */
    __IO uint32_t BKP15R;  /*!< RTC backup register 15,                                   Address offset: 0x8C */
    __IO uint32_t BKP16R;  /*!< RTC backup register 16,                                   Address offset: 0x90 */
    __IO uint32_t BKP17R;  /*!< RTC backup register 17,                                   Address offset: 0x94 */
    __IO uint32_t BKP18R;  /*!< RTC backup register 18,                                   Address offset: 0x98 */
    __IO uint32_t BKP19R;  /*!< RTC backup register 19,                                   Address offset: 0x9C */
} RTC_TypeDef;

/**
* @brief SD host Interface
*/

typedef struct
{
    __IO uint32_t POWER;          /*!< SDIO power control register,    Address offset: 0x00 */
    __IO uint32_t CLKCR;          /*!< SDI clock control register,     Address offset: 0x04 */
    __IO uint32_t ARG;            /*!< SDIO argument register,         Address offset: 0x08 */
    __IO uint32_t CMD;            /*!< SDIO command register,          Address offset: 0x0C */
    __I uint32_t  RESPCMD;        /*!< SDIO command response register, Address offset: 0x10 */
    __I uint32_t  RESP1;          /*!< SDIO response 1 register,       Address offset: 0x14 */
    __I uint32_t  RESP2;          /*!< SDIO response 2 register,       Address offset: 0x18 */
    __I uint32_t  RESP3;          /*!< SDIO response 3 register,       Address offset: 0x1C */
    __I uint32_t  RESP4;          /*!< SDIO response 4 register,       Address offset: 0x20 */
    __IO uint32_t DTIMER;         /*!< SDIO data timer register,       Address offset: 0x24 */
    __IO uint32_t DLEN;           /*!< SDIO data length register,      Address offset: 0x28 */
    __IO uint32_t DCTRL;          /*!< SDIO data control register,     Address offset: 0x2C */
    __I uint32_t  DCOUNT;         /*!< SDIO data counter register,     Address offset: 0x30 */
    __I uint32_t  STA;            /*!< SDIO status register,           Address offset: 0x34 */
    __IO uint32_t ICR;            /*!< SDIO interrupt clear register,  Address offset: 0x38 */
    __IO uint32_t MASK;           /*!< SDIO mask register,             Address offset: 0x3C */
    uint32_t      RESERVED0[2];   /*!< Reserved, 0x40-0x44                                  */
    __I uint32_t  FIFOCNT;        /*!< SDIO FIFO counter register,     Address offset: 0x48 */
    uint32_t      RESERVED1[13];  /*!< Reserved, 0x4C-0x7C                                  */
    __IO uint32_t FIFO;           /*!< SDIO data FIFO register,        Address offset: 0x80 */
} SDIO_TypeDef;

/**
* @brief Serial Peripheral Interface
*/

typedef struct
{
    //__IO uint16_t CR1;        /*!< SPI control register 1 (not used in I2S mode),      Address offset: 0x00 */

    union {
        __IO uint16_t CR1;
        struct {
            __IO uint16_t CPHA:1;//                        ((uint16_t)0x0001)            /*!<Clock Phase */
            __IO uint16_t CPOL:1;//                        ((uint16_t)0x0002)            /*!<Clock Polarity */
            __IO uint16_t MSTR:1;//                        ((uint16_t)0x0004)            /*!<Master Selection */
            __IO uint16_t BR:3;//                          ((uint16_t)0x0038)            /*!<BR[2:0] bits (Baud Rate Control) */
            __IO uint16_t SPE:1;//                         ((uint16_t)0x0040)            /*!<SPI Enable */
            __IO uint16_t LSBFIRST:1;//                    ((uint16_t)0x0080)            /*!<Frame Format */
            __IO uint16_t SSI:1;//                         ((uint16_t)0x0100)            /*!<Internal slave select */
            __IO uint16_t SSM:1;//                         ((uint16_t)0x0200)            /*!<Software slave management */
            __IO uint16_t RXONLY:1;//                      ((uint16_t)0x0400)            /*!<Receive only */
            __IO uint16_t DFF:1;//                         ((uint16_t)0x0800)            /*!<Data Frame Format */
            __IO uint16_t CRCNEXT:1;//                     ((uint16_t)0x1000)            /*!<Transmit CRC next */
            __IO uint16_t CRCEN:1;//                       ((uint16_t)0x2000)            /*!<Hardware CRC calculation enable */
            __IO uint16_t BIDIOE:1;//                      ((uint16_t)0x4000)            /*!<Output enable in bidirectional mode */
            __IO uint16_t BIDIMODE:1;//                    ((uint16_t)0x8000)            /*!<Bidirectional data mode enable */
        }BitCR1;
    };

    uint16_t      RESERVED0;  /*!< Reserved, 0x02                                                           */
    //__IO uint16_t CR2;        /*!< SPI control register 2,                             Address offset: 0x04 */

    union {
        __IO uint16_t CR2;
        struct {
            __IO uint16_t RXDMAEN:1;//                     ((uint8_t)0x01)               /*!<Rx Buffer DMA Enable */
            __IO uint16_t TXDMAEN:1;//                     ((uint8_t)0x02)               /*!<Tx Buffer DMA Enable */
            __IO uint16_t SSOE:1;//                        ((uint8_t)0x04)               /*!<SS Output Enable */
            __IO uint16_t Reserved3:1;
            __IO uint16_t FRF:1;
            __IO uint16_t ERRIE:1;//                       ((uint8_t)0x20)               /*!<Error Interrupt Enable */
            __IO uint16_t RXNEIE:1;//                      ((uint8_t)0x40)               /*!<RX buffer Not Empty Interrupt Enable */
            __IO uint16_t TXEIE:1;//                       ((uint8_t)0x80)               /*!<Tx buffer Empty Interrupt Enable */
            __IO uint16_t Reserved15_8:8;
        }BitCR2;
    };

    uint16_t      RESERVED1;  /*!< Reserved, 0x06                                                           */
    //__IO uint16_t SR;         /*!< SPI status register,                                Address offset: 0x08 */

    union {
        __IO uint16_t SR;
        struct {
            __IO uint16_t RXNE:1;//                         ((uint8_t)0x01)               /*!<Receive buffer Not Empty */
            __IO uint16_t TXE:1;//                          ((uint8_t)0x02)               /*!<Transmit buffer Empty */
            __IO uint16_t CHSIDE:1;//                       ((uint8_t)0x04)               /*!<Channel side */
            __IO uint16_t UDR:1;//                          ((uint8_t)0x08)               /*!<Underrun flag */
            __IO uint16_t CRCERR:1;//                       ((uint8_t)0x10)               /*!<CRC Error flag */
            __IO uint16_t MODF:1;//                         ((uint8_t)0x20)               /*!<Mode fault */
            __IO uint16_t OVR:1;//                          ((uint8_t)0x40)               /*!<Overrun flag */
            __IO uint16_t BSY:1;//                          ((uint8_t)0x80)               /*!<Busy flag */
            __IO uint16_t TIFRFE:1;
            __IO uint16_t Reserved15_9:7;
        }BitSR;
    };

    uint16_t      RESERVED2;  /*!< Reserved, 0x0A                                                           */
    __IO uint16_t DR;         /*!< SPI data register,                                  Address offset: 0x0C */
    uint16_t      RESERVED3;  /*!< Reserved, 0x0E                                                           */
    __IO uint16_t CRCPR;      /*!< SPI CRC polynomial register (not used in I2S mode), Address offset: 0x10 */
    uint16_t      RESERVED4;  /*!< Reserved, 0x12                                                           */
    __IO uint16_t RXCRCR;     /*!< SPI RX CRC register (not used in I2S mode),         Address offset: 0x14 */
    uint16_t      RESERVED5;  /*!< Reserved, 0x16                                                           */
    __IO uint16_t TXCRCR;     /*!< SPI TX CRC register (not used in I2S mode),         Address offset: 0x18 */
    uint16_t      RESERVED6;  /*!< Reserved, 0x1A                                                           */
    __IO uint16_t I2SCFGR;    /*!< SPI_I2S configuration register,                     Address offset: 0x1C */
    uint16_t      RESERVED7;  /*!< Reserved, 0x1E                                                           */
    __IO uint16_t I2SPR;      /*!< SPI_I2S prescaler register,                         Address offset: 0x20 */
    uint16_t      RESERVED8;  /*!< Reserved, 0x22                                                           */
} SPI_TypeDef;

/**
* @brief TIM
*/

typedef struct
{
    //__IO uint16_t CR1;         /*!< TIM control register 1,              Address offset: 0x00 */
    union {
        __IO uint16_t CR1;
        struct {
            __IO uint16_t CEN:1;//                         ((uint16_t)0x0001)            /*!<Counter enable */
            __IO uint16_t UDIS:1;//                        ((uint16_t)0x0002)            /*!<Update disable */
            __IO uint16_t URS:1;//                         ((uint16_t)0x0004)            /*!<Update request source */
            __IO uint16_t OPM:1;//                         ((uint16_t)0x0008)            /*!<One pulse mode */
            __IO uint16_t DIR:1;//                         ((uint16_t)0x0010)            /*!<Direction */
            __IO uint16_t CMS:2;//                         ((uint16_t)0x0060)            /*!<CMS[1:0] bits (Center-aligned mode selection) */
            __IO uint16_t ARPE:1;//                        ((uint16_t)0x0080)            /*!<Auto-reload preload enable */
            __IO uint16_t CKD:2;//                         ((uint16_t)0x0300)            /*!<CKD[1:0] bits (clock division) */
            __IO uint16_t Reserved10_15:6;
        }BitCR1;
    };

    uint16_t      RESERVED0;   /*!< Reserved, 0x02                                            */
    //__IO uint16_t CR2;         /*!< TIM control register 2,              Address offset: 0x04 */

    union {
        __IO uint16_t CR2;
        struct {
            __IO uint16_t CCPC:1;//                        ((uint16_t)0x0001)            /*!<Capture/Compare Preloaded Control */
            __IO uint16_t Reserved1:1;
            __IO uint16_t CCUS:1;//                        ((uint16_t)0x0004)            /*!<Capture/Compare Control Update Selection */
            __IO uint16_t CCDS:1;//                        ((uint16_t)0x0008)            /*!<Capture/Compare DMA Selection */
            __IO uint16_t MMS:3;//                         ((uint16_t)0x0070)            /*!<MMS[2:0] bits (Master Mode Selection) */
            __IO uint16_t TI1S:1;//                        ((uint16_t)0x0080)            /*!<TI1 Selection */
            __IO uint16_t OIS1:1;//                        ((uint16_t)0x0100)            /*!<Output Idle state 1 (OC1 output) */
            __IO uint16_t OIS1N:1;//                       ((uint16_t)0x0200)            /*!<Output Idle state 1 (OC1N output) */
            __IO uint16_t OIS2:1;//                        ((uint16_t)0x0400)            /*!<Output Idle state 2 (OC2 output) */
            __IO uint16_t OIS2N:1;//                       ((uint16_t)0x0800)            /*!<Output Idle state 2 (OC2N output) */
            __IO uint16_t OIS3:1;//                        ((uint16_t)0x1000)            /*!<Output Idle state 3 (OC3 output) */
            __IO uint16_t OIS3N:1;//                       ((uint16_t)0x2000)            /*!<Output Idle state 3 (OC3N output) */
            __IO uint16_t OIS4:1;//                        ((uint16_t)0x4000)            /*!<Output Idle state 4 (OC4 output) */
            __IO uint16_t Reserved15:1;
        }BitCR2;
    };

    uint16_t      RESERVED1;   /*!< Reserved, 0x06                                            */
    //__IO uint16_t SMCR;        /*!< TIM slave mode control register,     Address offset: 0x08 */
    union {
        __IO uint16_t SMCR;
        struct {
            __IO uint16_t SMS:3;//                        ((uint16_t)0x0007)            /*!<SMS[2:0] bits (Slave mode selection) */
            __IO uint16_t Reserved3:1;
            __IO uint16_t TS:3;//                         ((uint16_t)0x0070)            /*!<TS[2:0] bits (Trigger selection) */
            __IO uint16_t MSM:1;//                        ((uint16_t)0x0080)            /*!<Master/slave mode */
            __IO uint16_t ETF:4;//                        ((uint16_t)0x0F00)            /*!<ETF[3:0] bits (External trigger filter) */
            __IO uint16_t ETPS:2;//                       ((uint16_t)0x3000)            /*!<ETPS[1:0] bits (External trigger prescaler) */
            __IO uint16_t ECE:1;//                        ((uint16_t)0x4000)            /*!<External clock enable */
            __IO uint16_t ETP:1;//                        ((uint16_t)0x8000)            /*!<External trigger polarity */
        }BitSMCR;
    };

    uint16_t      RESERVED2;   /*!< Reserved, 0x0A                                            */
    //__IO uint16_t DIER;        /*!< TIM DMA/interrupt enable register,   Address offset: 0x0C */
    union {
        __IO uint16_t DIER;
        struct {
            __IO uint16_t UIE:1;//                        ((uint16_t)0x0001)            /*!<Update interrupt enable */
            __IO uint16_t CC1IE:1;//                      ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt enable */
            __IO uint16_t CC2IE:1;//                      ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt enable */
            __IO uint16_t CC3IE:1;//                      ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt enable */
            __IO uint16_t CC4IE:1;//                      ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt enable */
            __IO uint16_t COMIE:1;//                      ((uint16_t)0x0020)            /*!<COM interrupt enable */
            __IO uint16_t TIE:1;//                        ((uint16_t)0x0040)            /*!<Trigger interrupt enable */
            __IO uint16_t BIE:1;//                        ((uint16_t)0x0080)            /*!<Break interrupt enable */
            __IO uint16_t UDE:1;//                        ((uint16_t)0x0100)            /*!<Update DMA request enable */
            __IO uint16_t CC1DE:1;//                      ((uint16_t)0x0200)            /*!<Capture/Compare 1 DMA request enable */
            __IO uint16_t CC2DE:1;//                      ((uint16_t)0x0400)            /*!<Capture/Compare 2 DMA request enable */
            __IO uint16_t CC3DE:1;//                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 DMA request enable */
            __IO uint16_t CC4DE:1;//                      ((uint16_t)0x1000)            /*!<Capture/Compare 4 DMA request enable */
            __IO uint16_t COMDE:1;//                      ((uint16_t)0x2000)            /*!<COM DMA request enable */
            __IO uint16_t TDE:1;//                        ((uint16_t)0x4000)            /*!<Trigger DMA request enable */
            __IO uint16_t Reserved15:1;
        }BitDIER;
    };

    uint16_t      RESERVED3;   /*!< Reserved, 0x0E                                            */
    //__IO uint16_t SR;          /*!< TIM status register,                 Address offset: 0x10 */
    union {
        __IO uint16_t SR;
        struct {
            __IO uint16_t UIF:1;//                          ((uint16_t)0x0001)            /*!<Update interrupt Flag */
            __IO uint16_t CC1IF:1;//                        ((uint16_t)0x0002)            /*!<Capture/Compare 1 interrupt Flag */
            __IO uint16_t CC2IF:1;//                        ((uint16_t)0x0004)            /*!<Capture/Compare 2 interrupt Flag */
            __IO uint16_t CC3IF:1;//                        ((uint16_t)0x0008)            /*!<Capture/Compare 3 interrupt Flag */
            __IO uint16_t CC4IF:1;//                        ((uint16_t)0x0010)            /*!<Capture/Compare 4 interrupt Flag */
            __IO uint16_t COMIF:1;//                        ((uint16_t)0x0020)            /*!<COM interrupt Flag */
            __IO uint16_t TIF:1;//                          ((uint16_t)0x0040)            /*!<Trigger interrupt Flag */
            __IO uint16_t BIF:1;//                          ((uint16_t)0x0080)            /*!<Break interrupt Flag */
            __IO uint16_t Reserved8:1;
            __IO uint16_t CC1OF:1;//                        ((uint16_t)0x0200)            /*!<Capture/Compare 1 Overcapture Flag */
            __IO uint16_t CC2OF:1;//                        ((uint16_t)0x0400)            /*!<Capture/Compare 2 Overcapture Flag */
            __IO uint16_t CC3OF:1;//                        ((uint16_t)0x0800)            /*!<Capture/Compare 3 Overcapture Flag */
            __IO uint16_t CC4OF:1;//                        ((uint16_t)0x1000)            /*!<Capture/Compare 4 Overcapture Flag */
            __IO uint16_t Reserved13_15:3;
        }BitSR;
    };

    uint16_t      RESERVED4;   /*!< Reserved, 0x12                                            */
    //__IO uint16_t EGR;         /*!< TIM event generation register,       Address offset: 0x14 */
    union {
        __IO uint16_t EGR;
        struct {
            __IO uint16_t UG:1;//                          ((uint8_t)0x01)               /*!<Update Generation */
            __IO uint16_t CC1G:1;//                        ((uint8_t)0x02)               /*!<Capture/Compare 1 Generation */
            __IO uint16_t CC2G:1;//                        ((uint8_t)0x04)               /*!<Capture/Compare 2 Generation */
            __IO uint16_t CC3G:1;//                        ((uint8_t)0x08)               /*!<Capture/Compare 3 Generation */
            __IO uint16_t CC4G:1;//                        ((uint8_t)0x10)               /*!<Capture/Compare 4 Generation */
            __IO uint16_t COMG:1;//                        ((uint8_t)0x20)               /*!<Capture/Compare Control Update Generation */
            __IO uint16_t TG:1;//                          ((uint8_t)0x40)               /*!<Trigger Generation */
            __IO uint16_t BG:1;//                          ((uint8_t)0x80)               /*!<Break Generation */
            __IO uint16_t Reserved8_15:8;
        }BitEGR;
    };

    uint16_t      RESERVED5;   /*!< Reserved, 0x16                                            */
    //__IO uint16_t CCMR1;       /*!< TIM capture/compare mode register 1, Address offset: 0x18 */
    union {
        __IO uint16_t CCMR1;
        struct {
            __IO uint16_t OC1S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t OC1FE:1;//                     ((uint16_t)0x0004)            /*!<Output Compare 1 Fast enable */
            __IO uint16_t OC1PE:1;//                     ((uint16_t)0x0008)            /*!<Output Compare 1 Preload enable */
            __IO uint16_t OC1M:3;//                      ((uint16_t)0x0070)            /*!<OC1M[2:0] bits (Output Compare 1 Mode) */
            __IO uint16_t OC1CE:1;//                     ((uint16_t)0x0080)            /*!<Output Compare 1Clear Enable */
            __IO uint16_t OC2S:2;//                      ((uint16_t)0x0300)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
            __IO uint16_t OC2FE:1;//                     ((uint16_t)0x0400)            /*!<Output Compare 2 Fast enable */
            __IO uint16_t OC2PE:1;//                     ((uint16_t)0x0800)            /*!<Output Compare 2 Preload enable */
            __IO uint16_t OC2M:3;//                      ((uint16_t)0x7000)            /*!<OC2M[2:0] bits (Output Compare 2 Mode) */
            __IO uint16_t OC2CE:1;//                     ((uint16_t)0x8000)            /*!<Output Compare 2 Clear Enable */
        }BitOcCCMR1;
        struct {
            __IO uint16_t IC1S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC1PSC:2;//                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
            __IO uint16_t IC1F:4;//                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
            __IO uint16_t IC2S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC2PSC:1;//                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
            __IO uint16_t IC2F:4;//                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
        }BitIcCCMR1;
    };

    uint16_t      RESERVED6;   /*!< Reserved, 0x1A                                            */
    //__IO uint16_t CCMR2;       /*!< TIM capture/compare mode register 2, Address offset: 0x1C */
    union {
        __IO uint16_t CCMR2;
        struct {
            __IO uint16_t OC3S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t OC3FE:1;//                     ((uint16_t)0x0004)            /*!<Output Compare 1 Fast enable */
            __IO uint16_t OC3PE:1;//                     ((uint16_t)0x0008)            /*!<Output Compare 1 Preload enable */
            __IO uint16_t OC3M:3;//                      ((uint16_t)0x0070)            /*!<OC1M[2:0] bits (Output Compare 1 Mode) */
            __IO uint16_t OC3CE:1;//                     ((uint16_t)0x0080)            /*!<Output Compare 1Clear Enable */
            __IO uint16_t OC4S:2;//                      ((uint16_t)0x0300)            /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
            __IO uint16_t OC4FE:1;//                     ((uint16_t)0x0400)            /*!<Output Compare 2 Fast enable */
            __IO uint16_t OC4PE:1;//                     ((uint16_t)0x0800)            /*!<Output Compare 2 Preload enable */
            __IO uint16_t OC4M:3;//                      ((uint16_t)0x7000)            /*!<OC2M[2:0] bits (Output Compare 2 Mode) */
            __IO uint16_t OC4CE:1;//                     ((uint16_t)0x8000)            /*!<Output Compare 2 Clear Enable */
        }BitOcCCMR2;
        struct {
            __IO uint16_t IC3S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC3PSC:2;//                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
            __IO uint16_t IC3F:4;//                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
            __IO uint16_t IC4S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC4PSC:1;//                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
            __IO uint16_t IC4F:4;//                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
        }BitIcCCMR2;
    };

    uint16_t      RESERVED7;   /*!< Reserved, 0x1E                                            */
    //__IO uint16_t CCER;        /*!< TIM capture/compare enable register, Address offset: 0x20 */
    union {
        __IO uint16_t CCER;
        struct {
            __IO uint16_t CC1E:1;//                       ((uint16_t)0x0001)            /*!<Capture/Compare 1 output enable */
            __IO uint16_t CC1P:1;//                       ((uint16_t)0x0002)            /*!<Capture/Compare 1 output Polarity */
            __IO uint16_t CC1NE:1;//                      ((uint16_t)0x0004)            /*!<Capture/Compare 1 Complementary output enable */
            __IO uint16_t CC1NP:1;//                      ((uint16_t)0x0008)            /*!<Capture/Compare 1 Complementary output Polarity */
            __IO uint16_t CC2E:1;//                       ((uint16_t)0x0010)            /*!<Capture/Compare 2 output enable */
            __IO uint16_t CC2P:1;//                       ((uint16_t)0x0020)            /*!<Capture/Compare 2 output Polarity */
            __IO uint16_t CC2NE:1;//                      ((uint16_t)0x0040)            /*!<Capture/Compare 2 Complementary output enable */
            __IO uint16_t CC2NP:1;//                      ((uint16_t)0x0080)            /*!<Capture/Compare 2 Complementary output Polarity */
            __IO uint16_t CC3E:1;//                       ((uint16_t)0x0100)            /*!<Capture/Compare 3 output enable */
            __IO uint16_t CC3P:1;//                       ((uint16_t)0x0200)            /*!<Capture/Compare 3 output Polarity */
            __IO uint16_t CC3NE:1;//                      ((uint16_t)0x0400)            /*!<Capture/Compare 3 Complementary output enable */
            __IO uint16_t CC3NP:1;//                      ((uint16_t)0x0800)            /*!<Capture/Compare 3 Complementary output Polarity */
            __IO uint16_t CC4E:1;//                       ((uint16_t)0x1000)            /*!<Capture/Compare 4 output enable */
            __IO uint16_t CC4P:1;//                       ((uint16_t)0x2000)            /*!<Capture/Compare 4 output Polarity */
            __IO uint16_t Reserved14_15:2;
        }BitCCER;
    };

    uint16_t      RESERVED8;   /*!< Reserved, 0x22                                            */
    __IO uint32_t CNT;         /*!< TIM counter register,                Address offset: 0x24 */
    __IO uint16_t PSC;         /*!< TIM prescaler,                       Address offset: 0x28 */
    uint16_t      RESERVED9;   /*!< Reserved, 0x2A                                            */
    __IO uint32_t ARR;         /*!< TIM auto-reload register,            Address offset: 0x2C */
    __IO uint16_t RCR;         /*!< TIM repetition counter register,     Address offset: 0x30 */
    uint16_t      RESERVED10;  /*!< Reserved, 0x32                                            */
    __IO uint32_t CCR1;        /*!< TIM capture/compare register 1,      Address offset: 0x34 */
    __IO uint32_t CCR2;        /*!< TIM capture/compare register 2,      Address offset: 0x38 */
    __IO uint32_t CCR3;        /*!< TIM capture/compare register 3,      Address offset: 0x3C */
    __IO uint32_t CCR4;        /*!< TIM capture/compare register 4,      Address offset: 0x40 */
    //__IO uint16_t BDTR;        /*!< TIM break and dead-time register,    Address offset: 0x44 */
    union {
        __IO uint16_t BDTR;
        struct {
            __IO uint16_t DTG:8;//                        ((uint16_t)0x00FF)            /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
            __IO uint16_t LOCK:2;//                       ((uint16_t)0x0300)            /*!<LOCK[1:0] bits (Lock Configuration) */
            __IO uint16_t OSSI:1;//                       ((uint16_t)0x0400)            /*!<Off-State Selection for Idle mode */
            __IO uint16_t OSSR:1;//                       ((uint16_t)0x0800)            /*!<Off-State Selection for Run mode */
            __IO uint16_t BKE:1;//                        ((uint16_t)0x1000)            /*!<Break enable */
            __IO uint16_t BKP:1;//                        ((uint16_t)0x2000)            /*!<Break Polarity */
            __IO uint16_t AOE:1;//                        ((uint16_t)0x4000)            /*!<Automatic Output enable */
            __IO uint16_t MOE:1;//                        ((uint16_t)0x8000)            /*!<Main Output enable */
        }BitBDTR;
    };

    uint16_t      RESERVED11;  /*!< Reserved, 0x46                                            */
    //__IO uint16_t DCR;         /*!< TIM DMA control register,            Address offset: 0x48 */
    union {
        __IO uint16_t DCR;
        struct {
            __IO uint16_t DBA:5;//                         ((uint16_t)0x001F)            /*!<DBA[4:0] bits (DMA Base Address) */
            __IO uint16_t Reserved5_7:3;
            __IO uint16_t DBL:5;//                         ((uint16_t)0x1F00)            /*!<DBL[4:0] bits (DMA Burst Length) */
            __IO uint16_t Reserved13_15:3;
        }BitDCR;
    };

    uint16_t      RESERVED12;  /*!< Reserved, 0x4A                                            */
    __IO uint16_t DMAR;        /*!< TIM DMA address for full transfer,   Address offset: 0x4C */
    uint16_t      RESERVED13;  /*!< Reserved, 0x4E                                            */
    __IO uint16_t OR;          /*!< TIM option register,                 Address offset: 0x50 */
    uint16_t      RESERVED14;  /*!< Reserved, 0x52                                            */
} TIM_TypeDef;

/**
* @brief Universal Synchronous Asynchronous Receiver Transmitter
*/

typedef struct
{
    union {
        __IO uint16_t SR;
        struct {
            __IO uint16_t PE:1;//                         ((uint16_t)0x0001)            /*!<Parity Error */
            __IO uint16_t FE:1;//                         ((uint16_t)0x0002)            /*!<Framing Error */
            __IO uint16_t NE:1;//                         ((uint16_t)0x0004)            /*!<Noise Error Flag */
            __IO uint16_t ORE:1;//                        ((uint16_t)0x0008)            /*!<OverRun Error */
            __IO uint16_t IDLE:1;//                       ((uint16_t)0x0010)            /*!<IDLE line detected */
            __IO uint16_t RXNE:1;//                       ((uint16_t)0x0020)            /*!<Read Data Register Not Empty */
            __IO uint16_t TC:1;//                         ((uint16_t)0x0040)            /*!<Transmission Complete */
            __IO uint16_t TXE:1;//                        ((uint16_t)0x0080)            /*!<Transmit Data Register Empty */
            __IO uint16_t LBD:1;//                        ((uint16_t)0x0100)            /*!<LIN Break Detection Flag */
            __IO uint16_t CTS:1;//                        ((uint16_t)0x0200)            /*!<CTS Flag */
            __IO uint16_t Reserved15_10:6;
        }BitSR;
    };

    uint16_t      RESERVED0;  /*!< Reserved, 0x02                                                */
    __IO uint16_t DR;         /*!< USART Data register,                     Address offset: 0x04 */
    uint16_t      RESERVED1;  /*!< Reserved, 0x06                                                */
    __IO uint16_t BRR;        /*!< USART Baud rate register,                Address offset: 0x08 */
    uint16_t      RESERVED2;  /*!< Reserved, 0x0A                                                */

    union {
        __IO uint16_t CR1;
        struct {
            __IO uint16_t SBK:1;//                       ((uint16_t)0x0001)            /*!<Send Break */
            __IO uint16_t RWU:1;//                       ((uint16_t)0x0002)            /*!<Receiver wakeup */
            __IO uint16_t RE:1;//                        ((uint16_t)0x0004)            /*!<Receiver Enable */
            __IO uint16_t TE:1;//                        ((uint16_t)0x0008)            /*!<Transmitter Enable */
            __IO uint16_t IDLEIE:1;//                    ((uint16_t)0x0010)            /*!<IDLE Interrupt Enable */
            __IO uint16_t RXNEIE:1;//                    ((uint16_t)0x0020)            /*!<RXNE Interrupt Enable */
            __IO uint16_t TCIE:1;//                      ((uint16_t)0x0040)            /*!<Transmission Complete Interrupt Enable */
            __IO uint16_t TXEIE:1;//                     ((uint16_t)0x0080)            /*!<PE Interrupt Enable */
            __IO uint16_t PEIE:1;//                      ((uint16_t)0x0100)            /*!<PE Interrupt Enable */
            __IO uint16_t PS:1;//                        ((uint16_t)0x0200)            /*!<Parity Selection */
            __IO uint16_t PCE:1;//                       ((uint16_t)0x0400)            /*!<Parity Control Enable */
            __IO uint16_t WAKE:1;//                      ((uint16_t)0x0800)            /*!<Wakeup method */
            __IO uint16_t M:1;//                         ((uint16_t)0x1000)            /*!<Word length */
            __IO uint16_t UE:1;//                        ((uint16_t)0x2000)            /*!<USART Enable */
            __IO uint16_t Reserved15_14:2;
        }BitCR1;
    };

    uint16_t      RESERVED3;  /*!< Reserved, 0x0E                                                */
    //__IO uint16_t CR2;        /*!< USART Control register 2,                Address offset: 0x10 */

    union {
        __IO uint16_t CR2;
        struct {
            __IO uint16_t ADD:4;//                       ((uint16_t)0x000F)            /*!<Address of the USART node */
            __IO uint16_t Reserved4:1;
            __IO uint16_t LBDL:1;//                      ((uint16_t)0x0020)            /*!<LIN Break Detection Length */
            __IO uint16_t LBDIE:1;//                     ((uint16_t)0x0040)            /*!<LIN Break Detection Interrupt Enable */
            __IO uint16_t Reserved7:1;
            __IO uint16_t LBCL:1;//                      ((uint16_t)0x0100)            /*!<Last Bit Clock pulse */
            __IO uint16_t CPHA:1;//                      ((uint16_t)0x0200)            /*!<Clock Phase */
            __IO uint16_t CPOL:1;//                      ((uint16_t)0x0400)            /*!<Clock Polarity */
            __IO uint16_t CLKEN:1;//                     ((uint16_t)0x0800)            /*!<Clock Enable */
            __IO uint16_t STOP:2;//                      ((uint16_t)0x3000)            /*!<STOP[1:0] bits (STOP bits) */
            __IO uint16_t LINEN:1;//                     ((uint16_t)0x4000)            /*!<LIN mode enable */
            __IO uint16_t Reserved15:1;
        }BitCR2;
    };

    uint16_t      RESERVED4;  /*!< Reserved, 0x12                                                */
    //__IO uint16_t CR3;        /*!< USART Control register 3,                Address offset: 0x14 */

    union {
        __IO uint16_t CR3;
        struct {
            __IO uint16_t EIE:1;//                       ((uint16_t)0x0001)            /*!<Error Interrupt Enable */
            __IO uint16_t IREN:1;//                      ((uint16_t)0x0002)            /*!<IrDA mode Enable */
            __IO uint16_t IRLP:1;//                      ((uint16_t)0x0004)            /*!<IrDA Low-Power */
            __IO uint16_t HDSEL:1;//                     ((uint16_t)0x0008)            /*!<Half-Duplex Selection */
            __IO uint16_t NACK:1;//                      ((uint16_t)0x0010)            /*!<Smartcard NACK enable */
            __IO uint16_t SCEN:1;//                      ((uint16_t)0x0020)            /*!<Smartcard mode enable */
            __IO uint16_t DMAR:1;//                      ((uint16_t)0x0040)            /*!<DMA Enable Receiver */
            __IO uint16_t DMAT:1;//                      ((uint16_t)0x0080)            /*!<DMA Enable Transmitter */
            __IO uint16_t RTSE:1;//                      ((uint16_t)0x0100)            /*!<RTS Enable */
            __IO uint16_t CTSE:1;//                      ((uint16_t)0x0200)            /*!<CTS Enable */
            __IO uint16_t CTSIE:1;//                     ((uint16_t)0x0400)            /*!<CTS Interrupt Enable */
            __IO uint16_t ONEBIT:1;//                     ((uint16_t)0x0400)            /*!<CTS Interrupt Enable */
            __IO uint16_t Reserved15_12:4;
        }BitCR3;
    };

    uint16_t      RESERVED5;  /*!< Reserved, 0x16                                                */
    //__IO uint16_t GTPR;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */

    union {
        __IO uint16_t GTPR;
        struct {
            __IO uint16_t PSC:8;//                      ((uint16_t)0x00FF)            /*!<PSC[7:0] bits (Prescaler value) */
            __IO uint16_t GT:8;//                       ((uint16_t)0xFF00)            /*!<Guard time value */
        }BitGTPR;
    };

    uint16_t      RESERVED6;  /*!< Reserved, 0x1A                                                */
} USART_TypeDef;

/**
* @brief Window WATCHDOG
*/

typedef struct
{
    __IO uint32_t CR;   /*!< WWDG Control register,       Address offset: 0x00 */
    __IO uint32_t CFR;  /*!< WWDG Configuration register, Address offset: 0x04 */
    __IO uint32_t SR;   /*!< WWDG Status register,        Address offset: 0x08 */
} WWDG_TypeDef;

/**
* @brief Crypto Processor
*/

typedef struct
{
    __IO uint32_t CR;     /*!< CRYP control register,                            Address offset: 0x00 */
    __IO uint32_t SR;     /*!< CRYP status register,                             Address offset: 0x04 */
    __IO uint32_t DR;     /*!< CRYP data input register,                         Address offset: 0x08 */
    __IO uint32_t DOUT;   /*!< CRYP data output register,                        Address offset: 0x0C */
    __IO uint32_t DMACR;  /*!< CRYP DMA control register,                        Address offset: 0x10 */
    __IO uint32_t IMSCR;  /*!< CRYP interrupt mask set/clear register,           Address offset: 0x14 */
    __IO uint32_t RISR;   /*!< CRYP raw interrupt status register,               Address offset: 0x18 */
    __IO uint32_t MISR;   /*!< CRYP masked interrupt status register,            Address offset: 0x1C */
    __IO uint32_t K0LR;   /*!< CRYP key left  register 0,                        Address offset: 0x20 */
    __IO uint32_t K0RR;   /*!< CRYP key right register 0,                        Address offset: 0x24 */
    __IO uint32_t K1LR;   /*!< CRYP key left  register 1,                        Address offset: 0x28 */
    __IO uint32_t K1RR;   /*!< CRYP key right register 1,                        Address offset: 0x2C */
    __IO uint32_t K2LR;   /*!< CRYP key left  register 2,                        Address offset: 0x30 */
    __IO uint32_t K2RR;   /*!< CRYP key right register 2,                        Address offset: 0x34 */
    __IO uint32_t K3LR;   /*!< CRYP key left  register 3,                        Address offset: 0x38 */
    __IO uint32_t K3RR;   /*!< CRYP key right register 3,                        Address offset: 0x3C */
    __IO uint32_t IV0LR;  /*!< CRYP initialization vector left-word  register 0, Address offset: 0x40 */
    __IO uint32_t IV0RR;  /*!< CRYP initialization vector right-word register 0, Address offset: 0x44 */
    __IO uint32_t IV1LR;  /*!< CRYP initialization vector left-word  register 1, Address offset: 0x48 */
    __IO uint32_t IV1RR;  /*!< CRYP initialization vector right-word register 1, Address offset: 0x4C */
} CRYP_TypeDef;

/**
* @brief HASH
*/

typedef struct
{
    __IO uint32_t CR;        /*!< HASH control register,          Address offset: 0x00        */
    __IO uint32_t DIN;       /*!< HASH data input register,       Address offset: 0x04        */
    __IO uint32_t STR;       /*!< HASH start register,            Address offset: 0x08        */
    __IO uint32_t HR[5];     /*!< HASH digest registers,          Address offset: 0x0C-0x1C   */
    __IO uint32_t IMR;       /*!< HASH interrupt enable register, Address offset: 0x20        */
    __IO uint32_t SR;        /*!< HASH status register,           Address offset: 0x24        */
    uint32_t  RESERVED[52];  /*!< Reserved, 0x28-0xF4                                         */
    __IO uint32_t CSR[51];   /*!< HASH context swap registers,    Address offset: 0x0F8-0x1C0 */
} HASH_TypeDef;

/**
* @brief HASH
*/

typedef struct
{
    __IO uint32_t CR;  /*!< RNG control register, Address offset: 0x00 */
    __IO uint32_t SR;  /*!< RNG status register,  Address offset: 0x04 */
    __IO uint32_t DR;  /*!< RNG data register,    Address offset: 0x08 */
} RNG_TypeDef;

/**
* @}
*/

/** @addtogroup Peripheral_memory_map
* @{
*/

#define FLASH_BASE            ((uint32_t)0x08000000) /*!< FLASH base address in the alias region */
#define SRAM_BASE             ((uint32_t)0x20000000) /*!< SRAM base address in the alias region */
#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */

#define SRAM_BB_BASE          ((uint32_t)0x22000000) /*!< SRAM base address in the bit-band region */
#define PERIPH_BB_BASE        ((uint32_t)0x42000000) /*!< Peripheral base address in the bit-band region */

#define FSMC_R_BASE           ((uint32_t)0xA0000000) /*!< FSMC registers base address */

/*!< Peripheral memory map */
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000)

/*!< APB1 peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800)
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00)
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800)
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)

/*!< APB2 peripherals */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400)
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000)
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200)
#define ADC_BASE              (APB2PERIPH_BASE + 0x2300)
#define SDIO_BASE             (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800)

/*!< AHB1 peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00)
#define BKPSRAM_BASE          (AHB1PERIPH_BASE + 0x4000)
#define DMA1_BASE             (AHB1PERIPH_BASE + 0x6000)
#define DMA1_Stream0_BASE     (DMA1_BASE + 0x010)
#define DMA1_Stream1_BASE     (DMA1_BASE + 0x028)
#define DMA1_Stream2_BASE     (DMA1_BASE + 0x040)
#define DMA1_Stream3_BASE     (DMA1_BASE + 0x058)
#define DMA1_Stream4_BASE     (DMA1_BASE + 0x070)
#define DMA1_Stream5_BASE     (DMA1_BASE + 0x088)
#define DMA1_Stream6_BASE     (DMA1_BASE + 0x0A0)
#define DMA1_Stream7_BASE     (DMA1_BASE + 0x0B8)
#define DMA2_BASE             (AHB1PERIPH_BASE + 0x6400)
#define DMA2_Stream0_BASE     (DMA2_BASE + 0x010)
#define DMA2_Stream1_BASE     (DMA2_BASE + 0x028)
#define DMA2_Stream2_BASE     (DMA2_BASE + 0x040)
#define DMA2_Stream3_BASE     (DMA2_BASE + 0x058)
#define DMA2_Stream4_BASE     (DMA2_BASE + 0x070)
#define DMA2_Stream5_BASE     (DMA2_BASE + 0x088)
#define DMA2_Stream6_BASE     (DMA2_BASE + 0x0A0)
#define DMA2_Stream7_BASE     (DMA2_BASE + 0x0B8)
#define ETH_BASE              (AHB1PERIPH_BASE + 0x8000)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000)

/*!< AHB2 peripherals */
#define DCMI_BASE             (AHB2PERIPH_BASE + 0x50000)
#define CRYP_BASE             (AHB2PERIPH_BASE + 0x60000)
#define HASH_BASE             (AHB2PERIPH_BASE + 0x60400)
#define RNG_BASE              (AHB2PERIPH_BASE + 0x60800)

/*!< FSMC Bankx registers base address */
#define FSMC_Bank1_R_BASE     (FSMC_R_BASE + 0x0000)
#define FSMC_Bank1E_R_BASE    (FSMC_R_BASE + 0x0104)
#define FSMC_Bank2_R_BASE     (FSMC_R_BASE + 0x0060)
#define FSMC_Bank3_R_BASE     (FSMC_R_BASE + 0x0080)
#define FSMC_Bank4_R_BASE     (FSMC_R_BASE + 0x00A0)

/* Debug MCU registers base address */
#define DBGMCU_BASE           ((uint32_t )0xE0042000)

/**
* @}
*/

/** @addtogroup Peripheral_declaration
* @{
*/
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)
#define TIM12               ((TIM_TypeDef *) TIM12_BASE)
#define TIM13               ((TIM_TypeDef *) TIM13_BASE)
#define TIM14               ((TIM_TypeDef *) TIM14_BASE)
#define RTC                 ((RTC_TypeDef *) RTC_BASE)
#define WWDG                ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define USART3              ((USART_TypeDef *) USART3_BASE)
#define UART4               ((USART_TypeDef *) UART4_BASE)
#define UART5               ((USART_TypeDef *) UART5_BASE)
#define I2C1                ((I2C_TypeDef *) I2C1_BASE)
#define I2C2                ((I2C_TypeDef *) I2C2_BASE)
#define I2C3                ((I2C_TypeDef *) I2C3_BASE)
#define CAN1                ((CAN_TypeDef *) CAN1_BASE)
#define CAN2                ((CAN_TypeDef *) CAN2_BASE)
#define PWR                 ((PWR_TypeDef *) PWR_BASE)
#define DAC                 ((DAC_TypeDef *) DAC_BASE)
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define USART6              ((USART_TypeDef *) USART6_BASE)
#define ADC                 ((ADC_Common_TypeDef *) ADC_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)
#define ADC2                ((ADC_TypeDef *) ADC2_BASE)
#define ADC3                ((ADC_TypeDef *) ADC3_BASE)
#define SDIO                ((SDIO_TypeDef *) SDIO_BASE)
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define TIM9                ((TIM_TypeDef *) TIM9_BASE)
#define TIM10               ((TIM_TypeDef *) TIM10_BASE)
#define TIM11               ((TIM_TypeDef *) TIM11_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)
#define GPIOI               ((GPIO_TypeDef *) GPIOI_BASE)
#define CRC                 ((CRC_TypeDef *) CRC_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define DMA1                ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_Stream0        ((DMA_Stream_TypeDef *) DMA1_Stream0_BASE)
#define DMA1_Stream1        ((DMA_Stream_TypeDef *) DMA1_Stream1_BASE)
#define DMA1_Stream2        ((DMA_Stream_TypeDef *) DMA1_Stream2_BASE)
#define DMA1_Stream3        ((DMA_Stream_TypeDef *) DMA1_Stream3_BASE)
#define DMA1_Stream4        ((DMA_Stream_TypeDef *) DMA1_Stream4_BASE)
#define DMA1_Stream5        ((DMA_Stream_TypeDef *) DMA1_Stream5_BASE)
#define DMA1_Stream6        ((DMA_Stream_TypeDef *) DMA1_Stream6_BASE)
#define DMA1_Stream7        ((DMA_Stream_TypeDef *) DMA1_Stream7_BASE)
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)
#define DMA2_Stream0        ((DMA_Stream_TypeDef *) DMA2_Stream0_BASE)
#define DMA2_Stream1        ((DMA_Stream_TypeDef *) DMA2_Stream1_BASE)
#define DMA2_Stream2        ((DMA_Stream_TypeDef *) DMA2_Stream2_BASE)
#define DMA2_Stream3        ((DMA_Stream_TypeDef *) DMA2_Stream3_BASE)
#define DMA2_Stream4        ((DMA_Stream_TypeDef *) DMA2_Stream4_BASE)
#define DMA2_Stream5        ((DMA_Stream_TypeDef *) DMA2_Stream5_BASE)
#define DMA2_Stream6        ((DMA_Stream_TypeDef *) DMA2_Stream6_BASE)
#define DMA2_Stream7        ((DMA_Stream_TypeDef *) DMA2_Stream7_BASE)
#define ETH                 ((ETH_TypeDef *) ETH_BASE)
#define DCMI                ((DCMI_TypeDef *) DCMI_BASE)
#define CRYP                ((CRYP_TypeDef *) CRYP_BASE)
#define HASH                ((HASH_TypeDef *) HASH_BASE)
#define RNG                 ((RNG_TypeDef *) RNG_BASE)
#define FSMC_Bank1          ((FSMC_Bank1_TypeDef *) FSMC_Bank1_R_BASE)
#define FSMC_Bank1E         ((FSMC_Bank1E_TypeDef *) FSMC_Bank1E_R_BASE)
#define FSMC_Bank2          ((FSMC_Bank2_TypeDef *) FSMC_Bank2_R_BASE)
#define FSMC_Bank3          ((FSMC_Bank3_TypeDef *) FSMC_Bank3_R_BASE)
#define FSMC_Bank4          ((FSMC_Bank4_TypeDef *) FSMC_Bank4_R_BASE)
#define DBGMCU              ((DBGMCU_TypeDef *) DBGMCU_BASE)

/** @addtogroup Exported_macro
* @{
*/

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/**
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STM32F2xx_H */

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
