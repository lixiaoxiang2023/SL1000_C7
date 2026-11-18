/*******************************************************************************
文件名称：    cat3626.c
目录：        $PROJ_DIR$/../code/dev
项目名称：    事业一部技术平台
创建者：      胥强
创建日期：    2017.04.05
程序用途：    用于mpr03x驱动

版权声明：
Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
20170405:
第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_MPR03X)
/*******************************************************************************
常量定义
*******************************************************************************/
#define REG_SR        0x00
#define REG_E0FD      0x02
#define REG_E1FD      0x04
#define REG_E2FD      0x06

#define REG_E0BV      0x1A
#define REG_E1BV      0x1B
#define REG_E2BV      0x1C

#define REG_MHD       0x26
#define REG_NHD       0x27
#define REG_NCL       0x28

#define REG_E0TTH     0x29
#define REG_E0RTH     0x2A
#define REG_E1TTH     0x2B
#define REG_E1RTH     0x2C
#define REG_E2TTH     0x2D
#define REG_E2RTH     0x2E

#define REG_AFEC      0x41
#define REG_FC        0x43
#define REG_EC        0x44

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static struct {
    unsigned char byOpReg;
    struct {
        union {
            __IO unsigned char SR;
            struct {
                __IO unsigned char E0S:1;
                __IO unsigned char E1S:1;
                __IO unsigned char E2S:1;
                __IO unsigned char res6_3:4;
                __IO unsigned char OCF:1;
            }BitSR;
        };

        __IO unsigned char MHD;
        __IO unsigned char NHD;
        __IO unsigned char NCL;

        __IO unsigned char E0TTH;
        __IO unsigned char E0RTH;

        __IO unsigned char E1TTH;
        __IO unsigned char E1RTH;

        __IO unsigned char E2TTH;
        __IO unsigned char E2RTH;

        union {
            __IO unsigned char AFEC;
            struct {
                __IO unsigned char CDC:6;
                __IO unsigned char FFI:2;
            }BitAFEC;
        };

        union {
            __IO unsigned char FC;
            struct {
                __IO unsigned char ESI:3;
                __IO unsigned char SFI:2;
                __IO unsigned char CDT:3;
            }BitFC;
        };

        union {
            __IO unsigned char EC;
            struct {
                __IO unsigned char EleEn:4;
                __IO unsigned char ModeSel:2;
                __IO unsigned char CalLock:1;
                __IO unsigned char res8:1;
            }BitEC;
        };
    }stReg;

    unsigned char byI2cNo;
    unsigned char byIntGpioNO;
    unsigned char byIntChn;
}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/
static const DevMpr03xTdf * SetMHD(unsigned char byMhd);
static const DevMpr03xTdf * SetNHD(unsigned char byNhd);
static const DevMpr03xTdf * SetNCL(unsigned char byNcl);
static const DevMpr03xTdf * SetE0TTH(unsigned char byE0tth);
static const DevMpr03xTdf * SetE0RTH(unsigned char byE0rth);
static const DevMpr03xTdf * SetE1TTH(unsigned char byE1tth);
static const DevMpr03xTdf * SetE1RTH(unsigned char byE1rth);
static const DevMpr03xTdf * SetE2TTH(unsigned char byE2tth);
static const DevMpr03xTdf * SetE2RTH(unsigned char byE2rth);
static const DevMpr03xTdf * SetAfecFFI(unsigned char byFfi);
static const DevMpr03xTdf * SetAfecCDC(unsigned char byCdc);
static const DevMpr03xTdf * SetFcCDT(unsigned char byCdt);
static const DevMpr03xTdf * SetFcSFI(unsigned char bySfi);
static const DevMpr03xTdf * SetFcESI(unsigned char byEsi);
static const DevMpr03xTdf * SetFcEC(unsigned char byEleEn);
static const DevMpr03xTdf * SetEcEleEn(unsigned char byEleEn);
static const DevMpr03xTdf * SetEcModeSel(unsigned char byModeSel);
static const DevMpr03xTdf * SetEcCalLock(unsigned char byCalLock);
static const DevMpr03xTdf * SetSrOcf(void);
static int SetExe(void);
static unsigned char GetBaseline(unsigned int dwChn);
static unsigned short int GetFilter(unsigned int dwChn);
static unsigned char GetSr(void);

const static DevMpr03xTdf s_Dev = {
    SetMHD,
    SetNHD,
    SetNCL,
    SetE0TTH,
    SetE0RTH,
    SetE1TTH,
    SetE1RTH,
    SetE2TTH,
    SetE2RTH,
    SetAfecFFI,
    SetAfecCDC,
    SetFcCDT,
    SetFcSFI,
    SetFcESI,
    SetFcEC,
    SetEcEleEn,
    SetEcModeSel,
    SetEcCalLock,
    SetSrOcf,
    SetExe,

    GetBaseline,
    GetFilter,
    GetSr,
};

