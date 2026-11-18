/*******************************************************************************
文件名称：   buttons.h
文件目录：   $PROJ_DIR$/../code/dev/plat/inc
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2017.11.3
程序用途：   用于通用的按键驱动程序。

版权声明：   Copyright urit Co.,Ltd.All Right Reserved

*******************************************************************************/
#ifndef __BUTTONS_H
#define __BUTTONS_H

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    unsigned char byTimNO;        // 使用的定时器模块编号

    unsigned int dwTimPre;        // 用于定义定时器的分频
    unsigned int dwTimPeriod;     // 用于定义定时器的中断周期
} DevButtonInitTdf;

typedef struct {
    unsigned char byGpio;
    unsigned char byChn;

    unsigned int  dwTimPeriod;
    unsigned int  *pwdAddrGpio;
    unsigned int  dwPressedLev;

    unsigned int  dwPressTim;
    unsigned int  dwLongPressTim;
} DevButtonChnInitTdf;

typedef struct {
#if(CONFIG_BUTTONS_IS_PRESSED)
     bool (*IsPressed) (unsigned int dwDev);
#endif

#if(CONFIG_BUTTONS_PRESSED)
    int (*SetPressedHandler) (unsigned int dwDev, void (*Handler) (unsigned int));
    int (*SetPressedTimDelay) (unsigned int dwDev, unsigned int dwTimDelay);
#endif

#if(CONFIG_BUTTONS_LONG_PRESSED)
    int (*SetLongPressedHandler) (unsigned int dwDev, void (*Handler) (unsigned int));
    int (*SetLongPressedTimDelay) (unsigned int dwDev, unsigned int dwTimDelay);
#endif

    int (*AddChn) (unsigned int dwDev, DevButtonChnInitTdf *pstChn);
} DevButtonTdf;

/*******************************************************************************
宏定义函数申明
******************************************************************************/

/*******************************************************************************
外部全局函数声明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevButtonTdf *ButtonInit(DevButtonInitTdf *pstInit);
#endif