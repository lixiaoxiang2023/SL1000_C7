/*******************************************************************************
文件名称：    thb6128.c
文件目录：    $PROJ_DIR$/usr/Drive
项目名称：    事业一部技术平台
创建者：      
创建日期：    
程序用途：    thb6128电机驱动程序

版权声明：Copyright urit Co.,Ltd.All Right Reserved
log:


*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include <math.h>
#include "devplat.h"

#if(CONFIG_THB6128)
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


static struct {
    stTHB6128InitTdf stInit;                  //电机初始化参数
    stTHB6128Tdf stCtr;                       //电机控制参数
    
    unsigned short int dwOpTab[40];        //电机加速表
    unsigned int dwRunStep;                 //电机运行步数计数器
    unsigned int dwSteps;                   //走的真实值
    unsigned int dwUpPo;                    //加速终止点
    unsigned int dwDmPo;                    //减速开始点
    unsigned char byOpCnt;                  //电机步进加速状态计数
    unsigned char byOpCntBkp;               //电机步进加速状态计数备份
    
    unsigned char byOPRestCnt;              //电机到复位光耦位置内计数
    bool blNeedSpeedUp;                     //是否需要加速處理
    bool blOping;                           //电机加速状态
    
    bool blIsBusy;
} s_Dev[MAX_MO2];

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
    double fY;
    double fY_temp;
    unsigned int dwUsedSteps = 0;
    stTHB6128Tdf *pstPare;
    pstPare = &s_Dev[dwHwnd].stCtr;
    
    for (int i = 0; i < 40; i++) {
        //注意：定时器的基准频率时间为1uS,计算时间周期
        s_Dev[dwHwnd].dwOpTab[i] = (unsigned short int) ((double) (1 / (double) pstPare->dwMax) * 1000 * 1000);
    }
    
    if (pstPare->dwStart == pstPare->dwMax) return -1;
    //根据细分运算需要的电机步数
    dwUsedSteps = pstPare->dwSteps;
    if (pstPare->dwSpdUp * 40 * 2 >= dwUsedSteps) return -1;
    
    unsigned int dwDiffSpeed =  (pstPare->dwMax - pstPare->dwStart) >> 1;
#ifdef USE_SIN_UP_SPEED
    for (int fX = 1; fX < 20; fX++) {
        fY_temp = faSin[fX] * dwDiffSpeed;      //先计算加速值
        
        fY = pstPare->dwStart + fY_temp;   //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_Dev[dwHwnd].dwOpTab[fX - 1] = (unsigned short int)(fY + 0.5);
        
        fY = pstPare->dwMax - fY_temp;     //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_Dev[dwHwnd].dwOpTab[39 - fX] = (unsigned short int)(fY + 0.5);
    }
#else
    double fk = (double) dwDiffSpeed / (double) (20 * 20);
    for (int fX = 1; fX < 20; fX++) {
        fY_temp = fk * fX * fX;                 //先计算加速值
        
        fY = pstPare->dwStart + fY_temp;   //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_Dev[dwHwnd].dwOpTab[fX - 1] = (unsigned short int)(fY + 0.5);
        
        fY = pstPare->dwMax - fY_temp;     //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_Dev[dwHwnd].dwOpTab[39 - fX] = (unsigned short int)(fY + 0.5);
    }
#endif
    
    fY = pstPare->dwStart + dwDiffSpeed;       //计算中间值
    fY = (1 / fY) * 1000 * 1000;
    s_Dev[dwHwnd].dwOpTab[19] = (unsigned short int)(fY + 0.5);
    
    fY = pstPare->dwMax;                       //计算最大值
    fY = (1 / fY) * 1000 * 1000;
    s_Dev[dwHwnd].dwOpTab[39] = (unsigned short int)(fY + 0.5);
    
    s_Dev[dwHwnd].dwUpPo = pstPare->dwSpdUp * 40;
    s_Dev[dwHwnd].dwDmPo = pstPare->dwSteps - s_Dev[dwHwnd].dwUpPo;
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
    stTHB6128InitTdf *pstInit = &s_Dev[dwHwnd].stInit;
    
    DevTim->SetArrPwm(pstInit->dwTimNo, pstInit->dwTimChnNo,s_Dev[dwHwnd].dwOpTab[0], s_Dev[dwHwnd].dwOpTab[0] / 2);
    DevTim->StartPwm(pstInit->dwTimNo, pstInit->dwTimChnNo,MotorKernelProg , dwHwnd);
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
    stTHB6128InitTdf *pstInit = &s_Dev[dwHwnd].stInit;
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
        if (s_Dev[dwHwnd].dwRunStep % s_Dev[dwHwnd].stCtr.dwSpdUp == 0) s_Dev[dwHwnd].byOpCnt++;
    }
    else if ((s_Dev[dwHwnd].dwRunStep > s_Dev[dwHwnd].dwDmPo) && (s_Dev[dwHwnd].blNeedSpeedUp == TRUE)) {
        if (((s_Dev[dwHwnd].stCtr.dwSteps - s_Dev[dwHwnd].dwRunStep) % s_Dev[dwHwnd].stCtr.dwSpdUp == 0) && (s_Dev[dwHwnd].byOpCnt != 0)) {
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
                s_Dev[dwHwnd].dwOpTab[s_Dev[dwHwnd].byOpCnt] ,\
                    s_Dev[dwHwnd].dwOpTab[s_Dev[dwHwnd].byOpCnt] / 2);
    }
    
    //检查光耦,如果遇见光耦则停止
    if (s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 0) {
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
    if(dwHwnd > MAX_MO2) return;
    if( !s_Dev[dwHwnd].stInit.pwdOpGpio ) return;
    
    s_Dev[dwHwnd].stInit.pwdOpGpio = pwdOpGpio;
    s_Dev[dwHwnd].stInit.byAtOptLv = byLevel;
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
    if(dwHwnd > MAX_MO2) return -1;
    return s_Dev[dwHwnd].dwSteps;
}

/*******************************************************************************
函数名：int Write(uint32_t dwHwnd, stThb6128Tdf *pstThb6128Tdf)
函数功能：获取电机运行参数，启动电机运行
参数描述:
uint32_t dwHwnd: 电机句柄
stThb6128Tdf *pstThb6128Tdf：电机运行参数
{
bool          emIsDirCW;                              //电机运行的方向
emStopModeTdf      emStopMode;                             //电机停止模式，是否找光耦：
uint32_t                dwSteps;                                //电机需要运行的步数
uint32_t                dwMaxSteps;                             //电机需要运行的步数
uint32_t                dwSpeedStart;                           //电机起始频率
uint32_t                dwSpeedMax;                             //电机最大频率
uint32_t                dwStpPerSpdUp;                          //电机每一次加速内需要运行的步数
uint8_t                 byDiv;                                  //电机细分
bool          emIsHalfCtr;                            //是否使用小电流运行
bool          emRetNow;                               //是否立即返回
void                    (*pfRunFunc)(void);
 }
说明：
* 《使用步数停止模式》
* 需要设定dwSteps，此时驱动程序将自动使用抛物线进行加速，总运行步数为dwSteps
* 需要注意的是，1）当dwSteps<dwStpPerSpdUp*60时，驱动程序不能启动加速
* 此时电机将会按照dwSpeedMax设定的速度运行。2）当dwSteps》dwStpPerSpdUp*80时
* 驱动程序将会自动计算从dwSpeedStart到dwSpeedMax的加速曲线，并按照曲线运行。
* 《使用传感器光耦停止模式》
* 驱动程序将会使用dwSteps计算加速曲线，其计算方式与步数停止模式相同，
* 需要注意的是，1）dwMaxSteps为电机运行的最大步数，即当在dwMaxSteps步数内未找到光耦停止信号，
* 电机也会停止运行，2）电机加减速过程为dwSteps之内，如果电机在完成加减速之后仍然没有找到光耦
* 停止信号，电机将会按照dwSpeedStart速度完成剩下的运行步数。3）在此模式中只要检测到光耦停止信号，
* 电机将会紧急停止。
返回值描述：
0：电机启动运行正常
1：电机启动运行失败，即电机正在运行中
特殊算法：
注意事项：
*******************************************************************************/
static int Write(unsigned int dwHwnd,stTHB6128Tdf *pstThb6128Tdf)
{
    memcpy(&s_Dev[dwHwnd].stCtr,pstThb6128Tdf,sizeof(stTHB6128Tdf));
    
    
    //在光耦运行模式下，电机在光耦的位置直接返回错误
    if (s_Dev[dwHwnd].stCtr.BitCtr.byStopMode != 0) {
        if (*(s_Dev[dwHwnd].stInit.pwdOpGpio) == s_Dev[dwHwnd].stInit.byAtOptLv) return 1;
    }
    
    if (!SpeedUpTab(dwHwnd)) s_Dev[dwHwnd].blNeedSpeedUp = TRUE;
    else s_Dev[dwHwnd].blNeedSpeedUp = FALSE;
    
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
    
    s_Dev[dwHwnd].dwSteps = 0;
    s_Dev[dwHwnd].dwRunStep = 0;
    s_Dev[dwHwnd].byOpCnt = 0;
    s_Dev[dwHwnd].byOpCntBkp = 0;
    s_Dev[dwHwnd].byOPRestCnt = 0;
    
    s_Dev[dwHwnd].blIsBusy = TRUE;
    
    Start(dwHwnd);
    
    if (s_Dev[dwHwnd].stCtr.BitCtr.byIsReNow != 1) {
        while (1) {
            if (s_Dev[dwHwnd].blIsBusy == FALSE) return 0;
        }
    }
    
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

static int StartOnSensor(unsigned int dwHwnd)
{
    //u8 count_start=0;
    s_Dev[dwHwnd].stCtr.BitCtr.byStopMode=2;
    //   while(!DevTHB6128->Write(dwHwnd,120, 12000,  5000,  6200, 2, 0, 0))
    //   {
    //      count_start++;
    //      if(count_start>=20)
    //        return -1;
    //   
    //   };
    return 0;
}






static bool IsBusy(unsigned int dwHwnd)
{
    return s_Dev[dwHwnd].blIsBusy;
}

static const DevTHB6128Tdf s_DevMo = {
    Stop,
    OnSensor,
    CtrHalf,
    SetOp,
    // SetOp,
    Write,
    IsBusy,
    GetSteps,
    Start,
    StartOnSensor,
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
const DevTHB6128Tdf *THB6128Init(stTHB6128InitTdf* pstInit)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;
    
    s_Dev[pstInit->byHwnd].stInit = *pstInit;
    DevRccGetClocksFreq(&stRccFreq);
    
    if(pstInit->dwTimNo == 0 || pstInit->dwTimNo == 7) dwClk = stRccFreq.PCLK2;
    else dwClk = stRccFreq.PCLK1;
    
    dwPre = dwClk / (1000*1000) - 1;
    //设置定时器，为1M的时钟
    DevTim->Config(pstInit->dwTimNo, dwPre, 0);
    //配置定时器的pwm通道
    DevTim->ConfigPwm(pstInit->dwTimNo, pstInit->dwTimChnNo);
    
    return &s_DevMo;
}
#endif