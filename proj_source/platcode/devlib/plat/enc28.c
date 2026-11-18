/*******************************************************************************
文件名称：   enc28.c
文件目录：   $PROJ_DIR$/usr/Drive
项目名称：   事业一部技术平台
创建者：    黄鹏
创建日期：   20121008
程序用途：   以太网控制芯片驱动
版权声明：   Copyright urit Co.,Ltd.All Right Reserved

第一次修改：
修改人：胥强
修改内容：加入技术平台
日期：20160805
*******************************************************************************/

/******************************************************************************
Include files头文件
******************************************************************************/
#include "devplat.h"

#if(CONFIG_ENC28)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define DUMMY_BYTE       0xA5
/****************************SPI指令集*****************************************/
#define RCR              0x00      //读控制寄存器
#define RBM              0x3A      //读缓冲器
#define WCR              0x40      //写控制寄存器
#define WBM              0x7A      //写缓冲器
#define BFS              0x80      //位域置1
#define BFC              0xA0      //位域清零
#define SRESET           0xFF      //系统命令（软件复位）

/*******************************************************************************/
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
#define SPRD_MASK        0x80

/***************************通用控制寄存器*************************************/
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F

/************************Bank0控制寄存器***************************************/
#define ERDPTL           (0x00|0x00)
#define ERDPTH           (0x01|0x00)
#define EWRPTL           (0x02|0x00)
#define EWRPTH           (0x03|0x00)
#define ETXSTL           (0x04|0x00)
#define ETXSTH           (0x05|0x00)
#define ETXNDL           (0x06|0x00)
#define ETXNDH           (0x07|0x00)
#define ERXSTL           (0x08|0x00)
#define ERXSTH           (0x09|0x00)
#define ERXNDL           (0x0A|0x00)
#define ERXNDH           (0x0B|0x00)
#define ERXRDPTL         (0x0C|0x00)
#define ERXRDPTH         (0x0D|0x00)
#define ERXWRPTL         (0x0E|0x00)
#define ERXWRPTH         (0x0F|0x00)
#define EDMASTL          (0x10|0x00)
#define EDMASTH          (0x11|0x00)
#define EDMANDL          (0x12|0x00)
#define EDMANDH          (0x13|0x00)
#define EDMADSTL         (0x14|0x00)
#define EDMADSTH         (0x15|0x00)
#define EDMACSL          (0x16|0x00)
#define EDMACSH          (0x17|0x00)

/***********************Bank1控制寄存器****************************************/
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCSL           (0x10|0x20)
#define EPMCSH           (0x11|0x20)
#define EPMOL            (0x14|0x20)
#define EPMOH            (0x15|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)

/***********************Bank2控制寄存器****************************************/
#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPGL           (0x06|0x40|0x80)
#define MAIPGH           (0x07|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFLL          (0x0A|0x40|0x80)
#define MAMXFLH          (0x0B|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWRL            (0x16|0x40|0x80)
#define MIWRH            (0x17|0x40|0x80)
#define MIRDL            (0x18|0x40|0x80)
#define MIRDH            (0x19|0x40|0x80)

/***********************Bank3控制寄存器****************************************/
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCSL          (0x08|0x60)
#define EBSTCSH          (0x09|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUSL           (0x18|0x60)
#define EPAUSH           (0x19|0x60)

/*****************************PHT寄存器****************************************/
#define PHCON1              0x00
#define PHSTAT1             0x01
#define PHHID1              0x02
#define PHHID2              0x03
#define PHCON2              0x10
#define PHSTAT2             0x11
#define PHIE                0x12
#define PHIR                0x13
#define PHLCON              0x14

/*******************接收过滤器控制寄存器ERXFCON的位定义************************/
#define ERXFCON_UCEN        0x80
#define ERXFCON_ANDOR       0x40
#define ERXFCON_CRCEN       0x20
#define ERXFCON_PMEN        0x10
#define ERXFCON_MPEN        0x08
#define ERXFCON_HTEN        0x04
#define ERXFCON_MCEN        0x02
#define ERXFCON_BCEN        0x01

