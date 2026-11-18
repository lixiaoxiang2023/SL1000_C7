/*******************************************************************************
文件名称：   xcan.h
目录：       $PROJ_DIR$/../code/xcanlib/src
项目名称：   事业一部
创建者：     胥强
创建日期：   20130725
程序用途：   基于IO数据控制的can数据传输协议

版权声明：
Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
20130721：
第一次由胥强创建初始程序。
2018.10.11
由秦志福增加win32的支持
*******************************************************************************/
#ifndef __XCAN_H__
#define __XCAN_H__

/*******************************************************************************
Include file
*******************************************************************************/

/*******************************************************************************
全局宏变量定义
*******************************************************************************/
#undef NULL
#if defined(__cplusplus)
    #define NULL 0
#else
    #define NULL ((void *)0)
#endif

#undef  CALLBACK
#define CALLBACK

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    /*
    * 为什么这样做：
    *   1、xcan已经与can没有什么关系，只是名称是can
    *   2、xcan主要是用于基于内存交换或者内存读写的一种控制方式
    *   3、xcan可以支持多种数据传输设备，包括can、串口、以太网等
    *   4、xcan协议需要提供统一的接口便于设备的通用
    *   5、基于以上目的，抽象出一下的设备交互方法，各个设备在满足xcan协议的时候需要提供以下的接口
    */

    /*
    *   设备的标识符，每个设备唯一，用于协议栈明确数据的来源，因为一个设备可能注册多个IO资源，
    *   所以不能和IO资源相混合,主要是对有发送事件触发的设备需要。
    */
    unsigned int dwComChnID;

    /*
    *   根据协议定义，设备支持的单帧数据长度。
    */
    unsigned int dwFrameDataLen;
    /*
    *   数据接收注册接口，协议栈将调用此接口里面的回调参数，将协议栈的接收核心注册到设备的数据接收进程中
    */
    int(*pfReceive) (void(*pfHandler) (unsigned int dwDevId, unsigned int dwId, unsigned char *pbyIn, unsigned int byLen));

    /*
    *   数据发送接口
    */
    int(*pfSend) (unsigned int dwCommChnID, unsigned int dwPort, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen);

    /*
    *   数据发送是否可能，是否设备的发送FIFO空闲
    */
    bool(*pfIsSendFull) (unsigned int dwCommChnID, unsigned int dwPort);

    /*
    *   数据发送完成后，设备可以主动触发继续发送的接口，用于高速的数据发送，
    *   如果不注册此接口，数据的发送将使用协议栈进行发送，速度可能会慢一些
    */
    int(*pfSendEmptyNotice) (unsigned int dwCommChnID, void(*pfHandler) (unsigned int dwDevID));

    /*
    *   在协议栈无数据发送时，使用此接口关闭印件主动触发发送的功能
    */
    int(*pfSendEmptyNoticeUp) (unsigned int dwComChnID);

    /*
    *   在协议栈需要数据发送时，使用此接口触发主动触发发送的功能
    */
    int(*pfSendEmptyNoticeDown) (unsigned int dwComChnID);

    /*
    *   设定硬件的接收规则，不是注册的IO数据，不需要提交到协议栈。
    */
    int(*pfAddFilter) (unsigned int dwComChnID, unsigned short int wFilterID, unsigned int dwFilterLen);

    /*
    *   硬件连接丢失错误。
    */
    void(*Err) (unsigned int dwComChnID, unsigned int dwPort, unsigned int dwRemoteDevID, unsigned short int wIoUUID, unsigned char byFrame);
} stCommChnTdf;

typedef void(CALLBACK* XcanCoreInPortCb) (unsigned int dwDevId, unsigned int dwId, unsigned char *pbyIn, unsigned int byLen);

