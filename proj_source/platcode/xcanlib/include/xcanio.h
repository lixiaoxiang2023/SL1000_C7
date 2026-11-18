/*******************************************************************************
文件名称：   xcanio.h
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2019.5.30
程序用途：   xcan协议,用于对cpp的封装

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2019.5.30
修改人：胥强
修改内容：初始编写

*******************************************************************************/
#ifndef __XCANIO_H
#define __XCANIO_H

/*******************************************************************************
Include file
*******************************************************************************/
#include "xcannet.h"

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class XcanIOD;
class XCAN_EXPORTS_API XcanIO : public XcanNet {
public:
    /*
    * 桥接资源定义
    */
    typedef struct {
        XcanIO *pThis = nullptr;
        bool blIsAdd;

        short unsigned int mMyDevID;
        short unsigned int mRemoteDevID;
        unsigned int  mCommChnID;
    } stBridgeTdf;

    /*
    * 命令等待模式
    */
    typedef enum {
        WaitNo = 0,
        WaitRecCmd = 1,
        WaitCmdDone = 2
    } CmdWaitMode;

    /* 通信状态查询
    *   @ unsigned int dwUUID 数据ID
    *   @ unsigned short int *pwTxSN 发送的sn号，每次发送一个包+1
    *   @ unsigned short int *pwTxID sync数据包的ID，当发送成功后，收到对应的ID反馈后+1
    *   @ unsigned short int *pwRxSN 收到的接收方接收序号，没接收一个数据包+1
    *   @ unsigned short int *pwRxID 由接收方，返回的发送ID，接收方不修改，直接返回
    *   @ unsigned int *pwTrsLost 当发送方接收到返回ID与本地不一致时+1
    */
    typedef struct __stTrsCommStaTdf {
        unsigned short int wTxPkgID;
        unsigned short int wTxPkgSN;
        unsigned short int wRxPkgID;
        unsigned short int wRxPkgSN;
        unsigned int dwTxPkgLost;
    } stTrsCommStaTdf;

public:
    /*
    * 数据类型
    */
    class XCAN_EXPORTS_API XcanIoDesc {
    private:
        unsigned int *mIoD = nullptr;
        unsigned int *mMyDat = nullptr;

    public:
        typedef enum __Iotype : int {
            IO_U8 = 1,
            IO_U16 = 2,
            IO_U32 = 4
        } Iotype;

        XcanIO *mIoThis = nullptr;
        const char *mName = nullptr;    // 名称
        unsigned int mPosition;         // 位置
        unsigned short int mLen;        // 数据长度
        Iotype mType;                   // 类型
        unsigned int mArrayLen;         // 数组长度

        /// <summary>
        /// 用于回调数据，数据改变后将回调
        /// </summary>
        void (*mCallBackNotice) (void);

        XcanIoDesc(XcanIO *pIoThis, const char *name, Iotype dwType, unsigned short int wArrayLen);
        ~XcanIoDesc();

        unsigned int *Value(void);
        void Value(unsigned int dwIn);
        void Value(unsigned short int wIn);
        void Value(unsigned char byIn);
        void Value(unsigned int *pIn, unsigned int dwLen);
        void Value(unsigned short int *pIn, unsigned int dwLen);
        void Value(unsigned char *pIn, unsigned int dwLen);

        void SetRefreshNotice(void(*pHandler)(void));
        void CheakAndNotice(void);
    };

private:
    XcanIOD *d = nullptr;

    int BridgeDelete(XcanIO::stBridgeTdf *pBridge);
    int BridgeInsert(XcanIO::stBridgeTdf *pBridge);
    // 内部接收通知接口
    static void ReceiveNoticeHandler(unsigned int dwUUID);

    /*
     * for data read and write oparetions
    */
    static void RdWrNoticeHandler(unsigned int dwUUID, unsigned int dwOp); // dwOp: 1 read, 0 write

protected:
    int  BridgeLink(short unsigned int wRemoteDevID, unsigned int dwComChnID);
    int  BridgeRemove(void);

