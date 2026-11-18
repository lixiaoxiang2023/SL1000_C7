/*******************************************************************************
文件名称：   wifi.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   驱动平台
创建者：     XQSCU
创建日期：   20151123
程序用途：

版权声明： Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
20151123：
第一次由XQ创建初始程序。
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_WIFI)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define WIFI_RX_SIZE            1000
#define WIFI_TX_SIZE            50
#define WIFI_AP_NAME_LEN        20
#define WIFI_AP_MUN             6
#define WIFI_AP_PWD_LED         14
#define WIFI_JOIN_AP_BUFF       (WIFI_AP_NAME_LEN + WIFI_AP_PWD_LED + 8)

#define AT                      0
#define AT_SET_AP               1
#define AT_SET_STA              2
#define AT_SET_STA_DHCP_ON      3
#define AT_SET_ECHO_OFF         4
#define AT_SET_BAUD             5
#define AT_SET_STA_AUTO_CONN    6
#define AT_GET_AP               7
#define AT_GET_AP_LIST          8
#define AT_QUIT_AP              9
#define AT_JOIN_AP              10
#define AT_GET_IP               11

#define AT_TCP_GET_STATUS       12
#define AT_TCP_SET_UP           13
#define AT_TCP_SEND             14
#define AT_TCP_SET_DOWN         15
#define AT_TCP_NO_MUX           16

#define AT_JOIN_AP_PWD          17

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
内部全局变量定义
*******************************************************************************/
static const char* const AT_CMD[18] = {
    "AT\r\n",
    "AT+CWMODE=2\r\n",
    "AT+CWMODE=1\r\n",
    "AT+CWDHCP=1,1\r\n",

    "ATE0\r\n",
    
#if(CONFIG_WIFI_BAUD115200)
    "AT+CIOBAUD=115200,8,1,0,0\r\n",
#else
    "AT+CIOBAUD=9600,8,1,0,0\r\n",
#endif
    
    "AT+CWAUTOCONN=1\r\n",

    "AT+CWJAP?\r\n",                    // 7
    "AT+CWLAP\r\n",                     // 8
    "AT+CWQAP\r\n",                     // 9
    "AT+CWJAP=",                        // 10
    "AT+CIFSR\r\n",

    // TCP
    "AT+CIPSTATUS\r\n",                 // 12
    "AT+CIPSTART=\"TCP\",",
    "AT+CIPSEND=",
    "AT+CIPCLOSE\r\n",
    "AT+CIPMUX=0\r\n",

    " ",
};

static struct {
    unsigned char byUartNo;

    unsigned int *pdwPd;
    unsigned int *pdwRst;

    // 接收逻辑
    bool blIsEnd;
    bool blBusy;
    bool blCmdTxOk;
    unsigned int dwRxCnt;
    char byaRx[WIFI_RX_SIZE];
    char byaTx[WIFI_TX_SIZE];
    char byLastByte;

    // 网络状态
    bool blDevExist;

    // ap list
    unsigned char byApMun;
    unsigned char abyApConn[WIFI_AP_NAME_LEN];
    unsigned char abyApList[WIFI_AP_MUN][WIFI_AP_NAME_LEN];
    char acJoin[WIFI_JOIN_AP_BUFF];

    struct {
        char cStatus;
        char id;
        char type;
        char remote_ip;
        char remote_port;

        char local_port;
        char tetype;
    }stTcp;

    void (*pfHandler)(unsigned char );
}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
*函数功能：
*   @ 用户接口调试
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void DebugTxHandler(unsigned char byIn)
{
    DevUart->SendByte(0, byIn);
    DevUart->SendByte(s_stCtr.byUartNo, byIn);
}

