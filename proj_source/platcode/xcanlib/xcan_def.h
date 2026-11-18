/*******************************************************************************
文件名称：   xcan_def.h
目录：       $PROJ_DIR$/../code/xcanlib/src
项目名称：   事业一部
创建者：     胥强
创建日期：   20180915
程序用途：   用于xcan参数的定义,xcan内部函数的声明

版权声明：
Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
20130721：
第一次由胥强创建初始程序。
2018.10.11
由秦志福增加win32的支持
 ******************************************************************************/
#ifndef __XCAN_DEF_H
#define __XCAN_DEF_H

 /*******************************************************************************
 Include file
 *******************************************************************************/
#include <stdbool.h>
#include <string.h>
#include "xcan.h"

#if(XCAN_DEBUG)
#include <stdio.h>
#endif

 /*******************************************************************************
 全局变量定义
 *******************************************************************************/
#define XCAN_TXSTA_IDLE                     0x00
#define XCAN_TXSTA_WAIT                     0x01
#define XCAN_TXSTA_SEND                     0x02
#define XCAN_TXSTA_WAIT_ACK                 0x10
#define XCAN_TXSTA_WAIT_READ                0x20
#define XCAN_TXSTA_END                      0x40

#define XCAN_CODE_HEAD                      0   /*写入 */
#define XCAN_CODE_MID                       1
#define XCAN_CODE_END                       2
#define XCAN_CODE_SIG                       3

#define XCAN_FRAME_WRITE                    0
#define XCAN_FRAME_READ_BACK                1
#define XCAN_FRAME_READ                     2
#define XCAN_FRAME_WRITE_ACK                3
#define XCAN_FRAME_SYNC_SET                 4
#define XCAN_FRAME_SYNC_WRITE               5
#define XCAN_FRAME_SYNC_SET_ACK             6
#define XCAN_FRAME_SYNC_WRITE_ACK           7

#define XCAN_PORT_TRS_CHN_NUM               8

 // 宏定义
#define XCAN_DEEP_WRITE(uuid, obj, member, data, delayHandler)        \
        do{ \
            DevXcan->MustToRemote(uuid, (unsigned long)(&(obj.member)) - (unsigned long)(&(obj)), data, delayHandler);\
        } while(0);

// lock
#define XCAN_GET_MUTEX_T()                  do {if(pfGetMutex != NULL) pfGetMutex(); } while(0);
#define XCAN_PUT_MUTEX_T()                  do {if(pfPutMutex != NULL) pfPutMutex(); } while(0);

#if(PLAT_ARM_CORTEX)
    #define XCAN_ENTER_CRITICAL()           XCAN_GET_MUTEX_T()
    #define XCAN_EXIT_CRITICAL()            XCAN_PUT_MUTEX_T()

    #define XCAN_GET_MUTEX()
    #define XCAN_PUT_MUTEX()
#else
    #define XCAN_ENTER_CRITICAL()
    #define XCAN_EXIT_CRITICAL()

    #define XCAN_GET_MUTEX()                XCAN_GET_MUTEX_T()
    #define XCAN_PUT_MUTEX()                XCAN_PUT_MUTEX_T()
#endif

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef union {
    unsigned short int wId;
    struct {
        unsigned short int IDL : 7;
        unsigned short int IDL_RES : 1;
        unsigned short int IDH : 7;
        unsigned short int IDH_RES : 1;
    } bitId;
} unCanFilterTdf;

typedef union {
    unsigned int dwId;
    struct {
        unsigned int Res : 3;                 //保留不用数据
        unsigned int bit10Pkg_Addr : 10;     //数据需要操作的数据地址
        unsigned int bit3Type : 3;            //数据操作代码
        unsigned int bit2Pkg : 2;
        unsigned int bit14IoUUID : 14;
    }stHead;
}stHeadTdf;

typedef struct {
    //发送
    unsigned int  dwLastPkgNum;
    unsigned char bySta;
    unsigned char byFrameType;
    unsigned char byFunc;

    unsigned short int wPkgNum;
    unsigned short int wPkgCnt;
    unsigned short int wIoStart;

    unsigned char byaTx[XCAN_PORT_TRS_CHN_NUM];
    unsigned char *pbyTx;               //发送数据缓冲地址

    unsigned int dwChnTim;              // 发送数据的时间
    unsigned int dwReTrsTim;            // 重发的次数
} stTxCellTdf;

typedef struct {
    stCommChnTdf stChn;

    bool blUsing;
    bool blTxIrpDoing;
    bool blDoTrs;
} stCommDevChnTdf;

typedef struct {
    short unsigned int wRemoteDevID;    // 远程设备ID
    unsigned int  dwTimDelay;           // 用于设备的心跳延时
    unsigned int  dwCommChnID;          // 用于通信接口ID记录

    unsigned int  dwLinkIoNum;          // 连接的IO资源个数
    unsigned int  dwLinkIoNumBit;       // 连接的IO资源占位
    unsigned int  dwLinkIoOnBit;        // 被连接的IO资源数目占位

    bool blUsing;
} stRemoteAlianDevTdf;

typedef struct {
    unsigned char bySetCmd;         // 1:开启，0 关闭

    union {
        unsigned short int wStart;
        struct {
            unsigned char StartL;
            unsigned char StartH;
        } BitStart;
    }unStart;

    union {
        unsigned short int wEnd;
        struct {
            unsigned char EndL;
            unsigned char EndH;
        } BitEnd;
    }unEnd;

    union {
        unsigned short int wTim;
        struct {
            unsigned char TimL;
            unsigned char TimH;
        } BitTim;
    }unTim;
} stSynTrsSetTdf;

