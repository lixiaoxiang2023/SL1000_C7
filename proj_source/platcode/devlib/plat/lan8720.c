/*******************************************************************************
文件名称：   lan8720.c
文件目录：   $PROJ_DIR$/../code/plat
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2018.08.28
程序用途：   用于phy lan8720 驱动

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

#if(CONFIG_LAN8720)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
#define  PHY_LAN8720_ADDR           0

#define  CR                         0
#define  SR                         1
#define  IDL                        2
#define  IDH                        3
#define  ANA                        4
#define  ANLPA                      5
#define  ANE                        6
#define  SIREV                      16
#define  MCS                        17
#define  SMODE                      18
#define  ERRCNT                     26
#define  SCSR                       27
#define  SITC                       28
#define  INTSR                      28
#define  INTMASK                    30
#define  PHYSCR                     31

#define  PHY_TIME_OFF               0xffff

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    union unCR{
        __IO short unsigned int wCR;
        struct {
            __IO short unsigned int Res6_0:7;
            __IO short unsigned int CT:1;       /* Collision Test  */
            __IO short unsigned int DM:1;       /* Duplex Mode */
            __IO short unsigned int RST_AN:1;   /* Restart AutoNegotiate */
            __IO short unsigned int ISOLATE:1;  /* Isolate */
            __IO short unsigned int PDM:1;      /* Power Down  */
            __IO short unsigned int AN_EN:1;    /* AutoNegotiation Enable*/
            __IO short unsigned int SPEED:1;    /*Speed Select*/
            __IO short unsigned int LOOPBACK:1; /* Loopback */
            __IO short unsigned int RST:1;      /*Reset*/
        } BitCR;
    };

    union unSR {
        __IO short unsigned int wSR;
        struct {
            __IO short unsigned int EC:1;       /* Extended Capabilities */
            __IO short unsigned int JD:1;       /* Jabber Detect */
            __IO short unsigned int LINK:1;     /* Link Status */
            __IO short unsigned int AUNA:1;      /* Auto-Negotiate Ability */
            __IO short unsigned int RF:1;       /* Remote Fault */
            __IO short unsigned int ANC:1;      /* Auto-Negotiate Complete */
            __IO short unsigned int Res10_6:5;  /* AutoNegotiation Enable*/
            __IO short unsigned int HD_10T:1;   /* 10Base-T Half Duplex */
            __IO short unsigned int FD_10T:1;   /* 10Base-T Full Duplex */
            __IO short unsigned int HD_100T:1;  /* 100Base-TX Half Duplex */
            __IO short unsigned int FD_100T:1;  /* 100Base-TX Full Duplex */
            __IO short unsigned int T4_100T:1;  /* 100Base-T4 */
        } BitSR;
    };

    __IO short unsigned int wIDL;
    union unIDH {
        __IO short unsigned int wIDH;
        struct {
            __IO short unsigned int REV:4;      /* Revision Number */
            __IO short unsigned int MODE:6;     /* Model Number */
            __IO short unsigned int ID:6;       /* PHY ID Number */
        } BitIDH;
    } ;

    // Auto Negotiation Advertisement
    union unANA{
        __IO short unsigned int wANA;
        struct {
            __IO short unsigned int SF:5;       /* Selector Field  */
            __IO short unsigned int T10B:1;     /* 10Base-T */
            __IO short unsigned int FD_T10B:1;  /* 10Base-T Full Duplex */
            __IO short unsigned int T100B:1;    /* 100Base-TX */
            __IO short unsigned int FD_T100B:1; /* 100Base-TX Full Duplex */
            __IO short unsigned int T4_100B:1;  /* 100Base-T4 */
            __IO short unsigned int PAUSE:2;    /* Pause Operation */
            __IO short unsigned int Res12:1;    /* Reserved */
            __IO short unsigned int RF:1;       /* Remote Fault */
            __IO short unsigned int Res14:1;    /* Reserved */
            __IO short unsigned int NP:1;       /* Next Page */
        } BitANA;
    } ;

    // Auto Negotiation Link Partner Ability
    union unANLPA{
        __IO short unsigned int wANLPA;
        struct {
            __IO short unsigned int SF:5;       /* Selector Field  */
            __IO short unsigned int T10B:1;     /* 10Base-T */
            __IO short unsigned int FD_T10B:1;  /* 10Base-T Full Duplex */
            __IO short unsigned int T100B:1;    /* 100Base-TX */
            __IO short unsigned int FD_T100B:1; /* 100Base-TX Full Duplex */
            __IO short unsigned int T4_100B:1;  /* 100Base-T4 */
            __IO short unsigned int Res12_11:2; /* Reserved */
            __IO short unsigned int RF:1;       /* Remote Fault */
            __IO short unsigned int ACK:1;      /* Acknowledge */
            __IO short unsigned int NP:1;       /* Next Page */
        } BitANLPA;
    } ;

    // Auto Negotiation Expansion
    union unANE{
        __IO short unsigned int wANE;
        struct {
            __IO short unsigned int LPANA:1;    /* Link Partner AutoNegotiation Able  */
            __IO short unsigned int PR:1;       /* Page Received */
            __IO short unsigned int NPA:1;      /* Next Page Able */
            __IO short unsigned int LPNPA:1;    /* Link Partner Next Page Able */
            __IO short unsigned int PDF:1;      /* Parallel Detection Fault */
            __IO short unsigned int Res15_5:11; /* Reserved */
        } BitANE;
    } ;

    // Silicon Revision
    union unSIREV{
        __IO short unsigned int wSIREV;
        struct {
            __IO short unsigned int Res5_0:6;   /* Reserved */
            __IO short unsigned int REV:4;      /* Silicon Revision */
            __IO short unsigned int Res15_10:6; /* Reserved */
        } BitSIREV;
    } ;

    // Mode Control/Status
    union unMCS{
        __IO short unsigned int wMCS;
        struct {
            __IO short unsigned int Res0:1;     /* Reserved */
            __IO short unsigned int ENERGYON:1; /* ENERGYON */
            __IO short unsigned int FGLS:1;     /* Force Good Link Status */
            __IO short unsigned int PHYADBP:1;  /* PHY disregards PHY address in SMI access write */
            __IO short unsigned int Res5_4:2;   /* Reserved */
            __IO short unsigned int ALTINT:1;   /* Alternate Interrupt Mode */
            __IO short unsigned int Res8_7:2;   /* Reserved */
            __IO short unsigned int FARLOOPBACK:1;     /* FARLOOPBACK */
            __IO short unsigned int MDPREBP:1;  /* MDPREBP */
            __IO short unsigned int LOWSQEN:1;  /* LOWSQEN */
            __IO short unsigned int Res12:1;    /* Reserved */
            __IO short unsigned int EDPWRDOWN:1;/* EDPWRDOWN */
            __IO short unsigned int Res15_14:2; /* Reserved */
        } BitMCS;
    } ;

    // Special Modes
    union unSMODE{
        __IO short unsigned int wSMODE;
        struct {
            __IO short unsigned int PHYAD:5;    /* PHY Address */
            __IO short unsigned int MODE:3;     /* ENERGYON */
            __IO short unsigned int Res15_8:8;  /* Reserved */
        } BitSMODE;
    } ;

    // Symbol Error Counter
    __IO short unsigned int wERRCNT;

    // Special Control/Status Indications
    union unSCSR{
        __IO short unsigned int wSCSR;
        struct {
            __IO short unsigned int Res3_0:4;   /* Reserved */
            __IO short unsigned int XPOL:1;     /* Polarity state of the 10Base-T */
            __IO short unsigned int Res10_5:6;  /* Reserved */
            __IO short unsigned int SQEOFF:1;   /* Disable the SQE (Signal Quality Error) test */
            __IO short unsigned int Res12:1;    /* Reserved */
            __IO short unsigned int CH_SELECT:1;/* Manual Channel Select */
            __IO short unsigned int Res14:1;    /* Reserved */
            __IO short unsigned int AMDIXCTRL:1;/* HP Auto-MDIX control */
        } BitSCSR;
    } ;

    // Special Internal Testability Controls
    __IO short unsigned int wSITC;

    // Interrupt Source Flags
    union unINTSR{
        __IO short unsigned int wINTSR;
        struct {
            __IO short unsigned int Res0:1;     /* Reserved */
            __IO short unsigned int ANPR:1;     /* Auto-Negotiation Page Received */
            __IO short unsigned int PDF:1;      /* Parallel Detection Fault */
            __IO short unsigned int ANLPACK:1;  /* Auto-Negotiation LP Acknowledge */
            __IO short unsigned int LD:1;       /* Link Down (link status negated) */
            __IO short unsigned int RFD:1;      /* Remote Fault Detected */
            __IO short unsigned int ANC:1;      /* Auto-Negotiation complete */
            __IO short unsigned int ENERGYON:1; /* ENERGYON generated */
            __IO short unsigned int Res15_8:8;  /* Reserved */
        } BitINTSR;
    } ;

    // Interrupt Mask
    union unINTMASK{
        __IO short unsigned int wINTMASK;
        struct {
            __IO short unsigned int Res0:1;     /* Reserved */
            __IO short unsigned int ANPR:1;     /* Auto-Negotiation Page Received */
            __IO short unsigned int PDF:1;      /* Parallel Detection Fault */
            __IO short unsigned int ANLPACK:1;  /* Auto-Negotiation LP Acknowledge */
            __IO short unsigned int LD:1;       /* Link Down (link status negated) */
            __IO short unsigned int RFD:1;      /* Remote Fault Detected */
            __IO short unsigned int ANC:1;      /* Auto-Negotiation complete */
            __IO short unsigned int ENERGYON:1; /* ENERGYON generated */
            __IO short unsigned int Res15_8:8;  /* Reserved */
        } BitINTMASK;
    } ;

    // PHY Special Control/Status
    union unPHYSCR {
        __IO short unsigned int wPHYSCR;
        struct {
            __IO short unsigned int SD:1;       /* Scramble Disable */
            __IO short unsigned int Res1:1;     /* Reserved */
            __IO short unsigned int SI:3;       /* Speed Indication */
            __IO short unsigned int Res5:1;     /* Reserved */
            __IO short unsigned int EN4B5B:1;   /* Enable 4B5B */
            __IO short unsigned int GPO:3;      /* General Purpose Output connected to signals */
            __IO short unsigned int Res11_10:2; /* Reserved */
            __IO short unsigned int Autodone:1; /* Auto-negotiation done indication */
            __IO short unsigned int Res15_13:3;  /* Reserved */
        } BitPHYSCR;
    } ;
} PhyLanRegsTdf;

