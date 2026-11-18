/*******************************************************************************
文件名称：   parallel.c
文件目录：   $PROJ_DIR$/../code/dev/plat/src
项目名称：   Urit-50STM32升级版
创建者：     陆作沛
创建日期：   2013-08-25
程序用途：   1.并口通讯程序

版权声明：   Copyright urit Co.,Ltd.All Right Reserved

日期：2013-08-25 陆作沛
修改内容：格式规范

日期：2016-04-21 胥强
修改内容：加入技术平台

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_PARA)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
Local Variables Definition内部变量定义
*******************************************************************************/
static struct {
    DevParaInitTdf stInit;
    unsigned char byClkCnt;

    bool blIsBusy;
    unsigned char *pbyData;
    unsigned short int wLenth;
}s_DevCtr;

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

static void KernelProg(unsigned int dwPare);

/*******************************************************************************
函数功能：
*   @ 并口序列产生函数
参数描述：
*   @ 未使用
返回值描述：
*   @ 无
说明：
*   @ 此函数在定时器终端中使用
*******************************************************************************/
static void KernelProg(unsigned int dwPare)
{
    unsigned short int wIO;

    if(s_DevCtr.wLenth != 0) {
        switch(s_DevCtr.byClkCnt) {
        case 0:
            *s_DevCtr.stInit.pwdClkGpio = 1;
            break;//高电平
        case 1:
            wIO = DevIo->ReadGPIO(s_DevCtr.stInit.byGpioNo);
            wIO = (wIO & ~(((uint16_t) 0xff) << s_DevCtr.stInit.byGpioAlian)) | (((uint16_t)(*s_DevCtr.pbyData)) << s_DevCtr.stInit.byGpioAlian);
            DevIo->WriteGPIO(s_DevCtr.stInit.byGpioNo, wIO);
            s_DevCtr.pbyData ++;
        break;
        case 2:
            *s_DevCtr.stInit.pwdClkGpio = 0;
            break; //低电平
        default : break;
        }

        s_DevCtr.byClkCnt ++;
        if(s_DevCtr.byClkCnt == 3)
		{
			s_DevCtr.byClkCnt = 0;
			s_DevCtr.wLenth --;
		}
    }
    else {
        s_DevCtr.blIsBusy = false;
        DevTim->Stop(s_DevCtr.stInit.byTimNO);
    }
}
/*******************************************************************************
函数功能：
*   @ 使用并口打印
参数描述：
*   @ unsigned char *pbyData 数据源地址
*   @ unsigned short int wLenth 数据长度
返回值描述：
*   @ -1：参数错误
*   @  0：成功
说明：
*******************************************************************************/
static int Write(unsigned char *pbyIn, unsigned short int wLenth)
{
    if(!wLenth) return -1;
    if(!pbyIn) return -1;

    s_DevCtr.byClkCnt = 0;
    s_DevCtr.pbyData = pbyIn;
    s_DevCtr.wLenth = wLenth;
    s_DevCtr.blIsBusy = true;

    DevTim->TimEvent(s_DevCtr.stInit.byTimNO, 1000, KernelProg, 0);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 检查并口是否忙
参数描述：
*   @ 无
返回值描述：
*   @ false：设备空闲
*   @  true：设备忙
说明：
*******************************************************************************/
static bool IsBusy(void)
{
    return s_DevCtr.blIsBusy;
}

static DevParaTdf s_Dev = {
    Write,
    IsBusy,
};

/*******************************************************************************
函数功能：
*   @ 并口初始化
参数描述：
*   @ 无
返回值描述：
*   @ 并口操作方法
说明：
*******************************************************************************/
const DevParaTdf *ParallelInit(DevParaInitTdf *pstInit)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    DevRccGetClocksFreq(&stRccFreq);

    if(pstInit->dwTimNo == 0 || pstInit->dwTimNo == 7) dwClk = stRccFreq.PCLK2;
    else dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000) - 1;

    memset((char *)&s_DevCtr, 0, sizeof(s_DevCtr));

    s_DevCtr.stInit = *pstInit;
    DevTim->Config(pstInit->byTimNO, dwPre, 1000);
    return &s_Dev;
}

#endif