/**************************以太网中断允许寄存器EIE的位定义*********************/
#define EIE_INTIE           0x80
#define EIE_PKTIE           0x40
#define EIE_DMAIE           0x20
#define EIE_LINKIE          0x10
#define EIE_TXIE            0x08
#define EIE_WOLIE           0x04
#define EIE_TXERIE          0x02
#define EIE_RXERIE          0x01

/*******************以太网中断请求标志寄存器EIR的位定义************************/
#define EIR_PKTIF           0x40
#define EIR_DMAIF           0x20
#define EIR_LINKIF          0x10
#define EIR_TXIF            0x08
#define EIR_WOLIF           0x04
#define EIR_TXERIF          0x02
#define EIR_RXERIF          0x01

/*************************以太网状态寄存器ESTAT的位定义************************/
#define ESTAT_INT           0x80
#define ESTAT_LATECOL       0x10
#define ESTAT_RXBUSY        0x04
#define ESTAT_TXABRT        0x02
#define ESTAT_CLKRDY        0x01

/***********************以太网控制寄存器2 ECON2的位定义************************/
#define ECON2_AUTOINC       0x80
#define ECON2_PKTDEC        0x40
#define ECON2_PWRSV         0x20
#define ECON2_VRPS          0x08

/***********************以太网控制寄存器1 ECON1的位定义************************/
#define ECON1_TXRST         0x80
#define ECON1_RXRST         0x40
#define ECON1_DMAST         0x20
#define ECON1_CSUMEN        0x10
#define ECON1_TXRTS         0x08
#define ECON1_RXEN          0x04
#define ECON1_BSEL1         0x02
#define ECON1_BSEL0         0x01

/*************************MAC控制寄存器1 MACON1的位定义************************/
#define MACON1_LOOPBK       0x10
#define MACON1_TXPAUS       0x08
#define MACON1_RXPAUS       0x04
#define MACON1_PASSALL      0x02
#define MACON1_MARXEN       0x01

/*************************MAC控制寄存器2 MACON2的位定义************************/
#define MACON2_MARST        0x80
#define MACON2_RNDRST       0x40
#define MACON2_MARXRST      0x08
#define MACON2_RFUNRST      0x04
#define MACON2_MATXRST      0x02
#define MACON2_TFUNRST      0x01

/*************************MAC控制寄存器3 MACON3的位定义************************/
#define MACON3_PADCFG2      0x80
#define MACON3_PADCFG1      0x40
#define MACON3_PADCFG0      0x20
#define MACON3_TXCRCEN      0x10
#define MACON3_PHDRLEN      0x08
#define MACON3_HFRMLEN      0x04
#define MACON3_FRMLNEN      0x02
#define MACON3_FULDPX       0x01

/***************************MII命令寄存器 MICMD的位定义************************/
#define MICMD_MIISCAN       0x02
#define MICMD_MIIRD         0x01

/**************************MII状态寄存器 MISTAT的位定义************************/
#define MISTAT_NVALID       0x04
#define MISTAT_SCAN         0x02
#define MISTAT_BUSY         0x01

/*************************PHY控制寄存器1 PHCON1的位定义************************/
#define PHCON1_PRST         0x8000
#define PHCON1_PLOOPBK      0x4000
#define PHCON1_PPWRSV       0x0800
#define PHCON1_PDPXMD       0x0100

/*********************物理层状态寄存器1 PHSTAT1的位定义************************/
#define PHSTAT1_PFDPX       0x1000
#define PHSTAT1_PHDPX       0x0800
#define PHSTAT1_LLSTAT      0x0004
#define PHSTAT1_JBSTAT      0x0002

/*********************物理层状态寄存器1 PHSTAT1的位定义************************/
#define PHCON2_FRCLINK      0x4000
#define PHCON2_TXDIS        0x2000
#define PHCON2_JABBER       0x0400
#define PHCON2_HDLDIS       0x0100

