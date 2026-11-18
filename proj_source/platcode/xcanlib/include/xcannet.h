/*******************************************************************************
文件名称：   xcannet.h
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2019.5.30
程序用途：   xcan协议,用于对cpp的封装,主要是对通底层通信接口的支持

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2019.5.30
修改人：胥强
修改内容：初始编写

*******************************************************************************/
#ifndef __XCANNET_H
#define __XCANNET_H

/*******************************************************************************
Include file
*******************************************************************************/
#include <list>
#include <string.h>
#include <string>
#include "xcan.h"

using namespace std;

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class XcanNetD;
class XCAN_EXPORTS_API XcanNet {
    // xcan 内核,用于驱动通信接口
private:
    XcanNetD *d = nullptr;

public:
    typedef struct {
        XcanNet *pThis = nullptr;

        unsigned int dwCommChnID;
        stCommChnTdf stComm;
    } stXcanNetTdf;

    unsigned int mdwCommChnID;
    unsigned int mdwFrameLen;

    // 支持的远程设备通信方式，can可以有can1、can2，网络 IP1，IP2...
    // Xcan 主要对通信方法的实现
    XcanNet(unsigned int dwCommChnID, unsigned int dwFrameLen);
    XcanNet();
    virtual ~XcanNet();

    // 输入
    int In(unsigned int dwComChnID, unsigned char *pbyIn, unsigned int dwLen);

    // 发送
    // 需要在子类实现

    // 设备支持的传输方法
    static stXcanNetTdf GetCommDev(unsigned int dwDevID);
    static void  SetMyNetDevID(short unsigned int wDevID);
    static short unsigned int GetMyNetDevID(void);

    // IO mutex same with the xcannet mutex.
    static void GetMutex(void);
    static void PutMutex(void);
    static void NetServiceUp(void);
    static void NetServiceDown(void);

private:
    static int  AddFilterCb(unsigned int dwComChnID, unsigned short int wFilterID, unsigned int dwFilterLen);
    static bool IsSendFullCb(unsigned int dwComChnID, unsigned int dwPort);
    static int  SendCb(unsigned int dwCommChnID, unsigned int dwPort, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen);
    static void ErrCb(unsigned int dwComChnID, unsigned int dwPort, unsigned int dwRemoteDevID, unsigned short int wIoUUID, unsigned char byFrame);

public:
    virtual int  AddFilter(unsigned int dwPort, unsigned short int wFilterID, unsigned int dwFilterLen);
    virtual bool IsSendFull(unsigned int dwPort);
    virtual int  Send(unsigned int dwPort, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen);
    virtual void Err(unsigned int dwPort, unsigned int dwRemoteDevID, unsigned short int wIoUUID, unsigned char byFrame);

public:
    // 内部方法
    static int ProcessReceiveQueue(void);
    // 支持设备
    static const DevXcanTdf *mXcan;

    // 支持的通信链表
    static list<stXcanNetTdf> *mListCommNet;

protected:
    // 本机设备ID
    static short unsigned int wNetMyDevID;

};

#endif // __XCANNET_H
