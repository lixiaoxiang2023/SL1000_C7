/*******************************************************************************
文件名称：    rtc.h
文件目录：    $PROJ_DIR$/usr/Drive/inc
项目名称：    事业一部技术平台
创建者：      胥强
创建日期：    2016.03.12
程序用途：    基于Stm32的Rtc驱动程序头文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __RTC_H
#define __RTC_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <stdbool.h>

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define RTC_BKP_DR0                           0
#define RTC_BKP_DR1                           1
#define RTC_BKP_DR2                           2
#define RTC_BKP_DR3                           3
#define RTC_BKP_DR4                           4
#define RTC_BKP_DR5                           5
#define RTC_BKP_DR6                           6
#define RTC_BKP_DR7                           7
#define RTC_BKP_DR8                           8
#define RTC_BKP_DR9                           9
#define RTC_BKP_DR10                          10
#define RTC_BKP_DR11                          11
#define RTC_BKP_DR12                          12
#define RTC_BKP_DR13                          13
#define RTC_BKP_DR14                          14
#define RTC_BKP_DR15                          15
#define RTC_BKP_DR16                          16
#define RTC_BKP_DR17                          17
#define RTC_BKP_DR18                          18
#define RTC_BKP_DR19                          19

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef enum {
    HHMMSS = 0,
    HHMM = 1,
    TTHHMM = 2
}emTimTypeTdf;

typedef enum {
    YYMMDD = 0,
    DDMMYY = 1
}emDateTypeTdf;

typedef struct {
    unsigned short int wYear;
    unsigned char byMonth;
    unsigned char byDay;
    unsigned char byWeek;

    unsigned char byHour;
    unsigned char byMinute;
    unsigned char bySecond;
}stRtcTimTdf;

typedef struct {
    bool (*IsLeapYear) (unsigned int dwYear);
    bool (*IsTime) ( stRtcTimTdf *pstTim );
    int (*SetTime) (stRtcTimTdf *pstTim);
    int (*GetTim) (stRtcTimTdf *pstTim);
    unsigned int (*GetTimCnt) (void);
    int (*GetTimStr) (emTimTypeTdf enType, stRtcTimTdf *pstTim, char *pcOut);
    int (*GetDateStr) (emDateTypeTdf enType, stRtcTimTdf *pstTim, char *pcOut);
    void (*SecItUp) ( void );
    void (*SecItDown) ( void );
    int (*SetSecHandler) (void (* pHandler) (void));
    void (*Config) (void);
    void (*WrBkpReg) (unsigned int dwBkpDr, unsigned int wDat);
    unsigned int (*RdBkpReg) (unsigned int dwBkpDr);
}DevRtcTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevRtcTdf *DevRtc;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevRtcTdf *RtcInit(void);

#endif