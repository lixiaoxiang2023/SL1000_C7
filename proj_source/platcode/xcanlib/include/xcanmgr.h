/*******************************************************************************
文件名称：   xcannet.h
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2019.5.30
程序用途：   xcan协议,用于对cpp的封装,主要是对通底层通信接口的支持

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2019.5.30
修改人：胥强
修改内容：初始编写

*******************************************************************************/
#ifndef __XCANMGR_H
#define __XCANMGR_H

/*******************************************************************************
Include file
*******************************************************************************/
#include <list>
#include "xcannet.h"

using namespace std;

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class XCAN_EXPORTS_API XcanMgr : public XcanNet {

public:
    XcanMgr();
    virtual ~XcanMgr();

    static int StackUp(void);
    static int StackDown(void);
    static unsigned int GetSysTim(void);

    /*!
     * \brief  set Relink time
     * \param  [in]  unsigned int dwTim: tim out
     * \retval no
    */
    static int SetRelinkTim(unsigned int dwTim);

    /*!
     * \brief  set re trs time out
     * \param  [in]  unsigned int dwTim: tim out
     * \retval no
    */
    static int SetReTrsTimOut(unsigned int dwTim);

    static void Service(void);
    static void ServiceProcess(void *);

};
#endif // __XCANNET_H
