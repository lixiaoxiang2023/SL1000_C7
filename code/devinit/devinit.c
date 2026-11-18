/*******************************************************************************
文件名称：   devinit.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20160308
程序用途：   初始化平台的所有硬件驱动模块

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20160308：第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devinit.h"
#include "USART.h"
#include "freertos.h"
#include "iosrc.h"
#include "queue.h"
#include "TaskWasteDetect.h"
/*******************************************************************************
常量定义
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/

void UartGpioInit(void)
{
    DevIo->Config(PA, 9, 1, Mode_AF_PP);
    DevIo->Config(PA,10, 1, Mode_IN_FLOATING);
    DevUart->Config(0, 19200, 1, 0, true);
}

/*******************************************************************************
函数功能：
*   @ 初始化can设备
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：无
*******************************************************************************/
void XcanTrsInit(void)
{
    DevIo->RemapConfig(GPIO_Remap2_CAN1,ENABLE);
    DevIo->Config(PD, 0, 1, Mode_IPU);
    DevIo->Config(PD, 1, 1, Mode_AF_PP);
    DevXcan = XcanInit();
}

#if(CONFIG_DS18B20)
/*******************************************************************************
函数功能：
*   @ 对于蜂鸣器初始化
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void Ds18b20TmpInit(void)
{
    Ds18b20InitTdf stInit;

    DevIo->Config(PC, 3, 1, Mode_Out_OD); //sda
    stInit.byTimNO = 5; // tim6
    stInit.pwdOutSda = &BIT_GPIOC_ODR_ODR3;
    stInit.pwdInSda  = &BIT_GPIOC_IDR_IDR3;

    DevDs18b20 = Ds18b20Init(&stInit);
}
#endif

#if(CONFIG_AT45)
/*******************************************************************************
函数功能：
对于At45存储器的应用实例初始化
参数描述：
无
返回值描述：
无
说明：
无
*******************************************************************************/
void At45MemInit(void)
{
    At45InitTdf stInit;

    DevIo->Config(PB, 15, 1, Mode_AF_PP);      //
    DevIo->Config(PB, 14, 1, Mode_AF_PP);      //
    DevIo->Config(PB, 13, 1, Mode_AF_PP);      //
    DevIo->Config(PB, 12, 1, Mode_Out_PP);     // CS

    stInit.bySpiNo = 1;//SPI2
    stInit.pdwNcs = &BIT_GPIOB_ODR_ODR12;
    stInit.bySpiDmaRxNo = 0;//DMA1
    stInit.bySpiDmaRxChn = 4;
    stInit.bySpiDmaTxNo = 0;
    stInit.bySpiDmaTxChn = 5;

    DevAt45 = At45Init(&stInit);
}
#endif

#if(CONFIG_PWM)
/*******************************************************************************
函数功能：
单独输出输出信号初始化
参数描述：
无
返回值描述：
无
说明：
无
*******************************************************************************/
void PWMInit(void)
{
    DevPwmInitTdf stInit;

    DevIo->Config(PB, 15, 1, Mode_Out_PP); //PWM控制口
    DevIo->Config(PB, 14, 1, Mode_Out_PP); //PWM控制口
    memset(&stInit, 0, sizeof(DevPwmInitTdf));
    //stInit.byHwnd = 0;
    stInit.byTimNO = 3; //TIM1
    DevPwm = PwmInit(&stInit);
    DevPwm->AddDev(0,&BIT_GPIOB_ODR_ODR15,1);
    DevPwm->AddDev(1,&BIT_GPIOB_ODR_ODR14,1);


//    memset(&stInit, 0, sizeof(DevPwmInitTdf));
   // stInit.byHwnd = 1;
//    stInit.byTimNO = 4; //TIM5
//    DevPwm = PwmInit(&stInit);
//    DevPwm->AddDev(1,&BIT_GPIOB_ODR_ODR14,1);
}
#endif

#if(CONFIG_MPU6050)
/*******************************************************************************
函数功能：
    MPU6050应用实例初始化
参数描述：
    无
返回值描述：
    无
说明：
    无
*******************************************************************************/
void Mpu6050DevInit(void)
{
    Mpu6050InitTdf stInit;

    DevMpu6050 =  Mpu6050Init(&stInit);
    DevMpu6050->MPU6050_Init(0);
}
#endif
