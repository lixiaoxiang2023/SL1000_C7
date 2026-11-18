/*******************************************************************************
文件名称：   eth.c
文件目录：   $PROJ_DIR$/../code/xcanlib
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.15
程序用途：   以太网驱动程序,用于发送和接收数据

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
2016.05.24
修改人：胥强
1、修改自动重传误关闭问题
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "dev.h"
#include "misc.h"

#if(CONFIG_ETH)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define PHY_WRITE_READ_TIME_OUT         ((uint32_t)0x0002FFFF)

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
static struct EthBuff {
    const EthPhyInterfaceTdf *pstInterface;

    enum emEthSta{
        ETH_IDLE            = 0,
        ETH_NO_PHY,
        ETH_UNLINK_UNINIT,
        ETH_LINK_NO_START,
        ETH_LINK,
        ETH_DOWN
    } emEthStatus;

    enum {
        HI_SPD = 0,
        LOW_SPD
    }emSpeed;

    enum {
        FULL_DUPLEX = 0,
        HALF_DUPLEX
    }emDuplex;


    void (*pfReciveHandler) (const unsigned char *pbyDat, unsigned int byLen);
    void (*pfTxHandler) (bool);
    void (*pfEthStartHandler) (void);

    unsigned int dwRxCnt;
    unsigned int dwLock;
    unsigned char abyMac[6];

    unsigned int dwRxOff;
    unsigned char abyRxFrame[1560];
} s_stEth;

static struct {
    EthDmaRxDescFrameTdf  DMARxDescTab[CONFIG_ETH_RXBUFNB];               /* Ethernet Rx MA Descriptor */
    EthDmaTxDescFrameTdf  DMATxDescTab[CONFIG_ETH_TXBUFNB];               /* Ethernet Tx DMA Descriptor */

    unsigned char Rx_Buff[CONFIG_ETH_RXBUFNB][CONFIG_ETH_RX_BUF_SIZE];    /* Ethernet Receive Buffer */
    unsigned char Tx_Buff[CONFIG_ETH_TXBUFNB][CONFIG_ETH_TX_BUF_SIZE];    /* Ethernet Transmit Buffer */

    __IO EthDmaRxDescFrameTdf  *pstFrameRx;
    // __IO EthDmaTxDescFrameTdf  *pstFrameTx;
} s_stDMA;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
    @ 在发生phy外部中断时，检查phy是否连接
参数描述：
    @ 无
返回值描述：
    @ 无
说明：
    @ 无。
*******************************************************************************/
static void PhyIntHandler(void)
{
    if(s_stEth.pstInterface == NULL) return ;
    if(s_stEth.pstInterface->IsLinked == NULL) return ;
    if(s_stEth.pstInterface->IsLinked() == false) s_stEth.emEthStatus = ETH_UNLINK_UNINIT;
}

/*******************************************************************************
函数功能：
*    @ 设置phy的操作接口
参数描述：
*    @ 无
返回值描述：
*    @ -1 ： 失败
*    @  0 ： 成功
说明：
*    @ 无。
*******************************************************************************/
static void SetPhyInterface(const EthPhyInterfaceTdf *PhyInterface)
{
    s_stEth.pstInterface = PhyInterface;
}

