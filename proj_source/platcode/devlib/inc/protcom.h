/*******************************************************************************
文件名称：    protcom.h
版本：        1.00.1
文件目录：    $PROJ_DIR$/../
项目名称：    事业一部技术平台
创建者：      胥强
创建日期：    2018.03.14
程序用途：    用于基于串口的数据通信协议解析文件
版权声明：
    Copyright urit Co.,Ltd.All Right Reserved

第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __PROT_COM_H
#define __PROT_COM_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
typedef struct {
    unsigned char byDevNO;      //所使用的底层设备号

    unsigned short int wMaxIoDatLen;

    unsigned char *pbyRxBuff;
    unsigned char *pbyIo;

    int (*SendString) (unsigned char *pcString, unsigned int dwLen);
}stProtComChnInitTdf;

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    int (*AddChn) (stProtComChnInitTdf *);
    int (*Read) (unsigned int dwChnDev, unsigned char bySAddr, unsigned short int wLen);
    int (*Write) (unsigned int dwChnDev, unsigned char bySAddr, unsigned char *pbyIn, unsigned short int wLen, void (*pfProtSendEnd) (void));
    void (*ProtComService) (void);
    void (*ProtCmoCorePro) ( unsigned int dwDev, unsigned char byIn);
}ProtComTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/
extern const ProtComTdf *ProtCom;

/*******************************************************************************
输出函数申明
*******************************************************************************/
const ProtComTdf *ProtComInit(void);

#endif