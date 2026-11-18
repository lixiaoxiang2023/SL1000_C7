/******************************************************************************
文件名称：cis_dev.c
目录：$PROJ_DIR$/../code/dev
项目名称：urit-1600全自动尿液分析仪
创建者：  胥强
创建日期：20120810
程序用途：
用于CIS采样的驱动

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20120815：
第一次由XQ创建初始程序。

******************************************************************************/

/******************************************************************************
Include file头文件
******************************************************************************/
#include "ram.h"
#include "ad9822.h"
#include "tlc5922.h"
#include "sysdata.h"
#include "ff.h"
#include "cis.h"
#include "ngusfunc.h"


/******************************************************************************
Constant and Macro Definition 常量以及宏定义
******************************************************************************/
#define CIS_CH_R    (LED_5922_CH15 | LED_5922_CH14 | LED_5922_CH13)
#define CIS_CH_G    (LED_5922_CH12 | LED_5922_CH11 | LED_5922_CH10)
#define CIS_CH_B    (LED_5922_CH9 | LED_5922_CH8 | LED_5922_CH7)
#define CIS_CH_IR   (LED_5922_CH6 | LED_5922_CH5 | LED_5922_CH4)
#define CIS_CH_YG   (LED_5922_CH3 | LED_5922_CH2 | LED_5922_CH1 | LED_5922_CH0)
#define CIS_CH_ALL  LED_5922_CH_ALL


#define CIS_DECT_STATE                  ((GPIOE->IDR) & ((uint16_t)0x04))
//#define CIS_DECT_STATE                 ((GPIOD->IDR) & ((uint16_t)0x04))
#define BANLANCE_START                          128
#define BANLANCE_END                            1200
/******************************************************************************
全局变量定义
******************************************************************************/
uint8_t                                 g_byaWrFileBuff[512];
/******************************************************************************
外部变量定义
******************************************************************************/
extern uint16_t g_wCisMem[CIS_DATA_WIDTH][5][CIS_DATA_LENTH];
extern uint16_t g_wCisBlan[10][1216];
extern FATFS Fs;    // 逻辑驱动器的工作区(文件系统对象)
extern FIL f1;      // 文件对象
/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

/******************************************************************************
内部全局变量申明
******************************************************************************/
static LedChanelTypeDef                        s_stCisCurrent;


/******************************************************************************
内部函数申明
******************************************************************************/


/*****************************************************************************
函数名：void CisClean(void)
参数描述：
无
返回值描述：
无
特殊算法：
无
注意事项：
此函数用于清除CPLD内部数据，用以复位采样数据地址以及状态
******************************************************************************/
void CisClean(void)
{
    //清除cis以前存储的数据，将数据自动存入的数据地址清零
    REG_CPLD_CIS_CLR = 1;
    for(__IO uint32_t i = 0;i<0x0f;i++);//delay
    REG_CPLD_CIS_CLR = 0;
    //for(__IO uint32_t i = 0;i<0x0f;i++);//delay
    for(__IO int delay = 0; delay < 0xfff;delay ++);
}

/*****************************************************************************
函数名：void CisScanOnetime(void)
参数描述：
无
返回值描述：
无
特殊算法：
无
注意事项：
完成一次扫描
******************************************************************************/
void CisScanOneTim(void)
{
    REG_CPLD_CIS_START = 1;
	for(__IO int i = 0; i < 0x0A; i++){;}
    REG_CPLD_CIS_START = 0;
    for(__IO int i = 0; i < 0x0A; i++){;}
}

/*****************************************************************************
函数名：
U_bool CisGetBusyState(void)
参数描述：
无
返回值描述：
无
特殊算法：
无
注意事项：
取得检测是否正在继续状态
******************************************************************************/
int CisGetBusyState(void)
{
    __IO uint16_t tmp;
    tmp = CIS_DECT_STATE;
    return (tmp != 0)?1:0;
}