/*******************************************************************************
函数功能：
    @ 基本设置
参数描述：
    @ 无
返回值描述：
    @ -1 ： 失败
    @  0 ： 成功
说明：
    @ 无。
*******************************************************************************/
static void ConfigMac(void)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwHclk = 0;
    unsigned int dwTimOut = 0;

    s_stEth.emSpeed = HI_SPD;
    s_stEth.emDuplex = FULL_DUPLEX;

    // 检查phy接口参数
    if(s_stEth.pstInterface == NULL) return;

    s_stEth.emEthStatus = ETH_IDLE;
    // 复位ETH
    RCC->BitAHB1RSTR.ETHMACRST = 1;
    RCC->BitAHB1RSTR.ETHMACRST = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // 复位DMA模块
    ETH->BitDMABMR.SR = 1;
    dwTimOut = 0;
    while((ETH->BitDMABMR.SR == 1) && (dwTimOut ++ < PHY_WRITE_READ_TIME_OUT) ) {;}
    if(dwTimOut >= PHY_WRITE_READ_TIME_OUT) {
        s_stEth.emEthStatus = ETH_NO_PHY;
        return;
    }

    DevRccGetClocksFreq(&stRccFreq);
    dwHclk = stRccFreq.HCLK;

    /* Set CR bits depending on hclk value */
    if((dwHclk >= 20000000) && (dwHclk < 35000000)) ETH->BitMACMIIAR.CR = 2;         // 16
    else if((dwHclk >= 35000000) && (dwHclk < 60000000)) ETH->BitMACMIIAR.CR = 3;    // 26
    else if((dwHclk >= 60000000)&&(dwHclk < 100000000)) ETH->BitMACMIIAR.CR = 0;     // 42
    else if((dwHclk >= 100000000)&&(dwHclk < 150000000)) ETH->BitMACMIIAR.CR = 1;    // 62
    else ETH->BitMACMIIAR.CR = 4;                                                    // 102

    if(s_stEth.pstInterface->IsLinked) {
        if(s_stEth.pstInterface->Reset) s_stEth.pstInterface->Reset();
        while(s_stEth.pstInterface->IsLinked() == false && (dwTimOut ++ < PHY_WRITE_READ_TIME_OUT) ) {;}
        if(dwTimOut >= PHY_WRITE_READ_TIME_OUT) {
            // phy 未连接网线，不能协商网络参数
            s_stEth.emEthStatus = ETH_UNLINK_UNINIT;
            return;
        }

        if(dwTimOut != PHY_WRITE_READ_TIME_OUT) {
            // 设置自动协商
            if(s_stEth.pstInterface->RestartAutoNegotiation) {
                if(s_stEth.pstInterface->SetAutoNegotiation()) goto error;
            }

            if(s_stEth.pstInterface->IsHispeed) {
                if(s_stEth.pstInterface->IsHispeed () == false) s_stEth.emSpeed = LOW_SPD;
                else s_stEth.emSpeed = HI_SPD;
            }

            if(s_stEth.pstInterface->IsFullDuplex) {
                if(s_stEth.pstInterface->IsFullDuplex () == false) s_stEth.emDuplex = HALF_DUPLEX;
                else s_stEth.emDuplex = FULL_DUPLEX;
            }
        }
    }

