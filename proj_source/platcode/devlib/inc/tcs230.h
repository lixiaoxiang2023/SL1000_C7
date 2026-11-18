/*******************************************************************************
文件名称： tcs230.h
文件目录： $PROJ_DIR$/usr/Drive/inc
项目名称： 事业一部通用技术平台
创建者：   胥强
创建日期：
程序用途： 用于颜色传感器驱动

版权声明： Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __TCS230_H_
#define __TCS230_H_

/*******************************************************************************
Include file
*******************************************************************************/

/*******************************************************************************
定义常量
*******************************************************************************/
#define TCS230_R    0
#define TCS230_G    1
#define TCS230_B    2
#define TCS230_W    3

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

typedef enum
{
    emColorless = 0,//无色
    emRed1,         //红
    embrown1,       //棕
    emYellow1,      //黄
    emGreen1,       //绿
    emBlue1,        //蓝
    emPurple1,      //紫

    emOther1,       //其他
}emColorDdf1;

typedef struct
{
    unsigned int *pwdNOE;   //驱动器的控制引脚
    unsigned int *pwdS0;   //驱动器的控制引脚
    unsigned int *pwdS1;   //驱动器的控制引脚
    unsigned int *pwdS2;   //驱动器的控制引脚
    unsigned int *pwdS3;   //驱动器的控制引脚

    unsigned char byTimNO;
    unsigned char byTimCcmrChn;
}Tcs230InitTdf;

typedef struct
{
    void (*Rgb2Hsl) (const unsigned char* abyRGB, unsigned short int *pwHsl);
    unsigned int (*Read) (unsigned char byColor);
    unsigned int (*ReadColor) (unsigned int *pdwWrgb, unsigned char *pbyRgb, unsigned short int *pwHsl);
}DevTcs230Tdf;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevTcs230Tdf *Tcs230Init(Tcs230InitTdf *pstInit);

#endif