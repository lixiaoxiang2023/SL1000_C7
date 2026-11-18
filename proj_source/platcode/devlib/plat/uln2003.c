/*******************************************************************************
文件名称：   uln2003.c
文件目录：   $PROJ_DIR$/../dev/src
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.14
程序用途：   用于Uln2003电机驱动程序

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_ULN2003)
/*******************************************************************************
 Constant and Macro Definition常量以及宏定义
 ******************************************************************************/
static const uint8_t c_byaTabCW[4]  = {ULN2003_4_CW1,ULN2003_4_CW2,ULN2003_4_CW3,ULN2003_4_CW4};  // 逆时针
static const uint8_t c_byaTabCCW[4] = {ULN2003_4_CW4,ULN2003_4_CW3,ULN2003_4_CW2,ULN2003_4_CW1};  // 顺时针

#if(CONFIG_ULN2003_DIV)
static const uint8_t c_byaHTabCW[8] = {ULN2003_8_CW1,ULN2003_8_CW2,ULN2003_8_CW3,ULN2003_8_CW4,\
	                                     ULN2003_8_CW5,ULN2003_8_CW6,ULN2003_8_CW7,ULN2003_8_CW8};
static const uint8_t c_byaHTabCCW[8]= {ULN2003_8_CW8,ULN2003_8_CW7,ULN2003_8_CW6,ULN2003_8_CW5,\
	                                     ULN2003_8_CW4,ULN2003_8_CW3,ULN2003_8_CW2,ULN2003_8_CW1};
#if(CONFIG_ULN2003_NOT_DIV_MO)
static const uint8_t c_byaHTabCW4[8] = {ULN2003_4_CW1,ULN2003_4_CW1,ULN2003_4_CW2,ULN2003_4_CW2,\
	                                     ULN2003_4_CW3,ULN2003_4_CW3,ULN2003_4_CW4,ULN2003_4_CW4};
static const uint8_t c_byaHTabCCW4[8]= {ULN2003_4_CW4,ULN2003_4_CW4,ULN2003_4_CW3,ULN2003_4_CW3,\
	                                     ULN2003_4_CW2,ULN2003_4_CW2,ULN2003_4_CW1,ULN2003_4_CW1};
#endif

#endif

#ifdef USE_SIN_UP_SPEED1
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
	unsigned int dwSteps;     //电机需要运行的步数
    unsigned int dwMaxSteps;  //电机需要运行的最大步数步数，在光耦停止模式有效
    unsigned int dwRestSteps; //电机运行在发现光耦后继续运行此步数后停止
    unsigned int dwStart;     //电机起始频率
    unsigned int dwMax;       //电机最大频率
    unsigned int dwSpdUp;     //电机每一次加速内需要运行的步数

    union {
        unsigned char byCtr;
        struct {
            unsigned char byDir:1;          //电机运行的方向
            unsigned char byStopMode:2;     //电机停止模式，是否找光耦
            unsigned char byIsReNow:1;      //是否立即返回
            unsigned char byIsHalf:1;       //是否使用小电流运行
#if(CONFIG_ULN2003_DIV)
            unsigned char byDiv:2;          //电机细分
            unsigned char res:1;
#else
            unsigned char res:3;
#endif
        }BitCtr;
    };

    void (*pfRunFunc)(void);//电机运行过程中的用户应用接口
}DevUln2003CtrTdf;

static struct {
    DevUln2003InitTdf    stInit;                //电机初始化参数
    DevUln2003CtrTdf     stCtr;                 //电机控制参数

    uint16_t dwOpTab[40];                       //电机加速表
    unsigned int dwRunStep;                     //电机运行步数计数器
    unsigned int dwUpPo;                        //加速终止点
    unsigned int dwDmPo;                        //减速开始点
    uint8_t byOpStateCnt;                       //电机步进加速状态计数
    uint8_t byOpStateCntBkp;                    //电机步进加速状态计数备份
    bool blNeedSpeedUp;                         //是否需要加速處理
    bool blSpeedOping;                          //电机加速状态
    uint8_t byOPRestCnt;