/*****************************************************************************
函数名：
void CisInitCurrentOn(void)
参数描述：
无
返回值描述：
无
特殊算法：
无
注意事项：
初始化CIS检测模块
******************************************************************************/
void CisInitCurrentOn(void)
{
    memset(&s_stCisCurrent,0,sizeof(LedChanelTypeDef));
    //R
    s_stCisCurrent.Chanl_15.byCurrent = 30;
	s_stCisCurrent.Chanl_14.byCurrent = 30;
    s_stCisCurrent.Chanl_13.byCurrent = 0;
    //G
	s_stCisCurrent.Chanl_12.byCurrent = 30;
	s_stCisCurrent.Chanl_11.byCurrent = 30;
    s_stCisCurrent.Chanl_10.byCurrent = 0;
    //B
	s_stCisCurrent.Chanl_9.byCurrent = 30;
	s_stCisCurrent.Chanl_8.byCurrent = 30;
    s_stCisCurrent.Chanl_7.byCurrent = 0;
    //IR
	s_stCisCurrent.Chanl_6.byCurrent = 30;
	s_stCisCurrent.Chanl_5.byCurrent = 30;
    s_stCisCurrent.Chanl_4.byCurrent = 0;
    //YG
	s_stCisCurrent.Chanl_3.byCurrent = 75;
	s_stCisCurrent.Chanl_2.byCurrent = 75;
    s_stCisCurrent.Chanl_1.byCurrent = 0;
    s_stCisCurrent.Chanl_0.byCurrent = 0;

    Tlc5922LedWrite(&s_stCisCurrent,CIS_CH_ALL);
}

/*****************************************************************************
函数名：
void CisInitCurrentOff(void)
参数描述：
无
返回值描述：
无
特殊算法：
无
注意事项：
初始化CIS检测模块
******************************************************************************/
void CisInitCurrentOff(void)
{
    memset(&s_stCisCurrent,0,sizeof(LedChanelTypeDef));

    Tlc5922LedWrite(&s_stCisCurrent,CIS_CH_ALL);
}


/*****************************************************************************
函数名：uint32_t CisGetCal(CIS_PARE_DEF *pstPare,uint8_t byLine)
参数描述：
CIS_PARE_DEF *pstPare：参数的数据结构，此参数将带回参考快的光亮值
uint8_t byLine：需要处理的数据行
返回值描述：
无意义
特殊算法：
无
注意事项：
从原始数据中取得参考块的数据
******************************************************************************/
void CisGetCal(stCisPareTypeDef *pstPare,uint8_t byLine)
{
    uint32_t dwCalcBuff = 0;
    uint16_t *p;

    p = &pstPare->wRcal;
    for(uint32_t color = 0; color < 5; color ++) {
        dwCalcBuff = 0;
        for(uint32_t i = 0 ; i < 10 ; i ++) {
            dwCalcBuff += g_wCisMem[byLine][color][60 + i]; //70为百基片 300为测试条
        }

        dwCalcBuff /= 10;
        *p = dwCalcBuff;
        p ++;
    }
}

void CisGetCalRgb(stCisPareTypeDef *pstPare,uint8_t byLine)
{
    uint32_t dwCalcBuff = 0;
    uint16_t *p;

    p = &pstPare->wRcal;
    for(uint32_t color = 0; color < 5; color ++) {
        dwCalcBuff = 0;
        for(uint32_t i = 0 ; i < 16 ; i ++) {
            dwCalcBuff += g_wCisMem[byLine][color][70 + i] - g_wCisMem[byLine][color][30];
        }

        dwCalcBuff /= 16;
        *p = dwCalcBuff;
        p ++;
    }
}

/*****************************************************************************
函数名：uint32_t CisLightAdj(uint16_t wLightVal)
参数描述：
uint16_t wLightVal：需要将光亮值调整到此值的+-500
返回值描述：
0：设置成功
1：设置失败
特殊算法：
无
注意事项：
自动调节CIS光亮值的电流
******************************************************************************/
int CisLightAdj(uint16_t wLightVal)
{
    uint8_t byErrorExit = 0;
	stCisPareTypeDef stCisLightPare;
    stCisLedOnTimTypeDef *pstLedTim;

    pstLedTim = &s_stCorePar.stCisLedOnTim;

  if(wLightVal >= 16383) return 1;

    pstLedTim->wLedROnTim = 250;
    pstLedTim->wLedGOnTim = 250;
    pstLedTim->wLedBOnTim = 250;
    pstLedTim->wLedIROnTim = 250;
    pstLedTim->wLedYGOnTim = 250;


    REG_CPLD_CIS_LEDR_ON_TIM =  pstLedTim->wLedROnTim;
    REG_CPLD_CIS_LEDG_ON_TIM =  pstLedTim->wLedGOnTim;
    REG_CPLD_CIS_LEDB_ON_TIM =  pstLedTim->wLedBOnTim;
    REG_CPLD_CIS_LEDIR_ON_TIM = pstLedTim->wLedIROnTim;
    REG_CPLD_CIS_LEDYG_ON_TIM = pstLedTim->wLedYGOnTim;

	CisClean();
	CisScanOneTim();
    while(CisGetBusyState() == 1);

	while(1) {//光值过小
        REG_CPLD_CIS_LEDR_ON_TIM = pstLedTim->wLedROnTim;
		CisClean();
		CisScanOneTim();
        while(CisGetBusyState() == 1);
		CisGetCal(&stCisLightPare,0);

		if(stCisLightPare.wRcal < wLightVal - 50) {
            if(pstLedTim->wLedROnTim < 1000) {
                pstLedTim->wLedROnTim += 1;
            }
            else {
                byErrorExit |= 0x01;
                break;
            }
		}
		else if(stCisLightPare.wRcal > (wLightVal + 50)) {
			if(pstLedTim->wLedROnTim > 100) {
                pstLedTim->wLedROnTim -= 1;
            }
            else {
                byErrorExit |= 0x01;
                break;
            }
		}
		else {
			break;
		}
	}
	//---------------------------------------g--------------------------------------------//
	while(1)
	{//光值过小
        REG_CPLD_CIS_LEDG_ON_TIM = pstLedTim->wLedGOnTim;
		CisClean();
		CisScanOneTim();
        while(CisGetBusyState() == 1);
		CisGetCal(&stCisLightPare,0);

		if(stCisLightPare.wGcal < wLightVal - 50) {
            if(pstLedTim->wLedGOnTim < 1000) pstLedTim->wLedGOnTim += 1;
            else {
                byErrorExit |= 0x02;
                break;
            }
		}
		else if(stCisLightPare.wGcal > (wLightVal + 50)) {
            if(pstLedTim->wLedGOnTim >100) pstLedTim->wLedGOnTim -= 1;
            else {
                byErrorExit |= 0x02;
                break;
            }
		}
		else {
			break;
		}
	}

	//---------------------------------------b--------------------------------------------//
	while(1)
	{//光值过大
        REG_CPLD_CIS_LEDB_ON_TIM = pstLedTim->wLedBOnTim;
		CisClean();
		CisScanOneTim();
        while(CisGetBusyState() == 1);
		CisGetCal(&stCisLightPare,0);

		if(stCisLightPare.wBcal < wLightVal - 50) {
            if(pstLedTim->wLedBOnTim < 1000) pstLedTim->wLedBOnTim += 1;
            else {
                byErrorExit |= 0x04;
                break;
            }
		}
		else if(stCisLightPare.wBcal > (wLightVal + 50)) {
			if(pstLedTim->wLedBOnTim > 100) pstLedTim->wLedBOnTim -= 1;
            else {
                byErrorExit |= 0x04;
                break;
            }
		}
		else{
			break;
		}
	}

	//---------------------------------------ir--------------------------------------------//
	while(1)
	{//光值过小
        REG_CPLD_CIS_LEDIR_ON_TIM = pstLedTim->wLedIROnTim;
		CisClean();
		CisScanOneTim();
        while(CisGetBusyState() == 1);
		CisGetCal(&stCisLightPare,0);

		if(stCisLightPare.wIRcal < wLightVal - 50) {
            if(pstLedTim->wLedIROnTim < 1000) pstLedTim->wLedIROnTim += 1;
            else {
                byErrorExit |= 0x08;
                break;
            }
		}
		else if(stCisLightPare.wIRcal > (wLightVal + 50)){
			if(pstLedTim->wLedIROnTim > 100) pstLedTim->wLedIROnTim -= 1;
            else {
                byErrorExit |= 0x08;
                break;
            }
		}
		else {
			break;
		}
	}

	//---------------------------------------yg--------------------------------------------//
    pstLedTim->wLedYGOnTim = 250;
    REG_CPLD_CIS_LEDYG_ON_TIM = pstLedTim->wLedYGOnTim;

	return byErrorExit;
}

