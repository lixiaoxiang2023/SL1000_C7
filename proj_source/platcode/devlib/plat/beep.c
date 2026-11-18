/*******************************************************************************
文件名称：   beep.c
文件目录：   $PROJ_DIR$\..\code\User\Driver\src
项目名称：   Urit-50STM32升级版
创建者：     陆作沛
创建日期：   2013-04-10
程序用途：   1.完成按键蜂鸣器检测

版权声明：Copyright urit Co.,Ltd.All Right Reserved

日期：2013-04-15  陆作沛
第二次修改：完成编程格式规范

日期：2016.04.21 胥强
加入技术平台

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_BEEP)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
Local Variables Definition内部全局变量定义
*******************************************************************************/
static struct {
    unsigned char byTimNo;
    unsigned int *pwdBeep; //光耦输入信号的位定义地址

#if(CONFIG_BEEP_CLK_DRIVE)
    unsigned int dwTimPeriod;
    unsigned int dwTimCnt;
#endif
}s_DevCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 用于自动关闭蜂鸣器
参数描述：
*   @ 未使用
返回值描述：
*   @ 无
说明：
*   @ 此函数在定时器终端中使用
*******************************************************************************/
static void KernelProg(unsigned int dwPare)
{
#if(!CONFIG_BEEP_CLK_DRIVE)
    *s_DevCtr.pwdBeep = 0;
    DevTim->Stop(s_DevCtr.byTimNo);
#else
    if(s_DevCtr.dwTimCnt <= 1) {
        *s_DevCtr.pwdBeep = 0;
        DevTim->Stop(s_DevCtr.byTimNo);
        return;
    }

    if(s_DevCtr.dwTimCnt % 2 == 0) *s_DevCtr.pwdBeep = 0;
    else *s_DevCtr.pwdBeep = 1;

    s_DevCtr.dwTimCnt --;
#endif
}

#if(CONFIG_BEEP_CLK_DRIVE)
#if(CONFIG_BEEP_SET_SPD)
/*******************************************************************************
函数功能：
*   @ 设定驱动蜂鸣器的频率
参数描述：
*   @ 未使用
返回值描述：
*   @ 无
说明：
*   @ 此函数在定时器终端中使用
*******************************************************************************/
static void SetClkSpd(unsigned int dwClkSpd)
{
    s_DevCtr.dwTimPeriod = dwClkSpd;
}
#endif
#endif

/*******************************************************************************
函数功能：
*   @ 写蜂鸣器
参数描述：
*   @ unsigned int dwCnt：蜂鸣器响的延时时间50ms*wCnt
返回值描述：
*   @  0：ok
说明：
*   @ 无
*******************************************************************************/
static int Write(unsigned int dwCnt)
{
#if(!CONFIG_BEEP_CLK_DRIVE)
    *s_DevCtr.pwdBeep = 1;
    DevTim->TimEvent(s_DevCtr.byTimNo, 10 * dwCnt, KernelProg, 0);
#else
    s_DevCtr.dwTimCnt = dwCnt;
    DevTim->TimEvent(s_DevCtr.byTimNo, s_DevCtr.dwTimPeriod, KernelProg, 0);
#endif

    return 0;
}

#if(CONFIG_BEEP_WRITE_BIT)
/*******************************************************************************
函数功能：
*   @ 写蜂鸣器控制位
参数描述：
*   @ unsigned int dwCnt：向蜂鸣器管教写数据
返回值描述：
*   @  0：ok
说明：
*   @ 无
*******************************************************************************/
static int WriteBit(unsigned int dwCmd)
{
    *s_DevCtr.pwdBeep = dwCmd;

    return 0;
}
#endif

static const DevBeepTdf s_Dev = {

    Write,


#if(CONFIG_BEEP_CLK_DRIVE)
    #if(CONFIG_BEEP_SET_SPD)
    SetClkSpd,
    #endif
#endif

#if(CONFIG_BEEP_WRITE_BIT)
    WriteBit,
#endif
};

/*******************************************************************************
函数功能：
*   @ 蜂鸣器初始化
参数描述：
*   @ unsigned int dwCnt：蜂鸣器响的延时时间50ms*wCnt
返回值描述：
*   @  0：ok
说明：
*   @ 无
*******************************************************************************/
const DevBeepTdf *BeepInit(DevBeepInitTdf *pstInit)
{
    s_DevCtr.pwdBeep = pstInit->pwdBeepGpio;
    s_DevCtr.byTimNo = pstInit->byTimNO;

    *s_DevCtr.pwdBeep = 0;

    DevTim->Config(s_DevCtr.byTimNo, pstInit->dwTimPre, pstInit->dwTimPeriod);
#if(CONFIG_BEEP_CLK_DRIVE)
    s_DevCtr.dwTimPeriod = pstInit->dwTimPeriod;
#endif

    return &s_Dev;
}

#endif