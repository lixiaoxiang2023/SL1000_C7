/*******************************************************************************
文件名称：  protcom.c
文件目录：  $PROJ_DIR$\..\code\com
项目名称：  事业一部技术平台
创建者：    xuqiang
创建日期：  2018-03-14
程序用途：  建立串口通用控制协议平台，可用于串口通信设备
版权声明：  Copyright urit Co.,Ltd.All Right Reserved

日期：2018-03-14 胥强
设计

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_COM_PROTOCOL)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define PROTCOM_CHN_SEND_FIFO_NUM               3
#define PROTCOM_CHN_READ_FRAME_DAT_BUFF         4

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
Local Variables Definition 内部全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
typedef enum
{
	IDLE,
	GET_DATA_LENTH,
	GET_CRC1,
	GET_DATA,
	GET_CRC2,
}RX_ST;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static struct {
    unsigned char byUsedChn;

    struct stProtCmoChn {
        unsigned char byDevNO;

        RX_ST stState;                 //用于数据接收的状态机
        unsigned short int wRxLen;     //用于数据接受协议的数据长度存储
        unsigned short int wRxCnt;     //用于数据接受的计数器
        unsigned short int wMaxIoDatLen;

        unsigned char byCrc;            //用于数据接受协议的CRC计算
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
        unsigned char byDatLenCnt;
#endif
        struct stSendFifo {
            unsigned char byaFrame[PROTCOM_CHN_READ_FRAME_DAT_BUFF];
            unsigned char *pcString;
            unsigned short int wLen;
            unsigned char byFrameLen;
            bool blNeedSend;
            void (*pfProtSendEnd) (void);
        }staFifo[PROTCOM_CHN_SEND_FIFO_NUM];

        unsigned char *pbyRx;
        unsigned char *pbyIo;

        int (*SendString) (unsigned char *pcIn, unsigned int dwLen);
    } stChn[CONFIG_COM_PROTOCOL_CHN_NUM];
} s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 取得协同通道的控制数据
参数描述：
*   @ unsigned int dwDev：注册的设备地址
返回值描述：
*   @ 控制数据结构体
说明：
*   @ 无
*******************************************************************************/
static struct stProtCmoChn *GetChnCtr(unsigned int dwDev)
{
    bool blFindChn = false;
    struct stProtCmoChn *pstChn = s_stCtr.stChn;

    for(int i = 0 ; i < CONFIG_COM_PROTOCOL_CHN_NUM ; i ++ ) {
        if(pstChn->byDevNO == dwDev) {
            blFindChn = true;
            break;
        }
    }

    return (blFindChn)?pstChn:NULL;
}

/*******************************************************************************
函数功能：
*   @ 向下位机写入数据
参数描述：
*   @ unsigned char bySAddr：下位机资源的起始地址
*   @ unsigned char *pbyIn：数据
*   @ unsigned char byLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ProtAddTrs(unsigned int dwChnDev, unsigned char *pcFrame, unsigned char byFrameLen, unsigned char *pbyIn, unsigned short int wLen, void ( pfProtSendEnd ) (void))
{
    struct stProtCmoChn *pstChn = GetChnCtr(dwChnDev);
    struct stSendFifo *pstFifo;
    bool blSendOk = false;

    if(!pstChn) return -1;

    pstFifo = pstChn->staFifo;
    for(int i = 0; i < PROTCOM_CHN_SEND_FIFO_NUM; i ++, pstFifo ++) {
        __ASM("cpsid i");// 关中断
        __ASM("cpsid f");// 关异常

        if(pstFifo->blNeedSend) {
            __ASM("cpsie i");//开
            __ASM("cpsie f");
            continue;
        }

        pstFifo->blNeedSend = true;
        __ASM("cpsie i");//开
        __ASM("cpsie f");

        memset(pstFifo->byaFrame, 0, PROTCOM_CHN_READ_FRAME_DAT_BUFF);
        memcpy(pstFifo->byaFrame, pcFrame, byFrameLen);

        pstFifo->pcString   = pbyIn;
        pstFifo->wLen       = wLen;
        pstFifo->byFrameLen = byFrameLen;
        if(pfProtSendEnd)   pstFifo->pfProtSendEnd = pfProtSendEnd;
        blSendOk            = true;
        break;
    }

    if(blSendOk) return 0;
    return -1;
}

/*******************************************************************************
函数功能：
*   @ 读取下位机数据
参数描述：
*   @ unsigned char bySAddr：下位机资源的起始地址
*   @ unsigned char bySAddr：起始地址
*   @ unsigned short int wLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int Read(unsigned int dwChnDev, unsigned char bySAddr, unsigned short int wLen)
{
    unsigned char *pbyCpy = 0;
    unsigned char byaFrame[6];
    unsigned char byFrameLen = 3;

    // 发送资源
    memset( byaFrame, 0, 6);
    pbyCpy = byaFrame;

    *pbyCpy ++ = 0;
    *pbyCpy ++ = bySAddr;
    *pbyCpy ++ = (wLen & 0xff);
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
    *pbyCpy ++ = wLen >> 8;
    byFrameLen = 4;
#endif

    return ProtAddTrs(dwChnDev, byaFrame, byFrameLen, NULL, 0, NULL);
}

/*******************************************************************************
函数功能：
*   @ 向下位机写入数据
参数描述：
*   @ unsigned char bySAddr：下位机资源的起始地址
*   @ unsigned char *pbyIn：数据
*   @ unsigned char byLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int Write(unsigned int dwChnDev, unsigned char bySAddr, unsigned char *pbyIn, unsigned short int wLen, void (*pfProtSendEnd) (void))
{
    unsigned char byaFrame[6];
    unsigned char byFrameLen = 3;
    unsigned char *pbyCpy = 0;

    memset( byaFrame, 0, 6);
    pbyCpy = byaFrame;

    *pbyCpy ++ = 1;
    *pbyCpy ++ = bySAddr;
    *pbyCpy ++ = (wLen & 0xff);
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
    *pbyCpy ++ = wLen >> 8;
    byFrameLen = 4;
#endif

    return ProtAddTrs(dwChnDev, byaFrame, byFrameLen, pbyIn, wLen, pfProtSendEnd);
}

/*******************************************************************************
函数功能：
*   @ 执行数据
参数描述：
*   @ unsigned char bySAddr：下位机资源的起始地址
*   @ unsigned char *pbyIn：数据
*   @ unsigned char byLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SendExc( struct stProtCmoChn *pstChn, struct stSendFifo *pstFifo)
{
    unsigned short int wLen;
    unsigned char byaFrame[6];
    unsigned char byFrameLen = 2;
    unsigned char *pbyCpy = 0;
    unsigned char crc = 0;

    wLen = pstFifo->wLen + pstFifo->byFrameLen;
    //发送协议头
    memset(byaFrame, 0, 6);
    pbyCpy = byaFrame;
    *pbyCpy ++ = 0xfe;
    *pbyCpy ++ = (wLen & 0xff);
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
    *pbyCpy ++ = (wLen >> 8);
    byFrameLen = 3;
#endif

    // 计算 帧头的 和校验
    for(int i = 0; i < byFrameLen; i ++) crc += byaFrame[i];
    *pbyCpy = crc;

    crc = 0;
    for(int i = 0; i < pstFifo->byFrameLen; i ++) crc += pstFifo->byaFrame[i];
    for(int i = 0; i < pstFifo->wLen; i ++) crc += pstFifo->pcString[i];

    pstChn->SendString(byaFrame, byFrameLen + 1);
    pstChn->SendString(pstFifo->byaFrame, pstFifo->byFrameLen);
    pstChn->SendString(pstFifo->pcString, pstFifo->wLen);

    // 数据校验
    byaFrame[0] = crc; byaFrame[1] = 0;
    pstChn->SendString(byaFrame, 1);
}

/*******************************************************************************
函数功能：
*   @ 协议服务，主进程需要重复执行此服务，已到达数据发送
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void ProtComService(void)
{
    struct stProtCmoChn *pstChn = NULL;
    struct stSendFifo *pstFifo;

    pstChn = s_stCtr.stChn;

    for(int chn = 0; chn < CONFIG_COM_PROTOCOL_CHN_NUM; chn ++, pstChn ++) {
        pstFifo = pstChn->staFifo;

        for(int fifo = 0 ; fifo < PROTCOM_CHN_SEND_FIFO_NUM ; fifo ++, pstFifo ++) {
            if(!pstFifo->blNeedSend) continue;
            SendExc(pstChn, pstFifo);
            pstFifo->blNeedSend = false;
            if(pstFifo->pfProtSendEnd) pstFifo->pfProtSendEnd();
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 接收并解析数据包数据
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 用于从数据中提取数据命令
*******************************************************************************/
static void GetDat(unsigned int dwChnDev)
{
    struct stProtCmoChn *pstChn = GetChnCtr(dwChnDev);
    unsigned char *pbyDatIn;
    unsigned short int wRequireLen = 0;
    bool blRd = 0;
    unsigned char byAddr = 0;

    if(!pstChn) return;

    blRd = (pstChn->pbyRx[0] == 0x00)?true:false;
    byAddr = pstChn->pbyRx[1];
    wRequireLen = (unsigned short int)pstChn->pbyRx[2];
    pbyDatIn = &pstChn->pbyRx[3];
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
    wRequireLen += ((unsigned short int)pstChn->pbyRx[3] << 8);
    pbyDatIn = &pstChn->pbyRx[4];
#endif

    if((byAddr + wRequireLen) > pstChn->wMaxIoDatLen) return;

    if(blRd) Write(dwChnDev, byAddr, pstChn->pbyIo + byAddr, wRequireLen, NULL);     // 读取
    else memcpy( ((unsigned char*)(pstChn->pbyIo) + byAddr), pbyDatIn, wRequireLen); // 写入
}

/*******************************************************************************
函数功能：
*   @ 处理串口驱动收到的数据
参数描述：
*   @ 串口驱动传入的数据
返回值描述：
*   @ 无
说明：
*   @ 需要较短，不能太长，否者将会影响串口的数据接收。
*******************************************************************************/

static void ProtCmoCorePro( unsigned int dwDev, unsigned char byIn)
{
    struct stProtCmoChn *pstChn = GetChnCtr(dwDev);
    if(!pstChn) return;

    switch(pstChn->stState) {
    case IDLE:
        if(byIn == 0xfe) {
            pstChn->stState = GET_DATA_LENTH;
            pstChn->byCrc = 0xfe;
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
            pstChn->byDatLenCnt = 0;
#endif
        }
        return;

    case GET_DATA_LENTH:
#if(CONFIG_COM_PROTOCOL_DATA_LEN_2BYTE)
        if(pstChn->byDatLenCnt == 0) {
            pstChn->wRxLen = byIn;
        }
        else if(pstChn->byDatLenCnt == 1) {
            pstChn->wRxLen += ((unsigned short int)byIn << 8);
            pstChn->stState = GET_CRC1;
        }
        pstChn->byCrc += byIn;
        pstChn->byDatLenCnt ++;
#else
        pstChn->wRxLen = byIn;
        pstChn->byCrc += byIn;
        pstChn->stState = GET_CRC1;
#endif
        return;

    case GET_CRC1:
        if(byIn == pstChn->byCrc) {
            pstChn->wRxCnt = 0;
            pstChn->stState = GET_DATA;
            pstChn->byCrc = 0;
        }
        else pstChn->stState = IDLE;
        return;

    case GET_DATA:
        if(pstChn->wRxCnt < pstChn->wMaxIoDatLen) {
            pstChn->byCrc += byIn;
            pstChn->pbyRx[pstChn->wRxCnt] = byIn;
            pstChn->wRxCnt ++;

            if(pstChn->wRxCnt == pstChn->wRxLen) {
                pstChn->stState = GET_CRC2;
            }
        }
        else {
            pstChn->stState = IDLE;
        }

        return;

    case GET_CRC2:
        if(byIn == pstChn->byCrc) GetDat(dwDev);
        pstChn->stState = IDLE;
        return;

    default :
        pstChn->stState = IDLE;
        return;
    }
}

/*******************************************************************************
函数功能：
*   @ 向协议增加传输通道
参数描述：
*   @ 无
返回值描述：
*   @ 0 增加成功
*   @ -1 增加失败
说明：
*   @
*******************************************************************************/
static int AddChn(stProtComChnInitTdf *pstChnInit)
{
    if(s_stCtr.byUsedChn >= CONFIG_COM_PROTOCOL_CHN_NUM) return -1;
    struct stProtCmoChn *pstChn = &s_stCtr.stChn[s_stCtr.byUsedChn];

    pstChn->byDevNO         = pstChnInit->byDevNO;
    pstChn->wMaxIoDatLen    = pstChnInit->wMaxIoDatLen;
    pstChn->pbyRx           = pstChnInit->pbyRxBuff;
    pstChn->pbyIo           = pstChnInit->pbyIo;
    pstChn->SendString      = pstChnInit->SendString;
    return 0;
}

static const ProtComTdf s_Dev = {
    AddChn,
    Read,
    Write,
    ProtComService,
    ProtCmoCorePro
};

/*******************************************************************************
函数功能：
*   @ 初始化打印机驱动
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
const ProtComTdf *ProtComInit(void)
{
    memset(&s_stCtr, 0, sizeof(s_stCtr));

    return &s_Dev;
}

#endif