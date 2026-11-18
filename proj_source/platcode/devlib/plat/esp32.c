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
20200826：
第一次由马德嘉创建初始程序。
ESP32为蓝牙wifi模块，底层驱动分为基础指令，wifi指令、TCP和蓝牙指令4个部分，目前仅按照项目
需求做相关指令，扩展其他指令需要自行添加

基本工作流程：
wifi：
    WifiInit(void) 初始化，配置为station
    WifiConnect(char *Ssid,char *Pwd) 连接热点，若不知道需连接的热点信息则应先扫描
    WifiTcpConn()或WifiUdpConn() 建立传输协议  注意目标IP和网段必须正确，否则串口不会返回错误信息
透传
    WifiEstabTrans(void)配置透传
    完成透传配置后调用
    SppSend(char *pStr) 函数发送数据
    透传模式无法监控网络状态
指令传输
    WifiCmdSend()通过指令发送数据

透传模式无法检测连接断开


ble:
    BleInit(void) 初始化
    BleGattsSrvCfg(void) 配置服务
    BleAdv(char* AdvName) 启动广播
    等待手机连接蓝牙
    BleCheckConn(void) 检查连接状态

透传：
    BleSppCfg()配置透传
    完成透传配置后调用
    SppSend(char *pStr) 函数发送数据
    透传模式无法监控网络状态

指令选项：
    调用BleCmdSend(char *pStr) 发送数据


注意事项：ESP32仅具备2.4GHz频段收发功能，无法进行5GHz频段的通信

调试模式：将宏 CONFIG_BT_DEBUG 打开后，可通过串口助手查看ESP32返回信息，
也可配置DEBUG函数查看发出的信息
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_ESP32)

/*****************延时函数接口*******/
#include "FreeRTOS.h"
#include "task.h"

#define fDelayMs(x)   vTaskDelay(x)


/*******************************************************************************
常量定义
*******************************************************************************/
//内部常量
#define ESP32_RX_BUFF_SIZE  600	//
#define AP_SCAN             20   //最大AP显示数量
#define AP_NAME_LEN         35
#define AP_SSID_LEN         6



//基础指令
#define AT	0	//测试AT启动
#define AT_RST	1	//重启模块
#define AT_GMR	2	//查看版本信息
#define AT_GSLP	3	//进?Deep-sleep模式
#define ATE	4	//开关回显功能
#define AT_RESTORE	5	//恢复出?设置
#define AT_UART_CURUART	6	//当前临时配置
#define AT_UART_DEFUART	7	//默认配置，保存到Flash
#define AT_SLEEP	8	//设置sleep模式
#define AT_SYSRAM	9	//查询当前剩余RAM??
#define AT_SYSFLASH	10	//读写flash?户分区
#define AT_FS	11	//?件系统操作
#define AT_RFPOWER	12	//设置RFTXPower

//wifi指令
#define AT_CWMODE	0	//设置Wi-Fi模式(STA/AP/STA+AP)
#define AT_CWJAP	1	//连接AP
#define AT_CWLAPOPT	2	//设置#define AT_CWLAP指令扫描结果的属性
#define AT_CWLAP	3	//扫描附近的AP信息
#define AT_CWQAP	4	//与AP断开连接
#define AT_CWSAP	5	//设置ESP32SoftAP配置
#define AT_CWLIF	6	//获取连接到ESP32SoftAP的
#define AT_CWDHCP	7	//设置DHCP
#define AT_CWDHCPS	8	//设置ESP32SoftAPDHCP
#define AT_CWAUTOCONN	9	//设置上电时是否?动连接AP
#define AT_CWSTARTSMART	10	//开始SmartConfig
#define AT_CWSTOPSMART	11	//停?SmartConfig
#define AT_WPS	12	//设置WPS功能
#define AT_CWHOSTNAME	13	//设置ESP32station主机名称
#define AT_MDNS	14	//MDNS功能

//TCP/IP指令
#define AT_CIPSTATUS	0	//查询?络连接信息
#define AT_CIPDOMAIN	1	//域名解析功能
#define AT_CIPDNS	2	//?定义DNS服务器
#define AT_CIPSTAMAC	3	//设置ESP32Station的MAC地址
#define AT_CIPAPMAC	4	//设置ESP32SoftAP的MAC地址
#define AT_CIPSTA	5	//设置ESP32Station的IP地址
#define AT_CIPAP	6	//设置ESP32SoftAP的IP地址
#define AT_CIPSTART	7	//建?TCP连接，UDP传输或者SSL连接
#define AT_CIPSSLCCONF	8	//配置SSLClient
#define AT_CIPSEND	9	//发送数据
#define AT_CIPSENDEX	10	//发送数据，达到设置?度，或者遇到字符\0，则发送数据
#define AT_CIPCLOSE	11	//关闭TCP/UDP/SSL传输
#define AT_CIFSR	12	//查询本地IP地址
#define AT_CIPMUX	13	//设置多连接模式
#define AT_CIPSERVER	14	//设置TCP服务器
#define AT_CIPSERVERMAXCONN	15	//设置TCP服务器允许的最?连接数
#define AT_CIPMODE	16	//设置透传模式
#define AT_SAVETRANSLINK	17	//保存透传连接到Flash
#define AT_CIPSTO	18	//设置ESP32作为TCP服务器时的超时时间
#define AT_CIUPDATE	19	//通过Wi-Fi升级软件
#define AT_CIPDINFO	20	//接收?络数据时，+IPD是否提示对端IP和端?
#define AT_CIPSNTPCFG	21	//设置时域和SNTP服务器
#define AT_CIPSNTPTIME	22	//查询SNTP时间
#define AT_PING	23	//Ping功能

