/******************************************************************************
文件名称：tlc5922.c
目录：$PROJ_DIR$/../code/dev
项目名称：urit-1600全自动尿液分析仪
创建者：  胥强
创建日期：20120810
程序用途：
        用于CIS采样LED驱动

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
#include "tlc5922.h"
#include "tlc5922_hal.h"

/******************************************************************************
Constant and Macro Definition 常量以及宏定义
******************************************************************************/

/******************************************************************************
全局变量定义
******************************************************************************/

/******************************************************************************
外部变量定义
******************************************************************************/

/******************************************************************************
数据结构与数据类型定义
******************************************************************************/

/******************************************************************************
内部全局变量申明
******************************************************************************/
static LedChanelTypeDef s_stChanlStat;

/******************************************************************************
内部函数申明
******************************************************************************/


/*****************************************************************************
函数名：void Tlc5922LedDataDeinit(LedChanelTypeDef *pstLedChanl)
参数描述：
        LedChanelTypeDef *pstLedChanl：输入的电流参数地址
返回值描述：
        无返回
说明：
        用于初始化Tlc5922所需要电流参数，将所有的数据清零
特殊算法：无
注意事项：无
******************************************************************************/
void Tlc5922LedDataDeinit(LedChanelTypeDef *pstLedChanl)
{
        uint8_t *data;
        data = (uint8_t *)pstLedChanl;

        for(int i = 0 ;i< sizeof(LedChanelTypeDef); i++)
        {
                *data ++ = 0;
        }
}

/*****************************************************************************
函数名：uint32_t Tlc5922LedWrite(LedChanelTypeDef *pstLedChanl,uint16_t wUsedChanl)
参数描述：
        LedChanelTypeDef *pstLedChanl：输入的电流参数地址
        uint16_t wUsedChanl：需要操作的电流通道
返回值描述：
        0：设置电流成功
        1：设置电流失败
说明：
        用于向Tlc5922驱动器写入电流控制数据
特殊算法：无
注意事项：无
******************************************************************************/
uint32_t Tlc5922LedWrite(LedChanelTypeDef *pstLedChanl,uint16_t wUsedChanl)
{
        LedSetDef *write_led,*input;
        uint16_t falg;
        uint16_t state = 0;

        write_led = (LedSetDef *)(&s_stChanlStat);
        input = (LedSetDef *)pstLedChanl;

        falg = 0x01;
        for(uint32_t i = 0;i < 16; i++)
        {
                if(input->byCurrent > LED_TLC5922_MAX_CUR)
                        return 1;

                if((falg & wUsedChanl) == falg)
                {
                        write_led->byCurrent = (uint8_t)(input->byCurrent * 1.58);
                        write_led->byLedOnOff = input->byLedOnOff;
                }

                write_led ++;
                input ++;
                falg <<= 1;
        }

        falg = 0x0040;
        write_led = (LedSetDef *)(&s_stChanlStat);//init the pointer
        write_led += 15;
        //write byCurrent data
        WR_CUR_HEAD;
        for(int32_t led = 0;led < 16; led ++)
        {
                for(uint32_t i = 0; i < 7; i++)
                {
                        if((write_led->byCurrent & falg) == falg)
                        {
                                LED_5922_inH;
                        }
                        else
                        {
                                LED_5922_inL;
                        }

                        LED_DALAY;
                        falg >>= 1;
                        LED_5922_sclkH;

                        LED_DALAY;
                        LED_5922_sclkL;
                }
                falg = 0x0040;
                write_led --;
        }

        //shift the data
        LED_5922_xlatH;
        LED_DALAY;
        LED_5922_xlatL;

        write_led = (LedSetDef *)(&s_stChanlStat);
        for(uint32_t i = 0; i < 16; i++)
        {
                state += write_led->byLedOnOff;
                write_led ++;
        }

        if(state == 0)
                return 0;

        falg = 0x0001;
        write_led = (LedSetDef *)(&s_stChanlStat);
        write_led = write_led + 15;

        WR_ON_OFF_HEAD;
        for(uint32_t i = 0; i < 16; i++)
        {
                if((write_led->byLedOnOff & falg) == falg)
                {
                        LED_5922_inH;
                }
                else
                {
                        LED_5922_inL;
                }

                LED_DALAY;
                LED_5922_sclkH;

                LED_DALAY;
                LED_5922_sclkL;

                write_led --;
        }

        //shift the data
        LED_5922_blankH;
        LED_DALAY;
        LED_5922_xlatH;
        LED_DALAY;
        LED_5922_xlatL;
        LED_5922_blankL;

        return 0;
}

/*****************************************************************************
函数名：void Tlc5922LedRead(LedChanelTypeDef *pstLedChanl)
参数描述：
        LedChanelTypeDef *pstLedChanl：输入的电流参数地址
返回值描述：
        无
说明：
        读取Tlc5922驱动器的数据
特殊算法：无
注意事项：无
*****************************************************************************
void Tlc5922LedRead(LedChanelTypeDef *pstLedChanl)
{
        LedSetDef *read_led;
        read_led = (LedSetDef *)pstLedChanl;

        read_led = read_led + 15;
        //write byCurrent data
        WR_CUR_HEAD;
        for(uint32_t led = 0;led < 16;led ++)
        {
                for(uint32_t i = 0; i < 7; i++)
                {
                        LED_5922_sclkH;
                        LED_DALAY;

                        if(LED_5922_out)read_led->byCurrent |= 0x01;
                        read_led->byCurrent <<= 1;

                        LED_5922_sclkL;
                }
                read_led --;
        }

        read_led = (LedSetDef *)pstLedChanl;
        read_led = read_led + 15;

        WR_ON_OFF_HEAD;
        for(uint32_t i = 0; i < 16; i++)
        {
                LED_5922_sclkH;
                LED_DALAY;

                if(LED_5922_out)read_led->byLedOnOff |= 0x01;
                LED_5922_sclkL;

                read_led --;
        }
}*/

/*****************************************************************************
函数名：void Tlc5922LedWriteOnOff(LedChanelTypeDef *pstLedChanl,uint16_t wUsedChanl)
参数描述：
        LedChanelTypeDef *pstLedChanl：输入的电流参数地址
        uint16_t wUsedChanl：需要操作的电流通道
返回值描述：
        无
说明：
        控制led通道的关断
特殊算法：无
注意事项：无
******************************************************************************/
void Tlc5922LedWriteOnOff(LedChanelTypeDef *pstLedChanl,uint16_t wUsedChanl)
{
        LedSetDef *write_led,*input;
        write_led = (LedSetDef *)pstLedChanl;
        uint16_t falg = 0x01;

        write_led = (LedSetDef *)(&s_stChanlStat);
        input = (LedSetDef *)pstLedChanl;

        for(uint32_t i = 0;i < 16; i++)
        {
                if((falg & wUsedChanl) == falg)
                {
                        write_led->byLedOnOff = input->byLedOnOff;
                }
                write_led ++;
                input ++;
                falg <<= 1;
        }

        falg = 0x01;
        write_led = (LedSetDef *)(&s_stChanlStat);
        write_led += 15;
        WR_ON_OFF_HEAD;
        for(uint32_t i = 0; i < 16; i++)
        {
                if((write_led->byLedOnOff & falg) == falg)
                {
                        LED_5922_inH;
                }
                else
                {
                        LED_5922_inL;
                }

                LED_DALAY;
                LED_5922_sclkH;

                LED_DALAY;
                LED_5922_sclkL;

                write_led --;
        }

        //shift the data
        LED_5922_blankH;
        LED_DALAY;
        LED_5922_xlatH;
        LED_DALAY;
        LED_5922_xlatL;
        LED_5922_blankL;
}

/*****************************************************************************
函数名：void Tlc5922LedCmd(FunctionalState emCmd)
参数描述：
        ENABLE：使能电流输出
        DISABLE：失能电流输出
返回值描述：
        无返回
说明：
        无
特殊算法：无
注意事项：无
******************************************************************************/
void Tlc5922LedCmd(FunctionalState emCmd)
{
        if(emCmd == ENABLE)
        {
                LED_5922_blankL;
        }
        else
        {
                LED_5922_blankH;
        }
}

/*****************************************************************************
函数名：void Tlc5922LedInit(void)
参数描述：
        无
返回值描述：
        无返回
说明：
        初始化LED驱动器的管脚配置，并清空驱动器。
特殊算法：无
注意事项：无
******************************************************************************/
void Tlc5922LedInit(void)
{
        //LED_5922_RCC_INIT();
        //LED_5922_GPIO_INIT();

        LED_5922_blankL;
        LED_5922_xlatL;
        LED_5922_sclkL;
        LED_5922_inL;
        LED_5922_modeL;

        Tlc5922LedDataDeinit(&s_stChanlStat);

        Tlc5922LedWrite(&s_stChanlStat,LED_5922_CH_ALL);
}
//end of the file