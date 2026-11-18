/*******************************************************************************
文件名称：   xcan_out.c
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
2018.9.15
胥强，优化发送部分，独立
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
unsigned int mXCAN_RE_TRS_TIM = 1000;

/*******************************************************************************
外部全局变量申明
*******************************************************************************/
#if(XCAN_DEBUG)
extern const char *canFrameType[8];
extern const char *canFrame[4];
#endif

extern DevPortTdf s_stPort[SURPORT_MAX_PORT];
extern unsigned int (*pfGetSysTim)(void);
extern unsigned int mXCAN_SYNC_SET_RELINK_TIM;

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 8通道数据发送
参数描述：
*   @ bool blTrsOne 对于同一个硬件设备只触发一次传输。
*   @ unsigned int dwDevID 只处理来自同一个硬件设备的数据发送触发主要是用于中断的发送
返回值描述：
*   @ 无
说明：
*   @  1：只发送1个数据
*         在应用调用此函数开始发送时，如果连续多次发送，可能会使用较长的时间，导致、
*         中断进入抢占数据，造成数据的重复发送。同时can的中断为在有数据发送成功后再
*         触发中断，所有在应用开始发送数据时检查是否can的fifo为空，当为空时，不会再触发
*         发送中断，需要先发送给一个数据，用来触发can发送中断。
*      2：保证一个帧的数据发送完后再发送另一帧
*         每个注册的设备有8个发送通道，分别分配给：
*               XCAN_FRAME_WRITE        0：主动的数据写
*               XCAN_FRAME_READ_BACK    1：远程机需要的数据，在这里发送
*               XCAN_FRAME_READ         2：本机需要读数据发送
*               XCAN_FRAME_ACK          3：收到写入本地的确认帧的发送
*				.....
*         此函数将逐个扫描各个设备，同时逐个判断单设备内的各个通道的发送状态，当发现
*         有数据正在发送中，将根据协议发数据，直到发送完成，或者发送满后退出。档再次中断
*         进入后，执行相同的事情。
*         当一个通道发送完成，将自动扫描等待发送的通道数据，当有发现等待发送的数据时，
*         自动将其状态修改为正在发送，进入数据发送状态。
*******************************************************************************/
static bool CoreTxHasFifo(bool blTrsOne, unsigned int dwCommID)
{
    stCommChnTdf *pstCommDev = NULL;
    DevPortTdf *pstPort = s_stPort;
    stHeadTdf stHead;
    unsigned char *pbyTxS;
    unsigned int dwLen;

    for( int i = 0;i < SURPORT_MAX_PORT; i++, pstPort = &s_stPort[i]) {
        if (pstPort->blUsing == false) continue;
        if (pstPort->blActive == false) continue;
        if (pstPort->pstRemoteAlian == NULL) continue;
        if (pstPort->dwCommChnID != dwCommID) continue;

        pstCommDev = &pstPort->pstCommOps->stChn;
        if (pstCommDev->pfIsSendFull(pstPort->dwCommChnID, pstPort->dwDInID) == true) continue;

        for(int chn = 0; chn < XCAN_PORT_TRS_CHN_NUM; chn ++) {
            if(pstPort->stTx[chn].bySta == XCAN_TXSTA_SEND) {
                for(;;) {
                    if(pstCommDev->pfIsSendFull(pstPort->dwCommChnID, pstPort->dwDInID) == true) return false; //没有空闲的fifo了

                    stHead.stHead.bit14IoUUID = pstPort->wTxIoUUID;
                    stHead.stHead.bit3Type = pstPort->stTx[chn].byFrameType;

                    //发送写数据
                    if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_WRITE || pstPort->stTx[chn].byFrameType == XCAN_FRAME_READ_BACK \
                        || pstPort->stTx[chn].byFrameType == XCAN_FRAME_SYNC_WRITE) {
                        if(pstPort->stTx[chn].wPkgNum == 1) {
                            stHead.stHead.bit2Pkg = XCAN_CODE_SIG;
                            stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wIoStart;
                            dwLen = pstPort->stTx[chn].dwLastPkgNum;
                        }
                        else if(pstPort->stTx[chn].wPkgCnt == 0) {
                            stHead.stHead.bit2Pkg = XCAN_CODE_HEAD;
                            stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wPkgNum;
                            dwLen = pstCommDev->dwFrameDataLen;
                        }
                        else if(pstPort->stTx[chn].wPkgCnt == (pstPort->stTx[chn].wPkgNum - 1)) {
                            stHead.stHead.bit2Pkg = XCAN_CODE_END;
                            stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wIoStart;
                            dwLen = pstPort->stTx[chn].dwLastPkgNum;
                        }
                        else {
                            stHead.stHead.bit2Pkg = XCAN_CODE_MID;
                            stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wPkgNum - pstPort->stTx[chn].wPkgCnt;
                            dwLen = pstCommDev->dwFrameDataLen;
                        }
                    }
                    else if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_READ) {
                        stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wIoStart;
                        stHead.stHead.bit2Pkg = XCAN_CODE_SIG;
                        dwLen = (pstPort->stTx[chn].wPkgNum - 1) * pstCommDev->dwFrameDataLen + pstPort->stTx[chn].dwLastPkgNum;
                    }
                    else if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_SYNC_SET) {
                        stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wIoStart;
                        stHead.stHead.bit2Pkg = XCAN_CODE_SIG;
                        dwLen = (pstPort->stTx[chn].wPkgNum - 1) * pstCommDev->dwFrameDataLen + pstPort->stTx[chn].dwLastPkgNum;
                    }
                    else {
                        stHead.stHead.bit10Pkg_Addr = pstPort->stTx[chn].wIoStart;
                        stHead.stHead.bit2Pkg = XCAN_CODE_SIG;
                        dwLen = (pstPort->stTx[chn].wPkgNum - 1) * pstCommDev->dwFrameDataLen + pstPort->stTx[chn].dwLastPkgNum;
                    }

#if(XCAN_DEBUG)
                    printf("Send    %02x to %02x ", pstPort->byMyId, pstPort->byHisId);
                    printf("type:%s ", canFrameType[pstPort->stTx[chn].byFrameType]);
                    printf("frame:%s ", canFrame[stHead.stHead.bit2Pkg]);
                    printf("byPkgNum: %d, bit10Pkg_Addr:%d \r\n", pstPort->stTx[chn].byPkgNum, stHead.stHead.bit10Pkg_Addr);
#endif

                    pbyTxS = pstPort->stTx[chn].pbyTx + (pstPort->stTx[chn].wPkgCnt * pstCommDev->dwFrameDataLen);
                    pstCommDev->pfSend(pstPort->dwCommChnID, pstPort->dwDInID, stHead.dwId, pbyTxS, dwLen);
                    pstPort->stTx[chn].wPkgCnt ++;
                    pstPort->stTx[chn].dwChnTim = pfGetSysTim() + mXCAN_SYNC_SET_RELINK_TIM;

                    // 处理发送结束状态
                    if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_WRITE || pstPort->stTx[chn].byFrameType == XCAN_FRAME_SYNC_WRITE
                       || pstPort->stTx[chn].byFrameType == XCAN_FRAME_SYNC_SET) {
                        if(stHead.stHead.bit2Pkg == XCAN_CODE_SIG || stHead.stHead.bit2Pkg == XCAN_CODE_END) {
                            pstPort->stTx[chn].bySta = XCAN_TXSTA_WAIT_ACK;
                        }
                    }
                    else if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_READ_BACK) {
                        if(stHead.stHead.bit2Pkg == XCAN_CODE_SIG || stHead.stHead.bit2Pkg == XCAN_CODE_END) {
                            pstPort->stTx[chn].bySta = XCAN_TXSTA_END;
                        }
                    }
                    else if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_READ) {
                        pstPort->stTx[chn].bySta = XCAN_TXSTA_WAIT_READ;
                    }
                    else if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_SYNC_SET_ACK) {
                        pstPort->stTx[chn].bySta = XCAN_TXSTA_END;
                    }
                    else if(pstPort->stTx[chn].byFrameType == XCAN_FRAME_SYNC_WRITE_ACK) {
                        pstPort->stTx[chn].bySta = XCAN_TXSTA_END;
                    }
                    else {
                        pstPort->stTx[chn].bySta = XCAN_TXSTA_END;
                    }

                    if(blTrsOne) return false;
                    if(pstPort->stTx[chn].bySta & 0xf0) break;
                }
            }
        }
    }

    return false;
}

