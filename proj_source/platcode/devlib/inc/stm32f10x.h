/**
******************************************************************************
* @file stm32f10x.h
* @brief CMSIS Cortex-M3 Device Peripheral Access Layer Header File.
*        This file contains all the peripheral register's definitions, bits
*        definitions and memory mapping for STM32F10x High Density, Medium
*        Density and Low Density devices.
* @author STMicroelectronics - MCD Application Team
* @version V3.0.0
* @date 04/06/2009
******************************************************************************
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
******************************************************************************
*/

/** @addtogroup CMSIS
* @{
*/

/** @addtogroup stm32f10x
* @{
*/

#ifndef __STM32F10x_H
#define __STM32F10x_H

/** @addtogroup Library_configuration_section
* @{
*/

/* Uncomment the line below according to the target STM32 device used in your
application
*/

//#if !defined (STM32F10X_LD) && !defined (STM32F10X_MD) && !defined (STM32F10X_HD)
/* #define STM32F10X_LD */   /*!< STM32 Low density devices */
/* #define STM32F10X_MD */   /*!< STM32 Medium density devices */
#define STM32F10X_HD   /*!< STM32 High density devices */
//#endif
/*  Tip: To avoid modifying this file each time you need to switch between these
devices, you can define the device in your toolchain compiler preprocessor.

- Low-density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers
where the Flash memory density ranges between 16 and 32 Kbytes.
- Medium-density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers
where the Flash memory density ranges between 64 and 128 Kbytes.
- High-density devices are STM32F101xx and STM32F103xx microcontrollers where
the Flash memory density ranges between 256 and 512 Kbytes.
*/

#if !defined  USE_STDPERIPH_DRIVER
/**
* @brief Comment the line below if you will not use the peripherals drivers.
In this case, these drivers will not be included and the application code will
be based on direct access to peripherals registers
*/
#define USE_STDPERIPH_DRIVER
#endif

/**
* @brief In the following line adjust the value of External High Speed oscillator (HSE)
used in your application
*/
#define HSE_VALUE    ((uint32_t)8000000) /*!< Value of the External oscillator in Hz*/
/**
* @brief In the following line adjust the External High Speed oscillator (HSE) Startup
Timeout value
*/
#define HSEStartUp_TimeOut   ((uint16_t)0xFFFF) /*!< Time out for HSE start up */

#define HSI_VALUE    ((uint32_t)8000000) /*!< Value of the Internal oscillator in Hz*/


/*!< [31:16] STM32F10x Standard Peripheral Library main version */
#define __STM32F10X_STDPERIPH_VERSION_MAIN   (0x03)
/*!< [15:8]  STM32F10x Standard Peripheral Library sub1 version */
#define __STM32F10X_STDPERIPH_VERSION_SUB1   (0x00)
/*!< [7:0]  STM32F10x Standard Peripheral Library sub2 version */
#define __STM32F10X_STDPERIPH_VERSION_SUB2   (0x00)
/*!< STM32F10x Standard Peripheral Library version number */
#define __STM32F10X_STDPERIPH_VERSION       ((__STM32F10X_STDPERIPH_VERSION_MAIN << 16)\
| (__STM32F10X_STDPERIPH_VERSION_SUB1 << 8)\
    | __STM32F10X_STDPERIPH_VERSION_SUB2)

/**
* @}
*/

/** @addtogroup Configuration_section_for_CMSIS
* @{
*/

/**
* @brief Configuration of the Cortex-M3 Processor and Core Peripherals
*/
#define __MPU_PRESENT             0 /*!< STM32 does not provide a MPU present or not  */
#define __NVIC_PRIO_BITS          4 /*!< STM32 uses 4 Bits for the Priority Levels    */
#define __Vendor_SysTickConfig    0 /*!< Set to 1 if different SysTick Config is used */

/*!< Interrupt Number Definition */
typedef enum IRQn
{
    /******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
    NonMaskableInt_IRQn         = -14,    /*!< 2 Non Maskable Interrupt                             */
    MemoryManagement_IRQn       = -12,    /*!< 4 Cortex-M3 Memory Management Interrupt              */
    BusFault_IRQn               = -11,    /*!< 5 Cortex-M3 Bus Fault Interrupt                      */
    UsageFault_IRQn             = -10,    /*!< 6 Cortex-M3 Usage Fault Interrupt                    */
    SVCall_IRQn                 = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                       */
    DebugMonitor_IRQn           = -4,     /*!< 12 Cortex-M3 Debug Monitor Interrupt                 */
    PendSV_IRQn                 = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                       */
    SysTick_IRQn                = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                   */

    /******  STM32 specific Interrupt Numbers *********************************************************/
    WWDG_IRQn                   = 0,      /*!< Window WatchDog Interrupt                            */
    PVD_IRQn                    = 1,      /*!< PVD through EXTI Line detection Interrupt            */
    TAMPER_IRQn                 = 2,      /*!< Tamper Interrupt                                     */
    RTC_IRQn                    = 3,      /*!< RTC global Interrupt                                 */
    FLASH_IRQn                  = 4,      /*!< FLASH global Interrupt                               */
    RCC_IRQn                    = 5,      /*!< RCC global Interrupt                                 */
    EXTI0_IRQn                  = 6,      /*!< EXTI Line0 Interrupt                                 */
    EXTI1_IRQn                  = 7,      /*!< EXTI Line1 Interrupt                                 */
    EXTI2_IRQn                  = 8,      /*!< EXTI Line2 Interrupt                                 */
    EXTI3_IRQn                  = 9,      /*!< EXTI Line3 Interrupt                                 */
    EXTI4_IRQn                  = 10,     /*!< EXTI Line4 Interrupt                                 */
    DMA1_Channel1_IRQn          = 11,     /*!< DMA1 Channel 1 global Interrupt                      */
    DMA1_Channel2_IRQn          = 12,     /*!< DMA1 Channel 2 global Interrupt                      */
    DMA1_Channel3_IRQn          = 13,     /*!< DMA1 Channel 3 global Interrupt                      */
    DMA1_Channel4_IRQn          = 14,     /*!< DMA1 Channel 4 global Interrupt                      */
    DMA1_Channel5_IRQn          = 15,     /*!< DMA1 Channel 5 global Interrupt                      */
    DMA1_Channel6_IRQn          = 16,     /*!< DMA1 Channel 6 global Interrupt                      */
    DMA1_Channel7_IRQn          = 17,     /*!< DMA1 Channel 7 global Interrupt                      */
    ADC1_2_IRQn                 = 18,     /*!< ADC1 et ADC2 global Interrupt                        */
    USB_HP_CAN1_TX_IRQn         = 19,     /*!< USB High Priority or CAN1 TX Interrupts              */
    USB_LP_CAN1_RX0_IRQn        = 20,     /*!< USB Low Priority or CAN1 RX0 Interrupts              */
    CAN1_RX1_IRQn               = 21,     /*!< CAN1 RX1 Interrupt                                   */
    CAN1_SCE_IRQn               = 22,     /*!< CAN1 SCE Interrupt                                   */
    EXTI9_5_IRQn                = 23,     /*!< External Line[9:5] Interrupts                        */
    TIM1_BRK_IRQn               = 24,     /*!< TIM1 Break Interrupt                                 */
    TIM1_UP_IRQn                = 25,     /*!< TIM1 Update Interrupt                                */
    TIM1_TRG_COM_IRQn           = 26,     /*!< TIM1 Trigger and Commutation Interrupt               */
    TIM1_CC_IRQn                = 27,     /*!< TIM1 Capture Compare Interrupt                       */
    TIM2_IRQn                   = 28,     /*!< TIM2 global Interrupt                                */
    TIM3_IRQn                   = 29,     /*!< TIM3 global Interrupt                                */
#ifndef STM32F10X_LD
    TIM4_IRQn                   = 30,     /*!< TIM4 global Interrupt                                */
#endif
    I2C1_EV_IRQn                = 31,     /*!< I2C1 Event Interrupt                                 */
    I2C1_ER_IRQn                = 32,     /*!< I2C1 Error Interrupt                                 */
#ifndef STM32F10X_LD
    I2C2_EV_IRQn                = 33,     /*!< I2C2 Event Interrupt                                 */
    I2C2_ER_IRQn                = 34,     /*!< I2C2 Error Interrupt                                 */
#endif
    SPI1_IRQn                   = 35,     /*!< SPI1 global Interrupt                                */
    SPI2_IRQn                   = 36,     /*!< SPI2 global Interrupt                                */
    USART1_IRQn                 = 37,     /*!< USART1 global Interrupt                              */
    USART2_IRQn                 = 38,     /*!< USART2 global Interrupt                              */
#ifndef STM32F10X_LD
    USART3_IRQn                 = 39,     /*!< USART3 global Interrupt                              */
#endif
    EXTI15_10_IRQn              = 40,     /*!< External Line[15:10] Interrupts                      */
    RTCAlarm_IRQn               = 41,     /*!< RTC Alarm through EXTI Line Interrupt                */
    USBWakeUp_IRQn              = 42,     /*!< USB WakeUp from suspend through EXTI Line Interrupt  */
#ifdef STM32F10X_HD
    TIM8_BRK_IRQn               = 43,     /*!< TIM8 Break Interrupt                                 */
    TIM8_UP_IRQn                = 44,     /*!< TIM8 Update Interrupt                                */
    TIM8_TRG_COM_IRQn           = 45,     /*!< TIM8 Trigger and Commutation Interrupt               */
    TIM8_CC_IRQn                = 46,     /*!< TIM8 Capture Compare Interrupt                       */
    ADC3_IRQn                   = 47,     /*!< ADC3 global Interrupt                                */
    FSMC_IRQn                   = 48,     /*!< FSMC global Interrupt                                */
    SDIO_IRQn                   = 49,     /*!< SDIO global Interrupt                                */
    TIM5_IRQn                   = 50,     /*!< TIM5 global Interrupt                                */
    SPI3_IRQn                   = 51,     /*!< SPI3 global Interrupt                                */
    UART4_IRQn                  = 52,     /*!< UART4 global Interrupt                               */
    UART5_IRQn                  = 53,     /*!< UART5 global Interrupt                               */
    TIM6_IRQn                   = 54,     /*!< TIM6 global Interrupt                                */
    TIM7_IRQn                   = 55,     /*!< TIM7 global Interrupt                                */
    DMA2_Channel1_IRQn          = 56,     /*!< DMA2 Channel 1 global Interrupt                      */
    DMA2_Channel2_IRQn          = 57,     /*!< DMA2 Channel 2 global Interrupt                      */
    DMA2_Channel3_IRQn          = 58,     /*!< DMA2 Channel 3 global Interrupt                      */
    DMA2_Channel4_5_IRQn        = 59      /*!< DMA2 Channel 4 and Channel 5 global Interrupt        */
#endif
} IRQn_Type;

