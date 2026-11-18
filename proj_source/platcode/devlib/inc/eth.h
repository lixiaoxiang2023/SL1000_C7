/*******************************************************************************
文件名称：   bxcan.h
目录：       $PROJ_DIR$/../code/dev/src
项目名称：   事业一部驱动平台
创建者：     胥强
创建日期：   2018.08.25
程序用途：   以太网的驱动设备定义

版权声明：
*           Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息

*******************************************************************************/
#ifndef __ETH_H__
#define __ETH_H__

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#ifndef CONFIG_ETH_RXBUFNB
    #define CONFIG_ETH_RXBUFNB                          80
#endif
#ifndef CONFIG_ETH_RX_BUF_SIZE
    #define CONFIG_ETH_RX_BUF_SIZE                      128
#endif

#ifndef CONFIG_ETH_TXBUFNB
    #define CONFIG_ETH_TXBUFNB                          4
#endif
#ifndef CONFIG_ETH_TX_BUF_SIZE
    #define CONFIG_ETH_TX_BUF_SIZE                      1524
#endif

/*******************************************************************************
Include file
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
******************************************************************************/
typedef struct {
    union {
        __IO unsigned int TDES0;
        struct {
            __IO unsigned int DB:1;     __IO unsigned int UF:1;     __IO unsigned int ED:1;
            __IO unsigned int CC:4;     __IO unsigned int VF:1;     __IO unsigned int EC:1;
            __IO unsigned int LCO:1;    __IO unsigned int NC:1;     __IO unsigned int LCA:1;
            __IO unsigned int IPE:1;    __IO unsigned int FF:1;     __IO unsigned int JT:1;
            __IO unsigned int ES:1;     __IO unsigned int IHE:1;    __IO unsigned int TTSS:1;
            __IO unsigned int Res19_18:2; __IO unsigned int TCH:1;  __IO unsigned int TER:1;
            __IO unsigned int CIC:2;    __IO unsigned int Res24:1;  __IO unsigned int TTSE:1;
            __IO unsigned int DP:1;     __IO unsigned int DC:1;     __IO unsigned int FS:1;
            __IO unsigned int LS:1;     __IO unsigned int IC:1;     __IO unsigned int OWN:1;
        } BitTDES0;
    };

    union {
        __IO unsigned int TDES1;
        struct {
            __IO unsigned int TBS1:13;
            __IO unsigned int Res15_13:3;
            __IO unsigned int TBS2:13;
            __IO unsigned int Res31_29:3;
        } BitTDES1;
    };

    union {
      __IO unsigned int TBAP1;
      __IO unsigned int TTSL;
    };

    union {
      __IO unsigned int TBAP2;
      __IO unsigned int NDAP;
      __IO unsigned int TTSH;
    };
} EthDmaTxDescFrameTdf;

typedef struct {
    union {
        __IO unsigned int RDES0;
        struct {
            __IO unsigned int PCE_ESA:1;    __IO unsigned int CE:1;     __IO unsigned int DESCE:1;
            __IO unsigned int RE:1;         __IO unsigned int RWT:1;     __IO unsigned int FT:1;
            __IO unsigned int LCO:1;        __IO unsigned int IPHCE_TSV:1;     __IO unsigned int LS:1;
            __IO unsigned int FS:1;         __IO unsigned int VLAN:1;     __IO unsigned int OE:1;
            __IO unsigned int LE:1;         __IO unsigned int SAF:1;    __IO unsigned int DE:1;
            __IO unsigned int ES:1;         __IO unsigned int FL:14;  __IO unsigned int AFM:1;
            __IO unsigned int OWN:1;
        } BitRDES0;
    };

    union {
        __IO unsigned int RDES1;
        struct {
            __IO unsigned int RBS1:13;
            __IO unsigned int Res13:1;
            __IO unsigned int RCH:1;
            __IO unsigned int RER:1;
            __IO unsigned int RBS2:13;
            __IO unsigned int Res30_29:2;
            __IO unsigned int DIC:1;
        } BitRDES1;
    };

    union {
      __IO unsigned int RBAP1;
      __IO unsigned int RTSL;
    };

    union {
      __IO unsigned int RBAP2;
      __IO unsigned int RTSH;
    };
} EthDmaRxDescFrameTdf;

typedef struct {
    bool (*IsLinked) (void);
    int  (*Reset) (void);
    int  (*SetAutoNegotiation) (void);
    bool (*IsHispeed)  (void);
    bool (*IsFullDuplex)  (void);
    int  (*RestartAutoNegotiation) (void);

    /* eth向phy注册外部中断函数，用于有的phy有状态变化时会产生外部中断 */
    void (*GetEthIntHandlerSet) (void (*pfHandler) (void));
} EthPhyInterfaceTdf;

typedef struct {
    void (*SetPhyInterface) (const EthPhyInterfaceTdf *PhyInterface);
    void (*ConfigMac) (void);
    int  (*WritePhy) (unsigned char PHYAddr, unsigned char PHYReg, short unsigned int PHYValue);
    short unsigned int (*ReadPhy) (unsigned char PHYAddr, unsigned char byReg);

    int  (*EthStart) (const unsigned char *pbyAddr);

    bool (*GetLock) (void) ;
    void (*GiveLock) (void);
    int  (*Send) (unsigned char *pbyIn, unsigned int dwLength,
                short unsigned int (*pfDatInHadler)(void *pvIN, unsigned char *pbyInDat, unsigned int dwLenLimt), void *pvIN);
    int  (*SetRxHandler) (void (*pfHandler) (const unsigned char *pbyDat, unsigned int byLen));
    int  (*SetStartHandler) (void (*pfHandler) (void));
    void (*Service) (void);

    void (*EthDown) (void);
    void (*EthUp) (void);
    void (*SetMac) (const unsigned char *pbyMac);
    bool (*IsLinked) (void);
    bool (*Receive) (void);
} DevEthTdf;

/*******************************************************************************
内部全局变量申明
******************************************************************************/
extern const DevEthTdf *DevEth;

/*******************************************************************************
函数申明
******************************************************************************/
const DevEthTdf *EthInit(void);

#endif
