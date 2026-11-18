/*******************************************************************************
文件名称：   uln2003.h
文件目录：   $PROJ_DIR$\..\dev\inc
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.14
程序用途：   用于2003的电机驱动部分

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __ULN2003A_H
#define __ULN2003A_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define M1_NULL          0x00 /*0b00000001*/
#define M1_CCW           0x01 /*0b00000001*/
#define M1_CW            0x00 /*0b00000001*/

#define M1_STP           0x00 /*0b00000000*/
#define M1_SEN           0x02 /*0b00000010*/
#define M1_RST           0x04 /*0b00000100*/

#define M1_RE_NOW        0x08 /*0b00001000*/
#define M1_DIV2          0x20 /*0b00100000*/
#define M1_DIV4          0x40 /*0b01000000 4细分*/
#define M1_DIV8          0x60 /*0b01100000 8细分*/

//#define USE_SIN_UP_SPEED1 /*使用正弦加速曲线加速*/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct
{
    unsigned char byHwnd;         // 电机句柄
    unsigned char byTimNO;        // 使用的定时器模块编号

    unsigned char byGpioNo;       // GPIO 端口编号
    unsigned char byGpioAlian;    // 电机的控制端口在gpio的最低位，如：电机占用PA4567,则此位为4

    unsigned char byAtOptLv;      // 光耦使用电平，0为低电平，1为高电平
    unsigned int *pwdOpGpio;      // 光耦输入信号的位定义地址
}DevUln2003InitTdf;

typedef struct {
    int (*GoStep) (unsigned int dwHwnd);
    int (*Stop) (unsigned int dwHwnd);
    bool (*OnSensor) (unsigned int dwHwnd);
    int (*SetOp) (unsigned int dwHwnd, unsigned int *pwdOpGpio, unsigned char byLevel);
    int (*ExternOp) (unsigned int dwHwnd, unsigned char byCtr);
    int (*ExternEnd) (unsigned int dwHwnd);
    int (*Write) (unsigned int dwHwnd, unsigned int dwSteps, unsigned int dwMaxSteps, unsigned int dwStart, unsigned int dwMax, unsigned int dwSpdUp,\
    void (*pfRunFunc)(void), unsigned char byCtr);
    bool (*IsBusy) (unsigned int dwHwnd);
    int (*GetSteps) (unsigned int dwHwnd);
}DevUln2003Tdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevUln2003Tdf *Uln2003Init(DevUln2003InitTdf* pstInit);

#endif