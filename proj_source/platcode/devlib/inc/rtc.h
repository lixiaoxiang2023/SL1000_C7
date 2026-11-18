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
#define RTC_BKP_DR1                           ((unsigned short int)0x0004)
#define RTC_BKP_DR2                           ((unsigned short int)0x0008)
#define RTC_BKP_DR3                           ((unsigned short int)0x000C)
#define RTC_BKP_DR4                           ((unsigned short int)0x0010)
#define RTC_BKP_DR5                           ((unsigned short int)0x0014)
#define RTC_BKP_DR6                           ((unsigned short int)0x0018)
#define RTC_BKP_DR7                           ((unsigned short int)0x001C)
#define RTC_BKP_DR8                           ((unsigned short int)0x0020)
#define RTC_BKP_DR9                           ((unsigned short int)0x0024)
#define RTC_BKP_DR10                          ((unsigned short int)0x0028)
#define RTC_BKP_DR11                          ((unsigned short int)0x0040)
#define RTC_BKP_DR12                          ((unsigned short int)0x0044)
#define RTC_BKP_DR13                          ((unsigned short int)0x0048)
#define RTC_BKP_DR14                          ((unsigned short int)0x004C)
#define RTC_BKP_DR15                          ((unsigned short int)0x0050)
#define RTC_BKP_DR16                          ((unsigned short int)0x0054)
#define RTC_BKP_DR17                          ((unsigned short int)0x0058)
#define RTC_BKP_DR18                          ((unsigned short int)0x005C)
#define RTC_BKP_DR19                          ((unsigned short int)0x0060)
#define RTC_BKP_DR20                          ((unsigned short int)0x0064)
#define RTC_BKP_DR21                          ((unsigned short int)0x0068)
#define RTC_BKP_DR22                          ((unsigned short int)0x006C)
#define RTC_BKP_DR23                          ((unsigned short int)0x0070)
#define RTC_BKP_DR24                          ((unsigned short int)0x0074)
#define RTC_BKP_DR25                          ((unsigned short int)0x0078)
#define RTC_BKP_DR26                          ((unsigned short int)0x007C)
#define RTC_BKP_DR27                          ((unsigned short int)0x0080)
#define RTC_BKP_DR28                          ((unsigned short int)0x0084)
#define RTC_BKP_DR29                          ((unsigned short int)0x0088)
#define RTC_BKP_DR30                          ((unsigned short int)0x008C)
#define RTC_BKP_DR31                          ((unsigned short int)0x0090)
#define RTC_BKP_DR32                          ((unsigned short int)0x0094)
#define RTC_BKP_DR33                          ((unsigned short int)0x0098)
#define RTC_BKP_DR34                          ((unsigned short int)0x009C)
#define RTC_BKP_DR35                          ((unsigned short int)0x00A0)
#define RTC_BKP_DR36                          ((unsigned short int)0x00A4)
#define RTC_BKP_DR37                          ((unsigned short int)0x00A8)
#define RTC_BKP_DR38                          ((unsigned short int)0x00AC)
#define RTC_BKP_DR39                          ((unsigned short int)0x00B0)
#define RTC_BKP_DR40                          ((unsigned short int)0x00B4)
#define RTC_BKP_DR41                          ((unsigned short int)0x00B8)
#define RTC_BKP_DR42                          ((unsigned short int)0x00BC)

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
    void (*WrBkpReg) (unsigned int dwBkpDr, unsigned short int wDat);
    unsigned short int (*RdBkpReg) (unsigned int dwBkpDr);
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