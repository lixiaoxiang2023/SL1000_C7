/*******************************************************************************
文件名称：   xcan_sync.c
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2018.9.18
程序用途：   xcan协议,用于数据同步与错误重传

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2018.9.18
修改人：胥强
修改内容：初始编写

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
extern DevPortTdf s_stPort[SURPORT_MAX_PORT];
extern unsigned int (*pfGetSysTim)(void);

/*******************************************************************************
外部全局变量申明
*******************************************************************************/
unsigned int mXCAN_SYNC_SET_RELINK_TIM = 5000;

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 判断是否有有数据差异，如果有准备开始同步
参数描述：
*   @ 无
返回值描述：
*   @ -1：发现差异
*   @  0：未发现差异
说明：
*   @ 无
*******************************************************************************/
static bool HasDiff( DevPortTdf *pstPort )
{
    int dwDiffCnt = 0;
    unsigned char *pbyIO;
    unsigned char *pbyMonitor;
    __stSyncLocal *pstTrs = &pstPort->stSyncTrs.stSynLoacl;

    if(!pstPort) return false;
    if(!pstTrs->pbyShadow) return false;
    if(!pstTrs->pbyBuffer) return false;
    if(pstTrs->blWorking == false) return false;

    pbyIO = pstPort->pbyIoSrc + pstTrs->wMonitorStart;
    pbyMonitor = pstTrs->pbyShadow + pstTrs->wMonitorStart;

    // 计算有多少个差异的数据，找到差异起始点，与结束点
    for(dwDiffCnt = 0; dwDiffCnt < (pstTrs->wMonitorEnd - pstTrs->wMonitorStart); dwDiffCnt ++) {
        if(*pbyIO ++ != *pbyMonitor ++) return true;
    }

    return false;
}

