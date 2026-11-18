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
#include "xcanmgr.h"
#include "xcanio.h"
#include "wthread/WThread.h"

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
class XcanMgrD {
public:
    static bool blStackOn;
    static WThread mXcanThread;
};

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/
bool XcanMgrD::blStackOn = false;
WThread XcanMgrD::mXcanThread;

/*******************************************************************************
函数功能：
*   @ 实例构造
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanMgr::XcanMgr()
{
    XcanMgrD::blStackOn = false;
}

/*******************************************************************************
函数功能：
*   @ 析构
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
XcanMgr::~XcanMgr()
{
    XcanMgrD::blStackOn = false;
}

/*******************************************************************************
函数功能：
*   @ 启动协议栈，开始接受外部数据
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
int XcanMgr::StackUp()
{
    mXcan->SetTimPort(XcanMgr::GetSysTim);
    XcanMgrD::blStackOn = true;
    XcanNet::NetServiceUp();

    XcanMgrD::mXcanThread.SetTask(ServiceProcess);
    XcanMgrD::mXcanThread.start();
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭协议栈，取消数据接收
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
int XcanMgr::StackDown()
{
    XcanMgrD::blStackOn = false;
    XcanNet::NetServiceDown();

    XcanMgrD::mXcanThread.quit();
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 获取系统的时间节拍。
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
unsigned int XcanMgr::GetSysTim(void)
{
    static unsigned int nInterval = 0;
    static unsigned int nPreTime = 0;
    unsigned int nCurTime = 0;

#if(PLAT_WIN32_VC|PLAT_WIN32)
    nCurTime = GetTickCount();
#endif

#if(PLAT_LINUX_QT|PLAT_LINUX)
    struct timeval tv;
    gettimeofday(&tv,nullptr);
    nCurTime = (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec/1000);
#endif

    if(nPreTime)
    {
        nInterval += (nCurTime >= nPreTime) ? (nCurTime - nPreTime) : (nCurTime + 0xffffffff - nCurTime);//nCurTime + 0xffffffff - nCurTime 处理数据溢出
    }
    nPreTime = nCurTime;

    return nInterval;
}

/*!
 * \brief  set Relink time
 * \param  [in]  unsigned int dwTim: tim out
 * \retval no
*/
int XcanMgr::SetRelinkTim(unsigned int dwTim)
{
    XcanNet::mXcan->SetRelinkTim(dwTim);
    return 0;
}

/*!
 * \brief  set re trs time out
 * \param  [in]  unsigned int dwTim: tim out
 * \retval no
*/
int XcanMgr::SetReTrsTimOut(unsigned int dwTim)
{
    XcanNet::mXcan->SetReTrsTimOut(dwTim);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 服务主进程
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanMgr::Service(void)
{
    static int pre = 0;

    if(XcanMgrD::blStackOn != true) return;
    // 处理输入数据
    XcanIO::BridgeService();
    XcanNet::ProcessReceiveQueue();

    // 服务
    if (pre++ > 2) {
        XcanNet::mXcan->Service();
        pre = 0;
    }
}

/*******************************************************************************
函数功能：
*   @ 服务主进程
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @  无
*******************************************************************************/
void XcanMgr::ServiceProcess(void *)
{
    while(XcanMgrD::blStackOn) {
        XcanMgr::Service();
        WThread::msSleep(5);  // 5ms
    }
}
