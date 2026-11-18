/*******************************************************************************
文件名称：   at45.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20151123
程序用途：

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20151123：
第一次由XQ创建初始程序。
2016.03.12：
整理成驱动平台，at45将调用通用spi接口
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_AT45)
/*******************************************************************************
常量定义
*******************************************************************************/
#define AT45_PAGE_READ              0xD2
#define AT45_CONTINUOUS_READ_LEG    0xE8
#define AT45_CONTINUOUS_READ_LF     0x03
#define AT45_CONTINUOUS_READ        0x0B
#define AT45_BUF1_READ_LF           0xD1
#define AT45_BUF2_READ_LF           0xD3
#define AT45_BUF1_READ_SER          0xD4
#define AT45_BUF2_READ_SER          0xD6
#define AT45_BUF1_READ_8B           0x54
#define AT45_BUF2_READ_8B           0x56

#define AT45_BUF1_WRITE             0x84
#define AT45_BUF2_WRITE             0x87
#define AT45_BUF1_MEM_ERASE         0x83
#define AT45_BUF2_MEM_ERASE         0x86
#define AT45_BUF1_MEM_NOERASE       0x88
#define AT45_BUF2_MEM_NOERASE       0x89
#define AT45_PAGE_ERASE             0x81
#define AT45_BLOCK_ERASE            0x50
#define AT45_SECTOR_ERASE           0x7C
#define AT45_CHIP_ERASE             0xC7, 0x94, 0x80, 0x9A
#define AT45_PAGE_WRITE_BUF1        0x82
#define AT45_PAGE_WRITE_BUF2        0x85

#define AT45_PAGE_BUF1_TX           0x53
#define AT45_PAGE_BUF2_TX           0x55
#define AT45_PAGE_BUF1_CMP          0x60
#define AT45_PAGE_BUF2_CMP          0x61
#define AT45_AUTO_REWRITE_BUF1      0x58
#define AT45_AUTO_REWRITE_BUF2      0x59
#define AT45_DEEP_PDOWN             0xB9
#define AT45_RES_DEEP_PDOWN         0xAB
#define AT45_STATUS_READ            0xD7
#define AT45_ID_READ                0x9F

#define AT45_BINARY_PAGE_FIRST_OPCODE   0x3D
#define AT45_BINARY_PAGE                0x2A, 0x80, 0xA6
#define PAGE_BITS                   9

#if(AT45_PAGE_512)
#define AT45_BUFF_SIZE              (512 + 6)//文件系统最小扇区空间512字节
#else
#define AT45_BUFF_SIZE              (256 + 6)
#endif
#define AT45DBxxxD_MANUFACTURER_ID    0x1F

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static unsigned char s_byaAt45Buff[AT45_BUFF_SIZE];
static struct {
    unsigned int *pdwCs;
    unsigned char bySpiNo;
}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 读取At45状态
参数描述：
*   @ 无
返回值描述：
*   @ 1字节DataFlash存储器返回数据
说明：
*   @
*******************************************************************************/
static unsigned char ReadDF(void)
{
    unsigned char byaRet[2];

    byaRet[0] = AT45_STATUS_READ;         // send status register read op-code
    byaRet[1] = 0;                        // dummy write to get result

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, byaRet, byaRet, 2, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, byaRet, byaRet, 2, s_stCtr.pdwCs);
#endif

    return byaRet[1];                    // return the read status register value
}

/*******************************************************************************
函数功能：
*   @ 读取At45ID
参数描述：
*   @ 无
返回值描述：
*   @ 1字节DataFlash存储器返回数据
说明：
*   @
*******************************************************************************/
static unsigned short ReadID(void)
{
    unsigned char byaRet[5]={0};
    unsigned short usChipID;

    byaRet[0] = AT45_ID_READ;             // send status register read op-code
    byaRet[1] = 0;                        // dummy write to get result

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, byaRet, byaRet, 5, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, byaRet, byaRet, 1, s_stCtr.pdwCs);
#endif

	if(byaRet[1] != AT45DBxxxD_MANUFACTURER_ID) return 0;
    byaRet[2]=1<<((byaRet[1] & 0x1F)-2);
    usChipID = (((unsigned short)byaRet[1]) << 8) + byaRet[2];

    return usChipID;                    // return the read status register value
}

#if(CONFIG_AT45_ALL_FUNC)
/*******************************************************************************
函数功能：
*   @ 直接读取页内数据
参数描述：
*   @ unsigned short int wPageAdr ：页地址
*   @ unsigned short int wStartAdr ： 页内起始地址
*   @ unsigned short int wLen ： 数据长度
*   @ unsigned char *pbyOut ： 数据输出地址
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void ConArrayRead(unsigned short int wPageAdr, unsigned short int wStartAdr, unsigned short int wLen, unsigned char *pbyOut)
{
    unsigned char byTmp;

    s_byaAt45Buff[0] = AT45_CONTINUOUS_READ_LF;
    s_byaAt45Buff[1] = (unsigned char)(wPageAdr >> (16 - PAGE_BITS));
    byTmp = (unsigned char)(wPageAdr << (PAGE_BITS - 8));
    s_byaAt45Buff[2] = (unsigned char)(wStartAdr >> 8) | byTmp;
    s_byaAt45Buff[3] = (unsigned char)wStartAdr;
    s_byaAt45Buff[4] = (unsigned char)wStartAdr;

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, s_byaAt45Buff, s_byaAt45Buff, wLen + 5, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, s_byaAt45Buff, s_byaAt45Buff, wLen + 5, s_stCtr.pdwCs);
#endif
    memcpy(pbyOut, (const unsigned char*)&s_byaAt45Buff[5], wLen);
}
#endif

/*******************************************************************************
函数功能：
*   @ 页数据先到buffer
参数描述：
*   @ unsigned short int wPageAdr ：页地址
*   @ unsigned char byBufferNo ： 缓冲区编号
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void PageToBuffer(unsigned short int wPageAdr, unsigned char byBufferNo)
{
    unsigned char byaCmd[4];

    if(byBufferNo == 1) byaCmd[0] = AT45_PAGE_BUF1_TX;
    else byaCmd[0] = AT45_PAGE_BUF2_TX;

#if(AT45_PAGE_512)
    byaCmd[1] = (u8)(wPageAdr >> 6);
    byaCmd[2] = (u8)(wPageAdr << 2);
#else
    byaCmd[1] = (u8)(wPageAdr >> (16 - PAGE_BITS));
    byaCmd[2] = (u8)(wPageAdr << (PAGE_BITS - 8));
#endif

    byaCmd[3] = 0x00;

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, byaCmd,byaCmd,4, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#endif

    for(int i = 0;i < 0xff; i ++){;}
    while(!(ReadDF() & 0x80)) {
        for(int i = 0;i < 0xffff; i ++){;}
    }
}

/*******************************************************************************
函数功能：
*   @ buffer数据到页
参数描述：
*   @ unsigned short int wPageAdr ：页地址
*   @ unsigned char byBufferNo ： 缓冲区编号
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void BufferToPage( unsigned short int wPageAdr, unsigned char byBufferNo)
{
    unsigned char byaCmd[4];

    if(byBufferNo == 1) byaCmd[0] = AT45_BUF1_MEM_ERASE;
    else byaCmd[0] = AT45_BUF2_MEM_ERASE;

#if(AT45_PAGE_512)
    byaCmd[1] = (u8)(wPageAdr >> 6);
    byaCmd[2] = (u8)(wPageAdr << 2);
#else
    byaCmd[1] = (u8)(wPageAdr >> (16 - PAGE_BITS));
    byaCmd[2] = (u8)(wPageAdr << (PAGE_BITS - 8));
#endif

    byaCmd[3] = 0x00;

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#endif

    for(int i = 0;i < 0xff; i ++){;}
    while(!(ReadDF() & 0x80)){;}
}

/*******************************************************************************
函数功能：
*   @ 读取buffer数据
参数描述：
*   @ unsigned char byBufferNo ：缓冲区编号
*   @ unsigned short int wStartAdr ： 页内起始地址
*   @ unsigned short int wLen ： 数据长度
*   @ unsigned char *pbyOut ： 数据输出地址
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void BufferRead( unsigned char byBufferNo, unsigned short int wStartAdr, unsigned short int wLen, unsigned char *pbyOut)
{
    memset(s_byaAt45Buff, 0, AT45_BUFF_SIZE);
    if(byBufferNo == 1) s_byaAt45Buff[0] = AT45_BUF1_READ_SER;
    else s_byaAt45Buff[0] = AT45_BUF2_READ_SER;

    s_byaAt45Buff[1] = 0x00;
    s_byaAt45Buff[2] = (u8)(wStartAdr >> 8);
    s_byaAt45Buff[3] = (u8)(wStartAdr);
    s_byaAt45Buff[4] = 0;

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, s_byaAt45Buff,s_byaAt45Buff, wLen + 5, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, s_byaAt45Buff,s_byaAt45Buff, wLen + 5, s_stCtr.pdwCs);
#endif
    memcpy(pbyOut, (const unsigned char*)&s_byaAt45Buff[5], wLen);
}

/*******************************************************************************
函数功能：
*   @ 向buffer写数据
参数描述：
*   @ unsigned char byBufferNo ：缓冲区编号
*   @ unsigned short int wStartAdr ： 页内起始地址
*   @ unsigned short int wLen ： 数据长度
*   @ unsigned char *pbyIn ： 数据输入地址
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void BufferWrite( unsigned char byBufferNo, unsigned short int wStartAdr, unsigned short int wLen, const unsigned char *pbyIn)
{
    if(byBufferNo == 1) s_byaAt45Buff[0] = AT45_BUF1_WRITE;
    else s_byaAt45Buff[0] = AT45_BUF2_WRITE;

    s_byaAt45Buff[1] = 0x00;
    s_byaAt45Buff[2] = (unsigned char)(wStartAdr >> 8);
    s_byaAt45Buff[3] = (unsigned char)(wStartAdr);

    memcpy(&s_byaAt45Buff[4], pbyIn, wLen);

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, s_byaAt45Buff,s_byaAt45Buff, wLen + 4, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, s_byaAt45Buff,s_byaAt45Buff, wLen + 4, s_stCtr.pdwCs);
#endif
}

#if(CONFIG_AT45_ALL_FUNC)
/*******************************************************************************
函数功能：
*   @ 向buffer写数据
参数描述：
*   @ unsigned short int wPageAdr ：页编号
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void AutoPageRewrite(unsigned short int wPageAdr)
{
    unsigned char byaCmd[4];

    byaCmd[0] = AT45_AUTO_REWRITE_BUF1;
#if(AT45_PAGE_512)
    byaCmd[1] = (u8)(wPageAdr >> 6);
    byaCmd[2] = (u8)(wPageAdr << 2);
#else
    byaCmd[1] = (u8)(wPageAdr >> (16 - PAGE_BITS));
    byaCmd[2] = (u8)(wPageAdr << (PAGE_BITS - 8));
#endif
    byaCmd[3] = 0x00;

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#endif

    for(int i = 0;i < 0xff; i ++){;}
    while(!(ReadDF() & 0x80)){
        for(int i = 0;i < 0xffff; i ++){;}
    }
}
#endif

#if(CONFIG_AT45_ALL_FUNC)
/*******************************************************************************
函数功能：
*   @ 擦出页
参数描述：
*   @ @ unsigned short int wPageAdr ：页编号
返回值描述：
*   @ 无
说明：
*   @
*******************************************************************************/
static void PageErase(unsigned short int wPageAdr)
{
    unsigned char byaCmd[4];

    byaCmd[0] = AT45_PAGE_ERASE;
#if(AT45_PAGE_512)
    byaCmd[1] = (u8)(wPageAdr >> 6);
    byaCmd[2] = (u8)((wPageAdr& 0x007F) << 2);
#else
    byaCmd[1] = (u8)(wPageAdr >> (16 - PAGE_BITS));
    byaCmd[2] = (u8)(wPageAdr << (PAGE_BITS - 8));
#endif
    byaCmd[3] = 0x00;

#if(!CONFIG_AT45_SPI_DMA)
    DevSpi->Trs(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#else
    DevSpi->TrsDma(s_stCtr.bySpiNo, byaCmd,byaCmd, 4, s_stCtr.pdwCs);
#endif

    for(int i = 0;i < 0xff; i ++){;}
    while(!(ReadDF() & 0x80)){
        for(int i = 0;i < 0xffff; i ++){;}
    }
}
#endif

/*******************************************************************************
函数功能：
*   @ 读取页数据
参数描述：
*   @ unsigned short int wSector ：缓冲区编号
*   @ unsigned char *pbyOut ： 数据输入地址
*   @ unsigned short int wLen ： 数据长度
返回值描述：
*   @ 无
说明：
*   @ 一次操作1个页
*******************************************************************************/
static void ReadPage(unsigned short int wSector, unsigned char *pbyOut, unsigned short int wLen)
{
#if(AT45_PAGE_512)
	PageToBuffer(wSector, 1);
	BufferRead( 1, 0, wLen, pbyOut);
#else
	PageToBuffer((wSector << 1),1);
	BufferRead( 1, 0, wLen/2, pbyOut);
	PageToBuffer((wSector << 1) + 1, 1);
	BufferRead( 1, 0, wLen/2, pbyOut + 256); //AT45DB041 文件系统一个扇区对应操作两页
#endif
}

/*******************************************************************************
函数功能：
*   @ 写页数据
参数描述：
*   @ unsigned short int wSector ：缓冲区编号
*   @ unsigned char *pbyIn ： 数据输入地址
*   @ unsigned short int wLen ： 数据长度
返回值描述：
*   @ 无
说明：
*   @ 一次操作2个页，只要是为fat文件系统使用
*******************************************************************************/
void WritePage(unsigned short int wSector, const unsigned char *pbyIn, unsigned short int wLen)
{
#if(AT45_PAGE_512)
    BufferWrite( 1, 0, wLen, pbyIn);
    BufferToPage(wSector, 1);
#else
	BufferWrite( 1, 0, wLen/2, pbyIn);
    BufferToPage(wSector << 1, 1);
    BufferWrite( 1, 0, wLen/2, pbyIn + 256);
    BufferToPage((wSector << 1) + 1, 1);
#endif
}

static const DevAt45Tdf s_Dev = {
    ReadPage,
    WritePage,
#if(CONFIG_AT45_ALL_FUNC)
    PageErase,
#endif
    ReadID,
};

/*******************************************************************************
函数功能：
*   @ At45存储器初始化
参数描述：
*   @ 初始化参数
返回值描述：
*   @ At45操作接口
说明：
*   @ 无
*******************************************************************************/
const DevAt45Tdf *At45Init( At45InitTdf *pstInit)
{
    s_stCtr.bySpiNo = pstInit->bySpiNo;
    s_stCtr.pdwCs = pstInit->pdwNcs;
    *s_stCtr.pdwCs = 1;
    DevSpi->Config(s_stCtr.bySpiNo, pstInit->dwSpiBoud); // 128
#if(CONFIG_AT45_SPI_DMA)
    DevSpi->ConfigDmaRx(s_stCtr.bySpiNo, pstInit->bySpiDmaRxNo, pstInit->bySpiDmaRxChn);
    DevSpi->ConfigDmaTx(s_stCtr.bySpiNo, pstInit->bySpiDmaTxNo, pstInit->bySpiDmaTxChn);
#endif
    return &s_Dev;
}
#endif
