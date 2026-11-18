/*******************************************************************************
文件名称：  Thermistor.c
文件目录：
项目名称：
创建者：     廖江伟
创建日期：   2020.12.22
程序用途：   热敏电阻驱动

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"
/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
#define CONFIG_THERM_DEBUG 0

 //均值滤波配置选项  防止采样不稳数值跳变
#define FilterLen         15


struct
{
    uint32_t wMaxRange;//最大量程数值
    uint32_t ForeDivideRes;//前级分压电阻
    uint32_t wCurRes;//当前电阻
}ThermistorCtl;

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
//热敏电阻RT表，第一位为0℃，每格跨度1℃
uint32_t MCGF_103F_RT_TABLE[] =
{
    32754,31124,29584,28131,26755,25456,24227,23065,21965,20923,//0-9
    19938,19004,18119,17280,16485,15731,15016,14337,13693,13081,//10-19
    12500,11948,11423,10925,10451,10000, 9570, 9162, 8773, 8403,//20-29
     8051, 7715, 7395 ,7090, 6799, 6522, 6257, 6005, 5764, 5534,//30-39
     5314, 5104, 4904, 4713, 4530, 4355, 4187, 4027, 3874, 3728,//40-49
     3588, 3454, 3325, 3202, 3084, 2971, 2863, 2759, 2660, 2564,//50-59
     2473, 2385, 2301, 2220, 2143, 2068, 1997, 1928, 1862, 1798,//60-69
     1738, 1679, 1623, 1568, 1516, 1466, 1418, 1372, 1327, 1284,//70-79
     1243, 1203, 1164, 1127, 1092, 1058, 1024,  993,  962,  932,//80-89
      904,  876,  849,  824,  799,  775,  752,  730,  708,  687,//90-99


};



/*******************************************************************************
宏定义函数声明
*******************************************************************************/

/*******************************************************************************
输出函数声明
*******************************************************************************/


/*****************************************************************************
函数名：uint32_t CalcRes(uint16_t CurAdVal)
函数功能：计算阻值
参数描述：
uint16_t CurAdVal:      当前AD采样值
返回值描述：
特殊算法：…
注意事项：此函数只适用于测量两电阻分压，并且待测电阻靠近地的电路的电阻值
******************************************************************************/
static uint32_t CalcRes(uint16_t CurAdVal)
{
    uint32_t wRes;

    wRes =  ( ThermistorCtl.ForeDivideRes * CurAdVal ) / ( ThermistorCtl.wMaxRange - CurAdVal );

#if(CONFIG_THERM_DEBUG)

    char str[200];
    int len;

    sprintf(str,"AD:%d Res:%d\r\n",CurAdVal,wRes);
    len = strlen(str);
    DevUart->SendString(0,(u8*)str,len);

#endif
    return wRes;
}


/*****************************************************************************
函数名：static short signed int GetTemperature( uint16_t CurAdVal )
函数功能：查表获取温度值
参数描述：
uint16_t CurAdVal :ADC采样值
返回值描述：
特殊算法：…
注意事项：
此函数将获取电阻值与获得温度整合到一个函数中。

uint32_t wRes       ：电阻值
uint32_t *RT_Table  : 热敏电阻RT表
返回温度值 * 10 ,例如23℃返回230
******************************************************************************/
static bool GetTemperature( uint16_t CurAdVal ,short signed int *hwTemperature)
{
    uint32_t wRes;
    wRes = CalcRes(CurAdVal);

    for(short signed int i = 0 ; i < 100; i++)
    {
        if ( wRes > MCGF_103F_RT_TABLE[0] )     return false;
        else if(wRes > MCGF_103F_RT_TABLE[i])
        {

            *hwTemperature = i * 10;
            return hwTemperature;
        }
    }
    return false;

}


/*****************************************************************************
函数名：static short signed int GetTemperature( uint16_t CurAdVal )
函数功能：查表获取温度值
参数描述：
uint16_t CurAdVal :ADC采样值
short signed int hwTemperature: 温度值
返回值描述：
特殊算法：…
注意事项：
此函数将获取电阻值与获得温度整合到一个函数中。

uint32_t wRes       ：电阻值
uint32_t *RT_Table  : 热敏电阻RT表

数值乘10保留一位小数，例如23.1℃ 返回为231


******************************************************************************/
static bool GetTemperatureDecile( uint16_t CurAdVal ,short signed int *hwTemperature)
{
    uint32_t wRes;
    wRes = CalcRes(CurAdVal);
    short signed int sTemp;
    short signed int sDecile;

    for(short signed int i = 0 ; i < 99; i++)
    {
        if ( wRes > MCGF_103F_RT_TABLE[0] )     return false;
        else if(wRes > MCGF_103F_RT_TABLE[i])
        {
            sTemp = i * 10;
            sDecile = ( wRes - MCGF_103F_RT_TABLE[i] ) * 10 / ( MCGF_103F_RT_TABLE[i-1] - MCGF_103F_RT_TABLE[i] );
            sTemp = sTemp + sDecile;

            *hwTemperature = sTemp ;
            return true;

        }
    }
    return false;

}




short signed int TempFilter(short signed int hwTemperature)
{
    static char Cnt;
    static short signed int hwTemp[FilterLen];
    short signed int AvrTemp = 0;


    hwTemp[Cnt] = hwTemperature;
    Cnt++;
    if(Cnt >= FilterLen) Cnt = 0;

    for(int i = 0 ; i < FilterLen ; i++)
    {
        AvrTemp = AvrTemp + hwTemp[i];
    }

    return AvrTemp/FilterLen;

}

void TempFilterInit(short signed int hwTemperature)
{
    for(int i = 0 ; i < FilterLen ; i++)
    {
        TempFilter(hwTemperature);
    }

}

DevThermistorTdf s_Dev =
{
    GetTemperature,
    GetTemperatureDecile,
    TempFilterInit,
    TempFilter,
};

/*****************************************************************************
const DevThermistorTdf *ThermistorDevInit(uint32_t wMaxRange,uint32_t ForeDivideRes )
函数功能：热敏电阻初始化

参数描述：
uint32_t wMaxRange:最大量程，16位传入0xFFFF,12位传入0xFFF
uint32_t ForeDivideRes:前级分压电阻，单位欧姆
返回值描述：
特殊算法：…
注意事项：
此函数仅能处理分压电阻比待测电阻更靠近电源的电路

******************************************************************************/
const DevThermistorTdf *ThermistorDevInit(uint32_t wMaxRange,uint32_t ForeDivideRes )
{
    ThermistorCtl.wMaxRange = wMaxRange;
    ThermistorCtl.ForeDivideRes  = ForeDivideRes;

    return &s_Dev;
}





