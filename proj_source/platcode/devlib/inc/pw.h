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
#ifndef __PW_H
#define __PW_H

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define IWDG_PR_4                       0
#define IWDG_PR_8                       1
#define IWDG_PR_16                      2
#define IWDG_PR_32                      3
#define IWDG_PR_64                      4
#define IWDG_PR_128                     5
#define IWDG_PR_256                     6

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
#if(CONFIG_PW_DEINIT)
    void (*Deinit) (void);
#endif
#if(CONFIG_PW_PVD_EVENT_UP)
    int (*PvdEventUp) (unsigned int dwLevel, void (*pvHandler) (unsigned int));
#endif
#if(CONFIG_PW_PVD_EVENT_DOWN)
    int (*PvdEventDown) (void);
#endif
#if(CONFIG_PW_WAKEUP_PIN_ON)
    void (*WakeUpPinOn) (bool blIsOn);
#endif
#if(CONFIG_PW_SLEEP_NOW_WFI)
    void (*SleepNowWFI) (void);
#endif
#if(CONFIG_PW_SLEEP_NOW_WFE)
    void (*SleepNowWFE) (void);
#endif
#if(CONFIG_PW_SLEEP_ON_EXIT)
    void (*SleepOnExit) (void);
#endif
#if(CONFIG_PW_STOP_MODE)
    void (*StopMode) (void);
#endif
#if(CONFIG_PW_MODE_LOW_POWER)
    void (*StopModeLowPower) (void);
#endif
#if(CONFIG_PW_STANDBY_MODE)
    void (*StandbyMode) (void);
#endif
#if(CONFIG_PW_IS_FROM_WAKEUP)
    bool (*IsFromWakeUp) (void);
#endif
#if(CONFIG_PW_IWDG_CONFIG)
    void (*IwdgConfig) (unsigned char byIwdgPR, unsigned short int wIwdgRLR);
#endif
#if(CONFIG_PW_IWDG_UP)
    void (*IwdgUp) (void);
#endif
#if(CONFIG_PW_IWDG_DOWN)
    void (*IwdgDown) (void);
#endif
#if(CONFIG_PW_IWDG_FEED)
    void (*IwdgFeed) (void);
#endif

}DevPwTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevPwTdf* DevPw;
/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevPwTdf *PwInit(void);

#endif