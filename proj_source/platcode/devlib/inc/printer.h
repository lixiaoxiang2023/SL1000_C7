/******************************************************************************
文件名称：  printr.h
文件目录：  $PROJ_DIR$\..\code\dev\plat
项目名称：  事业一部技术平台
创建者：    陆作沛
创建日期：  2013-04-10
程序用途：  根据串口协议驱动685打印机模块
版权声明：  Copyright urit Co.,Ltd.All Right Reserved

日期： 2013-04-11 陆作沛
第一次修改：完成编程格式规范

日期：2016-05-12 胥强
加入技术平台
******************************************************************************/
#ifndef __PRINTER_H
#define __PRINTER_H

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
#define TX_BUF_SIZE        16

/******************************************************************************
数据结构与数据类型定义
******************************************************************************/
typedef struct
{
    unsigned char      byCmdCnt;       //指令计数器
    unsigned char      byCmd;          //命令
    unsigned char      byCmdReg;       //当前状态
    unsigned char      byHasPaper;     //是否有打印纸

    unsigned char      byPrtLev;       //打印浓度
    unsigned char      byIsAutoToSend; //自动发送无打印纸
    unsigned char      byaVer[4];      //版本号
    unsigned char      byPrintCnt;     //打印计数
}stPrinterIoTdf;

typedef struct
{
    unsigned char      byUartSn;
#if(CONFIG_PRINTER_COM_DMA)
    unsigned char      byUartDmaSn;
    unsigned char      byUartDmaChn;
#endif
    bool      blUseProtol;
}stPrinterInitTdf;

typedef struct
{
    void (*Read) (unsigned char bySAddr, unsigned char byLen);
    void (*Write) (unsigned char bySAddr, unsigned char *pbyIn, unsigned char byLen);
    void (*SetPrtLev) (unsigned char byPrtLev);
    bool (*HasPaper) (void);
    int (*FeedPaper) (void);
    int (*PrtTest) (void);
    int (*UpDate) (void);
    int (*Print) (unsigned char *pbyIn, unsigned int dwLen, bool blIsWait);
    int (*PrintCnt) (void);
    void (*Version) (unsigned char *pbyOut);
    bool (*IsConnect) (void);
    void (*UseProtol) (bool blCmd);
}DevPrinterTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/

/*******************************************************************************
外部全局函数声明
*******************************************************************************/
const DevPrinterTdf *PrinterInit(stPrinterInitTdf *pstInit);

#endif