/**************************包控制字节格式的的位定义****************************/
#define PKTCTRL_PHUGEEN     0x08
#define PKTCTRL_PPADEN      0x04
#define PKTCTRL_PCRCEN      0x02
#define PKTCTRL_POVERRIDE   0x01

#define PHIE_PGEIE          0x02
#define PHIE_PLNKIE         0x10

#define MEM_TOTAL           0x2000
#define MEM_RXSTART         0
#define MEM_RXEND           (0x1FFF-0x0600-1)
#define MEM_TXSTART         (0x1FFF-0x0600)
#define MEM_TXEND           0x1FFF
#define MAX_FRAMELEN        1518        // 允许接收或发送的最大帧长度
#define TIME_OUT            250
#define DELAY_TIME_OUT      0xffff
#define RX_TX_BUFF_LENTH    1540

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
　　内部全局变量定义
*******************************************************************************/
static struct
{
    struct {
        unsigned char bySpiNo;
        unsigned char bySpiDmaTxNo;
        unsigned char bySpiDmaTxChn;
        unsigned char bySpiDmaTxStream;

        unsigned char bySpiDmaRxNo;
        unsigned char bySpiDmaRxChn;
        unsigned char bySpiDmaRxStream;

        unsigned int  *pdwCs;
    }stConfig;

	unsigned short int wNextPackPtr;
	unsigned short int wTxCnt;
    unsigned int  dwIsLock;

	unsigned char  byBank;
    void    (*pvHandler)(void);

	unsigned char  byaTxBuff[RX_TX_BUFF_LENTH];//接收数据缓冲区
    unsigned char  byaRxBuff[RX_TX_BUFF_LENTH];//接收数据缓冲区
}s_stCtr;

/*******************************************************************************
外部函数申明
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 数据传输
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 将调用spi核心层进行数据发送
*******************************************************************************/
static void DmaTrs(void)
{
    if(s_stCtr.wTxCnt == 1) {
        //快速处理1个字节的数据操作
        DevSpi->Trs(s_stCtr.stConfig.bySpiNo, s_stCtr.byaTxBuff, s_stCtr.byaRxBuff, 1, s_stCtr.stConfig.pdwCs);
        return;
    }

    DevSpi->TrsDma(s_stCtr.stConfig.bySpiNo, s_stCtr.byaTxBuff, s_stCtr.byaRxBuff, s_stCtr.wTxCnt, s_stCtr.stConfig.pdwCs);
}

