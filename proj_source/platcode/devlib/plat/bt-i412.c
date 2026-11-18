/*******************************************************************************
文件名称：     bluetooth.c
目录：         plat/code/stdlib
项目名称：     事业一部技术平台
创建者：       xuqiang
创建日期：     20151123
程序用途：

版权声明：
            Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
20151123：
第一次由XQ创建初始程序。
2017.11.8 由胥强 修改 标准化并加入技术平台
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_BT_I412)
/*******************************************************************************
常量定义
*******************************************************************************/
#define AT_GET_VERSION          0
#define AT_GET_PAR              1
#define AT_SET_PAR_OFF          2
#define AT_SET_PAR_ON           3
#define AT_GET_BAUD             4
#define AT_SET_BAUD             5
#define AT_SET_FACT             6
#define AT_SET_RESET            7
#define AT_SET_NAME             8
#define AT_GET_NAME             9
#define AT_GET_ADDR            10
#define AT_SET_AVD_OFF         11
#define AT_SET_AVD_ON          12
#define AT_DEL_PRD             13
#define AT_GET_PRD             14
#define AT_SET_ROLE_S          15
#define AT_SET_ROLE_M          16
#define AT_SET_SEARCH          17
#define AT_SET_STOPSEARCH      18
#define AT_GET_ROLE            19

#define State0                  0
#define State1                  1
#define State2                  2
#define State3                  3
#define State4                  4
#define State5                  5
#define State6                  6
#define State7                  7
#define State8                  8
#define State9                  9
#define State10                 10
#define State11                 11
/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static const unsigned char* const AT_CMD[22][2] = {
    {"AT+B GVER\r",     "GVER "    },   //0
    {"AT+B GPAR\r",     "GPAR "    },   //1
    {"AT+B EPAR 0\r",   "EPAR "    },   //2
    {"AT+B EPAR 1\r",   "EPAR "    },   //3

    {"AT+B GBAUD\r",    "GTPL "    },   //4
    {"AT+B SBAUD",      "SBAUD "   },   //5
    {"AT+B FACT\r",     "FACT "    },   //6
    {"AT+B RESET\r",    "RESET "   },   //7

    {"AT+B SLDN ",      "SLDN "    },   //8
    {"AT+B GLDN\r",     "GLDN "    },   //9
    {"AT+B GLBD\r",     "GLBD "    },   //10
    {"AT+B ADV 0\r",    "ADV "     },   //11

    {"AT+B ADV 1\r",    "ADV "     },   //12
    {"AT+B DPRD\r",     "DPRD "    },   //13
    {"AT+B GPRD\r",     "GPRD "    },   //14
    {"AT+B SROLE 1\r",  "SROLE "   },   //15

    {"AT+B SROLE 0\r",  "SROLE "   },   //16
    {"AT+B SCAN 1\r",   "SCAN  "   },   //17
    {"AT+B SCAN 0\r",   "SCAN  "   },   //18
    {"AT+B GROLE\r",    "GROLE "   },   //19
};

static struct {
#if(CONFIG_BT_DEBUG)
    bool blDebug;
    unsigned char byDebugUart;
#endif

    unsigned char byUartNO;
    unsigned int  *pwdBtStaBit0;
    unsigned int  *pwdBtStaBit1;
    unsigned int  *pwdBtUnlink;

    // 连接状态
    bool blDevExsit;

    // 接收逻辑
    bool blBusy;
    unsigned char byRxCnt;
    unsigned char byaRx[CONFIG_BT_RX_BUFF_SIZE];
    unsigned char byaCmd[CONFIG_BT_RX_BUFF_SIZE];

    char byaConn[6];
    unsigned char byaMac[14];
    unsigned char byaName[22];
    unsigned char byaSearch[50];
    void (*Notice) (unsigned char);