//蓝牙指令
#define AT_BLEINIT      	0	//BLE初始化
#define AT_BLEADDR	1	//设置BLE设备地址
#define AT_BLENAME	2	//设置BLE设备名称
#define AT_BLESCANPARAM	3	//设置BLE扫描参数
#define AT_BLESCAN	4	//使能BLE扫描
#define AT_BLESCANRSPDATA	5	//设置BLE扫描响应
#define AT_BLEADVPARAM	6	//设置BLE?播参数
#define AT_BLEADVDATA	7	//设置BLE?播数据
#define AT_BLEADVSTART	8	//开始BLE?播
#define AT_BLEADVSTOP	9	//结束BLE?播
#define AT_BLECONN	10	//建?BLE连接
#define AT_BLECONNPARAM	11	//更新BLE连接参数
#define AT_BLEDISCONN	12	//断开BLE连接
#define AT_BLEDATALEN	13	//设置BLE数据包?度
#define AT_BLECFGMTU	14	//设置BLEMTU
#define AT_BLEGATTSSRVCRE	15	//GATTS创建服务
#define AT_BLEGATTSSRVSTART	16	//GATTS开启服务
#define AT_BLEGATTSSRVSTOP	17	//GATTS关闭服务
#define AT_BLEGATTSSRV	18	//GATTS查询服务
#define AT_BLEGATTSCHAR	19	//GATTS查询服务特征
#define AT_BLEGATTSNTFY	20	//GATTS通知服务特征值
#define AT_BLEGATTSIND	21	//GATTS指示服务特征值
#define AT_BLEGATTSSETATTR	22	//GATTS设置服务特征值
#define AT_BLEGATTCPRIMSRV	23	//GATTC发现基本服务
#define AT_BLEGATTCINCLSRV	24	//GATTC发现包含服务
#define AT_BLEGATTCCHAR	25	//GATTC查询服务特征
#define AT_BLEGATTCRD	26	//GATTC读取服务特征值
#define AT_BLEGATTCWR	27	//GATTC写服务特征值
#define AT_BLESPPCFG	28	//配置BLESPP
#define AT_BLESPP	                29	//开启BLESPP
#define AT_BLESECPARAM	30	//设置BLESMP
#define AT_BLEENC	                31	//开始BLE配对
#define AT_BLEENCRSP	32	//设置BLE配对响应
#define AT_BLEKEYREPLY	33	//回复BLE配对密钥
#define AT_BLECONFREPLY	34	//回复BLE配对结果
#define AT_BLEENCDEV	35	//列举已配对的设备
#define AT_BLEENCCLEAR	36	//解除绑定



/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
static struct {
#if(CONFIG_BT_DEBUG)
    bool blDebugTx;
    unsigned char byDebugUart;
#endif
    unsigned char byUartSn;
    bool blDevExsit;

    // 连接状态
    emConsta bIIsConn;//wifi连接状态
    bool blIsBleConn;//蓝牙连接状态

    // 接收逻辑
    unsigned short int byRxCnt;//接收计数
    unsigned char byaRx[ESP32_RX_BUFF_SIZE];//接收缓存

    unsigned char byApNum;  //扫描到的AP总数量
    struct //扫描wifi连接点信息 ;
    {
        char bSsid[AP_NAME_LEN];
        char bRssi[AP_SSID_LEN];
    }stApInfo[AP_SCAN];

    char brSsid[AP_NAME_LEN];//连接SSID记录，用于记录自动重连信息
    char byaWifiMac[14];

    char byaBleName[22];
    bool blSearchDev;
}s_stCtr;


typedef enum
{
    COM_CMD,
    BT_CMD,    // 蓝牙
    WIFI_CMD,
    TCP_CMD,
}emESP32CmdTdf;

emESP32CmdTdf  emESP32Cmd;
/*******************************************************************************
内部全局变量申明
*******************************************************************************/
//AT指令不区分大小写 以回车换行结束
//指令响应以回传+回应指令形式， 比如发AT 返回AT\r\n OK\r\n

//基础指令
static const unsigned char* const AT_CMD[14][2] = {
    {"AT","OK"},	     //测试AT启动		0
    {"AT+RST","OK"},	//重启模块		1
    {"AT+GMR","OK"},	//查看版本信息		2
    {"AT+GSLP","OK"},	//进入Deep-sleep模式		3
    {"ATE","OK"},	     //开关回显功能		4
    {"AT+RESTORE","OK"},	//恢复出?设置		5
    {"AT+UART_CURUART","OK"},	//当前临时配置		6
    {"AT+UART_DEFUART","OK"},	//默认配置，保存到Flash		7
    {"AT+SLEEP","OK"},	     //设置sleep模式		8
    {"AT+SYSRAM","OK"},	    //查询当前剩余RAM??		9
    {"AT+SYSFLASH","OK"},	//读写flash?户分区		10
    {"AT+FS","OK"},      	//文件系统操作		11
    {"AT+RFPOWER","OK"},	//设置RFTXPower		12
};