/*******************************************************************************
函数功能：
*   @ 软件复位
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SwReSet(void)
{
	s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = SRESET;

	DmaTrs();
}

/*******************************************************************************
函数功能：
*   @ 设置ENC25J60控制寄存器所在的区域
参数描述：
*   @ unsigned char  byReg 控制寄存器地址
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SetBank(unsigned char  byReg)
{
	if((byReg & ADDR_MASK) > 0x1a) return;//通用部分不需要分组
	if((byReg & BANK_MASK) == s_stCtr.byBank) return;//不需要重新设定分组

    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = BFC | ECON1;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = (ECON1_BSEL1 | ECON1_BSEL0);
    DmaTrs();

    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = BFS | ECON1;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = ((byReg & BANK_MASK)>>5);
    DmaTrs();
	s_stCtr.byBank = (byReg & BANK_MASK);
}

/*******************************************************************************
函数功能：
*   @ 将ETH相关的寄存器的位设定为1
参数描述：
*   @ unsigned char  byReg 控制寄存器地址
*   @ unsigned char  Indat需要写为1的位数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void WriteRegBit1(unsigned char byReg, unsigned char Indat)
{
	if(byReg & SPRD_MASK) return;
	SetBank(byReg);
	byReg &= ADDR_MASK;
	byReg |= BFS;
    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = byReg;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = Indat;
    DmaTrs();
}

/*******************************************************************************
函数功能：
*   @ 将ETH相关的寄存器的位设定为0
参数描述：
*   @ unsigned char  byReg 控制寄存器地址
*   @ unsigned char  Indat需要写为0的位数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void WriteRegBit0(unsigned char  byReg, unsigned char  Indat)
{
	if(!(byReg & SPRD_MASK)) return;
	SetBank(byReg);
	byReg &= ADDR_MASK;
	byReg |= BFC;
    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = byReg;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = Indat;
    DmaTrs();
}

/*******************************************************************************
函数功能：
*   @ 读取缓冲区
参数描述：
*   @ unsigned short int wLen 数据长度
*   @ unsigned char  *pbyOut 数据输出
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void ReadBuffer(unsigned short int wLen, unsigned char  *pbyOut)
{
    s_stCtr.wTxCnt = 0;
    memset(s_stCtr.byaTxBuff,DUMMY_BYTE,RX_TX_BUFF_LENTH);
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = RBM;
	memset((unsigned char *)&s_stCtr.byaTxBuff[s_stCtr.wTxCnt],0,wLen);
	s_stCtr.wTxCnt += wLen;
    DmaTrs();

    memcpy(pbyOut,(unsigned char *)&s_stCtr.byaRxBuff[1],wLen);
}

/*******************************************************************************
函数功能：
*   @ 驱动器的数据缓冲区写入数据
参数描述：
*   @ unsigned short int wLen 数据长度
*   @ unsigned char  *pbyOut 数据输入
*   @ bool blWithCtr true发送特殊控制字，false不发送
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void WriteBuffer(unsigned short int wLen, unsigned char * pbyInDat, bool blWithCtr)
{
    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = WBM;
    if(blWithCtr) s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = 0; //需要写入特殊的控制字
	memcpy((unsigned char *)&s_stCtr.byaTxBuff[s_stCtr.wTxCnt],pbyInDat,wLen);
	s_stCtr.wTxCnt += wLen;
    DmaTrs();
}

/*******************************************************************************
函数功能：
*   @ 读取ETH、MAC、MII数据据寄存器
参数描述：
*   @ unsigned char  byReg 控制寄存器地址
返回值描述：
*   @ 寄存器值
说明：
*   @ 无
*******************************************************************************/
static unsigned char  ReadReg(unsigned char byReg)
{
	unsigned char  byFlag = 0;
	SetBank(byReg);
	if(byReg & SPRD_MASK) byFlag = 1;
	byReg &= ADDR_MASK;
	byReg |= RCR;

    memset(s_stCtr.byaTxBuff, DUMMY_BYTE, 4);
    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = byReg;
	if(!byFlag) s_stCtr.wTxCnt += 1;//对于非ETH寄存器 SPI读的时候需要2个字节
	else s_stCtr.wTxCnt += 2;
    DmaTrs();

	return s_stCtr.byaRxBuff[s_stCtr.wTxCnt - 1];
}

/*******************************************************************************
函数功能：
*   @ 读取ETH、MAC、MII数据据寄存器
参数描述：
*   @ unsigned char  byReg 控制寄存器地址
*   @ unsigned char  Indat 值
返回值描述：
*   @ 寄存器值
说明：
*   @ 无
*******************************************************************************/
static void WriteReg(unsigned char  byReg, unsigned char  Indat)
{
	SetBank(byReg);
	byReg &= ADDR_MASK;
	byReg |= WCR;

    s_stCtr.wTxCnt = 0;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = byReg;
	s_stCtr.byaTxBuff[s_stCtr.wTxCnt ++] = Indat;
    DmaTrs();
}

/*******************************************************************************
函数功能：
*   @ 写ENC28J60的PHY寄存器
参数描述：
*   @ unsigned char  byReg 控制寄存器地址,
*   @ unsigned short int wData 写入的地址
返回值描述：
*   @ 寄存器值
说明：
*   @ 无
*******************************************************************************/
static void WritePhy(unsigned char  byReg, unsigned short int wData)
{
	WriteReg(MIREGADR, byReg);
	WriteReg(MIWRL, wData);
	WriteReg(MIWRH, wData >> 8);

	while(ReadReg(MISTAT) & MISTAT_BUSY) {
        asm("nop");
        asm("nop");
    }
}

