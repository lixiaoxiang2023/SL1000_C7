/*******************************************************************************
文件名称：   thb6128.h
文件目录：   $PROJ_DIR$\user\init\inc
项目名称：   事业一部技术平台
创建者：     
创建日期：   
程序用途：   基于6128的电机驱动

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __THB6128_H
#define __THB6128_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define M_CCW           (unsigned char )0x00 /*0b00000000*/
#define M_CW            (unsigned char )0x01 /*0b00000001*/

#define M_STP           (unsigned char )0x00 /*0b00000000 0 步数停止*/
#define M_SEN           (unsigned char )0x02 /*0b00000010 1 发现光耦后停止*/
#define M_RST           (unsigned char )0x04 /*0b00000100 2 发现光耦后再走相应的步数再停止，发现光耦只能在匀速阶段*/
#define M_SENSLOW       (unsigned char )0x06 /*0b00000110 3 发现光耦后开始减速停止*/
#define M_SENSETP       (unsigned char )0x08 /*0b00001000 4 发现光耦后强制走相应的步数，发现光耦可以在加减速阶段*/
#define M_NOSTOP        (unsigned char )0x0a /*0b00001010 5 电机不停止，电机可以修改运行速度*/

#define M_RE_NOW        (unsigned char )0x10 /*0b00010000*/
#define M_HALF          (unsigned char )0x20 /*0b00100000*/

#define M_DIV0          (unsigned char )0x00 /*0b00000000*/
#define M_DIV2          (unsigned char )0x40 /*0b01000000*/
#define M_DIV4          (unsigned char )0x80 /*0b10000000*/
#define M_DIV8          (unsigned char )0xc0 /*0b11000000*/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct
{
	unsigned int dwSteps;    //电机需要运行的步数
	unsigned int dwMaxSteps; //电机需要运行的最大步数步数，在光耦停止模式有效
    unsigned int dwRestSteps; //电机运行在发现光耦后继续运行此步数后停止
    unsigned int dwStart;    //电机起始频率
    unsigned int dwMax;      //电机最大频率
    unsigned int dwSpdUp;    //电机每一次加速内需要运行的步数
    
    union {
        unsigned char byCtr;
        struct {
            unsigned char byDir:1;          //电机运行的方向
            unsigned char byStopMode:3;     //电机停止模式，是否找光耦
            unsigned char byIsReNow:1;      //是否立即返回
            unsigned char byIsHalf:1;       //是否使用小电流运行
            unsigned char byDiv:2;          //电机细分
        }BitCtr;
    };
    
    void (*pfRunFunc)(void);//电机运行过程中的用户应用接口
}stTHB6128Tdf;

typedef struct
{
    unsigned char byHwnd; //电机句柄

    unsigned char byAtOptLv;                              //光耦使用电平，0为低电平，1为高电平
    unsigned int  dwTimNo;
                            /*
                                电机使用的定时器模块 需要按照:
                                    0:TIM1;
                                    1:TIM2;
                                    2:TIM3;
                                    3:TIM4;
                                    4:TIM5;
                                    5:TIM8;
                            */
    unsigned int dwTimChnNo; //PWM使用的定时器通道号

    unsigned int *pwdOpGpio;  //光耦输入信号的位定义地址
    unsigned int *pwdMS1Gpio; //电机细分低位位定义地址
    unsigned int *pwdMS2Gpio; //电机细分高位位定义地址
    unsigned int *pwdHalfGpio;//电机细分半流位定义地址
    unsigned int *pwdDirGpio; //电机细分方向位定义地址
}stTHB6128InitTdf;

typedef struct {
    void (*Stop) (unsigned int dwHwnd);
    bool (*OnSensor) (unsigned int dwHwnd);
    void (*CtrHalf) (unsigned int dwHwnd, bool blEnable);
    void (*SetOp) (unsigned int dwHwnd, unsigned int *pwdOpGpio, unsigned char byLevel);
	//int  (* GetOp) (unsigned int dwHwnd);
    int  (*Write) (unsigned int dwHwnd,stTHB6128Tdf *pstThb6128Tdf);
    bool (*IsBusy) (unsigned int dwHwnd);
    int  (*GetSteps) (unsigned int dwHwnd);
   // int  (*ModifySpeed) (unsigned int dwHwnd, unsigned int dwSpeed, unsigned int dwSpdUp, void (*pfRunFunc)(void));
   // bool (*IsSpeedUpDoing) (unsigned int dwHwnd);
    void (*Start)(unsigned int dwHwnd);
    int (*StartOnSensor)(unsigned int dwHwnd);
}DevTHB6128Tdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevTHB6128Tdf *THB6128Init(stTHB6128InitTdf* pstInit);

#endif
