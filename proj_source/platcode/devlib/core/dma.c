/*******************************************************************************
文件名称：   dma.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   201610308
程序用途：   用于dma的通用处理驱动

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

#if(PLAT_STM32_F1|PLAT_GD32_F30X)
#if(CONFIG_DMA1|CONFIG_DMA2)
/*******************************************************************************
常量定义
*******************************************************************************/
#define DMA_DIR_PeripheralDST              ((unsigned int)0x00000010)
#define DMA_DIR_PeripheralSRC              ((unsigned int)0x00000000)

#define DMA_PeripheralInc_Enable           ((unsigned int)0x00000040)
#define DMA_PeripheralInc_Disable          ((unsigned int)0x00000000)

#define DMA_MemoryInc_Enable               ((unsigned int)0x00000080)
#define DMA_MemoryInc_Disable              ((unsigned int)0x00000000)

#define DMA_PeripheralDataSize_Byte        ((unsigned int)0x00000000)
#define DMA_PeripheralDataSize_HalfWord    ((unsigned int)0x00000100)
#define DMA_PeripheralDataSize_Word        ((unsigned int)0x00000200)

#define DMA_MemoryDataSize_Byte            ((unsigned int)0x00000000)
#define DMA_MemoryDataSize_HalfWord        ((unsigned int)0x00000400)
#define DMA_MemoryDataSize_Word            ((unsigned int)0x00000800)

#define DMA_Mode_Circular                  ((unsigned int)0x00000020)
#define DMA_Mode_Normal                    ((unsigned int)0x00000000)

#define DMA_Priority_VeryHigh              ((unsigned int)0x00003000)
#define DMA_Priority_High                  ((unsigned int)0x00002000)
#define DMA_Priority_Medium                ((unsigned int)0x00001000)
#define DMA_Priority_Low                   ((unsigned int)0x00000000)

#define DMA_M2M_Enable                     ((unsigned int)0x00004000)
#define DMA_M2M_Disable                    ((unsigned int)0x00000000)

#define CCR_ENABLE_Set                     ((unsigned int)0x00000001)
#define CCR_ENABLE_Reset                   ((unsigned int)0xFFFFFFFE)

/* DMA1 Channelx interrupt pending bit masks */
#define DMA1_Channel1_IT_Mask    ((unsigned int)0x0000000F)
#define DMA1_Channel2_IT_Mask    ((unsigned int)0x000000F0)
#define DMA1_Channel3_IT_Mask    ((unsigned int)0x00000F00)
#define DMA1_Channel4_IT_Mask    ((unsigned int)0x0000F000)
#define DMA1_Channel5_IT_Mask    ((unsigned int)0x000F0000)
#define DMA1_Channel6_IT_Mask    ((unsigned int)0x00F00000)
#define DMA1_Channel7_IT_Mask    ((unsigned int)0x0F000000)

/* DMA2 Channelx interrupt pending bit masks */
#define DMA2_Channel1_IT_Mask    ((unsigned int)0x0000000F)
#define DMA2_Channel2_IT_Mask    ((unsigned int)0x000000F0)
#define DMA2_Channel3_IT_Mask    ((unsigned int)0x00000F00)
#define DMA2_Channel4_IT_Mask    ((unsigned int)0x0000F000)
#define DMA2_Channel5_IT_Mask    ((unsigned int)0x000F0000)

/* DMA2 FLAG mask */
#define FLAG_Mask                ((unsigned int)0x10000000)

/* DMA registers Masks */
#define CCR_CLEAR_Mask           ((unsigned int)0xFFFF800F)

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct
{
  unsigned int DMA_PeripheralBaseAddr;
  unsigned int DMA_MemoryBaseAddr;
  unsigned int DMA_DIR;
  unsigned int DMA_BufferSize;
  unsigned int DMA_PeripheralInc;
  unsigned int DMA_MemoryInc;
  unsigned int DMA_PeripheralDataSize;
  unsigned int DMA_MemoryDataSize;
  unsigned int DMA_Mode;
  unsigned int DMA_Priority;
  unsigned int DMA_M2M;
}DMA_InitTypeDef;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/

void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx)
{
    /* Disable the selected DMAy Channelx */
    DMAy_Channelx->CCR &= CCR_ENABLE_Reset;
    /* Reset DMAy Channelx control register */
    DMAy_Channelx->CCR  = 0;

    /* Reset DMAy Channelx remaining bytes register */
    DMAy_Channelx->CNDTR = 0;

    /* Reset DMAy Channelx peripheral address register */
    DMAy_Channelx->CPAR  = 0;

    /* Reset DMAy Channelx memory address register */
    DMAy_Channelx->CMAR = 0;
    switch (*(unsigned int*)&DMAy_Channelx)
    {
    case DMA1_Channel1_BASE:
        /* Reset interrupt pending bits for DMA1 Channel1 */
        DMA1->IFCR |= DMA1_Channel1_IT_Mask;
        break;
    case DMA1_Channel2_BASE:
        /* Reset interrupt pending bits for DMA1 Channel2 */
        DMA1->IFCR |= DMA1_Channel2_IT_Mask;
        break;
    case DMA1_Channel3_BASE:
        /* Reset interrupt pending bits for DMA1 Channel3 */
        DMA1->IFCR |= DMA1_Channel3_IT_Mask;
        break;
    case DMA1_Channel4_BASE:
        /* Reset interrupt pending bits for DMA1 Channel4 */
        DMA1->IFCR |= DMA1_Channel4_IT_Mask;
        break;
    case DMA1_Channel5_BASE:
        /* Reset interrupt pending bits for DMA1 Channel5 */
        DMA1->IFCR |= DMA1_Channel5_IT_Mask;
        break;
    case DMA1_Channel6_BASE:
        /* Reset interrupt pending bits for DMA1 Channel6 */
        DMA1->IFCR |= DMA1_Channel6_IT_Mask;
        break;
    case DMA1_Channel7_BASE:
        /* Reset interrupt pending bits for DMA1 Channel7 */
        DMA1->IFCR |= DMA1_Channel7_IT_Mask;
        break;
    case DMA2_Channel1_BASE:
        /* Reset interrupt pending bits for DMA2 Channel1 */
        DMA2->IFCR |= DMA2_Channel1_IT_Mask;
        break;
    case DMA2_Channel2_BASE:
        /* Reset interrupt pending bits for DMA2 Channel2 */
        DMA2->IFCR |= DMA2_Channel2_IT_Mask;
        break;
    case DMA2_Channel3_BASE:
        /* Reset interrupt pending bits for DMA2 Channel3 */
        DMA2->IFCR |= DMA2_Channel3_IT_Mask;
        break;
    case DMA2_Channel4_BASE:
        /* Reset interrupt pending bits for DMA2 Channel4 */
        DMA2->IFCR |= DMA2_Channel4_IT_Mask;
        break;
    case DMA2_Channel5_BASE:
        /* Reset interrupt pending bits for DMA2 Channel5 */
        DMA2->IFCR |= DMA2_Channel5_IT_Mask;
        break;

    default:
        break;
    }
}

static void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct)
{
    unsigned int tmpreg = 0;
    /*--------------------------- DMAy Channelx CCR Configuration -----------------*/
    /* Get the DMAy_Channelx CCR value */
    tmpreg = DMAy_Channelx->CCR;
    /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
    tmpreg &= CCR_CLEAR_Mask;
    /* Configure DMAy Channelx: data transfer, data size, priority level and mode */
    /* Set DIR bit according to DMA_DIR value */
    /* Set CIRC bit according to DMA_Mode value */
    /* Set PINC bit according to DMA_PeripheralInc value */
    /* Set MINC bit according to DMA_MemoryInc value */
    /* Set PSIZE bits according to DMA_PeripheralDataSize value */
    /* Set MSIZE bits according to DMA_MemoryDataSize value */
    /* Set PL bits according to DMA_Priority value */
    /* Set the MEM2MEM bit according to DMA_M2M value */
    tmpreg |= DMA_InitStruct->DMA_DIR | DMA_InitStruct->DMA_Mode |
        DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
            DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
                DMA_InitStruct->DMA_Priority | DMA_InitStruct->DMA_M2M;
    /* Write to DMAy Channelx CCR */
    DMAy_Channelx->CCR = tmpreg;
    /*--------------------------- DMAy Channelx CNDTR Configuration ---------------*/
    /* Write to DMAy Channelx CNDTR */
    DMAy_Channelx->CNDTR = DMA_InitStruct->DMA_BufferSize;
    /*--------------------------- DMAy Channelx CPAR Configuration ----------------*/
    /* Write to DMAy Channelx CPAR */
    DMAy_Channelx->CPAR = DMA_InitStruct->DMA_PeripheralBaseAddr;
    /*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
    /* Write to DMAy Channelx CMAR */
    DMAy_Channelx->CMAR = DMA_InitStruct->DMA_MemoryBaseAddr;
}

/*******************************************************************************
函数功能：
*   @ 主要是配置DMA内存到外设
参数描述：
*   @ unsigned int dwDev ：0-DMA1；1-DMA2
*   @ unsigned int dwChn ：x-通道x,从1到7
*   @ unsigned int dwMDatSize ：0-8位，1-16位，2-32位
*   @ unsigned int dwPDatSize ：0-8位，1-16位，2-32位
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ConfigM2P( unsigned int dwDev, unsigned int dwChn, unsigned int dwMDatSize, unsigned int dwPDatSize)
{
    DMA_TypeDef *pstDma;
    DMA_InitTypeDef  stInit;

    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    if(!dwChn) return -1;

    memset(&stInit, 0, sizeof(DMA_InitTypeDef));

    stInit.DMA_PeripheralBaseAddr = 0;
    stInit.DMA_MemoryBaseAddr = 0;

    stInit.DMA_DIR = DMA_DIR_PeripheralDST;
    stInit.DMA_BufferSize = 0;
    stInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    stInit.DMA_MemoryInc = DMA_MemoryInc_Enable;

    if(dwPDatSize == 0) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    else if(dwPDatSize == 1) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    else stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;

    if(dwMDatSize == 0) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//配置有误，现已改正
    else if(dwMDatSize == 1) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//配置有误，现已改正
    else stInit.DMA_PeripheralDataSize = DMA_MemoryDataSize_Word;//配置有误，现已改正

    stInit.DMA_Mode = DMA_Mode_Normal;
    stInit.DMA_Priority = DMA_Priority_Low;
    stInit.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(&pstDma->Stream[dwChn - 1], &stInit);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 主要是配置DMA外设到内存
参数描述：
*   @ unsigned int dwDev ：0-DMA1；1-DMA2
*   @ unsigned int dwChn ：x-通道x,从1到7
*   @ unsigned int dwMDatSize ：0-8位，1-16位，2-32位
*   @ unsigned int dwPDatSize ：0-8位，1-16位，2-32位
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ConfigP2M( unsigned int dwDev, unsigned int dwChn, unsigned int dwMDatSize, unsigned int dwPDatSize)
{
    DMA_TypeDef *pstDma;
    DMA_InitTypeDef  stInit;

    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    if(!dwChn) return -1;

    DMA_DeInit(&pstDma->Stream[dwChn - 1]);

    memset(&stInit, 0, sizeof(DMA_InitTypeDef));

    stInit.DMA_PeripheralBaseAddr = 0;
    stInit.DMA_MemoryBaseAddr = 0;

    stInit.DMA_DIR = DMA_DIR_PeripheralSRC;//外设作为数据来源
    stInit.DMA_BufferSize = 0;
    stInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设数据源地址不增加
    stInit.DMA_MemoryInc = DMA_MemoryInc_Enable;

    if(dwPDatSize == 0) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    else if(dwPDatSize == 1) stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    else stInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;

    if(dwMDatSize == 0) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//配置有误，现已改正
    else if(dwMDatSize == 1) stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//配置有误，现已改正
    else stInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//配置有误，现已改正。

    stInit.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;
    stInit.DMA_Priority = DMA_Priority_Low;//DMA_Priority_High;
    stInit.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(&pstDma->Stream[dwChn - 1], &stInit);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 通过DMA发送数据
参数描述：
*   @ unsigned int dwDev ：需要调用的设备
*   @ unsigned int dwChn ：x-通道x,从1到7
*   @ unsigned int dwPadd ： 外设的地址
*   @ unsigned int dwMadd ： 内存的地址
*   @ unsigned int dwLen ：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
static int Trs(unsigned int dwDev, unsigned int dwChn, unsigned int dwPadd, unsigned int dwMadd, unsigned int dwLen)
{
    DMA_TypeDef *pstDma;
    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    if(!dwChn) return -1;

    pstDma->Stream[dwChn-1].BitCCR.EN = 0;
    //    清除中断
    pstDma->IFCR |= (0x0f << ((dwChn-1)*4));
    pstDma->Stream[dwChn -1].CPAR = (unsigned int)dwPadd;
    pstDma->Stream[dwChn -1].CNDTR = (unsigned int)dwLen;
    pstDma->Stream[dwChn -1].CMAR = (unsigned int)dwMadd;
    pstDma->Stream[dwChn -1].BitCCR.EN = 1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 查询DMA是否在传输数据
参数描述：
*   @ unsigned int dwDev ：需要调用的设备
*   @ unsigned int dwChn ：x-通道x,从1到7
返回值描述：
*   @ true ：dma忙
*   @ false ；dma空闲
说明：
*   @ 通过查询dma的计数器判断是否存在数据传输。
*******************************************************************************/
static bool IsBusy(unsigned int dwDev, unsigned int dwChn)
{
    DMA_TypeDef *pstDma;

    if(!dwChn) return -1;

    if(dwDev == 0) pstDma = DMA1;
    else pstDma = DMA2;

    if(pstDma->Stream[dwChn -1].CNDTR) return true;
    return false;
}

static const DevDmaTdf s_Dev = {
    ConfigM2P,
    ConfigP2M,
    Trs,
    IsBusy,
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
const DevDmaTdf * DmaInit ( void )
{
#if(CONFIG_DMA1)
    RCC ->BitAHBENR.DMA1EN  = 1;
#endif
#if(CONFIG_DMA2)
    RCC ->BitAHBENR.DMA2EN  = 1;
#endif

    return &s_Dev;
}

#endif
#endif