/*****************************************************************************
函数名：int CisGetLightBanlance(void)
参数描述：
无
返回值描述：
0：设置成功
1：设置失败
特殊算法：
无
注意事项：
此函数将会计算CIS的数据的校准，校准的方式为（5000/值），取得的数据被
存在g_wCisBlan，ram的最后区域
******************************************************************************/
#define BANLANCE_LINE_NUM           18
void CisGetLightBanlanceWhite(void)
{
    uint32_t dwCalTmp;
    __IO uint32_t i;
  //  static uint8_t s_byebug = 0;
  //  uint8_t abyString[6] = {0};
  //  uint16_t wDatClaWt;
    u16 DataTemp[BANLANCE_LINE_NUM];
    for(uint32_t color = 0;color < 5;color ++)
    {
        for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)
        {
            g_wCisBlan[color][item] = 0;
        }
    }

    for(uint32_t count = 0; count < 4;count++)
    {
        CisClean();
        /**********************************************************************/
        for(int con1 = 0; con1 < BANLANCE_LINE_NUM; con1 ++)
        {
            CisScanOneTim();
            while(CisGetBusyState() == 1);

            for( i = 0;i < 0x5ffff;i ++);
        }

        for(uint32_t color = 0;color < 5;color ++)
        {


          for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)//项目个数//加入 取中值
          {
            for(u16 DataTempCount = 0 ; DataTempCount< BANLANCE_LINE_NUM ; DataTempCount++)
            {
              DataTemp[DataTempCount]=g_wCisMem[DataTempCount][color][item];
            }
            Sort(DataTemp,BANLANCE_LINE_NUM);
            dwCalTmp = 0;
            for(int i = 5; i < 13; i ++)
            {
              dwCalTmp += DataTemp[i];
            }
            g_wCisBlan[color][item] += (dwCalTmp >> 3);

//                dwCalTmp = 0;
//                for(int i = 2; i < BANLANCE_LINE_NUM; i ++)
//                {
//                    dwCalTmp += g_wCisMem[i][color][item];
//                }

 //               g_wCisBlan[color][item] += (dwCalTmp >> 4);

            }
        }
    }
    for(uint32_t color = 0;color < 5;color ++)
    {
        g_wCisBlan[color][0] = 0;//次位置存放最大值
        for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)
        {
            g_wCisBlan[color][item] >>= 2;
            g_wCisBlan[color][0] = g_wCisBlan[color][item] > g_wCisBlan[color][0]?g_wCisBlan[color][item]:g_wCisBlan[color][0];
        }
    }
}

