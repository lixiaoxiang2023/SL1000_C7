/*******************************************************************************
文件名称： esp32.h
目录：
项目名称：UC-58
创建者：  mdj
创建日期：
程序用途：Esp32驱动中间件

版权声明：
Copyright urit Co.,Ltd.All Right Reserved

修改及维护信息

*******************************************************************************/
#ifndef __ESP32_H
#define __ESP32_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "stdbool.h"
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/


/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

typedef struct
{
    unsigned char byUartSn;
    unsigned int  dwBaud;
}stEsp32InitTdf;

typedef enum
{
    emApConn = 2,
    emTcpConn = 3,
    emConnBreak = 4,
    emNoConn = 5,
}emConsta;



typedef struct {

    bool (*IsDevExist)(void);
    bool (*Restore)(void);
    bool (*WifiInit)(void);
    bool (*WifiConnect)(char *Ssid,char *Pwd);
    void (*WifiDisconnect)(void);
    emConsta (*WifiInqConnState)(void);
    emConsta (*WifiGetConnSta)(void);

    bool (*WifiSetLip) (char * lIp,char *Gateway, char *Netmask);
    void (*WifiGetIp)(char* plbStaAp ,char* pbMac);

    void (*WifiScan)(void);
    unsigned char (*WifiGetScanApNum)(void);
    char* (*WifiGetScanSsid)(int wCnt);
    void (*WifiCleanAp)(void);
    bool (*WifiTcpConn)(char *rIp, int rPort);
    bool (*WifiUdpConn)(char *rIp, int rPort, int udpLPort);
    void (*WifiEstabTrans)(void);
    void (*WifiCmdSend)(char *pbyStr, u16 wLen);

    bool (*BleInit)(void);
    void (*BleSetName)(char* bBleName);
    char* (*BleGetName)(void);
    bool (*BleSetAddr)(char* BleAddr);
    bool (*BleGetAddr)(char* BleAddr);
//    void (*BleSetName)(char* pcName);
    bool (*BleGattsSrvCfg)(void);
    bool (*BleAdv)(char* AdvName);
    bool (*BleCheckConn)(void);
    bool (*BleDisCon)(void);
    bool (*BleSppCfg)(void);
    void (*BleCmdSend)(char *pStr);

    void (*SppSend)(char *pStr);
    void (*StartRx)(void);
    bool (*SleepMode)(bool blSleep);
}DevEsp32Tdf;
/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevEsp32Tdf *Esp32Init(stEsp32InitTdf* pstInit);


#endif