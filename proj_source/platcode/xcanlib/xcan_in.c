/*******************************************************************************
文件名称：   xcan_in.c
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20130721
程序用途：   xcan协议,用于数据输入处理

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2016.03.19
修改人：胥强
修改内容：重新整理协议内容，并将协议结果模块化

2016.05.24
修改人：胥强
1、修改书发送逻辑，之前存在长数据帧发送出错问题
2、增加debug信息功能，用于调试
2018.9.15 胥强，统一数数据处理，并增加自动数据回传功能
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

/*******************************************************************************
外部全局变量申明
*******************************************************************************/
#if(XCAN_DEBUG)
extern const char *canFrameType[4];
extern const char *canFrame[5];
#endif

extern unsigned int(*pfGetSysTim)(void);
extern int XcanSyncSetOn(unsigned int dwUUID, short unsigned int wStart, short unsigned int wEnd);
extern unsigned int mXCAN_SYNC_SET_RELINK_TIM;

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 当接收到主动传入的数据时，本机应该返回数据接收成功的确认帧，此帧使用的数据传输通道为 确认帧通道
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 在确认帧中，真编码段将返回写入数据的IO起始地址，数据区第一个字节为数据起始地址，
*   @ 第二个数据为本次写入的数据长度
*   @ 20190530 add link status cheak.
*******************************************************************************/
static void WriteAck(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLen)
{
    unsigned int dwRetDLen = 4;
    unsigned int dwRetD = 0;
    unsigned short int wCnt = 0, wSN = 0;
    unsigned char byFrameType;
    short unsigned int wIoStart = pstHead->stHead.bit10Pkg_Addr;

    if(!pstPort) return;

    if(pstHead->stHead.bit3Type == XCAN_FRAME_WRITE) byFrameType = XCAN_FRAME_WRITE_ACK;
    else if(pstHead->stHead.bit3Type == XCAN_FRAME_SYNC_SET) byFrameType = XCAN_FRAME_SYNC_SET_ACK;
    else if(pstHead->stHead.bit3Type == XCAN_FRAME_SYNC_WRITE) byFrameType = XCAN_FRAME_SYNC_WRITE_ACK;
    else return;

    // 回传数据长度和连接远程状态,只有 XCAN_FRAME_ACK 和 XCAN_FRAME_SYNC_SET_ACK XCAN_FRAME_SYNC_WRITE_ACK
    dwRetD = pstPort->stSyncTrs.dwMySuncID ++;

    pstPort->stTx[byFrameType].byaTx[0] = (dwRetD & 0xff);
    pstPort->stTx[byFrameType].byaTx[1] = (dwRetD >> 8) & 0xff;
    pstPort->stTx[byFrameType].byaTx[2] = (dwRetD >> 16) & 0xff;
    pstPort->stTx[byFrameType].byaTx[3] = (dwRetD >> 24) & 0xff;

    if(byFrameType == XCAN_FRAME_SYNC_WRITE_ACK) {
        // 回写包序号
        wCnt = pstPort->stSyncTrs.stSynReq.wPkgID;
        wSN  = pstPort->stSyncTrs.stSynReq.wPkgSN;

        pstPort->stTx[byFrameType].byaTx[4] = (wCnt & 0xff);
        pstPort->stTx[byFrameType].byaTx[5] = (wCnt >> 8) & 0xff;

        pstPort->stTx[byFrameType].byaTx[6] = (wSN & 0xff);
        pstPort->stTx[byFrameType].byaTx[7] = (wSN >> 8) & 0xff;

        dwRetDLen = 8;
    }

    AddTrs ( pstPort, byFrameType, wIoStart, pstPort->stTx[byFrameType].byaTx, dwRetDLen);
}

