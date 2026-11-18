/*******************************************************************************
文件名称：   pwm.c
文件目录：   $PROJ_DIR$/usr/Drive
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2013-08-25
程序用途:    pwm控制程序驱动

版权声明：   Copyright urit Co.,Ltd.All Right Reserved

日期：2019.1.3  胥强重写初稿，由于换电脑文件丢失

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_PWM)
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
    bool blTimActive;
    unsigned char byTimDev;

    struct __stChn{
        struct __stConfig{
            unsigned int *pwdPort;              // 时钟的位定义地址
            unsigned char byActiveLevel;
            bool blUsing;
        } stConfig;

        struct __stRun{
            unsigned int  dwStep;               // 需要运行的步数
            unsigned int  dwStpCnt;             // 运行步数计数

            bool blRunning;
            unsigned char bySpd;                // 百分速度
            unsigned char bySpdCnt;
        } stRun;
    } staChn[ CONFIG_PWM_DEV_NUM ];
}DevCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 用于定时器的核心驱动
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void KernelProg(unsigned int dwTimdev)
{
    struct __stChn *pstChn = DevCtr.staChn;
    unsigned int dwChnActive = 0;

    for(int dev = 0 ; dev < CONFIG_PWM_DEV_NUM; dev ++, pstChn ++) {
        if (pstChn->stConfig.blUsing == false) continue;
        if (pstChn->stRun.blRunning == false) continue;

        dwChnActive ++;
        // 控制接口
        if (pstChn->stRun.bySpdCnt == 0) {
            *pstChn->stConfig.pwdPort = (unsigned int)(pstChn->stConfig.byActiveLevel);
        }
        else if (pstChn->stRun.bySpdCnt >= pstChn->stRun.bySpd) {   /* AI-FIX H-02: 相位可能已越过新 bySpd */
            *pstChn->stConfig.pwdPort = (unsigned int)(1 - pstChn->stConfig.byActiveLevel);
        }

        pstChn->stRun.bySpdCnt ++;
        if(pstChn->stRun.bySpdCnt == 100) {
            pstChn->stRun.bySpdCnt = 0;
            pstChn->stRun.dwStpCnt ++;

            if(pstChn->stRun.dwStpCnt >= pstChn->stRun.dwStep) {
                *pstChn->stConfig.pwdPort = (unsigned int)(1 - pstChn->stConfig.byActiveLevel);
                pstChn->stRun.blRunning = false;
                dwChnActive --;
            }
        }
    }

    if(dwChnActive == 0) {
        DevTim->Stop(DevCtr.byTimDev);
        DevCtr.blTimActive = false;
    }
}