    const uint8_t *pbyTab;
    bool blIsHalfCtr;
    bool blIsBusy;
}s_DevMo[MAX_MO1];

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
    DevUln2003CtrTdf *pstPare;
    pstPare = &s_DevMo[dwHwnd].stCtr;

    for (int i = 0; i < 40; i++) {
        //注意：定时器的基准频率时间为1uS,计算时间周期
        s_DevMo[dwHwnd].dwOpTab[i] = (unsigned short int) ((double) (1 / (double) pstPare->dwMax) * 1000 * 1000);
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
        s_DevMo[dwHwnd].dwOpTab[fX - 1] = (unsigned short int)(fY + 0.5);

        fY = pstPare->dwMax - fY_temp;     //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_DevMo[dwHwnd].dwOpTab[39 - fX] = (unsigned short int)(fY + 0.5);
    }
#else
    double fk = (double) dwDiffSpeed / (double) (20 * 20);
    for (int fX = 1; fX < 20; fX++) {
        fY_temp = fk * fX * fX;                 //先计算加速值

        fY = pstPare->dwStart + fY_temp;   //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_DevMo[dwHwnd].dwOpTab[fX - 1] = (unsigned short int)(fY + 0.5);

        fY = pstPare->dwMax - fY_temp;     //加速表前后对称计算
        fY = (1 / fY) * 1000 * 1000;
        s_DevMo[dwHwnd].dwOpTab[39 - fX] = (unsigned short int)(fY + 0.5);
    }