/*******************************************************************************
函数功能：
*   @ 当接收到数据发送的头时，表明多帧数据开始传输
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
void Head(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLenIn)
{
    //if(pstPort->stRx.blIsRxing) return;
    if(pstHead->stHead.bit10Pkg_Addr < 2) return;
    if(pstPort == NULL || pstPort->pbyRx == NULL) return;
    if(dwLenIn > pstPort->pstCommOps->stChn.dwFrameDataLen) return;

    pstPort->stRx.blIsRxing = true;
    pstPort->stRx.wRxPkgMun = pstHead->stHead.bit10Pkg_Addr;
    memcpy(pstPort->pbyRx, pbyIn, dwLenIn);

    pstPort->stRx.wNeedPkgNo = pstHead->stHead.bit10Pkg_Addr - 1;
    pstPort->stRx.wRxPkgCnt = 1;
}

/*******************************************************************************
函数功能：
*   @ 当接收到数据发送的中间帧时
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 在接收中间帧时，将会根据接收协议，验证帧编号，若帧编号验证失败，接收状态将设定为初始值，接收失败。
*******************************************************************************/
void Mid(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLenIn)
{
    unsigned int dwFrameLen = pstPort->pstCommOps->stChn.dwFrameDataLen;

    if(!pstPort->stRx.blIsRxing) return;
    if(pstPort->stRx.wNeedPkgNo != pstHead->stHead.bit10Pkg_Addr) {
        //处理出错的数据处理管理
        pstPort->stRx.blIsRxing = false;
        return;
    }

    memcpy(pstPort->pbyRx + (pstPort->stRx.wRxPkgCnt * dwFrameLen), pbyIn, dwLenIn);
    pstPort->stRx.wNeedPkgNo = pstHead->stHead.bit10Pkg_Addr - 1;
    pstPort->stRx.wRxPkgCnt ++;
}

/*******************************************************************************
函数功能：
*   @ 同步数据解包
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 按照比特压缩进行数据的解包
*******************************************************************************/
static void SyncUnpack(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLenIn)
{
    unsigned char *pbyS;
    unsigned char *pbyD = pstPort->pbyIoSrc;

    unsigned char byBitMapLen = pbyIn[2];
    unsigned char byBitMap;

    if(pbyD == NULL) return;
    if(byBitMapLen > 128) return; // 最大支持128*8的差异数据传输；
    if(byBitMapLen == 0) return;

    pbyD += pstHead->stHead.bit10Pkg_Addr;
    if(pbyD > pstPort->pbyIoSrc + pstPort->wMaxLen) return;

    // 取得包序号
    pstPort->stSyncTrs.stSynReq.wPkgID = ((unsigned short int) pbyIn[1] << 8) + pbyIn[0];
    if (pstPort->stSyncTrs.stSynReq.wPkgID == 0) pstPort->stSyncTrs.stSynReq.wPkgSN = 0;
    else pstPort->stSyncTrs.stSynReq.wPkgSN ++;

    XCAN_GET_MUTEX();
    pbyS = &pbyIn[byBitMapLen + 3];
    for(int i = 0 ; i < byBitMapLen; i ++ ) {
        byBitMap = pbyIn[3 + i];

        if(byBitMap == 0) {
            pbyD += 8;
            continue;
        }

        for(int j = 0; j < 8; j ++, pbyD ++) {
            if(pbyD > pstPort->pbyIoSrc + pstPort->wMaxLen) return;
            if(byBitMap & 0x80) *pbyD = *pbyS ++;

            byBitMap <<= 1;
            if(byBitMap == 0 && j != 7) {
                pbyD += (8 - j);
                break;
            }
        }
    }
    XCAN_PUT_MUTEX();
}

