/*******************************************************************************
文件名称：   enc28.h
文件目录：   $PROJ_DIR$/usr/Drive
项目名称：   事业一部技术平台
创建者：    黄鹏
创建日期：   20121008
程序用途：   以太网控制芯片驱动
版权声明：   Copyright urit Co.,Ltd.All Right Reserved

第一次修改：
修改人：胥强
修改内容：加入技术平台
日期：20160805
*******************************************************************************/
#ifndef __ENC28J60_H
#define __ENC28J60_H

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
    unsigned char byItGpioNO;
    unsigned char byItChn;

    unsigned char bySpiNo;
    unsigned char bySpiDmaTxNo;
    unsigned char bySpiDmaTxChn;
    unsigned char bySpiDmaTxStream;

    unsigned char bySpiDmaRxNo;
    unsigned char bySpiDmaRxChn;
    unsigned char bySpiDmaRxStream;

    unsigned int  dwSpiBaud;

    unsigned int *pdwNcs;
    unsigned int *pdwNrst;
    unsigned char *pbyMac;
}Enc28InitTdf;

typedef struct {
    int (*Read) (unsigned char * pbyOut);
    int (*Write) (unsigned char *pbyIn, unsigned int dwLen);
    int (*SetUp) (void);
    int (*SetDown) (void);
    unsigned int (*CheckETHLink) (void);
    unsigned char (*GetRev) (void);
    void (*OutClk) (unsigned char byClk);
    unsigned short int (*ReadEIR) (void);
    void (*RegRxHandler) (void (*Handler)(void));
    bool (*EnterUse) (void);
    int (*ExitUse) (void);
    int (*Config) (unsigned char *pbyMac);
}DevEnc28Tdf;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevEnc28Tdf *Enc28j60Init(Enc28InitTdf* pstInit);
#endif