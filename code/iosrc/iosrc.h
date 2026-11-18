//@file name :ican_slave_src.h
//@author : xq
//@data :20111014

#ifndef __IOSRC_H__
#define __IOSRC_H__

#include "iosrc_updata.h"
#include "clients/clients_def.h"

/*********************************start**************************************/
/***************************公共资源设备状态*********************************/
/****************************************************************************/
//#pragma pack(1)
//typedef struct
//{
//  struct {
//    uint8_t  byCmd;
//    uint8_t  byCmdCnt;
//    uint16_t  waCmd_Param[5];          // 数据参数交换部分
//  } stIn;
//  struct {
//      uint8_t  byCmdDown;               // 上次命令执行代码
//      uint8_t  byCmdCntDown;            // 命令执行计数器，有变化是有命令完成执行
//      uint8_t  byErr;                   // 错误代码
//      uint8_t  byErrCnt;                // 错误计数器，有变化是有错误发生
//
//      uint8_t  byCmdisBusy[6];
//      uint8_t  wDropOpc;               //光耦状态 ++
//	  uint8_t  wSlideCard;             // 染色位++
//	  uint16_t byTemp[2];
//      uint8_t  wDropOilCard;           //滴油位玻片 ++
//	  uint8_t  wCardBox;               //试纸盒光耦 1有，0 没有
//      uint16_t wMpuSwap[2];    //陀螺仪信息校准【0】X，【1】Y，当前【2】【3】
//      uint8_t byHandFlg;         //接收成功回馈标志
//      uint8_t byUpdataFinish;  // 固件升级完成标志
//      uint8_t byUpdataReady;
//      uint8_t byRet1;
//      uint8_t byRet2;
//      uint8_t  wSwapOutLen;
//      uint16_t waSwap[32];             // 数据交换部分
//      unsigned int  wHwVer;
//      unsigned int  wHwVerDate;
//      unsigned int  wSwVer;
//      unsigned int  wSwVerDate;
//  } stOut;
//   __StDownLoad stDownload;
//}_c7_src_dev;
//
//#pragma pack()

extern _c7_src_dev g_stC7;


#endif