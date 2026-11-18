/*******************************************************************************
文件名称：   buttons.c
文件目录：   devlib/plat/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2017.10.28
程序用途：   基于stm32 通用独立按键驱动

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_BUTTONS)
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
内部全局变量定义
*******************************************************************************/
static struct {
    unsigned char byTimDev;
    unsigned char byExistChn;

    struct BmChnTdf {
        bool blBusy;
        unsigned char byGpio;
        unsigned char byChn;

        unsigned int  dwTimPeriod;
        unsigned int  *pwdAddrGpio;
        unsigned int  dwPressedLev;

        unsigned int  dwPressTim;
        unsigned int  dwLongPressTim;

        void (*PressHandler) (unsigned int);
        void (*LongPressHandler) (unsigned int);
    } Dev[CONFIG_BUTTONS_NUM];
} s_stDev;

static struct {
    bool blFind;
    bool blFindLongPress;
    unsigned char byActiveDev;

    unsigned int dwCnt;
    unsigned int wDelayCnt;
}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

#if(CONFIG_BUTTONS_IS_PRESSED)
/*******************************************************************************
*函数功能：
*   @ 读取按键是否按下
*参数描述：
*   @ 无
*返回值描述：
*   @ true：按下
*   @ false：未按下
*说明：
*   @ 无
*******************************************************************************/
static bool IsPressed(unsigned int dwDev)
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return false;

    pstDev = &s_stDev.Dev[dwDev];
    return (pstDev->dwPressedLev == (*pstDev->pwdAddrGpio))?true:false;
}
#endif

#if(CONFIG_BUTTONS_SET_TIM_PERIOD)
/*******************************************************************************
*函数功能：
*   @ 设置按键延时时间
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ unsigned int dwTimDelay 消抖时间，时间依赖于设定的定时器频率分频
*返回值描述：
*   @ -1 失败
*   @  0 成功
*说明：
*   @ 无
*******************************************************************************/
static int SetTimPeriod(unsigned int dwDev, unsigned int dwTimPeriod)
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return -1;

    pstDev = &s_stDev.Dev[dwDev];
    pstDev->dwTimPeriod = dwTimPeriod;

    return 0;
}
#endif

#if(CONFIG_BUTTONS_PRESSED)
/*******************************************************************************
*函数功能：
*   @ 按键在设定的时间下仍然被按下时，驱动将调用的驱动接口
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ void (*Handler) (unsigned int) 驱动接口
*返回值描述：
*   @ -1 失败
*   @  0 成功
*说明：
*   @ 无
*******************************************************************************/
static int SetPressedHandler(unsigned int dwDev, void (*Handler) (unsigned int))
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return -1;

    pstDev = &s_stDev.Dev[dwDev];
    pstDev->PressHandler = Handler;

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 设置按键延时时间
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ unsigned int dwTimDelay 消抖时间，时间依赖于设定的定时器频率分频
*返回值描述：
*   @ -1 失败
*   @  0 成功
*说明：
*   @ 无
*******************************************************************************/
static int SetPressedTimDelay(unsigned int dwDev, unsigned int dwTimDelay)
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return -1;

    pstDev = &s_stDev.Dev[dwDev];
    pstDev->dwPressTim = dwTimDelay;

    return 0;
}
#endif

#if(CONFIG_BUTTONS_LONG_PRESSED)
/*******************************************************************************
*函数功能：
*   @ 按键在设定的时间下仍然被按下时，驱动将调用的驱动接口
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ void (*Handler) (unsigned int) 驱动接口
*返回值描述：
*   @ -1 失败
*   @  0 成功
*说明：
*   @ 无
*******************************************************************************/
static int SetLongPressedHandler(unsigned int dwDev, void (*Handler) (unsigned int))
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return -1;

    pstDev = &s_stDev.Dev[dwDev];
    pstDev->LongPressHandler = Handler;

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 设置按键延时时间
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ unsigned int dwTimDelay 消抖时间，时间依赖于设定的定时器频率分频
*返回值描述：
*   @ -1 失败
*   @  0 成功
*说明：
*   @ 无
*******************************************************************************/
static int SetLongPressedTimDelay(unsigned int dwDev, unsigned int dwTimDelay)
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return -1;

    pstDev = &s_stDev.Dev[dwDev];
    pstDev->dwLongPressTim = dwTimDelay;

    return 0;
}
#endif