/*******************************************************************************
内部全局变量定义
*******************************************************************************/
static PhyLanRegsTdf PhyLanRegs;
static const unsigned char s_byaRegs[] = {
    CR,SR,IDL,IDH,ANA,ANLPA,ANLPA,SIREV,MCS,SMODE,ERRCNT,SCSR,SITC,INTSR,INTMASK,PHYSCR, 0xff
};

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*    读取Phy的所有寄存器
参数描述：
*    无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static void Sync(void)
{
    short unsigned int *pwReg = (short unsigned int *)&PhyLanRegs;

    for(int i = 0; s_byaRegs[i] != 0xff; i ++ ) {
        pwReg[i] = DevEth->ReadPhy(PHY_LAN8720_ADDR, s_byaRegs[i]);
    }
}

/*******************************************************************************
函数功能：
*    @ 检查是否PHY有连接
参数描述：
*    无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static bool IsLinked (void)
{
    union unSR unSr;

    unSr.wSR = DevEth->ReadPhy(PHY_LAN8720_ADDR, SR);
    if(unSr.BitSR.LINK == 1) return true;
    return false;
}

/*******************************************************************************
函数功能：
*    @ 复位PHY
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static int Reset (void)
{
    union unCR unCr;
    unCr.wCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, CR);

    unCr.BitCR.RST = 1;
    DevEth->WritePhy(PHY_LAN8720_ADDR, CR, unCr.wCR);

    while(1) {
        unCr.wCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, CR);
        if(unCr.BitCR.RST == 0) break;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*    @ 设置PHY自适应
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ ETH的MAC将根据这些参数自动设计MAC的参数
*******************************************************************************/
static int SetAutoNegotiation (void)
{
    unsigned int dwTimout = 0;
    union unPHYSCR unReg;
    union unCR unCr;
    unCr.wCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, CR);

    unCr.BitCR.AN_EN = 1;
    DevEth->WritePhy(PHY_LAN8720_ADDR, CR, unCr.wCR);

    while(dwTimout ++ < PHY_TIME_OFF) {
        unReg.wPHYSCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, PHYSCR);
        if(unReg.BitPHYSCR.Autodone == 1) break;
    }

    if(dwTimout >= PHY_TIME_OFF) return -1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 判断phy是否为高速设备
