/*******************************************************************************
文件名称：  usart.h
文件目录：  $PROJ_DIR$/usr/Drive/inc
项目名称：  事业一部技术平台
创建者：    胥强
创建日期：  2016.03.08
程序用途：  串口驱动文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __DAC_H
#define __DAC_H

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/ 
#define DAC_CHN_1                       (0)
#define DAC_CHN_2                       (1)
#define DAC_CHN_NUM                     (2)

#define Trigger_T6_TRGO                0
#define Trigger_T38_TRGO               1
#define Trigger_T7_TRGO                2
#define Trigger_T5_TRGO                3
#define Trigger_T2_TRGO                4
#define Trigger_T4_TRGO                5
#define Trigger_Ext_IT9                6
#define Trigger_Software               7

#define WaveGeneration_None            0
#define WaveGeneration_Noise           1
#define WaveGeneration_Triangle        2

#define LFSRUnmask_Bit0                0
#define LFSRUnmask_Bits1_0             1
#define LFSRUnmask_Bits2_0             2
#define LFSRUnmask_Bits3_0             3
#define LFSRUnmask_Bits4_0             4
#define LFSRUnmask_Bits5_0             5
#define LFSRUnmask_Bits6_0             6
#define LFSRUnmask_Bits7_0             7
#define LFSRUnmask_Bits8_0             8
#define LFSRUnmask_Bits9_0             9
#define LFSRUnmask_Bits10_0            10
#define LFSRUnmask_Bits11_0            11
#define TriangleAmplitude_1            0
#define TriangleAmplitude_3            1
#define TriangleAmplitude_7            2
#define TriangleAmplitude_15           3
#define TriangleAmplitude_31           4
#define TriangleAmplitude_63           5
#define TriangleAmplitude_127          6
#define TriangleAmplitude_255          7
#define TriangleAmplitude_511          8
#define TriangleAmplitude_1023         9
#define TriangleAmplitude_2047         10
#define TriangleAmplitude_4095         11

#define OutputBuffer_Enable            0
#define OutputBuffer_Disable           1

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    void (*Config) (unsigned int dwDev, unsigned int Trigger, unsigned int Wave, unsigned int LFSRUnmask_TriangleAmp, unsigned int OutBuffer);
    #if(CONFIG_DAC_DMA)
    void (*ConfigDma)(unsigned int dwDev, unsigned short int *pwDat, unsigned int len);
    #endif
    int (*WriteOn) (unsigned int dwDev, unsigned short int wDat);
    int (*Down) (unsigned int dwDev);
}DevDacTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevDacTdf* DevDac;
/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevDacTdf *DacInit(void);

#endif