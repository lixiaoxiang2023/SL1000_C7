/*******************************************************************************
文件名称：    cat3626.h
文件目录：    $PROJ_DIR$/usr/Drive/inc
项目名称：    事业一部技术平台
创建者：      胥强
创建日期：    2016.03.14
程序用途：    Cat3626驱动程序头文件

版权声明：    Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __CAT3626_H
#define __CAT3626_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define CAT3626_CHN_LEDA1    0x01
#define CAT3626_CHN_LEDA2    0x02
#define CAT3626_CHN_LEDB1    0x04
#define CAT3626_CHN_LEDB2    0x08
#define CAT3626_CHN_LEDC1    0x10
#define CAT3626_CHN_LEDC2    0x20

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    unsigned char byI2cNo;

    unsigned int *pwdI2cEn;
#if(CONFIG_I2C_IO)
    uint8_t byI2cClk;
    uint8_t byI2cSda;
    unsigned char byGpioNo;       // GPIO 端口编号
    unsigned char bySda;         //
#endif

#if(CONFIG_I2C_DMA)
    unsigned char byI2cTxDmaNO;
    unsigned char byI2cTxDmaChn;
#if(CONFIG_I2C_RX_DMA)
    unsigned char byI2cRxDmaNO;
    unsigned char byI2cRxDmaChn;
#endif
#endif
}Cat3626InitTdf;

typedef struct {
    int (*SetDevCurrent) (unsigned char byChn, bool blWait);
    int (*TurnOn) (unsigned char byChn, bool blReWrCur, bool blWait);
    int (*TurnAllOn) (bool blWait);
    int (*TurnAllOff) (bool blWait);

    void (*ClrCurrent) (void);
    int (*SetCurrent) (unsigned char byChn, unsigned char byCur);
    void (*DevOn) (bool blCmd);
}DevCat3626Tdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevCat3626Tdf * Cat3626Init( Cat3626InitTdf *pstInit );

#endif