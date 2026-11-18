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
#ifndef __MOTORCTRL_H
#define __MOTORCTRL_H
/******************************************************************************
Include files头文件
******************************************************************************/

#include "stdint.h"

/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
//typedef  enum
//{
//    C7_NoErr            = 0,//：无错误
//    C7_M1MotorErr       = 0x01,//：M1轴电机复位故障。
//    C7_M2MotorErr       = 0x02,//：M2轴电机复位故障。
//    C7_M3MotorErr       = 0x03,//：M3轴电机复位故障。
//    C7_M4MotorErr       = 0x04,//：M4轴电机复位故障。
//    C7_TempFanErr       = 0x05,//風扇加热故障
//    C7_NoOilErr         = 0x06,//：无镜油
//    C7_AddOilErr        = 0x07,//：添加镜油故障
//    C7_TempErr          = 0x08,//：加热故障
//    C7_SlidBoxErr       = 0x09,//：试纸盒取出
//    C7_M5MotorErr       = 0x0a,//：M5滴镜油电机复位故障。
//    C7_M1Opc2Err        = 0x0b,//：M1电机光耦2复位故障。
//    C7_TempLowErr       = 0x0c,//制片温度过低
//    C7_TempHightErr     = 0x0d,//制片温度过高
//    C7_DyeOpcErr        = 0x0e,//染剂位光藕故障或无载玻片
//    C7_OilOpcErr        = 0x0f,//镜油位光藕故障或无载玻片
//    C7_CardOpcErr       = 0x10,//载玻片已用完,
//    C7_SlideCarErr      = 0x11,// 拨动载玻片卡
////    C7_Task0Busy        = 0xc70a,//：任务0忙
////    C7_Task1Busy        = 0xc70b,//：任务1忙
////    C7_Task2Busy        = 0xc70c,//：任务2忙
////    C7_Task3Busy        = 0xc70d,//：任务3忙
////    C7_Task4Busy        = 0xc70e,//：任务4忙
//}emC7ErrTdf;

typedef struct {
 void (*Init)(void);
 int (*M1_To_Drop)(uint32_t Steps,uint8_t byDebug);
 int (*M1_Opc_To_Drop)(uint32_t Steps);
 int (*M1_To_Opc)(uint32_t Steps,uint8_t byModle);
 int (*M1_Rest_To_Drop)(uint32_t Steps,uint8_t byDebug);
 //void (*M1_To_Heating)(uint32_t Steps);
 int (*M1_Rest)(uint32_t Steps);
 int (*M1_DebugRest)(void);
 int (*M2_Rest)(void);
 int (*M2_MoveOneStep)(uint32_t Steps);
 int (*M3_Up)(uint32_t Steps);
 int (*M3_Down)(uint32_t Steps);
 int (*M3_DownToWash)(uint32_t Steps);
 int (*M3_UpTOWash)(uint32_t Steps);
 int (*M3_Rest)(void);
 int (*ResetAll)(uint32_t Steps);
 void (*GetOp)(uint16_t *DataOut);
 int (*M4_Rest)(uint32_t Steps);
 int (*M4_Motion)(void);
 int (*M5_Rest)(void);
 int (*M5_DropStart)(unsigned int CountIn);
 int (*M5_FullOil)(void);
 int (*C7SetErr)(uint16_t byErr);
 uint8_t (*Get_ErrLevel)(uint16_t byErr);
 void (*MotorMove)(uint8_t dwHwnd,uint8_t byCtr, uint32_t Steps);
 int (*C7TransferToC6)(void);
 int (*M3_DownToFan)(uint32_t Steps);
 int (*C7Ageing)(void);
 int (*M2_MoveStep)(uint32_t Steps); 
}MotorCtrlTdf;

extern const MotorCtrlTdf MotorCtrl;



#endif
