/*******************************************************************************
文件名称：   xcan.c
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20130721
程序用途：   xcan协议

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2016.03.19
修改人：胥强
修改内容：重新整理协议内容，并将协议结果模块化

2016.05.24
修改人：胥强
1、修改书发送逻辑，之前存在长数据帧发送出错问题
2、增加debug信息功能，用于调试
2018.10.11
由秦志福增加win32的支持
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "xcan_def.h"
#include "xcan_dcfg.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
DevPortTdf s_stPort[SURPORT_MAX_PORT];
unsigned int (*pfGetSysTim)(void) = NULL;
void (*pfGetMutex) (void);
void (*pfPutMutex) (void);

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/
static stRemoteAlianDevTdf s_stRemoteAlianDev[XCAN_HW_DEV_SURPORT_NUM];
static stCommDevChnTdf s_stCommChn[XCAN_HW_DEV_SURPORT_COMMCHN];
static unsigned int s_XcanMyDevID = 0;

#if(XCAN_DEBUG)
    const char *canFrameType[8] = {
        "XCAN_FRAME_WRITE",
        "XCAN_FRAME_READ_BACK",
        "XCAN_FRAME_READ",
        "XCAN_FRAME_WRITE_ACK",
        "XCAN_FRAME_SYNC_SET",
        "XCAN_FRAME_SYNC_WRITE",
        "XCAN_FRAME_SYNC_SET_ACK",
        "XCAN_FRAME_SYNC_WRITE_ACK",
    };

    const char *canFrame[4] = {
        "XCAN_CODE_HEAD",
        "XCAN_CODE_MID",
        "XCAN_CODE_END",
        "XCAN_CODE_SIG"
    };
#endif

extern int ServicePortInit(unsigned int dwUUID, unsigned char *pbyShadow, unsigned char *pbyShadowBuff, unsigned char* pbyIoCache);
extern void Service(void);
extern unsigned int mXCAN_SYNC_SET_RELINK_TIM;
extern unsigned int mXCAN_RE_TRS_TIM;

/*******************************************************************************
函数功能：
*   @ 获取协议使用的设备/创建新的通信设备
参数描述：
*   @ DevPortTdf *pstPort
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static stRemoteAlianDevTdf *GetRemoteAlianDev(short unsigned int wRemoteDevID)
{
    for(int i = 0; i < XCAN_HW_DEV_SURPORT_NUM; i ++) {
        if(s_stRemoteAlianDev[i].wRemoteDevID == wRemoteDevID && s_stRemoteAlianDev[i].blUsing == true) return &s_stRemoteAlianDev[i];

        if(s_stRemoteAlianDev[i].blUsing == false) {
            s_stRemoteAlianDev[i].blUsing = true;
            s_stRemoteAlianDev[i].wRemoteDevID = wRemoteDevID;
            return &s_stRemoteAlianDev[i];
        }
    }

    return NULL;
}

/*******************************************************************************
函数功能：
*   @ 获取协议使用的设备
参数描述：
*   @ DevPortTdf *pstPort
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
stCommDevChnTdf *GetCommChn(unsigned int dwCommChnID)
{
    for (int i = 0; i < XCAN_HW_DEV_SURPORT_COMMCHN; i++) {
        if (s_stCommChn[i].stChn.dwComChnID == dwCommChnID) return &s_stCommChn[i];
        if (s_stCommChn[i].blUsing == false) {
            s_stCommChn[i].blUsing = true;
            return &s_stCommChn[i];
        }
    }

    return NULL;
}

/*******************************************************************************
函数功能：
*   @ 根据设备的地址，取得注册通道
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @ 设备的通道
说明：
*   @ 无
*******************************************************************************/
DevPortTdf *GetPort( unsigned int dwUUID)
{
    DevPortTdf *pstPort = s_stPort;

    for( int i = 0;i < SURPORT_MAX_PORT; i++, pstPort ++) {
        if (dwUUID == pstPort->dwUUID) break;
        if (pstPort->blUsing == false) continue;
    }

    if (pstPort->blUsing == false) return NULL;
    if (dwUUID != pstPort->dwUUID) return NULL;
    return pstPort;
}

/*******************************************************************************
函数功能：
*   @ 根据设备的地址，取得注册通道
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @ 设备的通道
说明：
*   @ 无
*******************************************************************************/
DevPortTdf *GetCommPort(unsigned int dwGUUID)
{
    DevPortTdf *pstPort = s_stPort;

    for (int i = 0; i < SURPORT_MAX_PORT; i++, pstPort++) {
        if (dwGUUID == pstPort->dwDevComUUID) break;
        if (pstPort->blUsing == false) continue;
    }

    if (pstPort->blUsing == false) return NULL;
    if (dwGUUID != pstPort->dwDevComUUID) return NULL;
    return pstPort;
}

/*******************************************************************************
函数功能：
*   @ 根据设备的地址，查询写入通道是否忙
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @  true ： 设备忙
*   @ false ： 设备在等待返回，或者传输完成
说明：
*   @ 无
*******************************************************************************/
static bool IsWrBusy(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (pstPort == NULL) return false;
    if(pstPort->stTx[XCAN_FRAME_WRITE].bySta == XCAN_TXSTA_WAIT || \
       pstPort->stTx[XCAN_FRAME_WRITE].bySta == XCAN_TXSTA_SEND ) return true;

    return false;
}

