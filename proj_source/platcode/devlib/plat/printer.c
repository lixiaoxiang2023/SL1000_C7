/*******************************************************************************
文件名称：  printr.c
文件目录：  $PROJ_DIR$\..\code\dev\plat
项目名称：  事业一部技术平台
创建者：    陆作沛
创建日期：  2013-04-10
程序用途：  根据串口协议驱动685打印机模块
版权声明：  Copyright urit Co.,Ltd.All Right Reserved

日期： 2013-04-11 陆作沛
第一次修改：完成编程格式规范

日期：2016-05-12 胥强
加入技术平台

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_PRINTER)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

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
    bool blUseProtol;

    unsigned char byUartSn;

	RX_ST stState;               //用于数据接收的状态机
	unsigned char byRxLen;     //用于数据接受协议的数据长度存储
	unsigned char byCrc;       //用于数据接受协议的CRC计算
	unsigned char byRxCnt;     //用于数据接受的计数器

    /*用于数据打印部分参数*/
    unsigned int dwLock;
    bool blHasPaper;

    unsigned char byPrtErr;
    unsigned char byaRx[TX_BUF_SIZE];
    unsigned char byaTx[TX_BUF_SIZE + 8];
}s_stCtr;

static stPrinterIoTdf s_stIo;
/*******************************************************************************
内部函数申明
*******************************************************************************/

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
static void Write(unsigned char bySAddr, unsigned char *pbyIn, unsigned char byLen)
{
    unsigned char *pbyCpy = 0;
    unsigned char crc = 0;

    DevIo->CommLock(&s_stCtr.dwLock, true);

    pbyCpy = s_stCtr.byaTx;
    *pbyCpy ++ = 0xfe;          //发送协议头
    *pbyCpy ++ = byLen + 3;
    *pbyCpy ++ = (unsigned char)( byLen + 0xfe + 3);
    *pbyCpy ++ = 1;
    *pbyCpy ++ = bySAddr;
    *pbyCpy ++ = byLen;

    crc += 1; crc += bySAddr; crc += byLen;
    for(int i = 0; i < byLen; i ++ , pbyIn ++) {
        *pbyCpy ++ = *pbyIn;
        crc += *pbyIn;
    }

    *pbyCpy ++ = crc;
    s_stCtr.byPrtErr ++;
#if(CONFIG_PRINTER_COM_DMA)
    DevUart->SendStringDma(s_stCtr.byUartSn, s_stCtr.byaTx, byLen + 7);
#else
    DevUart->SendString(s_stCtr.byUartSn, s_stCtr.byaTx, byLen + 7);
#endif
    DevIo->CommUnLock(&s_stCtr.dwLock);
}

/*******************************************************************************
函数功能：
*   @ 读取下位机数据
参数描述：
*   @ unsigned char bySAddr：下位机资源的起始地址
*   @ unsigned char *pbyIn：数据
*   @ unsigned char byLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void Read(unsigned char bySAddr, unsigned char byLen)
{
    unsigned char *pbyCpy = 0;
    unsigned char crc = 0;

    DevIo->CommLock(&s_stCtr.dwLock, true);

    pbyCpy = s_stCtr.byaTx;
    *pbyCpy ++ = 0xfe;//发送协议头
    *pbyCpy ++ = 3;
    *pbyCpy ++ = (unsigned char)( 0xfe + 3);
    *pbyCpy ++ = 0;
    *pbyCpy ++ = bySAddr;
    *pbyCpy ++ = byLen;

    crc += bySAddr; crc += byLen;
    *pbyCpy ++ = crc;

    s_stCtr.byPrtErr ++;
#if(CONFIG_PRINTER_COM_DMA)
    DevUart->SendStringDma(s_stCtr.byUartSn, s_stCtr.byaTx, 7);
#else
    DevUart->SendString(s_stCtr.byUartSn, s_stCtr.byaTx, 7);
#endif
    DevIo->CommUnLock(&s_stCtr.dwLock);
}

/*******************************************************************************
函数功能：
*   @ 读取下位机数据
参数描述：
*   @ unsigned char bySAddr：下位机资源的起始地址
*   @ unsigned char *pbyIn：数据
*   @ unsigned char byLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void SetPrtLev(unsigned char byPrtLev)
{
    Write(4, &byPrtLev, 1);
}

/*******************************************************************************
函数功能：
*   @ 读取打印机是否有纸
参数描述：
*   @ 无
返回值描述：
*   @ true：有纸
*   @ false：无
说明：
*   @ 无
*******************************************************************************/
static bool HasPaper(void)
{
    if(s_stCtr.blUseProtol) {
        if(s_stIo.byHasPaper) s_stCtr.blHasPaper = false;
        else s_stCtr.blHasPaper = true;
    }

    return s_stCtr.blHasPaper;
}

/*******************************************************************************
函数功能：
*   @ 向打印机写命令
参数描述：
*   @ unsigned char byCmd：命令编码
返回值描述：
*   @ 0：成功
*   @ -1 ：失败
说明：
*   @ 无
*******************************************************************************/
static int WriteCmd(unsigned char byCmd)
{
    unsigned int dwTimOut = 0xfffff;
    unsigned char byaCmd[2];
    unsigned char byCmdSta = s_stIo.byCmdReg;

    if(!s_stCtr.blUseProtol) return -1;

    byaCmd[0] = s_stIo.byCmdCnt + 1;
    byaCmd[1] = byCmd;

    Write(0, byaCmd, 2);
    while(byCmdSta == s_stIo.byCmdReg) {
        if (--dwTimOut == 0) return -1;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 进纸
参数描述：
*   @ 无
返回值描述：
*   @ 0：成功
*   @ -1 ：失败
说明：
*   @ 无
*******************************************************************************/
static int FeedPaper(void)
{
    return WriteCmd(1);
}

/*******************************************************************************
函数功能：
*   @ 打印测试信息
参数描述：
*   @ 无
返回值描述：
*   @ 0：成功
*   @ -1 ：失败
说明：
*   @ 无
*******************************************************************************/
static int PrtTest(void)
{
    return WriteCmd(2);
}

/*******************************************************************************
函数功能：
*   @ 升级功能
参数描述：
*   @ 无
返回值描述：
*   @ 0：成功
*   @ -1 ：失败
说明：
*   @ 无
*******************************************************************************/
static int UpDate(void)
{
    return WriteCmd(3);
}

/*******************************************************************************
函数功能：
*   @ 打印机打印机个数计数器
参数描述：
*   @ 无
返回值描述：
*   @ 打印机打印机个数计数器
说明：
*   @ 无
*******************************************************************************/
static int PrintCnt(void)
{
    return s_stIo.byPrintCnt;
}

/*******************************************************************************
函数功能：
*   @ 取得打印机版本
参数描述：
*   @ 数据地址，需要4个字节
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void Version(unsigned char *pbyOut)
{
    if(!pbyOut) return;
    memcpy(pbyOut, s_stIo.byaVer, 4);
}

/*******************************************************************************
函数功能：
*   @ 判断打印机是否连接
参数描述：
*   @ 无
返回值描述：
*   @ true：打印机连接，false：打印机断开
说明：
*   @ 在每次执行对打印机进行数据传输时，都会将s_stCtr.byPrtErr 加1，当任意接收到
1次数据后此数据被清零，当错误大于2时判断为打印机没有数据返回，连接断开
*******************************************************************************/
static bool IsConnect(void)
{
    if(s_stCtr.byPrtErr > 4) return false;
    return true;
}

/*******************************************************************************
函数功能：
*   @ 打开、关闭协议功能
参数描述：
*   @ true：使用协议
*   @ false：不使用协议
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void UseProtol(bool blCmd)
{
    unsigned char tmp[2];

    if(blCmd) {
        tmp[0] = 0;
        s_stCtr.blUseProtol = true;
    }
    else {
        tmp[0] = 1;
        s_stCtr.blUseProtol = false;
    }

    Write(5, tmp, 1);
}

/*******************************************************************************
函数功能：
*   @ 打印机数据
参数描述：
*   @ unsigned char *pbyIn ：数据地址。
*   @ unsigned int dwLen：数据长度
*   @ bool blIsWait：是否等待打印完成
返回值描述：
*   @ 非-1：打印机打印机个数计数器
*   @ -1 ：失败
说明：
*   @ 无
*******************************************************************************/
static int Print(unsigned char *pbyIn, unsigned int dwLen, bool blIsWait)
{
    unsigned int dwTimOut = 0xffffff;
    unsigned char byCmdSta = s_stIo.byPrintCnt;

    DevIo->CommLock(&s_stCtr.dwLock, true);
#if(CONFIG_PRINTER_COM_DMA)
    DevUart->SendStringDma(s_stCtr.byUartSn, pbyIn, dwLen);
#else
    DevUart->SendString(s_stCtr.byUartSn, pbyIn, dwLen);
#endif
    DevIo->CommUnLock(&s_stCtr.dwLock);

    if(blIsWait) {
        while(byCmdSta == s_stIo.byPrintCnt) {
            if (--dwTimOut == 0) return -1;
            if(!HasPaper()) return -1;
            if(!IsConnect()) return -2;
        }
    }

    return 0;
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
static void GetDat(void)
{
    unsigned char *pbyCpy;
    unsigned char byLen = 0;

    pbyCpy = (unsigned char*)(&s_stIo);
    pbyCpy = pbyCpy + s_stCtr.byaRx[1] + s_stCtr.byaRx[2];
    if(pbyCpy > (((unsigned char*)(&s_stIo)) + sizeof(s_stIo))) return;
    byLen = s_stCtr.byaRx[2];

    s_stCtr.byPrtErr = 0;
    if(s_stCtr.byaRx[0] == 0x00) { //读取打印板
        pbyCpy = ((unsigned char*)(&s_stIo)) + s_stCtr.byaRx[1];
        Write(s_stCtr.byaRx[1], pbyCpy, byLen);
    }
    else if(s_stCtr.byaRx[0] == 0x01) {//写入打印板
        pbyCpy = &s_stCtr.byaRx[3];
        memcpy( (unsigned char*)(&s_stIo) + s_stCtr.byaRx[1], pbyCpy, byLen);//数据长度
    }
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
static void CorePro(unsigned char byIn)
{
    if(!s_stCtr.blUseProtol) {
        if(byIn == 0xA5) s_stCtr.blHasPaper = false;
        if(byIn == 0xff) s_stCtr.blHasPaper = true;

        return ;
    }

    switch(s_stCtr.stState) {
    case IDLE:
        if(byIn == 0xfe) {
            s_stCtr.stState = GET_DATA_LENTH;
        }

        return;

    case GET_DATA_LENTH:
        s_stCtr.byCrc = s_stCtr.byRxLen = byIn;
        s_stCtr.byCrc += 0xfe;
        s_stCtr.stState = GET_CRC1;
        return;

    case GET_CRC1:
        if(byIn == s_stCtr.byCrc) {
            s_stCtr.byRxCnt = 0;
            s_stCtr.stState = GET_DATA;
            s_stCtr.byCrc = 0;
        }
        else s_stCtr.stState = IDLE;
        return;

    case GET_DATA:
        if(s_stCtr.byRxCnt < TX_BUF_SIZE) {
            s_stCtr.byCrc += byIn;
            s_stCtr.byaRx[s_stCtr.byRxCnt] = byIn;
            s_stCtr.byRxCnt ++;

            if(s_stCtr.byRxCnt == s_stCtr.byRxLen) {
                s_stCtr.stState = GET_CRC2;
            }
        }
        else {
            s_stCtr.stState = IDLE;
        }

        return;

    case GET_CRC2:
        if(byIn == s_stCtr.byCrc) GetDat();
        s_stCtr.stState = IDLE;
        return;

    default :
        s_stCtr.stState = IDLE;
        return;
    }
}

static const DevPrinterTdf s_stDev = {
    Read,
    Write,
    SetPrtLev,
    HasPaper,
    FeedPaper,
    PrtTest,
    UpDate,
    Print,
    PrintCnt,
    Version,
    IsConnect,
    UseProtol,
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
const DevPrinterTdf *PrinterInit(stPrinterInitTdf *pstInit)
{
    memset(&s_stCtr, 0, sizeof(s_stCtr));
    memset(&s_stIo, 0, sizeof(stPrinterIoTdf));

    s_stCtr.byUartSn = pstInit->byUartSn;
    s_stCtr.blUseProtol = pstInit->blUseProtol;

    DevUart->Config(s_stCtr.byUartSn, 9600, 1, 0, true);
#if(CONFIG_PRINTER_COM_DMA)
    DevUart->ConfigDma(s_stCtr.byUartSn, pstInit->byUartDmaSn, pstInit->byUartDmaChn);
#endif
    DevUart->RegHandler(s_stCtr.byUartSn,CorePro);
    UseProtol(s_stCtr.blUseProtol);

    return &s_stDev;
}

#endif