typedef struct {
    /*******************************************************************************
    函数功能：
    *   @ 向xcan协议增加可以传输数据的设备，包括其设备的IO资源，数据缓冲地址等。
    *   @ 主机模式
    参数描述：
    *	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
    *   @ short unsigned int wMyDevID ：本资源所属的本地设备ID
    *   @ short unsigned int wRemoteDevID ：本资源所属的远程设备ID
    *   @ unsigned short int wIoUUID ： 本系统内资源的唯一ID定义
    *   @ unsigned char *pbyIoSrc ： 注册设备的IO资源地址，在接受到正确数据后协议会自动写入，不用注册为NULL
    *   @ unsigned char *pbyBuff ： 协议在接受数据时需要的数据缓存
    *   @ unsigned short int wMaxLen ： IO口资源能够操作的最大数据长度
    返回值描述：
    *   @ -1：增加设备失败
    *   @  0：增加设备成功
    说明：
    *   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
    *   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
    *   @ 2020.11.14 增加本地设备ID参数
    *******************************************************************************/
    int  (*AddRxPort) (stCommChnTdf *pstCommChn, short unsigned int wRemoteDevID, unsigned short int wIoUUID, unsigned char *pbyIoSrc, unsigned char *pbyBuff, unsigned int dwMaxLen);

    /*******************************************************************************
    函数功能：
    *   @ 向xcan协议增加可以传输数据的设备，包括其设备的IO资源，数据缓冲地址等。
    *   @ 客户端模式
    参数描述：
    *	@ stXcanSportDevTdf *pstCommChn：与远程设备通信的方法
    *   @ short unsigned int wMyDevID ：本资源所属的本地设备ID
    *   @ short unsigned int wRemoteDevID ：本资源所属的远程设备ID
    *   @ unsigned short int wIoUUID ： 本系统内资源的唯一ID定义
    *   @ unsigned char *pbyIoSrc ： 注册设备的IO资源地址，在接受到正确数据后协议会自动写入，不用注册为NULL
    *   @ unsigned char *pbyBuff ： 协议在接受数据时需要的数据缓存
    *   @ unsigned short int wMaxLen ： IO口资源能够操作的最大数据长度
    返回值描述：
    *   @ -1：增加设备失败
    *   @  0：增加设备成功
    说明：
    *   @ 若此设备需要同另外的设备进行通信，必须使用此方法进行设备的注册，此函数将设定can硬件
    *   @ 的过滤器，同时在数据接收时将根据注册的信息进行数据自动处理
    *   @ 2020.11.14 增加本地设备ID参数
    *******************************************************************************/
    int  (*AddTxPort) (stCommChnTdf *pstCommChn, short unsigned int wRemoteDevID, unsigned short int wIoUUID, unsigned char *pbyIoSrc, unsigned char *pbyBuff, unsigned int dwMaxLen);

    int  (*RemovePort) (unsigned int dwUUID);

    int  (*Read) (unsigned int dwUUID, short unsigned int bySrAdr, unsigned int dwLen, unsigned int dwInTimOut,  bool blWaitAck);
    int  (*Write) (unsigned int dwUUID, short unsigned int bySrAdr, unsigned int dwLen, unsigned char *pbyDatIn, unsigned int dwInTimOut,  bool blWaitAck);

    int  (*ServicePortInit) (unsigned int dwUUID, unsigned char *pbyShadow, unsigned char *pbyShadowBuff, unsigned char *pbyIoCache);
    void (*SetTimPort) (unsigned int (*pfHandler)(void));
    void (*Service) (void);

    /*******************************************************************************
    函数功能：
    *   @ 主动开启远程自动同步功能
    参数描述：
    *   @ unsigned int dwUUID 资源ID
    *   @ bool  blSetOn  true 設置默认状态，false 不改变状态
    *   @ short unsigned int wSyncStart 开始同步数据地址
    *   @ short unsigned int wSyncEnd   同步结束地址
    *   @ short unsigned int wIoSyncEnd IO开始的地址, 包括in和out的数据总和，主要是去除io最后的用于升级的数据区
    *   @ short unsigned int wIoEnd     IO结束的地址，所有的io区域大小
    返回值描述：
    *   @ 无
    说明：
    *   @ 2020.11.30 增加IO范围的设定，用于指定IO的同步范围，主要用于对连接成功后的数据恢复功能
    *******************************************************************************/
    int  (*SynSet) (unsigned int dwUUID, bool blSetOn, short unsigned int wSyncStart, short unsigned int wSyncEnd, short unsigned int wIoSyncEnd, short unsigned int wIoEnd);
    int  (*SynSetDown) (unsigned int dwUUID);
    int  (*AddGroup) (unsigned int dwUUID1, unsigned int dwUUID2);
    bool (*SynOutLink) (unsigned int dwUUID);
    bool (*SynInLink) (unsigned int dwUUID);

    int  (*MustToRemote) (unsigned int dwUUID, unsigned int dwPos, unsigned char byDat, void(*pfDelayHandler)(void));
    int  (*RegReceiveNotice) (unsigned int dwUUID, void(*pfHandler)(unsigned int dwUUID));
    int  (*UnRegReceiveNotice) (unsigned int dwUUID);
    int  (*GetCommSta) (unsigned int dwUUID, unsigned short int *pwTxSN, \
        unsigned short int *pwTxID, unsigned short int *pwRxSN, unsigned short int *pwRxID, \
        unsigned int *pwTrsLost);

    int  (*RegReadWriteNotice) (unsigned int dwUUID, void (*pfHandler)(unsigned int dwUUID, unsigned int dwOp));
    int  (*UnRegReadWriteNotice) (unsigned int dwUUID);

    bool (*IsRdBusy) (unsigned int dwUUID);
    bool (*IsWrBusy) (unsigned int dwUUID);

   /*
    * 向内核设置本机的ID，重要
   */
    void(*SetMyDevID) (short unsigned int wMyDevID);

   /*
    * 获取内核的数据输入接口。
   */
    XcanCoreInPortCb (*GetCoreInPort)(void);

    /*
     * 向内核增加IO资源，不用绑定通信设备
    */
    int  (*AddIo) (short unsigned int wRemoteDevID, unsigned int dwComChnID, short unsigned int wIoUUID,
                   unsigned char *pbyIoSrc, unsigned char *pbyBuff,
                   unsigned int dwMaxLen, bool blIsRxIO);

    /*
     * 向内核绑定通信设备
    */
    int  (*AddCommIF) (stCommChnTdf *pstCommChn);

    /*
     * 启动资源的同步功能
    */
    int  (*PortActive) (unsigned int dwUUID);

    /*
     * 关闭接口的同步功能，但资源还在内和，只影响本地不启动同步
    */
    int  (*PortDisActive) (unsigned int dwUUID);

    /*
     * 设置锁接口
    */
    void (*SetMutex) (void(*pGetMutex)(void), void(*pPutMutex)(void));

    /*
     * set time out
    */
    void (*SetRelinkTim) (unsigned int dwTim);

    /*
     * set retrs tim out
    */
    void (*SetReTrsTimOut) (unsigned int dwTim);
}DevXcanTdf;

