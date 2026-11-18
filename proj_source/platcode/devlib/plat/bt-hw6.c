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

#if(CONFIG_BT_HW6)
/*******************************************************************************
常量定义
*******************************************************************************/
#define AT                      0
#define AT_SET_NAME             1
#define AT_SET_DEFAULT          2
#define AT_SET_ROLE_M           3
#define AT_SET_ROLE_S           4
#define AT_SET_PIN              5
#define AT_SET_CLEAR            6
#define AT_SET_SEARCH           7
#define AT_SET_LADD             8
#define AT_SET_NOTI_OFF         9
#define AT_SET_NOTI_ON         10
#define AT_SET_BAUD1200        11
#define AT_SET_BAUD2400        12
#define AT_SET_BAUD4800        13
#define AT_SET_BAUD9600        14
#define AT_SET_BAUD19200       15
#define AT_SET_BAUD38400       16
#define AT_SET_BAUD57600       17
#define AT_SET_BAUD115200      18

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static const unsigned char* const AT_CMD[19][2] = {
    {"AT",          "OK"            },
    {"AT+NAME",     "OK+Set:"       },
    {"AT+DEFAULT",  "OK+DEFAULT"    },
    {"AT+ROLEM",    "OK+Set:M"      },
    {"AT+ROLES",    "OK+Set:S"      },

//    {"AT+PINE",     "OK+Set:"       },
    {"AT+PIN",      "OK+Set:"       },

    {"AT+CLEAR",    "OK+CLEAR"      },
    {"AT+SEARCH",   "OK"            },

//    {"AT+ADDR?",    "OK+LADD:"      },
    {"AT+LADD?",    "OK+LADD:"      },

//    {"AT+NOTI0",    "OK+NOTI:0"     },
//    {"AT+NOTI1",    "OK+NOTI:1"     },

    {"AT+NOTI0",    "OK+Set:0"      },
    {"AT+NOTI1",    "OK+Set:1"      },

    {"AT+BAUD1",    "OK+Set:1200"   },
    {"AT+BAUD2",    "OK+Set:2400"   },
    {"AT+BAUD3",    "OK+Set:4800"   },
    {"AT+BAUD4",    "OK+Set:9600"   },
    {"AT+BAUD5",    "OK+Set:19200"  },
    {"AT+BAUD6",    "OK+Set:38400"  },
    {"AT+BAUD7",    "OK+Set:57600"  },
    {"AT+BAUD8",    "OK+Set:115200" },

//    {"AT+BAUD1",    "OK+Set:1"      },
//    {"AT+BAUD2",    "OK+Set:2"      },
//    {"AT+BAUD3",    "OK+Set:3"      },
//    {"AT+BAUD4",    "OK+Set:4"      },
//    {"AT+BAUD5",    "OK+Set:5"      },
//    {"AT+BAUD6",    "OK+Set:6"      },
//    {"AT+BAUD7",    "OK+Set:7"      },
//    {"AT+BAUD8",    "OK+Set:8"      },
};

static struct {
#if(CONFIG_BT_DEBUG)
    bool blDebug;
    unsigned char byDebugUart;
#endif

    unsigned char byUartNO;
    unsigned int  *dwRestPinAddr;

    // 连接状态
    bool blIsConn;
    bool blDevExsit;

    // 接收逻辑
    bool blBusy;
    unsigned char byRxCnt;
    char byaRx[CONFIG_BT_RX_BUFF_SIZE];
    char byaCmd[CONFIG_BT_RX_BUFF_SIZE];
    char byaLost[4];

    void (*Notice) (unsigned char );
}s_stCtr;