error:

    // 配置 MAC配置寄存器
    ETH->BitMACCR.APCS = 0;     // 去除自动 pad/CRC (Automatic pad/CRC stripping)
    ETH->BitMACCR.DM = (s_stEth.emDuplex == FULL_DUPLEX)?1:0;       // 全双工
    ETH->BitMACCR.LM = 0;       // 禁止回环
    ETH->BitMACCR.FES = (s_stEth.emSpeed == HI_SPD)?1:0;            // 快速以太网速度 (Fast Ethernet speed) 100Mhz
    ETH->BitMACCR.IFG = 7;      // 这些位控制发送期间帧间的最小间隙, 40位时间
    ETH->BitMACCR.JD = 1;       // 启动 Jabber
    ETH->BitMACCR.WD = 1;       // 启动看门狗
    ETH->BitMACCR.CSTF = 0;
    ETH->BitMACCR.ROD = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // 以太网 MAC VLAN 标记寄存器 (ETH_MACVLANTR)

    // macffr 以太网 MAC 帧过滤寄存器 (ETH_MACFFR)
    ETH->BitMACFFR.RA = 0;      // 接收所有 (Receive all)

    ETH->BitMACFFR.SAF = 0;     // 源地址过滤器 (Source address filter) 复位
    ETH->BitMACFFR.SAIF = 0;
    ETH->BitMACFFR.BFD = 0;     // 允许广播帧 (Broadcast frames disable)
    ETH->BitMACFFR.PAM = 0;     // 传送所有多播 (Pass all multicast) 禁止
    ETH->BitMACFFR.DAIF = 0;
    ETH->BitMACFFR.HM = 0;      // MAC 对多播帧执行完美目标地址过滤
    ETH->BitMACFFR.HU = 0;      // MAC 对单播帧执行完美目标地址过滤
    ETH->BitMACFFR.PM = 0;      // 禁止混乱模式
    ETH->BitMACFFR.HPF = 0;     // 地址过滤器传送与完美过滤或散列过滤匹配的帧
    ETH->BitMACFFR.PCF = 1;     // MAC 将除了暂停控制帧以外的所有控制帧转发到应用程序

    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // MACFCG 寄存器
    ETH->BitMACFCR.PT = 0;       // 暂停时间 (Pause time)
    ETH->BitMACFCR.ZQPD = 1;     // 零时间片暂停禁止 (Zero-quanta pause disable)
    ETH->BitMACFCR.PLT = 0;      // 暂停时间减去 4 个时隙
    ETH->BitMACFCR.UPFD = 0;
    ETH->BitMACFCR.RFCE = 0;
    ETH->BitMACFCR.TFCE = 0;
    ETH->BitMACFCR.FCB_BPA = 0;  // 流控制忙碌/背压激活 (Flow control busy/back pressure activate) 禁止
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // VLAN 虚拟网
    ETH->BitMACVLANTR.VLANTC = 0;
    ETH->BitMACVLANTR.VLANTI = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // DMA 工作模式 DMAOMR
    ETH->BitDMAOMR.DTCEFD = 1;  // 禁止丢弃 TCP/IP 校验和错误帧
    ETH->BitDMAOMR.RSF = 1;     // 接收存储并转发
    ETH->BitDMAOMR.DFRF = 0;    // 禁止刷新接收帧 (Disable flushing of received frames)
    ETH->BitDMAOMR.TSF = 1;     // 发送存储并转发 (Transmit store and forward)
    ETH->BitDMAOMR.TTC = 0;     // 发送阈值控制 (Transmit threshold control) 64
    ETH->BitDMAOMR.FEF = 0;     // 转发错误帧 (Forward error frame) 禁止
    ETH->BitDMAOMR.FUGF = 0;    // 转发过小的好帧 (Forward undersized good frame) 禁止
    ETH->BitDMAOMR.RTC2 = 0;     // 接收阈值控制 (Receive threshold control) 64
    ETH->BitDMAOMR.OSF = 0;     // 处理第二个帧 (Operate on second frame) 禁止
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // DMA BMR 总线模式寄存器 (ETH_DMABMR)
    ETH->BitDMABMR.MB = 0;      // 禁止丢弃 TCP/IP 校验和错误帧
    ETH->BitDMABMR.FB = 1;      // 固定突发
    ETH->BitDMABMR.AAB = 1;     // 地址对齐的节拍 (Address-aligned beats)
    ETH->BitDMABMR.RDP = 32;    // Rx DMA PBL 32
    ETH->BitDMABMR.PBL = 32;    // Tx DMA PBL 32
    ETH->BitDMABMR.PM = 1;      // Rx Tx 优先级比 (Rx Tx priority ratio) 2:1
    ETH->BitDMABMR.EDFE = 0;
    ETH->BitDMABMR.DSL = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // 中断配置
    ETH->BitDMAIER.RIE = 1;         // 接收中断
    // ETH->BitDMAIER.TIE = 1;         // 发送中断
    ETH->BitDMAIER.NISE = 1;        // 中断使能

    s_stEth.emEthStatus = ETH_LINK_NO_START;
    // 向phy注册外部中断接口
    if(s_stEth.pstInterface->GetEthIntHandlerSet) s_stEth.pstInterface->GetEthIntHandlerSet(PhyIntHandler);
}

