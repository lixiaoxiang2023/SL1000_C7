/*******************************************************************************
文件名称：    a3983.c
文件目录：    $PROJ_DIR$/usr/Drive
项目名称：    事业一部技术平台
创建者：      胥强
创建日期：    20130721
程序用途：    a3983电机驱动程序

版权声明：Copyright urit Co.,Ltd.All Right Reserved
log:
2013-11-23:
修改人：胥强
1）增加一个电机支持多个光耦传感器功能
2）增加一个定时器支持多个电机功能
2015-5-21:
修改人：胥强
1）将控制参数，方向，半流，细分等参数修改为位控制，便于函数的识别与使用。
2）增加对电机选择复位光耦与复位光耦电平的接口函数。
2016-01-05:
修改人：陶年生
1）修正抛物线加速表的加速度算法错误；
2）将加速表的算法统一更改为先计算加速值，再进行加速表前后对称计算；
3）double型fY四舍五入计算更改为先加0.5后取整
2016-03-10:
修改人：胥强
1）独立定时器模块，此版本之后的电机驱动需要定时器的依赖
2）整理独立电机接口
2018.11.17 胥强，增加电机在运行过程中实时修改电机速度方法。
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <math.h>
#include "devplat.h"

#if(CONFIG_A3983)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#ifdef USE_SIN_UP_SPEED
static const double faSin[20] = {
    0, 0.0784590957, 0.156434465,  0.2334453639, 0.3090169944,
    0.3826834324, 0.4539904997, 0.5224985647, 0.5877852523, 0.6494480483,
    0.7071067812, 0.7604059656, 0.8090169944, 0.8526401644, 0.8910065242,
    0.9238795325, 0.9510565163, 0.9723699204, 0.9876883406, 0.9969173337
};
#endif

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
typedef struct
{
	unsigned int dwSteps;       //电机需要运行的步数
	unsigned int dwMaxSteps;    //电机需要运行的最大步数步数，在光耦停止模式有效
    unsigned int dwRestSteps;   //电机运行在发现光耦后继续运行此步数后停止
    unsigned int dwStartSped;   //电机起始频率
    unsigned int dwMaxSped;     //电机最大频率
    unsigned int dwSpdUp;       //电机每一次加速内需要运行的步数

    unsigned short int wOpTab[40];         //电机加速表
    unsigned int dwOpSpedTab[40];           //电机对应速度表
    unsigned int dwUpPo;                    //加速终止点
    unsigned int dwDmPo;                    //减速开始点

    void (*pfRunFunc)(void);                //电机运行过程中的用户应用接口
    bool blNeedSpeedUp;                      //是否需要加速處理

    union {
        unsigned char byCtr;
        struct {
            unsigned char byDir:1;          //电机运行的方向
            unsigned char byStopMode:3;     //电机停止模式，是否找光耦
            unsigned char byIsReNow:1;      //是否立即返回
            unsigned char byIsHalf:1;       //是否使用小电流运行
            unsigned char byDiv:2;          //电机细分
        }BitCtr;
    };
}stA3983Tdf;

static struct {
    stA3983InitTdf stInit;                   //电机初始化参数
    stA3983Tdf stCtr;                        //电机控制参数

    unsigned short int wOpTab[40];         //电机加速表
    unsigned int dwOpSpedTab[40];           //电机对应速度表

    unsigned int dwRunStep;                 //电机运行步数计数器
    unsigned int dwSteps;                   //走的真实值
    unsigned int dwUpPo;                    //加速终止点
    unsigned int dwDmPo;                    //减速开始点
    unsigned int dwSpdUp;                   //电机每一次加速内需要运行的步数

    unsigned int dwCurrentSped;             //电机当前的运行速度

    unsigned char byOpCnt;                  //电机步进加速状态计数
    unsigned char byOpCntBkp;               //电机步进加速状态计数备份

    unsigned char byOPRestCnt;              //电机到复位光耦位置内计数
    bool blNeedSpeedUp;                      //是否需要加速處理
    bool blOping;                            //电机加速状态

    bool blIsBusy;
} s_Dev[MAX_MO];

/*******************************************************************************
内部全局变量定义
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/
static void MotorKernelProg(unsigned int dwHwnd);

/*******************************************************************************
函数功能：
*   @ 计算加速曲线
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @  0：电机启动运行正常
*   @ -1：电机启动运行失败，即电机正在运行中
说明：
*   @ 本函数将根据抛物线函数y= -k*(x-x0)^2 + C,计算加速曲线，即电机将执行匀加加速过程。
*******************************************************************************/
static int SpeedUpTab( unsigned int dwHwnd )
{
    stA3983Tdf *pstPare;
    unsigned int dwSpedStart;
    unsigned int dwSpedMax;
    unsigned int dwUsedSteps = 0;
    unsigned int  dwSpedTmp = 0;

    double fY;
    double fY_temp;
    unsigned short int wTimTmp = 0;
    bool blIsSpedUp = true;

    pstPare = &s_Dev[dwHwnd].stCtr;

    if(pstPare->dwStartSped > pstPare->dwMaxSped) {
        dwSpedStart = pstPare->dwMaxSped;
        dwSpedMax = pstPare->dwStartSped;
        blIsSpedUp = false;
    }
    else {
        dwSpedStart = pstPare->dwStartSped;
        dwSpedMax = pstPare->dwMaxSped;
    }

    pstPare->blNeedSpeedUp = false;
    for (int i = 0; i < 40; i++) {

        // 注意：定时器的基准频率时间为1uS,计算时间周期
        // 增加溢出处理 溢出按最小频率执行 lixx 2019.4.15 10：54
        pstPare->wOpTab[i] = (((double) (1 / (double) dwSpedMax) * 1000 * 1000)>0XFFFF)
          ? 0XFFFF:(unsigned short int) ((double) (1 / (double) dwSpedMax) * 1000 * 1000);
        // pstPare->wOpTab[i] = (unsigned short int) ((double) (1 / (double) dwSpedMax) * 1000 * 1000);

        pstPare->dwOpSpedTab[i] = dwSpedMax;
    }

    if (dwSpedStart == dwSpedMax) return -1;
    //根据细分运算需要的电机步数
    dwUsedSteps = pstPare->dwSteps;
    if (pstPare->dwSpdUp * 40 * 2 >= dwUsedSteps) return -1;

    unsigned int dwDiffSpeed =  (dwSpedMax - dwSpedStart) >> 1;
#ifdef USE_SIN_UP_SPEED
    for (int fX = 1; fX < 20; fX++) {
        fY_temp = faSin[fX] * dwDiffSpeed; //先计算加速值

        fY = dwSpedStart + fY_temp;   //加速表前后对称计算
        pstPare->dwOpSpedTab[fX - 1] = (unsigned int) fY;
        fY = (1 / fY) * 1000 * 1000;
        pstPare->wOpTab[fX - 1] = (unsigned short int)(fY + 0.5);

        fY = dwSpedMax - fY_temp;     //加速表前后对称计算
        pstPare->dwOpSpedTab[39 - fX] = (unsigned int) fY;
        fY = (1 / fY) * 1000 * 1000;
        pstPare->wOpTab[39 - fX] = (unsigned short int)(fY + 0.5);
    }
#else
    double fk = (double) dwDiffSpeed / (double) (20 * 20);
    for (int fX = 1; fX < 20; fX++) {
        fY_temp = fk * fX * fX;            //先计算加速值

        fY = dwSpedStart + fY_temp;   //加速表前后对称计算
        pstPare->dwOpSpedTab[fX - 1] = (unsigned int) fY;
        fY = (1 / fY) * 1000 * 1000;
        pstPare->wOpTab[fX - 1] = (unsigned short int)(fY + 0.5);

        fY = dwSpedMax - fY_temp;     //加速表前后对称计算
        pstPare->dwOpSpedTab[39 - fX] = (unsigned int) fY;
        fY = (1 / fY) * 1000 * 1000;
        pstPare->wOpTab[39 - fX] = (unsigned short int)(fY + 0.5);
    }
#endif

    fY = dwSpedStart + dwDiffSpeed;       //计算中间值
    pstPare->dwOpSpedTab[19] = (unsigned int) fY;
    fY = (1 / fY) * 1000 * 1000;
    pstPare->wOpTab[19] = (unsigned short int)(fY + 0.5);

    fY = dwSpedMax;                       //计算最大值
    pstPare->dwOpSpedTab[39] = (unsigned int) fY;
    fY = (1 / fY) * 1000 * 1000;
    pstPare->wOpTab[39] = (unsigned short int)(fY + 0.5);

    pstPare->dwUpPo = pstPare->dwSpdUp * 40;
    pstPare->dwDmPo = pstPare->dwSteps - pstPare->dwUpPo;
    pstPare->blNeedSpeedUp = true;

    if(blIsSpedUp == false) {
        // 处理开始减速情况
        for(int i = 0; i < 20 ; i ++) {
            dwSpedTmp = pstPare->dwOpSpedTab[39 - i];
            pstPare->dwOpSpedTab[39 - i] = pstPare->dwOpSpedTab[i];
            pstPare->dwOpSpedTab[i] = dwSpedTmp;

            wTimTmp = pstPare->wOpTab[39 - i];
            pstPare->wOpTab[39 - i] = pstPare->wOpTab[i];
            pstPare->wOpTab[i] = wTimTmp;
        }
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动电机运行
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 此函数将停止定时器，同时清除控制电机运行的各个参数
*******************************************************************************/
static void Start(unsigned int dwHwnd)
{
    stA3983InitTdf *pstInit = &s_Dev[dwHwnd].stInit;

    s_Dev[dwHwnd].blIsBusy = TRUE;
    DevTim->SetArrPwm(pstInit->dwTimNo, pstInit->dwTimChnNo, s_Dev[dwHwnd].wOpTab[0], s_Dev[dwHwnd].wOpTab[0] / 2);
    DevTim->StartPwm(pstInit->dwTimNo, pstInit->dwTimChnNo, MotorKernelProg , dwHwnd);
}

/*******************************************************************************
函数功能：
*   @ 停止电机运行
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 此函数将停止定时器，同时清除控制电机运行的各个参数。
*******************************************************************************/
static void Stop(unsigned int dwHwnd)
{
    stA3983InitTdf *pstInit = &s_Dev[dwHwnd].stInit;
    //半流控制
    s_Dev[dwHwnd].blIsBusy = FALSE;

    DevTim->StopPwm(pstInit->dwTimNo, pstInit->dwTimChnNo);
    if(pstInit->pwdHalfGpio) *pstInit->pwdHalfGpio = 1;
}

/*******************************************************************************
函数功能：
*   @ 电机运行主处理函数
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void MotorKernelProg(unsigned int dwHwnd)
{
    //运行步数增加
    s_Dev[dwHwnd].dwSteps ++;
    s_Dev[dwHwnd].dwRunStep ++;
    s_Dev[dwHwnd].blOping = TRUE;

    if ((s_Dev[dwHwnd].dwRunStep < s_Dev[dwHwnd].dwUpPo) && (s_Dev[dwHwnd].blNeedSpeedUp == TRUE)) {
        if (s_Dev[dwHwnd].dwRunStep % s_Dev[dwHwnd].dwSpdUp == 0) {
            s_Dev[dwHwnd].byOpCnt ++;
            s_Dev[dwHwnd].dwCurrentSped = s_Dev[dwHwnd].dwOpSpedTab[s_Dev[dwHwnd].byOpCnt];
        }
    }
    else if ((s_Dev[dwHwnd].dwRunStep > s_Dev[dwHwnd].dwDmPo) && (s_Dev[dwHwnd].blNeedSpeedUp == TRUE)) {
        if (((s_Dev[dwHwnd].stCtr.dwSteps - s_Dev[dwHwnd].dwRunStep) % s_Dev[dwHwnd].dwSpdUp == 0) && (s_Dev[dwHwnd].byOpCnt != 0)) {
            s_Dev[dwHwnd].byOpCnt --;
        }
        else {
            s_Dev[dwHwnd].blOping = FALSE;
        }
    }
    else {
        s_Dev[dwHwnd].blOping = FALSE;
    }

    //更新加速表
    if (s_Dev[dwHwnd].blOping == TRUE && (s_Dev[dwHwnd].byOpCntBkp != s_Dev[dwHwnd].byOpCnt)) {
        s_Dev[dwHwnd].byOpCntBkp = s_Dev[dwHwnd].byOpCnt;

        DevTim->SetArrPwm(s_Dev[dwHwnd].stInit.dwTimNo, \
            s_Dev[dwHwnd].stInit.dwTimChnNo, \
            s_Dev[dwHwnd].wOpTab[s_Dev[dwHwnd].byOpCnt] ,\
            s_Dev[dwHwnd].wOpTab[s_Dev[dwHwnd].byOpCnt] / 2);
    }

    //检查光耦,如果遇见光耦则停止
    if (s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 0 && s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 5) {
        if (*(s_Dev[dwHwnd].stInit.pwdOpGpio) == s_Dev[dwHwnd].stInit.byAtOptLv) {
            if(s_Dev[dwHwnd].byOPRestCnt <= 10) s_Dev[dwHwnd].byOPRestCnt ++;

            if (s_Dev[dwHwnd].byOPRestCnt == 10) {
                if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 1) s_Dev[dwHwnd].stCtr.dwMaxSteps = s_Dev[dwHwnd].dwRunStep;
                else if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 2) {
                    // 发现光耦后继续运行设定的步数
                    if(s_Dev[dwHwnd].dwRunStep < s_Dev[dwHwnd].dwDmPo && s_Dev[dwHwnd].dwRunStep > s_Dev[dwHwnd].dwUpPo) {
                        signed int dwStepDiff = (signed int)s_Dev[dwHwnd].dwDmPo - (signed int)s_Dev[dwHwnd].dwRunStep;
                        dwStepDiff = dwStepDiff - (signed int)s_Dev[dwHwnd].stCtr.dwRestSteps;
                        s_Dev[dwHwnd].dwRunStep = (signed int)s_Dev[dwHwnd].dwRunStep + dwStepDiff;
                    }
                }
                else if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 3) {
                    // 发现光耦后继续开始减速
                    if(s_Dev[dwHwnd].dwRunStep < s_Dev[dwHwnd].dwDmPo && s_Dev[dwHwnd].dwRunStep > s_Dev[dwHwnd].dwUpPo) {
                        s_Dev[dwHwnd].dwRunStep = s_Dev[dwHwnd].dwDmPo;
                    }
                }
                else if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 4) {
                    // 发现光耦后继续运行步数后停止
                    s_Dev[dwHwnd].stCtr.dwSteps = s_Dev[dwHwnd].dwRunStep + s_Dev[dwHwnd].stCtr.dwRestSteps;
                    s_Dev[dwHwnd].stCtr.dwMaxSteps = s_Dev[dwHwnd].stCtr.dwSteps;
                }
            }
        }
    }
    else {
        if (s_Dev[dwHwnd].dwRunStep == s_Dev[dwHwnd].stCtr.dwSteps) {        //按步数模式走完相应的步数，现在停止
            s_Dev[dwHwnd].dwRunStep = s_Dev[dwHwnd].stCtr.dwMaxSteps;
        }
    }

    if (s_Dev[dwHwnd].dwRunStep == s_Dev[dwHwnd].stCtr.dwMaxSteps) {
        Stop(dwHwnd);
    }

    if (s_Dev[dwHwnd].stCtr.pfRunFunc != NULL) s_Dev[dwHwnd].stCtr.pfRunFunc();
}

/*******************************************************************************
函数功能：
*   @ 停止电机运行
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ true：在光耦上面
*   @ false：不再光耦上面
说明：
*   @ 是否在光耦上面
*******************************************************************************/
static bool OnSensor(unsigned int dwHwnd)
{
    return (*(s_Dev[dwHwnd].stInit.pwdOpGpio) == s_Dev[dwHwnd].stInit.byAtOptLv)? true:false;
}

/*******************************************************************************
函数功能：
*   @ 停止电机运行
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
*   @ bool blEnable：true=启动；false=关闭
返回值描述：
*   @ 无
说明：
*   @ 启动/关闭半流控制
*******************************************************************************/
static void CtrHalf(unsigned int dwHwnd, bool blEnable)
{
    if(!s_Dev[dwHwnd].stInit.pwdHalfGpio) return;

    if (blEnable == TRUE) *s_Dev[dwHwnd].stInit.pwdHalfGpio = 1;
    else *s_Dev[dwHwnd].stInit.pwdHalfGpio = 0;
}

/*******************************************************************************
函数功能：
*   @ 设定电机的光耦端口与电平状态。
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
*   @ unsigned int *pwdOpGpio：光耦端口的位地址
*   @ unsigned char byLevel：在光耦处的电平
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SetOp(unsigned int dwHwnd, unsigned int *pwdOpGpio, unsigned char byLevel)
{
    if(dwHwnd > MAX_MO) return;
    if( !s_Dev[dwHwnd].stInit.pwdOpGpio ) return;

    s_Dev[dwHwnd].stInit.pwdOpGpio = pwdOpGpio;
    s_Dev[dwHwnd].stInit.byAtOptLv = byLevel;
}

/*******************************************************************************
函数功能：
*   @ 获取电机的光耦端口与电平状态。
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
*   @ unsigned int *pwdOpGpio：光耦端口的位地址
*   @ unsigned char byLevel：在光耦处的电平
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int GetOp(unsigned int dwHwnd)
{
	if(dwHwnd > MAX_MO) return -1;
    if( !s_Dev[dwHwnd].stInit.pwdOpGpio ) return -1;

    return *(s_Dev[dwHwnd].stInit.pwdOpGpio);
}

/*******************************************************************************
函数功能：
*   @ 取得电机的当前运行步数。
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int GetSteps(unsigned int dwHwnd)
{
    if(dwHwnd > MAX_MO) return -1;
    return s_Dev[dwHwnd].dwSteps;
}

/*******************************************************************************
函数功能：
*   @ 初始化运行参数
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int RunPareInit(unsigned int dwHwnd)
{
    // 拷贝速度参数
    for(int i = 0; i < 40 ; i ++) {
        s_Dev[dwHwnd].wOpTab[i] = s_Dev[dwHwnd].stCtr.wOpTab[i];
        s_Dev[dwHwnd].dwOpSpedTab[i] = s_Dev[dwHwnd].stCtr.dwOpSpedTab[i];
    }

    s_Dev[dwHwnd].dwUpPo = s_Dev[dwHwnd].stCtr.dwUpPo;
    s_Dev[dwHwnd].dwDmPo = s_Dev[dwHwnd].stCtr.dwDmPo;
    s_Dev[dwHwnd].dwSpdUp = s_Dev[dwHwnd].stCtr.dwSpdUp;

    s_Dev[dwHwnd].dwSteps = 0;
    s_Dev[dwHwnd].dwRunStep = 0;
    s_Dev[dwHwnd].byOpCnt = 0;
    s_Dev[dwHwnd].byOpCntBkp = 0;
    s_Dev[dwHwnd].byOPRestCnt = 0;

    s_Dev[dwHwnd].blNeedSpeedUp = s_Dev[dwHwnd].stCtr.blNeedSpeedUp;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 获取电机运行参数，启动电机运行
参数描述：
*   @ unsigned int dwHwnd：  句柄，电机参数
*   @ unsigned int dwSteps： 电机需要运行的步数，电机在运行到此步数时，其完成减速了
*   @ unsigned int dwMaxSteps： 电机运行的最大步数，到此步数时电机会立即停止，可以小于dwSteps，就没有减速了
*   @ unsigned int dwStart： 电机的启动频率
*   @ unsigned int dwMax： 电机的最大频率
*   @ unsigned int dwSpdUp： 电机在每一个加减速的频率上需要保持的步数
*   @ void (*pfRunFunc)(void)： 在电机每走一步后需要运行的应用层回调函数
*   @ unsigned char byCtr：控制参数
*   @     #define M_CCW           0x00 0b00000000
*   @     #define M_CW            0x01 0b00000001 开启正转方式

*   @     #define M_STP           0x00 0b00000000
*   @     #define M_SEN           0x02 0b00000010 开启光耦运行方式
*   @     #define M_RST           0x04 0b00000100 开启发现光耦后运行相应步数后减速停止

*   @     #define M_RE_NOW        0x08 0b00001000 开启立即返回
*   @     #define M_HALF          0x10 0b00010000 开启半流控制

*   @     #define M_DIV0          0x00 0b00000000 不细分
*   @     #define M_DIV2          0x20 0b00100000 2细分
*   @     #define M_DIV4          0x40 0b01000000 4细分
*   @     #define M_DIV8          0x60 0b01100000 8细分
返回值描述：
*   @  0：电机启动运行正常
*   @ -1：电机启动运行失败，即电机正在运行中
说明：
    * 《使用步数停止模式》
    * 需要设定dwSteps，此时驱动程序将自动使用抛物线进行加速，总运行步数为dwSteps
    * 需要注意的是，1）当dwSteps<dwSpdUp*60时，驱动程序不能启动加速
    * 此时电机将会按照dwSpeedMax设定的速度运行。2）当dwSteps》dwStpPerSpdUp*80时
    * 驱动程序将会自动计算从dwSpeedStart到dwSpeedMax的加速曲线，并按照曲线运行。
    * 《使用传感器光耦停止模式》
    * 驱动程序将会使用dwSteps计算加速曲线，其计算方式与步数停止模式相同，
    * 需要注意的是，1）dwMaxSteps为电机运行的最大步数，即当在dwMaxSteps步数内未找到光耦停止信号，
    * 电机也会停止运行，2）电机加减速过程为dwSteps之内，如果电机在完成加减速之后仍然没有找到光耦
    * 停止信号，电机将会按照dwSpeedStart速度完成剩下的运行步数。3）在此模式中只要检测到光耦停止信号，
    * 电机将会紧急停止。
    * 20160819：
    * 增加电机运行时发现光耦后继续运行相应步数的功能，此功能是运行在步数模式，dwMaxRestSteps 为剩余步数设定
    * 此功能不能再电机的加速或者减速的时候改变运行值，所以在使用此功能时，需要注意光耦信号必须在电机直线运行时发现
*******************************************************************************/
static int Write(unsigned int dwHwnd, unsigned int dwSteps, unsigned int dwMaxRestSteps, unsigned int dwStartSped, unsigned int dwMaxSped, unsigned int dwSpdUp,
                    void (*pfRunFunc)(void), unsigned char byCtr)
{
    s_Dev[dwHwnd].stCtr.byCtr = byCtr;
    s_Dev[dwHwnd].stCtr.dwSteps = dwSteps;
    s_Dev[dwHwnd].stCtr.dwMaxSteps = dwMaxRestSteps;
    s_Dev[dwHwnd].stCtr.dwStartSped = dwStartSped;
    s_Dev[dwHwnd].stCtr.dwMaxSped = dwMaxSped;
    s_Dev[dwHwnd].stCtr.dwSpdUp = dwSpdUp;
    s_Dev[dwHwnd].stCtr.pfRunFunc = pfRunFunc;

    //在光耦运行模式下，电机在光耦的位置直接返回错误
    if (s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 0 && s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 5) {
        if (*(s_Dev[dwHwnd].stInit.pwdOpGpio) == s_Dev[dwHwnd].stInit.byAtOptLv) return -1;
        if (s_Dev[dwHwnd].stInit.pwdOpGpio == NULL) return -1;

        if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 2) {
            s_Dev[dwHwnd].stCtr.dwMaxSteps = dwSteps;
            s_Dev[dwHwnd].stCtr.dwRestSteps = dwMaxRestSteps;
        }

        if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 4) {
            s_Dev[dwHwnd].stCtr.dwMaxSteps = 0xffffffff;
            s_Dev[dwHwnd].stCtr.dwRestSteps = dwMaxRestSteps;
        }
    }

    if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 5) {
        // 电机不停止
        s_Dev[dwHwnd].stCtr.dwSteps = (unsigned int)(-1);
        s_Dev[dwHwnd].stCtr.dwMaxSteps = (unsigned int)(-1);
        s_Dev[dwHwnd].stCtr.dwRestSteps = dwMaxRestSteps;
    }

    //半流
    if (s_Dev[dwHwnd].stInit.pwdHalfGpio) {
        if (s_Dev[dwHwnd].stCtr.BitCtr.byIsHalf) *s_Dev[dwHwnd].stInit.pwdHalfGpio = 1;
        else *s_Dev[dwHwnd].stInit.pwdHalfGpio = 0;
    }

    //細分
    if (s_Dev[dwHwnd].stInit.pwdMS1Gpio != 0) {
        *s_Dev[dwHwnd].stInit.pwdMS1Gpio = (s_Dev[dwHwnd].stCtr.BitCtr.byDiv & 0x01);
    }
    if (s_Dev[dwHwnd].stInit.pwdMS2Gpio != 0) {
        *s_Dev[dwHwnd].stInit.pwdMS2Gpio = ((s_Dev[dwHwnd].stCtr.BitCtr.byDiv >> 1) & 0x01);
    }

    //方向
    if (s_Dev[dwHwnd].stInit.pwdDirGpio) {
        if (s_Dev[dwHwnd].stCtr.BitCtr.byDir == 1) *s_Dev[dwHwnd].stInit.pwdDirGpio = 1;
        else *s_Dev[dwHwnd].stInit.pwdDirGpio = 0;
    }

    SpeedUpTab(dwHwnd);
    RunPareInit(dwHwnd);
    Start(dwHwnd);

    // 电机一致运行时不用等待
    if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode == 5) return 0;
    if (s_Dev[dwHwnd].stCtr.BitCtr.byIsReNow != 1) {
        while (1) {
#if(CONFIG_FREERTOS)
            SysDelay(1);
#endif
            if (s_Dev[dwHwnd].blIsBusy == FALSE) return 0;
        }
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 修改电机运行速度，在电机运行的时候直接调整电机速度
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
*   @ unsigned int dwToSpeed, 修改到的速度
*   @ unsigned int dwSpdUp  速度加速步进
*   @ void (*pfRunFunc)(void) 函数的回调
返回值描述：
*   @ -1 ：运行出错
*   @ 0  ：成功
说明：
*   @ 无
*******************************************************************************/
static int ModifySpeed(unsigned int dwHwnd, unsigned int dwToSpeed, unsigned int dwSpdUp, void (*pfRunFunc)(void))
{
    if(s_Dev[dwHwnd].blIsBusy != true) return -1;
    if(s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 5) return -1;    // 必须运行在电机不停止模式

    s_Dev[dwHwnd].stCtr.dwStartSped = s_Dev[dwHwnd].dwCurrentSped;
    s_Dev[dwHwnd].stCtr.dwMaxSped = dwToSpeed;
    s_Dev[dwHwnd].stCtr.dwSpdUp   = dwSpdUp;       //电机每一次加速内需要运行的步数

    SpeedUpTab(dwHwnd);
    RunPareInit(dwHwnd);
    s_Dev[dwHwnd].stCtr.pfRunFunc = pfRunFunc;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 获取电机运行状态
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ true：电机运行中
*   @ false：电机运行完成
说明：
*   @ 无
*******************************************************************************/
static bool IsBusy(unsigned int dwHwnd)
{
    return s_Dev[dwHwnd].blIsBusy;
}

/*******************************************************************************
函数功能：
*   @ 获取电机是否正在变速
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ true：变速中
*   @ false：变速完毕
说明：
*   @ 无
*******************************************************************************/
static bool IsSpeedUpDoing(unsigned int dwHwnd)
{
    return s_Dev[dwHwnd].blOping;
}

static const DevA3983Tdf s_DevMo = {
    Stop,
    OnSensor,
    CtrHalf,
    SetOp,
	GetOp,
    Write,
    IsBusy,
    GetSteps,
    ModifySpeed,
    IsSpeedUpDoing
};

/*******************************************************************************
函数功能：
*   @ 初始化电机
参数描述：
*   @ 电机的配置参数
返回值描述：
*   @ 模块操作方法
说明：
*   @ 无
*******************************************************************************/
const DevA3983Tdf *A3983Init(stA3983InitTdf* pstInit)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    s_Dev[pstInit->byHwnd].stInit = *pstInit;
    DevRccGetClocksFreq(&stRccFreq);

    if(pstInit->dwTimNo == 0 || pstInit->dwTimNo == 7 ||
       pstInit->dwTimNo == 8 || pstInit->dwTimNo == 9 || pstInit->dwTimNo == 10) dwClk = stRccFreq.PCLK2;
    else dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000) - 1;
    //设置定时器，为1M的时钟
    DevTim->Config(pstInit->dwTimNo, dwPre, 0);
    //配置定时器的pwm通道
    DevTim->ConfigPwm(pstInit->dwTimNo, pstInit->dwTimChnNo);
    return &s_DevMo;
}
#endif