/*******************************************************************************
函数功能：
*   @ 影子缓冲更新
参数描述：
*   @ DevPortTdf *pstPort ：硬件接收到的29bit的id
*   @ stHeadTdf *pstHead ： 帧头数据
*   @ unsigned char *pbyIn ： 本次接收到的数据地址，数据1为IO资源的操作的起始地址，数据2为读取长度
*   @ unsigned char byLen ： 本次接收到的数据长度
返回值描述：
*   @ 无
说明：
*   @ 在多组通信过程中，可能出现两种情况的数据写入，1：自己读的数据传入，需要更新等待状态
*   @ 2：设备主动数据写入，需要发送确认帧
*******************************************************************************/
static void SyncUpdate( DevPortTdf *pstPort, unsigned int dwAddrStart )
{
    unsigned char *pbyS;
    unsigned char *pbyD = pstPort->stSyncTrs.stSynLoacl.pbyShadow;
    unsigned char *pbyIn = pstPort->stSyncTrs.stSynLoacl.pbyBuffer;

    unsigned char byBitMapLen = pbyIn[2];
    unsigned char byBitMap;

    if(pbyD == NULL) return;
    if(byBitMapLen > 128) return; // 最大支持128*8的差异数据传输；
    if(byBitMapLen == 0) return;

    pbyD += dwAddrStart;
    pbyS = &pbyIn[byBitMapLen + 3];
    for(int i = 0 ; i < byBitMapLen; i ++ ) {
        byBitMap = pbyIn[3 + i];

        if(byBitMap == 0) {
            pbyD += 8;
            continue;
        }

        for(int j = 0; j < 8; j ++, pbyD ++) {
            if(byBitMap & 0x80) *pbyD = *pbyS ++;

            byBitMap <<= 1;
            if(byBitMap == 0 && j != 7) {
                pbyD += (8 - j);
                break;
            }
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 将IO资源区的地址与影子区进行对比，并将差异打包进缓存。
参数描述：
*   @ 无
返回值描述：
*   @ -1：发现差异
*   @  0：未发现差异
说明：
*   @ 无
*******************************************************************************/
static int GetDiff( DevPortTdf *pstPort)
{
    short unsigned int wStart = 0;
    short unsigned int wEnd = 0;
    int dwCharCnt = 0;
    int dwDiffCnt = 0;
    unsigned char *pbyIO;
    unsigned char *pbyMonitor;
    unsigned char *pbyBuff;
    unsigned char *pbyBitMap;
    __stSyncLocal *pstTrs = &pstPort->stSyncTrs.stSynLoacl;

    short unsigned int wRealDiff = 0;
    short unsigned int wHasFindDiff = 0;
    short unsigned int wRealFindDiff = 0;

    unsigned char BitMap = 0;
    unsigned char byBitMapCnt = 0;
    bool blFindStart = false;

    if(!pstPort) return -1;
    if(!pstTrs->pbyShadow) return -1;
    if(!pstTrs->pbyBuffer) return -1;
    if(pstTrs->blWorking == false) return -1;
    if(pstPort->stTx[XCAN_FRAME_SYNC_WRITE].bySta != XCAN_TXSTA_IDLE && pstPort->stTx[XCAN_FRAME_SYNC_WRITE].bySta != XCAN_TXSTA_END) return -1;

    // 将当前的数据备份
    pbyIO = pstPort->pbyIoSrc + pstTrs->wMonitorStart;
    pbyMonitor = pstTrs->pbyShadow + pstTrs->wMonitorStart;

    // 将当前的数据备份, 并在备份的区域查找差异
    // memcpy(pstTrs->pbyIoCache, pbyIO, pstTrs->wMonitorEnd - pstTrs->wMonitorStart);
    // pbyIO = pstTrs->pbyIoCache;

    wRealDiff = 0;
    // 计算有多少个差异的数据，找到差异起始点，与结束点
    for(dwDiffCnt = 0; dwDiffCnt < (pstTrs->wMonitorEnd - pstTrs->wMonitorStart); dwDiffCnt ++) {
        if(*pbyIO != *pbyMonitor) {
            if (blFindStart == true) wEnd = dwDiffCnt;
            else {
                blFindStart = true;
                wEnd = wStart = dwDiffCnt;
            }

            wRealDiff ++;
        }

        pbyIO ++; pbyMonitor ++;
    }

    // 是否有差异
    dwCharCnt = wEnd - wStart;
    if(wRealDiff == 0) return -1;

    dwCharCnt ++;

    // 发送包序号
    pstTrs->pbyBuffer[0] = (pstTrs->wTxPkgID & 0xff);
    pstTrs->pbyBuffer[1] = ((pstTrs->wTxPkgID >> 8) & 0xff);
    pstTrs->pbyBuffer[2] = (unsigned char)(dwCharCnt / 8);
    if (dwCharCnt % 8) pstTrs->pbyBuffer[2] ++;

    pbyIO = pstPort->pbyIoSrc + pstTrs->wMonitorStart + wStart;
    pbyMonitor = pstTrs->pbyShadow + pstTrs->wMonitorStart + wStart;
    pbyBitMap = &pstTrs->pbyBuffer[3];
    pbyBuff = &pstTrs->pbyBuffer[3 + pstTrs->pbyBuffer[2]];
    dwCharCnt = pstTrs->pbyBuffer[2] * 8;

    // 开始压缩
    byBitMapCnt = 0;
    wHasFindDiff = 0;
    wRealFindDiff = 0;
    for(dwDiffCnt = 0; dwDiffCnt < dwCharCnt && wHasFindDiff < wRealDiff; dwDiffCnt ++ , pbyIO ++, pbyMonitor ++) {
        if(*pbyIO != *pbyMonitor) {
            BitMap |= (0x01 << (7 - byBitMapCnt));
            *pbyBuff ++ = *pbyIO;
            wHasFindDiff ++;
        }

        wRealFindDiff ++;
        byBitMapCnt ++;
        if(byBitMapCnt == 8 || wHasFindDiff == wRealDiff) {
            *pbyBitMap ++ = BitMap;
            byBitMapCnt = 0;
            BitMap = 0;
            continue;
        }
    }

    if(!AddTrs ( pstPort, XCAN_FRAME_SYNC_WRITE, (pstTrs->wMonitorStart + wStart), pstTrs->pbyBuffer, 3 + pstTrs->pbyBuffer[2] + wHasFindDiff)) {
        // SyncUpdate(pstPort, pstTrs->wMonitorStart + wStart);
        pstTrs->emXcanSyncTrsSta = XcanTrs_WaitAck;
        pstTrs->wSyncIoStart = pstTrs->wMonitorStart + wStart;
        pstTrs->wTxPkgSN ++;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 同步连接管理。
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int SyncMgr(DevPortTdf *pstPort)
{
    unsigned char *pbyCpy = NULL;
    unsigned int dwSysTim = 0;
    stSynTrsSetTdf stSyn;
    __stSynReq *pstTrs = &pstPort->stSyncTrs.stSynReq;

    if(pstTrs->blNeedSync == false) return 0;
    // if(pstTrs->blSyncLink == true) return 0;
    if(!pfGetSysTim) return -1;

    /*
    * 对主设备，即资源不拥有设备，不广播信息，当远程发送设备连接到本设备时，主设备再广播信息
    */
    // if (pstPort->blImRxIO == true) {
    // 	if (pstPort->stSyncTrs.stSynLoacl.blWorking == false) return 0;
    // }

    dwSysTim = pfGetSysTim();
    if ((pstPort->pstRemoteAlian->dwLinkIoNumBit & pstPort->pstRemoteAlian->dwLinkIoOnBit) == pstPort->pstRemoteAlian->dwLinkIoNumBit) {
        if(pstPort->pstRemoteAlian->dwTimDelay > dwSysTim) return 0;
        pstPort->pstRemoteAlian->dwTimDelay = dwSysTim + mXCAN_SYNC_SET_RELINK_TIM;
    }
    else {
        if (pstTrs->dwSysTim > dwSysTim) return 0;
        pstTrs->dwSysTim = dwSysTim + mXCAN_SYNC_SET_RELINK_TIM;
    }

    stSyn.bySetCmd = 1;
    stSyn.unStart.wStart = pstTrs->wMonitorStart;
    stSyn.unEnd.wEnd = pstTrs->wMonitorEnd;
    stSyn.unTim.wTim = 0;

    pbyCpy = pstPort->stTx[XCAN_FRAME_SYNC_SET].byaTx;
    *pbyCpy ++ = stSyn.bySetCmd;
    *pbyCpy ++ = (unsigned char) stSyn.unStart.wStart;
    *pbyCpy ++ = (unsigned char) (stSyn.unStart.wStart >> 8);
    *pbyCpy ++ = (unsigned char) stSyn.unEnd.wEnd;
    *pbyCpy ++ = (unsigned char) (stSyn.unEnd.wEnd >> 8);
    *pbyCpy ++ = 0;
    *pbyCpy ++ = (pstPort->wMyDevID) & 0xff;
    *pbyCpy ++ = pstPort->wMyDevID >> 8;

    // memcpy(pstPort->stTx[XCAN_FRAME_SYNC_SET].byaTx, (unsigned char*)&stSyn, 5);
    return AddTrs ( pstPort, XCAN_FRAME_SYNC_SET, 0, pstPort->stTx[XCAN_FRAME_SYNC_SET].byaTx, 8);
}

/*******************************************************************************
函数功能：
*   @ 初始化服务接口，主要用于数据同步与错误检查与重传。
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int ServicePortInit(unsigned int dwUUID, unsigned char *pbyShadow, unsigned char *pbyShadowBuff, unsigned char *pbyIoCache)
{
    DevPortTdf *pstPort = GetPort(dwUUID);
    if (pstPort == NULL) return -1;

    pstPort->stSyncTrs.stSynLoacl.pbyShadow    = pbyShadow;
    pstPort->stSyncTrs.stSynLoacl.pbyBuffer    = pbyShadowBuff;
    pstPort->stSyncTrs.stSynLoacl.pbyIoCache   = pbyIoCache;
    // pstPort->Err = NULL;

    memset(pstPort->stSyncTrs.stSynLoacl.pbyShadow, 0, pstPort->wMaxLen);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 协议栈管理接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 监控自动同步功能与超时数据,在发现有数据变化后的10ms后开始处理同步，预留10ms让应用完成数据的写入
*******************************************************************************/
void Service(void)
{
    unsigned int dwTime = 0;
    DevPortTdf *pstPort = s_stPort;
    __stSyncLocal *pstTrs = NULL;

    // 处理同步差异
    if(!pfGetSysTim) return;
    for(int port  = 0 ; port < SURPORT_MAX_PORT; port ++, pstPort ++) {
        if (pstPort->blUsing == false) continue;
        if (pstPort->pstCommOps == NULL) {
            if(BindPort2CommChn(pstPort->dwUUID) != 0) continue;
        }

        if (pstPort->blActive == false) continue;

        pstTrs = &pstPort->stSyncTrs.stSynLoacl;
        // 收到到同步数据返回，更新数据
        if (pstTrs->emXcanSyncTrsSta == XcanTrs_GetAck) {
            pstTrs->emXcanSyncTrsSta =  XcanTrs_Idle;
            if(pstTrs->wRxPkgID == pstTrs->wTxPkgID) {
                SyncUpdate(pstPort, pstTrs->wSyncIoStart);
                pstTrs->wTxPkgID ++;
            }
            else {
                pstTrs->dwPkgLost ++;
            }
        }

        dwTime = pfGetSysTim();
        if ((pstTrs->emXcanSyncTrsSta == XcanTrs_Idle || pstTrs->dwTimDelay < dwTime + 500) && \
                pstPort->stSyncTrs.stSynReq.blSyncLink == true) {
            // 在被远程连接的情况下才会检查差异。并启动传输，同时当同步数据有返回或者超时500ms后启动扫描
            if (pstTrs->emXcanSyncSta == XCAN_SYNC_IDLE) {
                if (HasDiff(pstPort)) {
                    pstTrs->emXcanSyncSta = XCAN_SYNC_WAIT;
                    pstTrs->dwTimDelay = dwTime + 10;
                }
            }
            else {
                if (pstTrs->dwTimDelay < dwTime) {
                    XCAN_GET_MUTEX()
                    GetDiff(pstPort);
                    XCAN_PUT_MUTEX()
                    pstTrs->emXcanSyncSta = XCAN_SYNC_IDLE;
                }
            }
        }

        // 检查数据传输错误
        CheakResetTrs(pstPort, XCAN_FRAME_WRITE);
        CheakResetTrs(pstPort, XCAN_FRAME_READ);
        CheakResetTrs(pstPort, XCAN_FRAME_SYNC_SET);
        CheakResetTrs(pstPort, XCAN_FRAME_SYNC_WRITE);

        // 同步连接管理
        SyncMgr(pstPort);
    }
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */
