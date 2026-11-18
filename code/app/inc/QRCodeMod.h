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
#ifndef __QRCODEMOD_H
#define __QRCODEMOD_H
/******************************************************************************
Include files头文件
******************************************************************************/

#include "stdint.h"

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
typedef struct {
 void (*Init)(void);
 void (*PowerMode)(uint8_t Mode);
 void (*TriggerMode)(uint8_t Mode);
 void (*GetQrCode)(uint8_t *pData);
 void (*PowerOnPromptSet)(uint8_t Mode);
 void (*SilenceSet)(uint8_t Mode);
 void (*RestoreDefaultConfiguration)(void);
 void (*SensitivityLevel)(uint8_t Mode);
 void (*StableInductionTime)(uint8_t Mode);

}QRCodeModTdf;

extern const QRCodeModTdf QRCodeMod;



#endif