#endif

    fY = pstPare->dwStart + dwDiffSpeed;       //计算中间值
    fY = (1 / fY) * 1000 * 1000;
    s_DevMo[dwHwnd].dwOpTab[19] = (unsigned short int)(fY + 0.5);

    fY = pstPare->dwMax;                       //计算最大值
    fY = (1 / fY) * 1000 * 1000;
    s_DevMo[dwHwnd].dwOpTab[39] = (unsigned short int)(fY + 0.5);

    s_DevMo[dwHwnd].dwUpPo = pstPare->dwSpdUp * 40;
    s_DevMo[dwHwnd].dwDmPo = pstPare->dwSteps - s_DevMo[dwHwnd].dwUpPo;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 根据参数电机完成一步的运行，此函数可以通过外部单步触发
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @  0：电机启动运行正常
*   @ -1：电机启动运行失败，即电机正在运行中
说明：
*   @ 本函数可以在外部独立触发使用，特别是在半自动数据检测方面使用。
*******************************************************************************/
static int GoStep(unsigned int dwHwnd)
{
    uint16_t wIO;
    uint8_t byCode;
#if(CONFIG_ULN2003_DIV)
    uint8_t byDiv;
#endif
    //运行步数增加
    if(dwHwnd >= MAX_MO1) return -1;
    s_DevMo[dwHwnd].dwRunStep ++;

#if(CONFIG_ULN2003_DIV)
    byDiv = 4;
    if(s_DevMo[dwHwnd].stCtr.BitCtr.byDiv) byDiv = 8;
    byCode = s_DevMo[dwHwnd].pbyTab[s_DevMo[dwHwnd].dwRunStep % byDiv];
#else
    byCode = s_DevMo[dwHwnd].pbyTab[s_DevMo[dwHwnd].dwRunStep % 4];
#endif

    wIO = DevIo->ReadGPIO(s_DevMo[dwHwnd].stInit.byGpioNo);
    wIO = (wIO & ~(((uint16_t) 0x0f) << s_DevMo[dwHwnd].stInit.byGpioAlian)) | ((uint16_t)byCode << s_DevMo[dwHwnd].stInit.byGpioAlian);
    DevIo->WriteGPIO(s_DevMo[dwHwnd].stInit.byGpioNo, wIO);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动电机
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 本函数将注册一个定时器，并产生一个周期事件的定时器中断，用于驱动电机运行。
*******************************************************************************/
static void Start(unsigned int dwHwnd)
{
    s_DevMo[dwHwnd].blIsBusy = true;
    GoStep(dwHwnd);
    DevTim->TimEvent(s_DevMo[dwHwnd].stInit.byTimNO, s_DevMo[dwHwnd].dwOpTab[0], MotorKernelProg, dwHwnd);
}

/*******************************************************************************
函数功能：
*   @ 停止电机
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 此函数将停止定时器，同时清除电机端口电平，取消电机锁定。
*******************************************************************************/
static int Stop(unsigned int dwHwnd)
{
    uint16_t wIO;
    if(dwHwnd >= MAX_MO1) return -1;

    wIO = DevIo->ReadGPIO(s_DevMo[dwHwnd].stInit.byGpioNo);
    wIO = wIO & ~(((uint16_t) 0x0f) << s_DevMo[dwHwnd].stInit.byGpioAlian);
    DevIo->WriteGPIO(s_DevMo[dwHwnd].stInit.byGpioNo, wIO);

    s_DevMo[dwHwnd].blIsBusy = FALSE;
    DevTim->Stop(s_DevMo[dwHwnd].stInit.byTimNO);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 判断电机是否在光耦处。
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
    return (*(s_DevMo[dwHwnd].stInit.pwdOpGpio) == s_DevMo[dwHwnd].stInit.byAtOptLv)? true:false;
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
static int SetOp(unsigned int dwHwnd, unsigned int *pwdOpGpio, unsigned char byLevel)
{
    if(dwHwnd >= MAX_MO1) return -1;
    if( !s_DevMo[dwHwnd].stInit.pwdOpGpio ) return -1;

    s_DevMo[dwHwnd].stInit.pwdOpGpio = pwdOpGpio;
    s_DevMo[dwHwnd].stInit.byAtOptLv = byLevel;

    return 0;
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
    if(dwHwnd >= MAX_MO1) return -1;
    return s_DevMo[dwHwnd].dwRunStep;
}

#if(CONFIG_ULN2003_NOT_DIV_MO)
/*******************************************************************************
函数功能：
*   @ 用于判断是否为要按四拍运行的电机
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 当CONFIG_ULN2003_DIV有效时，又有电机需按四拍运行，则启用此宏定义
*******************************************************************************/
static bool IsNotDivMoNum(unsigned int dwHwnd)
{
    uint8_t byMoNotDivCode = CONFIG_ULN2003_NOT_DIV_MO;
    uint8_t byMo = 0;
    
    if((byMoNotDivCode & 0x80 )== 0) return false;
    if(dwHwnd >= MAX_MO1 || dwHwnd >= 7) return false;
    
    for(uint8_t byCnt = 0; byCnt < MAX_MO1; byCnt ++)
    {
        byMo = byMoNotDivCode & (0x01 << byCnt);
        
        if((byMo != 0) && (byCnt == dwHwnd)) {
            return true;
        }
    }
    
    return false;
}
#endif

/*******************************************************************************
函数功能：
*   @ 使能。
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
*   @ unsigned char byCtr：外部触发控制，包括方向与细分
返回值描述：
*   @ 无
说明：
*   @ 在不使用定时器驱动电机时，需要调用此函数进行初始化，之后可以通过周期调用GoStep，驱动电机运行
*******************************************************************************/
static int ExternOp(unsigned int dwHwnd, unsigned char byCtr)
{
    s_DevMo[dwHwnd].dwRunStep = 0;

    s_DevMo[dwHwnd].blIsBusy = true;
    s_DevMo[dwHwnd].stCtr.byCtr = byCtr;
    if(s_DevMo[dwHwnd].stCtr.BitCtr.byDir == 1) {
#if(CONFIG_ULN2003_DIV)
        if(s_DevMo[dwHwnd].stCtr.BitCtr.byDiv == 1) {
    #if(CONFIG_ULN2003_NOT_DIV_MO)
            if(IsNotDivMoNum(dwHwnd)) s_DevMo[dwHwnd].pbyTab = c_byaHTabCCW4;
            else s_DevMo[dwHwnd].pbyTab = c_byaHTabCCW;
    #else
            s_DevMo[dwHwnd].pbyTab = c_byaHTabCCW;
    #endif
        }
        else s_DevMo[dwHwnd].pbyTab = c_byaTabCCW;
#else
        s_DevMo[dwHwnd].pbyTab = c_byaTabCCW;
#endif
    }
    else {
#if(CONFIG_ULN2003_DIV)
        if(s_DevMo[dwHwnd].stCtr.BitCtr.byDiv == 1) {
    #if(CONFIG_ULN2003_NOT_DIV_MO)
            if(IsNotDivMoNum(dwHwnd)) s_DevMo[dwHwnd].pbyTab = c_byaHTabCW4;
            else s_DevMo[dwHwnd].pbyTab = c_byaHTabCW;
    #else
            s_DevMo[dwHwnd].pbyTab = c_byaHTabCW;
    #endif
        }
        else s_DevMo[dwHwnd].pbyTab = c_byaTabCW;
#else
        s_DevMo[dwHwnd].pbyTab = c_byaTabCW;
#endif
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 外部驱动同时条件。
参数描述：
*   @ unsigned int dwHwnd：句柄，电机参数
返回值描述：
*   @ 无
说明：
*   @ 此函数将清除相关参数，并将电机的控制端口清除，解除电机锁定
*******************************************************************************/
static int ExternEnd(unsigned int dwHwnd)
{
    uint16_t wIO;
    if(dwHwnd >= MAX_MO1) return -1;

    wIO = DevIo->ReadGPIO(s_DevMo[dwHwnd].stInit.byGpioNo);
    wIO = wIO & ~(((uint16_t) 0x0f) << s_DevMo[dwHwnd].stInit.byGpioAlian);
    DevIo->WriteGPIO(s_DevMo[dwHwnd].stInit.byGpioNo, wIO);

    s_DevMo[dwHwnd].blIsBusy = false;

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
*   @     #define M_CW            0x01 0b00000001： 开启正转方式
*   @     #define M_SEN           0x02 0b00000010： 开启光耦运行方式
*   @     #define M_RE_NOW        0x04 0b00000100： 开启立即返回
*   @     #define M_HALF          0x08 0b00001000： 开启半流控制
*   @     #define M_DIV0          0x00 0b00000000： 不细分
*   @     #define M_DIV2          0x10 0b00010000： 2细分
*   @     #define M_DIV4          0x20 0b00100000： 4细分
*   @     #define M_DIV8          0x30 0b00110000： 8细分
返回值描述：
*   @  0：电机启动运行正常
*   @ -1：电机启动运行失败，即电机正在运行中
说明：
*   @  《使用步数停止模式》
*   @  需要设定dwSteps，此时驱动程序将自动使用抛物线进行加速，总运行步数为dwSteps
*   @  需要注意的是，1）当dwSteps<dwSpdUp*60时，驱动程序不能启动加速
*   @  此时电机将会按照dwSpeedMax设定的速度运行。2）当dwSteps》dwStpPerSpdUp*80时
*   @  驱动程序将会自动计算从dwSpeedStart到dwSpeedMax的加速曲线，并按照曲线运行。
*   @  《使用传感器光耦停止模式》
*   @  驱动程序将会使用dwSteps计算加速曲线，其计算方式与步数停止模式相同，
*   @  需要注意的是，1）dwMaxSteps为电机运行的最大步数，即当在dwMaxSteps步数内未找到光耦停止信号，
*   @  电机也会停止运行，2）电机加减速过程为dwSteps之内，如果电机在完成加减速之后仍然没有找到光耦
*   @  停止信号，电机将会按照dwSpeedStart速度完成剩下的运行步数。3）在此模式中只要检测到光耦停止信号，
*   @  电机将会紧急停止。
*******************************************************************************/
static int Write(unsigned int dwHwnd, unsigned int dwSteps, unsigned int dwMaxRestSteps, unsigned int dwStart, unsigned int dwMax, unsigned int dwSpdUp,
                    void (*pfRunFunc)(void), unsigned char byCtr)
{
    s_DevMo[dwHwnd].stCtr.byCtr = byCtr;
    s_DevMo[dwHwnd].stCtr.dwSteps = dwSteps;
    s_DevMo[dwHwnd].stCtr.dwMaxSteps = dwMaxRestSteps;
    s_DevMo[dwHwnd].stCtr.dwStart = dwStart;
    s_DevMo[dwHwnd].stCtr.dwMax = dwMax;
    s_DevMo[dwHwnd].stCtr.dwSpdUp = dwSpdUp;
    s_DevMo[dwHwnd].stCtr.pfRunFunc = pfRunFunc;

    if(s_DevMo[dwHwnd].stCtr.BitCtr.byStopMode != 0) {
        if (*(s_DevMo[dwHwnd].stInit.pwdOpGpio) == s_DevMo[dwHwnd].stInit.byAtOptLv) return 1;//光耦管脚电平等于触发电平，只有管脚高，检测低才能通过
        if(!s_DevMo[dwHwnd].stInit.pwdOpGpio) return -1;

        if(s_DevMo[dwHwnd].stCtr.BitCtr.byStopMode == 2) {   //停止模式为2 传步数值
            s_DevMo[dwHwnd].stCtr.dwMaxSteps = dwSteps;
            s_DevMo[dwHwnd].stCtr.dwRestSteps = dwMaxRestSteps;
        }
    }

    if (SpeedUpTab(dwHwnd) == 0) s_DevMo[dwHwnd].blNeedSpeedUp = TRUE;  //计算是否需要加速
    else s_DevMo[dwHwnd].blNeedSpeedUp = FALSE;

    //方向
    if(s_DevMo[dwHwnd].stCtr.BitCtr.byDir == 1) {
#if(CONFIG_ULN2003_DIV)
        if(s_DevMo[dwHwnd].stCtr.BitCtr.byDiv == 1) {
    #if(CONFIG_ULN2003_NOT_DIV_MO)
            if(IsNotDivMoNum(dwHwnd)) s_DevMo[dwHwnd].pbyTab = c_byaHTabCCW4;
            else s_DevMo[dwHwnd].pbyTab = c_byaHTabCCW;
    #else
            s_DevMo[dwHwnd].pbyTab = c_byaHTabCCW;
    #endif
        }
        else s_DevMo[dwHwnd].pbyTab = c_byaTabCCW;
#else
        s_DevMo[dwHwnd].pbyTab = c_byaTabCCW;
#endif
    }
    else {
#if(CONFIG_ULN2003_DIV)
        if(s_DevMo[dwHwnd].stCtr.BitCtr.byDiv == 1) {
    #if(CONFIG_ULN2003_NOT_DIV_MO)
            if(IsNotDivMoNum(dwHwnd)) s_DevMo[dwHwnd].pbyTab = c_byaHTabCW4;
            else s_DevMo[dwHwnd].pbyTab = c_byaHTabCW;
    #else
            s_DevMo[dwHwnd].pbyTab = c_byaHTabCW;
    #endif
        }
        else s_DevMo[dwHwnd].pbyTab = c_byaTabCW;
#else
        s_DevMo[dwHwnd].pbyTab = c_byaTabCW;
#endif
    }

    s_DevMo[dwHwnd].dwRunStep = 0;
    s_DevMo[dwHwnd].byOpStateCnt = 0;
    s_DevMo[dwHwnd].byOpStateCntBkp = 0;
    s_DevMo[dwHwnd].byOPRestCnt = 0;
    Start(dwHwnd);

    if (s_DevMo[dwHwnd].stCtr.BitCtr.byIsReNow != 1) {
        while (1) {
            if (s_DevMo[dwHwnd].blIsBusy == FALSE) return 0;
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
static bool IsBusy(unsigned int dwHwnd)
{
    return s_DevMo[dwHwnd].blIsBusy;
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
    GoStep(dwHwnd);

    s_DevMo[dwHwnd].blSpeedOping = TRUE;
    if ((s_DevMo[dwHwnd].dwRunStep < s_DevMo[dwHwnd].dwUpPo) && (s_DevMo[dwHwnd].blNeedSpeedUp == TRUE)) {
        if (s_DevMo[dwHwnd].dwRunStep % s_DevMo[dwHwnd].stCtr.dwSpdUp == 0) s_DevMo[dwHwnd].byOpStateCnt ++;
    }
    else if ((s_DevMo[dwHwnd].dwRunStep > s_DevMo[dwHwnd].dwDmPo) && (s_DevMo[dwHwnd].blNeedSpeedUp == TRUE)) {
        if (((s_DevMo[dwHwnd].stCtr.dwSteps - s_DevMo[dwHwnd].dwRunStep)\
             % s_DevMo[dwHwnd].stCtr.dwSpdUp == 0) && (s_DevMo[dwHwnd].byOpStateCnt != 0))
        {
            s_DevMo[dwHwnd].byOpStateCnt --;
        }
        else{
            s_DevMo[dwHwnd].blSpeedOping = FALSE;
        }
    }
    else {
        s_DevMo[dwHwnd].blSpeedOping = FALSE;
    }

    //更新加速表
    if (s_DevMo[dwHwnd].blSpeedOping == TRUE && (s_DevMo[dwHwnd].byOpStateCntBkp != s_DevMo[dwHwnd].byOpStateCnt))
    {
        s_DevMo[dwHwnd].byOpStateCntBkp = s_DevMo[dwHwnd].byOpStateCnt;
        DevTim->SetArr(s_DevMo[dwHwnd].stInit.byTimNO, s_DevMo[dwHwnd].dwOpTab[s_DevMo[dwHwnd].byOpStateCnt]);
    }

    //检查光耦,如果遇见光耦则停止
    if (s_DevMo[dwHwnd].stCtr.BitCtr.byStopMode != 0) {
        if (*(s_DevMo[dwHwnd].stInit.pwdOpGpio) == s_DevMo[dwHwnd].stInit.byAtOptLv) {
            s_DevMo[dwHwnd].byOPRestCnt ++;
            if (s_DevMo[dwHwnd].byOPRestCnt == MO_ULN2003_OP_CONFIRM_STEPS) {
                if(s_DevMo[dwHwnd].stCtr.BitCtr.byStopMode == 1) s_DevMo[dwHwnd].dwRunStep = s_DevMo[dwHwnd].stCtr.dwMaxSteps;
                else {
                    //发现光耦后继续运行设定的步数
                    if(s_DevMo[dwHwnd].dwRunStep < s_DevMo[dwHwnd].dwDmPo && s_DevMo[dwHwnd].dwRunStep > s_DevMo[dwHwnd].dwUpPo) {
                        signed int dwStepDiff = (signed int)s_DevMo[dwHwnd].dwDmPo - (signed int)s_DevMo[dwHwnd].dwRunStep;
                        dwStepDiff = dwStepDiff - (signed int)s_DevMo[dwHwnd].stCtr.dwRestSteps;
                        s_DevMo[dwHwnd].dwRunStep = (signed int)s_DevMo[dwHwnd].dwRunStep + dwStepDiff;
                    }
                }
            }
        }
    }
    else {
        if(s_DevMo[dwHwnd].dwRunStep == s_DevMo[dwHwnd].stCtr.dwSteps) {//按步数模式走完相应的步数，现在停止
            s_DevMo[dwHwnd].dwRunStep = s_DevMo[dwHwnd].stCtr.dwMaxSteps;
        }
    }

    if (s_DevMo[dwHwnd].dwRunStep == s_DevMo[dwHwnd].stCtr.dwMaxSteps) {
        Stop(dwHwnd);
    }

    if (s_DevMo[dwHwnd].stCtr.pfRunFunc != NULL) s_DevMo[dwHwnd].stCtr.pfRunFunc();
}

static const DevUln2003Tdf s_Dev = {
    GoStep,
    Stop,
    OnSensor,
    SetOp,
    ExternOp,
    ExternEnd,
    Write,
    IsBusy,
    GetSteps,
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
const DevUln2003Tdf *Uln2003Init(DevUln2003InitTdf* pstInit)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    DevRccGetClocksFreq(&stRccFreq);

    if(pstInit->byTimNO == 0 || pstInit->byTimNO == 7) dwClk = stRccFreq.PCLK2;
    else dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000);
    dwPre = dwPre*2-1;
    s_DevMo[pstInit->byHwnd].stInit = *pstInit;

    //设置定时器，1M時鐘，0的预装值
    DevTim->Config(pstInit->byTimNO, dwPre, 0);

    return &s_Dev;
}
#endif