    u16 wRxCnt;
    unsigned char byaRxDev[250];//搜索设备
    bool blSearchDev;
    char byBTAddr[6];
    u8 cur_state;
    bool blBTAddrget;

}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
*函数功能：
*   @ 开始接收数据
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void StartRx(void)
{
    memset(s_stCtr.byaRx, 0, CONFIG_BT_RX_BUFF_SIZE);
    memset(s_stCtr.byaCmd, 0, CONFIG_BT_RX_BUFF_SIZE);

    s_stCtr.byRxCnt = 0;
    s_stCtr.blBusy = true;
}
/*******************************************************************************
*函数功能：
*   @ 开始接收数据
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void StartRxDev(void)
{
    memset(s_stCtr.byaRxDev, 0, sizeof(s_stCtr.byaRxDev));

    s_stCtr.wRxCnt = 0;
    s_stCtr.blBusy = true;
}
/*******************************************************************************
*函数功能：
*   @ 根据BT状态管脚，判断BT状态
*参数描述：
*   @ 无
*返回值描述：
*   @ BT_I412_STATUS_IDLE             BT空闲
*   @ BT_I412_STATUS_AVD              BT广播中
*   @ BT_I412_STATUS_LINKED_EMPTY     BT连接, 缓冲区空
*   @ BT_I412_STATUS_LINKED_FULL      BT连接，缓冲区不空
*说明：
*   @ 无
*******************************************************************************/
static int Status(void)
{
    int iStatus = 0;

    if(!s_stCtr.pwdBtStaBit0) return -1;
    if(!s_stCtr.pwdBtStaBit1) return -1;

    if(*s_stCtr.pwdBtStaBit0) iStatus = 1;
    if(*s_stCtr.pwdBtStaBit1) iStatus |= 2;

    return iStatus;
}

#if(CONFIG_BT_IS_CONNECTED)
/*******************************************************************************
*函数功能：
*   @ 检查蓝牙是否连接
*参数描述：
*   @ 无
*返回值描述：
*   @ true 蓝牙已经连接
*   @ false 蓝牙未连接
*说明：
*   @ 无
*******************************************************************************/
static bool IsConnected(void)
{
    return (*s_stCtr.pwdBtStaBit1)? true:false;
}
#endif

/*******************************************************************************
*函数功能：
*   @ 蓝牙数据接收处理模块
*参数描述：
*   @ unsigned char byIn 底层驱动传入的数据
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void BTRxHandler(unsigned char byIn)
{
    // 设备连接，通知上层
    if(*s_stCtr.pwdBtStaBit1) {
        if(s_stCtr.Notice) s_stCtr.Notice(byIn);
        return;
    }
    if( s_stCtr.blSearchDev ){
        //增加搜索设备
        switch(s_stCtr.cur_state){ //根据当前状态以及当前输入切换下一状态
        case State0 :
            s_stCtr.blBTAddrget=false;
            if(byIn=='0') s_stCtr.cur_state=State2;
            else
                s_stCtr.cur_state=State0;
            break;
        case State1 :
            s_stCtr.blBTAddrget=true;
            s_stCtr.cur_state=State1;
            break;
        case State2 :
            if(byIn=='0')
                s_stCtr.cur_state=State3;
            else if(byIn=='4')
                s_stCtr.cur_state=State7;
            else
                s_stCtr.cur_state=State0;
            break;
        case State3 :
            if(byIn=='1')
                s_stCtr.cur_state=State4;
            else
                s_stCtr.cur_state=State0;
            break;
        case State4 :
            if(byIn=='5')
                s_stCtr.cur_state=State5;
            else
                s_stCtr.cur_state=State0;
            break;
        case State5 :
            if(byIn=='8')
                s_stCtr.cur_state=State6;
            else
                s_stCtr.cur_state=State0;
            break;
        case State6 :
            if(byIn=='3')
                s_stCtr.cur_state=State1;
            else
                s_stCtr.cur_state=State0;
            break;
        case State7 :
            if(byIn=='7')
                s_stCtr.cur_state=State8;
            else
                s_stCtr.cur_state=State0;
            break;
        case State8 :
            if(byIn=='F')
                s_stCtr.cur_state=State9;
            else
                s_stCtr.cur_state=State0;
            break;
        case State9 :
            if(byIn=='0')
                s_stCtr.cur_state=State10;
            else
                s_stCtr.cur_state=State0;
            break;
        case State10:
            if(byIn=='E')
                s_stCtr.cur_state=State1;
            else
                s_stCtr.cur_state=State0;
            break;
        default: s_stCtr.cur_state=State0;
        }
        if(s_stCtr.wRxCnt < 50+200*s_stCtr.blBTAddrget && s_stCtr.blBusy == true){
            //未匹配到地址段前以50字节长度重复缓存，匹配到后多存200字节，收录“31_printer”自定义字段作为对打印机的校验
            s_stCtr.byaRxDev[s_stCtr.wRxCnt] = byIn;
            s_stCtr.wRxCnt ++;
        }
        else if(!s_stCtr.blBTAddrget&& s_stCtr.blBusy == true){//未搜索到，持续缓存
            s_stCtr.wRxCnt=0;
            s_stCtr.cur_state=State0;//状态置0，重新查找
        }
    }
    else{
        if(1) {
            s_stCtr.byaConn[0] = s_stCtr.byaConn[1];
            s_stCtr.byaConn[1] = s_stCtr.byaConn[2];
            s_stCtr.byaConn[2] = s_stCtr.byaConn[3];
            s_stCtr.byaConn[3] = s_stCtr.byaConn[4];
            s_stCtr.byaConn[4] = byIn;

            if( !strcmp("AT-B ", s_stCtr.byaConn)) {
                StartRx();
                return;
            }
        }

        if(byIn == '\r') {
            s_stCtr.blBusy = false;
            return;
        }
        if(s_stCtr.byRxCnt < CONFIG_BT_RX_BUFF_SIZE && s_stCtr.blBusy == true) {
            s_stCtr.byaRx[s_stCtr.byRxCnt] = byIn;
           s_stCtr.byRxCnt ++;
        }

#if(CONFIG_BT_DEBUG)
        if(s_stCtr.blDebug) {
            DevUart->SendByte(s_stCtr.byDebugUart, byIn);
        }
#endif
    }
}

#if(CONFIG_BT_DEBUG)
/*******************************************************************************
*函数功能：
*   @ 蓝牙调试数据发送接口
*参数描述：
*   @ unsigned char byIn 底层驱动传入的数据
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void DebugTxHandler(unsigned char byIn)
{
    DevUart->SendByte(s_stCtr.byDebugUart, byIn);
    DevUart->SendByte(s_stCtr.byUartNO, byIn);
}
#endif

/*******************************************************************************
*函数功能：
*   @ 等待命令结束
*参数描述：
*   @ unsigned char byCmd 命令序号
*   @ unsigned int dwTimOut 超时时间
*返回值描述：
*   @  0 数据正常
*   @ -1 失败
*说明：
*   @ 无
*******************************************************************************/
static int Wait(unsigned char byCmd, unsigned int dwTimOut)
{
    bool blOK = false;

    for(int wait = 0; wait < dwTimOut; wait ++) {
        if(s_stCtr.blBusy != true) {
            blOK = true;
            break;
        }
    }
    s_stCtr.blBusy  = false;
    return (blOK)?0:-1;
}