/*******************************************************************************
函数功能：
*   @ 4通道数据发送状态更新
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static bool CoreTxHasNewTrs(unsigned int dwCommID)
{
    DevPortTdf *pstPort = s_stPort;

    for( int i = 0;i < SURPORT_MAX_PORT; i++, pstPort ++) {
        if (pstPort->blUsing == false) continue;
        if (pstPort->blActive == false) continue;
        if (pstPort->pstRemoteAlian == NULL) continue;
        if (pstPort->pstCommOps == NULL) continue;
        if (pstPort->pstCommOps->stChn.dwComChnID != dwCommID) continue;
        if (pstPort->pstCommOps->stChn.pfIsSendFull(pstPort->dwCommChnID, pstPort->dwDInID) == true) continue;

        for(int chn = 0; chn < XCAN_PORT_TRS_CHN_NUM; chn ++) {
            //启动新的传输
            if(pstPort->stTx[XCAN_PORT_TRS_CHN_NUM - 1 - chn].bySta == XCAN_TXSTA_SEND) return true;
            if(pstPort->stTx[XCAN_PORT_TRS_CHN_NUM - 1 - chn].bySta == XCAN_TXSTA_WAIT) {
                pstPort->stTx[XCAN_PORT_TRS_CHN_NUM - 1 - chn].bySta = XCAN_TXSTA_SEND;
                return true;
            }
        }
    }

    return false;
}

/*******************************************************************************
函数功能：
*   @ 发送触发，供设备触发调用
参数描述：
*   @ bool blTrsOne ： 只发送一个数据
*	@ unsigned int dwDevID 设备ID
返回值描述：
*   @ 无
说明：
*   @ 主要用于can的中断发送
*******************************************************************************/
static void CoreTxProcess( unsigned int dwCommID )
{
    stCommDevChnTdf *pstDev = GetCommChn(dwCommID);

    XCAN_ENTER_CRITICAL();
    if (pstDev->blTxIrpDoing == false || pstDev->blDoTrs == true) {
        XCAN_EXIT_CRITICAL();
        return;
    }
    XCAN_EXIT_CRITICAL();

    while ( true ) {
        if (CoreTxHasNewTrs(dwCommID) == false) break;						// 检查是否有需要发送的数据，没有的话退出
        if (CoreTxHasFifo(true, dwCommID) == false) break;					// 发送以及没有发送空间，无空间退出发送
    }

    if ( CoreTxHasNewTrs(dwCommID) == false) {								// 检查是否还有空闲的通道，如果有说明，数据传输完毕
        if (pstDev->stChn.pfSendEmptyNoticeDown != NULL) {
            pstDev->stChn.pfSendEmptyNoticeDown(dwCommID);
            pstDev->blTxIrpDoing = false;
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 发送触发
参数描述：
*   @ bool blTrsOne ： 只发送一个数据
*	@ unsigned int dwDevID 设备ID
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int CoreTxTrigger( DevPortTdf * pstPort)
{
    stCommDevChnTdf *pstDev = pstPort->pstCommOps;
    bool blTrsOne = true;

    if (pstDev == NULL) return -1;

    XCAN_ENTER_CRITICAL();
    if(pstDev->blTxIrpDoing == true || pstDev->blDoTrs == true) {
        XCAN_EXIT_CRITICAL();
        return 0;
    }

    pstDev->blDoTrs = true;
    XCAN_EXIT_CRITICAL();

    if(pstDev->stChn.pfSendEmptyNotice != NULL) blTrsOne = true;
    else blTrsOne = false;

    while (1) {
        if (CoreTxHasNewTrs(pstDev->stChn.dwComChnID) == false) break;						// 检查是否有需要发送的数据，没有的话退出
        if (CoreTxHasFifo(blTrsOne, pstDev->stChn.dwComChnID) == false) break;			    // 发送以及没有发送空间，无空间退出发送
    }

    XCAN_ENTER_CRITICAL();
    pstDev->blDoTrs = false;
    XCAN_EXIT_CRITICAL();

    if(pstDev->stChn.pfSendEmptyNoticeUp != NULL && CoreTxHasNewTrs(pstDev->stChn.dwComChnID) == true) {
        pstDev->blTxIrpDoing = true;
        pstDev->stChn.pfSendEmptyNoticeUp(pstDev->stChn.dwComChnID);
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 向设备的发送功能区增加发送数据，根据帧类型的不同使用不同的传输通道
参数描述：
*   @ DevPortTdf * pstPort ： 注册的设备信息
*   @ unsigned char byFrameType ： 添加发送帧的类型
*   @ unsigned char byIoAdr ：   需要操作的IO资源起始地址
*   @ unsigned char *pbyDatIn ： 需要写入的数据地址
*   @ unsigned int dwLen ： 数据长度
返回值描述：
*   @ -1：设备忙
*   @  0：发送成功
说明：
*   @ 无
*******************************************************************************/
int AddTrs ( DevPortTdf * pstPort, unsigned char byFrameType, short unsigned int wIoAdr, unsigned char *pbyDatIn, unsigned int dwLen)
{
    stCommChnTdf *pstCommDev = NULL;
    stTxCellTdf *pstTrs;
    if( !pstPort ) return -1;

    pstCommDev = &pstPort->pstCommOps->stChn;
    //使用不同的传输通道
    pstTrs = &pstPort->stTx[byFrameType];

    // if(byFrameType == XCAN_FRAME_SYNC_WRITE) {
        if(pstTrs->bySta != XCAN_TXSTA_IDLE  && pstTrs->bySta != XCAN_TXSTA_END) return -1;
    // }

    pstTrs->bySta = XCAN_TXSTA_IDLE;
    pstTrs->pbyTx = pbyDatIn;
    pstTrs->byFrameType = byFrameType;

    if(dwLen % pstCommDev->dwFrameDataLen) pstTrs->wPkgNum = 1;
    else pstTrs->wPkgNum = 0;

    pstTrs->wPkgNum += (dwLen / pstCommDev->dwFrameDataLen);
    pstTrs->wPkgCnt = 0;
    pstTrs->wIoStart = wIoAdr;

    pstTrs->dwLastPkgNum = (dwLen % pstCommDev->dwFrameDataLen);
    if(!pstTrs->dwLastPkgNum) pstTrs->dwLastPkgNum = pstCommDev->dwFrameDataLen;
    pstTrs->dwReTrsTim = 0;

    //启动
    pstTrs->bySta = XCAN_TXSTA_WAIT;
    CoreTxTrigger( pstPort );
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 触发通道重发或者报错
参数描述：
*   @ DevPortTdf * pstPort ： 注册的设备信息
*   @ unsigned char byFrameType ： 添加发送帧的类型
返回值描述：
*   @ -1：出错
*   @  0：检查成功
说明：
*   @ 无
*******************************************************************************/
int CheakResetTrs ( DevPortTdf * pstPort, unsigned char byFrameType)
{
    stCommChnTdf *pstCommDev = &pstPort->pstCommOps->stChn;
    unsigned int dwTim;
    stTxCellTdf *pstTrs;
    if( !pstPort ) return -1;
    if( !pfGetSysTim ) return -1;

    //使用不同的传输通道
    pstTrs = &pstPort->stTx[byFrameType];
    if(pstTrs->bySta == XCAN_TXSTA_IDLE || pstTrs->bySta == XCAN_TXSTA_END) return 0;

    dwTim = pfGetSysTim();
    if(pstTrs->dwChnTim > dwTim) return 0;
    pstTrs->dwChnTim = dwTim + mXCAN_RE_TRS_TIM;

    if(pstTrs->dwReTrsTim ++ < XCAN_FRAME_RE_TRS_TIMES) {
        // 复位,重传
        pstTrs->bySta = XCAN_TXSTA_WAIT;
        pstTrs->wPkgCnt = 0;
        CoreTxTrigger( pstPort );
    }
    else {
        pstTrs->bySta = XCAN_TXSTA_IDLE;
        if(byFrameType == XCAN_FRAME_SYNC_SET || byFrameType == XCAN_FRAME_SYNC_WRITE) {
            pstPort->stSyncTrs.stSynReq.blSyncLink = false;
            pstPort->stSyncTrs.stSynLoacl.blWorking = false;
            pstPort->stSyncTrs.stSynLoacl.emXcanSyncTrsSta = XcanTrs_Idle;
            pstPort->pstRemoteAlian->dwLinkIoOnBit = 0;

            if (pstPort->Err != NULL) {
                pstPort->Err();
            }

            if (pstCommDev->Err != NULL) {
                pstCommDev->Err(pstPort->dwCommChnID, pstPort->dwDInID, pstPort->wRemoteDevID, pstPort->wIoUUID, byFrameType);
            }
        }
    }

    return 0;
}

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
void XcanOutInit(stCommChnTdf *pstCommDev)
{
    if(pstCommDev->pfSendEmptyNotice) pstCommDev->pfSendEmptyNotice(pstCommDev->dwComChnID, CoreTxProcess);
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */
