/*******************************************************************************
文件名称：   adc.c
文件目录：   $PROJ_DIR$/../code/usr/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.10
程序用途：   基于stm32 dac模块的通用驱动

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "dev.h"

#if(CONFIG_DAC)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

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

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 启动adc时钟
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void RccInit(void)
{
#if(CONFIG_DAC)
    RCC->BitAPB1ENR.DACEN = 1;
#endif
}

/*******************************************************************************
函数功能：
*   @ 配置单通道的Dac转换
参数描述：
*   @ unsigned int dwDev： DAC1口 0  DAC2，1
*   @ unsigned int Trigger ： dac触发源
*   @ unsigned char int Wave ： 使用波形
*   @ unsigned int LFSRUnmask_TriangleAmp ：幅度
*   @ unsigned int OutBuffer ：是否使用输出缓冲
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void Config(unsigned int dwDev, unsigned int Trigger, unsigned int Wave, unsigned int LFSRUnmask_TriangleAmp, unsigned int OutBuffer)
{
    if(dwDev == DAC_CHN_1) {
        DAC->BitCR.TSEL1 = Trigger;
        DAC->BitCR.WAVE1 = Wave;
        DAC->BitCR.MAMP1 = LFSRUnmask_TriangleAmp;
        DAC->BitCR.BOFF1 = OutBuffer;
    }
    else {
        DAC->BitCR.TSEL2 = Trigger;
        DAC->BitCR.WAVE2 = Wave;
        DAC->BitCR.MAMP2 = LFSRUnmask_TriangleAmp;
        DAC->BitCR.BOFF2 = OutBuffer;
    }
}

#if(CONFIG_DAC_DMA)
/*******************************************************************************
函数功能：
*   @ 配置单通道的Dac转换DMA
参数描述：
*   @ unsigned int dwDev： DAC1口 0  DAC2，1
*   @ unsigned short int *pwDat:DMA数据缓存
*   @ unsigned int len:DMA数据缓存长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void ConfigDma(unsigned int dwDev, unsigned short int *pwDat, unsigned int len)
{
    if(dwDev == DAC_CHN_1) {
        DevDma->ConfigM2P(DMA_DEV_1, DMA_STREAM_5, DMA_CHN_7, DMA_Circular_Mode, DMA_MSIZE_HALFWORD, DMA_PSIZE_HALFWORD);
        DevDma->Trs(DMA_DEV_1, DMA_STREAM_5, (unsigned int)&(DAC->DHR12R1), (unsigned int)pwDat, len);
        DAC->BitCR.DMAEN1 = 1;
        DAC->BitCR.TEN1 = 1;
        DAC->BitCR.EN1 = 1;
    }
    else {
        DevDma->ConfigM2P(DMA_DEV_1, DMA_STREAM_6, DMA_CHN_7, DMA_Circular_Mode, DMA_MSIZE_HALFWORD, DMA_PSIZE_HALFWORD);
        DevDma->Trs(DMA_DEV_1, DMA_STREAM_6, (unsigned int)&(DAC->DHR12R2), (unsigned int)pwDat, len);
        DAC->BitCR.DMAEN2 = 1;
        DAC->BitCR.TEN2 = 1;
        DAC->BitCR.EN2 = 1;
    }
}
#endif

/*******************************************************************************
函数功能：
*   @ 关闭DAC通道输出
参数描述：
*   @ unsigned int dwDev： DAC1口 0：DAC2，1
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int WriteOn(unsigned int dwDev, unsigned short int wDat)
{
    if(!dwDev) {
        DAC->BitCR.EN1 = 0;
        DAC->DHR12R1 = wDat;
        DAC->BitCR.EN1 = 1;
        DAC->BitSWTRIGR.SWTRIG1 = 1;
    }
    else {
        DAC->BitCR.EN2 = 0;
        DAC->DHR12R2 = wDat;
        DAC->BitCR.EN2 = 1;
        DAC->BitSWTRIGR.SWTRIG2 = 1;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭DAC通道输出
参数描述：
*   @ unsigned int dwDev： DAC1口 0：DAC2，1
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int Down(unsigned int dwDev)
{
    if(!dwDev) DAC->BitCR.EN1 = 0;
    else DAC->BitCR.EN2 = 0;

    return 0;
}

static const DevDacTdf s_Dev = {
    Config,
    #if(CONFIG_DAC_DMA)
    ConfigDma,
    #endif
    WriteOn,
    Down,
};

/*******************************************************************************
函数功能：
*   @ 初始化DAC模块
参数描述：
*   @ 无
返回值描述：
*   @ DAC模块的操作方法集合
说明：
*   @ 无
*******************************************************************************/
const DevDacTdf *DacInit(void)
{
    RccInit();
    DAC->CR = 0;

    return &s_Dev;
}
#endif