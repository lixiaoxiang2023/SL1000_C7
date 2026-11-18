/*******************************************************************************
文件名称：  usart.h
文件目录：  $PROJ_DIR$/usr/Drive/inc
项目名称：  事业一部技术平台
创建者：    胥强
创建日期：  2016.03.08
程序用途：  串口驱动文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __DMA_H
#define __DMA_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define DMA_DEV_1                               (0)
#define DMA_DEV_2                               (1)
#define DMA_DEV_NUM                             (2)

#define DMA_Normal_Mode                         (0)
#define DMA_Circular_Mode                       (1)

#define DMA_CHN_0                               (0)
#define DMA_CHN_1                               (1)
#define DMA_CHN_2                               (2)
#define DMA_CHN_3                               (3)
#define DMA_CHN_4                               (4)
#define DMA_CHN_5                               (5)
#define DMA_CHN_6                               (6)
#define DMA_CHN_7                               (7)
#define DMA_CHN_NUM                             (8)

#define DMA_STREAM_0                            (0)
#define DMA_STREAM_1                            (1)
#define DMA_STREAM_2                            (2)
#define DMA_STREAM_3                            (3)
#define DMA_STREAM_4                            (4)
#define DMA_STREAM_5                            (5)
#define DMA_STREAM_6                            (6)
#define DMA_STREAM_7                            (7)
#define DMA_STREAM_NUM                          (8)

#define DMA_SIZE_BYTE                           (0)
#define DMA_SIZE_HALFWORD                       (1)
#define DMA_SIZE_WORD                           (2)

#define DMA_PSIZE_BYTE                          (DMA_SIZE_BYTE)
#define DMA_PSIZE_HALFWORD                      (DMA_SIZE_HALFWORD)
#define DMA_PSIZE_WORD                          (DMA_SIZE_WORD)

#define DMA_MSIZE_BYTE                          (DMA_SIZE_BYTE)
#define DMA_MSIZE_HALFWORD                      (DMA_SIZE_HALFWORD)
#define DMA_MSIZE_WORD                          (DMA_SIZE_WORD)

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    int  (*ConfigM2P) ( unsigned int dwDev, unsigned int dwStream, unsigned int dwChn, unsigned int dwMode, unsigned int dwMDatSize, unsigned int dwPDatSize);
    int  (*ConfigP2M) ( unsigned int dwDev, unsigned int dwStream, unsigned int dwChn, unsigned int dwMode, unsigned int dwMDatSize, unsigned int dwPDatSize);
    int  (*Trs) (unsigned int dwDev, unsigned int dwChn, unsigned int dwPadd, unsigned int dwMadd, unsigned int dwLen);
    bool (*IsBusy) (unsigned int dwDev, unsigned int dwChn);
    void (*SetTrsHandler) (unsigned int dwDev, unsigned int dwStream, void (*Handler)(unsigned int dwIn), unsigned dwIn);
}DevDmaTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevDmaTdf *DevDma;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevDmaTdf * DmaInit (void);

#endif