/*******************************************************************************
*函数功能：
*   @ 发送控制命令
*参数描述：
*   @ unsigned char byCmd 需要发送的命令序号
*   @ unsigned int dwTimOut 等待数据返回时间
*   @ const unsigned char *pbyMore 其他的新增信息
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static int WrCmd(unsigned char byCmd, unsigned int dwTimOut, const unsigned char *pbyMore)
{
    const unsigned char *pcCmd = NULL;
    unsigned char byLen = 0;

    if(s_stCtr.blBusy) return -1;

    StartRx();

    pcCmd = AT_CMD[byCmd][0];
    byLen = strlen((const char *)pcCmd);
    DevUart->SendString(s_stCtr.byUartNO, (const unsigned char*)pcCmd, byLen);
    if(pbyMore) {
        byLen = strlen((const char *)pbyMore);
        DevUart->SendString(s_stCtr.byUartNO, pbyMore, byLen);
        DevUart->SendString(s_stCtr.byUartNO, "\r", 1);
    }

#if(CONFIG_BT_DEBUG)
    if(s_stCtr.blDebug) {
        DevUart->SendString(s_stCtr.byDebugUart, (const unsigned char*)pcCmd, byLen);
        DevUart->SendString(s_stCtr.byDebugUart, "\r", byLen);
    }
#endif

    return Wait(byCmd, dwTimOut);
}

/*******************************************************************************
*函数功能：
*   @ 检查响应的指令是否正确
*参数描述：
*   @ bool blIsMaster: true 主模式 false 从模式
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static const unsigned char *CheakRet(unsigned char byCmd)
{
    const unsigned char *pcCmd = NULL;
    unsigned char *pcRet = s_stCtr.byaRx;

    if(s_stCtr.blBusy) return 0;

    pcCmd = AT_CMD[byCmd][1];
    for(int i = 0; i < CONFIG_BT_RX_BUFF_SIZE; i ++) {
        if(*pcCmd != *pcRet) break;
        pcCmd ++;
        pcRet ++;
    }
    return (const unsigned char *)pcRet;
}

/*******************************************************************************
*函数功能：
*   @ 读取蓝牙默认机当前的主从模式
*参数描述：
*   @ bool blIsCurrent true 读取当前主从模式 false  读取默认主从模式
*返回值描述：
*   @  0 Central 主机
*   @  1 Peirpleral 从机
*   @  2 iBeacon
*   @  3 unKnown

*说明：
*   @ 无
*******************************************************************************/
static int Get_Cur_Role(bool blIsCurrent)
{
    const unsigned char *pbyRet = NULL;

    if( WrCmd(AT_GET_ROLE , 0x5fffff, NULL) ) return -1;
    pbyRet = CheakRet(AT_GET_ROLE);

    if(!pbyRet) return -1;

    if(blIsCurrent)
        return *(pbyRet+2)-'0'; //返回当前主从模式
    else
        return  *pbyRet-'0'; //返回默认的主从模式
}
#if(CONFIG_BT_SET_ROLE)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙主从模式
*参数描述：
*   @ bool blIsMaster: true 主模式 false 从模式
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static int SetRole(bool blIsMaster)
{
    const unsigned char *pbyRet = NULL;

    if(blIsMaster == true) {
        if( WrCmd(AT_SET_ROLE_M , 0x5fffff, NULL) ) return -1;
        pbyRet = CheakRet(AT_SET_ROLE_M);
    }
    else {
        if( WrCmd(AT_SET_ROLE_S , 0x5fffff, NULL) ) return -1;
        pbyRet = CheakRet(AT_SET_ROLE_S);
    }

    if(!pbyRet) return -1;
    return (*pbyRet == 0x31)?1:0;
}
#endif