/**
* @}
*/

#include "core_cm3.h"
//#include "misc.h"
#include "system_stm32f10x.h"
#include <stdint.h>

/** @addtogroup Exported_types
* @{
*/

/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy prupose) */
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

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
#include <stdbool.h>
#define FALSE false
#define TRUE  true

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

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
    //__IO uint32_t SR;
    union {
        __IO uint32_t SR;
        struct {
            __IO uint32_t AWD:1;
            __IO uint32_t EOC:1;
            __IO uint32_t JEOC:1;
            __IO uint32_t JSTRT:1;
            __IO uint32_t STRT:1;
            __IO uint32_t Reserved31_5:27;
        }BitSR;
    };
    //__IO uint32_t CR1;
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
            __IO uint32_t DUALMOD:4;
            __IO uint32_t Reserved21_20:2;
            __IO uint32_t JAWDEN:1;
            __IO uint32_t AWDEN:1;
            __IO uint32_t Reserved31_24:8;
        }BitCR1;
    };
    //__IO uint32_t CR2;
    union {
        __IO uint32_t CR2;
        struct {
            __IO uint32_t ADON:1;
            __IO uint32_t CONT:1;
            __IO uint32_t CAL:1;
            __IO uint32_t RSTCAL:1;
            __IO uint32_t Reserved7_4:4;
            __IO uint32_t DMA:1;
            __IO uint32_t Reserved10_9:2;
            __IO uint32_t ALIGN:1;
            __IO uint32_t JEXTSEL:3;
            __IO uint32_t JEXTTRIG:1;
            __IO uint32_t Reserved16:1;
            __IO uint32_t EXTSEL:3;
            __IO uint32_t EXTTRIG:1;
            __IO uint32_t JSWSTART:1;
            __IO uint32_t SWSTART:1;
            __IO uint32_t TSVREFE:1;
            __IO uint32_t Reserved31_24:8;
        }BitCR2;
    };
    __IO uint32_t SMPR1;
    __IO uint32_t SMPR2;
    __IO uint32_t JOFR1;
    __IO uint32_t JOFR2;
    __IO uint32_t JOFR3;
    __IO uint32_t JOFR4;
    __IO uint32_t HTR;
    __IO uint32_t LTR;

    //__IO uint32_t SQR1;
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
    //__IO uint32_t SQR2;
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
    //__IO uint32_t SQR3;
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
    //__IO uint32_t JSQR;
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

    __IO uint32_t JDR1;
    __IO uint32_t JDR2;
    __IO uint32_t JDR3;
    __IO uint32_t JDR4;
    __IO uint32_t DR;
} ADC_TypeDef;

/**
* @brief Backup Registers
*/

typedef struct
{
    uint32_t  RESERVED0;
    __IO uint16_t DR1;
    uint16_t  RESERVED1;
    __IO uint16_t DR2;
    uint16_t  RESERVED2;
    __IO uint16_t DR3;
    uint16_t  RESERVED3;
    __IO uint16_t DR4;
    uint16_t  RESERVED4;
    __IO uint16_t DR5;
    uint16_t  RESERVED5;
    __IO uint16_t DR6;
    uint16_t  RESERVED6;
    __IO uint16_t DR7;
    uint16_t  RESERVED7;
    __IO uint16_t DR8;
    uint16_t  RESERVED8;
    __IO uint16_t DR9;
    uint16_t  RESERVED9;
    __IO uint16_t DR10;
    uint16_t  RESERVED10;
    __IO uint16_t RTCCR;
    uint16_t  RESERVED11;
    __IO uint16_t CR;
    uint16_t  RESERVED12;
    __IO uint16_t CSR;
    uint16_t  RESERVED13[5];
    __IO uint16_t DR11;
    uint16_t  RESERVED14;
    __IO uint16_t DR12;
    uint16_t  RESERVED15;
    __IO uint16_t DR13;
    uint16_t  RESERVED16;
    __IO uint16_t DR14;
    uint16_t  RESERVED17;
    __IO uint16_t DR15;
    uint16_t  RESERVED18;
    __IO uint16_t DR16;
    uint16_t  RESERVED19;
    __IO uint16_t DR17;
    uint16_t  RESERVED20;
    __IO uint16_t DR18;
    uint16_t  RESERVED21;
    __IO uint16_t DR19;
    uint16_t  RESERVED22;
    __IO uint16_t DR20;
    uint16_t  RESERVED23;
    __IO uint16_t DR21;
    uint16_t  RESERVED24;
    __IO uint16_t DR22;
    uint16_t  RESERVED25;
    __IO uint16_t DR23;
    uint16_t  RESERVED26;
    __IO uint16_t DR24;
    uint16_t  RESERVED27;
    __IO uint16_t DR25;
    uint16_t  RESERVED28;
    __IO uint16_t DR26;
    uint16_t  RESERVED29;
    __IO uint16_t DR27;
    uint16_t  RESERVED30;
    __IO uint16_t DR28;
    uint16_t  RESERVED31;
    __IO uint16_t DR29;
    uint16_t  RESERVED32;
    __IO uint16_t DR30;
    uint16_t  RESERVED33;
    __IO uint16_t DR31;
    uint16_t  RESERVED34;
    __IO uint16_t DR32;
    uint16_t  RESERVED35;
    __IO uint16_t DR33;
    uint16_t  RESERVED36;
    __IO uint16_t DR34;
    uint16_t  RESERVED37;
    __IO uint16_t DR35;
    uint16_t  RESERVED38;
    __IO uint16_t DR36;
    uint16_t  RESERVED39;
    __IO uint16_t DR37;
    uint16_t  RESERVED40;
    __IO uint16_t DR38;
    uint16_t  RESERVED41;
    __IO uint16_t DR39;
    uint16_t  RESERVED42;
    __IO uint16_t DR40;
    uint16_t  RESERVED43;
    __IO uint16_t DR41;
    uint16_t  RESERVED44;
    __IO uint16_t DR42;
    uint16_t  RESERVED45;
} BKP_TypeDef;

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
    __IO uint32_t FR1;
    __IO uint32_t FR2;
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
    CAN_FilterRegister_TypeDef sFilterRegister[14];
} CAN_TypeDef;

/**
* @brief CRC calculation unit
*/

typedef struct
{
    __IO uint32_t DR;
    __IO uint8_t  IDR;
    uint8_t   RESERVED0;
    uint16_t  RESERVED1;
    __IO uint32_t CR;
} CRC_TypeDef;

/**
* @brief Digital to Analog Converter
*/

typedef struct
{
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
            __IO uint32_t Reserved15_13:3;
            __IO uint32_t EN2:1;
            __IO uint32_t BOFF2:1;
            __IO uint32_t TEN2:1;
            __IO uint32_t TSEL2:3;
            __IO uint32_t WAVE2:2;
            __IO uint32_t MAMP2:4;
            __IO uint32_t DMAEN2:1;
            __IO uint32_t Reserved31_29:3;
        }BitCR;
    };

    union {
        __IO uint32_t SWTRIGR;
        struct {
            __IO uint32_t SWTRIG1:1;
            __IO uint32_t SWTRIG2:1;
            __IO uint32_t Reserved31_2:30;
        }BitSWTRIGR;
    };

    __IO uint32_t DHR12R1;
    __IO uint32_t DHR12L1;
    __IO uint32_t DHR8R1;
    __IO uint32_t DHR12R2;
    __IO uint32_t DHR12L2;
    __IO uint32_t DHR8R2;
    __IO uint32_t DHR12RD;
    __IO uint32_t DHR12LD;
    __IO uint32_t DHR8RD;
    __IO uint32_t DOR1;
    __IO uint32_t DOR2;
} DAC_TypeDef;

/**
* @brief Debug MCU
*/

typedef struct
{
    __IO uint32_t IDCODE;

    union {
        __IO uint32_t CR;

        struct {
            __IO uint32_t DBG_SLEEP:1;
            __IO uint32_t DBG_STOP:1;
            __IO uint32_t DBG_STANDBY:1;
            __IO uint32_t Reserved4_3:2;
            __IO uint32_t TRACE_IOEN:1;
            __IO uint32_t TRACE_MODE:2;
            __IO uint32_t DBG_IWDG_STOP:1;
            __IO uint32_t DBG_WWDG_STOP:1;
            __IO uint32_t DBG_TIM1_STOP:1;
            __IO uint32_t DBG_TIM2_STOP:1;
            __IO uint32_t DBG_TIM3_STOP:1;
            __IO uint32_t DBG_TIM4_STOP:1;
            __IO uint32_t DBG_CAN1_STOP:1;
            __IO uint32_t DBG_I2C1_SMBUSTIMEOUT:1;
            __IO uint32_t DBG_I2C2_SMBUSTIMEOUT:1;
            __IO uint32_t DBG_TIM5_STOP:1;
            __IO uint32_t DBG_TIM6_STOP:1;
            __IO uint32_t DBG_TIM7_STOP:1;
            __IO uint32_t DBG_TIM8_STOP:1;
            __IO uint32_t DBG_CAN2_STOP:1;
            __IO uint32_t Reserved31_22:10;
        }bitCR;
    };
}DBGMCU_TypeDef;

/**
* @brief DMA Controller
*/

typedef struct
{
    union{
        __IO uint32_t CCR;
        struct{
            __IO uint32_t EN:1;
            __IO uint32_t TCIE:1;
            __IO uint32_t HTIE:1;
            __IO uint32_t TEIE:1;
            __IO uint32_t DIR:1;
            __IO uint32_t CIRC:1;
            __IO uint32_t PINC:1;
            __IO uint32_t MINC:1;
            __IO uint32_t PSIZE:2;
            __IO uint32_t MSIZE:2;
            __IO uint32_t PL:2;
            __IO uint32_t MEM2MEM:1;
            __IO uint32_t Reserved31_15:17;
        }BitCCR;
    };

    __IO uint32_t CNDTR;
    __IO uint32_t CPAR;
    __IO uint32_t CMAR;
    __IO uint32_t Res;
} DMA_Channel_TypeDef;

typedef struct
{
    __IO uint32_t ISR;
    __IO uint32_t IFCR;
    DMA_Channel_TypeDef Stream[7];
} DMA_TypeDef;

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
            __IO uint32_t Reserved31_20:12;
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
            __IO uint32_t Reserved31_20:12;
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
            __IO uint32_t Reserved31_20:12;
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
            __IO uint32_t Reserved31_20:12;
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
            __IO uint32_t Reserved31_20:12;
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
            __IO uint32_t Reserved31_20:12;
        }BitPR;
    };
} EXTI_TypeDef;

/**
* @brief FLASH Registers
*/

typedef struct
{
    __IO uint32_t ACR;
    __IO uint32_t KEYR;
    __IO uint32_t OPTKEYR;
    __IO uint32_t SR;
    __IO uint32_t CR;
    __IO uint32_t AR;
    __IO uint32_t RESERVED;
    __IO uint32_t OBR;
    __IO uint32_t WRPR;
} FLASH_TypeDef;

/**
* @brief Option Bytes Registers
*/

typedef struct
{
    __IO uint16_t RDP;
    __IO uint16_t USER;
    __IO uint16_t Data0;
    __IO uint16_t Data1;
    __IO uint16_t WRP0;
    __IO uint16_t WRP1;
    __IO uint16_t WRP2;
    __IO uint16_t WRP3;
} OB_TypeDef;

/**
* @brief Flexible Static Memory Controller
*/

typedef struct
{
    __IO uint32_t BTCR[8];
} FSMC_Bank1_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank1E
*/

typedef struct
{
    __IO uint32_t BWTR[7];
} FSMC_Bank1E_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank2
*/

typedef struct
{
    __IO uint32_t PCR2;
    __IO uint32_t SR2;
    __IO uint32_t PMEM2;
    __IO uint32_t PATT2;
    uint32_t  RESERVED0;
    __IO uint32_t ECCR2;
} FSMC_Bank2_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank3
*/

typedef struct
{
    __IO uint32_t PCR3;
    __IO uint32_t SR3;
    __IO uint32_t PMEM3;
    __IO uint32_t PATT3;
    uint32_t  RESERVED0;
    __IO uint32_t ECCR3;
} FSMC_Bank3_TypeDef;

/**
* @brief Flexible Static Memory Controller Bank4
*/

typedef struct
{
    __IO uint32_t PCR4;
    __IO uint32_t SR4;
    __IO uint32_t PMEM4;
    __IO uint32_t PATT4;
    __IO uint32_t PIO4;
} FSMC_Bank4_TypeDef;

/**
* @brief General Purpose IO
*/

typedef struct
{
    union{
        __IO uint32_t CRL;
        struct{
            __IO uint32_t MODE0:2;//                      ((uint32_t)0x00000003)        /*!< MODE0[1:0] bits (Port x mode bits, pin 0) */
            __IO uint32_t CNF0:2;//                       ((uint32_t)0x0000000C)        /*!< CNF0[1:0] bits (Port x configuration bits, pin 0) */
            __IO uint32_t MODE1:2;//                      ((uint32_t)0x00000030)        /*!< MODE1[1:0] bits (Port x mode bits, pin 1) */
            __IO uint32_t CNF1:2;//                       ((uint32_t)0x000000C0)        /*!< CNF1[1:0] bits (Port x configuration bits, pin 1) */
            __IO uint32_t MODE2:2;//                      ((uint32_t)0x00000300)        /*!< MODE2[1:0] bits (Port x mode bits, pin 2) */
            __IO uint32_t CNF2:2;//                       ((uint32_t)0x00000C00)        /*!< CNF2[1:0] bits (Port x configuration bits, pin 2) */
            __IO uint32_t MODE3:2;//                      ((uint32_t)0x00003000)        /*!< MODE3[1:0] bits (Port x mode bits, pin 3) */
            __IO uint32_t CNF3:2;//                       ((uint32_t)0x0000C000)        /*!< CNF3[1:0] bits (Port x configuration bits, pin 3) */
            __IO uint32_t MODE4:2;//                      ((uint32_t)0x00030000)        /*!< MODE4[1:0] bits (Port x mode bits, pin 4) */
            __IO uint32_t CNF4:2;//                       ((uint32_t)0x000C0000)        /*!< CNF4[1:0] bits (Port x configuration bits, pin 4) */
            __IO uint32_t MODE5:2;//                      ((uint32_t)0x00300000)        /*!< MODE5[1:0] bits (Port x mode bits, pin 5) */
            __IO uint32_t CNF5:2;//                       ((uint32_t)0x00C00000)        /*!< CNF5[1:0] bits (Port x configuration bits, pin 5) */
            __IO uint32_t MODE6:2;//                      ((uint32_t)0x03000000)        /*!< MODE6[1:0] bits (Port x mode bits, pin 6) */
            __IO uint32_t CNF6:2;//                       ((uint32_t)0x0C000000)        /*!< CNF6[1:0] bits (Port x configuration bits, pin 6) */
            __IO uint32_t MODE7:2;//                      ((uint32_t)0x30000000)        /*!< MODE7[1:0] bits (Port x mode bits, pin 7) */
            __IO uint32_t CNF7:2;//                       ((uint32_t)0xC0000000)        /*!< CNF7[1:0] bits (Port x configuration bits, pin 7) */
        }BitCRL;
    };
    union{
        __IO uint32_t CRH;
        struct{
            __IO uint32_t MODE8:2;//                       ((uint32_t)0x00000003)        /*!< MODE0[1:0] bits (Port x mode bits, pin 0) */
            __IO uint32_t CNF8:2;//                        ((uint32_t)0x0000000C)        /*!< CNF0[1:0] bits (Port x configuration bits, pin 0) */
            __IO uint32_t MODE9:2;//                       ((uint32_t)0x00000030)        /*!< MODE1[1:0] bits (Port x mode bits, pin 1) */
            __IO uint32_t CNF9:2;//                        ((uint32_t)0x000000C0)        /*!< CNF1[1:0] bits (Port x configuration bits, pin 1) */
            __IO uint32_t MODE10:2;//                      ((uint32_t)0x00000300)        /*!< MODE2[1:0] bits (Port x mode bits, pin 2) */
            __IO uint32_t CNF10:2;//                       ((uint32_t)0x00000C00)        /*!< CNF2[1:0] bits (Port x configuration bits, pin 2) */
            __IO uint32_t MODE11:2;//                      ((uint32_t)0x00003000)        /*!< MODE3[1:0] bits (Port x mode bits, pin 3) */
            __IO uint32_t CNF11:2;//                       ((uint32_t)0x0000C000)        /*!< CNF3[1:0] bits (Port x configuration bits, pin 3) */
            __IO uint32_t MODE12:2;//                      ((uint32_t)0x00030000)        /*!< MODE4[1:0] bits (Port x mode bits, pin 4) */
            __IO uint32_t CNF12:2;//                       ((uint32_t)0x000C0000)        /*!< CNF4[1:0] bits (Port x configuration bits, pin 4) */
            __IO uint32_t MODE13:2;//                      ((uint32_t)0x00300000)        /*!< MODE5[1:0] bits (Port x mode bits, pin 5) */
            __IO uint32_t CNF13:2;//                       ((uint32_t)0x00C00000)        /*!< CNF5[1:0] bits (Port x configuration bits, pin 5) */
            __IO uint32_t MODE14:2;//                      ((uint32_t)0x03000000)        /*!< MODE6[1:0] bits (Port x mode bits, pin 6) */
            __IO uint32_t CNF14:2;//                       ((uint32_t)0x0C000000)        /*!< CNF6[1:0] bits (Port x configuration bits, pin 6) */
            __IO uint32_t MODE15:2;//                      ((uint32_t)0x30000000)        /*!< MODE7[1:0] bits (Port x mode bits, pin 7) */
            __IO uint32_t CNF15:2;//                       ((uint32_t)0xC0000000)        /*!< CNF7[1:0] bits (Port x configuration bits, pin 7) */
        }BitCRH;
    };
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
    union {
        __IO uint32_t ODR;
        struct {
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
    union {
        __IO uint32_t BSRR;
        struct {
            __IO uint32_t BS0:1;//                        ((uint32_t)0x00000001)        /*!< Port x Set bit 0 */
            __IO uint32_t BS1:1;//                        ((uint32_t)0x00000002)        /*!< Port x Set bit 1 */
            __IO uint32_t BS2:1;//                        ((uint32_t)0x00000004)        /*!< Port x Set bit 2 */
            __IO uint32_t BS3:1;//                        ((uint32_t)0x00000008)        /*!< Port x Set bit 3 */
            __IO uint32_t BS4:1;//                        ((uint32_t)0x00000010)        /*!< Port x Set bit 4 */
            __IO uint32_t BS5:1;//                        ((uint32_t)0x00000020)        /*!< Port x Set bit 5 */
            __IO uint32_t BS6:1;//                        ((uint32_t)0x00000040)        /*!< Port x Set bit 6 */
            __IO uint32_t BS7:1;//                        ((uint32_t)0x00000080)        /*!< Port x Set bit 7 */
            __IO uint32_t BS8:1;//                        ((uint32_t)0x00000100)        /*!< Port x Set bit 8 */
            __IO uint32_t BS9:1;//                        ((uint32_t)0x00000200)        /*!< Port x Set bit 9 */
            __IO uint32_t BS10:1;//                       ((uint32_t)0x00000400)        /*!< Port x Set bit 10 */
            __IO uint32_t BS11:1;//                       ((uint32_t)0x00000800)        /*!< Port x Set bit 11 */
            __IO uint32_t BS12:1;//                       ((uint32_t)0x00001000)        /*!< Port x Set bit 12 */
            __IO uint32_t BS13:1;//                       ((uint32_t)0x00002000)        /*!< Port x Set bit 13 */
            __IO uint32_t BS14:1;//                       ((uint32_t)0x00004000)        /*!< Port x Set bit 14 */
            __IO uint32_t BS15:1;//                       ((uint32_t)0x00008000)        /*!< Port x Set bit 15 */

            __IO uint32_t BR0:1;//                        ((uint32_t)0x00010000)        /*!< Port x Reset bit 0 */
            __IO uint32_t BR1:1;//                        ((uint32_t)0x00020000)        /*!< Port x Reset bit 1 */
            __IO uint32_t BR2:1;//                        ((uint32_t)0x00040000)        /*!< Port x Reset bit 2 */
            __IO uint32_t BR3:1;//                        ((uint32_t)0x00080000)        /*!< Port x Reset bit 3 */
            __IO uint32_t BR4:1;//                        ((uint32_t)0x00100000)        /*!< Port x Reset bit 4 */
            __IO uint32_t BR5:1;//                        ((uint32_t)0x00200000)        /*!< Port x Reset bit 5 */
            __IO uint32_t BR6:1;//                        ((uint32_t)0x00400000)        /*!< Port x Reset bit 6 */
            __IO uint32_t BR7:1;//                        ((uint32_t)0x00800000)        /*!< Port x Reset bit 7 */
            __IO uint32_t BR8:1;//                        ((uint32_t)0x01000000)        /*!< Port x Reset bit 8 */
            __IO uint32_t BR9:1;//                        ((uint32_t)0x02000000)        /*!< Port x Reset bit 9 */
            __IO uint32_t BR10:1;//                       ((uint32_t)0x04000000)        /*!< Port x Reset bit 10 */
            __IO uint32_t BR11:1;//                       ((uint32_t)0x08000000)        /*!< Port x Reset bit 11 */
            __IO uint32_t BR12:1;//                       ((uint32_t)0x10000000)        /*!< Port x Reset bit 12 */
            __IO uint32_t BR13:1;//                       ((uint32_t)0x20000000)        /*!< Port x Reset bit 13 */
            __IO uint32_t BR14:1;//                       ((uint32_t)0x40000000)        /*!< Port x Reset bit 14 */
            __IO uint32_t BR15:1;//                       ((uint32_t)0x80000000)        /*!< Port x Reset bit 15 */
        }BitBSRR;
    };
    union {
        __IO uint32_t BRR;
        struct {
            __IO uint32_t BRR0:1;//                        ((uint16_t)0x0001)            /*!< Port input data, bit 0 */
            __IO uint32_t BRR1:1;//                        ((uint16_t)0x0002)            /*!< Port input data, bit 1 */
            __IO uint32_t BRR2:1;//                        ((uint16_t)0x0004)            /*!< Port input data, bit 2 */
            __IO uint32_t BRR3:1;//                        ((uint16_t)0x0008)            /*!< Port input data, bit 3 */
            __IO uint32_t BRR4:1;//                        ((uint16_t)0x0010)            /*!< Port input data, bit 4 */
            __IO uint32_t BRR5:1;//                        ((uint16_t)0x0020)            /*!< Port input data, bit 5 */
            __IO uint32_t BRR6:1;//                        ((uint16_t)0x0040)            /*!< Port input data, bit 6 */
            __IO uint32_t BRR7:1;//                        ((uint16_t)0x0080)            /*!< Port input data, bit 7 */
            __IO uint32_t BRR8:1;//                        ((uint16_t)0x0100)            /*!< Port input data, bit 8 */
            __IO uint32_t BRR9:1;//                        ((uint16_t)0x0200)            /*!< Port input data, bit 9 */
            __IO uint32_t BRR10:1;//                       ((uint16_t)0x0400)            /*!< Port input data, bit 10 */
            __IO uint32_t BRR11:1;//                       ((uint16_t)0x0800)            /*!< Port input data, bit 11 */
            __IO uint32_t BRR12:1;//                       ((uint16_t)0x1000)            /*!< Port input data, bit 12 */
            __IO uint32_t BRR13:1;//                       ((uint16_t)0x2000)            /*!< Port input data, bit 13 */
            __IO uint32_t BRR14:1;//                       ((uint16_t)0x4000)            /*!< Port input data, bit 14 */
            __IO uint32_t BRR15:1;//                       ((uint16_t)0x8000)            /*!< Port input data, bit 15 */
            __IO uint32_t Reserved16_31:16;
        }BitBRR;
    };
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
            __IO uint32_t Reserved17_31:15;
        }BitLCKR;
    };
} GPIO_TypeDef;

/**
* @brief Alternate Function IO
*/

typedef struct
{
    union
    {
        __IO uint32_t EVCR;
        struct
        {
            __IO uint32_t PIN:4;//                        ((uint8_t)0x0F)               /*!< PIN[3:0] bits (Pin selection) */
            __IO uint32_t PORT:3;//                       ((uint8_t)0x70)               /*!< PORT[2:0] bits (Port selection) */
            __IO uint32_t EVOE:1;//                       ((uint8_t)0x80)               /*!< Event Output Enable */
            __IO uint32_t Reserved8_31:24;
        }BitEVCR;
    };
    union
    {
        __IO uint32_t MAPR;
        struct
        {
            __IO uint32_t SPI1_REMAP:1;//                 ((uint32_t)0x00000001)        /*!< SPI1 remapping */
            __IO uint32_t I2C1_REMAP:1;//                 ((uint32_t)0x00000002)        /*!< I2C1 remapping */
            __IO uint32_t USART1_REMAP:1;//               ((uint32_t)0x00000004)        /*!< USART1 remapping */
            __IO uint32_t USART2_REMAP:1;//               ((uint32_t)0x00000008)        /*!< USART2 remapping */
            __IO uint32_t USART3_REMAP:2;//               ((uint32_t)0x00000030)        /*!< USART3_REMAP[1:0] bits (USART3 remapping) */
            __IO uint32_t TIM1_REMAP:2;//                 ((uint32_t)0x000000C0)        /*!< TIM1_REMAP[1:0] bits (TIM1 remapping) */
            __IO uint32_t TIM2_REMAP:2;//                 ((uint32_t)0x00000300)        /*!< TIM2_REMAP[1:0] bits (TIM2 remapping) */
            __IO uint32_t TIM3_REMAP:2;//                 ((uint32_t)0x00000C00)        /*!< TIM3_REMAP[1:0] bits (TIM3 remapping) */
            __IO uint32_t TIM4_REMAP:1;//                 ((uint32_t)0x00001000)        /*!< Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
            __IO uint32_t CAN_REMAP:2;//                  ((uint32_t)0x00006000)        /*!< CAN_REMAP[1:0] bits (CAN Alternate function remapping) */
            __IO uint32_t PD01_REMAP:1;//                 ((uint32_t)0x00008000)        /*!< Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
            __IO uint32_t TIM5CH4_IREMAP:1;//             ((uint32_t)0x00010000)        /*!< TIM5 Channel4 Internal Remap */
            __IO uint32_t ADC1_ETRGINJ_REMAP:1;//         ((uint32_t)0x00020000)        /*!< ADC 1 External Trigger Injected Conversion remapping */
            __IO uint32_t ADC1_ETRGREG_REMAP:1;//         ((uint32_t)0x00040000)        /*!< ADC 1 External Trigger Regular Conversion remapping */
            __IO uint32_t ADC2_ETRGINJ_REMAP:1;//         ((uint32_t)0x00080000)        /*!< ADC 2 External Trigger Injected Conversion remapping */
            __IO uint32_t ADC2_ETRGREG_REMAP:1;//         ((uint32_t)0x00100000)        /*!< ADC 2 External Trigger Regular Conversion remapping */
            __IO uint32_t ETH_REMAP:1;
            __IO uint32_t CAN2_REMAP:1;
            __IO uint32_t MII_RMII_SEL:1;
            __IO uint32_t SWJ_CFG:3;//                    ((uint32_t)0x07000000)        /*!< SWJ_CFG[2:0] bits (Serial Wire JTAG configuration) */
            __IO uint32_t Reserved27:1;
            __IO uint32_t SPI3_REMAP:1;
            __IO uint32_t TIM2ITR1_REMAP:1;
            __IO uint32_t PTP_PPS_REMAP:1;
            __IO uint32_t Reserved31:1;
        }BitMAPR;
    };
    union
    {
        __IO uint32_t EXTICR[4];
        struct
        {
            struct
            {
                __IO uint32_t EXTI0:4;
                __IO uint32_t EXTI1:4;
                __IO uint32_t EXTI2:4;
                __IO uint32_t EXTI3:4;
                __IO uint32_t Reserved1_16_31:16;
            }EXTICR1;
            struct
            {
                __IO uint32_t EXTI4:4;
                __IO uint32_t EXTI5:4;
                __IO uint32_t EXTI6:4;
                __IO uint32_t EXTI7:4;
                __IO uint32_t Reserved2_16_31:16;
            }EXTICR2;
            struct
            {
                __IO uint32_t EXTI8:4;
                __IO uint32_t EXTI9:4;
                __IO uint32_t EXTI10:4;
                __IO uint32_t EXTI11:4;
                __IO uint32_t Reserved3_16_31:16;
            }EXTICR3;
            struct
            {
                __IO uint32_t EXTI12:4;
                __IO uint32_t EXTI13:4;
                __IO uint32_t EXTI14:4;
                __IO uint32_t EXTI15:4;
                __IO uint32_t Reserved4_16_31:16;
            }EXTICR4;
        }BitEXTICR;
    };
} AFIO_TypeDef;
/**
* @brief Inter-integrated Circuit Interface
*/

typedef struct
{
    union
    {
        __IO uint16_t CR1;
        struct
        {
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
    uint16_t  RESERVED0;
    union
    {
        __IO uint16_t CR2;
        struct
        {
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
    uint16_t  RESERVED1;
    union
    {
        __IO uint16_t OAR1;
        struct
        {
            __IO uint16_t ADD0:1;//                       ((uint16_t)0x0001)            /*!<Bit 0 */
            __IO uint16_t ADD1_7:7;//                     ((uint16_t)0x00FE)            /*!<Interface Address */
            __IO uint16_t ADD8_9:2;//                     ((uint16_t)0x0300)            /*!<Interface Address */
            __IO uint16_t Reserved13_10:4;
            __IO uint16_t Reserved14:1;
            __IO uint16_t ADDMODE:1;//                    ((uint16_t)0x8000)            /*!<Addressing Mode (Slave mode) */
        }BitOAR1;
    };
    uint16_t  RESERVED2;
    union
    {
        __IO uint16_t OAR2;
        struct
        {
            __IO uint16_t ENDUAL:1;//
            __IO uint16_t ADD2_7_1:7;//                       ((uint16_t)0x0001)            /*!<Bit 0 */
            __IO uint16_t Reserved15_8:8;
        }BitOAR2;
    };
    uint16_t  RESERVED3;
    __IO uint16_t DR;
    uint16_t  RESERVED4;
    union
    {
        __IO uint16_t SR1;
        struct
        {
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
    uint16_t  RESERVED5;
    union
    {
        __IO uint16_t SR2;
        struct
        {
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

    uint16_t  RESERVED6;
    __IO uint16_t CCR;
    uint16_t  RESERVED7;
    __IO uint16_t TRISE;
    uint16_t  RESERVED8;
} I2C_TypeDef;

/**
* @brief Independent WATCHDOG
*/

typedef struct
{
    __IO uint32_t KR;
    __IO uint32_t PR;
    __IO uint32_t RLR;
    __IO uint32_t SR;
} IWDG_TypeDef;

/**
* @brief Power Control
*/

typedef struct
{
    //__IO uint32_t CR;
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
            __IO uint32_t Reserved31_9:23;
        }BitCR;
    };
    //__IO uint32_t CSR;
    union {
        __IO uint32_t CSR;
        struct {
            __IO uint32_t WUF:1;
            __IO uint32_t SBF :1;
            __IO uint32_t PVDO :1;
            __IO uint32_t Reserved7_3:5;
            __IO uint32_t EWUP:1;
            __IO uint32_t Reserved31_9:23;
        }BitCSR;
    };
} PWR_TypeDef;

/**
* @brief Reset and Clock Control
*/

typedef struct
{
    union
    {
        __IO uint32_t CR;
        struct
        {
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
            __IO uint32_t Reserved31_26:6;
        }BitCR;
    };
    union
    {
        __IO uint32_t CFGR;
        struct
        {
            __IO uint32_t SW:2;//                         ((uint32_t)0x00000003)        /*!< SW[1:0] bits (System clock Switch) */
            __IO uint32_t SWS:2;//                        ((uint32_t)0x0000000C)        /*!< SWS[1:0] bits (System Clock Switch Status) */
            __IO uint32_t HPRE:4;//                       ((uint32_t)0x000000F0)        /*!< HPRE[3:0] bits (AHB prescaler) */
            __IO uint32_t PPRE1:3;//                      ((uint32_t)0x00000700)        /*!< PRE1[2:0] bits (APB1 prescaler) */
            __IO uint32_t PPRE2:3;//                      ((uint32_t)0x00003800)        /*!< PRE2[2:0] bits (APB2 prescaler) */
            __IO uint32_t ADCPRE:2;//                     ((uint32_t)0x0000C000)        /*!< ADCPRE[1:0] bits (ADC prescaler) */
            __IO uint32_t PLLSRC:1;//                     ((uint32_t)0x00010000)        /*!< PLL entry clock source */
            __IO uint32_t PLLXTPRE:1;//                   ((uint32_t)0x00020000)        /*!< HSE divider for PLL entry */
            __IO uint32_t PLLMULL:4;//                    ((uint32_t)0x003C0000)        /*!< PLLMUL[3:0] bits (PLL multiplication factor) */
            __IO uint32_t USBPRE:1;//                     ((uint32_t)0x00400000)        /*!< USB prescaler */
            __IO uint32_t Reserved23:1;
            __IO uint32_t MCO:3;//                        ((uint32_t)0x07000000)        /*!< MCO[2:0] bits (Microcontroller Clock Output) */
            __IO uint32_t Reserved31_27:5;
        }BitCFGR;
    };
    union
    {
        __IO uint32_t CIR;
        struct
        {
            __IO uint32_t LSIRDYF:1;//                     ((uint32_t)0x00000001)        /*!< LSI Ready Interrupt flag */
            __IO uint32_t LSERDYF:1;//                     ((uint32_t)0x00000002)        /*!< LSE Ready Interrupt flag */
            __IO uint32_t HSIRDYF:1;//                     ((uint32_t)0x00000004)        /*!< HSI Ready Interrupt flag */
            __IO uint32_t HSERDYF:1;//                     ((uint32_t)0x00000008)        /*!< HSE Ready Interrupt flag */
            __IO uint32_t PLLRDYF:1;//                     ((uint32_t)0x00000010)        /*!< PLL Ready Interrupt flag */
            __IO uint32_t Reserved6_5:2;
            __IO uint32_t CSSF:1;//                        ((uint32_t)0x00000080)        /*!< Clock Security System Interrupt flag */
            __IO uint32_t LSIRDYIE:1;//                    ((uint32_t)0x00000100)        /*!< LSI Ready Interrupt Enable */
            __IO uint32_t LSERDYIE:1;//                    ((uint32_t)0x00000200)        /*!< LSE Ready Interrupt Enable */
            __IO uint32_t HSIRDYIE:1;//                    ((uint32_t)0x00000400)        /*!< HSI Ready Interrupt Enable */
            __IO uint32_t HSERDYIE:1;//                    ((uint32_t)0x00000800)        /*!< HSE Ready Interrupt Enable */
            __IO uint32_t PLLRDYIE:1;//                    ((uint32_t)0x00001000)        /*!< PLL Ready Interrupt Enable */
            __IO uint32_t Reserved15_13:3;
            __IO uint32_t LSIRDYC:1;//                     ((uint32_t)0x00010000)        /*!< LSI Ready Interrupt Clear */
            __IO uint32_t LSERDYC:1;//                     ((uint32_t)0x00020000)        /*!< LSE Ready Interrupt Clear */
            __IO uint32_t HSIRDYC:1;//                     ((uint32_t)0x00040000)        /*!< HSI Ready Interrupt Clear */
            __IO uint32_t HSERDYC:1;//                     ((uint32_t)0x00080000)        /*!< HSE Ready Interrupt Clear */
            __IO uint32_t PLLRDYC:1;//                     ((uint32_t)0x00100000)        /*!< PLL Ready Interrupt Clear */
            __IO uint32_t Reserved22_21:2;
            __IO uint32_t CSSC:1;//                        ((uint32_t)0x00800000)        /*!< Clock Security System Interrupt Clear */
            __IO uint32_t Reserved31_24:8;
        }BitCIR;
    };
    union
    {
        __IO uint32_t APB2RSTR;
        struct
        {
            __IO uint32_t AFIORST:1;//                ((uint16_t)0x0001)            /*!< Alternate Function I/O reset */
            __IO uint32_t Reserved1:1;
            __IO uint32_t IOPARST:1;//                ((uint16_t)0x0004)            /*!< I/O port A reset */
            __IO uint32_t IOPBRST:1;//                ((uint16_t)0x0008)            /*!< IO port B reset */
            __IO uint32_t IOPCRST:1;//                ((uint16_t)0x0010)            /*!< IO port C reset */
            __IO uint32_t IOPDRST:1;//                ((uint16_t)0x0020)            /*!< IO port D reset */
            __IO uint32_t IOPERST:1;//                ((uint16_t)0x0040)            /*!< IO port E reset */
            __IO uint32_t IOPFRST:1;//                ((uint16_t)0x0080)            /*!< IO port F reset */
            __IO uint32_t IOPGRST:1;//                ((uint16_t)0x0100)            /*!< IO port G reset */
            __IO uint32_t ADC1RST:1;//                ((uint16_t)0x0200)            /*!< ADC 1 interface reset */
            __IO uint32_t ADC2RST:1;//                ((uint16_t)0x0400)            /*!< ADC 2 interface reset */
            __IO uint32_t TIM1RST:1;//                ((uint16_t)0x0800)            /*!< TIM1 Timer reset */
            __IO uint32_t SPI1RST:1;//                ((uint16_t)0x1000)            /*!< SPI 1 reset */
            __IO uint32_t TIM8RST:1;//                ((uint16_t)0x2000)            /*!< TIM8 Timer reset */
            __IO uint32_t USART1RST:1;//              ((uint16_t)0x4000)            /*!< USART1 reset */
            __IO uint32_t ADC3RST:1;//                ((uint16_t)0x8000)            /*!< ADC3 interface reset */
            __IO uint32_t Reserved31_16:16;
        }BitAPB2RSTR;
    };
    union
    {
        __IO uint32_t APB1RSTR;
        struct
        {
            __IO uint32_t TIM2RST:1;//                ((uint32_t)0x00000001)        /*!< Timer 2 reset */
            __IO uint32_t TIM3RST:1;//                ((uint32_t)0x00000002)        /*!< Timer 3 reset */
            __IO uint32_t TIM4RST:1;//                ((uint32_t)0x00000004)        /*!< Timer 4 reset */
            __IO uint32_t TIM5RST:1;//                ((uint32_t)0x00000008)        /*!< Timer 5 reset */
            __IO uint32_t TIM6RST:1;//                ((uint32_t)0x00000010)        /*!< Timer 6 reset */
            __IO uint32_t TIM7RST:1;//                ((uint32_t)0x00000020)        /*!< Timer 7 reset */
            __IO uint32_t Reserved10_6:5;
            __IO uint32_t WWDGRST:1;//                ((uint32_t)0x00000800)        /*!< Window Watchdog reset */
            __IO uint32_t Reserved13_12:2;
            __IO uint32_t SPI2RST:1;//                ((uint32_t)0x00004000)        /*!< SPI 2 reset */
            __IO uint32_t SPI3RST:1;//                ((uint32_t)0x00008000)        /*!< SPI 3 reset */
            __IO uint32_t Reserved16:1;
            __IO uint32_t USART2RST:1;//              ((uint32_t)0x00020000)        /*!< USART 2 reset */
            __IO uint32_t USART3RST:1;//              ((uint32_t)0x00040000)        /*!< RUSART 3 reset */
            __IO uint32_t UART4RST:1;//               ((uint32_t)0x00080000)        /*!< USART 4 reset */
            __IO uint32_t UART5RST:1;//               ((uint32_t)0x00100000)        /*!< USART 5 reset */
            __IO uint32_t I2C1RST:1;//                ((uint32_t)0x00200000)        /*!< I2C 1 reset */
            __IO uint32_t I2C2RST:1;//                ((uint32_t)0x00400000)        /*!< I2C 2 reset */
            __IO uint32_t USBRST:1;//                 ((uint32_t)0x00800000)        /*!< USB reset */
            __IO uint32_t Reserved24:1;
            __IO uint32_t CANRST:1;//                 ((uint32_t)0x02000000)        /*!< CAN reset */
            __IO uint32_t Reserved26:1;
            __IO uint32_t BKPRST:1;//                 ((uint32_t)0x08000000)        /*!< Backup interface reset */
            __IO uint32_t PWRRST:1;//                 ((uint32_t)0x10000000)        /*!< Power interface reset */
            __IO uint32_t DACRST:1;//                 ((uint32_t)0x20000000)        /*!< DAC interface reset */
            __IO uint32_t Reserved31_30:2;
        }BitAPB1RSTR;
    };
    union
    {
        __IO uint32_t AHBENR;
        struct
        {
            __IO uint32_t DMA1EN:1;//                   ((uint16_t)0x0001)            /*!< DMA1 clock enable */
            __IO uint32_t DMA2EN:1;//                   ((uint16_t)0x0002)            /*!< DMA2 clock enable */
            __IO uint32_t SRAMEN:1;//                   ((uint16_t)0x0004)            /*!< SRAM interface clock enable */
            __IO uint32_t Reserved3:1;
            __IO uint32_t FLITFEN:1;//                  ((uint16_t)0x0010)            /*!< FLITF clock enable */
            __IO uint32_t Reserved5:1;
            __IO uint32_t CRCEN:1;//                    ((uint16_t)0x0040)            /*!< CRC clock enable */
            __IO uint32_t Reserved7:1;
            __IO uint32_t FSMCEN:1;//                   ((uint16_t)0x0100)            /*!< FSMC clock enable */
            __IO uint32_t Reserved9:1;
            __IO uint32_t SDIOEN:1;//                   ((uint16_t)0x0400)            /*!< SDIO clock enable */
            __IO uint32_t Reserved31_11:21;
        }BitAHBENR;
    };
    union
    {
        __IO uint32_t APB2ENR;
        struct
        {
            __IO uint32_t AFIOEN:1;//                  ((uint16_t)0x0001)            /*!< Alternate Function I/O clock enable */
            __IO uint32_t Reserved1:1;
            __IO uint32_t IOPAEN:1;//                  ((uint16_t)0x0004)            /*!< I/O port A clock enable */
            __IO uint32_t IOPBEN:1;//                  ((uint16_t)0x0008)            /*!< I/O port B clock enable */
            __IO uint32_t IOPCEN:1;//                  ((uint16_t)0x0010)            /*!< I/O port C clock enable */
            __IO uint32_t IOPDEN:1;//                  ((uint16_t)0x0020)            /*!< I/O port D clock enable */
            __IO uint32_t IOPEEN:1;//                  ((uint16_t)0x0040)            /*!< I/O port E clock enable */
            __IO uint32_t IOPFEN:1;//                  ((uint16_t)0x0080)            /*!< I/O port F clock enable */
            __IO uint32_t IOPGEN:1;//                  ((uint16_t)0x0100)            /*!< I/O port G clock enable */
            __IO uint32_t ADC1EN:1;//                  ((uint16_t)0x0200)            /*!< ADC 1 interface clock enable */
            __IO uint32_t ADC2EN:1;//                  ((uint16_t)0x0400)            /*!< ADC 2 interface clock enable */
            __IO uint32_t TIM1EN:1;//                  ((uint16_t)0x0800)            /*!< TIM1 Timer clock enable */
            __IO uint32_t SPI1EN:1;//                  ((uint16_t)0x1000)            /*!< SPI 1 clock enable */
            __IO uint32_t TIM8EN:1;//                  ((uint16_t)0x2000)            /*!< TIM8 Timer clock enable */
            __IO uint32_t USART1EN:1;//                ((uint16_t)0x4000)            /*!< USART1 clock enable */
            __IO uint32_t ADC3EN:1;//                  ((uint16_t)0x8000)            /*!< DMA1 clock enable */
            __IO uint32_t Reserved31_16:16;
        }BitAPB2ENR;
    };
    union
    {
        __IO uint32_t APB1ENR;
        struct
        {
            __IO uint32_t TIM2EN:1;//                  ((uint32_t)0x00000001)        /*!< Timer 2 clock enabled*/
            __IO uint32_t TIM3EN:1;//                  ((uint32_t)0x00000002)        /*!< Timer 3 clock enable */
            __IO uint32_t TIM4EN:1;//                  ((uint32_t)0x00000004)        /*!< Timer 4 clock enable */
            __IO uint32_t TIM5EN:1;//                  ((uint32_t)0x00000008)        /*!< Timer 5 clock enable */
            __IO uint32_t TIM6EN:1;//                  ((uint32_t)0x00000010)        /*!< Timer 6 clock enable */
            __IO uint32_t TIM7EN:1;//                  ((uint32_t)0x00000020)        /*!< Timer 7 clock enable */
            __IO uint32_t Reserved10_6:5;
            __IO uint32_t WWDGEN:1;//                  ((uint32_t)0x00000800)        /*!< Window Watchdog clock enable */
            __IO uint32_t Reserved13_12:2;
            __IO uint32_t SPI2EN:1;//                  ((uint32_t)0x00004000)        /*!< SPI 2 clock enable */
            __IO uint32_t SPI3EN:1;//                  ((uint32_t)0x00008000)        /*!< SPI 3 clock enable */
            __IO uint32_t Reserved16:1;
            __IO uint32_t USART2EN:1;//                ((uint32_t)0x00020000)        /*!< USART 2 clock enable */
            __IO uint32_t USART3EN:1;//                ((uint32_t)0x00040000)        /*!< USART 3 clock enable */
            __IO uint32_t UART4EN:1;//                 ((uint32_t)0x00080000)        /*!< USART 4 clock enable */
            __IO uint32_t UART5EN:1;//                 ((uint32_t)0x00100000)        /*!< USART 5 clock enable */
            __IO uint32_t I2C1EN:1;//                  ((uint32_t)0x00200000)        /*!< I2C 1 clock enable */
            __IO uint32_t I2C2EN:1;//                  ((uint32_t)0x00400000)        /*!< I2C 2 clock enable */
            __IO uint32_t USBEN:1;//                   ((uint32_t)0x00800000)        /*!< USB clock enable */
            __IO uint32_t Reserved24:1;
            __IO uint32_t CANEN:1;//                   ((uint32_t)0x02000000)        /*!< CAN clock enable */
            __IO uint32_t Reserved26:1;
            __IO uint32_t BKPEN:1;//                   ((uint32_t)0x08000000)        /*!< Backup interface clock enable */
            __IO uint32_t PWREN:1;//                   ((uint32_t)0x10000000)        /*!< Power interface clock enable */
            __IO uint32_t DACEN:1;//                   ((uint32_t)0x20000000)        /*!< DAC interface clock enable */
            __IO uint32_t Reserved31_30:2;
        }BitAPB1ENR;
    };

    union
    {
        __IO uint32_t BDCR;
        struct
        {
            __IO uint32_t LSEON:1;//                       ((uint32_t)0x00000001)        /*!< Internal Low Speed oscillator enable */
            __IO uint32_t LSERDY:1;//                      ((uint32_t)0x00000002)        /*!< Internal Low Speed oscillator Ready */
            __IO uint32_t LSEBYP:1;
            __IO uint32_t Reserved7_3:5;
            __IO uint32_t RTCSEL:2;//                        ((uint32_t)0x01000000)        /*!< Remove reset flag */
            __IO uint32_t Reserved14_10:5;
            __IO uint32_t RTCEN:1;//                     ((uint32_t)0x04000000)        /*!< PIN reset flag */
            __IO uint32_t BDRST:1;//                     ((uint32_t)0x08000000)        /*!< POR/PDR reset flag */
            __IO uint32_t Reserved31_17:15;
        }BitBDCR;
    };

    union
    {
        __IO uint32_t CSR;
        struct
        {
            __IO uint32_t LSION:1;//                       ((uint32_t)0x00000001)        /*!< Internal Low Speed oscillator enable */
            __IO uint32_t LSIRDY:1;//                      ((uint32_t)0x00000002)        /*!< Internal Low Speed oscillator Ready */
            __IO uint32_t Reserved23_2:22;
            __IO uint32_t RMVF:1;//                        ((uint32_t)0x01000000)        /*!< Remove reset flag */
            __IO uint32_t Reserved25:1;
            __IO uint32_t PINRSTF:1;//                     ((uint32_t)0x04000000)        /*!< PIN reset flag */
            __IO uint32_t PORRSTF:1;//                     ((uint32_t)0x08000000)        /*!< POR/PDR reset flag */
            __IO uint32_t SFTRSTF:1;//                     ((uint32_t)0x10000000)        /*!< Software Reset flag */
            __IO uint32_t IWDGRSTF:1;//                    ((uint32_t)0x20000000)        /*!< Independent Watchdog reset flag */
            __IO uint32_t WWDGRSTF:1;//                    ((uint32_t)0x40000000)        /*!< Window watchdog reset flag */
            __IO uint32_t LPWRRSTF:1;//                    ((uint32_t)0x80000000)        /*!< Low-Power reset flag */
        }BitCSR;
    };
} RCC_TypeDef;

/**
* @brief Real-Time Clock
*/

typedef struct
{
    //__IO uint16_t CRH;
    union {
        __IO uint16_t CRH;
        struct {
            __IO uint16_t SECIE:1;
            __IO uint16_t ALRIE:1;
            __IO uint16_t OWIE:1;
            __IO uint16_t Res15_3:13;
        }BitCRH;
    };
    uint16_t  RESERVED0;
    //__IO uint16_t CRL;

    union {
        __IO uint16_t CRL;
        struct {
            __IO uint16_t SECF:1;
            __IO uint16_t ALRF:1;
            __IO uint16_t OWF:1;
            __IO uint16_t RSF:1;
            __IO uint16_t CNF:1;
            __IO uint16_t RTOFF:1;
            __IO uint16_t Res15_6:10;
        }BitCRL;
    };

    uint16_t  RESERVED1;
    __IO uint16_t PRLH;
    uint16_t  RESERVED2;
    __IO uint16_t PRLL;
    uint16_t  RESERVED3;
    __IO uint16_t DIVH;
    uint16_t  RESERVED4;
    __IO uint16_t DIVL;
    uint16_t  RESERVED5;
    __IO uint16_t CNTH;
    uint16_t  RESERVED6;
    __IO uint16_t CNTL;
    uint16_t  RESERVED7;
    __IO uint16_t ALRH;
    uint16_t  RESERVED8;
    __IO uint16_t ALRL;
    uint16_t  RESERVED9;
} RTC_TypeDef;

/**
* @brief SD host Interface
*/

typedef struct
{
    __IO uint32_t POWER;
    __IO uint32_t CLKCR;
    __IO uint32_t ARG;
    __IO uint32_t CMD;
    __I uint32_t RESPCMD;
    __I uint32_t RESP1;
    __I uint32_t RESP2;
    __I uint32_t RESP3;
    __I uint32_t RESP4;
    __IO uint32_t DTIMER;
    __IO uint32_t DLEN;
    __IO uint32_t DCTRL;
    __I uint32_t DCOUNT;
    __I uint32_t STA;
    __IO uint32_t ICR;
    __IO uint32_t MASK;
    uint32_t  RESERVED0[2];
    __I uint32_t FIFOCNT;
    uint32_t  RESERVED1[13];
    __IO uint32_t FIFO;
} SDIO_TypeDef;

/**
* @brief Serial Peripheral Interface
*/

typedef struct
{
    union
    {
        __IO uint16_t CR1;
        struct
        {
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
    uint16_t  RESERVED0;
    union
    {
        __IO uint16_t CR2;
        struct
        {
            __IO uint16_t RXDMAEN:1;//                     ((uint8_t)0x01)               /*!<Rx Buffer DMA Enable */
            __IO uint16_t TXDMAEN:1;//                     ((uint8_t)0x02)               /*!<Tx Buffer DMA Enable */
            __IO uint16_t SSOE:1;//                        ((uint8_t)0x04)               /*!<SS Output Enable */
            __IO uint16_t Reserved4_3:2;
            __IO uint16_t ERRIE:1;//                       ((uint8_t)0x20)               /*!<Error Interrupt Enable */
            __IO uint16_t RXNEIE:1;//                      ((uint8_t)0x40)               /*!<RX buffer Not Empty Interrupt Enable */
            __IO uint16_t TXEIE:1;//                       ((uint8_t)0x80)               /*!<Tx buffer Empty Interrupt Enable */
            __IO uint16_t Reserved15_8:8;
        }BitCR2;
    };
    uint16_t  RESERVED1;
    union
    {
        __IO uint16_t SR;
        struct
        {
            __IO uint16_t RXNE:1;//                         ((uint8_t)0x01)               /*!<Receive buffer Not Empty */
            __IO uint16_t TXE:1;//                          ((uint8_t)0x02)               /*!<Transmit buffer Empty */
            __IO uint16_t CHSIDE:1;//                       ((uint8_t)0x04)               /*!<Channel side */
            __IO uint16_t UDR:1;//                          ((uint8_t)0x08)               /*!<Underrun flag */
            __IO uint16_t CRCERR:1;//                       ((uint8_t)0x10)               /*!<CRC Error flag */
            __IO uint16_t MODF:1;//                         ((uint8_t)0x20)               /*!<Mode fault */
            __IO uint16_t OVR:1;//                          ((uint8_t)0x40)               /*!<Overrun flag */
            __IO uint16_t BSY:1;//                          ((uint8_t)0x80)               /*!<Busy flag */
            __IO uint16_t Reserved15_8:8;
        }BitSR;
    };
    uint16_t  RESERVED2;
    __IO uint16_t DR;
    uint16_t  RESERVED3;
    __IO uint16_t CRCPR;
    uint16_t  RESERVED4;
    __IO uint16_t RXCRCR;
    uint16_t  RESERVED5;
    __IO uint16_t TXCRCR;
    uint16_t  RESERVED6;
    __IO uint16_t I2SCFGR;
    uint16_t  RESERVED7;
    __IO uint16_t I2SPR;
    uint16_t  RESERVED8;
} SPI_TypeDef;

/**
* @brief TIM
*/

typedef struct
{
    union
    {
        __IO uint16_t CR1;
        struct
        {
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
    uint16_t  RESERVED0;
    union
    {
        __IO uint16_t CR2;
        struct
        {
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
    uint16_t  RESERVED1;
    union
    {
        __IO uint16_t SMCR;
        struct
        {
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
    uint16_t  RESERVED2;
    union
    {
        __IO uint16_t DIER;
        struct
        {
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
    uint16_t  RESERVED3;
    union
    {
        __IO uint16_t SR;
        struct
        {
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
    uint16_t  RESERVED4;
    union
    {
        __IO uint16_t EGR;
        struct
        {
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
    uint16_t  RESERVED5;
    union
    {
        __IO uint16_t CCMR1;
        struct
        {
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
        struct
        {
            __IO uint16_t IC1S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC1PSC:2;//                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
            __IO uint16_t IC1F:4;//                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
            __IO uint16_t IC2S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC2PSC:1;//                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
            __IO uint16_t IC2F:4;//                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
        }BitIcCCMR1;
    };
    uint16_t  RESERVED6;
    union
    {
        __IO uint16_t CCMR2;
        struct
        {
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
        struct
        {
            __IO uint16_t IC3S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC3PSC:2;//                    ((uint16_t)0x000C)            /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
            __IO uint16_t IC3F:4;//                      ((uint16_t)0x00F0)            /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
            __IO uint16_t IC4S:2;//                      ((uint16_t)0x0003)            /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
            __IO uint16_t IC4PSC:1;//                    ((uint16_t)0x0C00)            /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
            __IO uint16_t IC4F:4;//                      ((uint16_t)0xF000)            /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
        }BitIcCCMR2;
    };
    uint16_t  RESERVED7;
    union
    {
        __IO uint16_t CCER;
        struct
        {
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
    uint16_t  RESERVED8;
    __IO uint16_t CNT;
    uint16_t  RESERVED9;
    __IO uint16_t PSC;
    uint16_t  RESERVED10;
    __IO uint16_t ARR;
    uint16_t  RESERVED11;
    __IO uint16_t RCR;
    uint16_t  RESERVED12;
    __IO uint16_t CCR1;
    uint16_t  RESERVED13;
    __IO uint16_t CCR2;
    uint16_t  RESERVED14;
    __IO uint16_t CCR3;
    uint16_t  RESERVED15;
    __IO uint16_t CCR4;
    uint16_t  RESERVED16;
    union
    {
        __IO uint16_t BDTR;
        struct
        {
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
    uint16_t  RESERVED17;
    union
    {
        __IO uint16_t DCR;
        struct
        {
            __IO uint16_t DBA:5;//                         ((uint16_t)0x001F)            /*!<DBA[4:0] bits (DMA Base Address) */
            __IO uint16_t Reserved5_7:3;
            __IO uint16_t DBL:5;//                         ((uint16_t)0x1F00)            /*!<DBL[4:0] bits (DMA Burst Length) */
            __IO uint16_t Reserved13_15:3;
        }BitDCR;
    };
    uint16_t  RESERVED18;
    __IO uint16_t DMAR;
    uint16_t  RESERVED19;
} TIM_TypeDef;

/**
* @brief Universal Synchronous Asynchronous Receiver Transmitter
*/

typedef struct
{
    union
    {
        __IO uint16_t SR;
        struct
        {
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
    uint16_t  RESERVED0;
    __IO uint16_t DR;
    uint16_t  RESERVED1;
    __IO uint16_t BRR;
    uint16_t  RESERVED2;
    union
    {
        __IO uint16_t CR1;
        struct
        {
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
    uint16_t  RESERVED3;
    union
    {
        __IO uint16_t CR2;
        struct
        {
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
    uint16_t  RESERVED4;
    union
    {
        __IO uint16_t CR3;
        struct
        {
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
            __IO uint16_t Reserved15_11:5;
        }BitCR3;
    };
    uint16_t  RESERVED5;
    union
    {
        __IO uint16_t GTPR;
        struct
        {
            __IO uint16_t PSC:8;//                      ((uint16_t)0x00FF)            /*!<PSC[7:0] bits (Prescaler value) */
            __IO uint16_t GT:8;//                       ((uint16_t)0xFF00)            /*!<Guard time value */
        }BitGTPR;
    };
    uint16_t  RESERVED6;
} USART_TypeDef;

/**
* @brief Window WATCHDOG
*/

typedef struct
{
    __IO uint32_t CR;
    __IO uint32_t CFR;
    __IO uint32_t SR;
} WWDG_TypeDef;

/**
* @}
*/

/** @addtogroup Peripheral_memory_map
* @{
*/

#define PERIPH_BB_BASE        ((uint32_t)0x42000000) /*!< Peripheral base address in the alias region */
#define SRAM_BB_BASE          ((uint32_t)0x22000000) /*!< SRAM base address in the alias region */

#define SRAM_BASE             ((uint32_t)0x20000000) /*!< Peripheral base address in the bit-band region */
#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< SRAM base address in the bit-band region */

#define FSMC_R_BASE           ((uint32_t)0xA0000000) /*!< FSMC registers base address */

/*!< Peripheral memory map */
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400)
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
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400)
#define BKP_BASE              (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400)

#define AFIO_BASE             (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE            (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE            (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE            (APB2PERIPH_BASE + 0x1800)
#define GPIOF_BASE            (APB2PERIPH_BASE + 0x1C00)
#define GPIOG_BASE            (APB2PERIPH_BASE + 0x2000)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE             (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x3400)
#define USART1_BASE           (APB2PERIPH_BASE + 0x3800)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x3C00)

#define SDIO_BASE             (PERIPH_BASE + 0x18000)

#define DMA1_BASE             (AHBPERIPH_BASE + 0x0000)
#define DMA1_Channel1_BASE    (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel2_BASE    (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel3_BASE    (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel4_BASE    (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel5_BASE    (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel6_BASE    (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel7_BASE    (AHBPERIPH_BASE + 0x0080)
#define DMA2_BASE             (AHBPERIPH_BASE + 0x0400)
#define DMA2_Channel1_BASE    (AHBPERIPH_BASE + 0x0408)
#define DMA2_Channel2_BASE    (AHBPERIPH_BASE + 0x041C)
#define DMA2_Channel3_BASE    (AHBPERIPH_BASE + 0x0430)
#define DMA2_Channel4_BASE    (AHBPERIPH_BASE + 0x0444)
#define DMA2_Channel5_BASE    (AHBPERIPH_BASE + 0x0458)
#define RCC_BASE              (AHBPERIPH_BASE + 0x1000)
#define CRC_BASE              (AHBPERIPH_BASE + 0x3000)

#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x2000) /*!< Flash registers base address */
#define OB_BASE               ((uint32_t)0x1FFFF800)    /*!< Flash Option Bytes base address */

#define FSMC_Bank1_R_BASE     (FSMC_R_BASE + 0x0000) /*!< FSMC Bank1 registers base address */
#define FSMC_Bank1E_R_BASE    (FSMC_R_BASE + 0x0104) /*!< FSMC Bank1E registers base address */
#define FSMC_Bank2_R_BASE     (FSMC_R_BASE + 0x0060) /*!< FSMC Bank2 registers base address */
#define FSMC_Bank3_R_BASE     (FSMC_R_BASE + 0x0080) /*!< FSMC Bank3 registers base address */
#define FSMC_Bank4_R_BASE     (FSMC_R_BASE + 0x00A0) /*!< FSMC Bank4 registers base address */

#define DBGMCU_BASE          ((uint32_t)0xE0042000) /*!< Debug MCU registers base address */

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
#define CAN1                ((CAN_TypeDef *) CAN1_BASE)
#define BKP                 ((BKP_TypeDef *) BKP_BASE)
#define PWR                 ((PWR_TypeDef *) PWR_BASE)
#define DAC                 ((DAC_TypeDef *) DAC_BASE)
#define AFIO                ((AFIO_TypeDef *) AFIO_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOG               ((GPIO_TypeDef *) GPIOG_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)
#define ADC2                ((ADC_TypeDef *) ADC2_BASE)
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define TIM8                ((TIM_TypeDef *) TIM8_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define ADC3                ((ADC_TypeDef *) ADC3_BASE)
#define SDIO                ((SDIO_TypeDef *) SDIO_BASE)
#define DMA1                ((DMA_TypeDef *) DMA1_BASE)
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)
#define DMA1_Channel1       ((DMA_Channel_TypeDef *) DMA1_Channel1_BASE)
#define DMA1_Channel2       ((DMA_Channel_TypeDef *) DMA1_Channel2_BASE)
#define DMA1_Channel3       ((DMA_Channel_TypeDef *) DMA1_Channel3_BASE)
#define DMA1_Channel4       ((DMA_Channel_TypeDef *) DMA1_Channel4_BASE)
#define DMA1_Channel5       ((DMA_Channel_TypeDef *) DMA1_Channel5_BASE)
#define DMA1_Channel6       ((DMA_Channel_TypeDef *) DMA1_Channel6_BASE)
#define DMA1_Channel7       ((DMA_Channel_TypeDef *) DMA1_Channel7_BASE)
#define DMA2_Channel1       ((DMA_Channel_TypeDef *) DMA2_Channel1_BASE)
#define DMA2_Channel2       ((DMA_Channel_TypeDef *) DMA2_Channel2_BASE)
#define DMA2_Channel3       ((DMA_Channel_TypeDef *) DMA2_Channel3_BASE)
#define DMA2_Channel4       ((DMA_Channel_TypeDef *) DMA2_Channel4_BASE)
#define DMA2_Channel5       ((DMA_Channel_TypeDef *) DMA2_Channel5_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define CRC                 ((CRC_TypeDef *) CRC_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define OB                  ((OB_TypeDef *) OB_BASE)
#define FSMC_Bank1          ((FSMC_Bank1_TypeDef *) FSMC_Bank1_R_BASE)
#define FSMC_Bank1E         ((FSMC_Bank1E_TypeDef *) FSMC_Bank1E_R_BASE)
#define FSMC_Bank2          ((FSMC_Bank2_TypeDef *) FSMC_Bank2_R_BASE)
#define FSMC_Bank3          ((FSMC_Bank3_TypeDef *) FSMC_Bank3_R_BASE)
#define FSMC_Bank4          ((FSMC_Bank4_TypeDef *) FSMC_Bank4_R_BASE)
#define DBGMCU              ((DBGMCU_TypeDef *) DBGMCU_BASE)

/**
* @}
*/

/**
* @}
*/

#ifdef USE_STDPERIPH_DRIVER
#include "stm32f10x_conf.h"
#endif

/** @addtogroup Exported_macro
* @{
*/

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~CLEARMASK)) | (SETMASK)))

#define Stm32BitClr(x, y)		*(volatile unsigned int *)((x & 0xf0000000) + 0x2000000 + ((x & 0xfffff) << 5) + (y << 2)) = 0x00
#define Stm32BitSet(x, y)		*(volatile unsigned int *)((x & 0xf0000000) + 0x2000000 + ((x & 0xfffff) << 5) + (y << 2)) = 0x01
#define Stm32BitGet(x, y)	        (*(volatile unsigned int *)((x & 0xf0000000) + 0x2000000 + ((x & 0xfffff) << 5) + (y << 2)))

#define RawReadC(addr)		        (*(volatile unsigned char *)addr)
#define RawWriteC(addr, val)	        *(volatile unsigned char *)(addr) = val;

#define RawReadS(addr)		        (*(volatile unsigned short *)addr)
#define RawWriteS(addr, val)	        *(volatile unsigned short *)(addr) = val;

#define RawReadL(addr)		        (*(volatile unsigned int *)addr)
#define RawRriteL(addr, val)	        *(volatile unsigned int *)(addr) = val;

/**
* @}
*/

#endif /* __STM32F10x_H */

/**
* @}
*/

/**
* @}
*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
