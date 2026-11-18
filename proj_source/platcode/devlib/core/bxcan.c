/*******************************************************************************
文件名称：   bxcan.c
文件目录：   $PROJ_DIR$/../code/xcanlib
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.15
程序用途：   can驱动程序,用于发送和接收数据

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

#if(CONFIG_CAN1|CONFIG_CAN2)

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define INIT_MODE                       0
#define NORMAL_MODE                     1
#define SLEEP_MODE                      2
#define SILENT_MODE                     3
#define LOOP_BACK_MODE                  4
#define LOOP_BACK_COMBINED_MODE         5

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
static struct BxcanBuff {
    unsigned int wId;
    unsigned char  byLen;
    unsigned char abyErr[3];
    unsigned char  abyDat[8];

    unsigned char byHwID;

    void (*pfHandler) ( unsigned int dwDevID, unsigned int dwId, unsigned char *pbyDat, unsigned int dwLen);
    void (*pfTxHandler) (unsigned int dwDev);
} s_stRx[CONFIG_CAN1 + CONFIG_CAN2];

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 基本设置
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static void BaseInit(void)
{
#if(CONFIG_CAN1)
    CAN1->MCR = 0;
    CAN1->BitMCR.TTCM = 0;
    CAN1->BitMCR.AWUM = 1;              //睡眠模式通过检测CAN报文，由硬件自动唤醒。唤醒的同时，硬件自动对CAN_MSR寄存器的SLEEP和SLAK位清’0’
    CAN1->BitMCR.NART = 0;              //使能报文自动重传功能
    CAN1->BitMCR.RFLM = 1;              //接收FIFO锁定模式
    CAN1->BitMCR.TXFP = 1;              //优先级由发送请求的顺序来决定
    CAN1->BitMCR.ABOM = 1;              //一旦硬件检测到128次11位连续的隐性位，则自动退出离线状态。
#endif

#if(CONFIG_CAN2)
    CAN2->MCR = 0;
    CAN2->BitMCR.TTCM = 0;
    CAN2->BitMCR.AWUM = 1;              //睡眠模式通过检测CAN报文，由硬件自动唤醒。唤醒的同时，硬件自动对CAN_MSR寄存器的SLEEP和SLAK位清’0’
    CAN2->BitMCR.NART = 0;              //使能报文自动重传功能
    CAN2->BitMCR.RFLM = 1;              //接收FIFO锁定模式
    CAN2->BitMCR.TXFP = 1;              //优先级由发送请求的顺序来决定
    CAN2->BitMCR.ABOM = 1;              //一旦硬件检测到128次11位连续的隐性位，则自动退出离线状态。
#endif
}

/*******************************************************************************
函数功能：
*   @ 开启can设备需要的时钟
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static void RccInit(void)
{
#if(PLAT_STM32_F1)
    RCC->BitAPB1ENR.CANEN = 1;
#endif

#if(PLAT_STM32_F2)
#if(CONFIG_CAN1)
    RCC->BitAPB1ENR.CAN1EN = 1;
#endif

#if(CONFIG_CAN2)
    // can2 为can1的从设备，使用can2必须启动can1的时钟
    RCC->BitAPB1ENR.CAN1EN = 1;
    RCC->BitAPB1ENR.CAN2EN = 1;
#endif
#endif

#if(PLAT_STM32_F4)
#if(CONFIG_CAN1)
    RCC->BitAPB1ENR.CAN1EN = 1;
#endif

#if(CONFIG_CAN2)
    RCC->BitAPB1ENR.CAN1EN = 1;
    RCC->BitAPB1ENR.CAN2EN = 1;
#endif
#endif
}

/*******************************************************************************
函数功能：
*   @ 设置can中断
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static void NvicInit(void)
{
    /*
    * USB_HP_CAN1_TX_IRQn         = 19 //using
    * USB_LP_CAN1_RX0_IRQn        = 20 //using
    * CAN1_RX1_IRQn               = 21 //using
    * CAN1_SCE_IRQn               = 22 //unusing
    */

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

#if(CONFIG_CAN1)
#if(PLAT_STM32_F1)
    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
#endif

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;
#endif

    NVIC_Init(&NVIC_InitStructure);

#if(PLAT_STM32_F1)
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
#endif

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
#endif
    NVIC_Init(&NVIC_InitStructure);

    // 接收1中断
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
    NVIC_Init(&NVIC_InitStructure);
#endif


#if(CONFIG_CAN2)
#if(PLAT_STM32_F1)
    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN2_TX_IRQn;
#endif

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;
#endif

    // 发送中断
    NVIC_Init(&NVIC_InitStructure);

  #if(PLAT_STM32_F1)
      NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN2_RX0_IRQn;
  #endif

  #if(PLAT_STM32_F2|PLAT_STM32_F4)
      NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
  #endif

    // 接收0中断
    NVIC_Init(&NVIC_InitStructure);

    // 接收1中断
    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    // 错误中断
    NVIC_InitStructure.NVIC_IRQChannel = CAN2_SCE_IRQn;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/*******************************************************************************
函数功能：
*   @ 取得设备
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static CAN_TypeDef *GetDevice(unsigned int deDev)
{
    if(deDev >= (CONFIG_CAN1 + CONFIG_CAN2)) return NULL;

#if((CONFIG_CAN1==1)&&(CONFIG_CAN2==1))
    if(deDev == 0) return CAN1;
    if(deDev == 1) return CAN2;
    return NULL;
#endif

#if((CONFIG_CAN1==1)&&(CONFIG_CAN2==0))
    return CAN1;
#endif

#if((CONFIG_CAN1==0)&&(CONFIG_CAN2==1))
    return CAN2;
#endif
}

/*******************************************************************************
函数功能：
*   @ 取得设备操作需要数据缓冲
参数描述：
*   @ 无
返回值描述：
*   @ null ： 失败
说明：
*   @ 无。
*******************************************************************************/
static struct BxcanBuff *GetCtrBuff(unsigned int deDev)
{
    if(deDev >= (CONFIG_CAN1 + CONFIG_CAN2)) return NULL;

#if((CONFIG_CAN1==1)&&(CONFIG_CAN2==1))
    if(deDev == 1) return &s_stRx[1];
#endif

    return &s_stRx[0];
}

/*******************************************************************************
函数功能：
*   @ 复位can设备
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static int Reset(unsigned int dwDev)
{
    CAN_TypeDef *pstCan = GetDevice(dwDev);
    if(pstCan == NULL) return -1;

    pstCan->BitMCR.RESET = 1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置can模式
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 无。
*******************************************************************************/
static int SetMode(unsigned int dwDev, unsigned char byMode)
{
    CAN_TypeDef *pstCan = GetDevice(dwDev);
    if(pstCan == NULL) return -1;

    if(byMode > LOOP_BACK_COMBINED_MODE) return -1;

    switch (byMode) {
    case INIT_MODE:
        pstCan->BitMCR.SLEEP = 0;
        // while (pstCan->BitMSR.SLAK) {;}
        pstCan->BitMCR.INRQ = 1;
        while (!pstCan->BitMSR.INAK) {;}
        break;

    case NORMAL_MODE:
        pstCan->BitMCR.SLEEP = 0;
        while (pstCan->BitMSR.SLAK) {;}
        pstCan->BitMCR.INRQ = 0;
        // while (pstCan->BitMSR.INAK) {;}
        break;

    case SLEEP_MODE:
        pstCan->BitMCR.INRQ = 0;
        while (pstCan->BitMSR.INAK) {;}
        pstCan->BitMCR.SLEEP = 1;
        while (!pstCan->BitMSR.SLAK) {;}
        break;

    case SILENT_MODE:
        pstCan->BitMCR.SLEEP = 0;
        while (pstCan->BitMSR.SLAK) {;}
        pstCan->BitMCR.INRQ = 1;
        while (!pstCan->BitMSR.INAK){;}
        pstCan->BitBTR.SILM = 1;
        pstCan->BitMCR.INRQ = 0;
        while (pstCan->BitMSR.INAK) {;}
        break;

    case LOOP_BACK_MODE:
        pstCan->BitMCR.SLEEP = 0;
        while (pstCan->BitMSR.SLAK) {;}
        pstCan->BitMCR.INRQ = 1;
        while (!pstCan->BitMSR.INAK) {;}
        pstCan->BitBTR.LBKM = 1;
        pstCan->BitMCR.INRQ = 0;

        while (pstCan->BitMSR.INAK) {;}
        break;
    default:
        break;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置波特率
参数描述：
*   @ 无
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ lixx 关于采样时间配置思路参考 cia官网 标准  波特率 默认500K 采样点位置默认 >80%
The sample point should be the same in all connected CAN nodes. However,
in practice it is sufficient if the sample points don't vary too much.
The automotive industry has specified some recommendations for the bit timing settings in the SAE J2284 series.
For CANopen networks, the recommendations given in CiA 301 should be used. 
The sample points should be at 87,5 percent of the bit time. 
For most of the specified bit-rates a range of 85 percent to 90 percent is allowed. 
Of course, the larger the tolerance range, the shorter the maximum possible length of the network。

*******************************************************************************/
static int SetBaud(unsigned dwDev, unsigned int dwBaud)
{
    ClocksTypeDef stRccFreq;
    unsigned char brp = 0;
    unsigned char tq = 0;


#define SJW_TQ 1

#ifndef CONFIG_CAN_TS2_TQ
    #define CONFIG_CAN_TS2_TQ 2
#endif

#if(PLAT_STM32_F1||PLAT_STM32_F2)
     //stRccFreq.PCLK1  36M
    #ifndef CONFIG_CAN_TS1_TQ
        #define CONFIG_CAN_TS1_TQ 9
    #endif

#endif

#if(PLAT_STM32_F4)
    #if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx)
        //stRccFreq.PCLK1  45M
        #ifndef CONFIG_CAN_TS1_TQ
            #define CONFIG_CAN_TS1_TQ 15
        #endif
    #else
        //stRccFreq.PCLK1  42M
        #ifndef CONFIG_CAN_TS1_TQ
            #define CONFIG_CAN_TS1_TQ 11
        #endif
    #endif
#endif

    CAN_TypeDef *pstCan = GetDevice(dwDev);
    if(pstCan == NULL) return -1;
    DevRccGetClocksFreq(&stRccFreq);
    
    tq = (1 + CONFIG_CAN_TS1_TQ + CONFIG_CAN_TS2_TQ); 
    brp = stRccFreq.PCLK1 / (tq * dwBaud) - 1;

    pstCan->BTR = 0;
    pstCan->BitBTR.SJW = SJW_TQ; 
    pstCan->BitBTR.TS2 = CONFIG_CAN_TS2_TQ - 1;
    pstCan->BitBTR.TS1 = CONFIG_CAN_TS1_TQ - 1;
    pstCan->BitBTR.BRP = brp; 

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置过滤器接口
参数描述：
*   @ unsigned char byDId ： 需要进行通信的设备地址
*   @ unsigned char bySId ： 本机地址
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ CAN2 共享CAN1的过滤配置部分，设置can2默认过滤配置位14开始。
*******************************************************************************/
static int AddFilter(unsigned int dwDev, unsigned short int wFilterID, unsigned int dwFilterLen)
{
    unsigned int dwFilterStart = 0;
    int i = 0;
    unsigned int wdVal = 0;
    unsigned int wdValFa1r = 0;
    unsigned int dwFA1R;

#if((CONFIG_CAN1==1)&&(CONFIG_CAN2==1))
    if(dwDev == 0) dwFilterStart = 0;
    else dwFilterStart = 14;                // CAN2从滤波器14开始配置
#elif(CONFIG_CAN2==1)
    dwFilterStart = 14;
#endif

    if(dwFilterLen > 29) return -1;

    dwFA1R = CAN1->FA1R;
    dwFA1R >>= dwFilterStart;
    for(i = 0; i < 28; i ++) {
        if((dwFA1R & 0x01) == 0) break;
        dwFA1R >>= 1;
    }

    i += dwFilterStart;
    if(i > 26) return -1;

    CAN1->BitFMR.FINIT = 1;        // 进入初始化过滤器模式
    wdValFa1r = CAN1->FA1R;        // 记录开始的配置情况
    CAN1->FA1R = 0;

    CAN1->FM1R = 0;                // mode 全部工作在位屏蔽模式
    CAN1->FS1R |= (1 << i);        // scale 过滤器位宽为单个32位

    wdVal = CAN1->FFA1R;           // fifo assignment
    wdVal &= 0x7ffffff;
    wdVal |= (unsigned int) ((i % 2) << i);
    CAN1->FFA1R = wdVal;

    wdVal = wFilterID;
    wdVal <<= (32 - dwFilterLen);
    CAN1->sFilterRegister[i].FR1 = wdVal;              // 设置ID

    wdVal = 0x0;
    for(int i = 0; i < dwFilterLen ; i ++) {
        wdVal |= 0x80000000;
        if(i < (dwFilterLen-1) )
        {
            wdVal >>= 1;
        }
    }

    CAN1->sFilterRegister[i].FR2 = wdVal;         // 设置屏蔽位,高8位全屏蔽,只接受发送给自己的数据包

    //恢复使能位
    wdValFa1r |= (1 << i);
    CAN1->FA1R = wdValFa1r;
    CAN1->BitFMR.FINIT = 0;        // 退出初始化过滤器模式

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 发送一帧的数据
参数描述：
*   @ unsigned int dwId ： 帧ID
*   @ unsigned char *pbyIn ： 数据输入的地址
*   @ unsigned char byLen ： 数据长度，不能大于8
返回值描述：
*   @ -1 ： 失败
*   @  0 ： 成功
说明：
*   @ 发送一个数据包，先判断是否有空闲的fifo，有的话开始发送。
*******************************************************************************/
static int SendOne(unsigned int dwDev, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen)
{
    unsigned char dwFifoNo;
    CAN_TypeDef *pstCan = GetDevice(dwDev);
    if(pstCan == NULL) return -1;

    if(dwLen > 8) dwLen = 8;

    if (pstCan->BitTSR.TME0 == 1) {
        dwFifoNo = 0;
        pstCan->BitTSR.RQCP0 = 1;
    }
    else if (pstCan->BitTSR.TME1 == 1) {
        dwFifoNo = 1;
        pstCan->BitTSR.RQCP1 = 1;
    }
    else if (pstCan->BitTSR.TME2 == 1) {
        dwFifoNo = 2;
        pstCan->BitTSR.RQCP2 = 1;
    }
    else return -1;

    dwId &= 0xfffffff8;
    pstCan->sTxMailBox[dwFifoNo].TIR = dwId;
    pstCan->sTxMailBox[dwFifoNo].BitTIR.RTR = 0;
    pstCan->sTxMailBox[dwFifoNo].BitTIR.IDE = 1;
    pstCan->sTxMailBox[dwFifoNo].BitTDTR.DLC = dwLen;

    //加载数据
    pstCan->sTxMailBox[dwFifoNo].TDLR = *(unsigned int *)pbyIn;
    pstCan->sTxMailBox[dwFifoNo].TDHR = *(unsigned int *)(pbyIn + 4);
    pstCan->sTxMailBox[dwFifoNo].BitTIR.TXRQ = 1;

	return 0;
}

/*******************************************************************************
函数功能：
*   @ 查询空的fifo数量
参数描述：
*   @ 无
返回值描述：
*   @ 空闲的fifo个数
说明：
*   @ 无。
*******************************************************************************/
static int GetEmpNum(unsigned int dwDev)
{
	unsigned int dwSendFifo = 0;
    CAN_TypeDef *pstCan = GetDevice(dwDev);

    if(pstCan == NULL) return -1;
    if (pstCan->BitTSR.TME0 == 1) dwSendFifo ++;
    if (pstCan->BitTSR.TME1 == 1) dwSendFifo ++;
    if (pstCan->BitTSR.TME2 == 1) dwSendFifo ++;

    //加载数据
	return dwSendFifo;
}

/*******************************************************************************
函数功能：
*   @ 开启发送中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 在连续传输数据时需要。
*******************************************************************************/
static int SendIrpOn(unsigned int dwDev)
{
    CAN_TypeDef *pstCan = GetDevice(dwDev);

    if(pstCan == NULL) return -1;

    pstCan->BitIER.TMEIE = 1;
	return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭发送中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int SendIrpOff(unsigned int dwDev)
{
    CAN_TypeDef *pstCan = GetDevice(dwDev);

    if(pstCan == NULL) return -1;

    pstCan->BitIER.TMEIE = 0;
	return 0;
}

/*******************************************************************************
函数功能：
*   @ 从can硬件层读取数据，并将ID，数据长度，数据缓存
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int GetMsg(CAN_TypeDef *pstCan, struct BxcanBuff *pstRx, unsigned int dwFifoNo)
{
    unsigned char byLen = 0;

    if(pstCan == NULL) return -1;
    if(pstRx == NULL) return -1;

    if (dwFifoNo > 2) return -1;
    if (pstCan->sFIFOMailBox[dwFifoNo].BitRIR.IDE == 1) {
        pstRx->wId = pstCan->sFIFOMailBox[dwFifoNo].RIR;
    }
    else return -1;//禁止使用标准协议，必须使用扩展协议

    byLen = pstCan->sFIFOMailBox[dwFifoNo].BitRDTR.DLC;
    pstRx->byLen = byLen;

    memcpy(pstRx->abyDat, (unsigned char*)&pstCan->sFIFOMailBox[dwFifoNo].RDLR, byLen);
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
static int SetRxHandler( unsigned int dwDev, void (*pfHandler) (unsigned int dwDevID, unsigned int dwId, unsigned char *pbyDat, unsigned int dwLen))
{
    struct BxcanBuff *pstRx = GetCtrBuff(dwDev);
    if(pstRx == NULL) return -1;

    pstRx->pfHandler = pfHandler;
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
static int SetTxHandler( unsigned int dwDev, void (*pfHandler) (unsigned int dwDev))
{
    struct BxcanBuff *pstRx = GetCtrBuff(dwDev);
    if(pstRx == NULL) return -1;

    pstRx->pfTxHandler = pfHandler;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置模块的地址
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 对设备进行地址的设定，默认为0。
*******************************************************************************/
static int SetHwID( unsigned int dwDev, unsigned char byID)
{
    struct BxcanBuff *pstRx = GetCtrBuff(dwDev);
    if(pstRx == NULL) return -1;

    pstRx->byHwID = byID;
    return 0;
}

#if(CONFIG_CAN1)
/*******************************************************************************
函数功能：
*   @ can1的fifo0接收数据处理中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
#if(PLAT_STM32_F1)
void USB_LP_CAN1_RX0_IRQHandler(void)
#else
void CAN1_RX0_IRQHandler(void)
#endif
{
    struct BxcanBuff *pstRx = GetCtrBuff(0);

    while (CAN1->BitRF0R.FMP0 != 0) {
        GetMsg(CAN1, pstRx, 0);
        CAN1->BitRF0R.RFOM0 = 1; //释放一个数据包
        if(pstRx->pfHandler) pstRx->pfHandler(pstRx->byHwID, pstRx->wId, pstRx->abyDat, pstRx->byLen);
    }
}

/*******************************************************************************
函数功能：
*   @ can1的fifo1接收数据处理
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
void CAN1_RX1_IRQHandler(void)
{
    struct BxcanBuff *pstRx = GetCtrBuff(0);

    while (CAN1->BitRF1R.FMP1 != 0) {
        GetMsg(CAN1, pstRx, 1);
        CAN1->BitRF1R.RFOM1 = 1; //释放一个数据包
        if(pstRx->pfHandler) pstRx->pfHandler( pstRx->byHwID, pstRx->wId, pstRx->abyDat, pstRx->byLen);
    }
}

/*******************************************************************************
函数功能：
*   @ can1的发送中断处理函数接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
#if(PLAT_STM32_F1)
void USB_HP_CAN1_TX_IRQHandler(void)
#else
void CAN1_TX_IRQHandler(void)
#endif
{
    struct BxcanBuff *pstRx = GetCtrBuff(0);
    if(pstRx->pfTxHandler) pstRx->pfTxHandler (pstRx->byHwID);
}

/*******************************************************************************
函数功能：
*   @ can1的错误中断处理
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
void CAN1_SCE_IRQHandler(void)
{
    struct BxcanBuff *pstRx = GetCtrBuff(0);

    CAN1->BitMSR.ERRI = 0;
    if(CAN1->BitTSR.TERR0) {
        pstRx->abyErr[0] ++;
        if(pstRx->abyErr[0] > 3) {
            CAN1->BitTSR.ABRQ0 = 1;
            CAN1->BitTSR.RQCP0 = 1;
            pstRx->abyErr[0] = 0;
        }
    }
    if(CAN1->BitTSR.TERR1) {
        pstRx->abyErr[1] ++;
        if(pstRx->abyErr[1] > 3) {
            CAN1->BitTSR.ABRQ1 = 1;
            CAN1->BitTSR.RQCP1 = 1;
            pstRx->abyErr[1] = 0;
        }
    }
    if(CAN1->BitTSR.TERR2) {
        pstRx->abyErr[2] ++;
        if(pstRx->abyErr[2] > 3) {
            CAN1->BitTSR.ABRQ2 = 1;
            CAN1->BitTSR.RQCP2 = 1;
            pstRx->abyErr[2] = 0;
        }
    }
}
#endif

#if(CONFIG_CAN2)
/*******************************************************************************
函数功能：
*   @ can1的fifo0接收数据处理中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
#if(PLAT_STM32_F1)
void USB_LP_CAN2_RX0_IRQHandler(void)
#else
void CAN2_RX0_IRQHandler(void)
#endif
{
    struct BxcanBuff *pstRx = GetCtrBuff(1);

    while (CAN2->BitRF0R.FMP0 != 0) {
        GetMsg(CAN2, pstRx, 0);
        CAN2->BitRF0R.RFOM0 = 1; //释放一个数据包
        if(pstRx->pfHandler) pstRx->pfHandler(pstRx->byHwID, pstRx->wId, pstRx->abyDat, pstRx->byLen);
    }
}

/*******************************************************************************
函数功能：
*   @ can1的fifo1接收数据处理
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
void CAN2_RX1_IRQHandler(void)
{
    struct BxcanBuff *pstRx = GetCtrBuff(1);

    while (CAN2->BitRF1R.FMP1 != 0) {
        GetMsg(CAN2, pstRx, 1);
        CAN2->BitRF1R.RFOM1 = 1; //释放一个数据包
        if(pstRx->pfHandler) pstRx->pfHandler( pstRx->byHwID, pstRx->wId, pstRx->abyDat, pstRx->byLen);
    }
}

/*******************************************************************************
函数功能：
*   @ can1的发送中断处理函数接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
#if(PLAT_STM32_F1)
void USB_HP_CAN2_TX_IRQHandler(void)
#else
void CAN2_TX_IRQHandler(void)
#endif
{
    struct BxcanBuff *pstRx = GetCtrBuff(1);

    if(pstRx->pfTxHandler) pstRx->pfTxHandler (pstRx->byHwID);
}

/*******************************************************************************
函数功能：
    can1的错误中断处理
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
void CAN2_SCE_IRQHandler(void)
{
    struct BxcanBuff *pstRx = GetCtrBuff(1);

    CAN2->BitMSR.ERRI = 0;
    if(CAN2->BitTSR.TERR0) {
        pstRx->abyErr[0] ++;
        if(pstRx->abyErr[0] > 3) {
            CAN2->BitTSR.ABRQ0 = 1;
            CAN2->BitTSR.RQCP0 = 1;
            pstRx->abyErr[0] = 0;
        }
    }
    if(CAN2->BitTSR.TERR1) {
        pstRx->abyErr[1] ++;
        if(pstRx->abyErr[1] > 3) {
            CAN2->BitTSR.ABRQ1 = 1;
            CAN2->BitTSR.RQCP1 = 1;
            pstRx->abyErr[1] = 0;
        }
    }
    if(CAN2->BitTSR.TERR2) {
        pstRx->abyErr[2] ++;
        if(pstRx->abyErr[2] > 3) {
            CAN2->BitTSR.ABRQ2 = 1;
            CAN2->BitTSR.RQCP2 = 1;
            pstRx->abyErr[2] = 0;
        }
    }
}

#endif

static const DevCanTdf s_Dev = {
    AddFilter,
    SendOne,
    GetEmpNum,
    SendIrpOn,
    SendIrpOff,
    SetRxHandler,
    SetTxHandler,
    SetHwID
};

/*******************************************************************************
函数功能：
*   @ 初始化can 设备, whth boud
参数描述：
*   @ 无
返回值描述：
*   @ can设备操作方法集合
说明：
*   @ 无。
*******************************************************************************/
const DevCanTdf *CanInit2(unsigned int dwBoud)
{
    CAN_TypeDef *pstCan = NULL;
    RccInit();

    for(int dev = 0; dev < CONFIG_CAN1 + CONFIG_CAN2; dev ++) {
        Reset(dev);
        BaseInit();
        pstCan = GetDevice(dev);
        //使能相应的中断
        pstCan->BitIER.FMPIE0 = 1; //FIFO 0消息挂号中断使能
        pstCan->BitIER.FMPIE1 = 1; //FIFO 1消息挂号中断使能
        pstCan->BitIER.TMEIE  = 0;
        pstCan->BitIER.ERRIE  = 0; //错误中断使能

        SetMode( dev, INIT_MODE);
        SetBaud( dev, dwBoud);
        SetMode( dev, NORMAL_MODE);
    }

    // can1 can2 公用过滤部分，can2的起始地址默认位14.
    CAN1->BitFMR.CAN2SB = 14;

    //配置完后再开中断，否则可能会有异常数据占用中断资源 20210519 16:00 lixx
    NvicInit();

    memset((unsigned char*) &s_stRx, 0, sizeof(s_stRx));
    return &s_Dev;
}

/*******************************************************************************
函数功能：
*   @ 初始化can 设备
参数描述：
*   @ 无
返回值描述：
*   @ can设备操作方法集合
说明：
*   @ 无。
*******************************************************************************/
const DevCanTdf *CanInit(void)
{
    return CanInit2(CONFIG_CAN_BOUD);
}

#endif