typedef enum __XcanSyncStatus {
    XCAN_SYNC_IDLE = 0,
    XCAN_SYNC_WAIT,
    XCAN_SYNC_DO
} emXcanSyncStatus;

typedef enum __XcanSyncTrsSta {
    XcanTrs_Idle = 0,
    XcanTrs_WaitAck = 1,
    XcanTrs_GetAck = 2
} emXcanSyncTrsStatus;

typedef struct {
    // 被远端请求同步
    unsigned char *pbyShadow;
    unsigned char *pbyBuffer;
    unsigned char *pbyIoCache;

    unsigned short int wMonitorStart;
    unsigned short int wMonitorEnd;

    unsigned int dwTimDelay;

    bool blTrsError;
    bool blWorking;
    emXcanSyncStatus emXcanSyncSta;
    // 同步传输状态控制
    emXcanSyncTrsStatus emXcanSyncTrsSta;

    // 本次跟新的开始地址
    unsigned short int wSyncIoStart;

    // 计数器
    unsigned short int wTxPkgID;        // 发包的ID
    unsigned short int wRxPkgID;        // 接收的 diff的计数器

    unsigned short int wTxPkgSN;
    unsigned short int wRxPkgSN;        // diff 的发送序号
    unsigned int dwPkgLost;             // 記錄丢包的个数
} __stSyncLocal;

typedef struct {
    unsigned int dwSysTim;
    // 请求远端同步
    unsigned short int wMonitorStart;
    unsigned short int wMonitorEnd;
    unsigned short int wPkgID;
    unsigned short int wPkgSN;

    // 同步
    unsigned short int wIoSyncEnd;
    unsigned short int wIoEnd;

    bool blNeedSync;
    bool blSyncLink;
} __stSynReq;

typedef struct {
    bool blUsing;                           // 资源接口使用中
    bool blActive;                          // 资源是否活跃，活跃资源将自动进行管理
    bool blImRxIO;                          // 是否为接收端口

    unsigned short int wMyDevID;            // 本机的设备ID
    unsigned short int wOrigalIoUUID;       // 未被处理的uuid
    unsigned short int wIoUUID;             // 用于接收的ID过滤标识
    unsigned short int wRxIoUUID;           // 用于接收的ID过滤标识
    unsigned short int wTxIoUUID;           // 用于发送的ID标识过滤
    unsigned short int wFilter;             // 用于特殊设备的过滤标识
    unsigned short int wFriendID;           // 用于桥接的其他对象ID

    unsigned int dwInDevQun;                // 在所支持的设备里面的bit排序

    // 设备ID
    short unsigned int wRemoteDevID;        // 远程设备ID
    unsigned int  dwCommChnID;              // 通信接口ID

    //资源
    unsigned int  dwUUID;                   // 设备的全局系统统一识别号: 使用wRxUUID + 远程设备
    unsigned int  dwDevComUUID;             // 在本机的ID资源唯一标识  ：使用wRxUUID + 通信接口ID，用于识别通信接口匹配
    unsigned int  dwDInID;                  // 记录通信输入的数据通道ID，用于数据的输出。
    unsigned char *pbyRx;                   // 接收数据缓冲地址
    unsigned char *pbyIoSrc;                // 用户io资源区
    unsigned short int wMaxLen;             // 支持的IO资源数据长度

    //发送缓冲结构
    stTxCellTdf stTx[8];
    struct {
        //接收
        unsigned char blIsRxing;

        unsigned char byTrsSn;
        unsigned short int wRxPkgMun;
        unsigned short int wRxPkgCnt;
        unsigned short int wNeedPkgNo;
        unsigned short int wIoStart;
        unsigned int dwLen;
    }stRx;

    struct __stSyncTrs {
        // 用于事件返回注册的IO资源数据
        __stSyncLocal stSynLoacl;

        // ctr status
        __stSynReq stSynReq;
        // 传输控制次数
        unsigned int dwRemoteSyncID;
        unsigned int dwMySuncID;
    } stSyncTrs;

    stRemoteAlianDevTdf *pstRemoteAlian;
    stCommDevChnTdf *pstCommOps;                                            // 通信方法
    void(*pfReceiveNotice) (unsigned int dwUUID);                           // 用于收到数据后进行通知触发
    void(*Err) (void);                                                      // IO 资源连接丢失
    void(*pfReadWriteEnd) (unsigned int dwUUID, unsigned int dwOp);         // 读/写数据完成后，接收到对方的最后以帧时调用
} DevPortTdf;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
extern void(*pfGetMutex) (void);
extern void(*pfPutMutex) (void);

/*******************************************************************************
函数申明
*******************************************************************************/
extern int AddTrs(DevPortTdf * pstPort, unsigned char byFrameType, unsigned short int wIoAdr, unsigned char *pbyDatIn, unsigned int dwLen);
extern DevPortTdf *GetPort(unsigned int dwUUID);
extern void XcanOutInit(stCommChnTdf *pstCommDev);
extern void XcanInInit(stCommChnTdf *pstCommDev);
extern stCommDevChnTdf *GetCommChn(unsigned int dwCommChnID);
extern int CheakResetTrs(DevPortTdf * pstPort, unsigned char byFrameType);
extern int CoreTxTrigger(DevPortTdf * pstPort);
extern XcanCoreInPortCb GetInportFromIn(void);
extern DevPortTdf *UpdatePortDinID(short unsigned int wIoUUID, short unsigned int wRemoteDevID, unsigned int dwDInID);
extern int BindPort2CommChn(unsigned int dwUUID);
extern DevPortTdf *GetCommPort(unsigned int dwGUUID);

#endif
