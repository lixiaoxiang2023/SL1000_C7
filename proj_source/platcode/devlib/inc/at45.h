/*****************************************************************************
文件名称：   at45.h
文件目录：   $PROJ_DIR$/usr/Drive/inc
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20151127
程序用途：   AT45DB161驱动程序头文件
版权声明：   Copyright urit Co.,Ltd.All Right Reserved

2016.03.12：
整理成驱动平台，at45将调用通用spi接口
******************************************************************************/
#ifndef __AT45_H
#define __AT45_H

/******************************************************************************
Include files头文件
******************************************************************************/

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/

/******************************************************************************
数据结构与数据类型定义
******************************************************************************/
typedef struct {
    unsigned char bySpiNo;
    unsigned char bySpiDmaTxNo;
    unsigned char bySpiDmaTxChn;
    unsigned char bySpiDmaRxNo;
    unsigned char bySpiDmaRxChn;
    unsigned int  dwSpiBoud;
    unsigned int  *pdwNcs;
}At45InitTdf;

typedef struct {
    void (*ReadPage) (unsigned short int wSector, unsigned char *pbyOut, unsigned short int wLen);
    void (*WritePage) (unsigned short int wSector, const unsigned char *pbyIn, unsigned short int wLen);
#if(CONFIG_AT45_ALL_FUNC)
    void (*PageErase) (unsigned short int wPageAdr);
#endif
    unsigned short (*ReadID)(void);
}DevAt45Tdf;

/***************************************************************************
宏定义函数申明
******************************************************************************/
#define AT45_APGE_SIZE      0x200
#define AT45_FLASH_SIZE     0x80000

/*****************************************************************************
输出函数申明
******************************************************************************/
const DevAt45Tdf *At45Init( At45InitTdf *pstInit);

#endif