    void SetCmd(unsigned int dwICmdPos, unsigned int dwICmdCntPos, unsigned int dwOCmdCntPos);
    void SetCmd(unsigned int dwICmdPos, unsigned int dwICmdCntPos, unsigned int  dwICmdDataPos, unsigned int dwOCmdCntPos);

public:
    XcanIO();
    virtual ~XcanIO();

    int  LinkToDevice(short unsigned int wRemoteDevID, unsigned int dwComChnID, short unsigned int wIoUUID, unsigned char *pbyIoSrc, bool blIsRx);
    int  LinkToDevice(short unsigned int wRemoteDevID, unsigned int dwComChnID, short unsigned int wIoUUID, bool blIsRx);

    static void BridgeService(void);

    int  SynSet(unsigned short int wStart, short unsigned int wOutStart, short unsigned int wEnd);
    int  SynSet(unsigned short int wInStart, short unsigned int wOutStart, short unsigned int wSyncEnd, short unsigned int wIoDStart, short unsigned int wIoDEnd);

    bool RemoteDeviceExist();
    bool LinkToRemoteDevice();
    bool DeepWrite(unsigned int dwPos, unsigned char byDat);
    bool Write(short unsigned int wAddr, unsigned int dwLen, unsigned char *pbyDat);
    bool Read(short unsigned int wAddr, unsigned int dwLen);
    bool ReadGetData(unsigned char* pbyOut);
    bool IsReadWriteBusy(void);

    unsigned int GetIoU32(unsigned int dwStartPos);
    void SetIoU32(unsigned int dwStartPos, unsigned int dwIn);
    short unsigned int GetIoU16(unsigned int dwStartPos);
    void SetIoU16(unsigned int dwStartPos, short unsigned int wIn);
    unsigned char GetIoU8(unsigned int dwStartPos);
    void SetIoU8(unsigned int dwStartPos, unsigned char byIn);
    int SetIoArrayU32(unsigned int dwStartPos, unsigned int * pdwIn, unsigned int dwLen);
    int GetIoArrayU32(unsigned int dwStartPos, unsigned int dwLen, unsigned int * pdwOut);
    int SetIoArrayU16(unsigned int dwStartPos, short unsigned int * pwIn, unsigned int dwLen);
    int GetIoArrayU16(unsigned int dwStartPos, unsigned int dwLen, short unsigned int * pwOut);
    int SetIoArrayBytes(unsigned int dwStartPos, unsigned char * pbyIn, unsigned int dwLen);
    int GetIoArrayBytes(unsigned int dwStartPos, unsigned int dwLen, unsigned char * pbyOut);

    // 通知数据接收据柄
    void SetReceiveNoticeHandler(void(*pNotice) (unsigned int dwIoUUID));
    unsigned int GetIoPosition(unsigned char *pbyInPos);
    bool IsRealDev(void);

    // 设置执行命令
    int SetCommand(unsigned char byCmd);
    int SetCommand(unsigned char byCmd, void(*pHandler)(unsigned int));
    int SetCommand(unsigned short wCmd);
    int SetCommand(unsigned short wCmd, unsigned short *data, int dataLen);

    int GetState(void);
    int SetCmdWithCheak(unsigned char byCmd, unsigned int dwInPos, unsigned int dwInLen, unsigned int dwOutPos,
        unsigned int dwOutLen, unsigned int dwDat, unsigned int dwDatMask);
    bool IsRemoteBusy(void);
    bool IsCommandExcEnd(void);
    bool IsCommandExcEndW(void);
    XcanIO::stTrsCommStaTdf GetTrsCommSta(void);

    void SetIoDescPos(unsigned int dwPos);
    unsigned int GetIoDescPos(void);
    void InsertIoDesc(XcanIoDesc *pClass);

    void SetIoInsertStart(void);
    void SetIoDSyncEnd(void);
    void SetIoDEnd(void);

    void SetIoSynInStart(void);
    void SetIoSynOutStart(void);
    void SetIoSynEnd(void);

};

#endif // __XCANSDK_H