/*****************************************************************************
函数名：int CisGetLightBanlance(void)
参数描述：
无
返回值描述：
0：设置成功
1：设置失败
特殊算法：
无
注意事项：
此函数将会计算CIS的数据的校准，校准的方式为（5000/值），取得的数据被
存在g_wCisBlan，ram的最后区域
******************************************************************************/
void CisGetLightBanlanceBlack(void)
{
    uint32_t dwCalTmp;
    __IO uint32_t i;
 //   static uint8_t s_byebug = 0;
 //   uint8_t abyString[5] = {0};
 //   uint16_t wDatClaWt;
    u16 DataTemp[BANLANCE_LINE_NUM];
    for(uint32_t color = 0;color < 5;color ++)
    {
        for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)
        {
            g_wCisBlan[color+5][item] = 0;
        }
    }

    for(uint32_t count = 0;count < 4;count ++)
    {
        CisClean();
        /**********************************************************************/
        for(int con1 = 0; con1 < BANLANCE_LINE_NUM; con1 ++)
        {
            CisScanOneTim();
            while(CisGetBusyState() == 1);

            for( i = 0;i < 0x5ffff;i ++);
        }

        for(uint32_t color = 0;color < 5;color ++)
        {
            for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)//项目个数//加入 取中值
            {
             for(u16 DataTempCount = 0 ; DataTempCount< BANLANCE_LINE_NUM ; DataTempCount++)
            {
              DataTemp[DataTempCount]=g_wCisMem[DataTempCount][5+color][item];
            }
            Sort(DataTemp,BANLANCE_LINE_NUM);
            dwCalTmp = 0;
            for(int i = 5; i < 13; i ++)
            {
              dwCalTmp += DataTemp[i];
            }
            g_wCisBlan[5+color][item] += (dwCalTmp >> 3);
//                dwCalTmp = 0;
//
//                for(int i = 2; i < BANLANCE_LINE_NUM; i ++)
//                {
//                    dwCalTmp += g_wCisMem[i][color][item];
//                }
//
//                g_wCisBlan[5 + color][item] += (dwCalTmp >> 4);
            }

        }
    }
    for(uint32_t color = 0;color < 5;color ++)
    {
        g_wCisBlan[color+5][0] = 0xffff;//存放最小值
        for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)
        {
            g_wCisBlan[color+5][item] >>= 2;
            g_wCisBlan[color+5][0] = g_wCisBlan[color+5][item] < g_wCisBlan[color+5][0]?g_wCisBlan[color+5][item]:g_wCisBlan[color+5][0];
        }
    }
}

/*****************************************************************************
函数名：int CisGetLightBanlance(void)
参数描述：
无
返回值描述：
0：设置成功
1：设置失败
特殊算法：
无
注意事项：
此函数将会计算CIS的数据的校准，校准的方式为（5000/值），取得的数据被
存在g_wCisBlan，ram的最后区域
******************************************************************************/
int CisGetLightBanlance(void)
{
    //long double calc;
    __IO FRESULT res;         // FatFs 函数公共结果代码
    UINT br;
    double dCalTmp = 0;
 //   static uint8_t s_byebug = 0;
 //   uint8_t abyString[6] = {0};

    for(uint32_t color = 0;color < 5;color ++) {
        for(int item = 0;item < BANLANCE_START; item ++) {
            g_wCisBlan[5 + color][item] = 0;
        }

        for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++) {//项目个数
            if(g_wCisBlan[color][item] > g_wCisBlan[5 + color][item]) {
                g_wCisBlan[color][item] -= g_wCisBlan[5 + color][item];
            }
            else {
                g_wCisBlan[color][item] = 0;
            }
        }

        for(int item = BANLANCE_END;item < CIS_DATA_LENTH; item ++) {
            g_wCisBlan[5 + color][item] = 0;
        }
    }

    for(uint32_t color = 0;color < 5;color ++) {
        for(int item = 0;item < BANLANCE_START; item ++) {
            g_wCisBlan[color][item] = 1;
        }

        dCalTmp = 0;

        for(int i = 0;i < 16;i++) {
            dCalTmp += g_wCisBlan[color][500 + i];
        }

        if(dCalTmp < 100)
            return 1;

        dCalTmp /= 16;
        /* dCalTmp = g_wCisBlan[color][0] - g_wCisBlan[color+5][0];
        if(dCalTmp < 100) return 1;
        g_wCisBlan[color][0] = 1;
        g_wCisBlan[color+5][0] = 0;*/

        for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++) {//项目个数
            if(dCalTmp != 0) {
                g_wCisBlan[color][item] = (uint16_t)((double)((dCalTmp) / (double)g_wCisBlan[color][item]) * 10000);
            }
            else {
                g_wCisBlan[color][item] = 1;
            }
        }
       for(int aaa=0;aaa<230;aaa++)//前面区域无法校准,不在范围用1.0代替
       {
        for(int bbb=0;bbb<4;bbb++)
        {
          if(g_wCisBlan[bbb][aaa]>11000 || g_wCisBlan[bbb][aaa]<9000)
          {
            g_wCisBlan[bbb][aaa] = 10000;

          }

        }
       }
        for(int item = BANLANCE_END;item < CIS_DATA_LENTH; item ++) {
            g_wCisBlan[color][item] = 1;
        }
    }



    while(xSemaphoreTake(xSemphrFlash,500) == pdFALSE) continue;
    //res = f_mount(0, &Fs);   // 为逻辑驱动器注册工作区
    res = f_open(&f1, "0:banlance.dat",FA_WRITE | FA_CREATE_ALWAYS | FA_READ);

    if(res != FR_OK)
    {
        return 1;
    }

    uint16_t *pwBuff;
    pwBuff = (uint16_t *)g_wCisBlan;
    uint16_t *pwSramBuff;

    for(int i = 5*1216*4; i > 0; i -= 512) {
        if(i>512) {
            pwSramBuff = (uint16_t *)g_byaWrFileBuff;
            for(int k = 0;k < 256;k ++) {
                *pwSramBuff ++ = *pwBuff ++;
            }

            res = f_write (&f1,g_byaWrFileBuff,512,&br);
        }
        else {
            pwSramBuff = (uint16_t *)g_byaWrFileBuff;
            for(int k = 0;k < (i>>1);k ++) {
                *pwSramBuff ++ = *pwBuff ++;
            }

            res = f_write (&f1,g_byaWrFileBuff,i,&br);
        }
    }

    f_close(&f1);

    xSemaphoreGive(xSemphrFlash);
    //res = f_mount(0, NULL);