typedef struct {
    int  (*SetMyIp) (unsigned int dwIP);
    int  (*SetMyUdpPort) (unsigned int dwUdpPort);
    int  (*SetMyTcpPort) (unsigned int dwTcpPort);
    int  (*SetLinkIp) (unsigned int dwIP);
    int  (*SetLinkTcpPort) (unsigned int dwTcpPort);
    int  (*ConfigDev) (const char *pcClass, const char *pcSubclass, const char *pcName, const char *pbyMac, \
        const char *pcSn, const char *pcIoID);

    void (*SetSendPort) (void (*pfBroadcast)(unsigned int dwPort, const char *pcInfo, unsigned int dwLen), \
        void (*pfUdp) (unsigned int dwIP, unsigned int dwPort, const char *pcInfo, unsigned int dwLen));
    void (*SetEventPort) ( void (*pfNotice) (const char *pcIn, unsigned int dwLen), \
        void  (*pfLink) (unsigned int dwIP, unsigned int dwPort), void  (*pfUnlink) (void));

    int  (*Scan) (void);
    int  (*Up) (unsigned int dwSn);
    int  (*Down) (unsigned int dwSn);
    void (*In) (const char *pcIn, unsigned int dwLen);
}DevXcanLinkTdf;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/

/*******************************************************************************
函数申明
*******************************************************************************/
#if defined(__clang__) || defined(__GNUC__)
    #include <pthread.h>
    #include <sys/time.h>
    #include <unistd.h>
    #define XCAN_STATIC                         static
    #define XCAN_SYS_MUTEX                      pthread_mutex_t
    #define XCAN_SYS_MUTEX_INIT(cs)             do { pthread_mutex_init(&cs, NULL);} while(0);
    #define XCAN_SYS_ENTER_CRITICAL(cs)         do { pthread_mutex_lock(&cs);} while(0);
    #define XCAN_SYS_EXIT_CRITICAL(cs)          do { pthread_mutex_unlock(&cs);} while(0);
    #define XCAN_EXPORTS_API
    #define ICVP_API
#elif defined(_MSC_VER)
    #include <Windows.h>
    #include <process.h>
    #undef	AddPort
    #define XCAN_STATIC                         static
    #define XCAN_SYS_MUTEX                      CRITICAL_SECTION
    #define XCAN_SYS_MUTEX_INIT(cs)             do { InitializeCriticalSection(&cs);} while(0);
    #define XCAN_SYS_ENTER_CRITICAL(cs)         do { EnterCriticalSection(&cs);} while(0);
    #define XCAN_SYS_EXIT_CRITICAL(cs)          do { LeaveCriticalSection(&cs);} while(0);
    #define XCAN_EXPORTS_API					__declspec(dllexport)
    #define ICVP_API                            __declspec(dllexport)
#else
    #define XCAN_STATIC                         static
    #define XCAN_SYS_MUTEX                      unsigned int
    #define XCAN_SYS_MUTEX_INIT(cs)
    #define XCAN_SYS_ENTER_CRITICAL(cs)
    #define XCAN_SYS_EXIT_CRITICAL(cs)
    #define XCAN_EXPORTS_API
    #define ICVP_API
#endif

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */
    ICVP_API const DevXcanTdf *XcanInit(void);
    ICVP_API const DevXcanLinkTdf *XcanLinkInit(void);
#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif
