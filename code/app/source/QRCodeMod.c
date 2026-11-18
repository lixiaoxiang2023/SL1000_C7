/******************************************************************************
文件名称：QRCodeMod.c
目录：$PROJ_DIR$/usr/app
项目名称：
创建者：GBS
创建日期：
程序用途：二维码模块驱动


版权声明：
Copyright urit Co.,Ltd.
All Right Reserved



******************************************************************************/

/******************************************************************************
Include file头文件
******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "QRCodeMod.h"
#include "usart.h"
#include "dev.h"
/******************************************************************************
Constant and Macro Definition 常量以及宏定义
******************************************************************************/
#define USARTNo              1     //串口2
#define USARTRXBUFLEN        64
/******************************************************************************
全局变量定义
******************************************************************************/

/******************************************************************************
外部变量定义
******************************************************************************/
uint16_t bDataCount = 0;
uint8_t  bDataBuf[USARTRXBUFLEN];
/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

/******************************************************************************
内部全局变量申明
******************************************************************************/

/******************************************************************************
外部函数申明
******************************************************************************/

/******************************************************************************
内部函数申明
******************************************************************************/
static void TriggerMode(uint8_t Mode);
static void PowerMode(uint8_t Mode);
static void SilenceSet(uint8_t Mode);

/*****************************************************************************
函数名：static void Uart2(void)
函数功能：中断复位函数
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void Uart2(uint8_t bycha)
{
    bDataBuf[bDataCount] = bycha;
    if (bDataCount < USARTRXBUFLEN)
    {
        bDataCount ++ ;
    }
}

/*****************************************************************************
函数名：static void Init(void)
函数功能：初始化
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void Init(void)
{
    DevIo->Config(PD, 5, 1, Mode_AF_PP);
    DevIo->Config(PD, 6, 1, Mode_AF_PP);
    DevIo->RemapConfig(GPIO_Remap_USART2,ENABLE);
    DevUart->Config(1, 115200, 1, 0, true);
    SilenceSet(1);
    DevUart->RegHandler(1, Uart2);
    for(__IO int i = 0; i<0xffff; i++);
    for(__IO int i = 0; i<0xffff; i++);
    TriggerMode(3);
    for(__IO int i = 0; i<0xffff; i++);
    for(__IO int i = 0; i<0xffff; i++);
    PowerMode(0);
}

/*****************************************************************************
函数名：static void PowerMode(uint8_t Mode)
函数功能：供电模式
参数描述:  Mode  0:连续供电  1：低功耗模式
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void PowerMode(uint8_t Mode)
{
    const uint8_t Mode1Data[9] = {0x07,0xC6,0x04,0x08,0x00,0x80,0x00,0xFE,0xA7};
    const uint8_t Mode2Data[9] = {0x07,0xC6,0x04,0x08,0x00,0x80,0x01,0xFE,0xA6};
    if (Mode == 0)
    {
        DevUart->SendString(USARTNo,Mode1Data,sizeof(Mode1Data));
    }
    else if (Mode == 1)
    {
        DevUart->SendString(USARTNo,Mode2Data,sizeof(Mode1Data));
    }
    else
    {
        return ;
    }
}

/*****************************************************************************
函数名：static void TriggerMode(uint8_t Mode)
函数功能：触发模式
参数描述:  Mode  0:手动模式  1：连续扫描 2: 感应模式  3:主机模式
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void TriggerMode(uint8_t Mode)
{
    const uint8_t Mode1Data[9] = {0x07,0xC6,0x04,0x08,0x00,0x8A,0x00,0xFE,0x9D};
    const uint8_t Mode2Data[9] = {0x07,0xC6,0x04,0x08,0x00,0x8A,0x04,0xFE,0x99};
    const uint8_t Mode3Data[9] = {0x07,0xC6,0x04,0x08,0x00,0x8A,0x09,0xFE,0x94};
    const uint8_t Mode4Data[9] = {0x07,0xC6,0x04,0x08,0x00,0x8A,0x08,0xFE,0x95};
    switch (Mode)
    {
    case 0:
        DevUart->SendString(USARTNo, Mode1Data, sizeof(Mode1Data));
        break;
    case 1:
        DevUart->SendString(USARTNo, Mode2Data, sizeof(Mode2Data));
        break;
    case 2:
        DevUart->SendString(USARTNo, Mode3Data, sizeof(Mode3Data));
        break;
    case 3:
        DevUart->SendString(USARTNo, Mode4Data, sizeof(Mode4Data));
        break;
    default :
        break;
    }
}

/*****************************************************************************
函数名：static void SilenceSet(uint8_t Mode)
函数功能：静音
参数描述:  Mode  0:禁止  1：使能
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void SilenceSet(uint8_t Mode)
{
    const uint8_t Mode1Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x0C,0X00,0xFE,0x28};
    const uint8_t Mode2Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x0C,0x01,0xFE,0x27};
    switch (Mode)
    {
    case 0:
        DevUart->SendString(USARTNo, Mode1Data, sizeof(Mode1Data));
        break;
    case 1:
        DevUart->SendString(USARTNo, Mode2Data, sizeof(Mode2Data));
        break;
    default :
        break;
    }
}

/*****************************************************************************
函数名：static void PowerOnPromptSet(uint8_t Mode)
函数功能：开机提示声
参数描述:  Mode  0:禁止  1：使能
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void PowerOnPromptSet(uint8_t Mode)
{
    const uint8_t Mode1Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x0D,0X00,0xFE,0x27};
    const uint8_t Mode2Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x0D,0x01,0xFE,0x26};
    switch (Mode)
    {
    case 0:
        DevUart->SendString(USARTNo, Mode1Data, sizeof(Mode1Data));
        break;
    case 1:
        DevUart->SendString(USARTNo, Mode2Data, sizeof(Mode2Data));
        break;
    default :
        break;
    }
}

/*****************************************************************************
函数名：static void RestoreDefaultConfiguration(void)
函数功能：恢复出厂设置
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void RestoreDefaultConfiguration(void)
{
    const uint8_t Mode1Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0xFF,0X00,0xFD,0x35};
    DevUart->SendString(USARTNo, Mode1Data, sizeof(Mode1Data));
}

/*****************************************************************************
函数名：static void SensitivityLevel(void)
函数功能：敏感等级
参数描述:  Mode  0:高灵敏   1：中灵敏  2：低灵敏
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void SensitivityLevel(uint8_t Mode)
{
    const uint8_t Mode1Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x04,0X01,0xFE,0x2F};
    const uint8_t Mode2Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x04,0X02,0xFE,0x2E};
    const uint8_t Mode3Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF2,0x04,0X03,0xFE,0x2D};
    switch (Mode)
    {
    case 0:
        DevUart->SendString(USARTNo, Mode1Data, sizeof(Mode1Data));
        break;
    case 1:
        DevUart->SendString(USARTNo, Mode2Data, sizeof(Mode2Data));
        break;
    case 2:
        DevUart->SendString(USARTNo, Mode3Data, sizeof(Mode3Data));
        break;
    default :
        break;
    }
}

/*****************************************************************************
函数名：static void StableInductionTime((uint8_t Mode)
函数功能：敏感等级
参数描述:  Mode  0:500MS   1：1000MS  2：300MS
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void StableInductionTime(uint8_t Mode)
{
    const uint8_t Mode1Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF3,0x02,0X05,0xFE,0x2C};
    const uint8_t Mode2Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF3,0x02,0X0A,0xFE,0x27};
    const uint8_t Mode3Data[10] = {0x08,0xC6,0x04,0x08,0x00,0xF3,0x02,0X03,0xFE,0x2E};
    switch(Mode)
    {
    case 0:
        DevUart->SendString(USARTNo, Mode1Data, sizeof(Mode1Data));
        break;
    case 1:
        DevUart->SendString(USARTNo, Mode2Data, sizeof(Mode2Data));
        break;
    case 2:
        DevUart->SendString(USARTNo, Mode3Data, sizeof(Mode3Data));
        break;
    default :
        break;
    }
}

/*****************************************************************************
函数名：static void GetQrCode(uint8_t Mode)
函数功能：
参数描述:
返回值描述：
特殊算法：
注意事项：
******************************************************************************/
static void GetQrCode(uint8_t *pData)
{
    const uint8_t Maxlen = 3;
    const uint8_t Mode1Data[3] = {0x37,0x54,0x0D};
    const uint8_t Frame_Header_Len = 6;
    bDataCount = 0;
    memset(bDataBuf, 0, USARTRXBUFLEN);
    memset(pData, 0, USARTRXBUFLEN);
    DevUart->SendString(USARTNo, Mode1Data, Maxlen);
    vTaskDelay(4000);
    memcpy(pData, bDataBuf+Frame_Header_Len, USARTRXBUFLEN-Frame_Header_Len);
}

const QRCodeModTdf QRCodeMod = {
    Init,
    PowerMode,
    TriggerMode,
    GetQrCode,
    PowerOnPromptSet,
    SilenceSet,
    RestoreDefaultConfiguration,
    SensitivityLevel,
    StableInductionTime,
};