/*******************************************************************************
函数功能：
*   @ 启动pwm设备
参数描述：
*   @ unsigned int dwDev 设备号
*   @ unsigned char bySpd 速度
*   @ unsigned int dwTim 运行时间
返回值描述：
*   @ 设备的操作集合
说明：
*   @ 无
*******************************************************************************/
static int Write(unsigned int dwDev, unsigned char bySpd, unsigned int dwTim)
{
    if(dwDev >= CONFIG_PWM_DEV_NUM) return -1;

    if(DevCtr.staChn[dwDev].stConfig.blUsing == false) return -1;

    /* AI-FIX H-01 2026-11-14
       原实现: bySpd==0 时直接 return -1，不改变任何输出状态。
       危害  : 调用方 TaskTempModule.c:171-176 在 Write 返回后无条件把
               wPwm_Old 更新为当前 wPwm，于是当 wPwm 由非零变为 0 时：
               Write 空转 -> 输出保持上一拍的 bySpd -> wPwm_Old==wPwm 恒成立
               -> 后续不再调用 Write -> 加热锁死在上一拍占空比上，
               直到温度超过 wHeatTemp+50 才被 Stop() 切断(固化通道为 118C)。
       修正  : 0 速等价于关断，与 Stop() 行为一致。 */
    if(bySpd == 0)
    {
        DevCtr.staChn[dwDev].stRun.bySpd     = 0;
        DevCtr.staChn[dwDev].stRun.blRunning = false;
        *DevCtr.staChn[dwDev].stConfig.pwdPort =
            (unsigned int)(1 - DevCtr.staChn[dwDev].stConfig.byActiveLevel);
        return 0;
    }

    if(bySpd > 100) bySpd = 100;
    if(dwTim == 0) dwTim = (unsigned int)(-1);

    DevCtr.staChn[dwDev].stRun.bySpd = bySpd;
    /* AI-FIX H-02 2026-11-14
       原实现: 每次 Write 把 bySpdCnt 清 0，而 KernelProg 在 bySpdCnt==0 时
               强制输出 ON。温度环几乎每拍(120ms)都可能改写占空比，
               导致 PWM 周期(100ms)被反复从相位 0 重启，
               每个被截断的周期都白送了 1 个以上的 ON 时隙，实际占空比
               系统性高于设定值，且纹波被放大。
       修正  : 保留相位，只更新速度。配合 KernelProg 中的 >= 比较，
               改小 bySpd 时本周期的剩余时隙立即转为 OFF，不会多加热。 */
    /* DevCtr.staChn[dwDev].stRun.bySpdCnt = 0;  AI-FIX H-02 保留相位 */
    DevCtr.staChn[dwDev].stRun.dwStep = dwTim;
    DevCtr.staChn[dwDev].stRun.dwStpCnt = 0;
    DevCtr.staChn[dwDev].stRun.blRunning = true;

    if(DevCtr.blTimActive == false) {
        // 启动定时器
        DevCtr.blTimActive = true;
        DevTim->TimEvent(DevCtr.byTimDev, 1000, KernelProg, 0);
    }

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 关闭PS2设备
*参数描述：
*   @ unsigned int dwDev 设备号
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int Stop(unsigned int dwDev)
{
    if(dwDev >= CONFIG_PWM_DEV_NUM) return -1;

    DevCtr.staChn[dwDev].stRun.blRunning = false;
    *DevCtr.staChn[dwDev].stConfig.pwdPort = (unsigned int)(1 - DevCtr.staChn[dwDev].stConfig.byActiveLevel);
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 向pwm驱动添加设备
*参数描述：
*   @ unsigned int dwDev 设备号
*   @ unsigned int *pwPort 驱动地址
*   @ unsigned int dwLevel 驱动口的有限电平
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int AddDev(unsigned int dwDev, unsigned int *pwPort, unsigned int dwLevel)
{
    if(dwDev >= CONFIG_PWM_DEV_NUM) return -1;
    if(DevCtr.staChn[dwDev].stConfig.blUsing == true) return -1;

    DevCtr.staChn[dwDev].stConfig.pwdPort = pwPort;
    DevCtr.staChn[dwDev].stConfig.byActiveLevel = (dwLevel == 1)?1:0;
    DevCtr.staChn[dwDev].stConfig.blUsing = true;

    return 0;
}

static const DevPwmTdf s_Dev = {
    AddDev,
    Stop,
    Write
};

/*******************************************************************************
函数功能：
*   @ 初始化
参数描述：
*   @ 初始化参数
返回值描述：
*   @ 设备的操作集合
说明：
*   @ 无
*******************************************************************************/
const DevPwmTdf *PwmInit(DevPwmInitTdf *pstInit)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    memset(&DevCtr, 0, sizeof(DevCtr));

    DevRccGetClocksFreq(&stRccFreq);

    if(pstInit->byTimNO == 0 || pstInit->byTimNO == 7 ||\
       pstInit->byTimNO == 8 || pstInit->byTimNO == 9 || pstInit->byTimNO == 10) dwClk = stRccFreq.PCLK2;
    else dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000) - 1;
    //设置定时器，为1M的时钟
    DevTim->Config(pstInit->byTimNO, dwPre, 1000);
    DevCtr.byTimDev = pstInit->byTimNO;

	return &s_Dev;
}
#endif