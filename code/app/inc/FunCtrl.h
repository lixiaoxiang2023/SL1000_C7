/******************************************************************************
文件名称：FlashDist.h
文件目录：$PROJ_DIR$\code\user\inc
项目名称：
创建者：
创建日期：
程序用途：
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/
#ifndef __FUNCTRL_H
#define __FUNCTRL_H
/******************************************************************************
Include files头文件
******************************************************************************/

#include "stdint.h"
/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
#define OLD_BOARD        0

typedef struct {
 void (*Init)(void);
 void (*SetFunPWM)(uint8_t Steps);
 uint16_t (*GetFanTemp)(void);
 uint32_t (*GetSpeed)(void);
 void (*OpendyeFan)(uint8_t byMode);
}FunCtrlTdf;

extern const FunCtrlTdf FunCtrl;

#endif