/*******************************************************************************
函数功能：
*   @ 读ENC28J60的PHY寄存器
参数描述：
*   @ unsigned char  byReg 控制寄存器地址
返回值描述：
*   @ 寄存器值
说明：
*   @ 无
*******************************************************************************/
static unsigned short int ReadPhy(unsigned char  byReg)
{
    unsigned short int wData;

	WriteReg(MIREGADR, byReg);
	WriteReg(MICMD, MICMD_MIIRD);
	WriteReg(MISTAT,MISTAT_BUSY);

	while(ReadReg(MISTAT) & MISTAT_BUSY) {
        asm("nop");
        asm("nop");
    }

    WriteReg(MICMD, 0x00);
	wData = ReadReg(MIRDH);
    wData <<= 8;
    wData |= ReadReg(MIRDL);

    return wData;
}

/*******************************************************************************
函数功能：
*   @ 进入锁定以太网设备
参数描述：
*   @ unsigned int *pdwLock 锁变量地址
返回值描述：
*   @ true：数据没有被占用，可以进行使用
*   @ false：数据被占用，不能使用
说明：
*   @ 在使用设备时需要先调用进行锁定，否则可能会存在设备冲突的问题。
*******************************************************************************/
static bool EnterUse(void)
{
    return DevIo->CommLock( &s_stCtr.dwIsLock, false);
}

/*******************************************************************************
函数功能：
*   @ 读ENC28J60的PHY寄存器
参数描述：
*   @ unsigned int *pdwLock 锁变量地址
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ExitUse(void)
{
    DevIo->CommUnLock( &s_stCtr.dwIsLock);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 检查网络连接状态
参数描述：
*   @ 无
返回值描述：
*   @ 1 ：连接
*   @ 0 ：未连接
说明：
*   @ 无
*******************************************************************************/
static unsigned int CheckETHLink(void)
{
    unsigned short int wData;

    wData = ReadPhy(PHSTAT2);
    return (wData & 0x0400)?1:0;
}

/*******************************************************************************
函数功能：
*   @ 读ENC28J60的以太网版本号
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 设备忙
*   @ 其他：版本
说明：
*   @ 无
*******************************************************************************/
static unsigned char GetRev(void)
{
    unsigned char  byDate;

    byDate = ReadReg(EREVID);
	return byDate;
}