/*******************************************************************************
函数功能：
    @ 开启can设备需要的时钟
参数描述：
    @ 无
返回值描述：
    @ -1 ： 失败
    @  0 ： 成功
说明：
    @ 无。
*******************************************************************************/
static void RccInit(void)
{
    RCC->BitAHB1ENR.ETHMACEN = 1;
    RCC->BitAHB1ENR.ETHMACRXEN = 1;
    RCC->BitAHB1ENR.ETHMACTXEN = 1;
    RCC->BitAHB1ENR.ETHMACPTPEN = 1;
}

/*******************************************************************************
函数功能：
    @ 设置can中断
参数描述：
    @ 无
返回值描述：
    @ -1 ： 失败
    @  0 ： 成功
说明：
    @ 无。
*******************************************************************************/
static void NvicInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;

    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
函数功能：
*    @ 写phy
参数描述：
*    @ 无
返回值描述：
*    @ 寄存器的值
说明：
*    @ 无。
*******************************************************************************/
static int WritePhy(unsigned char PHYAddr, unsigned char PHYReg, short unsigned int PHYValue)
{
    unsigned int dwTimOut = 0;

    if(s_stEth.emEthStatus == ETH_NO_PHY) return -1;

    ETH->BitMACMIIAR.PA5 = PHYAddr;
    ETH->BitMACMIIAR.MR = PHYReg;
    ETH->BitMACMIIAR.MW = 1;
    ETH->BitMACMIIAR.MB = 1;

    do { dwTimOut ++; } while ((ETH->BitMACMIIAR.MB == 1) && (dwTimOut < (uint32_t)PHY_WRITE_READ_TIME_OUT));

    if(dwTimOut == PHY_WRITE_READ_TIME_OUT) return -1;
    return 0;
}

/*******************************************************************************
函数功能：
*    @ 读取phy
参数描述：
*    @ 无
返回值描述：
*    @ 寄存器的值
说明：
*    @ 无。
*******************************************************************************/
static short unsigned int ReadPhy(unsigned char PHYAddr, unsigned char byReg)
{
    unsigned int dwTimOut = 0;

    if(s_stEth.emEthStatus == ETH_NO_PHY) return -1;

    ETH->BitMACMIIAR.PA5 = PHYAddr;
    ETH->BitMACMIIAR.MR = byReg & 0x1f;
    ETH->BitMACMIIAR.MW = 0;
    ETH->BitMACMIIAR.MB = 1;

    do { dwTimOut ++; } while ((ETH->BitMACMIIAR.MB == 1) && (dwTimOut < (uint32_t)PHY_WRITE_READ_TIME_OUT));

    if(dwTimOut == PHY_WRITE_READ_TIME_OUT) return 0;
    return (short unsigned int) ETH->BitMACMIIDR.MD;
}

/*******************************************************************************
函数功能：
*    @ 配置MAC地址
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static int MACAddressConfig(const unsigned char *pbyAddr)
{
    ETH->MACADDR[0].BitMACAxHR.MACA_5 = pbyAddr[5];
    ETH->MACADDR[0].BitMACAxHR.MACA_4 = pbyAddr[4];

    ETH->MACADDR[0].BitMACAxLR.MACA_3 = pbyAddr[3];
    ETH->MACADDR[0].BitMACAxLR.MACA_2 = pbyAddr[2];
    ETH->MACADDR[0].BitMACAxLR.MACA_1 = pbyAddr[1];
    ETH->MACADDR[0].BitMACAxLR.MACA_0 = pbyAddr[0];

    return 0;
}

/*------------------------------- DMA 操作部分 -------------------------------*/
/*******************************************************************************
函数功能：
    @ 配置 DMA TX 描述符
参数描述：
    @ 无
返回值描述：
    @ -1 ： 失败
    @  0 ： 成功
说明：
    @ 无。
*******************************************************************************/
static void DMADescChainInit( void )
{
    EthDmaTxDescFrameTdf *DMATxDesc;
    EthDmaRxDescFrameTdf *DMARxDesc;

    // 初始化操作指针
    s_stDMA.pstFrameRx = s_stDMA.DMARxDescTab;
    // s_stDMA.pstFrameTx = s_stDMA.DMATxDescTab;
    for(int i = 0; i < CONFIG_ETH_TXBUFNB; i ++) {
        DMATxDesc = s_stDMA.DMATxDescTab + i;
        DMATxDesc->TDES0 = 0;
        DMATxDesc->TDES1 = 0;
        DMATxDesc->TBAP1 = 0;
        DMATxDesc->TBAP2 = 0;

        DMATxDesc->BitTDES0.TCH = 1; // 使用环形链表
        DMATxDesc->TBAP1 = (unsigned int)s_stDMA.Tx_Buff[i];

        if(i < (CONFIG_ETH_TXBUFNB - 1)) {
            DMATxDesc->TBAP2 = (unsigned int)(s_stDMA.DMATxDescTab + i + 1);
        }
        else {
            DMATxDesc->TBAP2 = (unsigned int) s_stDMA.DMATxDescTab;
            DMATxDesc->BitTDES0.TER = 1;
        }
    }

    // 接收环处理
    for(int i = 0; i < CONFIG_ETH_RXBUFNB ; i ++) {
        DMARxDesc = s_stDMA.DMARxDescTab + i;
        DMARxDesc->RDES0 = 0;
        DMARxDesc->RDES1 = 0;
        DMARxDesc->RBAP1 = 0;
        DMARxDesc->RBAP2 = 0;

        DMARxDesc->BitRDES0.OWN = 1; // DMA 占有
        DMARxDesc->BitRDES1.RCH = 1;
        DMARxDesc->BitRDES1.RBS1 = CONFIG_ETH_RX_BUF_SIZE;
        DMARxDesc->RBAP1 = (unsigned int)s_stDMA.Rx_Buff[i];

        if(i < (CONFIG_ETH_RXBUFNB - 1)) {
            DMARxDesc->RBAP2 = (unsigned int)(s_stDMA.DMARxDescTab + i + 1);
        }
        else {
            DMARxDesc->RBAP2 = (unsigned int)(s_stDMA.DMARxDescTab);
            DMARxDesc->BitRDES1.RER = 1;
        }
    }

    /* Set Transmit Desciptor List Address Register */
    ETH->DMATDLAR = (unsigned int) s_stDMA.DMATxDescTab;
    ETH->DMARDLAR = (unsigned int) s_stDMA.DMARxDescTab;
}

/*******************************************************************************
函数功能：
*   @ 取得ETH使用权限
参数描述：
*   @ 无
返回值描述：
*   @ false： 未取得
*   @ true ： 取得
说明：
*   @ 无。
*******************************************************************************/
static bool GetLock(void)
{
    return DevIo->CommLock(&s_stEth.dwLock, false);
}

/*******************************************************************************
函数功能：
*   @ 退出使用权
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static void GiveLock(void)
{
    DevIo->CommUnLock(&s_stEth.dwLock);
}

/*******************************************************************************
函数功能：
*   @ 发送以太网数据
参数描述：
*   @ unsigned char *pbyIn, 数据输入地址
*   @ unsigned int dwLength 输入数据长度
*   @ int (*pfDatInHadler)(void *pvIN, unsigned char *pbyInDat, unsigned int dwLenLimt)：这是非安全的写法，将数据拷贝暴露，
*   @ 没有办法，主要是提升速度，少数据拷贝，回掉应用进行数据拷贝，主要是对于lwip的块内存不能memcpy
*   @ void *pvIN 回掉参数
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static int Send(unsigned char *pbyIn, unsigned int dwLength,
                short unsigned int (*pfDatInHadler)(void *pvIN, unsigned char *pbyInDat, unsigned int dwLenLimt), void *pvIN)
{
    unsigned int dwDmaReg = ETH->DMACHTDR;
    unsigned char *pbyBuffer = NULL;
    //__IO EthDmaTxDescFrameTdf *pstTxDesc = s_stDMA.pstFrameTx;
    __IO EthDmaTxDescFrameTdf *pstTxDesc = (__IO EthDmaTxDescFrameTdf *)dwDmaReg;

    if(pbyIn) if(dwLength == 0) return -1;
    if(dwLength > CONFIG_ETH_TX_BUF_SIZE) return -3;
    if( s_stEth.emEthStatus != ETH_LINK ) return -2;

    if(pstTxDesc->BitTDES0.OWN == 1) {
        // DMA 占用,查找到可以使用的数据缓冲。
        for(int i = 0; i < CONFIG_ETH_TXBUFNB; i ++) {
            pstTxDesc = (EthDmaTxDescFrameTdf *)pstTxDesc->TBAP2;
            if(pstTxDesc->BitTDES0.OWN == 0) break;
            if(i == (CONFIG_ETH_TXBUFNB - 1)) return -1;
        }
    }

    pbyBuffer = (u8 *)(pstTxDesc->TBAP1);
    if(pbyIn) memcpy(pbyBuffer, pbyIn, dwLength);
    else {
        if(pfDatInHadler) dwLength = pfDatInHadler(pvIN, pbyBuffer, CONFIG_ETH_TX_BUF_SIZE);
    }

    pstTxDesc->BitTDES0.FS = 1;
    pstTxDesc->BitTDES0.LS = 1;

    pstTxDesc->BitTDES1.TBS1 = dwLength;
    pstTxDesc->BitTDES0.OWN = 1;
    pstTxDesc->BitTDES0.TER = 1;
    // 下一个TX
    // pstTxDesc = (EthDmaTxDescFrameTdf *)pstTxDesc->TBAP2;

    if(ETH->BitDMASR.TBUS == 1 || ETH->BitDMASR.TPS == 0 || ETH->BitDMASR.TPS == 6) {
        ETH->BitDMASR.TBUS = 0;
        ETH->DMATPDR = 0;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 接收以太网数据，并通知上层应用
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static int ReceiveFrameAndNoticeUp(void)
{
    // unsigned int dwDmaReg = ETH->DMACHRDR;
    unsigned int dwLen = 0;
    s_stEth.dwRxOff = 0;
    // __IO EthDmaRxDescFrameTdf *pstRxDesc = (__IO EthDmaRxDescFrameTdf *)dwDmaReg;
    // __IO EthDmaRxDescFrameTdf *pstRxDesc = s_stDMA.pstFrameRx;

    /* scan descriptors owned by CPU */
    for(int i = 0;  ; i ++ , s_stDMA.pstFrameRx = (EthDmaRxDescFrameTdf *)s_stDMA.pstFrameRx->RBAP2) {
        //if(i != 0 && pstRxDesc == (__IO EthDmaRxDescFrameTdf *)(dwDmaReg)) break;
        //s_stDMA.pstFrameRx = pstRxDesc;

        if(s_stDMA.pstFrameRx->BitRDES0.OWN == 1) {
            break;
        }

        if(s_stDMA.pstFrameRx->BitRDES0.AFM == 1 || s_stDMA.pstFrameRx->BitRDES0.SAF == 1 && s_stDMA.pstFrameRx->BitRDES0.ES == 1) {
            s_stDMA.pstFrameRx->BitRDES0.OWN = 1;
            continue;
        }

        if(s_stDMA.pstFrameRx->BitRDES0.FS == 1 && s_stDMA.pstFrameRx->BitRDES0.LS == 0) {
            // 头帧
            memcpy( s_stEth.abyRxFrame, (const unsigned char* ) s_stDMA.pstFrameRx->RBAP1, CONFIG_ETH_RX_BUF_SIZE);
            s_stEth.dwRxOff = CONFIG_ETH_RX_BUF_SIZE;
            s_stDMA.pstFrameRx->BitRDES0.OWN = 1;
        }
        else if(s_stDMA.pstFrameRx->BitRDES0.FS == 0 && s_stDMA.pstFrameRx->BitRDES0.LS == 0) {
            memcpy( s_stEth.abyRxFrame + s_stEth.dwRxOff, (const unsigned char* ) s_stDMA.pstFrameRx->RBAP1, CONFIG_ETH_RX_BUF_SIZE);
            s_stEth.dwRxOff += CONFIG_ETH_RX_BUF_SIZE;
            s_stDMA.pstFrameRx->BitRDES0.OWN = 1;
        }
        else {
            if(s_stDMA.pstFrameRx->BitRDES0.FS == 1) s_stEth.dwRxOff = 0;
            memcpy( s_stEth.abyRxFrame + s_stEth.dwRxOff, (const unsigned char* ) s_stDMA.pstFrameRx->RBAP1, CONFIG_ETH_RX_BUF_SIZE);
            s_stDMA.pstFrameRx->BitRDES0.OWN = 1;

            dwLen = s_stDMA.pstFrameRx->BitRDES0.FL;
            if( s_stEth.pfReciveHandler != NULL) s_stEth.pfReciveHandler(s_stEth.abyRxFrame , dwLen - 4);

            if(ETH->BitDMASR.RBUS == 1) {
                ETH->BitDMASR.RBUS = 1;
                ETH->DMARPDR = 0;
            }
        }
    }

    if(ETH->BitDMASR.RBUS == 1) {
        ETH->BitDMASR.RBUS = 1;
        ETH->DMARPDR = 0;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 注册接收到数据后，驱动通知上层应用的接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int SetRxHandler(void (*pfHandler) (const unsigned char *pbyDat, unsigned int byLen))
{
    s_stEth.pfReciveHandler = pfHandler;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 注册接收到数据后，驱动通知上层应用的接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int SetStartHandler(void (*pfHandler) (void))
{
    s_stEth.pfEthStartHandler = pfHandler;
    return 0;
}

/*******************************************************************************
函数功能：
*    @ 驱动eth数据传输
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static int EthStart(const unsigned char *pbyAddr)
{
    if(!pbyAddr) return -1;

    DMADescChainInit();
    MACAddressConfig(pbyAddr);

    if(s_stEth.emDuplex == HALF_DUPLEX) {
        ETH->BitMACFCR.FCB_BPA = 1;
        for(__IO int i = 0; i < 0xfff; i ++) {;}
    }

    // MAC trs start
    ETH->BitMACCR.TE = 1;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // MAC rx start
    ETH->BitMACCR.RE = 1;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // Set the Flush Transmit FIFO bit
    ETH->BitDMAOMR.FTF = 1;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // Enable the DMA transmission
    ETH->BitDMAOMR.ST = 1;
    // Enable the DMA reception
    ETH->BitDMAOMR.SR = 1;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    s_stEth.emEthStatus = ETH_LINK;
    return 0;
}

/*******************************************************************************
函数功能：
*    @ 停止设备
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static void EthDown(void)
{
    if(s_stEth.emDuplex == HALF_DUPLEX) {
        ETH->BitMACFCR.FCB_BPA = 0;
        for(__IO int i = 0; i < 0xfff; i ++) {;}
    }

    // MAC trs start
    ETH->BitMACCR.TE = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // MAC rx start
    ETH->BitMACCR.RE = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // Set the Flush Transmit FIFO bit
    ETH->BitDMAOMR.FTF = 1;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    // Enable the DMA transmission
    ETH->BitDMAOMR.ST = 0;
    // Enable the DMA reception
    ETH->BitDMAOMR.SR = 0;
    for(__IO int i = 0; i < 0xfff; i ++) {;}

    s_stEth.emEthStatus = ETH_DOWN;
}

/*******************************************************************************
函数功能：
*    @ 启动设备
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static void EthUp(void)
{
    // 使用服务，将设备启动配置序列
    s_stEth.emEthStatus = ETH_UNLINK_UNINIT;
}

/*******************************************************************************
函数功能：
*    @ 启动设备
参数描述：
*    @ 无
返回值描述：
*    @ 无
说明：
*    @ 无。
*******************************************************************************/
static void SetMac(const unsigned char *pbyMac)
{
    // 使用服务，将设备启动配置序列
    for(int i = 0; i < 6; i ++) s_stEth.abyMac[i] = pbyMac[i];
}

/*******************************************************************************
函数功能：
*   @ 获取phy是否连接正常
参数描述：
*   @ 无
返回值描述：
*   @ false:连接异常
*   @ true: 连接正常
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static bool IsLinked(void)
{
    if( s_stEth.emEthStatus == ETH_LINK) return true;
    return false;
}

/*******************************************************************************
函数功能：
*   @ 接收数据管理
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static bool Receive(void)
{
    // if(ETH->BitDMASR.RS == 1) return false;
    if(s_stEth.dwRxCnt == 0) return false;

    ReceiveFrameAndNoticeUp();

    do { __asm("cpsie i"); __asm("cpsie f");} while(0);
    s_stEth.dwRxCnt = 0;
    do { __asm("cpsie i"); __asm("cpsie f");} while(0);

    return true;
}

/*******************************************************************************
函数功能：
*   @ eth 管理服务
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static void Service(void)
{
    bool blLink = true;

    if(s_stEth.emEthStatus == ETH_NO_PHY) return;
    if(s_stEth.emEthStatus == ETH_DOWN) return;

    if(s_stEth.pstInterface->GetEthIntHandlerSet == NULL) {
        // 如果phy的驱动没有断开中断驱动
        // 通过配置的查询接口进行断开的检查
        if(s_stEth.pstInterface->IsLinked) {
            blLink = s_stEth.pstInterface->IsLinked ();
       //     if(blLink == false ) s_stEth.emEthStatus = ETH_UNLINK_UNINIT;
        }
    }

//    if(blLink == true && s_stEth.emEthStatus == ETH_UNLINK_UNINIT) {
//        s_stEth.dwRxCnt = 0;
//        ConfigMac();
//    }

    if(s_stEth.emEthStatus == ETH_LINK_NO_START) {
        if(s_stEth.pfEthStartHandler) s_stEth.pfEthStartHandler();
        EthStart(s_stEth.abyMac);
    }

    if(blLink == true && s_stEth.emEthStatus == ETH_LINK) {
        if(ETH->BitDMASR.RBUS == 1) {
            //do { __asm("cpsie i"); __asm("cpsie f");} while(0);
            ReceiveFrameAndNoticeUp();
            //do { __asm("cpsie i"); __asm("cpsie f");} while(0);
        }
    }
}

/*******************************************************************************
函数功能：
*   @ eth的中断服务函数，用于通知上层数据
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
void ETH_IRQHandler(void)
{
    if(ETH->BitDMASR.RS == 1) {
        // ReceiveFrameAndNoticeUp();
        s_stEth.dwRxCnt ++;

        ETH->BitDMASR.RS = 1;
        ETH->BitDMASR.NIS = 1;
    }
}

static const DevEthTdf s_Dev = {
    SetPhyInterface,
    ConfigMac,
    WritePhy,
    ReadPhy,

    EthStart,

    GetLock,
    GiveLock,
    Send,
    SetRxHandler,
    SetStartHandler,
    Service,

    EthDown,
    EthUp,
    SetMac,
    IsLinked,
    Receive
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
const DevEthTdf *EthInit(void)
{
    memset ((u8*)&s_stDMA, 0, sizeof(s_stDMA) );
    memset ((u8*)&s_stEth, 0, sizeof(s_stEth) );
    // 配置 RMII
    SYSCFG->BitPMC.MII_RMII_SEL = 1;
    RccInit();
    NvicInit();

    return &s_Dev;
}
#endif