#if(CONFIG_BT_AUTO_LINK)
/*******************************************************************************
*函数功能：
*   @ 设置支持配对
*参数描述：
*   @ true: 支持配对
*   @ false 不支持配对
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @ 主模式使用
*******************************************************************************/
static int AutoLink(bool blAutoLink)
{
    const unsigned char *pbyRet = NULL;

    if(blAutoLink) {
        if( WrCmd(AT_SET_PAR_ON , 0x7fffff, NULL) ) return -1;
        pbyRet = CheakRet(AT_SET_PAR_ON);
    }
    else {
        if( WrCmd(AT_SET_PAR_OFF , 0x7fffff, NULL) ) return -1;
        pbyRet = CheakRet(AT_SET_PAR_OFF);
    }

    if(!pbyRet) return -1;
    return (*pbyRet == 0x31)?1:0;
}
#endif

#if(CONFIG_BT_SET_BAUD)
/*******************************************************************************
*函数功能：
*   @ 设置波特率
*参数描述：
*   @ 波特率
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @ 设置波特率后需要调用 获取 波特率函数
*******************************************************************************/
static int SetBaud(unsigned int dwBaud)
{
//    if(dwBaud == 1200 )         return WrCmd(AT_SET_BAUD1200 ,  0x7fffff, 0);
//    else if(dwBaud == 2400 )    return WrCmd(AT_SET_BAUD2400 ,  0x7fffff, 0);
//    else if(dwBaud == 4800 )    return WrCmd(AT_SET_BAUD4800 ,  0x7fffff, 0);
//    else if(dwBaud == 9600 )    return WrCmd(AT_SET_BAUD9600 ,  0x7fffff, 0);
//    else if(dwBaud == 19200 )   return WrCmd(AT_SET_BAUD19200 , 0x7fffff, 0);
//    else if(dwBaud == 38400 )   return WrCmd(AT_SET_BAUD38400 , 0x7fffff, 0);
//    else if(dwBaud == 57600 )   return WrCmd(AT_SET_BAUD57600 , 0x7fffff, 0);
//    else return WrCmd(AT_SET_BAUD115200 , 0x7fffff, 0);
    return 0;
}
#endif