/*******************************************************************************
*函数功能：
*   @ 数据接收处理接口
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void DebugProHandler(unsigned char byIn)
{
    DevUart->SendByte( 0, byIn);
}

/*******************************************************************************
*函数功能：
*   @ 数据接收处理接口
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void RxProHandler(unsigned char byIn)
{
    unsigned char byLast1 = 0;
    unsigned char byLast2 = 0;

    //先过滤"OK+"命令
    if(s_stCtr.dwRxCnt) byLast1 = s_stCtr.byaRx[s_stCtr.dwRxCnt - 1];
    if(s_stCtr.dwRxCnt > 1) byLast2 = s_stCtr.byaRx[s_stCtr.dwRxCnt - 2];
    if(s_stCtr.dwRxCnt < WIFI_RX_SIZE - 1) {
        s_stCtr.byaRx[s_stCtr.dwRxCnt] = byIn;
        s_stCtr.dwRxCnt ++;
    }

    // 判断结束
    if((byLast1 == 'O' && byIn == 'K') || (byLast1 == 0x0d && byIn == 'F')) {
        s_stCtr.blCmdTxOk = true;
        s_stCtr.blIsEnd = true;
    }
    else if(byLast1 == 'R' && byLast2 == 'O' && byIn == 'K') {
        s_stCtr.blCmdTxOk = false;
        s_stCtr.blIsEnd = true;
    }

    if(s_stCtr.blIsEnd) {
        if(byLast1 == 0x0d && byIn == 0x0a) s_stCtr.blBusy = false;
    }

    s_stCtr.byLastByte = byIn;
    // debug
    if(s_stCtr.pfHandler) s_stCtr.pfHandler(byIn);
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
static void StartRx(void)
{
    memset(s_stCtr.byaRx, 0, WIFI_RX_SIZE);
    s_stCtr.blCmdTxOk = false;
    s_stCtr.dwRxCnt = 0;
    s_stCtr.byLastByte = 0;
    s_stCtr.blIsEnd = false;
    s_stCtr.blBusy = true;
}

/*******************************************************************************
*函数功能：
*   @ 等待命名结束
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int Wait(unsigned int dwTimOut)
{
   // static int i = 0;
    while(s_stCtr.blBusy) {
        if(!dwTimOut --) break;
    }

    s_stCtr.blBusy = false;
    if(dwTimOut == 0) return -1;
    if(!s_stCtr.blCmdTxOk) return -1;
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 发送控制命令
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int WrCmd(unsigned char byCmd, unsigned int dwTimOut)
{
    const char *pcCmd = NULL;
    unsigned char byLen = 0;

    if(s_stCtr.blBusy) return -1;

    StartRx();

    if(byCmd == AT_JOIN_AP_PWD) pcCmd = (const char*)s_stCtr.acJoin;
    else pcCmd = AT_CMD[byCmd];

    byLen = strlen(pcCmd);
    DevUart->SendString(s_stCtr.byUartNo, (const unsigned char*)pcCmd, byLen);

    if(!dwTimOut) {
        s_stCtr.blBusy = false;
        return 0;
    }

    return Wait(dwTimOut);
}

/*******************************************************************************
*函数功能：
*   @ 检查tcp连接状态
*参数描述：
*   @ 无
*返回值描述：
*   @ true :  存在网卡，工作正常
*   @ false : 网卡不存在，或者工作不正常
*说明：
*   @ 无
*******************************************************************************/
static int GetTcpStatus(void)
{
    char *pcGet = NULL;

    if(!s_stCtr.blDevExist) return -1;
    if(WrCmd(AT_TCP_GET_STATUS, 0x1fffff)) return -1;

    /*
    STATUS:3
    +CIPSTATUS:0,"TCP","192.168.1.81",8080,0
    */

    pcGet = (char*)s_stCtr.byaRx;
    for(int i = 0 ; *pcGet != 0 && *pcGet != ':' && i < WIFI_RX_SIZE; pcGet ++, i ++) {;}
    if( *pcGet != ':') return -1;

    pcGet ++;
    s_stCtr.stTcp.cStatus = *pcGet;

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 检查tcp连接状态
*参数描述：
*   @ 无
*返回值描述：
*   @ true :  存在网卡，工作正常
*   @ false : 网卡不存在，或者工作不正常
*说明：
*   @ 无
*******************************************************************************/
static int TcpLink(unsigned char *pbyIp, unsigned short int wPort)
{
    const char * const C_SEND = {"\"%d.%d.%d.%d\",%d\r\n"};
    unsigned int dwTimOut = 0xffffff;
    unsigned int dwLenStr = 0;

    if(!s_stCtr.blDevExist) return -1;

    memset(s_stCtr.byaTx, 0, WIFI_TX_SIZE);
    sprintf(s_stCtr.byaTx, C_SEND, pbyIp[0], pbyIp[1], pbyIp[2], pbyIp[3], wPort);

    dwLenStr = strlen((const char*) s_stCtr.byaTx);
    WrCmd(AT_TCP_SET_UP, 0);

    StartRx();
    DevUart->SendString(s_stCtr.byUartNo, (const unsigned char*)s_stCtr.byaTx, dwLenStr);

    // 等待发送数据完成
    return Wait(dwTimOut);
}

/*******************************************************************************
*函数功能：
*   @ tcp发送数据
*参数描述：
*   @ 无
*返回值描述：
*   @ unsigned char *pbyIp 发送到的ip
*   @ unsigned short int wPort 使用的端口
*   @ const unsigned char *pbyIn 需要发送的数据
*   @ unsigned int dwLen 数据长度
*说明：
*   @ 无
*******************************************************************************/
static int TcpSend(unsigned char *pbyIp, unsigned short int wPort, const unsigned char *pbyIn, unsigned int dwLen)
{
  //  static int i =0;
    unsigned int dwTimOut = 0xffffff;
    unsigned int dwLenStr = 0;

    if(!s_stCtr.blDevExist) return -1;
    if(GetTcpStatus()) return -1;

    if(s_stCtr.stTcp.cStatus != '3') {
        if(TcpLink(pbyIp, wPort)) return -1;
        if(GetTcpStatus()) return -1;

        if(s_stCtr.stTcp.cStatus != '3') return -1;
    }

    memset(s_stCtr.byaTx, 0, WIFI_TX_SIZE);
    sprintf(s_stCtr.byaTx,"%d\r\n", dwLen);

    dwLenStr = strlen((const char*) s_stCtr.byaTx);
    WrCmd(AT_TCP_SEND, 0);
    DevUart->SendString(s_stCtr.byUartNo, (const unsigned char*)s_stCtr.byaTx, dwLenStr);

    while(dwTimOut --) {
        if(s_stCtr.byLastByte == '>') break;
    };

    s_stCtr.blBusy = true;
    s_stCtr.blIsEnd = false;
    DevUart->SendString(s_stCtr.byUartNo, (const unsigned char*)pbyIn, dwLen);

    // 等待发送数据完成
    dwTimOut = 0x5ffffff;
    return Wait(dwTimOut);
}

/*******************************************************************************
*函数功能：
*   @ 列出当前连接的ap
*参数描述：
*   @ 无
*返回值描述：
*   @ true :  存在网卡，工作正常
*   @ false : 网卡不存在，或者工作不正常
*说明：
*   @ 无
*******************************************************************************/
static int GetIp(char *pcIp, char *pcMac)
{
    char *pcTo = NULL;
    char *pcGet = NULL;

    if(!s_stCtr.blDevExist) return -1;
    WrCmd(AT_GET_IP, 0x1fffff);

    pcGet = (char*)s_stCtr.byaRx;
    for(int ap = 0; ap < 2; ap ++) {
        if(ap == 0) pcTo = pcIp;
        else pcTo = pcMac;

        for(int i = 0 ; *pcGet != 0 && *pcGet != '"' && i < WIFI_RX_SIZE; pcGet ++, i ++) {;}

        if(*pcGet != '"') break;
        pcGet ++;

        for(int get = 0; *pcGet != 0 && *pcGet != '"' && get < 18 ; get ++) {
            if(!*pcGet) return 0;
            if(pcTo) *pcTo ++ = *pcGet ++;
        }

        // 找下一行
        for(int i = 0 ; *pcGet != 0 && *pcGet != 0x0d && i < WIFI_RX_SIZE ; pcGet ++, i ++) {;}
        if(!*pcGet) break;
    }

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 断开ap
*参数描述：
*   @ 无
*返回值描述：
*   @ unsigned int dwAp ap序号
*   @ char *pcSsid ssid输出
*说明：
*   @ 无
*******************************************************************************/
static int QuitAp(void)
{
    int iRet = 0;

    iRet = WrCmd(AT_QUIT_AP, 0x2fffff);

    for(__IO int i = 0; i < 0xafffff; i++) { ; }
    return iRet;
}

/*******************************************************************************
*函数功能：
*   @ 连接ap
*参数描述：
*   @ 无
*返回值描述：
*   @ unsigned int dwAp ap序号
*   @ char *pcSsid ssid输出
*说明：
*   @ 无
*******************************************************************************/
static int JoinAp(unsigned int dwAp, char *pcPwd)
{
    char *pcGet = NULL;

    if(!pcPwd) return -1;
    if(dwAp > s_stCtr.byApMun) return -1;
    if(!s_stCtr.abyApList[dwAp][0]) return -1;

    memset(s_stCtr.acJoin, 0, WIFI_JOIN_AP_BUFF);
    pcGet = s_stCtr.acJoin;

    // ssid
    *pcGet ++ = '"';
    for(int i = 0; i < WIFI_AP_NAME_LEN && s_stCtr.abyApList[dwAp][i] != 0; i ++) {
        *pcGet ++ = s_stCtr.abyApList[dwAp][i];
    }
    *pcGet ++ = '"';
    *pcGet ++ = ',';

    //pwd
    *pcGet ++ = '"';
    for(int i = 0; i < WIFI_AP_PWD_LED && pcPwd[i] != 0; i ++) {
        *pcGet ++ = pcPwd[i];
    }
    *pcGet ++ = '"';
    *pcGet ++ = 0x0d; *pcGet ++ = 0x0a;

    QuitAp();
    WrCmd(AT_JOIN_AP, 0);

    if(WrCmd(AT_JOIN_AP_PWD, 0xaffffff)) return -1;

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 返回需要ap ssid
*参数描述：
*   @ 无
*返回值描述：
*   @ unsigned int dwAp ap序号
*   @ char *pcSsid ssid输出
*说明：
*   @ 无
*******************************************************************************/
static int GetApSsid(unsigned int dwAp, char *pcSsid)
{
    if(!pcSsid) return -1;
    if(dwAp > s_stCtr.byApMun) return -1;
    if(!s_stCtr.abyApList[dwAp][0]) return -1;

    for(int i = 0; i < WIFI_AP_NAME_LEN && s_stCtr.abyApList[dwAp][i] != 0; i ++) {
        *pcSsid ++ = s_stCtr.abyApList[dwAp][i];
    }

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 列出当前连接的ap
*参数描述：
*   @ 无
*返回值描述：
*   @ true :  存在网卡，工作正常
*   @ false : 网卡不存在，或者工作不正常
*说明：
*   @ 无
*******************************************************************************/
static int ListApConn(char *pbyList, unsigned int dwLenLimt)
{
    char *pcGet;
    int i;

    memset(s_stCtr.abyApConn, 0, WIFI_AP_NAME_LEN);
    if(!s_stCtr.blDevExist) return -1;

    WrCmd(AT_GET_AP, 0x1fffff);

    pcGet = (char*)s_stCtr.byaRx;
    for(i = 0;*pcGet != '"' && i < WIFI_RX_SIZE; pcGet ++, i ++) {
        if(!*pcGet) return 0;
    }

    if(*pcGet != '"') return -1;
    pcGet ++;

    memset(s_stCtr.abyApConn, 0, WIFI_AP_NAME_LEN);
    for(i = 0; i < dwLenLimt && *pcGet != '"'; i ++) {
        if(!*pcGet) return 0;
        if(pbyList) *pbyList ++ = *pcGet;
        s_stCtr.abyApConn[i] = *pcGet;
        pcGet ++;
    }

    return i ;
}

/*******************************************************************************
*函数功能：
*   @ 列出当前可用的ap
*参数描述：
*   @ 无
*返回值描述：
*   @ true :  存在网卡，工作正常
*   @ false : 网卡不存在，或者工作不正常
*说明：
*   @ 无
*******************************************************************************/
static int ListAp(char *pbyList, unsigned int dwLenLimt)
{
    char *pcGet;
    unsigned int ap = 0;
    unsigned char byApNameLen = 0;

    s_stCtr.byApMun = 0;
    memset(s_stCtr.abyApList, 0, (WIFI_AP_NAME_LEN * WIFI_AP_MUN));

    if(!s_stCtr.blDevExist) return -1;
    if(WrCmd(AT_GET_AP_LIST, 0xfffffff)) return -1;

    s_stCtr.byApMun = 0;
    memset(s_stCtr.abyApList, 0, (WIFI_AP_NAME_LEN*WIFI_AP_MUN));
    pcGet = (char*)s_stCtr.byaRx;
    for(ap = 0; ap < WIFI_AP_MUN; ap ++) {
        for(int i = 0 ; *pcGet != 0 && *pcGet != '"' && i < WIFI_RX_SIZE; pcGet ++, i ++) {;}

        if(*pcGet != '"') break;
        pcGet ++;

        for(int get = 0; *pcGet != 0 && *pcGet != '"' && (get < (WIFI_AP_NAME_LEN - 1)) ; get ++) {
            if(!*pcGet) continue;
            s_stCtr.abyApList[ap][get] = *pcGet ++;
        }

        // 找下一行
        for(int i = 0 ; *pcGet != 0 && *pcGet != 0x0d && i < WIFI_RX_SIZE ; pcGet ++, i ++) {;}

        if(!*pcGet) break;
    }

    s_stCtr.byApMun = ap;
    if(!pbyList) return 0;
    if(!ap) return -1;

    /*
    //先拷贝匹配
    for(int i = 0; i < ap ; i++) {
        byApNameLen = strlen((const char*)s_stCtr.abyApList[i]);

        if(!strcmp((const char*)s_stCtr.abyApList[i], (const char*)s_stCtr.abyApConn)) {
            *pbyList ++ = '*';
        }
        else {
            continue;
        }

        //sprintf(pbyList, "%02d ", (i + 1));
        //pbyList += strlen((const char*)pbyList);

        memcpy(pbyList, s_stCtr.abyApList[i], byApNameLen);
        pbyList += strlen((const char*)pbyList);
        *pbyList ++ = 0x0d; *pbyList ++ = 0x0a;

        break;
    }
    */

    for(int i = 0; i < ap ; i++) {
        byApNameLen = strlen((const char*)s_stCtr.abyApList[i]);

        if(!strcmp((const char*)s_stCtr.abyApList[i], (const char*)s_stCtr.abyApConn)) {
            *pbyList ++ = '*';
            //continue;
        }
        else {
            *pbyList ++ = ' ';
        }

        //sprintf(pbyList, "%02d ", (i + 1));
        //pbyList += strlen((const char*)pbyList);

        memcpy(pbyList, s_stCtr.abyApList[i], byApNameLen);
        pbyList += strlen((const char*)pbyList);
        *pbyList ++ = 0x0d; *pbyList ++ = 0x0a;
    }

    return ap;
}

/*******************************************************************************
*函数功能：
*   @ 取得网卡是否存在
*参数描述：
*   @ 无
*返回值描述：
*   @ true :  存在网卡，工作正常
*   @ false : 网卡不存在，或者工作不正常
*说明：
*   @ 无
*******************************************************************************/
static bool IsExist(void)
{
    return s_stCtr.blDevExist ;
}

/*******************************************************************************
*函数功能：
*   @ 硬件初始化
*参数描述：
*   @ 无
*返回值描述：
*   @  0 成功
*   @ -1 失败
*说明：
*   @ 无
*******************************************************************************/
static int HwInit(bool blInitHw)
{
#if(CONFIG_WIFI_BAUD115200)
    const unsigned int dwBaud1st = 9600;
    const unsigned int dwBaud2st = 115200;
#else
    const unsigned int dwBaud1st = 115200;
    const unsigned int dwBaud2st = 9600;
#endif
    
    // debug
    if(blInitHw) {
        DevUart->Config(s_stCtr.byUartNo, dwBaud1st, 1, 0, true);
        WrCmd(AT_SET_BAUD, 0);
        for(int i = 0; i < 0x1fffff; i++){ ; }

        DevUart->Config(s_stCtr.byUartNo, dwBaud2st, 1, 0, true);
        for(int i = 0; i < 0x1fffff; i++) { ; }
        WrCmd(AT_SET_BAUD, 0);
    }

    // 设定默认参数
    s_stCtr.blDevExist = false;
    WrCmd(AT, 0x1fffff);
    for(int i = 0; i < 0x1fffff; i++){ ; }
    if(WrCmd(AT_SET_ECHO_OFF, 0x1fffff)) return -1;
    for(int i = 0; i < 0x1fffff; i++){ ; }
    if(WrCmd(AT_SET_BAUD, 0x1fffff)) return -1;
    for(int i = 0; i < 0x1fffff; i++){ ; }
    if(WrCmd(AT_SET_STA, 0x1fffff)) return -1;
    for(int i = 0; i < 0x1fffff; i++){ ; }
    if(WrCmd(AT_SET_STA_DHCP_ON, 0x1fffff)) return -1;
    for(int i = 0; i < 0x1fffff; i++){ ; }
    if(WrCmd(AT_SET_STA_AUTO_CONN, 0x1fffff)) return -1;
    for(int i = 0; i < 0x1fffff; i++){ ; }
    if(WrCmd(AT_TCP_NO_MUX, 0x1fffff)) return -1;

    s_stCtr.blDevExist = true;
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 模块硬件复位, 並將波特率設定為9600.
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int RestHw(void)
{
    // debug
    for(int i = 0; i < 0x1fffff; i++){ ; }
    *s_stCtr.pdwRst = 0;
    for(int i = 0; i < 0x1fffff; i++){ ; }
    *s_stCtr.pdwRst = 1;
    for(int i = 0; i < 0x1ffffff; i++){ ; }

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 打开模块
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int PowerUp(void)
{
    *s_stCtr.pdwPd = 1;

    RestHw();
#if(CONFIG_WIFI_BAUD115200)
    DevUart->Config(s_stCtr.byUartNo, 115200, 1, 0, true);
#else
    DevUart->Config(s_stCtr.byUartNo, 9600, 1, 0, true);
#endif
    for(int i = 0; i < 0x1ffffff; i++){ ; }
    DevUart->RegHandler(s_stCtr.byUartNo, RxProHandler);

    s_stCtr.blDevExist = false;
    WrCmd(AT, 0x1fffff);

    if(WrCmd(AT_SET_ECHO_OFF, 0x1fffff)) return -1;

    s_stCtr.blDevExist = true;
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 关闭模块
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
void PowerOff(void)
{
    *s_stCtr.pdwPd = 0;
#if(CONFIG_WIFI_BAUD115200)
    DevUart->Config(s_stCtr.byUartNo, 115200, 1, 0, true);
#else
    DevUart->Config(s_stCtr.byUartNo, 9600, 1, 0, true);
#endif    
    s_stCtr.blDevExist = false;
}

/*******************************************************************************
*函数功能：
*   @ 调试功能开关
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void Debug(bool blIsOn)
{
    if(blIsOn) {
        DevUart->Config(0, 115200, 1, 0, true);
        s_stCtr.pfHandler = DebugProHandler;
        DevUart->RegHandler(0, DebugTxHandler);
    }
    else {
        s_stCtr.pfHandler = NULL;
        DevUart->RegHandler(0, NULL);
    }
}

static const DevWifiTdf s_Dev = {
    PowerUp,
    PowerOff,

    IsExist,
    HwInit,
    ListApConn,
    ListAp,

    GetApSsid,
    JoinAp,
    QuitAp,
    GetIp,

    TcpSend,
    TcpLink,
    GetTcpStatus,

    Debug,
};

/*******************************************************************************
*函数功能：
*   @ 初始化wifi模块硬件，只支持设备，不支持ap
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
const DevWifiTdf *WifiInit(WifiInitTdf* pstInit)
{
    memset(&s_stCtr, 0 , sizeof(s_stCtr));

    s_stCtr.byUartNo = pstInit->byUartNo;
    s_stCtr.pdwPd = pstInit->pdwPd;
    s_stCtr.pdwRst = pstInit->pdwRst;

    *s_stCtr.pdwPd = 0;
    return &s_Dev;
}

#endif