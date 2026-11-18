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

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    int (*ConfigM2P) ( unsigned int dwDev, unsigned int dwChn, unsigned int dwMDatSize, unsigned int dwPDatSize);
    int (*ConfigP2M) ( unsigned int dwDev, unsigned int dwChn, unsigned int dwMDatSize, unsigned int dwPDatSize);
    int (*Trs) (unsigned int dwDev, unsigned int dwChn, unsigned int dwPadd, unsigned int dwMadd, unsigned int dwLen);
    bool (*IsBusy) (unsigned int dwDev, unsigned int dwChn);
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