//蓝牙指令
const  unsigned char c_byBTAT[]            = {"AT"};
const  unsigned char c_byBTATSearch[]      = {"AT+CONLAST"};
const  unsigned char c_byBTInquireRole[]   = {"AT+ROLE?"};
const  unsigned char c_byBTSetRole_M[]     = {"AT+ROLEM"};
const  unsigned char c_byBTSetRole_S[]     = {"AT+ROLES"};
const  unsigned char c_byBTNoti[]          = {"AT+NOTI1"};
const  unsigned char c_byBTClear[]         = {"AT+CLEAR"};
const  unsigned char c_byBTGetName[]       = {"AT+NAME?"};    // 蓝牙名称查询
const  unsigned char c_byBTInquireAddr[]   = {"AT+ADDR?"};    // 蓝牙地址查询
const  unsigned char c_byBTBAUD[]          = {"AT+BAUD2"};    // 设置蓝牙波特率为9600

/*******************************************************************************
内部函数申明
*******************************************************************************/

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
    if(1) {
        // 检查蓝牙 数据连接
        s_stCtr.byaLost[0] = s_stCtr.byaLost[1];
        s_stCtr.byaLost[1] = s_stCtr.byaLost[2];
        s_stCtr.byaLost[2] = s_stCtr.byaLost[3];
        s_stCtr.byaLost[3] = byIn;

        if( !strcmp("LOST", s_stCtr.byaLost)) s_stCtr.blIsConn = false;
        if( !strcmp("CONN", s_stCtr.byaLost)) s_stCtr.blIsConn = true;
        if( !strcmp("CONT", s_stCtr.byaLost)) s_stCtr.blIsConn = true;
    }

    // 先过滤"OK+"命令
    if(s_stCtr.blIsConn) {
        if(s_stCtr.Notice) s_stCtr.Notice(byIn);
        return;
    }

    if(s_stCtr.byRxCnt < CONFIG_BT_RX_BUFF_SIZE) {
        s_stCtr.byaRx[s_stCtr.byRxCnt] = byIn;
        s_stCtr.byRxCnt ++;
    }

#if(CONFIG_BT_DEBUG)
    if(s_stCtr.blDebug) {
        DevUart->SendByte(s_stCtr.byDebugUart, byIn);
    }
#endif
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
*   @ 等待命名结束
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
        if(!strcmp(s_stCtr.byaCmd, s_stCtr.byaRx)) {
            blOK = true;
            break;
        }
    }

    s_stCtr.blBusy = false;
    return (blOK)?0:-1;
}

/*******************************************************************************
*函数功能：
*   @ 发送控制命令
*参数描述：
*   @ unsigned char byCmd 需要发送的命令序号
*   @ unsigned int dwTimOut 等待数据返回时间
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
    unsigned char byLenMore = 0;

    if(s_stCtr.blBusy) return -1;

    StartRx();
    pcCmd = AT_CMD[byCmd][1];
    byLen = strlen((const char *)pcCmd);
    memcpy(s_stCtr.byaCmd, pcCmd, byLen);

    if(pbyMore) {
        byLenMore = strlen((const char *)pbyMore);
        memcpy(&s_stCtr.byaCmd[byLen], (const unsigned char*)pbyMore, byLenMore);
    }

    pcCmd = AT_CMD[byCmd][0];
    byLen = strlen((const char *)pcCmd);
    DevUart->SendString(s_stCtr.byUartNO, (const unsigned char*)pcCmd, byLen);
    if(pbyMore) {
        byLen = strlen((const char *)pbyMore);
        DevUart->SendString(s_stCtr.byUartNO, pbyMore, byLen);
    }

#if(CONFIG_BT_DEBUG)
    if(s_stCtr.blDebug) {
        DevUart->SendString(s_stCtr.byDebugUart, (const unsigned char*)pcCmd, byLen);
        DevUart->SendString(s_stCtr.byDebugUart, "\r\n", byLen);
    }
#endif

    return Wait(byCmd, dwTimOut);
}

#if(CONFIG_BT_SET_ROLE)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙主从模式
*参数描述：
*   @ bool blIsMaster: true 主模式 false 从模式
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static int SetRole(bool blIsMaster)
{
    if(blIsMaster == true) {
        if( WrCmd(AT_SET_ROLE_M , 0x7fffff, NULL) ) return -1;
    }
    else {
        if( WrCmd(AT_SET_ROLE_S , 0x7fffff, NULL) ) return -1;
    }

    return 0;
}
#endif

#if(CONFIG_BT_SET_PIN)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙主从模式
*参数描述：
*   @ bool blIsMaster: true 主模式 false 从模式
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static int SetPin(const unsigned char *pbyPin)
{
    unsigned int dwLen;

    if(!pbyPin) return -1;

    dwLen = strlen((const char *)pbyPin);
    if(dwLen > 10) return -1;

    if( WrCmd(AT_SET_PIN , 0x7fffff, pbyPin) ) return -1;
    return 0;
}
#endif

#if(CONFIG_BT_CLEAR)
/*******************************************************************************
*函数功能：
*   @ 清除配置信息
*参数描述：
*   @ 无
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 主模式使用
*******************************************************************************/
static int Clear(void)
{
    return WrCmd(AT_SET_CLEAR , 0x7fffff, NULL);
}
#endif

