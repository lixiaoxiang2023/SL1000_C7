/*******************************************************************************
文件名称： spi.c
目录：     $PROJ_DIR$/../code/dev
项目名称： 事业一部技术平台
创建者：   胥强
创建日期： 20160308
程序用途：

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20160308：第一次由XQ创建初始程序。
*******************************************************************************/
#ifndef __SPI_H
#define __SPI_H

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
    int (*Config) (unsigned int dwDev, unsigned int dwBaud);
    int (*ConfigPOL_PHA)(unsigned int dwDev, unsigned char byPOL, unsigned char byPHA);

#if(CONFIG_SPI_DMA)
    #if(PLAT_STM32_F2|PLAT_STM32_F4)
    int (*ConfigDmaTx) ( unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn, unsigned int dwStream);
    int (*ConfigDmaRx) ( unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn, unsigned int dwStream);
    #else
    int (*ConfigDmaTx) ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn);
    int (*ConfigDmaRx) ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn);
    #endif
#endif

    int (*Trs) (unsigned int dwDev,const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs);
    int (*TrsHold) (unsigned int dwDev,const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs);
#if(CONFIG_SPI_DMA)
    int (*TrsDma) (unsigned int dwDev,const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs);
#endif

#if(CONFIG_SPI_DMA_RETURN)
    int (*TrsDmaRet) (unsigned int dwDev,const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs);
#endif
}DevSpiTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevSpiTdf *DevSpi;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevSpiTdf* SpiInit( void );
#endif