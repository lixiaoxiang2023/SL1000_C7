/*******************************************************************************
文件名称：   usart.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   201610308
程序用途：

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20151123：
第一次由XQ创建初始程序。
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "dev.h"

#if(CONFIG_USART1|CONFIG_USART2|CONFIG_USART3|CONFIG_USART4|CONFIG_USART5|CONFIG_USART6)
/*******************************************************************************
常量定义
*******************************************************************************/
#if(PLAT_STM32_F4)
#define USART_MAX_DEV_NUM        8
#endif
#if(PLAT_STM32_F2)
#define USART_MAX_DEV_NUM        6
#endif
#if(PLAT_STM32_F1)
#define USART_MAX_DEV_NUM        5
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
    USART_TypeDef *pstUart;
    unsigned char byDmaNo;
    unsigned char byDmaChn;

    void (*pfHandler)(unsigned char );
#if(CONFIG_USART_IT)
    struct {
        unsigned int dwLen;
        const unsigned char *pbyAddr;
    }stTx;
#endif
} s_staDev[USART_MAX_DEV_NUM];

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 初始化串口中断部分
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static void NvicInit ( void )
{
#if(CONFIG_USART1)
    DevIo->NvicUp(USART1_IRQn, 9, 0);

    USART1->BitCR1.RXNEIE = 1;
    USART1->BitCR1.UE = 1;
#endif
#if(CONFIG_USART2)
    DevIo->NvicUp(USART2_IRQn, 9, 0);

    USART2->BitCR1.RXNEIE = 1;
    USART2->BitCR1.UE = 1;
#endif
#if(CONFIG_USART3)
    DevIo->NvicUp(USART3_IRQn, 9, 0);

    USART3->BitCR1.RXNEIE = 1;
    USART3->BitCR1.UE = 1;
#endif
#if(CONFIG_USART4)
    DevIo->NvicUp(UART4_IRQn, 9, 0);

    UART4->BitCR1.RXNEIE = 1;
    UART4->BitCR1.UE = 1;
#endif
#if(CONFIG_USART5)
    DevIo->NvicUp(UART5_IRQn, 9, 0);

    UART5->BitCR1.RXNEIE = 1;
    UART5->BitCR1.UE = 1;
#endif

#if(PLAT_STM32_F2)
#if(CONFIG_USART6)
    DevIo->NvicUp(USART6_IRQn, 9, 0);

    USART6->BitCR1.RXNEIE = 1;
    USART6->BitCR1.UE = 1;
#endif
#endif

#if(PLAT_STM32_F4)
#if(CONFIG_USART6)
    DevIo->NvicUp(USART6_IRQn, 9, 0);

    USART6->BitCR1.RXNEIE = 1;
    USART6->BitCR1.UE = 1;
#endif

#if(CONFIG_USART7)
    DevIo->NvicUp(UART7_IRQn, 9, 0);

    UART7->BitCR1.RXNEIE = 1;
    UART7->BitCR1.UE = 1;
#endif
#if(CONFIG_USART8)
    DevIo->NvicUp(UART8_IRQn, 9, 0);

    UART8->BitCR1.RXNEIE = 1;
    UART8->BitCR1.UE = 1;
#endif
#endif
}

/*******************************************************************************
函数功能：
*   @ 开启设备时钟
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static void RccInit ( void )
{
#if(CONFIG_USART1)
    RCC->BitAPB2ENR.USART1EN = 1;
#endif
#if(CONFIG_USART2)
    RCC->BitAPB1ENR.USART2EN = 1;
#endif
#if(CONFIG_USART3)
    RCC->BitAPB1ENR.USART3EN = 1;
#endif
#if(CONFIG_USART4)
    RCC->BitAPB1ENR.UART4EN = 1;
#endif
#if(CONFIG_USART5)
    RCC->BitAPB1ENR.UART5EN = 1;
#endif

#if(PLAT_STM32_F2)
#if(CONFIG_USART6)
    RCC->BitAPB2ENR.USART6EN = 1;
#endif

#endif

#if(PLAT_STM32_F4)
    #if(CONFIG_USART6)
        RCC->BitAPB2ENR.USART6EN = 1;
    #endif
    #if(CONFIG_USART7)
        RCC->BitAPB1ENR.UART7EN = 1;
    #endif
    #if(CONFIG_USART8)
        RCC->BitAPB1ENR.UART8EN = 1;
    #endif
#endif
}

/*******************************************************************************
函数功能：
*   @ 初始化串口设备参数
参数描述：
*   @ unsigned int dwDev：设备号0...4，对应串口1~5.
*   @ unsigned int dwBaud：波特率
*   @ int iStop：1：停止位1，其他停止位2
*   @ int iParity：0：无校验，1奇校验，2偶校验
*   @ int iDatLen ： 7=7位数据， 88位数据
*   @ bool blRx：true：发送、接收，false：发送
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int Config2 ( unsigned int dwDev, unsigned int dwBaud, int iStop, int iParity , int iDatLen, bool blRx)
{
    ClocksTypeDef stRccFreq;
    unsigned int tmpreg = 0x00, apbclock = 0x00;
    unsigned int integerdivider = 0x00;
    unsigned int fractionaldivider = 0x00;

    if(dwDev >= USART_MAX_DEV_NUM) return -1;
    DevRccGetClocksFreq(&stRccFreq);

    s_staDev[dwDev].pstUart->BitCR1.UE = 0;

    if(iDatLen == 7) s_staDev[dwDev].pstUart->BitCR1.M = 0;
    else {
        if(iParity != 0) s_staDev[dwDev].pstUart->BitCR1.M = 1;
        else s_staDev[dwDev].pstUart->BitCR1.M = 0;
    }

    if(iStop == 1) s_staDev[dwDev].pstUart->BitCR2.STOP = 0;
    else s_staDev[dwDev].pstUart->BitCR2.STOP = 2;

    if(iParity == 0) s_staDev[dwDev].pstUart->BitCR1.PCE = 0;
    else if(iParity == 1) {
        s_staDev[dwDev].pstUart->BitCR1.PCE = 1;
        s_staDev[dwDev].pstUart->BitCR1.PS = 1;
    }
    else {
        s_staDev[dwDev].pstUart->BitCR1.PCE = 1;
        s_staDev[dwDev].pstUart->BitCR1.PS = 0;
    }

    s_staDev[dwDev].pstUart->BitCR3.CTSE = 0;
    s_staDev[dwDev].pstUart->BitCR3.RTSE = 0;

    s_staDev[dwDev].pstUart->BitCR1.RE = 0;
    s_staDev[dwDev].pstUart->BitCR1.TE = 1;
    if(blRx) {
        s_staDev[dwDev].pstUart->BitCR1.RE = 1;
    }

    if(dwDev == 0 || dwDev == 5) apbclock = stRccFreq.PCLK2;
    else apbclock = stRccFreq.PCLK1;

    integerdivider = ((0x19 * apbclock) / (0x04 * (dwBaud)));
    tmpreg = (integerdivider / 0x64) << 0x04;
    /* Determine the fractional part */
    fractionaldivider = integerdivider - (0x64 * (tmpreg >> 0x04));
    tmpreg |= ((((fractionaldivider * 0x10) + 0x32) / 0x64)) & ((uint8_t)0x0F);
    /* Write to USART BRR */
    s_staDev[dwDev].pstUart->BRR = (uint16_t)tmpreg;

    s_staDev[dwDev].pstUart->BitCR1.UE = 1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 初始化串口设备参数
参数描述：
*   @ unsigned int dwDev：设备号0...4，对应串口1~5.
*   @ unsigned int dwBaud：波特率
*   @ int iStop：1：停止位1，其他停止位2
*   @ int iParity：0：无校验，1奇校验，2偶校验
*   @ bool blRx：true：发送、接收，false：发送
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int Config ( unsigned int dwDev, unsigned int dwBaud, int iStop, int iParity, bool blRx)
{
    Config2 ( dwDev, dwBaud, iStop, iParity , 8, blRx);
    return 0;
}