#if(CONFIG_BT_SEARCH)
/*******************************************************************************
*函数功能：
*   @ 主模式扫描设备
*参数描述：
*   @ 无
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 主模式使用
*******************************************************************************/
static int Search(void)
{
    return WrCmd(AT_SET_SEARCH , 0x7fffff, NULL);
}
#endif

#if(CONFIG_BT_GET_MAC)
/*******************************************************************************
*函数功能：
*   @ 主模式扫描设备
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
    char *pbyGet = s_stCtr.byaRx;

    if(!pbyMac) return -1;
    WrCmd(AT_SET_LADD , 0x7fffff, NULL);

    for(__IO int delay = 0; delay < 0xfffff; delay ++) {;}
    for(int i = 0; i < CONFIG_BT_RX_BUFF_SIZE; i ++ ) {
        if(*pbyGet ++  == ':') break;
    }

    if(*pbyGet == 0) return -1;
    for( ; *pbyGet != 0 ; pbyGet ++) *pbyMac ++ = *pbyGet;

    return 0;
}
#endif

#if(CONFIG_BT_SET_BAUD)
/*******************************************************************************
*函数功能：
*   @ 设置波特率
*参数描述：
*   @ 波特率
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 设置波特率后需要调用 获取 波特率函数
*******************************************************************************/
static int SetBaud(unsigned int dwBaud)
{
    if(dwBaud == 1200 )         return WrCmd(AT_SET_BAUD1200 ,  0x7fffff, 0);
    else if(dwBaud == 2400 )    return WrCmd(AT_SET_BAUD2400 ,  0x7fffff, 0);
    else if(dwBaud == 4800 )    return WrCmd(AT_SET_BAUD4800 ,  0x7fffff, 0);
    else if(dwBaud == 9600 )    return WrCmd(AT_SET_BAUD9600 ,  0x7fffff, 0);
    else if(dwBaud == 19200 )   return WrCmd(AT_SET_BAUD19200 , 0x7fffff, 0);
    else if(dwBaud == 38400 )   return WrCmd(AT_SET_BAUD38400 , 0x7fffff, 0);
    else if(dwBaud == 57600 )   return WrCmd(AT_SET_BAUD57600 , 0x7fffff, 0);
    else return WrCmd(AT_SET_BAUD115200 , 0x7fffff, 0);
}
#endif

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
    DevUart->SendString(s_stCtr.byUartNO, pbyIn, dwLen);
    return 0;
}

#if(CONFIG_BT_UNLINK)
/*******************************************************************************
*函数功能：
*   @ 断开连接
*参数描述：
*   @ 无
*返回值描述：
*   @  0 成功
*   @ -1 失败
*说明：
*   @ 模块处于待机状态下，PIO0接地100MS, 断开所有连接
*******************************************************************************/
static void Unlink(void)
{
    if(!s_stCtr.dwRestPinAddr) return ;

    *s_stCtr.dwRestPinAddr = 1;
    for(volatile int i = 0; i < 0x7ffff; i ++){;}
    *s_stCtr.dwRestPinAddr = 0;
}
#endif

#if(CONFIG_BT_CHEAK_BAUD)
/*******************************************************************************
*函数功能：
*   @ 检查波特率
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static unsigned int CheakBaud(void)
{
    bool blFind = false;
    unsigned int dwBaud = 1200;

    for(int i = 0; i < 8; i ++) {
        Unlink();
        DevUart->Config(s_stCtr.byUartNO, dwBaud, 1, 0, true);
        for(int delay = 0 ; delay < 0xfffff; delay ++ ) {;}

        if( !WrCmd(AT, 0x7ffff, NULL)) {
            blFind = true;
            break;
        }

        dwBaud *= 2;
    }

    s_stCtr.blDevExsit = blFind;
    return (blFind)? dwBaud:0;
}
#endif

#if(CONFIG_BT_SET_NAME)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙设备是否通知状态
*参数描述：
*   @ bool blNoticeOn ：true 启动通知，false 关闭通知
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int SetName(const char *pcName)
{
    return WrCmd(AT_SET_NAME, 0x7fffff, (const unsigned char *)pcName);
}
#endif

#if(CONFIG_BT_NITOCE)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙设备是否通知状态
*参数描述：
*   @ bool blNoticeOn ：true 启动通知，false 关闭通知
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int SetNotice(bool blNoticeOn)
{
    if(blNoticeOn == true) return WrCmd(AT_SET_NOTI_ON , 0x7fffff, NULL);
    return WrCmd(AT_SET_NOTI_OFF , 0x7fffff, NULL);
}
#endif

#if(CONFIG_BT_SET_DEFAULT)
/*******************************************************************************
*函数功能：
*   @ 设置蓝牙设备恢复出厂
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 设置回复出厂后，需要等待500MS，后重新取得波特率。
*******************************************************************************/
static int SetDefault(void)
{
    return WrCmd(AT_SET_DEFAULT , 0x7fffff, NULL);
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
    return s_stCtr.blIsConn;
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

static const DevBtTdf s_Dev = {
    Write,
#if(CONFIG_BT_UNLINK)
    Unlink,
#endif
#if(CONFIG_BT_CHEAK_BAUD)
    CheakBaud,
#endif
#if(CONFIG_BT_SET_NAME)
    SetName,
#endif
#if(CONFIG_BT_SET_ROLE)
    SetRole,
#endif
#if(CONFIG_BT_SET_PIN)
    SetPin,
#endif
#if(CONFIG_BT_CLEAR)
    Clear,
#endif
#if(CONFIG_BT_SEARCH)
    Search,
#endif
#if(CONFIG_BT_GET_MAC)
    GetMac,
#endif
#if(CONFIG_BT_SET_BAUD)
    SetBaud,
#endif
#if(CONFIG_BT_NITOCE)
    SetNotice,
#endif
#if(CONFIG_BT_SET_DEFAULT)
    SetDefault,
#endif
#if(CONFIG_BT_SET_NITOCE_HANLDER)
    SetNoticehandler,
#endif
#if(CONFIG_BT_IS_CONNECTED)
    IsConnected,
#endif
#if(CONFIG_BT_IS_DEV_EXIST)
    IsDevExist,
#endif
#if(CONFIG_BT_DEBUG)
    Debug,
#endif
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
const DevBtTdf *BtInit(DevBtInitTdf* pstInit)
{
    memset(&s_stCtr, 0 , sizeof(s_stCtr));

    s_stCtr.byUartNO        = pstInit->byUartNO;
    s_stCtr.dwRestPinAddr   = pstInit->byPinRestAddr;

    if(s_stCtr.dwRestPinAddr) *s_stCtr.dwRestPinAddr  = 0;
    DevUart->RegHandler(pstInit->byUartNO, BTRxHandler);

    CheakBaud();
    return &s_Dev;
}
#endif