/*******************************************************************************
文件名称：   bt.h
文件目录：   $PROJ_DIR$/usr/Drive/inc
项目名称：   事业一部硬件驱动技术平台
创建者：     xuqiang
创建日期：   20151130
程序用途：   bluetooth 模块 驱动程序头文件

版权声明：
            Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
2017.11.8 胥强整理加入硬件驱动平台
*******************************************************************************/
#ifndef __BT_H
#define __BT_H

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
    unsigned char byUartNO;
    unsigned int  dwBaud;
    unsigned int *byPinRestAddr;
}DevBtInitTdf;

typedef struct {
    int (*Write) ( unsigned char *pbyIn, unsigned int dwLen);
#if(CONFIG_BT_UNLINK)
    void (*Unlink) (void);
#endif
#if(CONFIG_BT_CHEAK_BAUD)
    unsigned int (*CheakBaud) (void);
#endif
#if(CONFIG_BT_SET_NAME)
    int (*SetName) (const char *pcName);
#endif
#if(CONFIG_BT_SET_ROLE)
    int (*SetRole) (bool blIsMaster);
#endif
#if(CONFIG_BT_SET_PIN)
    int (*SetPin) (const unsigned char *pbyPin);
#endif
#if(CONFIG_BT_CLEAR)
    int (*Clear) (void);
#endif
#if(CONFIG_BT_SEARCH)
    int (*Search) (void);
#endif
#if(CONFIG_BT_GET_MAC)
    int (*GetMac) (char *pbyMac);
#endif
#if(CONFIG_BT_SET_BAUD)
    int (*SetBaud) (unsigned int dwBaud);
#endif
#if(CONFIG_BT_NITOCE)
    int (*SetNotice) (bool blNoticeOn);
#endif
#if(CONFIG_BT_SET_DEFAULT)
    int (*SetDefault) (void);
#endif
#if(CONFIG_BT_SET_NITOCE_HANLDER)
    void (*SetNoticehandler) (void (*Hanlder) (unsigned char));
#endif
#if(CONFIG_BT_IS_CONNECTED)
    bool (*IsConnected) (void);
#endif
#if(CONFIG_BT_IS_DEV_EXIST)
    bool (*IsDevExist) (void);
#endif
#if(CONFIG_BT_DEBUG)
    void (*Debug) (bool blOn, unsigned int dwDebugUart);
#endif
}DevBtTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevBtTdf *DevBt;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevBtTdf *BtInit();
#endif