/*******************************************************************************
函数功能：
*   @ 根据设备的地址，读取发送状态
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @  XCAN_TXSTA_IDLE       ：设备空闲
*   @  XCAN_TXSTA_WAIT       ：就绪，等待发送
*   @  XCAN_TXSTA_SEND       ：数据发送中
*   @  XCAN_TXSTA_WAIT_ACK   ：等待写入数据的返回确认
*   @  XCAN_TXSTA_END        ：传输结束
说明：
*   @ 无
*******************************************************************************/
static unsigned char GetWrStatus(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (!pstPort) return XCAN_TXSTA_IDLE;
    return pstPort->stTx[XCAN_FRAME_WRITE].bySta;
}

/*******************************************************************************
函数功能：
*   @ 根据设备的地址，查询读取通道是否忙
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @  true ： 设备忙
*   @ false ： 设备在等待返回，或者传输完成
说明：
*   @ 无
*******************************************************************************/
static bool IsRdBusy(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (!pstPort) return false;
    if(pstPort->stTx[XCAN_FRAME_READ].bySta == XCAN_TXSTA_WAIT || \
       pstPort->stTx[XCAN_FRAME_READ].bySta == XCAN_TXSTA_SEND) return true;

    return false;
}

/*******************************************************************************
函数功能：
*   @ 根据设备的地址，读取读通道状态
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @  XCAN_TXSTA_IDLE       ：设备空闲
*   @  XCAN_TXSTA_WAIT       ：就绪，等待发送
*   @  XCAN_TXSTA_SEND       ：数据发送中
*   @  XCAN_TXSTA_WAIT_READ  ：等待数据的返回
*   @  XCAN_TXSTA_END        ：传输结束
说明：
*   @ 无
*******************************************************************************/
static unsigned char GetRdStatus(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (!pstPort) return XCAN_TXSTA_IDLE;
    return pstPort->stTx[XCAN_FRAME_READ].bySta;
}

