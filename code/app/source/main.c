/*****************************************************************************************
文件名称：main.c
目录：$PROJ_DIR$/../code/app
项目名称：
创建者：
创建日期：
程序用途：


版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息

*****************************************************************************************/

/*****************************************************************************************
Include file
*****************************************************************************************/
#include "devinit.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sysdata.h"
#include "xcan.h"
#include "sensor.h"
#include "MotorCtrl.h"
#include "TaskWasteDetect.h"
#include "Tasktempmodule.h"
#include "tempfan.h"
#include "iosrc.h"
#include "functrl.h"
//#include "QRCodeMod.h"
#include "XH711.h"
/*****************************************************************************************
全局变量定义
*****************************************************************************************/

/*****************************************************************************************
数据结构与数据类型定义
*****************************************************************************************/

/*****************************************************************************************
内部全局变量申明
*****************************************************************************************/

/*****************************************************************************************
内部函数申明
*****************************************************************************************/

/******************************************************************************
外部变量声明
******************************************************************************/
/******************************************************************************
外部函数
******************************************************************************/
extern void XcanProtocolInit(void);
extern void XcanTaskInit(void);
extern void MoveModuleTask(void);
extern void CreateSendDataTask(void);
/*****************************************************************************************
函数名：int main(void)
参数描述：
无
返回值描述：无
说明：
主函数

特殊算法：
注意事项：
*****************************************************************************************/
void main(void)
{
    for(__IO int i = 0; i<0xffff; i++);
    for(__IO int i = 0; i<0xffff; i++);
    SystemInit();
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);//0x8000
    __set_PRIMASK(0);
    DevCoreInit();
    DevIo->RemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //开启SWD调试，关闭jtag
    MotorCtrl.Init();
    Temperature.Init();
    TempModuleInit();
    FunCtrl.Init();
    OpSensor.Init();
    //Mpu6050DevInit();
    Init_HX711pin();//没有称重模块需要屏蔽，否则可能会出现卡死，并把bOldVer 设置为0
    XcanTrsInit();
    XcanProtocolInit();
    for(__IO int i = 0; i<0xfffff; i++);
    SysDatainit();
    CreateTempModuleTask();
    CreateFanTempTask();
    MoveModuleTask();
    CreateSendDataTask();//没有称重模块需要屏蔽
    XcanTaskInit();
    CreateLedCheckTask();
    vTaskStartScheduler();
    while(1);
}

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {}
}
#endif