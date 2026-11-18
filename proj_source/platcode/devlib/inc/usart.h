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
#ifndef __USART_H
#define __USART_H

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
    int (*Config) ( unsigned int dwDev, unsigned int dwBaud, int iStop, int iParity , bool blRx);
    int (*Config2) ( unsigned int dwDev, unsigned int dwBaud, int iStop, int iParity , int iDatLen, bool blRx);
#if(CONFIG_USART_DMA)
    int (*ConfigDma) ( unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn);
#endif
    int (*SendByte) (unsigned int dwDev, unsigned char byIn);
    int (*RxCmd) (unsigned int dwDev, bool blIsOn);
#if(CONFIG_USART_DMA)
    int (*SendStringDma) (unsigned int dwDev, const unsigned char *pbyIn, unsigned int dwLen);
#endif
#if(CONFIG_USART_IT)
    int (*SendStringIt) (unsigned int dwDev, const unsigned char *pbyIn, unsigned int dwLen);
#endif
    int (*SendString) (unsigned int dwDev, const unsigned char *pbyIn, unsigned int dwLen);
    int (*RegHandler) (unsigned int dwDev, void (*pvHandler)(unsigned char));
}DevUartTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevUartTdf *DevUart;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevUartTdf * UsartInit ( void );

#endif