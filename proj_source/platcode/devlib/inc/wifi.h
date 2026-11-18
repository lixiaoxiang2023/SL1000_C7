/*******************************************************************************
文件名称：   enc28.h
文件目录：   $PROJ_DIR$/usr/Drive
项目名称：   事业一部技术平台
创建者：    xuqiagn
创建日期：   20170309
程序用途：   wifi模块驱动
版权声明：   Copyright urit Co.,Ltd.All Right Reserved

第一次修改：
修改人：胥强
修改内容：加入技术平台
日期：20160805
*******************************************************************************/
#ifndef __WIFI_H
#define __WIFI_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    unsigned char byUartNo;

    unsigned int *pdwPd;
    unsigned int *pdwRst;
}WifiInitTdf;

typedef struct {
    int  (*PowerUp)  (void);
    void (*PowerOff) (void);
    bool (*IsExist)  (void);
    int  (*HwInit)   (bool blInitHw);
    int (*ListApConn) (char *pbyList, unsigned int dwLenLimt);
    int (*ListAp) (char *pbyList, unsigned int dwLenLimt);
    int (*GetApSsid) (unsigned int dwAp, char *pcSsid);
    int (*JoinAp) (unsigned int dwAp, char *pcPwd);
    int (*QuitAp) (void);
    int (*GetIp) (char *pcIp, char *pcMac);
    int (*TcpSend) (unsigned char *pbyIp, unsigned short int wPort, const unsigned char *pbyIn, unsigned int dwLen);
    int (*TcpLink) (unsigned char *pbyIp, unsigned short int wPort);
    int (*GetTcpStatus) (void);

    void (*Debug) (bool blIsOn);
}DevWifiTdf;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevWifiTdf *WifiInit(WifiInitTdf* pstInit);

#endif