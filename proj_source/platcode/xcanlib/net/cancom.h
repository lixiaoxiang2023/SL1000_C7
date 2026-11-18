/*******************************************************************************
文件名称：   cancom.cpp
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   20130721
程序用途：   xcan协议,用于linux的can驱动

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2016.03.19
修改人：胥强
修改内容：重新整理协议内容，并将协议结果模块化


*******************************************************************************/
#ifndef CANCOM_H
#define CANCOM_H

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "xcanlib/cpp/xcanmgr.h"
#include "CanCommLib/qsocketcan.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define ICAN_DEV_ID                     0xf0    //主机ID

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
class CanCom : public XcanNet
{
public:
    CanCom();
    ~CanCom();

    //终止线程
    void StopThread(void);
    void StartThread(void);

    // 从CAN总线上接收到数据帧
    static void InPort(can_frame *canFrame);

private:
    bool m_ThreadFlag;                          // 线程运行开关
    static CanCom *mpCanCom;
    QSocketCan *mpSocketCanLib;

    // C 库的接口
    int  Send (unsigned int dwDevId, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen) override;
    bool IsSendFull ( unsigned int dwDevID ) override;
    void Err (unsigned int dwComChnID, unsigned int dwRemoteDevID, unsigned short int wIoUUID, unsigned char byFrame) override;
};
#endif // CANCOM_H
