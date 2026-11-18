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
#ifndef __DEVPLAT_H
#define __DEVPLAT_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "dev.h"

/*******************************************************************************
功能定义
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

#if(CONFIG_FLASH)
    #include "StDriverCfg.h"
#endif
#if(CONFIG_A3983)
    #include "a3983.h"
    extern const DevA3983Tdf *DevA3983;
#endif
#if(CONFIG_THB6128)
    #include "THB6128.h"
    extern const DevTHB6128Tdf *DevTHB6128;
#endif
#if(CONFIG_AT45)
    #include "at45.h"
    extern const DevAt45Tdf *DevAt45;
#endif

#if(CONFIG_CAT3626)
    #include "cat3626.h"
    extern const DevCat3626Tdf *DevCat3626;
#endif
#if(CONFIG_MCP3202)
    #include "mcp3202.h"
    extern const DevMcp3202Tdf *DevMcp3202;
#endif
#if(CONFIG_ULN2003)
    #include "uln2003.h"
    extern const DevUln2003Tdf *DevUln2003;
#endif
#if(CONFIG_XCAN)
    #include "xcan.h"
    extern const DevXcanTdf *DevXcan;
#endif
#if(CONFIG_AD9822)
#include "ad9822.h"
    extern const DevAd9822Tdf *DevAd9822;
#endif
#if(CONFIG_AP3156)
    #include "ap3156.h"
    extern const DevAp3156Tdf *DevAp3156;
#endif
#if(CONFIG_PARA)
    #include "parallel.h"
    extern const DevParaTdf *DevPara;
#endif
#if(CONFIG_BEEP)
    #include "beep.h"
    extern const DevBeepTdf *DevBeep;
#endif
#if(CONFIG_PS2)
    #include "ps2.h"
    extern const DevPs2Tdf *DevPs2;
#endif
#if(CONFIG_PRINTER)
    #include "printer.h"
    extern const DevPrinterTdf *DevPrinter;
#endif
#if(CONFIG_UART_LCD)
    #include "uartlcd.h"
    extern const DevUartLcdTdf *DevUartLcd;
#endif
#if(CONFIG_DS18B20)
    #include "ds18b20.h"
    extern const DevDs18b20Tdf *DevDs18b20;
#endif
#if(CONFIG_TCS230)
    #include "tcs230.h"
    extern const DevTcs230Tdf *DevTcs230;
#endif
#if(CONFIG_ADS7843)
    #include "ads7843.h"
    extern const DevAds7843Tdf *DevAds7843;
#endif
#if(CONFIG_ENC28)
    #include "enc28.h"
    extern const DevEnc28Tdf *DevEnc28;
#endif
#if(CONFIG_WIFI)
    #include "wifi.h"
    extern const DevWifiTdf *DevWifi;
#endif
#if(CONFIG_MPR03X)
    #include "mpr03x.h"
    extern const DevMpr03xTdf *DevMpr03x;
#endif
#if(CONFIG_BUTTONS)
    #include "buttons.h"
    extern const DevButtonTdf *DevButton;
#endif
#if(CONFIG_BT_HW6)
    #include "bt-hw6.h"
    extern const DevBtTdf *DevBt;
#endif
#if(CONFIG_BT_I412)
    #include "bt-i412.h"
    extern const DevBti412Tdf *DevBt;
#endif
#if(CONFIG_COM_PROTOCOL)
    #include "protcom.h"
    extern const ProtComTdf *ProtCom;
#endif
#if(CONFIG_LAN8720)
    #include "lan8720.h"
    extern const DevLan8720Tdf *DevLan8720;
#endif
#if(CONFIG_PWM)
    #include "pwm.h"
    extern const DevPwmTdf *DevPwm;
#endif

#if(CONFIG_IPS1704L)
#include "IPS1704L.h"
    extern const DevIPS1704LTdf *DevIPS1704;
#endif

#if(CONFIG_LCD7)
#include "lcd7.h"
    extern const DevLcd7Tdf *DevLcd7;
#endif

#if(CONFIG_ESP32)
    #include "esp32.h"
    extern const DevEsp32Tdf *DevEsp32;
#endif
#if(CONFIG_TEMPCTRL)
#include "Temperature.h"
    extern const DevTempType *DevTempCtrl;
#endif

#if(CONFIG_CH9350)
#include "ch9350.h"
    extern const DevCh9350Tdf *DevCh9350;
#endif

#if(CONFIG_KB1281)
#include "KB1281.h"
    extern const DevKB1281Tdf *DevKB1281;
#endif

#if(CONFIG_THERMISTOR)
#include "thermistor.h"
    extern const DevThermistorTdf *DevThermistor;
#endif

#if(CONFIG_SNAPSW)
#include "snapsw.h"
    extern const SnapSwTdf *DevSnapSw;
#endif
/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
void DevPlatPutMutex(unsigned int *pdwMutex);
bool DevPlatGetMutex(unsigned int *pdwMutex, bool blWaitLock);

#endif