参数描述：
*   @ 无
返回值描述：
*   @ true:高速
*   @ false:低速
说明：
*   @ ETH的MAC将根据这些参数自动设计MAC的参数
*******************************************************************************/
static bool IsHispeed  (void)
{
    union unPHYSCR unReg;

    unReg.wPHYSCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, PHYSCR);
    if(unReg.BitPHYSCR.SI == 2 || unReg.BitPHYSCR.SI == 6) return true;
    return false;
}

/*******************************************************************************
函数功能：
*   @ 判断是否为全双工
参数描述：
*   @ 无
返回值描述：
*   @ true：全双工
*   @ false：半双工
说明：
*   @ ETH的MAC将根据这些参数自动设计MAC的参数
*******************************************************************************/
bool IsFullDuplex  (void)
{
    union unPHYSCR unReg;

    unReg.wPHYSCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, PHYSCR);
    if(unReg.BitPHYSCR.SI == 6 || unReg.BitPHYSCR.SI == 5) return true;
    return false;
}

/*******************************************************************************
函数功能：
*    @ 重置自适应
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ ETH的MAC将根据这些参数自动设计MAC的参数
*******************************************************************************/
static int RestartAutoNegotiation (void)
{
    union unPHYSCR unReg;
    union unCR unCr;
    unCr.wCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, CR);

    unCr.BitCR.RST_AN = 1;
    DevEth->WritePhy(PHY_LAN8720_ADDR, CR, unCr.wCR);

    while(1) {
        unReg.wPHYSCR = DevEth->ReadPhy(PHY_LAN8720_ADDR, PHYSCR);
        if(unReg.BitPHYSCR.Autodone == 1) break;
    }

    return 0;
}

static const EthPhyInterfaceTdf Interface = {
    .IsLinked = IsLinked,
    .Reset = Reset,
    SetAutoNegotiation,
    IsHispeed,
    IsFullDuplex,
    RestartAutoNegotiation,
    .GetEthIntHandlerSet = NULL
};

/*******************************************************************************
函数功能：
*   @ 取得ETH man的驱动参数接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
const EthPhyInterfaceTdf *GetPhyEthInteface(void)
{
    return &Interface;
}

static const DevLan8720Tdf s_Dev = {
    Sync,
    GetPhyEthInteface,
};

/*******************************************************************************
函数功能：
    初始化can 设备
参数描述：
    无
返回值描述：
    @ can设备操作方法集合
说明：
    @ 无。
*******************************************************************************/
const DevLan8720Tdf *Lan8720Init(void)
{
    SetAutoNegotiation();
    return &s_Dev;
}
#endif