/*******************************************************************************
 文件名称：   bxcan.h
 目录：       $PROJ_DIR$/../code/dev/src
 项目名称：   事业一部技术平台
 创建者：     胥强
 创建日期：   2016.03.15
 程序用途：   Can总线的驱动设备定义

 版权声明：
 Copyright urit Co.,Ltd. All Right Reserved

 修改及维护信息
 20130721：
 第一次由胥强创建初始程序。
2016.03.15：
胥强：整理协议，并将架构平台化
 ******************************************************************************/
#ifndef __BXCAN_H__
#define __BXCAN_H__

/*******************************************************************************
 Include file
 ******************************************************************************/

/*******************************************************************************
 全局变量定义
 ******************************************************************************/

/*******************************************************************************
 数据结构与数据类型定义
 ******************************************************************************/
typedef struct {
    int (*AddFilter)    ( unsigned int dwDev, unsigned short int wFilterID, unsigned int dwFilterLen);
    int (*SendOne)      ( unsigned int dwDev, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen);
    int (*GetEmpNum)    ( unsigned int dwDev );
    int (*SendIrpOn)    ( unsigned int dwDev );
    int (*SendIrpOff)   ( unsigned int dwDev );
    int (*SetRxHandler) ( unsigned int dwDev, void (*pfHandler) ( unsigned int dwDevID, unsigned int dwId, unsigned char *pbyDat, unsigned int dwLen));
    int (*SetTxHandler) ( unsigned int dwDev, void (*pfHandler) (unsigned int dwDev));
    int (*SetHwID)      ( unsigned int dwDev, unsigned char byID);
}DevCanTdf;

/*******************************************************************************
 内部全局变量申明
 ******************************************************************************/
extern const DevCanTdf *DevCan;

/*******************************************************************************
 函数申明
 ******************************************************************************/
const DevCanTdf *CanInit(void);
const DevCanTdf *CanInit2(unsigned int dwBoud);

#endif