/*******************************************************************************
函数功能：
*   @ 设置 Electrode Enable
参数描述：
*   @ unsigned char byEleEn: 使用模式
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetEcEleEn(unsigned char byEleEn)
{
    s_stCtr.byOpReg = REG_EC;
    s_stCtr.stReg.BitEC.EleEn = byEleEn;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Mode Select
参数描述：
*   @ unsigned char byModeSel: 使用模式
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetEcModeSel(unsigned char byModeSel)
{
    s_stCtr.byOpReg = REG_EC;
    s_stCtr.stReg.BitEC.ModeSel = byModeSel;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Calibration Lock
参数描述：
*   @ unsigned char byModeSel: 使用模式
*   @ 0 Enabled – In this state baseline calibration is enabled.
*   @ 1 Disabled – In this state baseline calibration is disabled.
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetEcCalLock(unsigned char byCalLock)
{
    s_stCtr.byOpReg = REG_EC;
    s_stCtr.stReg.BitEC.ModeSel = byCalLock;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Electrode Enable
参数描述：
*   @ unsigned char byEleEn: 使用模式
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetFcEC(unsigned char byEleEn)
{
    s_stCtr.byOpReg = REG_EC;
    s_stCtr.stReg.BitEC.EleEn = byEleEn;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Electrode Sample Interval
参数描述：
*   @ unsigned char byEsi: 采样间隔
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetFcESI(unsigned char byEsi)
{
    s_stCtr.byOpReg = REG_FC;
    s_stCtr.stReg.BitFC.ESI = byEsi;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Second Filter Iterations – The Second Filter Iterations field selects the number of
*   @ samples taken for the second level filter
参数描述：
*   @ unsigned char byFsi: 二级采样数
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetFcSFI(unsigned char bySfi)
{
    s_stCtr.byOpReg = REG_FC;
    s_stCtr.stReg.BitFC.SFI = bySfi;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Charge Discharge Time
参数描述：
*   @ unsigned char byCdt: 充电时间
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetFcCDT(unsigned char byCdt)
{
    s_stCtr.byOpReg = REG_FC;
    s_stCtr.stReg.BitFC.CDT = byCdt;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Charge Discharge Current
参数描述：
*   @ unsigned char byCdc: 充电电流
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetAfecCDC(unsigned char byCdc)
{
    s_stCtr.byOpReg = REG_AFEC;
    s_stCtr.stReg.BitAFEC.CDC = byCdc;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Charge Discharge Current
参数描述：
*   @ unsigned char byCdc: 充电电流
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetAfecFFI(unsigned char byFfi)
{
    s_stCtr.byOpReg = REG_AFEC;
    s_stCtr.stReg.BitAFEC.FFI = byFfi;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Release Threshold
参数描述：
*   @ unsigned char byE2rth: 按键释放阈值
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetE2RTH(unsigned char byE2rth)
{
    s_stCtr.byOpReg = REG_E2RTH;
    s_stCtr.stReg.E2RTH = byE2rth;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Touch Threshold
参数描述：
*   @ unsigned char byE2tth: 按键触发阈值
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetE2TTH(unsigned char byE2tth)
{
    s_stCtr.byOpReg = REG_E2TTH;
    s_stCtr.stReg.E2TTH = byE2tth;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Release Threshold
参数描述：
*   @ unsigned char byE1rth: 按键释放阈值
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetE1RTH(unsigned char byE1rth)
{
    s_stCtr.byOpReg = REG_E1RTH;
    s_stCtr.stReg.E1RTH = byE1rth;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Touch Threshold
参数描述：
*   @ unsigned char byE2tth: 按键触发阈值
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetE1TTH(unsigned char byE1tth)
{
    s_stCtr.byOpReg = REG_E1TTH;
    s_stCtr.stReg.E1TTH = byE1tth;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Release Threshold
参数描述：
*   @ unsigned char byE0rth: 按键释放阈值
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetE0RTH(unsigned char byE0rth)
{
    s_stCtr.byOpReg = REG_E0RTH;
    s_stCtr.stReg.E0RTH = byE0rth;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Touch Threshold
参数描述：
*   @ unsigned char byE2tth: 按键触发阈值
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetE0TTH(unsigned char byE0tth)
{
    s_stCtr.byOpReg = REG_E0TTH;
    s_stCtr.stReg.E0TTH = byE0tth;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Noise Count Limit Register
参数描述：
*   @ unsigned char byNcl:
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetNCL(unsigned char byNcl)
{
    s_stCtr.byOpReg = REG_NCL;
    s_stCtr.stReg.NCL = byNcl;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Noise Count Limit Register
参数描述：
*   @ unsigned char byNhd:
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetNHD(unsigned char byNhd)
{
    s_stCtr.byOpReg = REG_NHD;
    s_stCtr.stReg.NHD = byNhd;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 Noise Count Limit Register
参数描述：
*   @ unsigned char byMhd:
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetMHD(unsigned char byMhd)
{
    s_stCtr.byOpReg = REG_MHD;
    s_stCtr.stReg.MHD = byMhd;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置 SR 的电流过载位
参数描述：
*   @ unsigned char byMhd:
返回值描述：
*   @ 操作集合
说明：
*******************************************************************************/
static const DevMpr03xTdf * SetSrOcf(void)
{
    s_stCtr.byOpReg = REG_SR;
    s_stCtr.stReg.BitSR.OCF = 1;

    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 设置数据
参数描述：
*   @ 无
返回值描述：
*   @  0 成功
*   @ -1 失败
说明：
*******************************************************************************/
static int SetExe(void)
{
    unsigned char abySet[3] = {0, 0, 0};

    abySet[0] = CONFIG_MPR03X_ADDR;
    abySet[1] = s_stCtr.byOpReg;

    switch(s_stCtr.byOpReg) {
    case REG_SR    : abySet[2] = s_stCtr.stReg.SR;      break;
    case REG_MHD   : abySet[2] = s_stCtr.stReg.MHD;     break;
    case REG_NHD   : abySet[2] = s_stCtr.stReg.NHD;     break;
    case REG_NCL   : abySet[2] = s_stCtr.stReg.NCL;     break;
    case REG_E0TTH : abySet[2] = s_stCtr.stReg.E0TTH;   break;
    case REG_E0RTH : abySet[2] = s_stCtr.stReg.E0RTH;   break;
    case REG_E1TTH : abySet[2] = s_stCtr.stReg.E1TTH;   break;
    case REG_E1RTH : abySet[2] = s_stCtr.stReg.E1RTH;   break;
    case REG_E2TTH : abySet[2] = s_stCtr.stReg.E2TTH;   break;
    case REG_E2RTH : abySet[2] = s_stCtr.stReg.E2RTH;   break;
    case REG_AFEC  : abySet[2] = s_stCtr.stReg.AFEC;    break;
    case REG_FC    : abySet[2] = s_stCtr.stReg.FC;      break;
    case REG_EC    : abySet[2] = s_stCtr.stReg.EC;      break;
    default    : return -1;
    }

    return DevI2c->Trs(s_stCtr.byI2cNo, abySet, NULL, 3);
}

/*******************************************************************************
函数功能：
*   @ 读取寄存器
参数描述：
*   @ unsigned char byReg 寄存器
*   @ unsigned int dwLen 需要的长度，不大于2
返回值描述：
*   @ 3626的操作方法集合
说明：
*******************************************************************************/
static int ReadReg(unsigned char byReg, unsigned int dwLen)
{
    unsigned char abySet[2];
    unsigned char byaGet[4];

    if(dwLen > 2) return -1;

    abySet[0] = CONFIG_MPR03X_ADDR;
    abySet[1] = byReg;
    memset(byaGet, 0, 4);

    DevI2c->Read(s_stCtr.byI2cNo, abySet, 2, byaGet, dwLen);

    if(dwLen == 1) return byaGet[0];

    return (byaGet[0] | (byaGet[1] << 8));
}

/*******************************************************************************
函数功能：
*   @ 检查是否过流
参数描述：
*   @ 无
返回值描述：
*   @ true :发生过流
*   @ false:未发生过流
说明：
*******************************************************************************/
static unsigned char GetSr(void)
{
    return ReadReg(REG_SR, 1);
}

/*******************************************************************************
函数功能：
*   @  取得Filtered
参数描述：
*   @ 无
返回值描述：
*   @ true :发生过流
*   @ false:未发生过流
说明：
*******************************************************************************/
static unsigned short int GetFilter(unsigned int dwChn)
{
    int ExFD = 0;
    unsigned char byReg = 0;

    if(dwChn == 0) byReg = REG_E0FD;
    else if(dwChn == 1) byReg = REG_E1FD;
    else byReg = REG_E2FD;

    ExFD = ReadReg(byReg, 2);
    if(ExFD == -1) return -1;

    return (unsigned short int)ExFD;
}

/*******************************************************************************
函数功能：
*   @  GetBaseline
参数描述：
*   @ 无
返回值描述：
*   @ true :发生过流
*   @ false:未发生过流
说明：
*******************************************************************************/
static unsigned char GetBaseline(unsigned int dwChn)
{
    int ExFD = 0;
    unsigned char byReg = 0;

    if(dwChn == 0) byReg = REG_E0BV;
    else if(dwChn == 1) byReg = REG_E1BV;
    else byReg = REG_E2BV;

    ExFD = ReadReg(byReg, 1);
    if(ExFD == -1) return -1;

    return (unsigned char)ExFD;
}

/*******************************************************************************
函数功能：
*   @ 初始化CAT3626
参数描述：
*   @ 无
返回值描述：
*   @ 3626的操作方法集合
说明：
*******************************************************************************/
const DevMpr03xTdf * Mpr03xInit( Mpr03xInitTdf *pstInit )
{
    memset(&s_stCtr, 0, sizeof(s_stCtr));

    s_stCtr.byI2cNo     = pstInit->byI2cNo;
    s_stCtr.byIntGpioNO = pstInit->byIntGpioNO;
    s_stCtr.byIntChn    = pstInit->byIntChn;
    DevI2c->Config(pstInit->byI2cNo, 400000, true);

    return &s_Dev;
}
#endif