//WiFi指令
static const unsigned char* const AT_WifiCMD[15][2] = {
    {"AT+CWMODE","OK"},	//设置Wi-Fi模式(STA/AP/STA+AP)	0
    {"AT+CWJAP","OK"},	//连接AP	1
    {"AT+CWLAPOPT","OK"},	//设置AT+CWLAP指令扫描结果的属性	2
    {"AT+CWLAP","OK"},	//扫描附近的AP信息	3
    {"AT+CWQAP","OK"},	//与AP断开连接	4
    {"AT+CWSAP","OK"},	//设置ESP32SoftAP配置	5
    {"AT+CWLIF","OK"},	//获取连接到ESP32SoftAP的	6
    {"AT+CWDHCP","OK"},	//设置DHCP	7
    {"AT+CWDHCPS","OK"},	//设置ESP32SoftAPDHCP	8
    {"AT+CWAUTOCONN","OK"},	//设置上电时是否?动连接AP	9
    {"AT+CWSTARTSMART","OK"},	//开始SmartConfig	10
    {"AT+CWSTOPSMART","OK"},	//停?SmartConfig	11
    {"AT+WPS","OK"},	//设置WPS功能	12
    {"AT+CWHOSTNAME","OK"},	//设置ESP32station主机名称	13
    {"AT+MDNS","OK"},	//MDNS功能	14
};

//tcp指令
static const unsigned char* const AT_TCPCMD[24][2] = {
    {"AT+CIPSTATUS","OK"},	//查询?络连接信息			0
    {"AT+CIPDOMAIN","OK"},	//域名解析功能			1
    {"AT+CIPDNS","OK"},	//定义DNS服务器			2
    {"AT+CIPSTAMAC","OK"},	//设置ESP32Station的MAC地址			3
    {"AT+CIPAPMAC","OK"},	//设置ESP32SoftAP的MAC地址			4
    {"AT+CIPSTA","OK"},	//设置ESP32Station的IP地址			5
    {"AT+CIPAP","OK"},	//设置ESP32SoftAP的IP地址			6
    {"AT+CIPSTART","OK"},	//建?TCP连接，UDP传输或者SSL连接			7
    {"AT+CIPSSLCCONF","OK"},	//配置SSLClient			8
    {"AT+CIPSEND","OK"},	//发送数据			9
    {"AT+CIPSENDEX","OK"},	//发送数据，达到设置?度，或者遇到字符\0，则发送数据			10
    {"AT+CIPCLOSE","OK"},	//关闭TCP/UDP/SSL传输			11
    {"AT+CIFSR","OK"},	//查询本地IP地址			12
    {"AT+CIPMUX","OK"},	//设置多连接模式			13
    {"AT+CIPSERVER","OK"},	//设置TCP服务器			14
    {"AT+CIPSERVERMAXCONN","OK"},	//设置TCP服务器允许的最?连接数			15
    {"AT+CIPMODE","OK"},	//设置透传模式			16
    {"AT+SAVETRANSLINK","OK"},	//保存透传连接到Flash			17
    {"AT+CIPSTO","OK"},	//设置ESP32作为TCP服务器时的超时时间			18
    {"AT+CIUPDATE","OK"},	//通过Wi-Fi升级软件			19
    {"AT+CIPDINFO","OK"},	//接收?络数据时，+IPD是否提示对端IP和端?			20
    {"AT+CIPSNTPCFG","OK"},	//设置时域和SNTP服务器			21
    {"AT+CIPSNTPTIME","OK"},	//查询SNTP时间			22
    {"AT+PING","OK"},	//Ping功能			23
};

//蓝牙指令
static const unsigned char* const AT_BtCMD[40][2] = {
    {"AT+BLEINIT","OK"},	//BLE初始化	0
    {"AT+BLEADDR","OK"},	//设置BLE设备地址	1
    {"AT+BLENAME","OK"},	//设置BLE设备名称	2
    {"AT+BLESCANPARAM","OK"},	//设置BLE扫描参数	3
    {"AT+BLESCAN","OK"},	//使能BLE扫描	4
    {"AT+BLESCANRSPDATA","OK"},	//设置BLE扫描响应	5
    {"AT+BLEADVPARAM","OK"},	//设置BLE?播参数	6
    {"AT+BLEADVDATA","OK"},	//设置BLE?播数据	7
    {"AT+BLEADVSTART","OK"},	//开始BLE?播	8
    {"AT+BLEADVSTOP","OK"},	//结束BLE?播	9
    {"AT+BLECONN","OK"},	//建?BLE连接	10
    {"AT+BLECONNPARAM","OK"},	//更新BLE连接参数	11
    {"AT+BLEDISCONN","OK"},	//断开BLE连接	12
    {"AT+BLEDATALEN","OK"},	//设置BLE数据包?度	13
    {"AT+BLECFGMTU","OK"},	//设置BLEMTU	14
    {"AT+BLEGATTSSRVCRE","OK"},	//GATTS创建服务	15
    {"AT+BLEGATTSSRVSTART","OK"},	//GATTS开启服务	16
    {"AT+BLEGATTSSRVSTOP","OK"},	//GATTS关闭服务	17
    {"AT+BLEGATTSSRV","OK"},	//GATTS查询服务	18
    {"AT+BLEGATTSCHAR","OK"},	//GATTS查询服务特征	19
    {"AT+BLEGATTSNTFY","OK"},	//GATTS通知服务特征值	20
    {"AT+BLEGATTSIND","OK"},	//GATTS指示服务特征值	21
    {"AT+BLEGATTSSETATTR","OK"},	//GATTS设置服务特征值	22
    {"AT+BLEGATTCPRIMSRV","OK"},	//GATTC发现基本服务	23
    {"AT+BLEGATTCINCLSRV","OK"},	//GATTC发现包含服务	24
    {"AT+BLEGATTCCHAR","OK"},	//GATTC查询服务特征	25
    {"AT+BLEGATTCRD","OK"},	//GATTC读取服务特征值	26
    {"AT+BLEGATTCWR","OK"},	//GATTC写服务特征值	27
    {"AT+BLESPPCFG","OK"},	//配置BLESPP	28
    {"AT+BLESPP","OK"},	//开启BLESPP	29
    {"AT+BLESECPARAM","OK"},	//设置BLESMP	30
    {"AT+BLEENC","OK"},	//开始BLE配对	31
    {"AT+BLEENCRSP","OK"},	//设置BLE配对响应	32
    {"AT+BLEKEYREPLY","OK"},	//回复BLE配对密钥	33
    {"AT+BLECONFREPLY","OK"},	//回复BLE配对结果	34
    {"AT+BLEENCDEV","OK"},	//列举已配对的设备	35
    {"AT+BLEENCCLEAR","OK"},	//解除绑定	36
};
/*******************************************************************************
内部函数声明
*******************************************************************************/
static emConsta WifiInqConnState(void);

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
    if(s_stCtr.byRxCnt < ESP32_RX_BUFF_SIZE ) {
        s_stCtr.byaRx[s_stCtr.byRxCnt] = byIn;
        s_stCtr.byRxCnt ++;
    }
    else  {
        memset(s_stCtr.byaRx, 0, ESP32_RX_BUFF_SIZE);
        s_stCtr.byRxCnt = 0;
    }

    if(s_stCtr.byRxCnt>6)
    {
        if(strstr( (char const*)s_stCtr.byaRx, "+BLECONNPARAM") ){
            if(strstr( (char const*)s_stCtr.byaRx, "+WRITE:") )
                s_stCtr.blIsBleConn = true;
        }
        else if(strstr( (char const*)s_stCtr.byaRx, "+BLEDISCONN") ){
            s_stCtr.blIsBleConn = false;
        }
        else if(strstr( (char const*)s_stCtr.byaRx, "WIFI DISCONNECT")){
            s_stCtr.bIIsConn = emConnBreak;

        }
        else if(strstr( (char const*)s_stCtr.byaRx, "CLOSED")){
            s_stCtr.bIIsConn = emConnBreak;
        }
    }

    #if(CONFIG_BT_DEBUG)
          DevUart->SendByte(s_stCtr.byDebugUart, byIn);
    #endif
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
    memset(s_stCtr.byaRx, 0, ESP32_RX_BUFF_SIZE);
    s_stCtr.byRxCnt = 0;
}

