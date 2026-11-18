/*******************************************************************************
文件名称：   adc.c
文件目录：   $PROJ_DIR$/../code/usr/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.10
程序用途：   基于stm32 adc模块的通用驱动

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

#if(PLAT_STM32_F1|PLAT_GD32_F30X)
#if(CONFIG_ADC1|CONFIG_ADC2|CONFIG_ADC3)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/* AI-FIX H-05 2026-11-14
   ADC 三个等待循环(复位/校准/EOC)的最长空转次数上限。
   按 72MHz 内核、每次循环约 6~10 个时钟估算，50000 次约 5~7ms；
   而实际一次完整转换(239.5+12.5 周期 @14MHz)约 18us、校准约 6us，
   余量 >250 倍，足以覆盖最坏情况，仅用于把"永久死等"降级为故障返回。 */
#define ADC_WAIT_MAX_LOOP   50000

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
static struct {
    unsigned char byDmaNo;
    unsigned char byDmaChn;

    unsigned char byChnNum;
}s_staDev[3];

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
    启动adc时钟
参数描述：
    无
返回值描述：
    无
说明：
    无
*******************************************************************************/
static void RccInit(void)
{
#if(CONFIG_ADC1)
    RCC->BitAPB2ENR.ADC1EN = 1;
#endif
#if(CONFIG_ADC2)
    RCC->BitAPB2ENR.ADC2EN = 1;
#endif
#if(CONFIG_ADC3)
    RCC->BitAPB2ENR.ADC3EN = 1;
#endif
}

/*******************************************************************************
函数功能：
*   @ 配置单通道的ADC转换
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
返回值描述：
*   @ 无
说明：
*   @ 本方法只适合对单个ADC使用1个转换通道的情况使用，对于单个ADC需要转换多个通道不适合
*******************************************************************************/
static void Config(unsigned int dwDev, unsigned char byDualMode, bool blScanMode)
{
    ADC_TypeDef *pstAdc;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    pstAdc->CR1 = 0;
    pstAdc->BitCR1.DUALMOD = byDualMode;
    pstAdc->BitCR1.SCAN = (blScanMode)?1:0;     //使用扫描模式

    pstAdc->CR2 = 0;
    pstAdc->BitCR2.ALIGN = 0;       //右对齐
    pstAdc->BitCR2.CONT = 0;        //单次转换模式
    pstAdc->BitCR2.EXTSEL = 7;      //SWSTART
    pstAdc->BitCR2.EXTTRIG = 1;     //用外部事件启动转换
}

/*******************************************************************************
函数功能：
*   @ 清除已经增加的adc通道
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
*   @ unsigned short int byChn： GPIO通道 0：ch0; 1:ch1...
*   @ unsigned char bySmpTim：该通道的采样时间
返回值描述：
*   @ 无
说明：
*   @ 如果需要使用ADC功能，必须先调用方法想ADC增加转换通道，可连续增加通道，其数据输出将按照增加的
*   @ 顺序输出
*******************************************************************************/
static int ClrChn(unsigned int dwDev)
{
    ADC_TypeDef *pstAdc;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    s_staDev[dwDev].byChnNum = 0;
    pstAdc->BitSQR1.L = 0;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 向ADC增加通道
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
*   @ unsigned short int byChn： GPIO通道 0：ch0; 1:ch1...
*   @ unsigned char bySmpTim：该通道的采样时间
返回值描述：
*   @ 无
说明：
*   @ 如果需要使用ADC功能，必须先调用方法想ADC增加转换通道，可连续增加通道，其数据输出将按照增加的
*   @ 顺序输出
*******************************************************************************/
static int AddChn(unsigned int dwDev, unsigned char byChn, unsigned char bySmpTim)
{
    ADC_TypeDef *pstAdc;
    unsigned int dwLen = 0;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    if(s_staDev[dwDev].byChnNum >=16) return -1;
    dwLen = s_staDev[dwDev].byChnNum;
    byChn &= 0x1f;

    if(dwLen <= 5) pstAdc->SQR3 = ((unsigned int)byChn << (dwLen * 5)) + (pstAdc->SQR3 & ~((unsigned int)0x1f << (dwLen * 5)));
    else if(dwLen <= 11) pstAdc->SQR2 = ((unsigned int)byChn << ((dwLen - 6) * 5)) + (pstAdc->SQR2 & ~((unsigned int)0x1f << ((dwLen - 6) * 5)));
    else pstAdc->SQR1 = ((unsigned int)byChn << ((dwLen - 12) * 5)) + (pstAdc->SQR1 & ~((unsigned int)0x1f << ((dwLen - 12) * 5)));
    pstAdc->BitSQR1.L += 1;

    if(byChn <= 9) pstAdc->SMPR2 = (pstAdc->SMPR2 & ~((unsigned int)0x07 << (byChn * 3))) + ((unsigned int)bySmpTim << (byChn * 3));
    else pstAdc->SMPR1 = (pstAdc->SMPR1 & ~((unsigned int)0x07 << ((byChn - 10) * 3))) + ((unsigned int)(bySmpTim) << ((byChn - 10) * 3));

    pstAdc->BitSQR1.L = s_staDev[dwDev].byChnNum;
    s_staDev[dwDev].byChnNum ++;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 启动连续的ADC转换
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
返回值描述：
*   @ 无
说明：
*   @ 本方法强烈建议在只有一个adc通道工作的情况下使用，即adc一直在工作转换
*******************************************************************************/
static void ContinuousAdOn(unsigned int dwDev)
{
    ADC_TypeDef *pstAdc;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    pstAdc->BitCR2.CONT = 1;
    pstAdc->BitCR2.ADON = 1;     //开启adc

    pstAdc->BitCR2.RSTCAL = 1;   //复位
    while(pstAdc->BitCR2.RSTCAL == 1) {;}
    pstAdc->BitCR2.CAL = 1;      //校准
    while(pstAdc->BitCR2.CAL == 1) {;}
}

/*******************************************************************************
函数功能：
*   @ 关闭连续的ADC转换
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
返回值描述：
*   @ 无
说明：
*   @ 本方法强烈建议在只有一个adc通道工作的情况下使用，即adc一直在工作转换
*******************************************************************************/
static void ContinuousAdOff(unsigned int dwDev)
{
    ADC_TypeDef *pstAdc;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    pstAdc->BitCR2.CONT = 0;
    pstAdc->BitCR2.ADON = 0;     //关闭adc
}

/*******************************************************************************
函数功能：
*   @ 检查ADC是否忙
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
返回值描述：
*   @ true ：dma忙
*   @ false ；dma空闲
说明：
*   @ 无
*******************************************************************************/
static bool IsBusy(unsigned int dwDev)
{
    ADC_TypeDef *pstAdc;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    if(pstAdc->BitSR.EOC) return false;
    return true;
}

#if(CONFIG_ADC_DMA)
/*******************************************************************************
函数功能：
*   @ 检查ADC是否忙
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
*   @ unsigned int dwDmaDev ：使用的Dma设备号
*   @ unsigned int dwDmaChn ：Dma使用的通道
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int ConfigDma(unsigned int dwDev, unsigned int dwDmaDev, unsigned int dwDmaChn)
{
    ADC_TypeDef *pstAdc;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    DevDma->ConfigP2M(dwDmaDev, dwDmaChn, 1, 1); //数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChn = dwDmaChn;
    pstAdc->BitCR2.DMA = 1;//使用DMA模式

    return 0;
}
#endif

/*******************************************************************************
函数功能：
*   @ 读取dma单通道转换结果
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
返回值描述：
*   @ dma转换结果数据
说明：
*   @ 无
*******************************************************************************/
static unsigned short int ReadOne(unsigned int dwDev, unsigned int dwChn, unsigned char bySmpTim, unsigned int dwDoTims)
{
    ADC_TypeDef *pstAdc;
    unsigned int dwTmp = 0;
    unsigned int dwWait = 0;                    /* AI-FIX H-05 等待计数器 */

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    /* AI-FIX H-05: 原实现对 dwDoTims 无保护，传入 0 会在末尾 %dwDoTims 除零。 */
    if(dwDoTims == 0) dwDoTims = 1;

    ClrChn(dwDev);
    AddChn( dwDev, dwChn, bySmpTim);

    pstAdc->BitCR2.CONT = 1;
    pstAdc->BitCR1.SCAN = 1;
    pstAdc->BitCR2.ADON = 1;                    //开启adc

    /* AI-FIX H-05 2026-11-14
       原实现三处 while(...){}; 为无超时裸死等，一旦 ADC 时钟未使能、
       通道配置异常或 EOC 被并发读取清掉，该任务将永久卡死在本函数内，
       加热输出则由 PWM 的 dwStep 保活机制继续维持(最长 50000 周期
       = 83 分钟)，形成无保护加热。
       修正  : 每次等待加次数上限；超时后关闭 ADON 并返回 0。
               上层 tempfan.c 的中值判定中 0 <= OPEN_VALUE(10)，
               会转成 3000(300.0C) 哨兵并触发停机，方向是安全的。 */
    pstAdc->BitCR2.RSTCAL = 1;                  //复位
    dwWait = 0;
    while(pstAdc->BitCR2.RSTCAL == 1)           //等待复位完毕
    {
        if(++dwWait > ADC_WAIT_MAX_LOOP) { pstAdc->BitCR2.ADON = 0; return 0; }
    }
    pstAdc->BitCR2.CAL = 1;                     //校准
    dwWait = 0;
    while(pstAdc->BitCR2.CAL == 1)              //等待校准完毕
    {
        if(++dwWait > ADC_WAIT_MAX_LOOP) { pstAdc->BitCR2.ADON = 0; return 0; }
    }

    pstAdc->BitCR2.SWSTART = 1;
    dwWait = 0;
    while(pstAdc->BitSR.EOC == 0)               //等待转换结束
    {
        if(++dwWait > ADC_WAIT_MAX_LOOP) { pstAdc->BitCR2.ADON = 0; return 0; }
    }

    for(int i = 0; i < dwDoTims; i ++) {
        pstAdc->BitCR2.SWSTART = 1;
        dwWait = 0;
        while(pstAdc->BitSR.EOC == 0)           //等待转换结束
        {
            if(++dwWait > ADC_WAIT_MAX_LOOP) { pstAdc->BitCR2.ADON = 0; return 0; }
        }
        dwTmp += pstAdc->DR;
    }

    pstAdc->BitCR2.ADON = 0;                    //关闭AD转换器
    dwTmp /= dwDoTims;

    return (unsigned short int)dwTmp;
}

#if(CONFIG_ADC_DMA)
/*******************************************************************************
函数功能：
*   @ 读取dma多个通道转换结果
参数描述：
*   @ unsigned int dwDev： ADC1口 0：ADC2，1：ADC3口 2
*   @ unsigned short int *pwOut ： 数据输出地址
返回值描述：
*   @ 无
说明：
*   @ 此方法是用于使用dma对多通道的数据进行连续转换, 由于adc第一个数据可能是不确定的数据
*******************************************************************************/
static int ReadMultiple(unsigned int dwDev, unsigned short int *pwOut)
{
    ADC_TypeDef *pstAdc;
    unsigned short int wLastDat;
    unsigned int dwLen = 0;

    unsigned char byDmaNo;
    unsigned char byDmaChn;

    if(dwDev > 2) return -1;
    byDmaNo = s_staDev[dwDev].byDmaNo;
    byDmaChn = s_staDev[dwDev].byDmaChn;

    if(dwDev == 0) pstAdc = ADC1;
    else if(dwDev == 1) pstAdc = ADC2;
    else pstAdc = ADC3;

    pstAdc->BitCR1.SCAN = 1;
    pstAdc->BitCR2.ADON = 1;                    //开启adc
    pstAdc->BitCR2.RSTCAL = 1;                  //复位
    while(pstAdc->BitCR2.RSTCAL == 1) {;}      //等待复位完毕
    pstAdc->BitCR2.CAL = 1;                     //校准
    while(pstAdc->BitCR2.CAL == 1) {;}         //等待校准完毕

    pstAdc->BitCR2.SWSTART = 1;
    while(pstAdc->BitSR.EOC == 0) {;}          //等待转换结束

    DevDma->Trs(byDmaNo, byDmaChn, (unsigned int )&pstAdc->DR, (unsigned int)pwOut, pstAdc->BitSQR1.L + 1);
    pstAdc->BitCR2.SWSTART = 1;
    while(DevDma->IsBusy(byDmaNo, byDmaChn)) {;}
    pstAdc->BitCR2.SWSTART = 0;
    pstAdc->BitCR2.ADON = 0;                    //关闭AD转换器

    dwLen = pstAdc->BitSQR1.L;                  //处理adc第一个数据不是正常数据问题。
    wLastDat = pwOut[0];
    for(int i = 0; i < dwLen; i ++) pwOut[i] = pwOut[i + 1];
    pwOut[dwLen] = wLastDat;

    return 0;
}
#endif

/*******************************************************************************
函数功能：
*   @ 设置RCC CLK 时钟
参数描述：
*   @ 无
返回值描述：
*   @ 时钟分频
说明：
*   @ 无
*******************************************************************************/
static void SetPRE(unsigned int dwPRE)
{
    RCC->BitCFGR.ADCPRE = (dwPRE & 0x03);
}

static const DevAdcTdf s_Dev = {
    Config,
    ClrChn,
    AddChn,
    ContinuousAdOn,
    ContinuousAdOff,
    IsBusy,
#if(CONFIG_ADC_DMA)
    ConfigDma,
    ReadMultiple,
#endif
    ReadOne,

#if(CONFIG_ADC_SET_PRE)
    SetPRE,
#endif
};

/*******************************************************************************
函数功能：
*   @ 初始化ADC模块
参数描述：
*   @ 无
返回值描述：
*   @ ADC模块的操作方法集合
说明：
*   @ 无
*******************************************************************************/
const DevAdcTdf *AdcInit(void)
{
    RccInit();
    SetPRE(PRE_PCLK2_DIV4);

    return &s_Dev;
}
#endif
#endif