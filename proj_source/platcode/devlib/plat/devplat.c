/*******************************************************************************
文件名称：   devplat.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20160308
程序用途：   初始化平台的所有硬件驱动模块设备申明

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20160308：第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

/*******************************************************************************
常量定义
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/
#if(CONFIG_A3983)
    const DevA3983Tdf *DevA3983;
#endif
#if(CONFIG_AT45)
    const DevAt45Tdf *DevAt45;
#endif
#if(CONFIG_THB6128)
    const DevTHB6128Tdf *DevTHB6128;
#endif
#if(CONFIG_CAT3626)
    const DevCat3626Tdf *DevCat3626;
#endif
#if(CONFIG_MCP3202)
    const DevMcp3202Tdf *DevMcp3202;
#endif
#if(CONFIG_ULN2003)
    const DevUln2003Tdf *DevUln2003;
#endif
#if(CONFIG_XCAN)
    const DevXcanTdf *DevXcan;
#endif
#if(CONFIG_AD9822)
    const DevAd9822Tdf *DevAd9822;
#endif
#if(CONFIG_AP3156)
    const DevAp3156Tdf *DevAp3156;
#endif
#if(CONFIG_PARA)
    const DevParaTdf *DevPara;
#endif
#if(CONFIG_BEEP)
    const DevBeepTdf *DevBeep;
#endif
#if(CONFIG_PS2)
    const DevPs2Tdf *DevPs2;
#endif
#if(CONFIG_PRINTER)
    const DevPrinterTdf *DevPrinter;
#endif
#if(CONFIG_UART_LCD)
    const DevUartLcdTdf *DevUartLcd;
#endif
#if(CONFIG_DS18B20)
    const DevDs18b20Tdf *DevDs18b20;
#endif
#if(CONFIG_TCS230)
    const DevTcs230Tdf *DevTcs230;
#endif
#if(CONFIG_ADS7843)
    const DevAds7843Tdf *DevAds7843;
#endif
#if(CONFIG_ENC28)
    const DevEnc28Tdf *DevEnc28;
#endif
#if(CONFIG_WIFI)
    const DevWifiTdf *DevWifi;
#endif
#if(CONFIG_BUTTONS)
    const DevButtonTdf *DevButton;
#endif
#if(CONFIG_BT_HW6)
    const DevBtTdf *DevBt;
#endif
#if(CONFIG_BT_I412)
    const DevBti412Tdf *DevBti412;
#endif
#if(CONFIG_COM_PROTOCOL)
    const ProtComTdf *ProtCom;
#endif
#if(CONFIG_LAN8720)
    const DevLan8720Tdf *DevLan8720;
#endif

#if(CONFIG_PWM)
    const DevPwmTdf *DevPwm;
#endif

#if(CONFIG_IPS1704L)
    const DevIPS1704LTdf *DevIPS1704;
#endif

#if(CONFIG_DevLcd7)
    const DevLcd7Tdf *DevLcd7;
#endif

#if(CONFIG_ESP32)
    const DevEsp32Tdf *DevEsp32;
#endif

#if(CONFIG_TEMPCTRL)
    const DevTempType *DevTempCtrl;
#endif

#if(CONFIG_CH9350)
    const DevCh9350Tdf *DevCh9350;
#endif

#if(CONFIG_KB1281)
    const DevKB1281Tdf *DevKB1281;
#endif

#if(CONFIG_THERMISTOR)
    const DevThermistorTdf *DevThermistor;
#endif

#if(CONFIG_SNAPSW)
    const SnapSwTdf *DevSnapSw;
#endif
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
*   @ 使用驱动函数进行加锁
参数描述：
*   @ 无
返回值描述：
*   @ unsigned int *pdwMutex 锁句柄
*   @ bool blWaitLock true等待取得锁，false 检查锁，并立即返回
说明：
*   @ 无
*******************************************************************************/
bool DevPlatGetMutex(unsigned int *pdwMutex, bool blWaitLock)
{
    return DevIo->CommLock(pdwMutex, blWaitLock);
}

/*******************************************************************************
函数功能：
*   @ 释放锁
参数描述：
*   @ 无
返回值描述：
*   @ unsigned int *pdwMutex 锁句柄
说明：
*   @ 无
*******************************************************************************/
void DevPlatPutMutex(unsigned int *pdwMutex)
{
    DevIo->CommUnLock(pdwMutex);
}