/*******************************************************************************
函数功能：
*   @ ENC28J60频率输出
参数描述：
*   @ unsigned char clk 需要输出的频率
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void OutClk(unsigned char byClk)
{
    //setup clkout: 2 is 12.5MHz:
	WriteReg(ECOCON, byClk & 0x7);
}

/*******************************************************************************
函数功能：
*   @ ENC28J60发送一个数据包
参数描述：
*   @ unsigned char *pbyIn 数据输入
*   @ unsigned int dwLen 数据长度
返回值描述：
*   @ -1 设备占用，
*   @ -2 网口忙
*   @ 0 正常
说明：
*   @ 无
*******************************************************************************/
static int Write(unsigned char *pbyIn, unsigned int dwLen)
{
    unsigned char  byGetReg = 0;

    //查询mac是否正在发送数据
    byGetReg = ReadReg(ECON1);
    if(byGetReg & ECON1_TXRTS) return -2;
    if(!dwLen) return -1;

	// 设置写指针开始的传输缓冲区域
	WriteReg(EWRPTL, MEM_TXSTART & 0xFF);
	WriteReg(EWRPTH, MEM_TXSTART >> 8);

    //根据给定数据域的大小配置发送缓冲区的末尾地址
	WriteReg(ETXNDL, (MEM_TXSTART + dwLen) & 0xFF);
	WriteReg(ETXNDH, (MEM_TXSTART + dwLen) >> 8);

	// 复制包到传输缓冲
	WriteBuffer(dwLen, pbyIn, true);
    //发送缓冲区的内容在网络上传输
	WriteRegBit1(ECON1, ECON1_TXRTS);

	if( (ReadReg(EIR) & EIR_TXERIF) ) {
        WriteRegBit0(ECON1, ECON1_TXRTS);
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 返回EIR数据寄存器数据
参数描述：
*   @ 无
返回值描述：
*   @ 数据
说明：
*   @ 无
*******************************************************************************/
static unsigned short int ReadEIR(void)
{
    unsigned char  byDat;

    byDat = ReadReg(EIR);
    return (unsigned short int)byDat;
}

/*******************************************************************************
函数功能：
*   @ 数据接收到处理接口
参数描述：
*   @ 无
返回值描述：
*   @ 数据
说明：
*   @ 注册驱动上层统治函数，本驱动需要尽量避免的通知函数内部进行数据操作，因为
            有大量的等待操作，应避免在中断中进行
*******************************************************************************/
static void RegRxHandler(void (*Handler)(void))
{
    s_stCtr.pvHandler = Handler;
}

/*******************************************************************************
函数功能：
*   @ 复位InitEnc28j60 缓冲器
参数描述：
*   @ 无
返回值描述：
*   @ 数据
说明：
*   @ 无
*******************************************************************************/
static void ResetBuff(void)
{
    WriteRegBit0(ECON1, ECON1_RXEN);
    while(ReadReg(EPKTCNT) != 0) {
		WriteRegBit1(ECON2, ECON2_PKTDEC);  //设置数据指针自增
    }

    //初始化接收缓冲区，设置接收起始地址
	s_stCtr.wNextPackPtr = MEM_RXSTART;  //读下一数据包指针
	WriteReg(ERXSTL, MEM_RXSTART & 0xFF);
	WriteReg(ERXSTH, MEM_RXSTART >> 8);
	//设置接收读指针指向地址
	WriteReg(ERXRDPTL, MEM_RXSTART & 0xFF);
	WriteReg(ERXRDPTH, MEM_RXSTART >> 8);
	//设置接收缓冲区的末尾地址
	// ERXND寄存器默认指向整个缓冲区的最后一个单元
	WriteReg(ERXNDL, MEM_RXEND & 0xFF);
	WriteReg(ERXNDH, MEM_RXEND >> 8);
	//设置发送缓冲区的起始地址
	//ETXST寄存器默认地址是整个缓冲区的第一个单元
	WriteReg(ETXSTL, MEM_TXSTART & 0xFF);
	WriteReg(ETXSTH, MEM_TXSTART >> 8);
	//设置发送缓冲区的末尾地址
	WriteReg(ETXNDL, MEM_TXEND & 0xFF);
	WriteReg(ETXNDH, MEM_TXEND >> 8);

    WriteReg(EIR, 0);
	// 使能数据包接收
	WriteRegBit1(ECON1, ECON1_RXEN);
}

/*******************************************************************************
函数功能：
*   @ ENC28J60接收一个数据包
参数描述：
*   @ unsigned short int wMaxlen 数据操作的最大长度
*   @ unsigned char * pPacket 数据输出地址
返回值描述：
*   @ 数据长度
说明：
*   @ 无
*******************************************************************************/
static int Read(unsigned char * pbyOut)
{
    unsigned char  byaGet[6];
	unsigned short int wRxstat;
	unsigned int dwLen = 0;

    //检查是否接收一个数据包到缓冲
	if(ReadReg(EPKTCNT) == 0) return(0);

    //设置读指针指向开始接收到的数据包
	WriteReg(ERDPTL, (s_stCtr.wNextPackPtr));
	WriteReg(ERDPTH, (s_stCtr.wNextPackPtr) >> 8);
    ReadBuffer(6, byaGet);
    s_stCtr.wNextPackPtr = byaGet[0] + (byaGet[1] << 8);
    dwLen = byaGet[2] + (byaGet[3] << 8);
    wRxstat = byaGet[4] + (byaGet[5] << 8);  //读取接收状态

    if((dwLen > RX_TX_BUFF_LENTH - 1) || !(wRxstat & 0x80)) {
        /*出错之后复位数据缓冲区*/
        ResetBuff();
        return 0;
    }

    ReadBuffer(dwLen, pbyOut);  //检查CRC错误
	//将RX指针指向下一个接收数据包,释放已读出的内存空间
	WriteReg(ERXRDPTL, (s_stCtr.wNextPackPtr));
	WriteReg(ERXRDPTH, (s_stCtr.wNextPackPtr) >> 8);
	WriteRegBit1(ECON2, ECON2_PKTDEC);  //设置数据指针自增

	return (dwLen);
}

/*******************************************************************************
函数功能：
*   @ 关闭接收与重设缓冲区
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int SetDown(void)
{
    WriteRegBit0(ECON1, ECON1_RXEN);
    ResetBuff();
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 打开接收
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int SetUp(void)
{
    WriteRegBit1(ECON1, ECON1_RXEN);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置以太网
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int Config(unsigned char *pbyMac)
{
    //*****Bank0区相关寄存器配置
	SwReSet();//软件复位
    for(unsigned char  i = 0;i < 250;i++) {
        for(unsigned char  j =0;j < 250;j++) {;}
    }

    //初始化接收缓冲区，设置接收起始地址
	s_stCtr.wNextPackPtr = MEM_RXSTART;  // 读下一数据包指针
	WriteReg(ERXSTL, MEM_RXSTART & 0xFF);
	WriteReg(ERXSTH, MEM_RXSTART >> 8);
	//设置接收读指针指向地址
	WriteReg(ERXRDPTL, MEM_RXSTART & 0xFF);
	WriteReg(ERXRDPTH, MEM_RXSTART >> 8);
	//设置接收缓冲区的末尾地址
	// ERXND寄存器默认指向整个缓冲区的最后一个单元
	WriteReg(ERXNDL, MEM_RXEND & 0xFF);
	WriteReg(ERXNDH, MEM_RXEND >> 8);
	//设置发送缓冲区的起始地址
	//ETXST寄存器默认地址是整个缓冲区的第一个单元
	WriteReg(ETXSTL, MEM_TXSTART & 0xFF);
	WriteReg(ETXSTH, MEM_TXSTART >> 8);
	//设置发送缓冲区的末尾地址
	WriteReg(ETXNDL, MEM_TXEND & 0xFF);
	WriteReg(ETXNDH, MEM_TXEND >> 8);

    //*****Bank1区相关寄存器配置
	//设置包过滤寄存器
    WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
    WriteReg(EPMM0, 0x3f);
    WriteReg(EPMM1, 0x30);
    WriteReg(EPMCSL, 0xf9);
    WriteReg(EPMCSH, 0xf7);  //校验广播地址和数据格式  FF FF FF FF FF FF XX XX XX XX XX XX 08 06
	//*****Bank2区相关寄存器配置
    //MAC初始化配置
	//MACON2清零，让MAC退出复位状态
    WriteReg(MACON2, 0x00);
    //MAC接收使能，下行程序段表示使能MAC接收，使能IEEE流量控制
	WriteReg(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	// 启用自动填充到60个字节和CRC操作
	WriteReg(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
	//配置非背对背包之间的间隔
	WriteReg(MAIPGL, 0x12);
	//WriteReg(MAIPGH, 0x0C);
	//配置背对背包之间的间隔
	WriteReg(MABBIPG, 0x15);
	//设置允许接收或发送的最大帧长度编程
	WriteReg(MAMXFLL, MAX_FRAMELEN & 0xFF);
	WriteReg(MAMXFLH, MAX_FRAMELEN >> 8);

    //*****Bank3区相关寄存器配置
	// 将MAC地址写入MAADR0-MAADR5寄存器中
	WriteReg(MAADR5, pbyMac[0]);
	WriteReg(MAADR4, pbyMac[1]);
	WriteReg(MAADR3, pbyMac[2]);
	WriteReg(MAADR2, pbyMac[3]);
	WriteReg(MAADR1, pbyMac[4]);
	WriteReg(MAADR0, pbyMac[5]);

	WritePhy(PHCON1, PHCON1_PDPXMD);
	//阻止发送回路的自动环回
	WritePhy(PHCON2, PHCON2_HDLDIS);
    //*****Bank0区相关寄存器配置
	SetBank(ECON1);
	// 使能中断
	WriteRegBit1(EIE, EIE_INTIE|EIE_PKTIE|EIE_LINKIE);
	// 使能数据包接收
	WriteRegBit1(ECON1, ECON1_RXEN);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 以太网中断回调函数
参数描述：
*   @ 无
返回值描述：
*   @ 方法集合
说明：
*   @ 无
*******************************************************************************/
static void Encj28Handler(unsigned int dwePare)
{
    if(s_stCtr.pvHandler != 0) s_stCtr.pvHandler();
}

static const DevEnc28Tdf s_Dev = {
    Read,
    Write,

    SetUp,
    SetDown,

    CheckETHLink,
    GetRev,
    OutClk,

    ReadEIR,
    RegRxHandler,
    EnterUse,
    ExitUse,
    Config,
};

/*******************************************************************************
函数功能：
*   @ 初始化以太网驱动
参数描述：
*   @ 无
返回值描述：
*   @ 方法集合
说明：
*   @ 无
*******************************************************************************/
const DevEnc28Tdf *Enc28j60Init(Enc28InitTdf* pstInit)
{
    memset((unsigned char  *)&s_stCtr,0,sizeof(s_stCtr));

    DevSpi->Config(pstInit->bySpiNo, pstInit->dwSpiBaud); // 36M 4分频 9M

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    DevSpi->ConfigDmaRx(pstInit->bySpiNo, pstInit->bySpiDmaRxNo, pstInit->bySpiDmaRxChn, pstInit->bySpiDmaRxStream);
    DevSpi->ConfigDmaTx(pstInit->bySpiNo, pstInit->bySpiDmaTxNo, pstInit->bySpiDmaTxChn, pstInit->bySpiDmaTxStream);
#else
    DevSpi->ConfigDmaRx(pstInit->bySpiNo, pstInit->bySpiDmaRxNo, pstInit->bySpiDmaRxChn);
    DevSpi->ConfigDmaTx(pstInit->bySpiNo, pstInit->bySpiDmaTxNo, pstInit->bySpiDmaTxChn);
#endif

    s_stCtr.stConfig.bySpiDmaRxChn  = pstInit->bySpiDmaRxChn;
    s_stCtr.stConfig.bySpiDmaRxNo   = pstInit->bySpiDmaRxNo;
    s_stCtr.stConfig.bySpiDmaTxChn  = pstInit->bySpiDmaTxChn;
    s_stCtr.stConfig.bySpiDmaTxNo   = pstInit->bySpiDmaTxNo;
    s_stCtr.stConfig.bySpiNo        = pstInit->bySpiNo;
    s_stCtr.stConfig.pdwCs          = pstInit->pdwNcs;
    s_stCtr.stConfig.bySpiDmaTxStream = pstInit->bySpiDmaTxStream;
    s_stCtr.stConfig.bySpiDmaRxStream = pstInit->bySpiDmaRxStream;

    DevIo->AddIrp(pstInit->byItGpioNO, pstInit->byItChn, Trigger_Rising_Falling, Encj28Handler);
    *pstInit->pdwNcs = 1;
    *pstInit->pdwNrst = 1;
    for(__IO unsigned int i = 0;i < 250;i++);
    *pstInit->pdwNrst = 0;
    for(__IO unsigned int i = 0;i < 250;i++);
    *pstInit->pdwNrst = 1;
    for(__IO unsigned int i = 0;i < 250;i++);
    *pstInit->pdwNcs = 1;

    return &s_Dev;
}
#endif