/*****************************************************************************
函数名：static void EspDelay(int dwTimOut)
函数功能：延时
参数描述：

返回值描述：
特殊算法：…
注意事项： 如果延时时间不够，会出现无法处理指令，返回ERROR的问题
******************************************************************************/
static void EspDelay(unsigned int dwTimOut)
{
    for(int wait = 0; wait < dwTimOut; wait ++)
    {
        for(int j = 0 ; j < 20000 ; j++);

    }
}
/*****************************************************************************
函数名：static int WrCmd(unsigned char byCmd, unsigned int dwTimOut, char *pbyPar,emESP32CmdTdf emCmd)

函数功能：发送控制命令
参数描述：
*   @ unsigned char byCmd 需要发送的命令序号
*   @ unsigned int dwTimOut 等待数据返回时间
*   @ const unsigned char *pbyPar 参数设置
返回值描述：
特殊算法：…
注意事项： 如果延时时间不够，会出现无法处理指令，返回ERROR的问题
******************************************************************************/
static int WrCmd(unsigned char byCmd, unsigned int dwTimOut, char *pbyPar,emESP32CmdTdf emCmd)
{
    char *pcCmd = NULL;
    unsigned char byLen = 0;
    char abyCmdStr[100]={0};

    pcCmd =abyCmdStr;
    if(emCmd == COM_CMD)//使用不同的模式指令
        strcpy(pcCmd, (char const*)AT_CMD[byCmd][0]);
    else if(emCmd == BT_CMD)
        strcpy(pcCmd, (char const*)AT_BtCMD[byCmd][0]);
    else if(emCmd == WIFI_CMD)
        strcpy(pcCmd, (char const*)AT_WifiCMD[byCmd][0]);
    else  strcpy(pcCmd, (char const*)AT_TCPCMD[byCmd][0]);

    if( pbyPar)//如果需要带参数
        strcat(pcCmd, (char const*)pbyPar);//拼接字符串

    strcat(pcCmd, "\r\n");//拼接结束符

    StartRx();//开启接收
    byLen = strlen((const char *)pcCmd);
    DevUart->SendString(s_stCtr.byUartSn, (const unsigned char*)pcCmd, byLen);

#if(CONFIG_BT_DEBUG)
    if(s_stCtr.blDebugTx) {
        DevUart->SendString(s_stCtr.byDebugUart, (const unsigned char*)pcCmd, byLen);
    }
#endif

    EspDelay(dwTimOut);//延时等待
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 判断返回值 是否正确
*参数描述：
*   @
*返回值描述：
*   @  0 命令执行成功
*   @ -1 命令执行失败
*说明：
*   @ 无
*******************************************************************************/
static bool CheakRet(unsigned char byCmd,unsigned char *pbyPar)
{
    const unsigned char *pcCmd = NULL;
    unsigned char *pcRet = s_stCtr.byaRx;

    if(*pbyPar){//查找是否有字符串 有则返回true
        char *p=strstr((char const*)pcRet,(char const*)pbyPar);//查找接收数据是否一致

        if(p == NULL)
            return false;
        else  return true;
    }
    else{//查找默认的回应值
        pcCmd = AT_CMD[byCmd][1];
        char *p=strstr((char const*)pcRet,(char const*)pcCmd);//查找接收数据是否一致

        if(p == NULL)
            return false;
        else  return true;
    }
}
/*******************************************************************************
*函数功能：
*   @ 检查模块是否存在
*参数描述： t通过发送AT指令 如果有回应则有设备存在。
*   @ 无
*返回值描述：
*   @ true 设备存在
*   @ false 设备不存在
*说明：
*   @ 无
*******************************************************************************/
static bool IsDevExist(void)
{
    bool blRet = false;

    WrCmd(AT , 500, NULL,COM_CMD);
    blRet = CheakRet(NULL,"OK");

    if(!blRet)
        s_stCtr.blDevExsit = false;
    else
        s_stCtr.blDevExsit = true;

    return blRet;
}



/*******************************************************************************
*函数功能：
*   @ 回复出厂设置
*参数描述：
*   @ 无
*返回值描述：
*说明：
*   @ 无
*******************************************************************************/
static bool Restore(void)
{
    bool blRet = false;

    WrCmd(AT_RESTORE , 1500, NULL,COM_CMD);
    blRet = CheakRet(NULL,"OK");

    return blRet;
}
/*****************************************************************************
函数名：static void WifiDisconnect(void)
函数功能：断开与AP的连接
参数描述：
char *Ssid:
char *Pwd
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static bool SleepMode(bool blSleep)
{
    bool blRet = false;

    if(blSleep)
    WrCmd(AT_SLEEP , 500,"=1",COM_CMD);
    else WrCmd(AT_SLEEP , 500,"=0",COM_CMD);

    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;

    return true;
}
/*****************************************************************************
函数名：static void CleanApInf(void)
函数功能：WIFI参数初始化
参数描述：

返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void CleanApInf(void)
{
    memset(s_stCtr.stApInfo,0,sizeof(s_stCtr.stApInfo));
}
/*****************************************************************************
函数名：static void WifiInit(void)
函数功能：开启WIFI功能
参数描述：

返回值描述：
特殊算法：…
注意事项：Station 模式
******************************************************************************/
static bool WifiInit(void)
{
    bool blRet = false;

    IsDevExist();
    if(!s_stCtr.blDevExsit) return false;
    memset(s_stCtr.stApInfo,0,sizeof(s_stCtr.stApInfo));//清除AP信息缓存

    WrCmd(AT_CWMODE , 500,"=1",WIFI_CMD);

    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;

    return true;

}

/*****************************************************************************
函数名：static void WifiConnect(char *Ssid,char *Pwd)
函数功能：连接AP
参数描述：
返回值描述：TRUE:连接成功，FALSE：连接失败
特殊算法：…
注意事项：
******************************************************************************/
static bool WifiConnect(char *Ssid,char *Pwd)
{
    char str[50];
    bool blRet = false;
    char cCheckTime = 16;


    sprintf(str,"=%s,\"%s\"",Ssid,Pwd);

    WrCmd(AT_CWJAP , 4000, str,WIFI_CMD);
    blRet = CheakRet(NULL,"WIFI GOT IP");

    if(!blRet)
    {
        while(cCheckTime--)
        {
//             blRet = CheakRet(NULL,"WIFI CONNECTED");
            blRet = CheakRet(NULL,"WIFI GOT IP");
             if(blRet)break;

             blRet = CheakRet(NULL,"ERROR");
             if(blRet) break;

             fDelayMs(1000);
        }

        s_stCtr.bIIsConn = WifiInqConnState();
        if(s_stCtr.bIIsConn == emApConn)
            return true;
        else
            return false;
    }
    else
    {
        s_stCtr.bIIsConn = WifiInqConnState();
        sprintf(s_stCtr.brSsid,"%s",Ssid);
        return true;
    }

}

/*****************************************************************************
函数名：static void WifiDisconnect(void)
函数功能：断开与AP的连接
参数描述：
char *Ssid:
char *Pwd
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void WifiDisconnect(void)
{
//    bool blRet = false;
    WrCmd(AT_CWQAP , 2000, NULL,WIFI_CMD);
//    blRet = CheakRet(NULL,"OK");

    s_stCtr.bIIsConn = WifiInqConnState();


}


/*****************************************************************************
函数名：static bool WifiInqConnState(void)
函数功能：查询连接状态，目前主要用于检测自动连接
参数描述：
char *pSsid: 返回记录最后连接的ssid的指针
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static emConsta WifiInqConnState(void)
{
    char *pby ;

    WrCmd(AT_CIPSTATUS , 500, NULL,TCP_CMD);

    pby = strstr( (const char*)s_stCtr.byaRx, "STATUS:");

    if(pby)
    {
        pby = pby + 7;
        switch(*pby)
        {
        case '2': return emApConn;
        case '3': return emTcpConn;
        case '4': return emConnBreak;
        case '5': return emNoConn;
        default:  return emNoConn;
        }
    }
    return emNoConn;

}


/*****************************************************************************
函数名：static bool WifiGetConnSta(void)
函数功能：查询连接状态
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static emConsta WifiGetConnSta(void)
{
    return s_stCtr.bIIsConn;
}




/*****************************************************************************
函数名：static void WifiSetLip (char * lIp,char *Gateway, char *Netmask)
函数功能：设置本地IP,同时设置网关和掩码
参数描述：
char * lIp:本地IP
char *Gateway:网关
char *Netmask:子网掩码
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static bool WifiSetLip (char * lIp,char *Gateway, char *Netmask)
{
    char str[60];
    bool blRet = false;
    sprintf(str,"=\"%s\",\"%s\",\"%s\"",lIp,Gateway,Netmask);
    WrCmd(AT_CIPSTA , 500, str,TCP_CMD);

    if(!blRet)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************
函数功能：返回扫描到的总热点数量
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static unsigned char WifiGetScanApNum(void)
{
    return s_stCtr.byApNum;
}

/*****************************************************************************
函数名：static char* WifiGetScanSsid(int wCnt)
函数功能：获取扫描AP信息
参数描述：

返回值描述：AP信息结构体
特殊算法：…
注意事项：
******************************************************************************/
static char* WifiGetScanSsid(int wCnt)
{
    if(wCnt >= AP_SCAN) return NULL;

    return s_stCtr.stApInfo[wCnt].bSsid;
}


static void WifiCleanAp(void)
{
    memset(s_stCtr.stApInfo,0,sizeof(s_stCtr.stApInfo) );
}
/*****************************************************************************
函数名：static void WifiGetIp(void)
函数功能：获取本机IP及MAC，仅输出Station IP
参数描述：

返回值描述：
特殊算法：…
注意事项：
    返回信息举例

    +CIFSR:APIP,"192.168.4.1"
    +CIFSR:APMAC,"84:0d:8e:d0:8d:0d"
    +CIFSR:STAIP,"0.0.0.0"
    +CIFSR:STAMAC,"84:0d:8e:d0:8d:0c"
******************************************************************************/
static void WifiInqIpMac(char* plbStaAp ,char* pbMac)
{

    WrCmd(AT_CIFSR , 500, NULL,TCP_CMD);
    char str[40];

    char *pby = (char*)s_stCtr.byaRx;
    char *pbye = 0;

    pby = strstr( pby, "STAIP,");  //查找到地址前端
    if(pby != 0)
    {
        //提取STAIP
        pby = pby + 7;                      //跨过第一个引号
        pbye = strstr( pby  , "\"");        //搜索第二个引号
        pbye = pbye;

        memset(str,0,40);
        memcpy(str,pby,pbye - pby);
        strcpy(plbStaAp,str);           //结果不保留引号

    }

    pby = strstr( pby, "STAMAC,");  //查找到地址前端字符
    if(pby != 0)
    {
        pby = pby + 8;              //跨过第一个引号

        memset(str,0,40);
        memcpy(str,pby,17);
        strcpy(pbMac,str);
    }
}

/*****************************************************************************
函数名：static void WifiScan(void)
函数功能：AP扫描
参数描述：

返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void WifiScan(void)
{
    CleanApInf();   //每次扫描前清空扫描信息
    WrCmd(AT_CWLAPOPT , 500, "=1,6",WIFI_CMD);//
    WrCmd(AT_CWLAP , 2500, NULL,WIFI_CMD);

//    返回信息格式
//    AT+CWLAP
//    +CWLAP:("ssid",rssi)
//
//    举例 :
//    AT+CWLAP
//    +CWLAP:("office_01",-55,)
//    +CWLAP:("BC3",-58,)

    char *pby = (char*)s_stCtr.byaRx;
    char *pbye = 0;
    int wCnt = 0;
    int wStrLen = 0;

    while(pby != 0)
    {
        pby = strstr( pby, "+CWLAP:(");  //查找到地址前端
        if(pby != 0)
        {
            //提取SSID,保留了冒号
            pby = pby + 8;
            pbye = strstr( pby, "\",");
            pbye = pbye + 1;

            wStrLen = pbye - pby;
            if(wStrLen > AP_NAME_LEN - 1) wStrLen = AP_NAME_LEN - 1;//限制长度
            memcpy(s_stCtr.stApInfo[wCnt].bSsid,pby,wStrLen);

            //提取RSSI
            pby = pbye + 1;
            pbye = strstr(pby,")");

            wStrLen = pbye - pby;
            if(wStrLen > AP_SSID_LEN - 1) wStrLen = AP_SSID_LEN - 1;//限制长度
            memcpy(s_stCtr.stApInfo[wCnt].bRssi,pby,wStrLen);

            pby = pbye;
            wCnt++;
            if(wCnt >= AP_SCAN) break;
        }
    }
    s_stCtr.byApNum = wCnt;
}

/*****************************************************************************
函数名：static void WifiTcpConn(char *rIp, int rPort)
函数功能：建立Tcp连接
参数描述：

返回值描述：TRUE :TCP成功连接;FALSE 连接失败
特殊算法：…
注意事项：
******************************************************************************/
static bool WifiTcpConn(char *rIp, int rPort)
{
    char str[60];
    bool blRet = false;
    char cCheckTime = 16;

    sprintf(str,"=\"TCP\",\"%s\",%d",rIp,rPort);
    WrCmd(AT_CIPSTART , 1500, str,TCP_CMD);
    blRet = CheakRet(NULL,"CONNECT");
    if(!blRet)
    {
        while(cCheckTime--)
        {
             blRet = CheakRet(NULL,"CONNECT");
             if(blRet) break;
             blRet = CheakRet(NULL,"ERROR");
             if(blRet) break;
             fDelayMs(1000);
        }
    }

    s_stCtr.bIIsConn = WifiInqConnState();
    if(s_stCtr.bIIsConn == emTcpConn )
        return true;
    else
        return false;
}

/*****************************************************************************
函数名：static int WifiUdpConn(char *rIp, int rPort, int udpLPort, int udpMode)
函数功能：建立UDP连接
参数描述：
char *rIp:远端IP
int rPort:远端端口
int udpLPort:本地端口
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static bool WifiUdpConn(char *rIp, int rPort, int udpLPort)
{
    char str[60];
    bool blRet = false;

    sprintf(str,"=\"UDP\",\"%s\",%d,%d,0",rIp,rPort,udpLPort);
    WrCmd(AT_CIPSTART , 1500, str,TCP_CMD);
    blRet = CheakRet(NULL,"CONNECT");
    if(!blRet) return false;
    else return true;
}

/*****************************************************************************
函数名：static void WifiEstabTrans(void)
函数功能：建立透传
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static void WifiEstabTrans(void)
{
    WrCmd(AT_CIPMODE , 500, "=1",TCP_CMD);     //设置透传模式
    WrCmd(AT_CIPSEND ,500 , NULL,TCP_CMD);
}



/*****************************************************************************
函数名：static void WifiCmdSend(char *pbyStr, u16 wLen)
函数功能：指令传输
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static void WifiCmdSend(char *pbyStr, u16 wLen)
{
    char str[30];

    sprintf(str,"=%d",wLen);
    WrCmd(AT_CIPSEND , 500, str ,TCP_CMD);
    DevUart->SendString(s_stCtr.byUartSn, (const unsigned char*)pbyStr, wLen);
}
/*****************************************************************************
函数名：static bool BleInit(void)
函数功能：BLE初始化
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static bool BleInit(void)
{
  bool blRet = false;

    WrCmd(AT_BLEINIT , 1200, "=2",BT_CMD);//初始化蓝牙 设置蓝牙为SERVER
    blRet = CheakRet(NULL,"OK");


  if( blRet) return true ;
  return false;
}

/*****************************************************************************
函数名：static void BleSetName(char* bBleName)
函数功能：BLE设置名称
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static void BleSetName(char* bBleName)
{
    strcat(bBleName,s_stCtr.byaBleName);
}

/*****************************************************************************
函数名：static char* BleGetName(void)
函数功能：BLE获取名称
参数描述：
返回值描述：
特殊算法：…
注意事项：
******************************************************************************/
static char* BleGetName(void)
{
   return s_stCtr.byaBleName;
}
/*****************************************************************************
函数名：static void BleSetAddr(char* BleAddr)
函数功能：BLE设置地址
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：输入项BleAddr二进制最高两位必须全为1
******************************************************************************/
static bool BleSetAddr(char* BleAddr)
{
    char str[60];
    bool blRet = false;

    if(*BleAddr & 0xC0 != 0xC0 ) return false;

    sprintf(str,"=1,\"%s\"",BleAddr);
    WrCmd(AT_BLEADDR, 500, str,BT_CMD);//设置蓝牙为从机
    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;
    else return true;
}


/*****************************************************************************
函数名：static void BleGetAddr(char* BleAddr)
函数功能：BLE设置地址
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：输入项BleAddr二进制最高两位必须全为1
******************************************************************************/
static bool BleGetAddr(char* BleAddr)
{
    char *pby = (char*)s_stCtr.byaRx;

    WrCmd(AT_BLEADDR, 500, "?",BT_CMD);//设置蓝牙为从机
    pby = strstr( pby, "+BLEADDR:");
    if(pby)
    {
      pby = pby + 9;
      for(int i=0;i<17;i++){
        if(*pby==0)break;

        if(*pby!=':')
          *BleAddr++=*pby;
        *pby++;
      }
        return true;
    }

    return false;
}
/*****************************************************************************
函数名：static bool BleGattsSrvCfg(void)
函数功能：BLE配置服务
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static bool BleGattsSrvCfg(void)
{
    bool blRet = false;

    WrCmd(AT_BLEGATTSSRVCRE, 200, NULL,BT_CMD);//配置GATTS服务
    blRet = CheakRet(NULL,"OK");
   // if(!blRet) return false;
    for(int i=0;i<0x1fffff;i++);

    WrCmd(AT_BLEGATTSSRVSTART, 200, NULL,BT_CMD);//开启GATTS服务
    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;

    return true;
}

/*****************************************************************************
函数名：static bool BleAdv(char* AdvData)
函数功能：BLE广播
参数描述：
char* AdvData :广播参数
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
    数据排布：
    长度 | 域标识 | 数据
    例如广播数据0201060709424C455F4E5706FFE502434847030302A003174103
    解析为
    长度 | 域标识 | 数据
    02       01      06
    07       09      42 4C 45 5F 4E 57
    06       FF      E5 02 43 48 47
    03       03      02 A0
    03       17      41 03

其中09域为设备广播名称，有效广播数据最大长度37字节
经测试，广播名称最长为11字节，超过则广播失败
******************************************************************************/
static bool BleAdv(char* AdvName)
{

    char *pAdvStr = AdvName;
    char temp[3] = {0};
    char StrNameHex[40] = {0};
    int wNameLen = strlen(AdvName);
    int wHexLen = 0;
    int wStrCnt = 0;

    for(int i = 0 ; i < wNameLen ; i++ ) //字符串的HEX值转换为字符串
    {
        sprintf(temp,"%02x",*(pAdvStr++));
        memcpy(&StrNameHex[wStrCnt],temp,2);
        wStrCnt = wStrCnt + 2;
        wHexLen++;
    }
    wHexLen = wHexLen + 1;//加上域长度

    char str[120];
    bool blRet = false;

    memset(str,0,120);
    sprintf(str,"=\"020106%02x09%s06FFE502434847030302A003174103\"",wHexLen,StrNameHex);//格式化广播数据

    WrCmd(AT_BLEADVDATA, 100, str,BT_CMD);//配置广播数据
    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;

    WrCmd(AT_BLEADVSTART, 100, NULL,BT_CMD);//开启广播
    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;

    return true;
}

/******************************************************************************
函数名：static bool BleCheckConn(void)
函数功能：BLE广播
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：

+BLECONN:0,"74:de:0b:6d:a4:ad"
+BLECONNPARAM:0,0,0,6,0,500
+WRITE:0,1,7,1,2,+BLECONNPARAM:0,0,0,36,0,500

******************************************************************************/
static bool BleCheckConn(void)
{
    return s_stCtr.blIsBleConn;
}

/******************************************************************************
函数名：static bool BleSppCfg(void)
函数功能：BLE广播
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static bool BleDisCon(void)
{
    bool blRet = false;
//
    WrCmd(AT_BLEDISCONN, 500, "=0",BT_CMD);//
    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;
    return true;
}
/******************************************************************************
函数名：static bool BleSppCfg(void)
函数功能：BLE广播
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
static bool BleSppCfg(void)
{
    bool blRet = false;

    WrCmd(AT_BLESPPCFG, 100, "=1,1,7,1,5",BT_CMD);//透传配置
    blRet = CheakRet(NULL,"OK");
  //  if(!blRet) return false;

    WrCmd(AT_BLESPP, 100, NULL,BT_CMD);//开启透传
    blRet = CheakRet(NULL,"OK");
    if(!blRet) return false;
    return true;
}

/*****************************************************************************
函数名：static bool BleGattsSrvCfg(void)
函数功能：BLE配置服务
参数描述：
返回值描述：0 成功  -1  失败
特殊算法：…
注意事项：
******************************************************************************/
//static bool BleGattssrv(void)
//{
//    bool blRet = false;
//
//    WrCmd(AT_BLEGATTSCHAR, 200, '?',BT_CMD);//查询特征
//    blRet = CheakRet(NULL,"AT_BLEGATTSCHAR");
//    if(!blRet) return false;
//
//    WrCmd(AT_BLEGATTSSRVSTART, 200, NULL,BT_CMD);//开启GATTS服务
//    blRet = CheakRet(NULL,"OK");
//    if(!blRet) return false;
//
//    return true;
//}


/*****************************************************************************
函数名：void BleCmdSend(char *pStr)
函数功能：指令发送
参数描述：
char *pStr :待发送的数据
返回值描述：
特殊算法：…
注意事项： 此为蓝牙写特征发送指令，注意写的特征为用于传输的特征
******************************************************************************/
void BleCmdSend(char *pStr)
{
    bool blRet = false;
    u32 dwLen = strlen((const char *)pStr);
    u16 wSendLen = 0;
    while(dwLen){
        blRet = false;
        WrCmd(AT_BLEGATTSNTFY, 200, "=0,1,6,15",BT_CMD);//写特征 一次15个字节
        blRet = CheakRet(NULL,">");
        if(!blRet) return  ;

        if(dwLen>=15){
            dwLen -=15;
            wSendLen = 15;
            DevUart->SendString(s_stCtr.byUartSn, (const unsigned char*)pStr, wSendLen);
            pStr+=wSendLen;
            for(int i=0;i<0xffff;i++);

        }
        else {
            wSendLen = dwLen;
            DevUart->SendString(s_stCtr.byUartSn, (const unsigned char*)pStr, wSendLen);
            if(wSendLen<=15){
            for(int i=0;i<15-wSendLen;i++) //填充15个字符
                DevUart->SendByte(s_stCtr.byUartSn,0);
            }
            break;
        }
    }
}

/*****************************************************************************
函数名：static void SppSend(char *pStr)
函数功能：透传发送
参数描述：
char *pStr :待发送的数据
返回值描述：
特殊算法：…
注意事项： 此为透传发送，需完成透传配置否则无法通信
******************************************************************************/
static void SppSend(char *pStr)
{
    unsigned int byLen;
    byLen = strlen((const char *)pStr);
    DevUart->SendString(s_stCtr.byUartSn, (const unsigned char*)pStr, byLen);
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
}



/*******************************************************************************
*函数功能：
*   @ 调试模块，将数据发送到相应的调试串口
*参数描述：
*   @ bool blOn :true 打开发送调试 false 关闭发送调试，仅查看接收数据
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
        s_stCtr.blDebugTx = false;
    }
    else    s_stCtr.blDebugTx = true;

     s_stCtr.byDebugUart = dwDebugUart;
     DevUart->Config(0, 115200, 1, 0, false);//配置波特率115200
     DevUart->RegHandler( s_stCtr.byDebugUart, DebugTxHandler);
}
#endif



static const DevEsp32Tdf s_Dev = {

    IsDevExist,
    Restore,
//WIFI部分
    WifiInit,
    WifiConnect,
    WifiDisconnect,
    WifiInqConnState,
    WifiGetConnSta,

    WifiSetLip,
    WifiInqIpMac,

    WifiScan,
    WifiGetScanApNum,
    WifiGetScanSsid,
    WifiCleanAp,
    WifiTcpConn,
    WifiUdpConn,
    WifiEstabTrans,
    WifiCmdSend,

//BLE部分
    BleInit,
    BleSetName,
    BleGetName,
    BleSetAddr,
    BleGetAddr,

    BleGattsSrvCfg,
    BleAdv,
    BleCheckConn,
    BleDisCon,
    BleSppCfg,
    BleCmdSend,

    SppSend,
    StartRx,
    SleepMode,
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
const DevEsp32Tdf *Esp32Init(stEsp32InitTdf* pstInit)
{
    memset(&s_stCtr, 0 , sizeof(s_stCtr));

    s_stCtr.byUartSn     = pstInit->byUartSn;

    DevUart->Config(s_stCtr.byUartSn, pstInit->dwBaud, 1, 0, true);//配置串口
    DevUart->RegHandler(pstInit->byUartSn, BTRxHandler);//配置接收中断回调

#if(CONFIG_BT_DEBUG)
    Debug(false,0);
#endif

    for(__IO int i = 0; i < 0xfffff; i ++) {;}

    return &s_Dev;
}
#endif