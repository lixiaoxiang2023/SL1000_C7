/*******************************************************************************
文件名称： spi.c
目录：     $PROJ_DIR$/../code/dev
项目名称： 事业一部技术平台
创建者：   胥强
创建日期： 20160308
程序用途：

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20160308：第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "dev.h"

#if(CONFIG_SPI1|CONFIG_SPI2|CONFIG_SPI3|CONFIG_SPI4|CONFIG_SPI5|CONFIG_SPI6)
/*******************************************************************************
常量定义
*******************************************************************************/
#if(PLAT_STM32_F4)
    #define SPI_DEV_NUM                     6
    #else
    #define SPI_DEV_NUM                     3
#endif

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
    SPI_TypeDef *stSpiDev;
    unsigned int *pdwCs;

    unsigned int dwLock;           /* 锁程序 */

#if(CONFIG_SPI_DMA)
    unsigned char byDmaNo;
    unsigned char byDmaChnTx;
    unsigned char byDmaChnRx;
#if(PLAT_STM32_F2|PLAT_STM32_F4)
    unsigned char byDmaStreamTx;
    unsigned char byDmaStreamRx;
#endif
#endif
}s_staDev[SPI_DEV_NUM];

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ pi设备的时钟初始化
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void RccInit(void)
{
#if(CONFIG_SPI1)
    RCC->BitAPB2ENR.SPI1EN = 1;
#endif
#if(CONFIG_SPI2)
    RCC->BitAPB1ENR.SPI2EN = 1;
#endif
#if(CONFIG_SPI3)
    RCC->BitAPB1ENR.SPI3EN = 1;
#endif

#if(PLAT_STM32_F4)
    #if(CONFIG_SPI4)
    RCC->BitAPB2ENR.SPI4EN = 1;
    #endif
    #if(CONFIG_SPI5)
    RCC->BitAPB2ENR.SPI5EN = 1;
    #endif
    #if(CONFIG_SPI6)
    RCC->BitAPB2ENR.SPI6EN = 1;
    #endif
#endif
}

/*******************************************************************************
函数功能：
*   @ spi设备的初始化
参数描述：
*   @ unsigned int dwDev：spi设备号
*   @ unsigned int dwBaudPres： 分频值
返回值描述：
    无
说明：
*   @ 无
*******************************************************************************/
static int Config(unsigned int dwDev, unsigned int dwBaud)
{
    ClocksTypeDef stRccFreq;
    unsigned short int wConfig = 0;
    unsigned short int dwBaudPres = 0;
    unsigned short int wBaud = 0;

    if(dwDev > SPI_DEV_NUM) return -1;
    DevRccGetClocksFreq(&stRccFreq);

    s_staDev[dwDev].stSpiDev->CR1 = 0x00;

    if(dwDev == 1 || dwDev == 2) {
        // SPI2 SPI3 在APB1
        dwBaudPres = stRccFreq.PCLK1 / dwBaud;
    }
    else {
        dwBaudPres = stRccFreq.PCLK2 / dwBaud;
    }

    /*spi1 configuration
    s_staDev[dwDev].stSpiDev->CR1 = (SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | \
    SPI_DataSize_8b | SPI_CPOL_Low | SPI_CPHA_1Edge | \
    SPI_NSS_Soft | SPI_BaudRatePrescaler_2 | SPI_FirstBit_MSB);
    */
    dwBaudPres <<= 1;

    if(dwBaudPres >= 256) wBaud = ((unsigned short int)0x0038);
    else if(dwBaudPres >= 128) wBaud = ((unsigned short int)0x0030);
    else if(dwBaudPres >= 64 ) wBaud = ((unsigned short int)0x0028);
    else if(dwBaudPres >= 32 ) wBaud = ((unsigned short int)0x0020);
    else if(dwBaudPres >= 16 ) wBaud = ((unsigned short int)0x0018);
    else if(dwBaudPres >= 8  ) wBaud = ((unsigned short int)0x0010);
    else if(dwBaudPres >= 4  ) wBaud = ((unsigned short int)0x0008);
    else wBaud = ((unsigned short int)0x0000);

    //  SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    /* Initialize the SPI_CPHA member */
    //  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;

    wConfig = (((unsigned short int)0x0000) | ((unsigned short int)0x0104) | \
        ((unsigned short int)0x0000) | ((unsigned short int)0x0000) | ((unsigned short int)0x0000) | \
            ((unsigned short int)0x0200) | (unsigned short int)wBaud | ((unsigned short int)0x0000));

    s_staDev[dwDev].stSpiDev->CR1 = wConfig;
    s_staDev[dwDev].stSpiDev->BitCR1.SPE = 1;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置spi设备的时钟极性和时钟相位
参数描述：
*   @ unsigned int dwDev：spi设备号
*   @ unsigned char byPOL：时钟极性
*   @ unsigned char byPHA：时钟相位
返回值描述：
    无
说明：
*   @ 无
*******************************************************************************/
static int ConfigPOL_PHA(unsigned int dwDev, unsigned char byPOL, unsigned char byPHA)
{
    unsigned short int wConfig = 0;

    if(dwDev > SPI_DEV_NUM) return -1;

    s_staDev[dwDev].stSpiDev->BitCR1.SPE = 0;

    wConfig = s_staDev[dwDev].stSpiDev->CR1;

    if (byPHA) wConfig |= 0x0001;
    else wConfig &= 0xFFFE;

    if (byPOL) wConfig |= 0x0002;
    else wConfig &= 0xFFFD;

    s_staDev[dwDev].stSpiDev->CR1 = wConfig;
    s_staDev[dwDev].stSpiDev->BitCR1.SPE = 1;

    return 0;
}

#if(CONFIG_SPI_DMA)
#if(PLAT_STM32_F2|PLAT_STM32_F4)
/*******************************************************************************
函数功能：
*   @ 配置SPI的发送DMA接口
参数描述：
*   @ unsigned int dwDev：spi设备号
*   @ unsigned int dwDmaDev： spiDMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int ConfigDmaTx ( unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn, unsigned int dwStream)
{
    if(dwDev > SPI_DEV_NUM) return -1;
    DevDma->ConfigM2P(dwDmaDev, dwStream, dwDmaChn, DMA_Normal_Mode, 0, 0);               // 数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChnTx = dwDmaChn;
    s_staDev[dwDev].byDmaStreamTx = dwStream;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置SPI的发送DMA接口
参数描述：
*   @ unsigned int dwDev：spi设备号
*   @ unsigned int dwDmaDev： spiDMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int ConfigDmaRx ( unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn, unsigned int dwStream)
{
    if(dwDev > SPI_DEV_NUM) return -1;
    DevDma->ConfigP2M(dwDmaDev, dwStream, dwDmaChn, DMA_Normal_Mode, 0, 0); //数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChnRx = dwDmaChn;
    s_staDev[dwDev].byDmaStreamRx = dwStream;

    return 0;
}

#else
/*******************************************************************************
函数功能：
*   @ 配置SPI的发送DMA接口
参数描述：
*   @ unsigned int dwDev：spi设备号
*   @ unsigned int dwDmaDev： spiDMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int ConfigDmaTx ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn)
{
    if(dwDev > SPI_DEV_NUM) return -1;
    DevDma->ConfigM2P(dwDmaDev, dwDmaChn, DMA_Mode_Normal, 0, 0);               // 数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChnTx = dwDmaChn;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置SPI的发送DMA接口
参数描述：
*   @ unsigned int dwDev：spi设备号
*   @ unsigned int dwDmaDev： spiDMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int ConfigDmaRx ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn)
{
    if(dwDev > SPI_DEV_NUM) return -1;
    DevDma->ConfigP2M(dwDmaDev, dwDmaChn, DMA_Mode_Normal, 0, 0); //数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChnRx = dwDmaChn;

    return 0;
}
#endif
#endif

/*******************************************************************************
函数功能：
*   @ spi设备数据传输
参数描述：
*   @ unsigned int dwDev：SPI设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int Trs(unsigned int dwDev, const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs)
{
    unsigned char byRd = 0;
    if(dwDev > SPI_DEV_NUM) return -1;

    DevIo->CommLock(&s_staDev[dwDev].dwLock, true);

    s_staDev[dwDev].pdwCs = pdwCs;
    while (s_staDev[dwDev].stSpiDev->BitSR.TXE == 0);
    if(s_staDev[dwDev].pdwCs) *s_staDev[dwDev].pdwCs = 0;

    for(int i = 0; i < dwLen; i++) {
        while(s_staDev[dwDev].stSpiDev->BitSR.TXE == 0);
        s_staDev[dwDev].stSpiDev->DR = (pbyIn != NULL)? (*pbyIn):0xff;

        while(s_staDev[dwDev].stSpiDev->BitSR.RXNE == 0);
        byRd = s_staDev[dwDev].stSpiDev->DR;
        if(pbyOut != NULL) *pbyOut = byRd;

        if(pbyIn != NULL) pbyIn ++;
        if(pbyOut != NULL)  pbyOut ++;
    }

	if(s_staDev[dwDev].pdwCs) *s_staDev[dwDev].pdwCs = 1;
    DevIo->CommUnLock(&s_staDev[dwDev].dwLock);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ spi设备数据传输
参数描述：
*   @ unsigned int dwDev：SPI设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int TrsHold(unsigned int dwDev, const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs)
{
    unsigned char byRd = 0;
    if(dwDev > SPI_DEV_NUM) return -1;
    DevIo->CommLock(&s_staDev[dwDev].dwLock, true);

    s_staDev[dwDev].pdwCs = pdwCs;
    while (s_staDev[dwDev].stSpiDev->BitSR.TXE == 0);
    *s_staDev[dwDev].pdwCs = 0;

    for(int i = 0; i < dwLen; i++) {
        while(s_staDev[dwDev].stSpiDev->BitSR.TXE == 0);
        s_staDev[dwDev].stSpiDev->DR = *pbyIn;

        while(s_staDev[dwDev].stSpiDev->BitSR.RXNE == 0);
        byRd = s_staDev[dwDev].stSpiDev->DR;
        if(pbyOut != NULL) *pbyOut = byRd;

        if(pbyIn != NULL) pbyIn ++;
        if(pbyOut != NULL) pbyOut ++;
    }

	// *s_staDev[dwDev].pdwCs = 1;
    DevIo->CommUnLock(&s_staDev[dwDev].dwLock);
    return 0;
}

#if(CONFIG_SPI_DMA)
/*******************************************************************************
*函数功能：
*   spi设备数据传输
*参数描述：
*   @ unsigned int dwDev：SPI设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
*返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   2017.1.6 增加对总线独立配置cs引脚
*******************************************************************************/
static int TrsDma(unsigned int dwDev,const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs)
{
    unsigned char byDmaNo;
    unsigned char byDmaTx;
    unsigned char byDmaRx;

    if(dwDev > SPI_DEV_NUM) return -1;
    DevIo->CommLock(&s_staDev[dwDev].dwLock, true);

    s_staDev[dwDev].pdwCs = pdwCs;
    byDmaNo = s_staDev[dwDev].byDmaNo;

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    byDmaTx = s_staDev[dwDev].byDmaStreamTx;
    byDmaRx = s_staDev[dwDev].byDmaStreamRx;
#else
    byDmaTx = s_staDev[dwDev].byDmaChnTx;
    byDmaRx = s_staDev[dwDev].byDmaChnRx;
#endif

    while (s_staDev[dwDev].stSpiDev->BitSR.TXE == 0);
    *s_staDev[dwDev].pdwCs = 0;

    //clear data
    __IO unsigned short int wClr = s_staDev[dwDev].stSpiDev->DR;
    DevDma->Trs(byDmaNo, byDmaRx, (unsigned int)&s_staDev[dwDev].stSpiDev->DR, (unsigned int)pbyOut, dwLen);
    DevDma->Trs(byDmaNo, byDmaTx, (unsigned int)&s_staDev[dwDev].stSpiDev->DR, (unsigned int)pbyIn, dwLen);

    s_staDev[dwDev].stSpiDev->BitCR2.RXDMAEN = 1;
    s_staDev[dwDev].stSpiDev->BitCR2.TXDMAEN = 1;

	while(DevDma->IsBusy(byDmaNo, byDmaRx)){;}

    s_staDev[dwDev].stSpiDev->BitCR2.RXDMAEN = 0;
    s_staDev[dwDev].stSpiDev->BitCR2.TXDMAEN = 0;
	*s_staDev[dwDev].pdwCs = 1;

    DevIo->CommUnLock(&s_staDev[dwDev].dwLock);
    return 0;
}
#endif

#if(CONFIG_SPI_DMA_RETURN)
/*******************************************************************************
*函数功能：
*   @ DMA数据传输完成
*参数描述：
*   @ unsigned int dwDev：SPI设备号
*返回值描述：
*   @ 无
说明：
*   用于清除DMA传输时的SPI关闭以及CS脚拉高
*******************************************************************************/
static void HandlerSpiTrsEnd(unsigned int dwDev)
{
    s_staDev[dwDev].stSpiDev->CR2 &= (~(unsigned short int)0x03);
    *s_staDev[dwDev].pdwCs = 1;
}

/*******************************************************************************
*函数功能：
*   spi设备数据传输
*参数描述：
*   @ unsigned int dwDev：SPI设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
*返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   2017.1.6 增加对总线独立配置cs引脚
*******************************************************************************/
static int TrsDmaRet(unsigned int dwDev,const unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, unsigned int *pdwCs)
{
    unsigned char byDmaNo;
    unsigned char byDmaTx;
    unsigned char byDmaRx;

    if(dwDev > SPI_DEV_NUM) return -1;
    DevIo->CommLock(&s_staDev[dwDev].dwLock, true);

    s_staDev[dwDev].pdwCs = pdwCs;
    *s_staDev[dwDev].pdwCs = 1;

    // s_staDev[dwDev].stSpiDev->CR2 &= (~(unsigned short int)0x03);
    byDmaNo = s_staDev[dwDev].byDmaNo;

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    byDmaTx = s_staDev[dwDev].byDmaStreamTx;
    byDmaRx = s_staDev[dwDev].byDmaStreamRx;
#else
    byDmaTx = s_staDev[dwDev].byDmaChnTx;
    byDmaRx = s_staDev[dwDev].byDmaChnRx;
#endif

    while (s_staDev[dwDev].stSpiDev->BitSR.TXE == 0);
    *s_staDev[dwDev].pdwCs = 0;

#if(PLAT_STM32_F2|PLAT_STM32_F4)
    DevDma->SetTrsHandler((unsigned int)byDmaNo, (unsigned int) byDmaRx, HandlerSpiTrsEnd, dwDev);
#endif
    //clear data
    __IO unsigned short int wClr = s_staDev[dwDev].stSpiDev->DR;
    DevDma->Trs(byDmaNo, byDmaRx, (unsigned int)&s_staDev[dwDev].stSpiDev->DR, (unsigned int)pbyOut, dwLen);
    DevDma->Trs(byDmaNo, byDmaTx, (unsigned int)&s_staDev[dwDev].stSpiDev->DR, (unsigned int)pbyIn, dwLen);

    s_staDev[dwDev].stSpiDev->CR2 |= 0x03;
    DevIo->CommUnLock(&s_staDev[dwDev].dwLock);
    return 0;
}
#endif

static const DevSpiTdf s_Dev = {
    Config,
    ConfigPOL_PHA,
#if(CONFIG_SPI_DMA)
    ConfigDmaTx,
    ConfigDmaRx,
#endif
    Trs,
    TrsHold,
#if(CONFIG_SPI_DMA)
    TrsDma,
#endif
#if(CONFIG_SPI_DMA_RETURN)
    TrsDmaRet,
#endif
};

/*******************************************************************************
函数功能：
*   @ spi设备的初始化
参数描述：
*   @ void (*pvGpioInit)(unsigned char)：gpio初始化
*   @ unsigned char byDat：gpio初始化时需要带入的参数
返回值描述：
    SPI设备提供的操作接口
说明：
*   @ 无
*******************************************************************************/
const DevSpiTdf* SpiInit( void )
{
    RccInit();

#if(CONFIG_SPI1)
    s_staDev[0].stSpiDev = SPI1;
    Config(0, 4*1000*1000);
    #if(CONFIG_SPI_DMA)
    s_staDev[0].byDmaNo = 0;
    s_staDev[0].byDmaChnTx = 3;
    s_staDev[0].byDmaChnRx = 2;
    #endif
#endif

#if(CONFIG_SPI2)
    s_staDev[1].stSpiDev = SPI2;
    Config(1, 4*1000*1000);
    #if(CONFIG_SPI_DMA)
    s_staDev[1].byDmaNo = 0;
    s_staDev[1].byDmaChnTx = 5;
    s_staDev[1].byDmaChnRx = 4;
    #endif
#endif

#if(CONFIG_SPI3)
    s_staDev[2].stSpiDev = SPI3;
    Config(2, 4*1000*1000);
    #if(CONFIG_SPI_DMA)
    s_staDev[2].byDmaNo = 1;
    s_staDev[2].byDmaChnTx = 2;
    s_staDev[2].byDmaChnRx = 1;
    #endif
#endif

#if(PLAT_STM32_F4)
    #if(CONFIG_SPI4)
    s_staDev[3].stSpiDev = SPI4;
    Config(3, 4*1000*1000);
    #if(CONFIG_SPI_DMA)
    s_staDev[3].byDmaNo = 1;
    s_staDev[3].byDmaChnTx = 2;
    s_staDev[3].byDmaChnRx = 1;
    #endif
    #endif

    #if(CONFIG_SPI5)
    s_staDev[4].stSpiDev = SPI5;
    Config(4, 4*1000*1000);
    #if(CONFIG_SPI_DMA)
    s_staDev[4].byDmaNo = 1;
    s_staDev[4].byDmaChnTx = 2;
    s_staDev[4].byDmaChnRx = 1;
    #endif
    #endif

    #if(CONFIG_SPI6)
    s_staDev[5].stSpiDev = SPI6;
    Config(5, 4*1000*1000);
    #if(CONFIG_SPI_DMA)
    s_staDev[5].byDmaNo = 1;
    s_staDev[5].byDmaChnTx = 2;
    s_staDev[5].byDmaChnRx = 1;
    #endif
    #endif
#endif

    return &s_Dev;
}
#endif