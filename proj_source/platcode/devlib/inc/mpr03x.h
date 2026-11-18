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
#ifndef __MPR03X_H
#define __MPR03X_H

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
    unsigned char byI2cNo;

    unsigned char byIntGpioNO;
    unsigned char byIntChn;
#if(CONFIG_I2C_IO)
    unsigned int *pwdI2cClk;
    unsigned int *pwdI2cSda;
#endif

#if(CONFIG_I2C_DMA)
    unsigned char byI2cTxDmaNO;
    unsigned char byI2cTxDmaChn;
#if(CONFIG_I2C_RX_DMA)
    unsigned char byI2cRxDmaNO;
    unsigned char byI2cRxDmaChn;
#endif
#endif
}Mpr03xInitTdf;

typedef struct DevMpr03xTdf_t DevMpr03xTdf;

struct DevMpr03xTdf_t {
    const DevMpr03xTdf * (*SetMHD) (unsigned char byMhd);
    const DevMpr03xTdf * (*SetNHD) (unsigned char byNhd);
    const DevMpr03xTdf * (*SetNCL) (unsigned char byNcl);
    const DevMpr03xTdf * (*SetE0TTH) (unsigned char byE0tth);
    const DevMpr03xTdf * (*SetE0RTH) (unsigned char byE0rth);
    const DevMpr03xTdf * (*SetE1TTH) (unsigned char byE1tth);
    const DevMpr03xTdf * (*SetE1RTH) (unsigned char byE1rth);
    const DevMpr03xTdf * (*SetE2TTH) (unsigned char byE2tth);
    const DevMpr03xTdf * (*SetE2RTH) (unsigned char byE2rth);
    const DevMpr03xTdf * (*SetAfecFFI) (unsigned char byFfi);
    const DevMpr03xTdf * (*SetAfecCDC) (unsigned char byCdc);
    const DevMpr03xTdf * (*SetFcCDT) (unsigned char byCdt);
    const DevMpr03xTdf * (*SetFcSFI) (unsigned char bySfi);
    const DevMpr03xTdf * (*SetFcESI) (unsigned char byEsi);
    const DevMpr03xTdf * (*SetFcEC) (unsigned char byEleEn);
    const DevMpr03xTdf * (*SetEcEleEn) (unsigned char byEleEn);
    const DevMpr03xTdf * (*SetEcModeSel) (unsigned char byModeSel);
    const DevMpr03xTdf * (*SetEcCalLock) (unsigned char byCalLock);
    const DevMpr03xTdf * (*SetSrOcf) (void);
    int (*SetExe) (void);
    unsigned char (*GetBaseline) (unsigned int dwChn);
    unsigned short int (*GetFilter) (unsigned int dwChn);
    unsigned char (*GetSr) (void);
};

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevMpr03xTdf * Mpr03xInit( Mpr03xInitTdf *pstInit );

#endif