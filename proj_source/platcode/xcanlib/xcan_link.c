/*******************************************************************************
文件名称：   xcan_link.c
文件目录：   $PROJ_DIR$/../xcanlib
项目名称：   事业一部技术平台项目
创建者：     lixx
创建日期：   2019.2
程序用途：   用于xcan协议的设备自举协议，通过自动发送和解析，完成设备的管理

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "xcan.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if(PLAT_WIN32)
#pragma warning(disable:4996)
#endif

/*******************************************************************************
Constant Definition
*******************************************************************************/
#define FUNC_CLASS        0
#define FUNC_SUBCLASS     1
#define FUNC_NAME         2
#define FUNC_SN           3
#define FUNC_IOID         4
#define FUNC_MAC          5
#define FUNC_IP           6
#define FUNC_UDPPORT      7
#define FUNC_TCPPORT      8
#define FUNC_LINKIP       9
#define FUNC_LINKTCP     10

#define OP_RET			  0
#define OP_SCAN			  1
#define OP_DOLINK		  2
#define OP_DISLINK		  3

#define STA_NULL          0
#define STA_ERR           1
#define STA_LINKED        2
#define STA_UNLINK        3

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
外部变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct __stXcanLink {
    // 定义通信内容
	char  a2CTxt[11][32];

    unsigned char byaMac[6];
    unsigned int dwIP;
    unsigned int dwUdpPort;
    unsigned int dwTcpPort;

    unsigned int dwLinkIP;
    unsigned int dwLinkTcpPort;

    unsigned char byOp;
    unsigned char bySta;

} stXcanLinkTdf;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static struct {
    stXcanLinkTdf  stMyConfig;                                 // 本机配置

    struct {
        stXcanLinkTdf stRemoteDev;                             // 连接到的远程设备
        bool blLinked;                                        // 连接状态
    } stLinkedDevs[XCAN_LINK_SURPORT_NUM];

    struct {
        stXcanLinkTdf stRemoteDev[XCAN_LINK_SURPORT_NUM];       // 连接到的远程设备
        unsigned int dwDevCnt;                                // 连接计数
    } stScanDevs;

    struct {
        stXcanLinkTdf  stRx;                                    // 接收缓存
        unsigned char op;
        unsigned char status;
    } stRx;

    bool blGetIp;                                              // 是否已有IP，或者地址

    void  (*pfBroadcast) (unsigned int dwPort, const char *pcInfo, unsigned int dwLen);       // 用于发送广播信息
    void  (*pfUdp) (unsigned int dwIP, unsigned int dwPort, const char *pcInfo, unsigned int dwLen);    // 用于发送带端口号的信号，如UDP

    void  (*pfNotice) (const char *pcIn, unsigned int dwLen);   // 接收到远程返回通知
    void  (*pfLink) (unsigned int dwIP, unsigned int dwPort);   // 启动远程连接
    void  (*pfUnlink) (void);                                      // 关闭远程连接

    char abyTxBuff[240];
} stDev;

static char abyRxMutric[13][32];

static const char * const XlinkHead[] = {
    "class:",
    "subcalss:",
    "name:",
    "sn:",
    "ioid:",
    "mac:",

    "ip:",
    "udpport:",
    "tcpport:",

    "linkip:",
    "linktcp:",

    "op:ret",
    "op:scan",
    "op:link",
    "op:dislink",

    "status:null",
    "status:err",
    "status:linked",
    "status:unlink"
};

/*******************************************************************************
内部函数申明
*******************************************************************************/


/*******************************************************************************
函数定义
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 设置本机ip
参数描述：
*   @ unsigned int dwIP, 设备的ip
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int SetMyIp(unsigned int dwIP)
{
    unsigned char byaIP[4];

    if(dwIP == 0) {
        memset(stDev.stMyConfig.a2CTxt[FUNC_IP], 0, sizeof(stDev.stMyConfig.a2CTxt[FUNC_IP]));
        stDev.stMyConfig.dwIP = 0;

        return 0;
    }

    for (int i = 0; i < 4; i ++) byaIP[i] = dwIP >> (8 * i);
    sprintf( stDev.stMyConfig.a2CTxt[FUNC_IP], "%d.%d.%d.%d", byaIP[3], byaIP[2], byaIP[1], byaIP[0]);
    stDev.stMyConfig.dwIP = dwIP;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置udp端口
参数描述：
*   @ unsigned int dwIP, 设备的ip
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int SetMyUdpPort(unsigned int dwUdpPort)
{
    if(dwUdpPort == 0) {
        memset(stDev.stMyConfig.a2CTxt[FUNC_UDPPORT], 0, sizeof(stDev.stMyConfig.a2CTxt[FUNC_UDPPORT]));
        stDev.stMyConfig.dwUdpPort = 0;

        return 0;
    }

    sprintf( stDev.stMyConfig.a2CTxt[FUNC_UDPPORT], "%d", dwUdpPort);
    stDev.stMyConfig.dwUdpPort = dwUdpPort;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置tcp端口
参数描述：
*   @ unsigned int dwIP, 设备的ip
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int SetMyTcpPort(unsigned int dwTcpPort)
{
    if(dwTcpPort == 0) {
        memset(stDev.stMyConfig.a2CTxt[FUNC_TCPPORT], 0, sizeof(stDev.stMyConfig.a2CTxt[FUNC_TCPPORT]));
        stDev.stMyConfig.dwTcpPort = 0;

        return 0;
    }

    sprintf( stDev.stMyConfig.a2CTxt[FUNC_TCPPORT], "%d", dwTcpPort);
    stDev.stMyConfig.dwTcpPort = dwTcpPort;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 连接IP
参数描述：
*   @ unsigned int dwIP, 设备的ip
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int SetLinkIp(unsigned int dwIP)
{
    unsigned char byaIP[4];

    if(dwIP == 0) {
        memset(stDev.stMyConfig.a2CTxt[FUNC_LINKIP], 0, sizeof(stDev.stMyConfig.a2CTxt[FUNC_LINKIP]));
        stDev.stMyConfig.dwLinkIP = 0;

        return 0;
    }

    for (int i = 0; i < 4; i ++) byaIP[i] = dwIP >> (8 * i);
    sprintf( stDev.stMyConfig.a2CTxt[FUNC_LINKIP], "%d.%d.%d.%d", byaIP[0], byaIP[1], byaIP[2], byaIP[3]);
    stDev.stMyConfig.dwIP = dwIP;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置连接到远程的tcp端口
参数描述：
*   @ unsigned int dwIP, 设备的ip
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int SetLinkTcpPort(unsigned int dwTcpPort)
{
    if(dwTcpPort == 0) {
        memset(stDev.stMyConfig.a2CTxt[FUNC_LINKTCP], 0, sizeof(stDev.stMyConfig.a2CTxt[FUNC_LINKTCP]));
        stDev.stMyConfig.dwLinkTcpPort = 0;

        return 0;
    }

    sprintf( stDev.stMyConfig.a2CTxt[FUNC_LINKTCP], "%d", dwTcpPort);
    stDev.stMyConfig.dwLinkTcpPort = dwTcpPort;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置xcan自动扫描接口
参数描述：
*   @ const char *pcClass 设备类
*   @ const char *pcSubclass 设备子类
*   @ const char *pcName 名称
*   @ const unsigned char *pbyMac mac地址
*   @ unsigned int (*pfIp)(void) 获取IP的接口
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int ConfigDev(const char *pcClass, const char *pcSubclass, const char *pcName, const char *pbyMac, \
    const char *pcSn, const char *pcIoID)
{
    unsigned int dwLenClass = 0;
    unsigned int dwLenSubclass = 0;
    unsigned int dwLenName = 0;
    unsigned int dwLenIoID = 0;
	unsigned int dwLenMac = 0;
	unsigned int dwLenSn = 0;

    if(!pcClass) return -1;
    if(!pcSubclass) return -1;
    if(!pcName) return -1;
    if(!pbyMac) return -1;

    dwLenClass = strlen(pcClass);
    dwLenSubclass = strlen(pcSubclass);
    dwLenName = strlen(pcName);
    dwLenIoID = strlen(pcIoID);
	dwLenMac = strlen(pbyMac);
	dwLenSn = strlen(pcSn);

	if (dwLenClass > 7) return -1;
	if (dwLenSubclass > 7) return -1;
	if (dwLenName > 15) return -1;
	if (dwLenIoID > 15) return -1;
	if (dwLenMac > 19) return -1;
	if (dwLenSn > 15) return -1;

    memset(&stDev, 0, sizeof(stDev));
	memcpy(stDev.stMyConfig.a2CTxt[FUNC_CLASS], pcClass, dwLenClass);
	memcpy(stDev.stMyConfig.a2CTxt[FUNC_SUBCLASS], pcSubclass, dwLenSubclass);
	memcpy(stDev.stMyConfig.a2CTxt[FUNC_NAME], pcName, dwLenName);
	memcpy(stDev.stMyConfig.a2CTxt[FUNC_IOID], pcIoID, dwLenIoID);
	memcpy(stDev.stMyConfig.a2CTxt[FUNC_MAC], pbyMac, dwLenMac);
	memcpy(stDev.stMyConfig.a2CTxt[FUNC_SN], pcSn, dwLenSn);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 配置发送接口
参数描述：
*   @ void (*Broadcast)(const char *pcInfo), 用于发送广播信息
*   @ void (*Udp)(const char *pcInfo, unsigned int dwPort) 用于发送带端口号的信号，如UDP
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void SetSendPort(void (*pfBroadcast)(unsigned int dwPort, const char *pcInfo, unsigned int dwLen), \
    void (*pfUdp) (unsigned int dwIP, unsigned int dwPort, const char *pcInfo, unsigned int dwLen))
{
    stDev.pfBroadcast = pfBroadcast;
    stDev.pfUdp = pfUdp;
}

/*******************************************************************************
函数功能：
*   @ 配置发送接口
参数描述：
*   @ void (*Broadcast)(const char *pcInfo), 用于发送广播信息
*   @ void (*Udp)(const char *pcInfo, unsigned int dwPort) 用于发送带端口号的信号，如UDP
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void SetEventPort( void (*pfNotice) (const char *pcIn, unsigned int dwLen), \
    void  (*pfLink) (unsigned int dwIP, unsigned int dwPort),\
    void  (*pfUnlink) (void))
{
    stDev.pfLink = pfLink;
    stDev.pfUnlink = pfUnlink;
    stDev.pfNotice = pfNotice;
}

/*******************************************************************************
函数功能：
*   @ 填充数据头
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static char* Head(char *pcIn)
{
    unsigned int dwLen = 0;
    char *cCp = pcIn;
    const char *cS;

    // class subclass name sn ip mac port
    for(int i = 0; i < 11; i ++) {
        cS = XlinkHead[i];
        dwLen = strlen((const char *)cS);
        memcpy(cCp, cS, dwLen);
        cCp += dwLen;

        cS = stDev.stMyConfig.a2CTxt[i];
        if(cS[0] == 0) {
            *cCp ++ = '\r'; *cCp ++ = '\n';
            continue;
        }

        dwLen = strlen((const char *)cS);
        memcpy(cCp, cS, dwLen);
        cCp += dwLen;

        *cCp ++ = '\r'; *cCp ++ = '\n';
    }

    if (stDev.stMyConfig.byOp > 3) stDev.stMyConfig.byOp = 3;
    if (stDev.stMyConfig.bySta > 3) stDev.stMyConfig.bySta = 3;

    cS = XlinkHead[11 + stDev.stMyConfig.byOp];
    dwLen = strlen((const char *)cS);
    memcpy(cCp, cS, dwLen);
    cCp += dwLen;
	*cCp++ = '\r'; *cCp++ = '\n';

    cS = XlinkHead[15 + stDev.stMyConfig.byOp];
    dwLen = strlen((const char *)cS);
    memcpy(cCp, cS, dwLen);
    cCp += dwLen;
	*cCp++ = '\r'; *cCp++ = '\n';

    return cCp;
}

/*******************************************************************************
函数功能：
*   @ 按照协议扫描
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int Scan(void)
{
    unsigned int dwLen = 0;
    char *cCp = (char *)stDev.abyTxBuff;

    stDev.stMyConfig.byOp = OP_SCAN;
    stDev.stMyConfig.bySta = STA_NULL;

    memset(stDev.abyTxBuff, 0, 128);
    Head(cCp);
    stDev.stScanDevs.dwDevCnt = 0;

    dwLen = strlen((const char *)stDev.abyTxBuff);
    if(stDev.pfBroadcast) stDev.pfBroadcast(stDev.stMyConfig.dwUdpPort, (const char *)stDev.abyTxBuff, dwLen);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 按照协议扫描
参数描述：
*   @ unsigned int dwNO 扫描发现远程的序列号
*   @ unsigned int dwTcpPort 需要使用tcp端口
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int Up(unsigned int dwSn)
{
    stXcanLinkTdf *pDev;
    unsigned int dwLen = 0;
    char *cCp = (char *)stDev.abyTxBuff;

    if (dwSn >= stDev.stScanDevs.dwDevCnt) return -1;

    pDev = &stDev.stScanDevs.stRemoteDev[dwSn];
    stDev.stMyConfig.byOp = OP_DOLINK;
    stDev.stMyConfig.bySta = STA_NULL;

    memset(stDev.abyTxBuff, 0, 128);
    Head(cCp);

    dwLen = strlen((const char *)stDev.abyTxBuff);
    if(stDev.pfUdp) stDev.pfUdp( pDev->dwIP, pDev->dwUdpPort, (const char *)stDev.abyTxBuff, dwLen);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 按照协议扫描
参数描述：
*   @ unsigned int dwNO 扫描发现远程的序列号
*   @ unsigned int dwTcpPort 需要使用tcp端口
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
int Down(unsigned int dwSn)
{
    stXcanLinkTdf *pDev;
    unsigned int dwLen = 0;
    char *cCp = (char *)stDev.abyTxBuff;

    if (dwSn >= stDev.stScanDevs.dwDevCnt) return -1;

    pDev = &stDev.stScanDevs.stRemoteDev[dwSn];
    stDev.stMyConfig.byOp = OP_DISLINK;
    stDev.stMyConfig.bySta = STA_NULL;

    memset(stDev.abyTxBuff, 0, 128);
    Head(cCp);

    dwLen = strlen((const char *)stDev.abyTxBuff);
    if(stDev.pfUdp) stDev.pfUdp( pDev->dwIP, pDev->dwUdpPort, (const char *)stDev.abyTxBuff, dwLen);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ str转ip
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int str2ip(const char *ip)
{
	char str_ip_index[4]={'\0'};
	unsigned int ip_int, ip_add = 0;
	int j = 0, a = 3;

	for(unsigned int i = 0;i <= strlen(ip); i ++) {
		if (ip[i] == '\0'|| ip[i] == '.') {
			ip_int = atoi(str_ip_index);
			if (ip_int > 255) return 0;

			ip_add += (ip_int * ((unsigned int) pow(256.0, a)));

			a --;
			memset(str_ip_index,0,sizeof(str_ip_index));
			j = 0;
			continue;
		}

		str_ip_index[j] = ip[i];
		j ++;
	}

	return ip_add;
}


/*******************************************************************************
函数功能：
*   @ 处理返回的数据,增加到列表
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void AddDevs(const char *pcIn, unsigned int dwLen)
{
    if (stDev.stRx.stRx.dwIP == 0) return;
    if (stDev.stScanDevs.dwDevCnt >= XCAN_LINK_SURPORT_NUM) return;

    if(stDev.stScanDevs.dwDevCnt == 0) {
        stDev.stScanDevs.stRemoteDev[stDev.stScanDevs.dwDevCnt] = stDev.stRx.stRx;
        stDev.stScanDevs.dwDevCnt ++;

        return;
    }

    for(unsigned int i = 0; i < stDev.stScanDevs.dwDevCnt; i ++) {
        if(stDev.stRx.stRx.dwIP == stDev.stScanDevs.stRemoteDev[i].dwIP) {
            stDev.stScanDevs.stRemoteDev[stDev.stScanDevs.dwDevCnt] = stDev.stRx.stRx;
            return;
        }
    }

    stDev.stScanDevs.stRemoteDev[stDev.stScanDevs.dwDevCnt] = stDev.stRx.stRx;
    stDev.stScanDevs.dwDevCnt ++;
}

/*******************************************************************************
函数功能：
*   @ 适配扫描返回
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int DoReturn(void)
{
    unsigned int dwLen = 0;
    char *cCp = (char *)stDev.abyTxBuff;

    stDev.stMyConfig.byOp = OP_RET;
    if(stDev.stMyConfig.dwLinkIP != 0 && stDev.stMyConfig.dwLinkTcpPort != 0) {
        stDev.stMyConfig.bySta = STA_LINKED;
    }
    else stDev.stMyConfig.bySta = STA_UNLINK;

    memset(stDev.abyTxBuff, 0, 128);
    Head(cCp);

    dwLen = strlen((const char *)stDev.abyTxBuff);
    if(stDev.pfUdp) stDev.pfUdp( stDev.stRx.stRx.dwIP, stDev.stRx.stRx.dwUdpPort, (const char *)stDev.abyTxBuff, dwLen);
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 连接断开
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void DisLink(void)
{
    stDev.stMyConfig.dwIP = 0;
    for(int i = 0; i < 32; i ++) stDev.stMyConfig.a2CTxt[FUNC_IP][i] = 0;
}

/*******************************************************************************
函数功能：
*   @ 输入信号
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void In(const char *pcIn, unsigned int dwLen)
{
    unsigned char byLine = 0;
    unsigned char byCol = 0;
    unsigned int dwLenIndex = 0;
    unsigned int dwLenRx = 0;
	char cGet = 0;

    // 接收分段
    memset(abyRxMutric, 0, sizeof(abyRxMutric));
    for(int i = 0; i < 256 && pcIn[i] != 0 && byCol < 31; i ++) {
        if(byLine > 13) break;

        if(pcIn[i] == 0x0d && pcIn[i + 1] == 0x0a) {
            byCol = 0;
            byLine ++;
            i ++;
            continue;
        }

		if (pcIn[i] == 0x0d && pcIn[i + 1] != 0x0a) return;
		if (pcIn[i] != 0x0d && pcIn[i + 1] == 0x0a) return;

        abyRxMutric[byLine][byCol ++] = pcIn[i];
    }

    // 初始化接收方法
    memset(&stDev.stRx, 0, sizeof(stDev.stRx));

    // 提取数据
    for(int line = 0; line < 11; line ++) {
        dwLenIndex = strlen((const char *)XlinkHead[line]);
		dwLenRx    = strlen((const char *)abyRxMutric[line]);

		if (dwLenRx < dwLenIndex) return;
		if (dwLenRx == dwLenIndex) continue;

        for(unsigned int i = dwLenIndex; i < dwLenRx; i ++) {
			cGet = abyRxMutric[line][i];
			stDev.stRx.stRx.a2CTxt[line][i - dwLenIndex] = cGet;
			cGet = 0;
			cGet = stDev.stRx.stRx.a2CTxt[line][i - dwLenIndex];
        }

		abyRxMutric[line][dwLenIndex] = 0;
		if (strcmp((const char*)abyRxMutric[line], (const char*)XlinkHead[line]) != 0) return;
    }

    // 处理数据
    // ip
    stDev.stRx.stRx.dwIP = str2ip( (const char *)stDev.stRx.stRx.a2CTxt[FUNC_IP]);
    stDev.stRx.stRx.dwLinkIP = str2ip((const char *)stDev.stRx.stRx.a2CTxt[FUNC_LINKIP]);

    // port
    stDev.stRx.stRx.dwUdpPort = atoi((const char *)stDev.stRx.stRx.a2CTxt[FUNC_UDPPORT]);
    stDev.stRx.stRx.dwTcpPort = atoi((const char *)stDev.stRx.stRx.a2CTxt[FUNC_TCPPORT]);
    stDev.stRx.stRx.dwLinkTcpPort = atoi((const char *)stDev.stRx.stRx.a2CTxt[FUNC_LINKTCP]);

    // mac

    // op
    if(strcmp((const char*)abyRxMutric[11] , (const char*)XlinkHead[11 + OP_RET]) == 0) stDev.stRx.op = 0;
    else if(strcmp((const char*)abyRxMutric[11] , (const char*)XlinkHead[11 + OP_SCAN]) == 0) stDev.stRx.op = 1;
    else if(strcmp((const char*)abyRxMutric[11] , (const char*)XlinkHead[11 + OP_DOLINK]) == 0) stDev.stRx.op = 2;
    else if(strcmp((const char*)abyRxMutric[11] , (const char*)XlinkHead[11 + OP_DISLINK]) == 0) stDev.stRx.op = 3;
    else stDev.stRx.op = 5;

    // statuc
    if(strcmp((const char*)abyRxMutric[12] , (const char*)XlinkHead[15 + STA_NULL]) == 0) stDev.stRx.status = 0;
    else if(strcmp((const char*)abyRxMutric[12] , (const char*)XlinkHead[15 + STA_ERR]) == 0) stDev.stRx.op = 1;
    else if(strcmp((const char*)abyRxMutric[12] , (const char*)XlinkHead[15 + STA_LINKED]) == 0) stDev.stRx.status = 2;
    else if(strcmp((const char*)abyRxMutric[12] , (const char*)XlinkHead[15 + STA_UNLINK]) == 0) stDev.stRx.status = 3;
    else stDev.stRx.status = 5;

    // 处理结果
    if(stDev.stRx.status == 5 || stDev.stRx.op == 5) return;
    if(stDev.stRx.op == OP_RET) {
        AddDevs(pcIn, dwLen);
        if (stDev.pfNotice) stDev.pfNotice(pcIn, dwLen);
    }

    DoReturn();
    if(stDev.stRx.op == OP_SCAN) return;
    if(stDev.stRx.op == OP_DISLINK && stDev.stRx.stRx.dwIP == stDev.stMyConfig.dwIP) DisLink();
    if (stDev.pfLink) stDev.pfLink(stDev.stRx.stRx.dwLinkIP, stDev.stRx.stRx.dwTcpPort);
}

static const DevXcanLinkTdf s_Dev = {
    SetMyIp,
    SetMyUdpPort,
    SetMyTcpPort,
    SetLinkIp,
    SetLinkTcpPort,
    ConfigDev,

    SetSendPort,
	SetEventPort,

    Scan,
    Up,
    Down,
    In
};

/*******************************************************************************
函数功能：
*   @ 初始化xcan协议设备
参数描述：
*   @ 无
返回值描述：
    串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
const DevXcanLinkTdf *XcanLinkInit(void)
{
    //注册通道,初始化。
    memset(&stDev, 0, sizeof(stDev));
    return &s_Dev;
}