#if(CONFIG_BT_SET_DEFAULT)
/*******************************************************************************
*函数功能：
*   @ 设置恢复出厂
*参数描述：
*   @ 无
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static int SetDefault(void)
{
    const unsigned char *pbyRet = NULL;

    if( WrCmd(AT_SET_FACT , 0x5fffff, NULL) ) return -1;
    pbyRet = CheakRet(AT_SET_FACT);

    if(!pbyRet) return -1;
    return ((*pbyRet) == 0x30)?0:-1;
}
#endif

#if(CONFIG_BT_SET_RESET)
/*******************************************************************************
*函数功能：
*   @ 设置重启
*参数描述：
*   @ 无
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static int SetReset(void)
{
    const unsigned char *pbyRet = NULL;

    if( WrCmd(AT_SET_RESET , 0x7fffff, NULL) ) return -1;
    pbyRet = CheakRet(AT_SET_RESET);

    if(!pbyRet) return -1;
    return (*pbyRet == 0x30)?0:-1;
}
#endif

#if(CONFIG_BT_SET_NAME)
/*******************************************************************************
*函数功能：
*   @ 设置BT名字
*参数描述：
*   @ const char *pcName：蓝牙名字
*返回值描述：
*   @  0 设置成功
*   @ -1 命令执行失败
*说明：
*   @ 名字不得大于20字节。
*******************************************************************************/
static int SetName(const char *pcName)
{
    char caName[21];
    const unsigned char *pbyRet = NULL;

    memset(caName, 0, 21);
    for(int i = 0; i < 20 && *pcName != 0; i ++, pcName ++) caName[i] = *pcName;

    if( WrCmd(AT_SET_NAME , 0x3fffff, (const unsigned char *)caName) ) return -1;
    pbyRet = CheakRet(AT_SET_NAME);

    if(!pbyRet) return -1;

    for(int i = 0; i < 20 && *pcName != 0; i ++, pcName ++) {
        s_stCtr.byaName[i] = *pcName;
    }

    return 0;
}
#endif

#if(CONFIG_BT_GET_NAME)
/*******************************************************************************
*函数功能：
*   @ 读取BT名字
*参数描述：
*   @ const char *pcName：蓝牙名字
*返回值描述：
*   @  0 设置成功
*   @ -1 命令执行失败
*说明：
*   @ MAC 12个字节。
*******************************************************************************/
static int GetName(char *pcName)
{
    const unsigned char *pbyRet = NULL;

    if(!pcName) return -1;
    if(IsConnected()) {
        for(int i = 0; i < 20 && s_stCtr.byaName[i] != 0; i ++, pbyRet ++) pcName[i] = s_stCtr.byaName[i];
        return 0;
    }
    //s_stCtr.blBusy = false;
    if( WrCmd(AT_GET_NAME , 0x1fffff, NULL)) return -1;
    pbyRet = CheakRet(AT_GET_NAME);

    if(!pbyRet) return -1;
    for(int i = 0; i < 20 && *pbyRet != 0; i ++, pbyRet ++) pcName[i] = *pbyRet;

    return 0;
}
#endif

#if(CONFIG_BT_GET_MAC)
/*******************************************************************************
*函数功能：
*   @ 获取蓝牙地址
*参数描述：
*   @ 无
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 主模式使用
*******************************************************************************/
static int GetMac(char *pbyMac)
{
    const unsigned char *pbyRet = NULL;

    if(!pbyMac) return -1;
    if(IsConnected()) {
        for(int i = 0; i < 20 && s_stCtr.byaMac[i] != 0; i ++, pbyRet ++) pbyMac[i] = s_stCtr.byaMac[i];
        return 0;
    }

    if( WrCmd(AT_GET_ADDR , 0x1fffff, NULL)) return -1;
    pbyRet = CheakRet(AT_GET_ADDR);

    if(!pbyRet) return -1;
    if(*pbyRet == 1) return 1;
    //pbyRet += 2;
    for(int i = 0; i < 12 && *pbyRet != 0; i ++, pbyRet ++) pbyMac[i] = *pbyRet;

    return 0;
}
#endif

#if(CONFIG_BT_SET_ADV)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙是否开始广播
*参数描述：
*   @  true 开始广播
*   @ false 停止广播
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static int Advert(bool blIsOn)
{
    const unsigned char *pbyRet = NULL;

    if(blIsOn) {
        if( WrCmd(AT_SET_AVD_ON , 0x7fffff, NULL) ) return -1;
        pbyRet = CheakRet(AT_SET_AVD_ON);
    }
    else {
        if( WrCmd(AT_SET_AVD_OFF , 0x7fffff, NULL) ) return -1;
        pbyRet = CheakRet(AT_SET_AVD_OFF);
    }

    if(!pbyRet) return -1;
    return (*pbyRet == 0x30)?0:1;
}
#endif