//    if(s_byebug == 1)//调试用
//    {
//        uint16_t wDatClaWt;
//        for(uint32_t color = 0;color < 5;color ++)
//        {
//
//            abyString[0] = 'A'+color;
//            abyString[1] = 0x0d;
//            abyString[2] = 0x0a;
//            abyString[3] = 0;
//            while(UsartSerWrite(abyString,3,1));
//            vTaskDelay(50);
//
//            for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++)//项目个数
//            {
//                wDatClaWt = g_wCisBlan[color][item];
//                abyString[0] = (wDatClaWt /10000) + 0x30;
//                abyString[1] = (wDatClaWt % 10000) / 1000 + 0x30;
//                abyString[2] = (wDatClaWt % 1000) /100  + 0x30;
//                abyString[3] = (wDatClaWt % 100) /10  + 0x30;
//                abyString[4] =  wDatClaWt % 10  + 0x30;
//                abyString[5] =  0x20;
//                while(UsartSerWrite(abyString,6,1));
//                vTaskDelay(10);
//            }
//
//            abyString[0] = 0x0d;
//            abyString[1] = 0x0a;
//            abyString[2] = 0;
//            while(UsartSerWrite(abyString,2,1));
//            vTaskDelay(50);
//
//        }
//    }



    return 0;
}

/*****************************************************************************
函数名：void CisCalcLightBanlance(uint8_t byLineProc)
参数描述：
uint8_t byLineProc：需要处理的原始数据的行数
返回值描述：
无
特殊算法：
无
注意事项：
此函数将会计算CIS的数据的校准，校准的方式为（校准值*原始值），取得的数据被
存在原始数据中
******************************************************************************/
void CisCalcLightBanlance(uint8_t byLineProc)
{
    for(int line = 0;line < byLineProc; line ++) {
        for(uint32_t color = 0;color < 4;color ++) {
            for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++) {
                //项目个数
                if(g_wCisMem[line][color][item] > g_wCisBlan[5 + color][item])  {
                    g_wCisMem[line][color][item] -= g_wCisBlan[5 + color][item];
                }
                else {
                    g_wCisMem[line][color][item] = 0;
                }
            }
        }
    }

    for(uint8_t line = 0;line < byLineProc; line ++) {
        for(uint32_t color = 0; color < 4; color ++) {//2015.4.10 取消570校准算法
            for(uint32_t item = BANLANCE_START; item < BANLANCE_END; item ++) {
                g_wCisMem[line][color][item] = \
					(uint16_t)((double )g_wCisMem[line][color][item]* \
                        ((double )g_wCisBlan[color][item])/10000);
            }
        }
    }
}

