/*******************************************************************************
文件名称：   i2c.h
文件目录：   $PROJ_DIR$/usr/Drive/inc
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.12
程序用途：   基于STM32的I2c驱动程序头文件

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __I2C_H
#define __I2C_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    int (*Config) ( unsigned int dwDev, unsigned int dwBaud, bool blIsDutyCycle2);
    int (*Trs) (unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen);
    int (*SaTrs)(unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen);
    int (*CheckOnline)(unsigned int dwDev,unsigned char byAdrr);
#if(CONFIG_I2C_READ)
    int (*SaRead)(unsigned int dwDev,unsigned char *pbyOut, unsigned int dwLen);
    int (*Read) (unsigned int dwDev, unsigned char *pbyOut, unsigned int dwLen);
    int (*ReadData) (unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen);
#endif

#if(CONFIG_I2C_IO)
    int (*SetPin)(uint8_t byDev, uint8_t Scl_Port, uint8_t Scl_Pin, uint8_t Sda_Port, uint8_t Sda_Pin, void (*delay)(void));
    int (*TrsIo) (unsigned int dwDev,unsigned char *pbyIn, unsigned int dwLen);
#endif

#if(CONFIG_I2C_DMA)
    int (*TrsDma) (unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, bool blWait);
    int (*ConfigDmaTx) ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn);
#if(CONFIG_I2C_RX_DMA)
    int (*ConfigDmaRx) ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn);
#endif
#endif

#if(CONFIG_I2C_IRP)
    int (*TrsIrq) (unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, bool blWait);
#endif

}DevI2cTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const DevI2cTdf *DevI2c;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevI2cTdf * I2cInit( void );

#endif