/*******************************************************************************
函数功能：
*   @ 当接收到数据发送的结尾帧时，表明数据接收到
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned int dwLenIn ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 结束帧，并完成数据的拷贝处理。
*******************************************************************************/
static void End(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLenIn)
{
    unsigned int dwFrameLen = pstPort->pstCommOps->stChn.dwFrameDataLen;
    unsigned int dwLen;
    short unsigned int wIoStart = pstHead->stHead.bit10Pkg_Addr;

    if( !pstPort->stRx.blIsRxing ) return;
    if( pstPort->stRx.wNeedPkgNo != 1 || pstPort->stRx.wRxPkgMun <= 1) {
        //处理错误，数据丢失
        pstPort->stRx.blIsRxing = false;
        return;
    }

    // 数据接收完成
    dwLen = (pstPort->stRx.wRxPkgMun - 1) * dwFrameLen;
    if((dwLen + dwLenIn) > pstPort->wMaxLen) return;
    memcpy(pstPort->pbyRx + dwLen, pbyIn, dwLenIn);
    dwLen += dwLenIn;

    if(pstHead->stHead.bit3Type == XCAN_FRAME_SYNC_WRITE) {
        SyncUnpack(pstPort, pstHead, pstPort->pbyRx, dwLen);
        WriteAck(pstPort, pstHead, pbyIn, 2);
        if (pstPort->pfReceiveNotice) pstPort->pfReceiveNotice(pstPort->dwUUID);

        return;
    }

    // 数据接收完成，拷贝数据
    XCAN_GET_MUTEX();
    memcpy(pstPort->pbyIoSrc + wIoStart, pstPort->pbyRx, dwLen);
    XCAN_PUT_MUTEX();
    pstPort->stRx.blIsRxing = false;

    // 如果接收的是对方主动传输数据通道的数据，则需要返回确认数据
    // 如果接收到的是自己读取的数据通道的数据，更新读取通道数据状态
    if(pstHead->stHead.bit3Type == XCAN_FRAME_WRITE) {
        WriteAck(pstPort, pstHead, pbyIn, pstPort->stRx.wRxPkgMun);
    }
    else {
        if(pstPort->stTx[XCAN_FRAME_READ].bySta == XCAN_TXSTA_WAIT_READ) {
            pstPort->stTx[XCAN_FRAME_READ].bySta = XCAN_TXSTA_END;
        }
    }

    if (pstPort->pfReceiveNotice) pstPort->pfReceiveNotice(pstPort->dwUUID);
}

/*******************************************************************************
函数功能：
*   @ 当接收到数据发送的单帧时，表明数据接收到
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void Sig(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLen)
{
    short unsigned int wIoStart;

    if(pstHead->stHead.bit3Type == XCAN_FRAME_SYNC_WRITE) {
        SyncUnpack(pstPort, pstHead, pbyIn, dwLen);
        WriteAck(pstPort, pstHead, pbyIn, 1);
        if (pstPort->pfReceiveNotice) pstPort->pfReceiveNotice(pstPort->dwUUID);

        return;
    }

    // if( pstPort->stRx.blIsRxing ) return;
    wIoStart = pstHead->stHead.bit10Pkg_Addr;
    if(dwLen > pstPort->wMaxLen) return;
    if(dwLen > pstPort->pstCommOps->stChn.dwFrameDataLen) dwLen = pstPort->pstCommOps->stChn.dwFrameDataLen;
    //对于单数据包传输时，功能区搭载的只是IO资源区的数据起始地址
    XCAN_GET_MUTEX();
    memcpy(pstPort->pbyIoSrc + wIoStart, pbyIn, dwLen);
    XCAN_PUT_MUTEX();
    pstPort->stRx.blIsRxing = false;

    // 如果接收的是对方主动传输数据通道的数据，则需要返回确认数据
    // 如果接收到的是自己读取的数据通道的数据，更新读取通道数据状态
    if(pstHead->stHead.bit3Type == XCAN_FRAME_WRITE) {
        WriteAck(pstPort, pstHead, pbyIn, 1);
    }
    else {
        if(pstPort->stTx[XCAN_FRAME_READ].bySta == XCAN_TXSTA_WAIT_READ) {
            pstPort->stTx[XCAN_FRAME_READ].bySta = XCAN_TXSTA_END;
        }
    }

    if (pstPort->pfReceiveNotice) pstPort->pfReceiveNotice(pstPort->dwUUID);
}

/*******************************************************************************
函数功能：
*   @ 当接收到数据取功能帧时，我们需要返回数据，返回数据使用的是读数据返回通道
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 在接收到此帧后，数据写入通道的状态变为传输完成，表明数据传输成功
*******************************************************************************/
static void ReadAck(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLen)
{
    short unsigned int wIoStart;
    unsigned int dwLenTx;

    if(!pstPort) return;

    wIoStart = pstHead->stHead.bit10Pkg_Addr;
    dwLenTx = ((unsigned int)pbyIn[0] + ((unsigned int)pbyIn[1] << 8));
    if(wIoStart + dwLenTx > pstPort->wMaxLen) return;

    AddTrs ( pstPort, XCAN_FRAME_READ_BACK, wIoStart, pstPort->pbyIoSrc + wIoStart, dwLenTx);
}

