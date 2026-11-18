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

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "cancom.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/
CanCom *CanCom::mpCanCom = nullptr;

/*******************************************************************************
函数功能：
*   @ 构造函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
CanCom::CanCom() : XcanNet(ICAN_DEV_ID, 8)
{
    m_ThreadFlag = true;
    mpCanCom = this;

    // 构造一个 8 数据长度的 can通信设备
    mdwCommChnID = ICAN_DEV_ID;
    mdwFrameLen = 8;

    // init socketcan
    mpSocketCanLib = new QSocketCan("can0");

    if(mpSocketCanLib != nullptr) {
        mpSocketCanLib->Init();

        void (*pmyHandler) (can_frame *) = &CanCom::InPort;
        mpSocketCanLib->RegRxHandler(pmyHandler);
    }
}

/*******************************************************************************
函数功能：
*   @ 析构函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
CanCom::~CanCom()
{
    m_ThreadFlag = false;
    if(mpSocketCanLib == nullptr) delete mpSocketCanLib;

    mpSocketCanLib = nullptr;
}

/*******************************************************************************
函数功能：
*   @ 数据发送方法
参数描述：
*   @ unsigned int dwComChnID 通信ID
*   @ unsigned int dwId 数据逻辑ID
*   @ unsigned char * pbyIn 数据
*   @ unsigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @ 使用can进行数据的发送
*******************************************************************************/
int CanCom::Send(unsigned int dwComChnID, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen)
{
    can_frame canFrame;

    if(dwLen > 8) dwLen = 8;
    canFrame.can_id = (dwId >> 3) | CAN_EFF_FLAG ;
    canFrame.can_dlc = dwLen;

    memset(canFrame.data, 0, 8);
    memcpy(canFrame.data, pbyIn, dwLen);

    mpSocketCanLib->Send(canFrame);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 通信设备是否忙
参数描述：
*   @ nsigned int dwComChnID 通信ID
返回值描述：
*   @ 无
说明：
*   @ 可以通过检查同信状态返回true，实现数据停止传输
*******************************************************************************/
bool CanCom::IsSendFull(unsigned int dwComChnID)
{
    return false;
}

/*******************************************************************************
函数功能：
*   @ 数据传输出错的回调实现
参数描述：
*   @ unsigned int dwComChnID 通信ID
*   @ unsigned short int wIoUUID 资源ID
*   @ unsigned char byFrame 帧类型
返回值描述：
*   @ 无
说明：
*   @ 协议栈出现传输问题
*******************************************************************************/
void CanCom::Err(unsigned int dwComChnID, unsigned int dwRemoteDevID, unsigned short int wIoUUID, unsigned char byFrame)
{
    ;
}

/*******************************************************************************
函数功能：
*   @ 通过信号接收数据
参数描述：
*   @ can_frame *pCanFrame can数据帧
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void CanCom::InPort(can_frame *canFrame)
{
    unsigned char *pbyIn = NULL;
    unsigned char byaFrame[8 + sizeof(unsigned int)] = {0};
    unsigned int dwLen = 0;

    pbyIn = byaFrame;
    *(unsigned int*) pbyIn = canFrame->can_id << 3;
    pbyIn += sizeof(unsigned int);

    dwLen = canFrame->can_dlc;
    memcpy(pbyIn, canFrame->data, 8);
    dwLen += sizeof(unsigned int);

    mpCanCom->In(ICAN_DEV_ID, byaFrame, dwLen);
}

/*******************************************************************************
函数功能：
*   @ 关闭线程
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void CanCom::StopThread()
{
    m_ThreadFlag = false;
    mpSocketCanLib->Stop();
}

/*******************************************************************************
函数功能：
*   @ 启动线程
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void CanCom::StartThread(void)
{
    m_ThreadFlag = true;
    mpSocketCanLib->Start();
}