#if(CONFIG_BT_CLEAR)
/*******************************************************************************
*函数功能：
*   @ 清除配置信息
*参数描述：
*   @ 无
*返回值描述：
*   @  1 设置成功
*   @  0 设置失败
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static int Clear(void)
{
    const unsigned char *pbyRet = NULL;

    if( WrCmd(AT_DEL_PRD , 0x7fffff, NULL) ) return -1;
    pbyRet = CheakRet(AT_DEL_PRD);

    if(!pbyRet) return -1;
    return (*pbyRet == 0x30)?0:1;
}
#endif

#if(CONFIG_BT_GET_CONN_INFO)
/*******************************************************************************
*函数功能：
*   @ 清除配置信息
*参数描述：
*   @ 无
*返回值描述：
*   @  1 有配置信息
*   @  0 无配置信息
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static int HasConnInfo(void)
{
    const unsigned char *pbyRet = NULL;

    if( WrCmd(AT_GET_PRD , 0x7fffff, NULL) ) return -1;
    pbyRet = CheakRet(AT_GET_PRD);

    if(!pbyRet) return -1;
    return (*pbyRet != 0x30)?1:0;
}
#endif
/*******************************************************************************
*函数功能：
*   @ 清除配置信息
*参数描述：
*   @ 无
*返回值描述：
*   @  1 有配置信息
*   @  0 无配置信息
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
/*******************************************************************************
*函数功能：
*   @ 清除配置信息
*参数描述：
*   @ 无
*返回值描述：
*   @  1 有配置信息
*   @  0 无配置信息
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static int Search(u8 *pbyMac)
{
    char *pbyPos = NULL;
    u32 dwCnt = 0;
    u8 temp =0;
    if( WrCmd(AT_SET_SEARCH , 0x2fffff, NULL) ){
        if( WrCmd(AT_SET_STOPSEARCH , 0x7fffff, NULL) );
        s_stCtr.blBusy = false;
        return temp;
    }
    while( s_stCtr.blBusy == true){//判断蓝牙是否为忙碌状态
        if(dwCnt++>0xffff){
            s_stCtr.blSearchDev =0;
        }
        break;
    };
      StartRxDev();
      s_stCtr.blSearchDev =true;
      s_stCtr.cur_state=0;
      s_stCtr.blBTAddrget=false;//检索地址标志,false:未检索到 true：检索到
      for(long i=0;i<0x1ffffff;i++);
      s_stCtr.blBusy = false;
      s_stCtr.blSearchDev =0;
      if(s_stCtr.wRxCnt>135){//进行查找判断
          pbyPos = strstr((char const*)s_stCtr.byaRxDev,"U31_printer");
          if(pbyPos){
              if(*(pbyPos-134)!=0)
                  memcpy(pbyMac,pbyPos-134,12);
              temp = 1;
          }
          else memcpy(pbyMac,0,12);
      }
    if( WrCmd(AT_SET_STOPSEARCH , 0x7fffff, NULL) );
    return temp;
}
/*******************************************************************************
*函数功能：
*   @ 发送搜索指令
*参数描述：
*   @ 无
*返回值描述：
*   @  1 有配置信息
*   @  0 无配置信息
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static void SendSearchCmd(void)
{
    u32 dwCnt = 0;
    if( WrCmd(AT_SET_SEARCH , 0x2fffff, NULL) ){
        if( WrCmd(AT_SET_STOPSEARCH , 0x7fffff, NULL) );
        s_stCtr.blBusy = false;
        return ;
    }
    while( s_stCtr.blBusy == true){//判断蓝牙是否为忙碌状态
        if(dwCnt++>0xffff){
            s_stCtr.blSearchDev =0;
        }
        break;
    };
      StartRxDev();
      s_stCtr.blSearchDev =true;
      s_stCtr.cur_state=0;
      s_stCtr.blBTAddrget=false;//检索地址标志,false:未检索到 true：检索到
}
/*******************************************************************************
*函数功能：
*   @ 获取蓝牙打印机蓝牙地址
*参数描述：
*   @ 无
*返回值描述：
*   @  1 有配置信息
*   @  0 无配置信息
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static int GetPrinterMac(u8 *pbyMac)
{
    char *pbyPos = NULL;
    u8 temp =0;
    s_stCtr.blBusy = false;
    s_stCtr.blSearchDev =0;
    if(s_stCtr.wRxCnt>135){//进行查找判断
        pbyPos = strstr((char const*)s_stCtr.byaRxDev,"U31_printer");
        if(pbyPos){
            if(*(pbyPos-134)!=0)
                memcpy(pbyMac,pbyPos-134,12);
            temp = 1;
        }
        else memcpy(pbyMac,0,12);
    }
    if( WrCmd(AT_SET_STOPSEARCH , 0x7fffff, NULL) );
    return temp;
}
/*******************************************************************************
*函数功能：
*   @ 清除配置信息
*参数描述：
*   @ 无
*返回值描述：
*   @  1 有配置信息
*   @  0 无配置信息
*   @ -1 命令执行失败
*说明：
*   @
*******************************************************************************/
static int ConnectDevice(u8 *pbyDecMac)
{
    u32 dwCnt   = 0;
    StartRxDev();
    s_stCtr.blSearchDev =true;
    DevUart->SendString(s_stCtr.byUartNO, (const unsigned char*)pbyDecMac, strlen((char const*)pbyDecMac ));
    for(int i=0;i<0xfffff;i++);
    while(IsConnected()==false){
        if(dwCnt++ >0xffff){
            s_stCtr.blBusy = false;
            s_stCtr.blSearchDev =0;
            return -1;
        }
    }
    for(int i=0;i<0xffff;i++);
    s_stCtr.blSearchDev =0;
    s_stCtr.blBusy = false;
    return 0;

}
/*******************************************************************************
*函数功能：
*   @ 数据发送
*参数描述：
*   @ unsigned char *pbyIn 数据输入地址
*   @ unsigned int dwLen 数据输入长度
*返回值描述：
*   @  0 成功
*   @ -1 失败
*说明：
*   @ 无
*******************************************************************************/
static int Write(unsigned char *pbyIn, unsigned int dwLen)
{
    unsigned int dwTimOut;
    if(!IsConnected()) return -1;
    if(!s_stCtr.blDevExsit) return -1;

    for(int i = 0 ; i < dwLen; i ++) {
        dwTimOut = 0xfffffff;

        while(Status() != BT_I412_STATUS_LINKED_EMPTY) {
            if(dwTimOut == 0) return -1;
            if(!*s_stCtr.pwdBtStaBit1) return -1;
            dwTimOut --;
        }

        if(dwTimOut == 0) return -1;
        DevUart->SendByte(s_stCtr.byUartNO, pbyIn[i]);

    }

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 数据发送
*参数描述：
*   @ unsigned char *pbyIn 数据输入地址
*   @ unsigned int dwLen 数据输入长度
*返回值描述：
*   @  0 成功
*   @ -1 失败
*说明：
*   @ 无
*******************************************************************************/
static int MainWrite(unsigned char *pbyIn, unsigned int dwLen)
{

    u16 wLen=0;
    if(!IsConnected()) return -1;
    if(!s_stCtr.blDevExsit) return -1;

    u8 abySend[42]={"AT+B WRITE 1,A,13,"};
    //    u8 abySend1[42]={"AT+B WRITE 1,A,13,\x02\x0d\x0aNo.000007  2000-\x0d"};
    unsigned int dwCnt=0;

    while(1)
    {
        if(dwCnt<=dwLen-19&&dwLen>19)
        {
            memset(&abySend[18],0,22);
            memcpy(&abySend[18],&pbyIn[dwCnt],19);
            abySend[strlen((char const*)abySend)] = 0x0d;

            wLen = strlen((char const*)abySend);
            DevUart->SendString(s_stCtr.byUartNO, &abySend[0],wLen);
            dwCnt+=19;
            for(int i=0;i<0xaffff;i++);

        }
        else {
            wLen = dwLen-dwCnt;
            memset(&abySend[18],0,22);
            memcpy(&abySend[18],&pbyIn[dwCnt],wLen);
            if( wLen>=16){
                abySend[15] = '1';
                abySend[16] = wLen-16+0x30;
            }
            else  {
                abySend[15] = '0';
                if(wLen>=10)
                    abySend[16] = wLen-10+0x41;
                else
                    abySend[16] = wLen+0x30;
            }
            abySend[strlen((char const*)abySend)] = 0x0d;
            wLen = strlen((char const*)abySend);
            DevUart->SendString(s_stCtr.byUartNO, &abySend[0],wLen);
            for(int i=0;i<0xaffff;i++);
            break;
        }
    }
    return 0;
}
#if(CONFIG_BT_UNLINK)
/*******************************************************************************
*函数功能：
*   @ 断开连接
*参数描述：
*   @ true 清除配置信息
*   @ false 断开连接
*返回值描述：
*   @  0 成功
*   @ -1 失败
*说明：
*   @ 模块处于待机状态下，PIO0接地100MS, 断开所有连接
*******************************************************************************/
static void Unlink(bool blClrInfo)
{
   u32 dwCnt =0;
    *s_stCtr.pwdBtUnlink = 0;
    while(IsConnected()) {
      if(dwCnt++>0xffffff)
        break;}
    *s_stCtr.pwdBtUnlink = 1;

    if(!blClrInfo) return;
    Clear();
}
#endif

#if(CONFIG_BT_SET_NITOCE_HANLDER)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙接收数据后通知接口
*参数描述：
*   @ void (*Hanlder) (unsigned char) 通知接口回调
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void SetNoticehandler(void (*Hanlder) (unsigned char))
{
    s_stCtr.Notice = Hanlder;
}
#endif

#if(CONFIG_BT_IS_DEV_EXIST)
/*******************************************************************************
*函数功能：
*   @ 检查蓝牙是否连接
*参数描述：
*   @ 无
*返回值描述：
*   @ true 设备存在
*   @ false 设备不存在
*说明：
*   @ 无
*******************************************************************************/
static bool IsDevExist(void)
{
    return s_stCtr.blDevExsit;
}
#endif

#if(CONFIG_BT_DEBUG)
/*******************************************************************************
*函数功能：
*   @ 调试模块，将数据发送到相应的调试串口
*参数描述：
*   @ bool blOn :true 打开调试 false 关闭调试
*   @ unsigned int dwDebugUart：调试的时候将数据发到对应的串口
*返回值描述：
*   @  0 成功
*   @ -1 失败
*说明：
*   @ 无
*******************************************************************************/
static void Debug(bool blOn, unsigned int dwDebugUart)
{
    if(!blOn) {
        s_stCtr.blDebug = false;
        return ;
    }

    s_stCtr.blDebug = true;
    s_stCtr.byDebugUart = dwDebugUart;

    DevUart->RegHandler( s_stCtr.byDebugUart, DebugTxHandler);
}
#endif

static const DevBti412Tdf s_Dev = {
#if(CONFIG_BT_IS_CONNECTED)
    IsConnected,
#endif

#if(CONFIG_BT_SET_ROLE)
    SetRole,
#endif

#if(CONFIG_BT_AUTO_LINK)
    AutoLink,
#endif

#if(CONFIG_BT_SET_BAUD)
    SetBaud,
#endif

#if(CONFIG_BT_SET_DEFAULT)
    SetDefault,
#endif

#if(CONFIG_BT_SET_RESET)
    SetReset,
#endif

#if(CONFIG_BT_SET_NAME)
    SetName,
#endif

#if(CONFIG_BT_GET_NAME)
    GetName,
#endif

#if(CONFIG_BT_GET_MAC)
    GetMac,
#endif

#if(CONFIG_BT_SET_ADV)
    Advert,
#endif

#if(CONFIG_BT_CLEAR)
    Clear,
#endif

#if(CONFIG_BT_GET_CONN_INFO)
    HasConnInfo,
#endif

    Write,

#if(CONFIG_BT_UNLINK)
    Unlink,
#endif

#if(CONFIG_BT_SET_NITOCE_HANLDER)
    SetNoticehandler,
#endif

#if(CONFIG_BT_IS_DEV_EXIST)
    IsDevExist,
#endif

#if(CONFIG_BT_DEBUG)
    Debug,
#endif
    Search,
    SendSearchCmd,
    GetPrinterMac,
    ConnectDevice,
    MainWrite,
    Get_Cur_Role,
};

/*******************************************************************************
*函数功能：
*   @ 初始化Bt模块硬件，只支持设备，不支持ap
*参数描述：
*   @ 初始化参数
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
const DevBti412Tdf *Bti412Init(DevBti412InitTdf* pstInit)
{
    memset(&s_stCtr, 0 , sizeof(s_stCtr));

    s_stCtr.byUartNO     = pstInit->byUartNO;
    s_stCtr.pwdBtStaBit0 = pstInit->pwdBtStaBit0;
    s_stCtr.pwdBtStaBit1 = pstInit->pwdBtStaBit1;
    s_stCtr.pwdBtUnlink  = pstInit->pwdBtUnlink;

    DevUart->Config(s_stCtr.byUartNO, pstInit->dwBaud, 1, 0, true);
    DevUart->RegHandler(pstInit->byUartNO, BTRxHandler);

    if(s_stCtr.pwdBtUnlink) *s_stCtr.pwdBtUnlink = 1;
    // Unlink(false);

    for(__IO int i = 0; i < 0x1fffff; i ++) {;}
    s_stCtr.blDevExsit = true;
  //  if(GetMac((char*)s_stCtr.byaMac)) s_stCtr.blDevExsit = false;
 //   if(GetName((char*)s_stCtr.byaName)) s_stCtr.blDevExsit = false;

    return &s_Dev;
}
#endif