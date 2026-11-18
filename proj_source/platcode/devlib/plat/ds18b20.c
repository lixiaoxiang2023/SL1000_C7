/*******************************************************************************
文件名称：   ds18b20.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20160308
程序用途：   ds18b20驱动程序

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20160308：第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_DS18B20)
/*******************************************************************************
Constant Definition
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
外部变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
//static struct
//{
//    Ds18b20InitTdf stInit;
//    bool blIsExist;
//    bool blIsWork;
//    Ds18b20CtrlType stCtrl;
//
//}stDs18b20[MAX_TS];

static struct
{
    unsigned int *pwdOutSda;   //驱动器的控制引脚
    unsigned int *pwdInSda;   //驱动器的控制引脚
    unsigned char byTimNO;

    bool blIsExist;
    bool blIsWork;

    unsigned int dwCnt;
    unsigned int dwSeqNeed;//需要的序列个数
	unsigned char abySeq[6];//序列

    union {
        short signed int wDat;
        struct {
            unsigned char byDatL;
            unsigned char byDatH;
        }stDat;
    };
}stDs18b20[MAX_TS];


static u8 s_byHwn =0;
/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 对ds18b20复位时需要的定时器中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void ResetHandler(unsigned int Hwnd)
{
    Hwnd = s_byHwn;
    switch (stDs18b20[Hwnd].dwCnt)
    {
    case 0: *stDs18b20[Hwnd].pwdOutSda = 1; break; //DQ= 1复位
    case 1: *stDs18b20[Hwnd].pwdOutSda = 0; break; //DQ= 1复位
    case 9: *stDs18b20[Hwnd].pwdOutSda = 1; break; //延时600微秒（该时间的时间范围可以从480到960微秒）
    case 10:
        if(*stDs18b20[Hwnd].pwdInSda == 1) stDs18b20[Hwnd].blIsExist = false;
        else stDs18b20[Hwnd].blIsExist = true;
        break;
    case 17:
        DevTim->Stop(stDs18b20[Hwnd].byTimNO);
        stDs18b20[Hwnd].blIsWork = false;
    default :
        break;
    }

    stDs18b20[Hwnd].dwCnt ++;
}

/*******************************************************************************
函数功能：
*   @ 对ds18b20复位
参数描述：
*   @ 无
返回值描述：
*   @ 是否存在传感器
说明：
*   @ 使用定时器进行操作
*******************************************************************************/
static int Reset(unsigned int Hwnd)
{
    stDs18b20[Hwnd].dwCnt = 0;
    stDs18b20[Hwnd].blIsWork = true;
    DevTim->TimEvent(stDs18b20[Hwnd].byTimNO, 60, ResetHandler, 0);
    u32 dwCnt=0;
    while(stDs18b20[Hwnd].blIsWork) {;
    if(dwCnt++>0x1fffff)
      break;

    }

    if(!stDs18b20[Hwnd].blIsExist) return -1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ ds18b20温度读取操作序列的定时器回调函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*******************************************************************************/
static void CoreHandler(unsigned int Hwnd)
{
    Hwnd = s_byHwn;
    unsigned char byFunc = stDs18b20[Hwnd].dwCnt % 8;
    unsigned char byCharSqr = stDs18b20[Hwnd].dwCnt / 64;
    unsigned char byCharBit = (stDs18b20[Hwnd].dwCnt / 8) % 8;

    if(stDs18b20[Hwnd].dwCnt >= stDs18b20[Hwnd].dwSeqNeed * 64) {
        DevTim->Stop(stDs18b20[Hwnd].byTimNO);
        stDs18b20[Hwnd].blIsWork = false;
        return ;
    }

    switch(byFunc) {
    case 0: *stDs18b20[Hwnd].pwdOutSda = 0; break;
    case 2:
        if(stDs18b20[Hwnd].abySeq[byCharSqr] & (1<<byCharBit)) *stDs18b20[Hwnd].pwdOutSda = 1;
        else *stDs18b20[Hwnd].pwdOutSda = 0;
        break;

    case 3:
        stDs18b20[Hwnd].wDat >>= 1;
        if(*stDs18b20[Hwnd].pwdInSda == 1) stDs18b20[Hwnd].wDat |= 0x8000;
        else  stDs18b20[Hwnd].wDat &= ~0x8000;
        break;

    case 6: *stDs18b20[Hwnd].pwdOutSda = 1; break;
    default : break;
    }

    stDs18b20[Hwnd].dwCnt ++;
}

/*******************************************************************************
函数功能：
*   @ ds18b20温度读取操作序列
参数描述：
*   @ 需要操作的数据个数
返回值描述：
*   @ 无
说明：
*******************************************************************************/
static void WrRd(unsigned int Hwnd, unsigned int dwCnt)
{
    stDs18b20[Hwnd].blIsWork = true;
    stDs18b20[Hwnd].dwSeqNeed = dwCnt;
    stDs18b20[Hwnd].dwCnt = 0;
    uint32_t dwlCnt = 0;
    DevTim->TimEvent(stDs18b20[Hwnd].byTimNO, 7, CoreHandler, 0);
    while(stDs18b20[Hwnd].blIsWork) {
            if(dwlCnt++>0x1fffff)
      break;}
}

/*******************************************************************************
函数功能：
*   @ ds18b20温度读取
参数描述：
*   @ 无
返回值描述：
*   @ 温度*10
说明：
*******************************************************************************/
static short signed int Read(unsigned int Hwnd)
{
    s_byHwn = Hwnd;
    short unsigned int wTemp = 0;
    if( Reset(Hwnd) ) return -1;

    stDs18b20[Hwnd].abySeq[0] = 0xCC;//跳过读序号列号的操作
    stDs18b20[Hwnd].abySeq[1] = 0x44;//启动温度转换

    WrRd(Hwnd,2);

    Reset(Hwnd);

    stDs18b20[Hwnd].abySeq[0] = 0xCC;//跳过读序号列号的操作
    stDs18b20[Hwnd].abySeq[1] = 0xBE;//读取温度寄存器
    stDs18b20[Hwnd].abySeq[2] = 0xff;//启动温度转换
    stDs18b20[Hwnd].abySeq[3] = 0xff;//启动温度转换

    WrRd(Hwnd,4);

    wTemp = (short unsigned int)((stDs18b20[Hwnd].stDat.byDatH << 8) | stDs18b20[Hwnd].stDat.byDatL);
    wTemp &= 0x07ff;
    return ((wTemp >> 4)*10 + (short unsigned int)((float)(wTemp & 0x000F)*0.625));
}

static const DevDs18b20Tdf s_Dev = {
    Read,
};

/*******************************************************************************
函数功能：
*   @ ds18b20温度传感器初始化
参数描述：
*   @ 初始化参数
返回值描述：
*   @ 方法合集
说明：
*******************************************************************************/
const DevDs18b20Tdf* Ds18b20Init(Ds18b20InitTdf *pstInit)
{
    unsigned int dwPre = 0;
    ClocksTypeDef stRcc;

    DevRccGetClocksFreq(&stRcc);
    if(pstInit->byTimNO == 0 || pstInit->byTimNO == 7) dwPre = stRcc.PCLK2 / (1000*1000);
    dwPre = stRcc.PCLK1 / (1000*1000);

    dwPre  = dwPre*2-1;
    stDs18b20[pstInit->byHwnd].pwdOutSda = pstInit->pwdOutSda;
    stDs18b20[pstInit->byHwnd].pwdInSda = pstInit->pwdInSda;
    stDs18b20[pstInit->byHwnd].byTimNO = pstInit->byTimNO;

    DevTim->Config(pstInit->byTimNO, dwPre, 15);
    Reset(pstInit->byHwnd);

    return &s_Dev;
}

#endif