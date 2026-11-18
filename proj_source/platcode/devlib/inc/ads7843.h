/*******************************************************************************
文件名称：   ads7843.h
文件目录：   $PROJ_DIR$/../dev/inc
项目名称：   事业一部技术平台
创建者：     马德嘉
创建日期：   2016.06.27
程序用途：   ads7843驱动文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __ADS7843_H
#define __ADS7843_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef enum
{
    KEY_DM,
    KEY_PRESS,
    KEY_LONGPRESS,
    KEY_UP,
    KEY_IDLE
}KeyStateTdf;

typedef struct
{
    int x[5], xfb[5]; //xfb 使用坐标，x为采样值
    int y[5], yfb[5];
    float a[7];
}stCalibTdf;

typedef struct
{
    KeyStateTdf           emKeyState;

    unsigned short int wKeyX;
    unsigned short int wKeyY;

    unsigned short int wAdcKeyX;
    unsigned short int wAdcKeyY;
}KeyMsgTdf;

typedef struct {
    unsigned char byPenGpioNO;
    unsigned char byPenChn;

    unsigned char byTimNO;

    unsigned char bySpiNo;      //0-spi1
    unsigned char bySpiTxDmaNO;
    unsigned char bySpiTxDmaChn;
    unsigned char bySpiTxDmaStream;
    unsigned int  dwSpiBaud;

    unsigned char bySpiRxDmaNO;
    unsigned char bySpiRxDmaChn;
    unsigned char bySpiRxDmaStream;

    unsigned int  *pwdSpiNcs;   //cs管脚
    unsigned int  *pwdPenIn;    //Pen输入状态位
}Ads7843InitTdf;

typedef struct {
    void (*RegTsHandler) (void (*TsHandler) (KeyMsgTdf *pstMsg));
    int  (*Calibrate) (stCalibTdf *pstCalib);
    int  (*SetCalibrate) (stCalibTdf *pstCalib);
    bool (*IsTouch) (void);
    void (*TouchUp) (void);
    void (*TouchDown) (void);
}DevAds7843Tdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevAds7843Tdf *Ads7843Init(Ads7843InitTdf *pstInit);

#endif