/*******************************************************************************
*函数功能：
*   @ 定时器通道中断
*参数描述：
*   @ unsigned int dwChn 中断通道号
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void CoreTimProg(unsigned int dwChn)
{
    struct BmChnTdf *pstDev = &s_stDev.Dev[s_stCtr.byActiveDev];

    if(pstDev->dwPressedLev == *pstDev->pwdAddrGpio) {
        if((s_stCtr.blFind == false) && (s_stCtr.dwCnt == pstDev->dwPressTim)) {
            s_stCtr.blFind = true;
            // 当没有注册长按时，就立即执行按键，否则在释放时触发按键动作。
            if(pstDev->PressHandler != 0 && pstDev->LongPressHandler == 0) pstDev->PressHandler(s_stCtr.byActiveDev);
        }

        if(s_stCtr.dwCnt == pstDev->dwLongPressTim) {
            s_stCtr.dwCnt ++;
            s_stCtr.blFindLongPress = true;
            if(pstDev->LongPressHandler) pstDev->LongPressHandler(s_stCtr.byActiveDev);
        }

        if(s_stCtr.dwCnt < pstDev->dwLongPressTim) s_stCtr.dwCnt ++;
    }
    else {
        // 当没有注册长按时，就立即执行按键，否则在释放时触发按键动作,此时按键释放。
        if(s_stCtr.blFind == true && pstDev->LongPressHandler != 0) {
            s_stCtr.blFind = false;
            if(pstDev->PressHandler && !s_stCtr.blFindLongPress) pstDev->PressHandler(s_stCtr.byActiveDev);
        }

        s_stCtr.wDelayCnt ++;

        if(s_stCtr.wDelayCnt > 2) {
            DevTim->Stop(s_stDev.byTimDev);
            for(int j = 0 ; j < s_stDev.byExistChn ; j ++) DevIo->IrpUp(s_stDev.Dev[j].byChn, 1);
            pstDev->blBusy = false;
        }
    }
}

/*******************************************************************************
*函数功能：
*   @ 按键驱动中断进入接口
*参数描述：
*   @ unsigned int dwChn 中断通道号
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void CoreProg(unsigned int dwChn)
{
    int i;
    struct BmChnTdf *pstDev = NULL;

    for(i = 0 ; i < CONFIG_BUTTONS_NUM ; i ++) {
        if(s_stDev.Dev[i].byChn == dwChn) {
            pstDev = &s_stDev.Dev[i];
            break;
        }
    }

    if(!pstDev) return ;
//    if(pstDev->blBusy) return ;

    // 开始使用定时器检查按键，并消抖
    s_stCtr.blFind = false;
    s_stCtr.blFindLongPress = false;
    s_stCtr.dwCnt = 0;
    s_stCtr.wDelayCnt = 0;

    pstDev->blBusy = true;
    s_stCtr.byActiveDev = i;

    for(int j = 0 ; j < s_stDev.byExistChn ; j ++) DevIo->IrpDown(s_stDev.Dev[j].byChn, 1);
    DevTim->TimEvent(s_stDev.byTimDev, pstDev->dwTimPeriod, CoreTimProg, s_stCtr.byActiveDev);
}

/*******************************************************************************
*函数功能：
*   @ 按键在设定的时间下仍然被按下时，驱动将调用的驱动接口
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ void (*Handler) (unsigned int) 驱动接口
*返回值描述：
*   @ -1 失败
*   @  0 成功
*说明：
*   @ 无
*******************************************************************************/
static int AddChn(unsigned int dwDev, DevButtonChnInitTdf *pstChn)
{
    struct BmChnTdf *pstDev;

    if(dwDev >= CONFIG_BUTTONS_NUM) return -1;

    pstDev = &s_stDev.Dev[dwDev];
    pstDev->blBusy          = false;
    pstDev->byGpio          = pstChn->byGpio;
    pstDev->byChn           = pstChn->byChn;

    pstDev->dwLongPressTim  = pstChn->dwLongPressTim;
    pstDev->dwPressTim      = pstChn->dwPressTim;

    pstDev->dwTimPeriod     = pstChn->dwTimPeriod;
    pstDev->pwdAddrGpio     = pstChn->pwdAddrGpio;
    pstDev->dwPressedLev    = pstChn->dwPressedLev;
    s_stDev.byExistChn ++;

    if(pstChn->dwPressedLev) DevIo->AddIrp(pstChn->byGpio, pstChn->byChn, Trigger_Rising, CoreProg);
    else DevIo->AddIrp(pstChn->byGpio, pstChn->byChn, Trigger_Falling, CoreProg);

    return 0;
}

static const DevButtonTdf s_Dev = {
#if(CONFIG_BUTTONS_IS_PRESSED)
    IsPressed,
#endif

#if(CONFIG_BUTTONS_PRESSED)
    SetPressedHandler,
    SetPressedTimDelay,
#endif

#if(CONFIG_BUTTONS_LONG_PRESSED)
    SetLongPressedHandler,
    SetLongPressedTimDelay,
#endif

    AddChn,
};

/*******************************************************************************
*函数功能：
*    @ 按键初始化
*参数描述：
*    @ 无
*返回值描述：
*    @ 无
*说明：
*    @ 无
*******************************************************************************/
const DevButtonTdf *ButtonInit(DevButtonInitTdf *pstInit)
{
    memset(&s_stDev, 0, sizeof(s_stDev));

    s_stDev.byTimDev = pstInit->byTimNO;
    DevTim->Config(pstInit->byTimNO, pstInit->dwTimPre, pstInit->dwTimPeriod);
    return &s_Dev;
}
#endif