/*******************************************************************************
函数功能：
*   @ 读取注册设备的数据
参数描述：
*   @ unsigned char byHisId ： 需要写入的设备的地址
*   @ unsigned char byIoAdr ： 需要写入的IO资源的起始地址
*   @ unsigned char byLen ：   读取长度
*   @ unsigned char *pbyDatIn ： 需要写入的数据地址
*   @ unsigned char dwInTimOut ： 如果数据写入通道忙，上一次数据读取仍在发送，将等待设定的超时时间
                                  当超过设定的时间后，若设备还忙，将返回失败
*   @ void (*pfWait)( unsigned char) ： 在发送完读取命令后，让用户自行判断是否等待数据返回，以及
                                        如何等待，建议在等待的时间内，通过查询接口查询是否接受到确认帧。
                                        其参数为设备地址。
返回值描述：
*   @ -2：确认未返回
*   @ -1：设备忙
*   @  0：发送成功
说明：
*   @ 在确认帧中，真编码段将返回写入数据的IO起始地址，数据区第一个字节为数据起始地址，
*   @ 第二个数据为本次写入的数据长度
*******************************************************************************/
static int Write(unsigned int dwUUID, short unsigned int wIoAdr, unsigned int dwLen, unsigned char *pbyDatIn, unsigned int dwInTimOut, bool blWaitAck)
{
    unsigned int dwTimOut = 0;
    DevPortTdf *pstPort = GetPort(dwUUID);
    bool blTimOut = false;

    if(!pstPort) return -1;
  //  if ((wIoAdr + dwLen) > pstPort->wMaxLen) return -1; //wMaxLen 为同步最大长度 固件升级用这个接口 用这个判断条件有问题 20201020 10：57 lixx

    while(dwInTimOut != 0 && IsWrBusy(dwUUID)) {
        if(dwTimOut ++ > dwInTimOut) {
            blTimOut = true;
            break;
        }
    }

    if(blTimOut) return -1;
    if(IsWrBusy(dwUUID)) return -1;

    AddTrs ( pstPort, XCAN_FRAME_WRITE, wIoAdr, pbyDatIn, dwLen);
    dwLen >>= 3;

    if( blWaitAck ) {
        blTimOut = true;
        for(unsigned int timout = 0; timout < 0xffffff * (dwLen + 1); timout ++) {
            if(GetWrStatus(dwUUID) == XCAN_TXSTA_END) {
                blTimOut = false;
                break;
            }
        }
    }

    if(blTimOut) return -2;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取注册设备的数据
参数描述：
*   @ unsigned char byHisId ： 需要读取的设备的地址
*   @ unsigned char byIoAdr ： 需要读取的IO资源的起始地址
*   @ unsigned char byLen ：   读取长度
*   @ unsigned char dwInTimOut ： 如果数据读取通道忙，上一次数据读取仍在发送，将等待设定的超时时间
                                  当超过设定的时间后，若设备还忙，将返回失败
*   @ void (*pfWait)( unsigned char) ： 在发送完读取命令后，让用户自行判断是否等待数据返回，以及
                                        如何等待，建议在等待的时间内，通过查询接口查询是否接受到读取数据。
返回值描述：
*   @ -2：确认未返回
*   @ -1：设备忙
*   @  0：发送成功
说明：
*   @ 在确认帧中，帧编码段将返回写入数据的IO起始地址，数据区第一个字节为数据起始地址，
*   @ 第二个数据为本次写入的数据长度，读取的数据将自动填充到注册的IO资源上
*******************************************************************************/
static int Read(unsigned int dwUUID, short unsigned int byIoAdr, unsigned int dwLen, unsigned int dwInTimOut, bool blWaitAck)
{
    unsigned int dwTimOut = 0;
    DevPortTdf *pstPort = GetPort(dwUUID);
    bool blTimOut = false;

    if(!pstPort) return -1;
    if ((byIoAdr + dwLen) > pstPort->wMaxLen) return -1;

    while(dwInTimOut != 0 && IsRdBusy(dwUUID) == true) {
        if(dwTimOut ++ > dwInTimOut) {
            blTimOut = true;
            break;
        }
    }

    if(blTimOut) return -1;
    if(IsRdBusy(dwUUID)) return -1;

    pstPort->stTx[XCAN_FRAME_READ].byaTx[0] = (dwLen & 0xff);
    pstPort->stTx[XCAN_FRAME_READ].byaTx[1] = (dwLen >> 8) & 0xff;

    AddTrs ( pstPort, XCAN_FRAME_READ, byIoAdr, pstPort->stTx[XCAN_FRAME_READ].byaTx, 2);
    dwLen >>= 3;

    if( blWaitAck ) {
        blTimOut = true;
        for(unsigned int timout = 0; timout < 0xffffff * (dwLen + 1); timout ++) {
            if(GetRdStatus(dwUUID) == XCAN_TXSTA_END) {
                blTimOut = false;
                break;
            }
        }
    }

    if(blTimOut) return -2;
    return 0;
}

#if(PLAT_WIN32|PLAT_WIN32_VC)
/*******************************************************************************
函数功能：
*   @ 在win平台读写时间节拍
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static unsigned int GetSysTim(void)
{
    return (unsigned int)GetTickCount();
}
#endif

/*******************************************************************************
函数功能：
*   @ 设置系统时间接口，可以通过此接口读到系统的时间
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 在同步功能和错误检查与重传的模块需要使用，在不使用此接口时，可以不处理
*******************************************************************************/
XCAN_STATIC void SetTimPort(unsigned int (*pfHandler)(void))
{
#if(PLAT_WIN32|PLAT_WIN32_VC)
    pfGetSysTim = GetSysTim;
#else
    pfGetSysTim = pfHandler;
#endif
}

/*******************************************************************************
函数功能：
*   @ 主动开启远程自动同步功能
参数描述：
*   @ unsigned int dwUUID 资源ID
*   @ bool blSetOn true設置默认状态，false 不改变状态
*   @ short unsigned int wSyncStart 开始同步数据地址
*   @ short unsigned int wSyncEnd   同步结束地址
*   @ short unsigned int wIoSyncEnd IO开始的地址, 包括in和out的数据总和，主要是去除io最后的用于升级的数据区
*   @ short unsigned int wIoEnd     IO结束的地址，所有的io区域大小
返回值描述：
*   @ 无
说明：
*   @ 2020.11.30 增加IO范围的设定，用于指定IO的同步范围，主要用于对连接成功后的数据恢复功能
*******************************************************************************/
XCAN_STATIC int SynSet(unsigned int dwUUID, bool blSetOn, short unsigned int wSyncStart, short unsigned int wSyncEnd,
    short unsigned int wIoSyncEnd, short unsigned int wIoEnd)
{
    DevPortTdf *pstPort = GetPort(dwUUID);
    __stSynReq *pstTrs = NULL;

    if (!pstPort) return -1;
    pstTrs = &pstPort->stSyncTrs.stSynReq;

    if(wSyncStart > wSyncEnd) return -1;
    if(wSyncEnd > pstPort->wMaxLen) return -1;

    pstTrs->blNeedSync = true;
    pstTrs->wMonitorStart = wSyncStart;
    pstTrs->wMonitorEnd = wSyncEnd;

    pstTrs->wIoSyncEnd = wIoSyncEnd;
    pstTrs->wIoEnd = wIoEnd;

    if(blSetOn == true) pstTrs->blSyncLink = false;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭同步连接
参数描述：
*   @ unsigned int dwUUID 资源ID
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int SynSetDown(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);
    __stSynReq *pstTrs = NULL;

    if (!pstPort) return -1;
    pstTrs = &pstPort->stSyncTrs.stSynReq;
    pstTrs->blNeedSync = false;
    pstTrs->blSyncLink = false;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 主动开启远程自动同步功能
参数描述：
*   @ unsigned int dwUUID 资源ID
*   @ short unsigned int wStart 开始同步数据地址
*   @ short unsigned int wEnd 同步结束地址
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanSyncSetOn(unsigned int dwUUID, short unsigned int wSyncStart, short unsigned int wSyncEnd,
    short unsigned int wIoStart, short unsigned int wIoEnd)
{
    return SynSet(dwUUID, false, wSyncStart, wSyncEnd, wIoStart, wIoEnd);
}

/*******************************************************************************
函数功能：
*   @ 创建透传需要的通信组，用于自动协商数据长度
参数描述：
*   @ unsigned int dwUUID1 通道1
*   @ unsigned int dwUUID2 通道2
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int AddGroup(unsigned int dwUUID1, unsigned int dwUUID2)
{
    DevPortTdf *pstPort = GetPort(dwUUID1);

    if(dwUUID1 == dwUUID2) return -1;

    if (!pstPort) return -1;
    pstPort->wFriendID = dwUUID2;
    pstPort->stSyncTrs.stSynReq.blSyncLink = false;

    pstPort = GetPort(dwUUID2);
    if (!pstPort) return -1;
    pstPort->wFriendID = dwUUID1;
    pstPort->stSyncTrs.stSynReq.blSyncLink = false;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 查询输入连接情况
参数描述：
*   @ 查询输出连接情况
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC bool SynInLink(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);
    __stSynReq *pstTrs = NULL;

    if (!pstPort) return false;
    pstTrs = &pstPort->stSyncTrs.stSynReq;

    return pstTrs->blSyncLink;
}

/*******************************************************************************
函数功能：
*   @ 查询输出连接情况
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC bool SynOutLink(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);
    __stSyncLocal *pstTrs = NULL;

    if (!pstPort) return false;
    pstTrs = &pstPort->stSyncTrs.stSynLoacl;

    return pstTrs->blWorking;
}

/*******************************************************************************
函数功能：
*   @ 向IO资源写入数据，并监控影子缓冲区的数据是否和IO区一致。
参数描述：
*   @ unsigned int dwUUID ：资源ID
*   @ unsigned int dwPos ： 资源位置
*   @ unsigned char byDat ： 需要写入的值
*   @ void (*pfDelayHandler)(void) ： 延时接口
返回值描述：
*   @ -1：增加设备失败
*   @  0：增加设备成功
说明：
*   @ 如果数据变化并和IO区一致，说明差异同步已经发出。
*******************************************************************************/
static int MustToRemote(unsigned int dwUUID, unsigned int dwPos, unsigned char byDat, void (*pfDelayHandler)(void))
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (pstPort == NULL) return -1;
    if (dwPos > pstPort->wMaxLen) return -1;
    if (dwPos < pstPort->stSyncTrs.stSynLoacl.wMonitorStart) return -1;
    if (dwPos >= pstPort->stSyncTrs.stSynLoacl.wMonitorEnd) return -1;

    pstPort->pbyIoSrc[dwPos] = byDat;

    while (pstPort->pbyIoSrc[dwPos] != pstPort->stSyncTrs.stSynLoacl.pbyShadow[dwPos]) {
        if (pstPort->stSyncTrs.stSynLoacl.blWorking == false) break;
        if (pstPort->stSyncTrs.stSynReq.blNeedSync == false) break;
        if (pstPort->stSyncTrs.stSynReq.blSyncLink == false) break;

        if (pfDelayHandler) pfDelayHandler(); // 主要用于给应用层进行延时，这里没有通用的延时方案。
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置收到数据后的通知接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 协议在收到数据后将触发此接口
*******************************************************************************/
XCAN_STATIC int RegReceiveNotice(unsigned int dwUUID, void (*pfHandler)(unsigned int dwUUID))
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if(pstPort) {
        pstPort->pfReceiveNotice = pfHandler;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 清除收到数据后的通知接口消息
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int UnRegReceiveNotice(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (!pstPort) return -1;
    pstPort->pfReceiveNotice = NULL;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置读数据后的通知接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 协议在收到读数据完成后调用
*******************************************************************************/
XCAN_STATIC int RegReadWriteNotice(unsigned int dwUUID, void (*pfHandler)(unsigned int dwUUID, unsigned int dwOp))
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if(pstPort) {
        pstPort->pfReadWriteEnd = pfHandler;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 清除收到数据后的通知接口消息
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int UnRegReadWriteNotice(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (!pstPort) return -1;
    pstPort->pfReadWriteEnd = NULL;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 获取差异传输的传输数据统计
参数描述：
*   @ unsigned int dwUUID 数据ID
*   @ unsigned short int *pwTxSN 发送的sn号，每次发送一个包+1
*   @ unsigned short int *pwTxID sync数据包的ID，当发送成功后，收到对应的ID反馈后+1
*   @ unsigned short int *pwRxSN 收到的接收方接收序号，没接收一个数据包+1
*   @ unsigned short int *pwRxID 由接收方，返回的发送ID，接收方不修改，直接返回
*   @ unsigned int *pwTrsLost 当发送方接收到返回ID与本地不一致时+1
返回值描述：
*   @ -1 参数异常，0 成功
说明：
*   @ 协议在收到数据后将触发此接口
*******************************************************************************/
XCAN_STATIC int GetCommSta(unsigned int dwUUID, unsigned short int *pwTxSN, \
    unsigned short int *pwTxID, unsigned short int *pwRxSN, unsigned short int *pwRxID, \
    unsigned int *pwTrsLost)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if(!pstPort) return -1;

    if(pwTxSN != NULL) *pwTxSN = pstPort->stSyncTrs.stSynLoacl.wTxPkgSN;
    if(pwTxID != NULL) *pwTxID = pstPort->stSyncTrs.stSynLoacl.wTxPkgID;

    if(pwRxSN != NULL) *pwRxSN = pstPort->stSyncTrs.stSynLoacl.wRxPkgSN;
    if(pwRxID != NULL) *pwRxID = pstPort->stSyncTrs.stSynLoacl.wRxPkgID;

    if(pwTrsLost != NULL) *pwTrsLost = pstPort->stSyncTrs.stSynLoacl.dwPkgLost;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 移除接口
参数描述：
*   @ unsigned char byHisId ： 注册的设备地址
返回值描述：
*   @  0 ： 成功
*   @ -1 ： 失败
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int RemovePort(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (pstPort == NULL) return -1;
    pstPort->blUsing = false;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭接口，暂时不启用自动同步功能
参数描述：
*   @ unsigned int dwUUID ： port ID
返回值描述：
*   @  0 ： 成功
*   @ -1 ： 失败
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int PortDisActive(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (pstPort == NULL) return -1;
    pstPort->blActive = false;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启用自动同步功能
参数描述：
*   @ unsigned int dwUUID ： port ID
返回值描述：
*   @  0 ： 成功
*   @ -1 ： 失败
说明：
*   @ 主要用于某些设备的部分子设备可能在有些情况下不使用
*******************************************************************************/
XCAN_STATIC int PortActive(unsigned int dwUUID)
{
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (pstPort == NULL) return -1;
    pstPort->blActive = true;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 向xcan协议增加可以传输数据的设备，包括其设备的IO资源，数据缓冲地址等。
参数描述：
*	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
*   @ unsigned int dwRemoteDevID ：本资源所属的远程设备ID
*   @ unsigned short int wIoUUID ： 本系统内资源的唯一ID定义
*   @ unsigned char *pbyIoSrc ： 注册设备的IO资源地址，在接受到正确数据后协议会自动写入，不用注册为NULL
*   @ unsigned char *pbyBuff ： 协议在接受数据时需要的数据缓存
*   @ unsigned short int wMaxLen ： IO口资源能够操作的最大数据长度。
*	@ bool blIsRxIO 是否为接收资源
返回值描述：
*   @ -1：增加设备失败
*   @  0：增加设备成功
说明：
*   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
*   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
*******************************************************************************/
XCAN_STATIC int AddPort(stCommChnTdf *pstCommChn,
    short unsigned int wRemoteDevID,
    unsigned short int wIoUUID,
    unsigned char *pbyIoSrc, unsigned char *pbyBuff,
    unsigned int dwMaxLen, bool blIsRxIO)
{
    unsigned int dwUUID;
    DevPortTdf *pstPort;
    stRemoteAlianDevTdf *pstAlianDev = NULL;
    stCommDevChnTdf *pstCommDevChn = NULL;

    unsigned short int wOrigalIoUUID = wIoUUID;
    unsigned short int wTxIoUUID;
    unsigned short int wRxIoUUID;
    unCanFilterTdf wID2Bit14;

    /*
    * 使用14位的ID，必须将bit7清除，bit6置1
    * 高位ID与地位ID不可以相同
    */
    wID2Bit14.wId = wIoUUID;
    wID2Bit14.bitId.IDH_RES = 0;
    wID2Bit14.bitId.IDH |= 0x40;
    wID2Bit14.bitId.IDL_RES = 0;
    wID2Bit14.bitId.IDL |= 0x40;

    if(wID2Bit14.bitId.IDH == wID2Bit14.bitId.IDL) return -1;
    wIoUUID = wID2Bit14.wId;

    unsigned char UUIDL = wIoUUID & 0x007f;
    unsigned char UUIDH = (wIoUUID >> 8) & 0x007f;

    if (blIsRxIO == true) {
        wTxIoUUID = ((unsigned short int)UUIDH << 7) + UUIDL;
        wRxIoUUID = ((unsigned short int)UUIDL << 7) + UUIDH;
    }
    else {
        wTxIoUUID = ((unsigned short int)UUIDL << 7) + UUIDH;
        wRxIoUUID = ((unsigned short int)UUIDH << 7) + UUIDL;
    }

    wIoUUID = wRxIoUUID;
    dwUUID = ((unsigned int)wRemoteDevID << 16) + wOrigalIoUUID;

    pstPort = &s_stPort[0];
    for (int i = 0; i < SURPORT_MAX_PORT; i++, pstPort ++) {
        if(dwUUID == pstPort->dwUUID) return 0;
        if(!pstPort->blUsing) break;
    }

    if(pstPort->blUsing) return 0;

    pstPort->blActive   = true;
    pstPort->blImRxIO   = blIsRxIO;
    pstPort->wIoUUID	= wIoUUID;
    pstPort->wOrigalIoUUID = wOrigalIoUUID;
    pstPort->dwUUID     = dwUUID;
    pstPort->dwDevComUUID = 0;
    pstPort->wTxIoUUID  = wTxIoUUID;
    pstPort->wRxIoUUID  = wRxIoUUID;
    pstPort->wFilter	= wRxIoUUID;
    pstPort->pbyIoSrc   = pbyIoSrc;
    pstPort->pbyRx      = pbyBuff;
    pstPort->wMaxLen    = dwMaxLen;
    pstPort->wMyDevID   = s_XcanMyDevID;
    pstPort->dwCommChnID= pstCommChn->dwComChnID;
    pstPort->wRemoteDevID = wRemoteDevID;

    for(int i = 0; i < XCAN_PORT_TRS_CHN_NUM; i++ ) {
        pstPort->stTx[i].bySta = XCAN_TXSTA_IDLE;
    }

    // 配置远程设备属性
    pstAlianDev = GetRemoteAlianDev(wRemoteDevID);
    if (pstAlianDev == NULL) return 0;

    pstPort->pstRemoteAlian = pstAlianDev;
    pstPort->pstRemoteAlian->dwLinkIoNumBit <<= 1;
    pstPort->pstRemoteAlian->dwLinkIoNumBit |= 0x01;

    pstPort->dwInDevQun = pstPort->pstRemoteAlian->dwLinkIoNum;
    pstPort->pstRemoteAlian->dwLinkIoNum ++;
    pstPort->pstRemoteAlian->dwCommChnID = pstCommChn->dwComChnID;

    // 配置通信接口
    pstCommDevChn = GetCommChn(pstCommChn->dwComChnID);
    if (pstCommChn == NULL) return 0;

    pstPort->pstCommOps = pstCommDevChn;
    pstPort->pstCommOps->stChn = *pstCommChn;

    XcanInInit(&pstPort->pstCommOps->stChn);
    XcanOutInit(&pstPort->pstCommOps->stChn);

    if(pstCommChn->pfAddFilter) pstCommChn->pfAddFilter(pstCommChn->dwComChnID, pstPort->wFilter, 14);
    pstPort->blUsing = true;
    return pstPort->dwUUID;
}

/*******************************************************************************
函数功能：
*   @ 向xcan协议增加可以传输数据的设备，包括其设备的IO资源，数据缓冲地址等。
参数描述：
*	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
*   @ short unsigned int wMyDevID ：本资源所属的本地设备ID
*   @ unsigned int dwRemoteDevID ：本资源所属的远程设备ID
*   @ unsigned short int wIoUUID ： 本系统内资源的唯一ID定义
*   @ unsigned char *pbyIoSrc ： 注册设备的IO资源地址，在接受到正确数据后协议会自动写入，不用注册为NULL
*   @ unsigned char *pbyBuff ： 协议在接受数据时需要的数据缓存
*   @ unsigned short int wMaxLen ： IO口资源能够操作的最大数据长度
返回值描述：
*   @ -1：增加设备失败
*   @  0：增加设备成功
说明：
*   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
*   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
*******************************************************************************/
XCAN_STATIC int AddRxPort(stCommChnTdf *pstCommChn,
    short unsigned int wRemoteDevID,
    unsigned short int wIoUUID,
    unsigned char *pbyIoSrc, unsigned char *pbyBuff,
    unsigned int dwMaxLen)
{
    return AddPort(pstCommChn, wRemoteDevID, wIoUUID, pbyIoSrc, pbyBuff, dwMaxLen, true);
}

/*******************************************************************************
函数功能：
*   @ 向xcan协议增加可以传输数据的设备，包括其设备的IO资源，数据缓冲地址等。
参数描述：
*	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
*   @ short unsigned int wMyDevID ：本资源所属的本地设备ID
*   @ short unsigned int wRemoteDevID ：本资源所属的远程设备ID
*   @ unsigned short int wIoUUID ： 本系统内资源的唯一ID定义
*   @ unsigned char *pbyIoSrc ： 注册设备的IO资源地址，在接受到正确数据后协议会自动写入，不用注册为NULL
*   @ unsigned char *pbyBuff ： 协议在接受数据时需要的数据缓存
*   @ unsigned short int wMaxLen ： IO口资源能够操作的最大数据长度
返回值描述：
*   @ -1：增加设备失败
*   @  0：增加设备成功
说明：
*   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
*   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
*   @ 2020.11.14 增加本地设备ID参数
*******************************************************************************/
XCAN_STATIC int AddTxPort(stCommChnTdf *pstCommChn,
    short unsigned int wRemoteDevID,
    unsigned short int wIoUUID,
    unsigned char *pbyIoSrc, unsigned char *pbyBuff,
    unsigned int dwMaxLen)
{
    return AddPort(pstCommChn, wRemoteDevID, wIoUUID, pbyIoSrc, pbyBuff, dwMaxLen, false);
}

/*******************************************************************************
函数功能：
*   @ 向xcan协议增加IO资源，数据缓冲地址等,无数据发送方式
参数描述：
*   @ unsigned int dwRemoteDevID ：本资源所属的远程设备ID
*   @ unsigned short int wIoUUID ： 本系统内资源的唯一ID定义
*   @ unsigned char *pbyIoSrc ： 注册设备的IO资源地址，在接受到正确数据后协议会自动写入，不用注册为NULL
*   @ unsigned char *pbyBuff ： 协议在接受数据时需要的数据缓存
*   @ unsigned short int wMaxLen ： IO口资源能够操作的最大数据长度。
*	@ bool blIsRxIO 是否为接收资源
返回值描述：
*   @ -1：增加失败
*   @  0：增加成功
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int AddIo(short unsigned int wRemoteDevID,
    unsigned int dwCommChnID,
    short unsigned int wIoUUID,
    unsigned char *pbyIoSrc, unsigned char *pbyBuff,
    unsigned int dwMaxLen, bool blIsRxIO)
{
    unsigned int dwUUID;
    DevPortTdf *pstPort;
    stRemoteAlianDevTdf *pstAlianDev = NULL;

    unsigned short int wOrigalIoUUID = wIoUUID;
    unsigned short int wTxIoUUID;
    unsigned short int wRxIoUUID;
    unCanFilterTdf wID2Bit14;

    /*
    * 使用14位的ID，必须将bit7清除，bit6置1
    * 高位ID与地位ID不可以相同
    */
    wID2Bit14.wId = wIoUUID;
    wID2Bit14.bitId.IDH_RES = 0;
    wID2Bit14.bitId.IDH |= 0x40;
    wID2Bit14.bitId.IDL_RES = 0;
    wID2Bit14.bitId.IDL |= 0x40;

    if (wID2Bit14.bitId.IDH == wID2Bit14.bitId.IDL) return -1;
    wIoUUID = wID2Bit14.wId;

    unsigned char UUIDL = wIoUUID & 0x007f;
    unsigned char UUIDH = (wIoUUID >> 8) & 0x007f;

    if (blIsRxIO == true) {
        wTxIoUUID = ((unsigned short int)UUIDH << 7) + UUIDL;
        wRxIoUUID = ((unsigned short int)UUIDL << 7) + UUIDH;
    }
    else {
        wTxIoUUID = ((unsigned short int)UUIDL << 7) + UUIDH;
        wRxIoUUID = ((unsigned short int)UUIDH << 7) + UUIDL;
    }

    wIoUUID = wRxIoUUID;
    dwUUID = ((unsigned int)wRemoteDevID << 16) + wOrigalIoUUID;

    pstPort = s_stPort;
    for (int i = 0; i < SURPORT_MAX_PORT; i++, pstPort++) {
        if (dwUUID == pstPort->dwUUID) return 0;
        if (!pstPort->blUsing) break;
    }

    if (pstPort->blUsing) return 0;

    pstPort->blActive = true;
    pstPort->blImRxIO = blIsRxIO;
    pstPort->wIoUUID = wIoUUID;
    pstPort->wOrigalIoUUID = wOrigalIoUUID;
    pstPort->dwUUID = dwUUID;
    pstPort->dwDevComUUID = 0;
    pstPort->wTxIoUUID = wTxIoUUID;
    pstPort->wRxIoUUID = wRxIoUUID;
    pstPort->wFilter = wRxIoUUID;
    pstPort->pbyIoSrc = pbyIoSrc;
    pstPort->pbyRx = pbyBuff;
    pstPort->wMaxLen = dwMaxLen;
    pstPort->wMyDevID = s_XcanMyDevID;
    pstPort->dwCommChnID = dwCommChnID;
    pstPort->wRemoteDevID = wRemoteDevID;

    for (int i = 0; i < XCAN_PORT_TRS_CHN_NUM; i++) {
        pstPort->stTx[i].bySta = XCAN_TXSTA_IDLE;
    }

    // 配置远程设备属性
    pstAlianDev = GetRemoteAlianDev(wRemoteDevID);
    if (pstAlianDev == NULL) return 0;

    pstPort->pstRemoteAlian = pstAlianDev;
    pstPort->pstRemoteAlian->dwLinkIoNumBit <<= 1;
    pstPort->pstRemoteAlian->dwLinkIoNumBit |= 0x01;

    pstPort->dwInDevQun = pstPort->pstRemoteAlian->dwLinkIoNum;
    pstPort->pstRemoteAlian->dwLinkIoNum ++;
    pstPort->pstRemoteAlian->dwCommChnID = dwCommChnID;

    // 配置通讯接口
    pstPort->pstCommOps = NULL;
    pstPort->blUsing = true;

    return pstPort->dwUUID;
}

/*******************************************************************************
函数功能：
*   @ 向xcan协议增加可以传输数据的设备。
参数描述：
*	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
返回值描述：
*   @ -1：增加设备失败
*   @  0：增加设备成功
说明：
*   @ 无
*******************************************************************************/
XCAN_STATIC int AddCommIF(stCommChnTdf *pstCommChn)
{
    stCommDevChnTdf *pstCommDevChn = NULL;

    // 配置通信接口
    pstCommDevChn = GetCommChn(pstCommChn->dwComChnID);
    if (pstCommChn == NULL) return 0;

    pstCommDevChn->stChn = *pstCommChn;
    XcanInInit(pstCommChn);
    XcanOutInit(pstCommChn);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 将通信设备绑定到IO port。
参数描述：
*	@ u16 wIoUUID 设备的IO UUID
*   @ u16 wRemoteDevID 远程设备ID
*   @ u32 dwDInID 数据输入ID
返回值描述：
*   @ 被更新的port
说明：
*   @ 自动学习数据来源并认证数据端口
*******************************************************************************/
DevPortTdf *UpdatePortDinID(short unsigned int wIoUUID, short unsigned int wRemoteDevID, unsigned int dwDInID)
{
    DevPortTdf *pstRet = NULL;
    DevPortTdf *pstPort = s_stPort;

    for (int i = 0; i < SURPORT_MAX_PORT; i++, pstPort++) {
        if (pstPort->blUsing == false) break;
        if (pstPort->wIoUUID == wIoUUID && pstPort->wRemoteDevID == wRemoteDevID) {
            pstPort->dwDevComUUID = ((dwDInID << 16) + wIoUUID);
            pstPort->dwDInID = dwDInID;
            pstRet = pstPort;
            break;
        }
    }

    return pstRet;
}

/*******************************************************************************
函数功能：
*   @ 将port 绑定到 COMM。
参数描述：
*	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
返回值描述：
*   @ -1：绑定失败
*   @  0：绑定成功
说明：
*   @ 无
*******************************************************************************/
int BindPort2CommChn(unsigned int dwUUID)
{
    // 需要增加将IO绑定到通信接口的功能
    stCommDevChnTdf *pstCommChn = NULL;
    DevPortTdf *pstPort = GetPort(dwUUID);

    if (pstPort == NULL) return -1;
    if (pstPort->dwCommChnID == 0) return -1;

    pstCommChn = GetCommChn(pstPort->dwCommChnID);
    if (pstCommChn == NULL) return -1;

    pstPort->pstCommOps = pstCommChn;
    if(pstCommChn->stChn.pfAddFilter) pstCommChn->stChn.pfAddFilter(pstCommChn->stChn.dwComChnID, pstPort->wFilter, 14);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置本机的通信设备ID
参数描述：
*	@ short unsigned int wMyDevID 通信ID
返回值描述：
*   @ 输入接口的函数地址
说明：
*   @ 设置本机的XCAN内核的设备ID，用于识别资源信息
*******************************************************************************/
XCAN_STATIC void SetMyDevID(short unsigned int wMyDevID)
{
    s_XcanMyDevID = wMyDevID;
}

/*******************************************************************************
函数功能：
*   @ 获取协议栈的输入接口。
参数描述：
*	@ 无
返回值描述：
*   @ 输入接口的函数地址
说明：
*   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
*   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
*******************************************************************************/
XCAN_STATIC XcanCoreInPortCb GetCoreInPort(void)
{
    return GetInportFromIn();
}

/*******************************************************************************
函数功能：
*   @ 获取协议栈的输入接口。
参数描述：
*	@ 无
返回值描述：
*   @ 输入接口的函数地址
说明：
*   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
*   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
*******************************************************************************/
XCAN_STATIC void SetMutex(void (*pGetMutex)(void), void(*pPutMutex)(void))
{
    pfGetMutex = pGetMutex;
    pfPutMutex = pPutMutex;
}

/*******************************************************************************
函数功能：
*   @ set time out
参数描述：
*	@ unsigned int dwTim: time
返回值描述：
*   @ no
说明：
*   @ no
*******************************************************************************/
XCAN_STATIC void SetRelinkTim(unsigned int dwTim)
{
    mXCAN_SYNC_SET_RELINK_TIM = dwTim;
}

/*******************************************************************************
函数功能：
*   @ set retrs time out
参数描述：
*	@ unsigned int dwTim: time
返回值描述：
*   @ no
说明：
*   @ no
*******************************************************************************/
XCAN_STATIC void SetReTrsTimOut(unsigned int dwTim)
{
    mXCAN_RE_TRS_TIM = dwTim;
}

#if(PLAT_ARM_CORTEX)
/*******************************************************************************
函数功能：
*   @ cortex 内核关闭异常。
参数描述：
*	@ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void MxCriticalGet(void)
{
    __asm("cpsid i");
    __asm("cpsid f");
}

/*******************************************************************************
函数功能：
*   @ cortex 内核打开异常。
参数描述：
*	@ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void MxCriticalPut(void)
{
    __asm("cpsie i");
    __asm("cpsie f");
}
#endif

static const DevXcanTdf s_Dev = {
    AddRxPort,
    AddTxPort,
    RemovePort,

    Read,
    Write,

    ServicePortInit,
    SetTimPort,
    Service,

    SynSet,
    SynSetDown,
    AddGroup,
    SynOutLink,
    SynInLink,

    MustToRemote,
    RegReceiveNotice,
    UnRegReceiveNotice,
    GetCommSta,

    RegReadWriteNotice,
    UnRegReadWriteNotice,
    IsRdBusy,
    IsWrBusy,

    SetMyDevID,
    GetCoreInPort,

    AddIo,
    AddCommIF,
    PortActive,
    PortDisActive,
    SetMutex,
    .SetRelinkTim = SetRelinkTim,
    .SetReTrsTimOut = SetReTrsTimOut
};

/*******************************************************************************
函数功能：
*   @ 初始化xcan协议设备
参数描述：
*   @ 无
返回值描述：
    串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
const DevXcanTdf *XcanInit(void)
{
    //注册通道,初始化。
    memset(&s_stPort, 0, sizeof(DevPortTdf) * SURPORT_MAX_PORT);
    memset(&s_stRemoteAlianDev, 0, sizeof(stRemoteAlianDevTdf) * XCAN_HW_DEV_SURPORT_NUM);
    memset(&s_stCommChn, 0, sizeof(stCommDevChnTdf) * XCAN_HW_DEV_SURPORT_COMMCHN);
    pfGetSysTim = NULL;

#if(PLAT_ARM_CORTEX)
    pfGetMutex = MxCriticalGet;
    pfPutMutex = MxCriticalPut;
#else
    pfGetMutex = NULL;
    pfPutMutex = NULL;
#endif

    if(mXCAN_SYNC_SET_RELINK_TIM < 5000 || mXCAN_SYNC_SET_RELINK_TIM > 50000) mXCAN_SYNC_SET_RELINK_TIM = 5000;
    if(mXCAN_RE_TRS_TIM < 1000 || mXCAN_RE_TRS_TIM > 30000) mXCAN_RE_TRS_TIM = 1000;
    return &s_Dev;
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */
