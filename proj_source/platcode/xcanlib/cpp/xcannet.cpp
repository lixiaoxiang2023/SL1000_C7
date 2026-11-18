/*******************************************************************************
文件名称：   xcannet.cpp
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2019.5.30
程序用途：   xcan协议,用于对cpp的封装，用于通信设备通道的封装

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2019.5.30
修改人：胥强
修改内容：初始编写

*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "xcannet.h"
#include <stdio.h>

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
list<XcanNet::stXcanNetTdf> *XcanNet::mListCommNet = new list<XcanNet::stXcanNetTdf>();
const DevXcanTdf *XcanNet::mXcan = XcanInit();
short unsigned int XcanNet::wNetMyDevID = 0;

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
class XcanNetD {
public:
    static bool mMutexInited;
    static list<unsigned char *> *mListInList;
    static list<unsigned int> *mListInLenList;

    // 重载技术器
    static unsigned int mCoreCnt;
    static XCAN_SYS_MUTEX mMutexNet;

    static XCAN_SYS_MUTEX mMutex;
    static bool blNetServiceOn;
};

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/
bool XcanNetD::mMutexInited = false;
list<unsigned char *> *XcanNetD::mListInList = new list<unsigned char *>();
list<unsigned int> *XcanNetD::mListInLenList = new list<unsigned int>();
unsigned int XcanNetD::mCoreCnt = 0;
XCAN_SYS_MUTEX XcanNetD::mMutexNet;

bool XcanNetD::blNetServiceOn = false;
XCAN_SYS_MUTEX XcanNetD::mMutex;

static XcanCoreInPortCb mReceiveHandler = nullptr;

/*******************************************************************************
函数功能：
*   @ 构造函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanNet::XcanNet(unsigned int dwCommChnID, unsigned int dwFrameLen)
{
    XcanNetD::mCoreCnt ++;

    /*
    if (mXcan == nullptr) mXcan = XcanInit();
    if (mListInList == nullptr) mListInList = new list<unsigned char *>();
    if (mListInLenList == nullptr) mListInLenList = new list<unsigned int>();
    if (mListCommNet == nullptr) mListCommNet = new list<stCommChnTdf>();
    */

    mdwCommChnID = dwCommChnID;
    mdwFrameLen = dwFrameLen;

    XcanNet::stXcanNetTdf stComm;

    stComm.pThis = this;
    stComm.dwCommChnID = dwCommChnID;

    stComm.stComm.dwComChnID = dwCommChnID;
    stComm.stComm.dwFrameDataLen = dwFrameLen;
    stComm.stComm.Err = XcanNet::ErrCb;
    stComm.stComm.pfAddFilter = XcanNet::AddFilterCb;
    stComm.stComm.pfIsSendFull = XcanNet::IsSendFullCb;
    stComm.stComm.pfReceive = nullptr;
    stComm.stComm.pfSend = XcanNet::SendCb;
    stComm.stComm.pfSendEmptyNotice = nullptr;
    stComm.stComm.pfSendEmptyNoticeDown = nullptr;
    stComm.stComm.pfSendEmptyNoticeUp = nullptr;

    mListCommNet->push_back(stComm);

    if (XcanNetD::mMutexInited == false) {
        XcanNetD::mMutexInited = true;
        XCAN_SYS_MUTEX_INIT(XcanNetD::mMutexNet);
        XCAN_SYS_MUTEX_INIT(XcanNetD::mMutex);
    }

    // 获取数据输入接口
    if (mReceiveHandler == nullptr) {
        mReceiveHandler = mXcan->GetCoreInPort();
        mXcan->SetMutex(GetMutex, PutMutex);
    }

    // 注册设备数据接口
    mXcan->AddCommIF(&stComm.stComm);
}

/*******************************************************************************
函数功能：
*   @ 构造函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanNet::XcanNet()
{
    XcanNetD::mCoreCnt ++;

    if (XcanNetD::mMutexInited == false) {
        XcanNetD::mMutexInited = true;
        XCAN_SYS_MUTEX_INIT(XcanNetD::mMutexNet);
        XCAN_SYS_MUTEX_INIT(XcanNetD::mMutex);
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
*   @  无
*******************************************************************************/
XcanNet::~XcanNet()
{
    // 防止被错误释放，必须所有的类被释放后才释放所有的资源
    XcanNetD::mCoreCnt --;
    if(XcanNetD::mCoreCnt != 0) return;

    for (; ; ) {
        if (XcanNetD::mListInList->empty() == true) break;

        unsigned char *pGet = XcanNetD::mListInList->front();
        if (pGet != nullptr) delete pGet;

        XcanNetD::mListInLenList->pop_front();
        XcanNetD::mListInList->pop_front();
    }

    for(;;) {
        if (mListCommNet->empty() == true) break;
        mListCommNet->pop_front();
    }

    delete XcanNetD::mListInList;
    delete XcanNetD::mListInLenList;
    delete mListCommNet;
}

/*******************************************************************************
函数功能：
*   @ 获取任务锁
参数描述：
*   @ 无
返回值描述：
*   @ 传输状态数据
说明：
*   @ 无
*******************************************************************************/
void XcanNet::GetMutex(void)
{
    XCAN_SYS_ENTER_CRITICAL(XcanNetD::mMutex);
}

/*******************************************************************************
函数功能：
*   @ 无
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanNet::PutMutex(void)
{
    XCAN_SYS_EXIT_CRITICAL(XcanNetD::mMutex);
}

/*
 * \bref    service down
 * \param   no
 * \retual  no
*/
void XcanNet::NetServiceDown(void)
{
    XcanNetD::blNetServiceOn = false;
}

/*
 * \bref    service on
 * \param   no
 * \retual  no
*/
void XcanNet::NetServiceUp(void)
{
    XcanNetD::blNetServiceOn = true;
}

/*******************************************************************************
函数功能：
*   @ 取得当前需要使用的通信设备
参数描述：
*   @ unsigned int dwDevID 通信ID
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
XcanNet::stXcanNetTdf XcanNet::GetCommDev(unsigned int dwCommChnID)
{
    stXcanNetTdf stNet;

    stNet.dwCommChnID = 0;
    for (std::list<XcanNet::stXcanNetTdf >::iterator it = mListCommNet->begin(); it != mListCommNet->end(); it++) {
        if(it->dwCommChnID == dwCommChnID) {
            stNet = *it;
            break;
        }
    }

    return stNet;
}

/*******************************************************************************
函数功能：
*   @ 设置本机设备ID
参数描述：
*   @ unsigned int dwDevID 本机设备ID
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void  XcanNet::SetMyNetDevID(short unsigned int wDevID)
{
    wNetMyDevID = wDevID;
    mXcan->SetMyDevID(wDevID);
}

/*******************************************************************************
函数功能：
*   @ 获取本机设备ID
参数描述：
*   @ 无
返回值描述：
*   @ unsigned int dwDevID 本机设备ID
说明：
*   @ 无
*******************************************************************************/
short unsigned int XcanNet::GetMyNetDevID(void)
{
    return wNetMyDevID;
}

/*******************************************************************************
函数功能：
*   @ 出错通知C接口
参数描述：
*   @ unsigned int dwComChnID 通信接口
*   @ unsigned int dwRemoteDevID 远程设备ID
*   @ unsigned short int wIoUUID 资源ID
*   @ unsigned char byFrame 帧类型
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanNet::ErrCb(unsigned int dwComChnID, unsigned int dwPort, unsigned int dwRemoteDevID, unsigned short int wIoUUID, unsigned char byFrame)
{
    stXcanNetTdf stNet = GetCommDev(dwComChnID);

    if(!stNet.dwCommChnID) return;
    return stNet.pThis->Err(dwPort, dwRemoteDevID, wIoUUID, byFrame);
}

/*******************************************************************************
函数功能：
*   @ 出错通知
参数描述：
*   @ unsigned int dwComChnID 通信接口
*   @ unsigned int dwRemoteDevID 远程设备ID
*   @ unsigned short int wIoUUID 资源ID
*   @ unsigned char byFrame 帧类型
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanNet::Err(unsigned int, unsigned int, unsigned short int, unsigned char)
{
    ;
}

/*******************************************************************************
函数功能：
*   @ 设置过滤器C接口
参数描述：
*   @ unsigned int dwComChnID 通信接口
*   @ unsigned short int wFilterID 过滤器ID
*   @ unsigned int dwFilterLen 长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanNet::AddFilterCb(unsigned int dwComChnID, unsigned short int wFilterID, unsigned int dwFilterLen)
{
    stXcanNetTdf stNet = GetCommDev(dwComChnID);

    if(!stNet.dwCommChnID) return -1;
    return stNet.pThis->AddFilter(dwComChnID, wFilterID, dwFilterLen);
}

/*******************************************************************************
函数功能：
*   @ 设置过滤器
参数描述：
*   @ unsigned int dwComChnID 通信接口
*   @ unsigned short int wFilterID 过滤器ID
*   @ unsigned int dwFilterLen 长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanNet::AddFilter(unsigned int, unsigned short int, unsigned int)
{
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 通信接口是否工作正常C接口
参数描述：
*   @ unsigned int dwComChnID 通信接口
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanNet::IsSendFullCb(unsigned int dwComChnID, unsigned int dwPort)
{
    stXcanNetTdf stNet = GetCommDev(dwComChnID);

    if(!stNet.dwCommChnID) return true;
    return stNet.pThis->IsSendFull(dwPort);
}

/*******************************************************************************
函数功能：
*   @ 通信接口是否工作正常
参数描述：
*   @ unsigned int dwComChnID 通信接口
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
bool XcanNet::IsSendFull(unsigned int)
{
    return false;
}

/*******************************************************************************
函数功能：
*   @ 数据发送接口C接口
参数描述：
*   @ unsigned int dwComChnID 通信接口
*   @ unsigned int dwId 帧ID
*   @ unsigned char *pbyIn 数据
*   @ unsigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanNet::SendCb(unsigned int dwCommChnID, unsigned int dwPort,
    unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen)
{
    stXcanNetTdf stNet = GetCommDev(dwCommChnID);

    if(!stNet.dwCommChnID) return -1;
    return stNet.pThis->Send(dwPort, dwId, pbyIn, dwLen);
}

/*******************************************************************************
函数功能：
*   @ 数据发送接口
参数描述：
*   @ unsigned int dwComChnID 通信接口
*   @ unsigned int dwId 帧ID
*   @ unsigned char *pbyIn 数据
*   @ unsigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int XcanNet::Send(unsigned int, unsigned int, unsigned char *, unsigned int)
{
    return false;
}

/*******************************************************************************
函数功能：
*   @ 处理网络输入数据
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
int XcanNet::ProcessReceiveQueue(void)
{
    unsigned int dwLen = 0;
    unsigned char *pbyGetFromList = nullptr;
    unsigned char *pbyGet = nullptr;

    // 检查列表空
    XCAN_SYS_ENTER_CRITICAL(XcanNetD::mMutexNet);
    {
        if(XcanNetD::mListInList->empty() == true) {
            XCAN_SYS_EXIT_CRITICAL(XcanNetD::mMutexNet)
            return 0;
        }

        // 得到数据长度
        dwLen = XcanNetD::mListInLenList->front();
        XcanNetD::mListInLenList->pop_front();

        pbyGetFromList = XcanNetD::mListInList->front();
        XcanNetD::mListInList->pop_front();
    }
    XCAN_SYS_EXIT_CRITICAL(XcanNetD::mMutexNet)

    // 得到通信ID
    pbyGet = pbyGetFromList;
    unsigned int dwInDID = *(unsigned int*)pbyGet;
    pbyGet += sizeof(unsigned int);

    // 得到逻辑ID
    unsigned int dwID = ((unsigned int)pbyGet[3] << 24) + ((unsigned int)pbyGet[2] << 16)
        + ((unsigned int)pbyGet[1] << 8) + pbyGet[0];
    pbyGet += sizeof(unsigned int);

    mReceiveHandler(dwInDID, dwID, pbyGet, dwLen - sizeof(unsigned int));
    delete pbyGetFromList;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 协议栈的数据输入，外部调用此类函数接口
参数描述：
*   @ unsigned int dwComChnID 通信通道ID
*   @ unsigned char *pbyIn 输入数据
*   @ unsigned int dwLen 数据长度
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
int XcanNet::In(unsigned int dwInDID, unsigned char *pbyIn, unsigned int dwLen)
{
    if (XcanNetD::blNetServiceOn == false) return -1;
    if (dwLen <= sizeof(unsigned int)) return -1;
    if (mReceiveHandler == nullptr) return -1;

    unsigned char *get = new unsigned char[dwLen + sizeof(unsigned int)];

    // 拷贝数据
    unsigned char *pbyCp = get;
    *(unsigned int*)pbyCp = dwInDID;
    pbyCp += sizeof(unsigned int);

    for (unsigned int i = 0; i < dwLen; i ++) *pbyCp ++ = *pbyIn ++;

    // 加入列表
    XCAN_SYS_ENTER_CRITICAL(XcanNetD::mMutexNet);
    XcanNetD::mListInList->push_back(get);
    XcanNetD::mListInLenList->push_back(dwLen);
    XCAN_SYS_EXIT_CRITICAL(XcanNetD::mMutexNet)

    return 0;
}