/*******************************************************************************
函数功能：
*   @ 检查远程设备是否有连接断开的情况，在存在断开的情况时，需要进行重新连接
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char bySta ： 远程连接状态字
返回值描述：
*   @ 是否需要货期远程的现场数据
说明：
*   @ 在接收到此帧后，数据写入通道的状态变为传输完成，表明数据传输成功
*******************************************************************************/
static bool CheakRemoteDoUnlink(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn)
{
    // unsigned char bySta = pbyIn[2];
    unsigned int dwTimDiff = 0;
    unsigned int dwRemoteSyncAckID = 0;

    for (int i = 0; i < 4; i++) {
        dwRemoteSyncAckID <<= 8;
        dwRemoteSyncAckID += pbyIn[3 - i];
    }

    /*
    * dwRemoteSyncAckCnt 远程ack的数据，远程接收到的确认数据计数器；
    * pstPort->stSyncTrs.stSynLoacl.dwSyncAckCnt 本地接收到的数据确认数据
    * 如果本地的数据 < 远程的数据 说明本地设备可能不是最新的现场，需要读取远程数据的现场
    * 如果本地的数据 > 远程的数据 说明远程的数据不是最新，由远程自己恢复需要的数据
    */

    if (pstPort->stSyncTrs.dwRemoteSyncID < dwRemoteSyncAckID) {
        dwTimDiff = dwRemoteSyncAckID - pstPort->stSyncTrs.dwRemoteSyncID;
        if(dwTimDiff < 3 && pstPort->stSyncTrs.dwRemoteSyncID != 0) {
            pstPort->stSyncTrs.dwRemoteSyncID = dwRemoteSyncAckID;
            return false;       // 连接正常
        }
    }

    // 连接异常
    // if (bySta != 0xa5) return false;
    pstPort->stSyncTrs.dwRemoteSyncID = dwRemoteSyncAckID;
    if (pstPort->pstRemoteAlian->dwLinkIoOnBit == pstPort->pstRemoteAlian->dwLinkIoNumBit) {
        pstPort->pstRemoteAlian->dwLinkIoOnBit = 0;
    }

    return true;
}

/*******************************************************************************
函数功能：
*   @ 当接收到写入数据帧的确认帧时，将些数据帧发送通道的状态修改为传输完成
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 在接收到此帧后，数据写入通道的状态变为传输完成，表明数据传输成功
*******************************************************************************/
static void GetWriteAck(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLen)
{
    // 检查复位状态
    CheakRemoteDoUnlink(pstPort, pstHead, pbyIn);

    if(pstPort->stTx[XCAN_FRAME_WRITE].bySta == XCAN_TXSTA_WAIT_ACK) {
        pstPort->stTx[XCAN_FRAME_WRITE].bySta = XCAN_TXSTA_END;
    }
}

/*******************************************************************************
函数功能：
*   @ 同步设置，确认帧
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void GetSyncSetAck(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLenIn)
{
    unsigned short int wLen = pstPort->stSyncTrs.stSynReq.wIoSyncEnd;
    // bool blLastLink = pstPort->stSyncTrs.stSynReq.blSyncLink;
    bool blReread = false;

    // 检查是否远端有断开时间发生，如果有发生， 重新读取
    // 检查复位状态
    if(CheakRemoteDoUnlink(pstPort, pstHead, pbyIn)) blReread = true;

    // 建立连接状态
    pstPort->pstRemoteAlian->dwLinkIoOnBit |= (1 << pstPort->dwInDevQun);
    pstPort->stSyncTrs.stSynReq.blSyncLink = true;

    if (pstPort->stTx[XCAN_FRAME_SYNC_SET].bySta == XCAN_TXSTA_WAIT_ACK) {
        pstPort->stTx[XCAN_FRAME_SYNC_SET].bySta = XCAN_TXSTA_END;
    }

    // 回读状态
    if (/*blLastLink == false || */ blReread == true) {
        if(wLen > pstPort->wMaxLen) return;
#if(XCAN_DATA_RESUME)
        AddTrs ( pstPort, XCAN_FRAME_WRITE, 0/* read start whis 0*/, pstPort->pbyIoSrc, wLen);
#else
        wLen = pstPort->stSyncTrs.stSynReq.wMonitorEnd - pstPort->stSyncTrs.stSynReq.wMonitorStart;

        pstPort->stTx[XCAN_FRAME_READ].byaTx[0] = (wLen & 0xff);
        pstPort->stTx[XCAN_FRAME_READ].byaTx[1] = (wLen >> 8) & 0xff;

        AddTrs ( pstPort, XCAN_FRAME_READ,
                pstPort->stSyncTrs.stSynReq.wMonitorStart,
                pstPort->stTx[XCAN_FRAME_READ].byaTx,
                2);
#endif
    }
}

/*******************************************************************************
函数功能：
*   @ 同步写，确认帧
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址
*   @ unsigned int dwLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void GetSyncWriteAck(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int byLen)
{
    // 检查复位状态
    unsigned short int wID = 0, wSN = 0;
    CheakRemoteDoUnlink(pstPort, pstHead, pbyIn);

    if(pstPort->stTx[XCAN_FRAME_SYNC_WRITE].bySta == XCAN_TXSTA_WAIT_ACK) {
        pstPort->stTx[XCAN_FRAME_SYNC_WRITE].bySta = XCAN_TXSTA_END;

        // 请求同步成功
        pstPort->stSyncTrs.stSynReq.blSyncLink = true;
        if (pstPort->stSyncTrs.stSynLoacl.emXcanSyncTrsSta == XcanTrs_WaitAck) {
            wID = ((unsigned short int) pbyIn[5] << 8) + pbyIn[4];
            pstPort->stSyncTrs.stSynLoacl.wRxPkgID = wID;

            wSN = ((unsigned short int) pbyIn[3] << 7) + pbyIn[6];
            pstPort->stSyncTrs.stSynLoacl.wRxPkgSN = wSN;

            pstPort->stSyncTrs.stSynLoacl.emXcanSyncTrsSta = XcanTrs_GetAck;
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 设置控制资源的自动同步功能开启
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址
*   @ unsigned char byLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 收到的数据将自动按照配置数据长度监控差异数据并返回
*******************************************************************************/
static void SyncOn(DevPortTdf *pstPort, stHeadTdf *pstHead, unsigned char *pbyIn, unsigned int dwLen)
{
    // if(dwLen != 5) return;

    short unsigned int wStart = pbyIn[2] * 256 + pbyIn[1];
    short unsigned int wEnd = pbyIn[4] * 256 + pbyIn[3];

    if(pbyIn[0] == 0) {
        pstPort->stSyncTrs.stSynLoacl.blWorking = false;
        return;
    }

    pstPort->stSyncTrs.stSynLoacl.wMonitorStart = wStart;
    pstPort->stSyncTrs.stSynLoacl.wMonitorEnd   = wEnd;

    WriteAck(pstPort, pstHead, pbyIn, 5);
    pstPort->stSyncTrs.stSynLoacl.blWorking = true;

    // 增加组合控制，检查组透传检查，启动开始同步
    if(pstPort->wFriendID != 0) {
        if(GetPort( pstPort->wFriendID ) == NULL ) return;
        XcanSyncSetOn(pstPort->wFriendID, wStart, wEnd);
    }
}

/*******************************************************************************
函数功能：
*   @ 从can硬件接收数据，同时根据接收的数据帧类型，做不同的处理。
参数描述：
*   @ unsigned int dwInID: 数据输入的通道的ID，用于给定数据输出的参数，决定port的数据发往集图的管道
*   @ unsigned int dwId ：硬件接收到的29bit的id
*   @ unsigned char *pbyIn ： 本次接收到的数据地址
*   @ unsigned char byLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 此方法将在数据接收被调用，在根据帧类型对数据进行不同的处理。
*******************************************************************************/
static void XcanKernelProgMsg( unsigned int dwDInID, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen)
{
    unsigned int dwDevComUUID;
    stHeadTdf stHead;
    DevPortTdf *pstPort;
    unsigned int dwSysTim;
    unsigned short int wIoUUID;
    unsigned char bit2Pkg = 0;

    // 将14位的id转成16位ID7270
    stHead.dwId = dwId;
    wIoUUID = stHead.stHead.bit14IoUUID;

    //检查数据是发往哪个设备
    dwDevComUUID = (dwDInID << 16) + wIoUUID;
    pstPort = GetCommPort(dwDevComUUID);
    if (pstPort == NULL) {
        // 通知有新的连接进入
        if (stHead.stHead.bit3Type != XCAN_FRAME_SYNC_SET) return;
        if (dwLen != 8) return;

        short unsigned int wRemoteDevID = pbyIn[7];
        wRemoteDevID <<= 8;
        wRemoteDevID += pbyIn[6];

        pstPort = UpdatePortDinID( wIoUUID, wRemoteDevID, dwDInID);
        if(pstPort == NULL) return;
    }

    if (pstPort->pstRemoteAlian == NULL) return;
    bit2Pkg = stHead.stHead.bit2Pkg;

#if(XCAN_DEBUG)
    printf("Receive %02x to %02x ", pstPort->byMyId, pstPort->byHisId);
    printf("type:%s ", canFrameType[stHead.stHead.bit3Type]);
    printf("frame:%s ", canFrame[bit2Pkg]);
    printf("byPkg_Addr:%d \r\n", stHead.stHead.byPkg_Addr);
#endif

    if ((pfGetSysTim != NULL) && (stHead.stHead.bit3Type == XCAN_FRAME_WRITE_ACK || \
        stHead.stHead.bit3Type == XCAN_FRAME_READ_BACK || \
        stHead.stHead.bit3Type == XCAN_FRAME_SYNC_WRITE_ACK || \
        stHead.stHead.bit3Type == XCAN_FRAME_SYNC_SET_ACK))
    {
        dwSysTim = pfGetSysTim();
        if(stHead.stHead.bit3Type == XCAN_FRAME_READ_BACK) {
            pstPort->stTx[XCAN_FRAME_READ].dwChnTim = dwSysTim + mXCAN_SYNC_SET_RELINK_TIM;
        }

        if (pstPort->stSyncTrs.stSynReq.blNeedSync == true &&\
            (pstPort->pstRemoteAlian->dwLinkIoNumBit & pstPort->pstRemoteAlian->dwLinkIoOnBit) == pstPort->pstRemoteAlian->dwLinkIoNumBit)
        {
            pstPort->pstRemoteAlian->dwTimDelay = dwSysTim + mXCAN_SYNC_SET_RELINK_TIM;
        }
    }

    switch (stHead.stHead.bit3Type) {
    case XCAN_FRAME_WRITE:
    case XCAN_FRAME_SYNC_WRITE:
    case XCAN_FRAME_READ_BACK:
        switch (bit2Pkg) {
        case XCAN_CODE_HEAD:  Head(pstPort, &stHead, pbyIn, dwLen);  break;
        case XCAN_CODE_MID:   Mid (pstPort, &stHead, pbyIn, dwLen);  break;
        case XCAN_CODE_END:   End (pstPort, &stHead, pbyIn, dwLen);  break;
        case XCAN_CODE_SIG:   Sig (pstPort, &stHead, pbyIn, dwLen);  break;
        default : break;
        }
        break;
    case XCAN_FRAME_READ:
        ReadAck(pstPort, &stHead, pbyIn, dwLen);
        break;
    case XCAN_FRAME_WRITE_ACK:
        GetWriteAck(pstPort, &stHead, pbyIn, dwLen);
        break;
    case XCAN_FRAME_SYNC_SET:
        SyncOn(pstPort, &stHead, pbyIn, dwLen);
        break;
    // 同步ACK
    case XCAN_FRAME_SYNC_SET_ACK:
        GetSyncSetAck(pstPort, &stHead, pbyIn, dwLen);
        break;

    case XCAN_FRAME_SYNC_WRITE_ACK:
        GetSyncWriteAck(pstPort, &stHead, pbyIn, dwLen);
        break;

    default :break;
    }
}

/*******************************************************************************
函数功能：
*   @ C库中获取输入接口地址
参数描述：
*   @ 无
返回值描述：
*   @ 串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
XcanCoreInPortCb GetInportFromIn(void)
{
    return (XcanCoreInPortCb) XcanKernelProgMsg;
}

/*******************************************************************************
函数功能：
*   @ 初始化xcan协议设备
参数描述：
*   @ 无
返回值描述：
*   @ 串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
void XcanInInit(stCommChnTdf *pstCommDev)
{
    if(pstCommDev->pfReceive) pstCommDev->pfReceive(XcanKernelProgMsg);
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */
