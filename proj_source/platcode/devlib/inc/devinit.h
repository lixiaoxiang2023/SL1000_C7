/*******************************************************************************
文件名称：    devinit.h
文件目录：    $PROJ_DIR$/usr/Drive/inc
项目名称：    事业一部通用技术平台
创建者：      胥强
创建日期：
程序用途：    初始化平台的所有硬件驱动模块

版权声明： Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __DEVINIT_H
#define __DEVINIT_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

/*******************************************************************************
功能定义
*******************************************************************************/

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
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
extern void At45MemInit(void);
extern void Motor3983Init(void);
extern void MotorTHB6128Init(void);
extern void UartGpioInit ( void );
extern void Cat3626LedInit(void);
extern void Mcp3202AdcInit(void);
extern void Uln2003MoInit(void);
extern void Ad9822AdcInit(void);
extern void XcanTrsInit(void);
extern void PrinterDevInit(void);
extern void ParaPortInit(void);
extern void Ps2PortInit(void);
extern void PrintePortInit(void);
extern void UartLcdPortInit(void);
extern void Ap3156LedInit(void);
extern void BeepSondInit(void);
extern void Ds18b20TmpInit(void);
extern void Tcs230ColorInit(void);
extern void Ads7843AdcInit(void);
extern void Enc28NetInit(unsigned char *pbyMac);
extern void WifiNetInit(void);
extern void BottonsKeyInit(void);
extern void Bti412ComInit();
extern void ProtBtComInit(void);
extern void PWMInit(void);
extern void IPS1704LRamInit(void);
extern void MotorInt(void);
extern void CISInit(void);
extern void Uart6GpioInit ( void );
extern void StripScan(void);
extern void LcdInit(void);
extern void Ds18b20PortInit(void);
extern void TemperatureCtrlInit(void);
extern void Ch9350Init(void);
extern void PwrKeyInit ( void );
extern void Esp32PortInit(void);
extern void KB1281PortInit(void);
extern void SnapSwPortInit(void);
extern void ThermistorInit(void);
#endif