#if(CONFIG_USART_DMA)
/*******************************************************************************
函数功能：
*   @ 配置串口的DMA接口
参数描述：
*   @ unsigned int dwDev：设备号0...4，对应串口1~5.
*   @ unsigned int dwDmaDev： 串口发送通道的DMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int ConfigDma ( unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn)
{
    if(dwDev >= USART_MAX_DEV_NUM) return -1;
    DevDma->ConfigM2P(dwDmaDev, dwDmaChn, 0, 0); //数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChn = dwDmaChn;

    return 0;
}
#endif

/*******************************************************************************
函数功能：
*   @ 发送单个字节
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned char byDat： 数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int SendByte(unsigned int dwDev, unsigned char byIn)
{
    if(dwDev >= USART_MAX_DEV_NUM) return -1;
    while(s_staDev[dwDev].pstUart->BitSR.TXE == 0){;}
    s_staDev[dwDev].pstUart->DR = byIn;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 打开或者关闭串口接收功能
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned char byDat： 数据
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int RxCmd(unsigned int dwDev, bool blIsOn)
{
    if(dwDev >= USART_MAX_DEV_NUM) return -1;
    s_staDev[dwDev].pstUart->BitCR1.RE = (blIsOn)?1:0;

    return 0;
}

#if(CONFIG_USART_DMA)
/*******************************************************************************
函数功能：
*   @ 通过DMA发送数据
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned char byDat： 数据引入地址
*   @ unsigned int dwLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int SendStringDma(unsigned int dwDev, const unsigned char *pbyIn, unsigned int dwLen)
{
    unsigned char byDmaNo;
    unsigned char byChn;
    if(dwDev >= USART_MAX_DEV_NUM) return -1;

    while(s_staDev[dwDev].pstUart->BitSR.TXE == 0){;}
    byDmaNo = s_staDev[dwDev].byDmaNo;
    byChn = s_staDev[dwDev].byDmaChn;

    DevDma->Trs(byDmaNo, byChn, (unsigned int)&s_staDev[dwDev].pstUart->DR, (unsigned int)pbyIn, dwLen);
    s_staDev[dwDev].pstUart->BitCR3.DMAT = 1;

    while(DevDma->IsBusy(byDmaNo, byChn)){;}
    s_staDev[dwDev].pstUart->BitCR3.DMAT = 0;

    return 0;
}
#endif

/*******************************************************************************
函数功能：
*   @ 使用普通的排队方法发送数据
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned char byDat： 数据引入地址
*   @ unsigned int dwLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int SendString(unsigned int dwDev, const unsigned char *pbyIn, unsigned int dwLen)
{
    if(dwDev >= USART_MAX_DEV_NUM) return -1;

    for(int i = 0; i < dwLen; i++) {
        while(s_staDev[dwDev].pstUart->BitSR.TXE == 0){;}
        s_staDev[dwDev].pstUart->DR = *pbyIn ++;
    }

    while(s_staDev[dwDev].pstUart->BitSR.TXE == 0){;}
    return 0;
}

#if(CONFIG_USART_IT)
/*******************************************************************************
函数功能：
*   @ 使用普通的排队方法发送数据
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ unsigned char byDat： 数据引入地址
*   @ unsigned int dwLen：数据长度
返回值描述：
*   @ 无
说明：
*   @ 此函数将阻塞。
*******************************************************************************/
static int SendStringIt(unsigned int dwDev, const unsigned char *pbyIn, unsigned int dwLen)
{
    if(dwDev >= USART_MAX_DEV_NUM) return -1;

    if(!dwLen) return -1;
    if(!pbyIn) return -1;
    if(s_staDev[dwDev].stTx.dwLen) return -1;

    while(s_staDev[dwDev].pstUart->BitSR.TXE == 0) {;}
    s_staDev[dwDev].stTx.pbyAddr = pbyIn;
    s_staDev[dwDev].stTx.dwLen = dwLen;

    s_staDev[dwDev].pstUart->BitCR1.TXEIE = 1;
    return 0;
}
#endif

/*******************************************************************************
函数功能：
*   @ 注册串口接收到数据后需要调用的函数接口
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ void (*pvHandler)(unsigned char)： 带参数的函数地址
返回值描述：
*   @ 无
说明：
*   @ 中断函数在接受到数据后将自动调用此注册的函数接口。
*******************************************************************************/
static int RegHandler(unsigned int dwDev, void (*pvHandler)(unsigned char))
{
    if(dwDev >= USART_MAX_DEV_NUM) return -1;
    s_staDev[dwDev].pfHandler = pvHandler;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 中断函数接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 中断函数在接受到数据后将自动调用此注册的函数接口。
*******************************************************************************/
#if(CONFIG_USART1)
void USART1_IRQHandler(void)
{
    unsigned char byDat = 0;
#if(CONFIG_USART_IT)
    if(s_staDev[0].stTx.dwLen) {
        if(USART1->BitSR.TXE == 1) {
            if(s_staDev[0].stTx.dwLen == 1) USART1->BitCR1.TXEIE = 0;
            USART1->DR = *s_staDev[0].stTx.pbyAddr ++;
            s_staDev[0].stTx.dwLen --;
        }
    }
#endif

    if(USART1->SR & 0x000f) {   //防止PE/FE/NE/ORE错误（SR低4位）
        byDat = USART1->DR;
        USART1->SR &= 0xfff0;
    }
    if(USART1->BitSR.RXNE) {
        USART1->BitSR.RXNE = 0;
        byDat = USART1->DR;
        if(s_staDev[0].pfHandler) s_staDev[0].pfHandler( byDat);
    }
}
#endif

#if(CONFIG_USART2)
void USART2_IRQHandler(void)
{
    unsigned char byDat = 0;
#if(CONFIG_USART_IT)
    if(s_staDev[1].stTx.dwLen) {
        if(USART2->BitSR.TXE == 1) {
            if(s_staDev[1].stTx.dwLen == 1) USART2->BitCR1.TXEIE = 0;
            USART2->DR = *s_staDev[1].stTx.pbyAddr ++;
            s_staDev[1].stTx.dwLen --;
        }
    }
#endif

    if(USART2->SR & 0x000f) {   //防止PE/FE/NE/ORE错误（SR低4位）
        byDat = USART2->DR;
        USART2->SR &= 0xfff0;
    }
    if(USART2->BitSR.RXNE) {
        USART2->BitSR.RXNE = 0;
        byDat = USART2->DR;
        if(s_staDev[1].pfHandler) s_staDev[1].pfHandler( byDat);
    }
}
#endif

#if(CONFIG_USART3)
void USART3_IRQHandler(void)
{
    unsigned char byDat = 0;
#if(CONFIG_USART_IT)
    if(s_staDev[2].stTx.dwLen) {
        if(USART3->BitSR.TXE == 1) {
            if(s_staDev[2].stTx.dwLen == 1) USART3->BitCR1.TXEIE = 0;
            USART3->DR = *s_staDev[2].stTx.pbyAddr ++;
            s_staDev[2].stTx.dwLen --;
        }
    }
#endif
    
    if(USART3->SR & 0x000f) {   //防止PE/FE/NE/ORE错误（SR低4位）
        byDat = USART3->DR;
        USART3->SR &= 0xfff0;
    }
    if(USART3->BitSR.RXNE) {
        USART3->BitSR.RXNE = 0;
        byDat = USART3->DR;
        if(s_staDev[2].pfHandler) s_staDev[2].pfHandler( byDat);
    }
}
#endif

#if(CONFIG_USART4)
void UART4_IRQHandler(void)
{
    unsigned char byDat = 0;
#if(CONFIG_USART_IT)
    if(s_staDev[3].stTx.dwLen) {
        if(UART4->BitSR.TXE == 1) {
            if(s_staDev[3].stTx.dwLen == 1) UART4->BitCR1.TXEIE = 0;
            UART4->DR = *s_staDev[3].stTx.pbyAddr ++;
            s_staDev[3].stTx.dwLen --;
        }
    }
#endif

    if(UART4->SR & 0x000f) {   //防止PE/FE/NE/ORE错误（SR低4位）
        byDat = UART4->DR;
        UART4->SR &= 0xfff0;
    }
    if(UART4->BitSR.RXNE) {
        UART4->BitSR.RXNE = 0;
        byDat = UART4->DR;
        if(s_staDev[3].pfHandler) s_staDev[3].pfHandler( byDat);
    }
}
#endif

#if(CONFIG_USART5)
void UART5_IRQHandler(void)
{
    unsigned char byDat = 0;
#if(CONFIG_USART_IT)
    if(s_staDev[4].stTx.dwLen) {
        if(UART5->BitSR.TXE == 1) {
            if(s_staDev[4].stTx.dwLen == 1) UART5->BitCR1.TXEIE = 0;
            UART5->DR = *s_staDev[4].stTx.pbyAddr ++;
            s_staDev[4].stTx.dwLen --;
        }
    }
#endif

    if(UART5->BitSR.LBD) UART5->BitSR.LBD = 0;
    if(UART5->BitSR.ORE) UART5->BitSR.ORE = 0;
    if(UART5->BitSR.RXNE == 1) {
        UART5->BitSR.RXNE = 0;
        byDat = UART5->DR;
        if(s_staDev[4].pfHandler) s_staDev[4].pfHandler( byDat);
    }
}
#endif

#if(PLAT_STM32_F2)
    #if(CONFIG_USART6)
    void USART6_IRQHandler(void)
    {
        unsigned char byDat = 0;
    #if(CONFIG_USART_IT)
        if(s_staDev[5].stTx.dwLen) {
            if(USART6->BitSR.TXE == 1) {
                if(s_staDev[5].stTx.dwLen == 1) USART6->BitCR1.TXEIE = 0;
                USART1->DR = *s_staDev[5].stTx.pbyAddr ++;
                s_staDev[5].stTx.dwLen --;
            }
        }
    #endif

        if(USART6->BitSR.RXNE) {
            USART6->BitSR.RXNE = 0;
            byDat = USART6->DR;
            if(s_staDev[5].pfHandler) s_staDev[5].pfHandler( byDat);
        }
    }
    #endif
#endif

#if(PLAT_STM32_F4)
    #if(CONFIG_USART6)
    void USART6_IRQHandler(void)
    {
        unsigned char byDat = 0;
    #if(CONFIG_USART_IT)
        if(s_staDev[5].stTx.dwLen) {
            if(USART6->BitSR.TXE == 1) {
                if(s_staDev[5].stTx.dwLen == 1) USART6->BitCR1.TXEIE = 0;
                USART1->DR = *s_staDev[5].stTx.pbyAddr ++;
                s_staDev[5].stTx.dwLen --;
            }
        }
    #endif

        if(USART6->BitSR.RXNE) {
            USART6->BitSR.RXNE = 0;
            byDat = USART6->DR;
            if(s_staDev[5].pfHandler) s_staDev[5].pfHandler( byDat);
        }
    }
    #endif

    #if(CONFIG_USART7)
    void UART7_IRQHandler(void)
    {
        unsigned char byDat = 0;
    #if(CONFIG_USART_IT)
        if(s_staDev[6].stTx.dwLen) {
            if(UART7->BitSR.TXE == 1) {
                if(s_staDev[6].stTx.dwLen == 1) UART7->BitCR1.TXEIE = 0;
                UART7->DR = *s_staDev[6].stTx.pbyAddr ++;
                s_staDev[6].stTx.dwLen --;
            }
        }
    #endif

        if(UART7->BitSR.LBD) UART7->BitSR.LBD = 0;
        if(UART7->BitSR.ORE) UART7->BitSR.ORE = 0;
        if(UART7->BitSR.RXNE == 1) {
            UART7->BitSR.RXNE = 0;
            byDat = UART7->DR;
            if(s_staDev[6].pfHandler) s_staDev[6].pfHandler( byDat);
        }
    }
    #endif

    #if(CONFIG_USART5)
    void UART8_IRQHandler(void)
    {
        unsigned char byDat = 0;
    #if(CONFIG_USART_IT)
        if(s_staDev[7].stTx.dwLen) {
            if(UART8->BitSR.TXE == 1) {
                if(s_staDev[7].stTx.dwLen == 1) UART8->BitCR1.TXEIE = 0;
                UART8->DR = *s_staDev[7].stTx.pbyAddr ++;
                s_staDev[7].stTx.dwLen --;
            }
        }
    #endif

        if(UART8->BitSR.LBD) UART8->BitSR.LBD = 0;
        if(UART8->BitSR.ORE) UART8->BitSR.ORE = 0;
        if(UART8->BitSR.RXNE == 1) {
            UART8->BitSR.RXNE = 0;
            byDat = UART5->DR;
            if(s_staDev[7].pfHandler) s_staDev[7].pfHandler( byDat);
        }
    }
    #endif
#endif

#if(_DLIB_FILE_DESCRIPTOR)
int fputc(int ch, FILE *f)
{
    while(USART1->BitSR.TXE == 0){;}

    USART1->DR = ch;
    return (ch);
}

int fgetc(FILE *f)
{
	while(USART1->BitSR.RXNE == 0) {;}
	return (int)USART1->DR;
}
#endif

/*******************************************************************************
函数功能：
*   @ 通过设备号初始化相关的设备参数
参数描述：
*   @ unsigned int dwDev：需要调用的设备
*   @ void (*pvHandler)(unsigned char)： 带参数的函数地址
返回值描述：
*   @ 无
说明：
*   @ 中断函数在接受到数据后将自动调用此注册的函数接口。
*******************************************************************************/
static void DevInit(void)
{
#if(CONFIG_USART1)
    s_staDev[0].pstUart = USART1;
    s_staDev[0].pfHandler = 0;
    s_staDev[0].byDmaNo = 0;//dma1
    s_staDev[0].byDmaChn = 4;
#endif
#if(CONFIG_USART2)
    s_staDev[1].pstUart = USART2;
    s_staDev[1].pfHandler = 0;
    s_staDev[1].byDmaNo = 0;//dma1
    s_staDev[1].byDmaChn = 7;
#endif
#if(CONFIG_USART3)
    s_staDev[2].pstUart = USART3;
    s_staDev[2].pfHandler = 0;
    s_staDev[2].byDmaNo = 0;//dma1
    s_staDev[2].byDmaChn = 2;
#endif
#if(CONFIG_USART4)
    s_staDev[3].pstUart = UART4;
    s_staDev[3].pfHandler = 0;
    s_staDev[3].byDmaNo = 1;//dma2
    s_staDev[3].byDmaChn = 5;
#endif
#if(CONFIG_USART5)
    s_staDev[4].pstUart = UART5;
    s_staDev[4].pfHandler = 0;
    s_staDev[4].byDmaNo = 1;//dma2
    s_staDev[4].byDmaChn = 5;
#endif

    #if(PLAT_STM32_F2)
#if(CONFIG_USART6)
    s_staDev[5].pstUart = USART6;
    s_staDev[5].pfHandler = 0;
    s_staDev[5].byDmaNo = 1;//dma2
    s_staDev[5].byDmaChn = 5;
#endif
#endif

#if(PLAT_STM32_F4)
    #if(CONFIG_USART6)
    s_staDev[5].pstUart = USART6;
    s_staDev[5].pfHandler = 0;
    s_staDev[5].byDmaNo = 1;//dma2
    s_staDev[5].byDmaChn = 5;
    #endif

    #if(CONFIG_USART7)
    s_staDev[6].pstUart = UART7;
    s_staDev[6].pfHandler = 0;
    s_staDev[6].byDmaNo = 0;//dma1
    s_staDev[6].byDmaChn = 5;
    #endif

    #if(CONFIG_USART8)
    s_staDev[7].pstUart = UART8;
    s_staDev[7].pfHandler = 0;
    s_staDev[7].byDmaNo = 0;//dma1
    s_staDev[7].byDmaChn = 5;
#endif
#endif
}

static const DevUartTdf s_Dev = {
    Config,
    Config2,
#if(CONFIG_USART_DMA)
    ConfigDma,
#endif
    SendByte,
    RxCmd,
#if(CONFIG_USART_DMA)
    SendStringDma,
#endif

#if(CONFIG_USART_IT)
    SendStringIt,
#endif

    SendString,
    RegHandler,
};

/*******************************************************************************
函数功能：
*   @ 初始化串口设备
参数描述：
*   @ void (*pvGpioInit)(unsigned char)：gpio初始化
*   @ unsigned char byDat：gpio初始化时需要带入的参数
返回值描述：
*   @ 串口设备提供的操作接口
说明：
*   @ 无
*******************************************************************************/
const DevUartTdf * UsartInit ( void )
{
    RccInit();
    DevInit();
    NvicInit();
#if(CONFIG_USART1)
    Config(0, 115200, 1, 0, false);
#endif
#if(CONFIG_USART2)
    Config(1, 9600, 1, 0, false);
#endif
#if(CONFIG_USART3)
    Config(2, 9600, 1, 0, false);
#endif
#if(CONFIG_USART4)
    Config(3, 9600, 1, 0, false);
#endif
#if(CONFIG_USART5)
    Config(4, 9600, 1, 0, false);
#endif

#if(PLAT_STM32_F2)
#if(CONFIG_USART6)
    Config(5, 9600, 1, 0, false);
#endif
#endif

#if(PLAT_STM32_F4)
    #if(CONFIG_USART6)
        Config(5, 9600, 1, 0, false);
    #endif
    #if(CONFIG_USART7)
        Config(6, 9600, 1, 0, false);
    #endif
    #if(CONFIG_USART8)
        Config(7, 9600, 1, 0, false);
    #endif
#endif

    return &s_Dev;
}
#endif