/*****************************************************************************
函数名：int CisCheakStart(void)
参数描述：
无
返回值描述：
0：CIS发现试纸
1：CIS未发现试纸
特殊算法：
无
注意事项：
此函数将扫描CIS，并判断是否发现了有试纸。
******************************************************************************/
int CisCheakStart(void)
{
    uint8_t con = 0;

    CisClean();
    CisScanOneTim();
    while(CisGetBusyState() == 1);

    for(uint32_t i = 500; i < 950; i ++) {
        if(g_wCisMem[0][3][i] > 9000){
            con ++;
        }
        else {
            con = 0;
        }

        if(con > 10)
            break;
    }

    if(con > 10)
        return 0;
    return 1;
}

/*****************************************************************************
函数名：
void CisInit(void)
参数描述：
无
返回值描述：
0-正常,1-校准文件异常,2-硬件连接异常,其它-某种波长异常
特殊算法：
无
注意事项：
初始化CIS检测模块
******************************************************************************/
int CisInit(void)
{
    __IO FRESULT res;         // FatFs 函数公共结果代码
    UINT br;
        FIL f1;
    stCisPareTypeDef stCisLightPare;
    u8 byLightCount = 0;
    u16 wCisChkCnt = 0;

    REG_CPLD_CIS_EN = 1;

    memset(&s_stCisCurrent,0,sizeof(LedChanelTypeDef));
    //R
    s_stCisCurrent.Chanl_15.byCurrent = 30;
	s_stCisCurrent.Chanl_14.byCurrent = 30;
    s_stCisCurrent.Chanl_13.byCurrent = 0;
    //G
	s_stCisCurrent.Chanl_12.byCurrent = 30;
	s_stCisCurrent.Chanl_11.byCurrent = 30;
    s_stCisCurrent.Chanl_10.byCurrent = 0;
    //B
	s_stCisCurrent.Chanl_9.byCurrent = 30;
	s_stCisCurrent.Chanl_8.byCurrent = 30;
    s_stCisCurrent.Chanl_7.byCurrent = 0;
    //IR
	s_stCisCurrent.Chanl_6.byCurrent = 30;
	s_stCisCurrent.Chanl_5.byCurrent = 30;
    s_stCisCurrent.Chanl_4.byCurrent = 0;
    //YG
	s_stCisCurrent.Chanl_3.byCurrent = 50;
	s_stCisCurrent.Chanl_2.byCurrent = 50;
    s_stCisCurrent.Chanl_1.byCurrent = 50;
    s_stCisCurrent.Chanl_0.byCurrent = 0;

    Tlc5922LedWrite(&s_stCisCurrent,CIS_CH_ALL);

    res = f_open(&f1, "0:banlance.dat",FA_READ | FA_WRITE);

    if(res == FR_NO_FILESYSTEM) {
        return 0x01;//校准文件异常
    }
    if(res != FR_OK) {
        res = f_open(&f1, "0:banlance.dat",FA_WRITE | FA_CREATE_ALWAYS | FA_READ);
    }
    else
    {
        uint16_t *pwBuff;
        pwBuff = (uint16_t *)g_wCisBlan;
        uint16_t *pwSramBuff;

        if(f1.fsize < 10) {
            f_close(&f1);
            return 0x01;
        }

        for(int i = 5*1216*4; i > 0; i -= 512) {
            if(i>512) {
                pwSramBuff = (uint16_t *)g_byaWrFileBuff;
                res = f_read (&f1,g_byaWrFileBuff,512,&br);

                for(int k = 0;k < 256;k ++) {
                    *pwBuff++ = *pwSramBuff++;
                }
            }
            else {
                pwSramBuff = (uint16_t *)g_byaWrFileBuff;
                res = f_read (&f1,g_byaWrFileBuff,i,&br);

                for(int k = 0;k < (i>>1);k ++) {
                    *pwBuff++ = *pwSramBuff++;
                }
            }
        }
    }

    f_close(&f1);
    stCisLedOnTimTypeDef *stCisLedOnTim;
    stCisLedOnTim=&s_stCorePar.stCisLedOnTim;

    REG_CPLD_CIS_LEDR_ON_TIM = stCisLedOnTim->wLedROnTim;
    REG_CPLD_CIS_LEDG_ON_TIM = stCisLedOnTim->wLedGOnTim;
    REG_CPLD_CIS_LEDB_ON_TIM = stCisLedOnTim->wLedBOnTim;
    REG_CPLD_CIS_LEDIR_ON_TIM = stCisLedOnTim->wLedIROnTim;
    REG_CPLD_CIS_LEDYG_ON_TIM = stCisLedOnTim->wLedYGOnTim;

    for(int i = 0;i< 50;i++)
    {
        CisClean();
        CisScanOneTim();
        if(CisGetBusyState() == 1) {
            asm("nop");
        }

        //连接超时判断
        wCisChkCnt = 0;
        while(CisGetBusyState() == 1) {
            wCisChkCnt ++;
            for(int i=0; i<0xffff; i++);
            if(wCisChkCnt >= 1000)
                return 0x02;
        }

        CisGetCal(&stCisLightPare,0);
        if(stCisLightPare.wRcal < 200 && stCisLightPare.wGcal < 200 &&\
            stCisLightPare.wBcal < 200 && stCisLightPare.wIRcal < 200 &&\
                stCisLightPare.wYGcal < 200)
        {
            return 0x02;//光亮值异常
        }
        if(stCisLightPare.wRcal < 9000) {
            byLightCount |= 0x01;
        }

        byLightCount <<= 1;
        if(stCisLightPare.wGcal < 9000) {
            byLightCount |= 0x01;
        }

        byLightCount <<= 1;
        if(stCisLightPare.wBcal < 9000) {
            byLightCount |= 0x01;
        }

        byLightCount <<= 1;
        if(stCisLightPare.wIRcal < 9000) {
            byLightCount |= 0x01;
        }

//        s_stStaticPar.wWhiteAdc[0] = stCisLightPare.wRcal/2;
//        s_stStaticPar.wWhiteAdc[1] = stCisLightPare.wGcal/2;
//        s_stStaticPar.wWhiteAdc[2] = stCisLightPare.wBcal/2;

        /*//不对黄绿光做判断
        byLightCount <<= 1;
        if(stCisLightPare.wYGcal < 200) {
            byLightCount |= 0x01;
        }
        */
        for(int i = 0;i< 0x5ffff;i++);
        if(byLightCount != 0)
        {
            return byLightCount;
        }
    }

    return 0;
}

/*****************************************************************************
函数名：void CisInit(void)
参数描述： 无
返回值描述： 无
特殊算法： 无
说明：为不重启单独增加了一个CIS模块硬件初始化函数
注意事项： 初始化CIS检测模块
******************************************************************************/
void cisHwInit(void)
{

    REG_CPLD_CIS_EN = 1;
    memset(&s_stCisCurrent,0,sizeof(LedChanelTypeDef));
    //R
    s_stCisCurrent.Chanl_15.byCurrent = 30;
	s_stCisCurrent.Chanl_14.byCurrent = 30;
    s_stCisCurrent.Chanl_13.byCurrent = 0;
    //G
	s_stCisCurrent.Chanl_12.byCurrent = 30;
	s_stCisCurrent.Chanl_11.byCurrent = 30;
    s_stCisCurrent.Chanl_10.byCurrent = 0;
    //B
	s_stCisCurrent.Chanl_9.byCurrent = 30;
	s_stCisCurrent.Chanl_8.byCurrent = 30;
    s_stCisCurrent.Chanl_7.byCurrent = 0;
    //IR
	s_stCisCurrent.Chanl_6.byCurrent = 30;
	s_stCisCurrent.Chanl_5.byCurrent = 30;
    s_stCisCurrent.Chanl_4.byCurrent = 0;
    //YG
	s_stCisCurrent.Chanl_3.byCurrent = 50;
	s_stCisCurrent.Chanl_2.byCurrent = 50;
    s_stCisCurrent.Chanl_1.byCurrent = 50;
    s_stCisCurrent.Chanl_0.byCurrent = 0;

    Tlc5922LedWrite(&s_stCisCurrent,CIS_CH_ALL);

    REG_CPLD_CIS_LEDR_ON_TIM = 500;
    REG_CPLD_CIS_LEDG_ON_TIM = 500;
    REG_CPLD_CIS_LEDB_ON_TIM = 500;
    REG_CPLD_CIS_LEDIR_ON_TIM = 500;
    REG_CPLD_CIS_LEDYG_ON_TIM = 500;
}