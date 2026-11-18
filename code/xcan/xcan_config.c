/*******************************************************************************
文件名称：   xcan_config.c
文件目录：   $PROJ_DIR$/../xcanlib/
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2018.09.15
程序用途：   xcan协议,初始化

版权声明：   Copyright urit Co.,Ltd.All Right Reserved
2018.09.15
修改人：胥强
修改内容：建立

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devinit.h"
#include "xcan.h"

//#include "iosrc_main.h"
//#include "iosrc_liq.h"
//#include "iosrc_pmt.h"
//#include "protif_net.h"
#include  "iosrc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
_c7_src_dev g_stC7;
/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/


//static u8 A2CanRxBuff[sizeof(_a2_src_dev)];
//static u8 A2CanShadow[sizeof(_a2_src_dev)];
//static u8 A2CanShadowBuff[sizeof(_a2_src_dev)];
//static u8 A2CanShadowBuff2[sizeof(_a2_src_dev)];

//static u8 MemC7[1024];
static u8 MemC7CanBuff[sizeof(_c7_src_dev)];
static u8 MemC7CanShadow[sizeof(_c7_src_dev)];
static u8 MemC7CanShadowBuff[sizeof(_c7_src_dev)];
static u8 MemC7CanShadowBuff2[sizeof(_c7_src_dev)];

/*******************************************************************************
函数功能：
*   @ CAN 接口
参数描述：
*   @ 无
返回值描述：
    串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
int XcanConfigAddFilter1 (u32 dwDevID, u16 wFilterID,u32 dwLen)
{
    return DevCan->AddFilter(0, wFilterID,dwLen);
}

bool XcanConfigIsSendFull1 (u32 dwDevID ,unsigned int dwPort)
{
    return (DevCan->GetEmpNum(0))? false:true;
}

int XcanConfigSend1 (unsigned int dwDevId,unsigned int dwPort, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen)
{
    return DevCan->SendOne(0, dwId, pbyIn, dwLen);
}

int XcanConfigReceive1 (void (*pfHandler) (unsigned int dwDevId, unsigned int dwId, unsigned char *pbyIn, unsigned int byLen))
{
    return DevCan->SetRxHandler(0, pfHandler);
}

int XcanConfigSendNotice1 (u32 dwDevID, void (*pfHandler) (unsigned int dwDev))
{
    return DevCan->SetTxHandler(0, pfHandler);
}

int XcanConfigSendNoticeDm1 (u32 dwDevID)
{
    return DevCan->SendIrpOff(0);
}

int XcanConfigSendNoticeUp1 (u32 dwDevID)
{
    return DevCan->SendIrpOn(0);
}

static u32 sys_now(void)
{
    return xTaskGetTickCount ();
}

unsigned int ulTime1msCx = 0;

void Timer1ms_Handler(unsigned int dwPare)
{
    ulTime1msCx++;
}

//定时器初始化
void TimerInit(void)
{
    #define TIMER_2 1

    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    DevRccGetClocksFreq(&stRccFreq);

    dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000) - 1;
    //设置定时器，为1M的时钟

    DevTim->Config(TIMER_2, dwPre, 0);

    DevTim->TimEvent(TIMER_2, 1000, Timer1ms_Handler, 0);

}

static u32 sys_timer_now(void)
{
    return ulTime1msCx;
}

/*******************************************************************************
函数功能：
*   @ 以太网接口接口
参数描述：
*   @ 无
返回值描述：
    串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
//int XcanConfigSendNet ( unsigned int dwDevId, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen)
//{
//    vTaskDelay(2);
//    return ProtifNetSend(dwId, pbyIn, dwLen, true, NULL);
//}
//
//int XcanConfigReceiveNet (void (*pfHandler) (unsigned int dwDevId, unsigned int dwId, unsigned char *pbyIn, unsigned int dwLen))
//{
//    return ProtifNetRegRxHanlder(pfHandler);
//}
//
//void XcanConfigErrNet (unsigned int dwDevId, unsigned short int wIoUUID, u8 byFrame)
//{
//    ProtifNetReLink();
//}

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
void XcanProtocolInit(void)
{
    u32 dwUUID = 0;
    stCommChnTdf stInit;

    DevXcan = XcanInit();
    DevXcan->SetMyDevID(0x1607);//0x70c7
    // can1 参数配置
    DevCan->SetHwID(0, 0xf0);
    stInit.dwComChnID               = 0xf0;
    stInit.dwFrameDataLen           = 8;
    stInit.pfAddFilter              = XcanConfigAddFilter1;
    stInit.pfIsSendFull             = XcanConfigIsSendFull1;
    stInit.pfSend                   = XcanConfigSend1;

    stInit.pfReceive                = XcanConfigReceive1;
    stInit.pfSendEmptyNotice        = XcanConfigSendNotice1;
    stInit.pfSendEmptyNoticeDown    = XcanConfigSendNoticeDm1;
    stInit.pfSendEmptyNoticeUp      = XcanConfigSendNoticeUp1;
    stInit.Err                      = NULL;

    // 添加液路控制板IO资源
    dwUUID = DevXcan->AddTxPort(&stInit, 0x800C, 0xC0C7, ( u8* )&g_stC7, MemC7CanBuff,sizeof(_c7_src_dev));
    DevXcan->ServicePortInit(dwUUID, MemC7CanShadow, MemC7CanShadowBuff,MemC7CanShadowBuff2);
//    DevXcan->SynSet(dwUUID, true, 0, sizeof(g_stC7.stIn),sizeof(g_stC7),sizeof(g_stC7));
    DevXcan->SynSet(dwUUID, true, 0, sizeof(g_stC7.stIn),sizeof(g_stC7) - sizeof(g_stC7.stDownload),sizeof(g_stC7));
    DevXcan->SetTimPort(sys_now);
}

/*******************************************************************************
函数功能：
*   @ 初始化xcan协议设备
参数描述：
*   @ bootloader不跑系统时用
返回值描述：
    串口设备提供的操作接口
说明：
*   @ 掉用此接口完成设备的初始化
*******************************************************************************/
void bootXcanProtocolInit(void)
{
    u32 dwUUID = 0;
    stCommChnTdf stInit;

    DevXcan = XcanInit();
    DevXcan->SetMyDevID(0x1607);
    // can1 参数配置
    DevCan->SetHwID(0, 0xf0);
    stInit.dwComChnID               = 0xf0;
    stInit.dwFrameDataLen           = 8;
    stInit.pfAddFilter              = XcanConfigAddFilter1;
    stInit.pfIsSendFull             = XcanConfigIsSendFull1;
    stInit.pfSend                   = XcanConfigSend1;

    stInit.pfReceive                = XcanConfigReceive1;
    stInit.pfSendEmptyNotice        = XcanConfigSendNotice1;
    stInit.pfSendEmptyNoticeDown    = XcanConfigSendNoticeDm1;
    stInit.pfSendEmptyNoticeUp      = XcanConfigSendNoticeUp1;
    stInit.Err                      = NULL;

    // 添加液路控制板IO资源
    dwUUID = DevXcan->AddTxPort(&stInit, 0x800C, 0xC0C7, ( u8* )&g_stC7, MemC7CanBuff,sizeof(_c7_src_dev));
    DevXcan->ServicePortInit(dwUUID, MemC7CanShadow, MemC7CanShadowBuff,MemC7CanShadowBuff2);
    DevXcan->SynSet(dwUUID, true, 0, sizeof(g_stC7),sizeof(g_stC7),sizeof(g_stC7));

    TimerInit();
    DevXcan->SetTimPort(sys_timer_now);
}

/*******************************************************************************
函数功能：
*   @ xcan协议的应用进程
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void XcanTask(void *pv)
{
    for(;;) {
        DevXcan->Service();
        vTaskDelay(5);
    }
}

/*******************************************************************************
函数功能：
*   @ 创建xcan服务进程
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void XcanTaskInit(void)
{
    xTaskCreate( XcanTask , ( const char *const )  "XcanTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL );//tskIDLE_PRIORITY
}
