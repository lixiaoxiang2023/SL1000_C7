/*****************************************************************************
文件名称：TaskNove.c
文件目录：$PROJ_DIR$/usr/
项目名称：
创建者：
创建日期：
程序用途：
系统参数管理

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息

******************************************************************************/

/******************************************************************************
Include files头文件
******************************************************************************/
#include "devinit.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "sensor.h"
#include "TaskMoveStrip.h"
#include "SysData.h"
#include "motorctrl.h"
#include "iosrc.h"
#include "functrl.h"
//#include "QRCodeMod.h"
#include "bootMain_Cfg.h"
#include "TaskTempModule.h"
#include "stm32f10x_flash.h"
#include "TaskWasteDetect.h"
#include "clients/clients_def.h"
#include "ver_type.h"
#include "ver_def.h"
#include "XH711.h"
/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
#define     SubTaskNO                  5

xQueueHandle           g_Queue_SubTask[SubTaskNO];

/******************************************************************************
Global Variables Definition全局变量定义
******************************************************************************/
bool bTareCal = false;
volatile bool bCmdReturn = true;   /* AI-FIX N-11: MoveTask 与 SubTask1 共享, 必须 volatile */
uint16_t wDebugBk = 0;
bool bDebugSta = false;
//uint16_t wX_Offset = 0;
//uint16_t wY_Offset = 0;
static uint16_t wMpuData_X = 0;
static uint16_t wMpuData_Y = 0;

int Ageing = 0;
/******************************************************************************
外部全局变量申明
******************************************************************************/
__no_init static unsigned long firmware_update_flag@0x20000000;
/******************************************************************************
内部数据结构与数据类型定义
******************************************************************************/

#if(CONFIG_MPU6050)
    const DevMpu6050Tdf *DevMpu6050;
#endif

typedef struct
{
    unsigned char Cmd;
    void (*Task)(void);
    unsigned char cTaskNum;
}CtrlCmdTdf;


CtrlCmdTdf  OldCmd[SubTaskNO] =
{
    {0XFF,NULL,0},
    {0XFF,NULL,0},
    {0XFF,NULL,0},
    {0XFF,NULL,0},
    {0XFF,NULL,0},
};
/******************************************************************************
AI-FIX N-11 : SubTask1 槽位命令快照
  缺陷: SubTask1 收尾时用 g_stC7.stIn.byCmd / byCmdCnt 判断并回灌, 二者均为
        实时值。若该槽位执行期间上位机已下发新命令, stIn 已被覆盖, 就会把
        新命令的计数回灌进 stoutCmd.byCmdCnt, 使 MoveTask 门闸
        (stoutCmd.byCmdCnt != stIn.byCmdCnt) 永不成立, 新命令被永久丢弃:
        不投递、byCmdisBusy 不置位、动作不执行、上位机读到空闲即报完成。
        且 c71b(CAR_TO_OPC) 本在排除名单内, 因判据读实时值导致排除失效,
        反而执行了回灌, 属白名单被实时值绕过。
  修正: 投递时快照本槽位命令码与计数, 收尾只用快照, 不再读实时 stIn。
******************************************************************************/
static volatile unsigned char bySubTask1Cmd    = 0xFF;
static volatile unsigned char bySubTask1CmdCnt = 0;
static volatile unsigned char byCurCmd    = 0xFF;   /* N-11b: 门闸内原子快照-本拍命令码 */
static volatile unsigned char byCurCmdCnt = 0;      /* N-11b: 门闸内原子快照-本拍命令计数 */
/******************************************************************************
内部全局变量定义
******************************************************************************/
xTaskHandle    g_xFunCtrlTaskHandle;
/******************************************************************************
内部函数申明
******************************************************************************/
void MoveTest(int loop);
static void RefreshParameter(uint16_t *Data);
void MoveTask(void *pvParameters);
void MoveModuleTask(void);
void GetVersion(void);
void Reset(void);
void C7TodropStep(void);
void C7dropMoveStep(void);
void M1_Rest_To_Drop(void);
//void C7M3UpStep(void);
void C7M2Run(void);
//void C7M3Run(void);
void OpenFun(void);
void CloseFun(void);
void Debug(void);
void GetQRCode(void);
void GetOp(void);
//void DropOil(void);
void Renovate(void);
void LoadingParameters(void);
void SaveData(void);
void SubTask1(void *pvParameters);
void SubTask2(void *pvParameters);
void SubTask3(void *pvParameters);
void SubTask4(void *pvParameters);
void SubTask5(void *pvParameters);
void C7M3Up(void);
void C7M3Down(void);
void C7M3DownTOWash(void);
void C7M3UpTOWash(void);
void MoveTest(int loop);
void M4_Transfer(void);
void M4_Rest();
void NULLCMD(void);
void Upgrade(void);
void PumpStart(void);
void FullOil(void);
void CheckSlidBox(void);
void M1ToOpcStep(void);
void M1OpcToDrop(void);
void OpenFunHeat(void);
void CloseFunHeat(void);
void CloseTemp(void);
//void GetUpdataFinish(void);
unsigned char GetUpdataFinish(void);
void TransferToC6(void);
void C7GetMpuData(void);
void C7SetTemp(void);
void C7SetFanTemp(void);
void ClearTare(void);
void SendDataTask(void *pvPare);
void DyeFunOpen(void);
void DyeFunClose(void);
void DyeFunDown(void);
void FanUpToDye(void);
void C7Aging(void);
#define  CommandSet   40

CtrlCmdTdf CtrlCmd[CommandSet] = {
    {CMD_C7_GETVER,              GetVersion,0} ,       //  获取版本号
    {CMD_C7_RESET,               Reset,0} ,            //  复位
    {CMD_C7_CARTODROP,           C7TodropStep,0},      //  落带位置到滴样位置
    {CMD_C7_OPEN_FAN_HEAT,       OpenFunHeat,1},       //
    {CMD_C7_TRANONCE,            C7M2Run,1},           //  传送一格
    {CMD_C7_CLOSE_FAN_HEAT,      CloseFunHeat,1},      //   M3电机转一圈，玻片送到镜检平台
    {CMD_C7_AIRDRY,              OpenFun,2},           //  打开吹干风扇
    {CMD_C7_M3DOWNTOWASH,        C7M3DownTOWash,1},    //  M3下降到冲洗位
    {CMD_C7_GETOPC,              GetOp,0},             //  获取光耦信息
    {CMD_C7_DROPOIL,             PumpStart,3},         //  滴镜油 DropOil  PumpStart
    {CMD_C7_DEBUG,               Debug,0},             //  调试
    {CMD_C7_SAVE,                SaveData,0},          //  保存参数  ， 将调试好的参数进行保存。
    {CMD_C7_REFRESH,             Renovate,0},          //  刷新参数
    {CMD_C7_LOAD,                LoadingParameters,0}, //  加载参数
    {CMD_C7_M3_UP_TO_WASH,       C7M3UpTOWash,1},      //  M3向上到染剂位置
    {CMD_C7_GET_FINISH,          NULLCMD,0},           // 获取升级完成状态
    {CMD_C7_UPDATA_START,        Upgrade,0},
    {CMD_C7_MOTO_UP,             C7M3Up,1},
    {CMD_C7_MOTO_DWON,           C7M3Down,1},
    {CMD_C7_TRANC_TO_DMP,        M4_Transfer,3},       //M4传送载玻片到C6机构
    {CMD_C7_REST_TO_DMP,         M4_Rest,3},           //M4复位到C7
    {CMD_C7_RESE_TO_DROP,        M1_Rest_To_Drop,0},   //复位到滴样，不推载玻片
    {CMD_C7_TEMP_DERAT,          DeratTemp,0},         //加热降额
    {CMD_C7_TEMP_RECOVER,        RecoverTemp,0},       //恢复制片加热
    {CMD_C7_FULL_OIL,            FullOil,0},           //充满镜油
    {CMD_C7_CHECK_SLIDBOX,       CheckSlidBox,4},      //检测载玻片盒子
    {CMD_C7_COLSEFUNPWM,         CloseFun,3},          //  关闭吹干风扇
    {CMD_C7_CAR_TO_OPC,          M1ToOpcStep,0},       //  载玻片拨到光耦检测位
    {CMD_C7_OPC_TO_DROP,         M1OpcToDrop,0},       //  载玻片拨到光耦检测位
    {CMD_C7_FULL_OIL_RE,         FullOil,0},           //充满镜油,用于流程调用，立即返回
    {CMD_C7_TEMP_CLOSE,          CloseTemp,0},         //加热降额
    {CMD_C7_TRANSFER_TO_C6,      TransferToC6,0},      //载玻片传送到C6
    {CMD_C7_GETMPUDATA,          C7GetMpuData,0},      //获取MPU数据
//    {CMD_C7_SETTEMP,             C7SetTemp,1},         //设置固化温度
    {CMD_C7_TARE,                ClearTare,0},         //去皮
    {CMD_C7_STAINFAN_OPEN,       DyeFunOpen,0},        //染色风扇打开
    {CMD_C7_STAINFAN_CLOSE,      DyeFunClose,0},       //染色风扇关闭
    {CMD_C7_M3DOWNTODRY,         DyeFunDown,1},        //M3下降到吹干位置    
    {CMD_C7_M3UPTODRY,           FanUpToDye,1},        //M3上升到染色位  
    {CMD_C7_AGING,               C7Aging,0},
    {CMD_C7_M1_STEP,           C7dropMoveStep,0},      //  落带位置到滴样位置
//    {CMD_C7_SETFANTEMP,          C7SetFanTemp,1},         //设置烘干温度  
};

//升级程序应该先关闭加热
/*******************************************************************
功能：NULL
参数：无
返回值：无
其它说明：
*******************************************************************/
void NULLCMD(void)
{

}

/*******************************************************************
功能：void ClearTare(void)
参数：无
返回值：无
其它说明：镜油称重去皮
*******************************************************************/
//void ClearTare(void)
//{
//    bTareCal = true;
//    vTaskDelay(100);
//    Get_Maopi();
//    vTaskDelay(100);
//    g_pstC7SysPare->wTareVal = Get_Maopi();
//    bTareCal = false;
 //   //SaveData();
//}

#define TARE_SAMPLE_COUNT      10      // 采样总次数
#define TARE_SAMPLE_DELAY_MS   50      // 每次采样延时（ms）

void ClearTare(void)
{
    bTareCal = true;

    int samples[TARE_SAMPLE_COUNT];
    int sum = 0;
    int maxVal = -2147483647;
    int minVal = 2147483647;

    // 采样
    for(int i = 0; i < TARE_SAMPLE_COUNT; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(TARE_SAMPLE_DELAY_MS));
        samples[i] = Get_Maopi();

        if(samples[i] > maxVal) maxVal = samples[i];
        if(samples[i] < minVal) minVal = samples[i];
    }

    // 计算去最大最小的平均值
    for(int i = 0; i < TARE_SAMPLE_COUNT; i++)
    {
        if(samples[i] != maxVal && samples[i] != minVal)
        {
            sum += samples[i];
        }
    }

    int validCount = TARE_SAMPLE_COUNT - 2; // 去掉最大最小
    if(validCount <= 0) validCount = 1;     // 防止异常
    g_pstC7SysPare->wTareVal = sum / validCount;

    bTareCal = false;
    //SaveData(); // 可根据需求保存
}



/*******************************************************************
功能：void Upgrade(void)
参数：无
返回值：无
其它说明：U盘升级
*******************************************************************/
void Upgrade(void)
{
    DevIo->Write(PC,15,0);
    g_stC7.stOut.byHandFlg = 0;
    g_stC7.stOut.byUpdataReady = 0;
    g_stC7.stOut.byUpdataFinish = 0;
    vTaskDelay(1000);
    FLASH_Unlock();
    //写入用户固件下载成功标记
    FLASH_ProgramWord(USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS, 0);//
    FLASH_Lock();
    firmware_update_flag = 0x55AA55AA;
    FLASH_Unlock();
    //写入用户固件下载成功标记
    FLASH_ProgramWord((USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS + 4), 0xAA55AA55);//
    FLASH_Lock();
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset();//软复位后接下来进入bootloader

    return;
}

/*******************************************************************
功能：创建温控任务
参数：无
返回值：无
其它说明：
*******************************************************************/
void MoveModuleTask(void)
{
    for(u8 Count = 0; Count<SubTaskNO; Count++)
    {
        g_Queue_SubTask[Count] = xQueueCreate(4, sizeof(CtrlCmdTdf *));
    }
    xTaskCreate( MoveTask ,( char const * )"MoveTask", 128, NULL, tskIDLE_PRIORITY, NULL);//tskIDLE_PRIORITY
    xTaskCreate( SubTask1 ,( char const * )"SubTask1", 128, NULL, tskIDLE_PRIORITY, NULL);//tskIDLE_PRIORITY
    xTaskCreate( SubTask2 ,( char const * )"SubTask2", 128, NULL, tskIDLE_PRIORITY, NULL);//tskIDLE_PRIORITY
    xTaskCreate( SubTask3 ,( char const * )"SubTask3", 128, NULL, tskIDLE_PRIORITY, NULL);//tskIDLE_PRIORITY
    xTaskCreate( SubTask4 ,( char const * )"SubTask4", 128, NULL, tskIDLE_PRIORITY, NULL);//tskIDLE_PRIORITY
    xTaskCreate( SubTask5 ,( char const * )"SubTask5", 128, NULL, tskIDLE_PRIORITY, NULL);//tskIDLE_PRIORITY
}

/******************************************************************************
函数名：void SubTask1(void *pvParameters)
参数描述：无
返回值描述：无
说明：子任务1
特殊算法：无
注意事项：无
******************************************************************************/
void SubTask1(void *pvParameters)
{
    CtrlCmdTdf* Queuebuf;
    while(1)
    {
        if(xQueueReceive(g_Queue_SubTask[0], &Queuebuf, portMAX_DELAY) == pdPASS)
        {
            if(Queuebuf->Task != NULL)Queuebuf->Task();
            Queuebuf->Cmd  = 0XFF;
            Queuebuf->Task = NULL;
            Queuebuf->cTaskNum = 0;
            g_stC7.stOut.byCmdisBusy[0] = 0;
            if((bySubTask1Cmd != CMD_C7_CAR_TO_OPC) && (bySubTask1Cmd != CMD_C7_OPC_TO_DROP) && (bySubTask1Cmd != CMD_C7_FULL_OIL_RE) && (bySubTask1Cmd != CMD_C7_RESE_TO_DROP))
            {//除这两个流程需立即返回，其余在调试界面完成返回
                g_stC7.stOut.stoutCmd.byCmdCnt = bySubTask1CmdCnt;
                bCmdReturn = true;
            }
        }
    }
}

/******************************************************************************
函数名：void SubTask2(void *pvParameters)
参数描述：无
返回值描述：无
说明：子任务1
特殊算法：无
注意事项：无
******************************************************************************/
void SubTask2(void *pvParameters)
{
    CtrlCmdTdf* Queuebuf;
    while(1)
    {
        if(xQueueReceive(g_Queue_SubTask[1],&Queuebuf,portMAX_DELAY) == pdPASS)
        {
            if(Queuebuf->Task != NULL) Queuebuf->Task();
            Queuebuf->Cmd  = 0XFF;
            Queuebuf->Task = NULL;
            g_stC7.stOut.byCmdisBusy[1] = 0;
        }
    }
}

/******************************************************************************
函数名：void SubTask3(void *pvParameters)
参数描述：无
返回值描述：无
说明：子任务1
特殊算法：无
注意事项：无
******************************************************************************/
void SubTask3(void *pvParameters)
{
    CtrlCmdTdf* Queuebuf;
    while(1)
    {
        if(xQueueReceive(g_Queue_SubTask[2],&Queuebuf,portMAX_DELAY) == pdPASS)
        {
            if(Queuebuf->Task != NULL) Queuebuf->Task();
            Queuebuf->Cmd  = 0XFF;
            Queuebuf->Task = NULL;
            g_stC7.stOut.byCmdisBusy[2] = 0;
        }
    }
}

/******************************************************************************
函数名：void SubTask4(void *pvParameters)
参数描述：无
返回值描述：无
说明：子任务1
特殊算法：无
注意事项：无
******************************************************************************/
void SubTask4(void *pvParameters)
{
    CtrlCmdTdf* Queuebuf;
    while(1)
    {
        if(xQueueReceive(g_Queue_SubTask[3],&Queuebuf,portMAX_DELAY) == pdPASS)
        {
            if(Queuebuf->Task != NULL) Queuebuf->Task();
            Queuebuf->Cmd  = 0XFF;
            Queuebuf->Task = NULL;
            g_stC7.stOut.byCmdisBusy[3] = 0;
        }
    }
}

/******************************************************************************
函数名：void SubTask5(void *pvParameters)
参数描述：无
返回值描述：无
说明：子任务1
特殊算法：无
注意事项：无
******************************************************************************/
void SubTask5(void *pvParameters)
{
    CtrlCmdTdf* Queuebuf;
    while(1)
    {
        if(xQueueReceive(g_Queue_SubTask[4],&Queuebuf,portMAX_DELAY) == pdPASS)
        {
            if(Queuebuf->Task != NULL) Queuebuf->Task();
            Queuebuf->Cmd  = 0XFF;
            Queuebuf->Task = NULL;
            g_stC7.stOut.byCmdisBusy[4] = 0;
        }
    }
}

/******************************************************************************
函数名：void SubTaskCtrl(void)
参数描述：无
返回值描述：无
说明：任务1
特殊算法：无
注意事项：无
******************************************************************************/
void SubTaskCtrl(u16  byCmd)
{
    CtrlCmdTdf* pCtrlCmd;
    while((OldCmd[0].Cmd == byCmd) || (OldCmd[1].Cmd == byCmd) || (OldCmd[2].Cmd == byCmd)
          || (OldCmd[3].Cmd == byCmd) || (OldCmd[4].Cmd == byCmd))
    {
        vTaskDelay(10);
    };

    for(u16 Count = 0; Count < CommandSet; Count++)
    {
        if(CtrlCmd[Count].Cmd == byCmd)
        {
            if(CtrlCmd[Count].cTaskNum == 0)
            {
                if(OldCmd[0].Cmd == 0xff)
                {
                    g_stC7.stOut.byCmdisBusy[0] = 1;
                    bySubTask1Cmd    = (unsigned char)byCmd;
                    bySubTask1CmdCnt = byCurCmdCnt;
                    memcpy(&OldCmd[0], &CtrlCmd[Count], sizeof(OldCmd[0]));
                    pCtrlCmd = &OldCmd[0];
                    xQueueSend(g_Queue_SubTask[0], &pCtrlCmd, 0);
                }
                else
                {
                    /* N-11b: 槽位被占用(前一条 cTaskNum==0 命令仍在执行)而未能投递时
                       也必须重新开门。否则 bCmdReturn 停在 false, 门闸永久关闭,
                       后续所有命令被丢弃(与 N-01 同族的门闸锁死)。 */
                    bCmdReturn = true;
                }
                break;
            }
            else if(CtrlCmd[Count].cTaskNum == 1)
            {

                if(OldCmd[1].Cmd == 0xff)
                {
                    g_stC7.stOut.byCmdisBusy[1] = 1;
                    memcpy(&OldCmd[1], &CtrlCmd[Count], sizeof(OldCmd[1]));
                    pCtrlCmd = &OldCmd[1];
                    xQueueSend(g_Queue_SubTask[1], &pCtrlCmd, 0);
                }
                break;
            }
            else if(CtrlCmd[Count].cTaskNum == 2)
            {
                if(OldCmd[2].Cmd == 0xff)
                {
                    g_stC7.stOut.byCmdisBusy[2] = 1;
                    memcpy(&OldCmd[2], &CtrlCmd[Count], sizeof(OldCmd[2]));
                    pCtrlCmd = &OldCmd[2];
                    xQueueSend(g_Queue_SubTask[2], &pCtrlCmd, 0);
                }
                break;
            }
            else if(CtrlCmd[Count].cTaskNum == 3)
            {
                if(OldCmd[3].Cmd == 0xff)
                {
                    g_stC7.stOut.byCmdisBusy[3] = 1;
                    memcpy(&OldCmd[3], &CtrlCmd[Count], sizeof(OldCmd[3]));
                    pCtrlCmd = &OldCmd[3];
                    xQueueSend(g_Queue_SubTask[3], &pCtrlCmd, 0);
                }
                break;
            }
            else if(CtrlCmd[Count].cTaskNum == 4)
            {
                if(OldCmd[4].Cmd == 0xff)
                {
                    g_stC7.stOut.byCmdisBusy[4] = 1;
                    memcpy(&OldCmd[4], &CtrlCmd[Count], sizeof(OldCmd[4]));
                    pCtrlCmd = &OldCmd[4];
                    xQueueSend(g_Queue_SubTask[4], &pCtrlCmd, 0);
                }
                break;
            }
            else
            {
                //系统忙
                break;
            }
        }
    }
}

/******************************************************************************
函数名：void MoveTask(void *pvParameters)
参数描述：无
返回值描述：
说明：运动控制任务
特殊算法：无
注意事项：无
******************************************************************************/
void MoveTask(void *pvParameters)
{
    bool bDoCmd = false;
    //int test = 0;
    RecoverTemp();
    vTaskDelay(3000);
    g_stC7.stOut.byDropOpc = 0;
    g_stC7.stOut.stVer.dwArch = c_dwArch;
    g_stC7.stOut.stVer.dwVer = c_dwVer;
    g_stC7.stOut.stVer.dwVerDate = c_dwVerDate ;
    g_stC7.stOut.stVer.dwHwVer = c_dwVerHw;
    g_stC7.stOut.stVer.dwHwVerDate = c_dwVerHwDate;
    g_stC7.stOut.stoutCmd.byCmdCnt  = g_stC7.stIn.byCmdCnt;
    g_stC7.stOut.byUpdataReady = 0;
    g_stC7.stOut.byUpdataFinish = GetUpdataFinish();
//    Get_Maopi();
//    vTaskDelay(2000);
//    Get_Maopi();
    while(1)
    {
        //Get_Weight();
//        ACCEL_X = DevMpu6050->MPU6050_Get_Data(0,ACCEL_XOUT_H);
//        ACCEL_Y = DevMpu6050->MPU6050_Get_Data(0,ACCEL_YOUT_H);
//        ACCEL_Z = DevMpu6050->MPU6050_Get_Data(0,ACCEL_ZOUT_H);
//        TEMP = DevMpu6050->MPU6050_Get_Data(0,TEMP_OUT_H) / 340.0 + 36.53;
//        GYRO_X = DevMpu6050->MPU6050_Get_Data(0,GYRO_XOUT_H);
//        GYRO_Y = DevMpu6050->MPU6050_Get_Data(0,GYRO_YOUT_H);
//        GYRO_Z = DevMpu6050->MPU6050_Get_Data(0,GYRO_ZOUT_H);
        vTaskDelay(2);   /* AI-FIX N-11: 10ms->2ms, 缩小新命令写入与门闸检查的竞态窗口 */
        /* N-11b: 门闸前一次性原子快照, 门闸判断与后续处理全部复用同一份快照,
           避免 byCmd 与 byCmdCnt 分次读取造成 torn-read / 码与计数不自洽 */
        taskENTER_CRITICAL();
        byCurCmd    = g_stC7.stIn.byCmd;
        byCurCmdCnt = g_stC7.stIn.byCmdCnt;
        taskEXIT_CRITICAL();
        if((g_stC7.stOut.stoutCmd.byCmdCnt != byCurCmdCnt) && (bCmdReturn == true))
        {
            bCmdReturn = false;
            bDoCmd = false;   /* 每条命令处理前复位，避免上次残留影响未知命令判定 */
            g_stC7.stOut.stoutCmd.byCmd = byCurCmd;

            for(int Count = 0;Count < sizeof(CtrlCmd)/sizeof(CtrlCmd[0]); Count++)
            {
                if(byCurCmd == CtrlCmd[Count].Cmd)
                {
                    bDoCmd = true;
                    SubTaskCtrl(byCurCmd);
                    if((CtrlCmd[Count].cTaskNum != 0) || (byCurCmd == CMD_C7_CAR_TO_OPC) 
                    || (byCurCmd == CMD_C7_OPC_TO_DROP) || (byCurCmd == CMD_C7_FULL_OIL_RE) 
                    || (byCurCmd == CMD_C7_RESE_TO_DROP))
                    {
                        g_stC7.stOut.stoutCmd.byCmdCnt = byCurCmdCnt;
                        bCmdReturn = true;
                    }
                }
            }
            if(bDoCmd == false)
            {//非定义命令
                 g_stC7.stOut.stoutCmd.byCmdCnt = byCurCmdCnt;
                 bCmdReturn = true;   /* 未知命令也须开门，否则后续命令被永久丢弃 */
            }
        }
    }
}

/******************************************************************************
函数名：void MoveTest(int loop)
参数描述：无
返回值描述：无
说明：运动控制任务
特殊算法：无
注意事项：无
******************************************************************************/
void MoveTest(int loop)
{
    for (int Count=0; Count < loop; Count++)
    {
        MotorCtrl.M1_To_Drop(g_pstC7SysPare->wM1ToDripStep, 0);
        GetOp();
        MotorCtrl.M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
        MotorCtrl.M3_Up(g_pstC7SysPare->wM3UpStep);
        FunCtrl.SetFunPWM(100);
        vTaskDelay(1000);
        FunCtrl.SetFunPWM(0);
        MotorCtrl.M3_Down(g_pstC7SysPare->wM3UpStep);
    }
}

/******************************************************************************
函数名：void RefreshParameter(u16 *Data)
参数描述： *Data，数据输出指针
返回值描述：无
说明：刷新参数
特殊算法：无
注意事项：无
******************************************************************************/
static void RefreshParameter(uint16_t *Data)
{
    g_stC7.stOut.bySwapOutLen = 13;
    Data[0] = g_pstC7SysPare->wM1ToDripStep;
    Data[1] = g_pstC7SysPare->wM3UpStep;
    Data[2] = g_pstC7SysPare->wM2_MoveOne;
    Data[3] = g_pstC7SysPare->OilVolume;
    Data[4] = g_pstC7SysPare->wM1RestToDripStep;
    Data[5] = g_pstC7SysPare->wM3DownToWashStep;
    Data[6] = g_pstC7SysPare->wMpuX;
    Data[7] = g_pstC7SysPare->wMpuY;
    Data[8] = g_pstC7SysPare->wM3DownToFanStep;
    Data[9] = g_pstC7SysPare->wHeatTemp/10;
    Data[10] = g_pstC7SysPare->wHeatFanTemp/10;
    Data[11] = g_pstC7SysPare->wM4Steps;
    Data[12] = g_pstC7SysPare->wM2MoveSteps;
}

/*******************************************************************
功能：获取版本号
参数：无
返回值：无
其它说明：
*******************************************************************/
void GetVersion(void)
{
    //memset(&g_stC7.stOut,0,sizeof(g_stC7.stOut));
    vTaskDelay(100);
    g_stC7.stOut.stVer.dwArch = c_dwArch;
    g_stC7.stOut.stVer.dwVer = c_dwVer;
    g_stC7.stOut.stVer.dwVerDate = c_dwVerDate ;
    g_stC7.stOut.stVer.dwHwVer = c_dwVerHw;
    g_stC7.stOut.stVer.dwHwVerDate = c_dwVerHwDate;
}

/*******************************************************************
功能：复位
参数：无
返回值：无
其它说明：
*******************************************************************/
void Reset(void)
{
    g_stC7.stOut.stoutCmd.byErr = 0;
    bDebugSta = false;
    GetVersion();
    MotorCtrl.ResetAll(g_pstC7SysPare->wM1RestToDripStep);
    if( DevIo->Read(PD,6) == 0)
    {
        vTaskDelay(200);
        if(DevIo->Read(PD,6) == 0)
        {
            MotorCtrl.C7SetErr(ERR_C7_BOX);
            g_stC7.stOut.byCardBox = 0;//无盒子
        }
    }
}

/*******************************************************************
功能：光耦位置到滴样位置
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7TodropStep(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M1_To_Drop(g_pstC7SysPare->wM1ToDripStep,0);
    }
}
/*******************************************************************
功能：从滴样位置在移动一小步
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7dropMoveStep(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M2_MoveStep(g_pstC7SysPare->wM2MoveSteps);
    }
}

/*******************************************************************
功能：光耦位置到滴样位置
参数：无
返回值：无
其它说明：
*******************************************************************/
void M1OpcToDrop(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M1_Opc_To_Drop(g_pstC7SysPare->wM1ToDripStep);
    }
}

/*******************************************************************
功能：试纸盒到滴样光耦检测位置
参数：无
返回值：无
其它说明：
*******************************************************************/
void M1ToOpcStep(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M1_To_Opc(g_pstC7SysPare->wM1ToDripStep,(uint8_t)g_stC7.stIn.wParm[0]);
    }
}

/*******************************************************************
功能：M2复位到滴样位置
参数：无
返回值：无
其它说明：
*******************************************************************/
void M1_Rest_To_Drop(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M1_Rest_To_Drop(g_pstC7SysPare->wM1RestToDripStep, 0);
    }
}

/*******************************************************************
功能：传送一格
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7M2Run(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
        if(DevIo->Read(PD,3) == 0x00)
        {
            g_stC7.stOut.bySlideCard ++ ;
        }
        if(DevIo->Read(PD,4) == 0x00)
        {
            g_stC7.stOut.byDropOilCard ++ ;
        }
    }
}

/*******************************************************************
功能：开风扇
参数：无
返回值：无
其它说明：
*******************************************************************/
void OpenFun(void)
{
    uint8_t byErrLevel = 0;
//    uint16_t wTempFan = 0;
//    uint16_t wTempFan1 = 0;
//    uint16_t wTempDif = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        u8 DutyCycle = 80;
        FunCtrl.SetFunPWM(DutyCycle);
//        vTaskDelay(10000);
//        vTaskDelay(10000);
//        vTaskDelay(10000);
//        vTaskDelay(8000);
//        FunCtrl.SetFunPWM(0);
    }
}

/*******************************************************************
功能：关闭风扇
参数：无
返回值：无
其它说明：
*******************************************************************/
void CloseFun(void)
{
    FunCtrl.SetFunPWM(0);
}

/*******************************************************************
功能：关闭风扇
参数：无
返回值：无
其它说明：
*******************************************************************/
void OpenFunHeat(void)
{
   // FunCtrl.OpenHeatFan();
   OpenHeatFan();
}

/*******************************************************************
功能：关闭风扇
参数：无
返回值：无
其它说明：
*******************************************************************/
void CloseFunHeat(void)
{
   // FunCtrl.CloseHeatFan();
    CloseHeatFan();
}

/*******************************************************************
功能：调试
参数：无
返回值：无
其它说明：
*******************************************************************/
void Debug(void)
{
    if(wDebugBk != g_stC7.stIn.wParm[0]) bDebugSta = false;
    wDebugBk = g_stC7.stIn.wParm[0];
    switch(g_stC7.stIn.wParm[0])
    {
    case C7_D_TODROP_STEP:          //拨杆电机拨到滴样位步数调试。
        if(g_stC7.stIn.wParm[1] == 0) break;
        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM1ToDripStep = g_stC7.stIn.wParm[1];
            MotorCtrl.M1_DebugRest();
            MotorCtrl.M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
            MotorCtrl.M1_To_Drop(g_pstC7SysPare->wM1ToDripStep, 1);
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM1ToDripStep)
            {
                MotorCtrl.MotorMove(0, 1, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM1ToDripStep));
                g_pstC7SysPare->wM1ToDripStep = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM1ToDripStep)
            {
                MotorCtrl.MotorMove(0, 0, (g_pstC7SysPare->wM1ToDripStep - g_stC7.stIn.wParm[1]));
                g_pstC7SysPare->wM1ToDripStep = g_stC7.stIn.wParm[1];
            }
            else
            {
                ;
            }
        }
        break;
    case C7_D_UP_STEP:          //染剂上升步数调试。
        if(g_stC7.stIn.wParm[1] == 0) break;
        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM3UpStep = g_stC7.stIn.wParm[1];
            MotorCtrl.M3_Rest();
            MotorCtrl.M3_Up(g_pstC7SysPare->wM3UpStep);
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM3UpStep)
            {
                MotorCtrl.MotorMove(2, 1, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM3UpStep));
                g_pstC7SysPare->wM3UpStep = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM3UpStep)
            {
                MotorCtrl.MotorMove(2, 0, (g_pstC7SysPare->wM3UpStep - g_stC7.stIn.wParm[1]));
                g_pstC7SysPare->wM3UpStep = g_stC7.stIn.wParm[1];
            }
            else
            {
                ;
            }
        }
        break;
    case C7_D_M2_TAKE_A_STEP:       //M2电机将玻片拨动一格步数调试
        if(g_stC7.stIn.wParm[1] == 0) break;

        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM2_MoveOne = g_stC7.stIn.wParm[1];
            MotorCtrl.M2_MoveOneStep(g_pstC7SysPare->wM2_MoveOne);
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM2_MoveOne)
            {
                MotorCtrl.MotorMove(1, 1, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM2_MoveOne));
                g_pstC7SysPare->wM2_MoveOne = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM2_MoveOne)
            {
                MotorCtrl.MotorMove(1, 0, (g_pstC7SysPare->wM2_MoveOne - g_stC7.stIn.wParm[1]));
                g_pstC7SysPare->wM2_MoveOne = g_stC7.stIn.wParm[1];
            }
            else {
                ;
            }
        }
        break;
    case C7_D_DROPOIL:              // 滴镜油调试
        if(g_stC7.stIn.wParm[1] == 0) break;
        else g_pstC7SysPare->OilVolume = g_stC7.stIn.wParm[1];
        MotorCtrl.M5_DropStart(g_pstC7SysPare->OilVolume);
        break;
    case C7_D_REST_TO_TODROP_STEP:   // M1复位到滴样位置
        if(g_stC7.stIn.wParm[1] == 0) break;
        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM1RestToDripStep = g_stC7.stIn.wParm[1];
            MotorCtrl.M1_DebugRest();
            MotorCtrl.M1_Rest_To_Drop(g_pstC7SysPare->wM1RestToDripStep,1);
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM1RestToDripStep)
            {
                MotorCtrl.MotorMove(0, 1, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM1RestToDripStep));
                g_pstC7SysPare->wM1RestToDripStep = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM1RestToDripStep)
            {
                MotorCtrl.MotorMove(0, 0, (g_pstC7SysPare->wM1RestToDripStep - g_stC7.stIn.wParm[1]));
               g_pstC7SysPare->wM1RestToDripStep = g_stC7.stIn.wParm[1];
            }
            else {
                ;
            }
        }
        break;
    case C7_D_M3_DOWN_STEP:          //M3下降到冲洗步数调试。
        if(g_stC7.stIn.wParm[1] == 0) break;
        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM3DownToWashStep = g_stC7.stIn.wParm[1];
            MotorCtrl.M3_Rest();
            MotorCtrl.M3_Up(g_pstC7SysPare->wM3UpStep);
            MotorCtrl.M3_DownToWash(g_pstC7SysPare->wM3DownToWashStep);
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM3DownToWashStep)
            {
                MotorCtrl.MotorMove(2, 1, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM3DownToWashStep));
                g_pstC7SysPare->wM3DownToWashStep = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM3DownToWashStep)
            {
                MotorCtrl.MotorMove(2, 0,(g_pstC7SysPare->wM3DownToWashStep - g_stC7.stIn.wParm[1]));
                g_pstC7SysPare->wM3DownToWashStep = g_stC7.stIn.wParm[1];
            }
            else
            {
                ;
            }
        }
        break;
    case C7_D_MPU_CAL:          //校准
//        MPU6050_Angle Data1;
//        Data1.X_Angle = 0;
//        Data1.Y_Angle = 0;
//        for(int i=0;i<5;i++)
//        {
//            vTaskDelay(5);
//            DevMpu6050->MPU6050_Get_Angle(&data);
//            Data1.X_Angle += data.X_Angle;
//            Data1.Y_Angle += data.Y_Angle;
//        }
//        g_pstC7SysPare->wMpuX = (uint16_t)(Data1.X_Angle/5*100);
//        g_pstC7SysPare->wMpuY = (uint16_t)(Data1.Y_Angle/5*100);

        break;
    case C7_D_FAN_DOWN_STEP:          //M3下降到吹风步数调试。
        if(g_stC7.stIn.wParm[1] == 0) break;
        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM3DownToFanStep = g_stC7.stIn.wParm[1];
            MotorCtrl.M3_Rest();
            MotorCtrl.M3_Up(g_pstC7SysPare->wM3UpStep);
            MotorCtrl.M3_DownToFan(g_pstC7SysPare->wM3DownToFanStep);
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM3DownToWashStep)
            {
                MotorCtrl.MotorMove(2, 1, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM3DownToFanStep));
                g_pstC7SysPare->wM3DownToFanStep = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM3DownToFanStep)
            {
                MotorCtrl.MotorMove(2, 0,(g_pstC7SysPare->wM3DownToFanStep - g_stC7.stIn.wParm[1]));
                g_pstC7SysPare->wM3DownToFanStep = g_stC7.stIn.wParm[1];
            }
            else
            {
                ;
            }
        }
        break;
    case C7_D_SETTEMP:          //设置固化温度
            g_pstC7SysPare->wHeatTemp = g_stC7.stIn.wParm[1]*10;
        break;
    case C7_D_SETFANTEMP:          //设置烘干温度
        g_pstC7SysPare->wHeatFanTemp = g_stC7.stIn.wParm[1]*10;
        break;       
    case C7_D_M4SETPS:          //M4步数调试。
        if(g_stC7.stIn.wParm[1] == 0) break;
        if(bDebugSta == false)
        {
            g_pstC7SysPare->wM4Steps = g_stC7.stIn.wParm[1];
            MotorCtrl.M4_Rest(1);
            MotorCtrl.MotorMove(3, 0, (g_pstC7SysPare->wM4Steps));
            bDebugSta = true;
        }
        else
        {
            if(g_stC7.stIn.wParm[1] > g_pstC7SysPare->wM4Steps)
            {
                MotorCtrl.MotorMove(3, 0, (g_stC7.stIn.wParm[1] - g_pstC7SysPare->wM4Steps));
                g_pstC7SysPare->wM4Steps = g_stC7.stIn.wParm[1];
            }
            else if(g_stC7.stIn.wParm[1] < g_pstC7SysPare->wM4Steps)
            {
                MotorCtrl.MotorMove(3, 1,(g_pstC7SysPare->wM4Steps - g_stC7.stIn.wParm[1]));
                g_pstC7SysPare->wM4Steps = g_stC7.stIn.wParm[1];
            }
            else
            {
                ;
            }
        }
        break;            
    case C7_M2_STEP2:
        if(g_stC7.stIn.wParm[1] == 0) break;
        else g_pstC7SysPare->wM2MoveSteps = g_stC7.stIn.wParm[1];
        MotorCtrl.M2_MoveStep(g_pstC7SysPare->wM2MoveSteps);
        break;
    default: break;
    }
}

/*******************************************************************
功能：获取二维码
参数：无
返回值：无
其它说明：
*******************************************************************/
//void GetQRCode(void)
//{
//    QRCodeMod.GetQrCode((u8*)&g_stC7.stOut.waSwap[0]);
//}

/*******************************************************************
功能：获取光耦值
参数：无
返回值：无
其它说明：
*******************************************************************/
void GetOp(void)
{
    OpSensor.GetSensor(&g_stC7.stOut.waSwap[0]);
}

/*******************************************************************
功能：刷新参数
参数：无
返回值：无
其它说明：
*******************************************************************/
void  Renovate(void)
{
    RefreshParameter(&g_stC7.stOut.waSwap[0]);
}

/*******************************************************************
功能：加载参数
参数：无
返回值：无
其它说明：
*******************************************************************/
void  LoadingParameters(void)
{
    SysDatainit();
}

/*******************************************************************
功能：加载参数
参数：无
返回值：无
其它说明：
*******************************************************************/
void  SaveData(void)
{
    bDebugSta = false;
    SaveSysData();
}

/*******************************************************************
功能： M3电机向上，玻片倾斜
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7M3Up(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M3_Up(g_pstC7SysPare->wM3UpStep);
    }
}

/*******************************************************************
功能： M3电机向下，玻片放平
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7M3Down(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M3_Down(g_pstC7SysPare->wM3UpStep);
    }
}

/*******************************************************************
功能： M3电机向下，玻片下降到冲洗
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7M3DownTOWash(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M3_DownToWash(g_pstC7SysPare->wM3DownToWashStep);
    }
}

/*******************************************************************
功能： M3电机向上
参数：无
返回值：无
其它说明：
*******************************************************************/
void C7M3UpTOWash(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M3_UpTOWash(g_pstC7SysPare->wM3DownToWashStep);
    }
}

/*******************************************************************
功能： M3电机向下，玻片下降到风干位
参数：无
返回值：无
其它说明：
*******************************************************************/
void DyeFunDown(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M3_DownToFan(g_pstC7SysPare->wM3DownToFanStep);
    }
}

/*******************************************************************
功能： M3电机由风干位置上升到染色位置
参数：无
返回值：无
其它说明：
*******************************************************************/
void FanUpToDye(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M3_UpTOWash(g_pstC7SysPare->wM3DownToFanStep);
    }
}

/******************************************************************************
函数名：void M4_Transfer(void)
参数描述：无
返回值描述：
无

说明：
运动控制任务

特殊算法：无
注意事项：无
******************************************************************************/
void M4_Transfer(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.M4_Motion();
    }
}

/******************************************************************************
函数名：void M4_Rest(void)
参数描述：无
返回值描述：
无

说明：
运动控制任务

特殊算法：无
注意事项：无
******************************************************************************/
void M4_Rest(void)
{
    MotorCtrl.M4_Rest(g_pstC7SysPare->wM4Steps);
}

/******************************************************************************
函数名：void PumpStart(void)
参数描述：无
返回值描述：
无

说明：
运动控制任务

特殊算法：无
注意事项：无
******************************************************************************/
void PumpStart(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        //  OilCtrl.DropStart(vTaskDelay,g_pstC7SysPare->OilVolume);
        MotorCtrl.M5_DropStart(g_pstC7SysPare->OilVolume);
    }
}

/******************************************************************************
函数名：void FullOil(void)
参数描述：无
返回值描述：
无

说明：
充满镜油

特殊算法：无
注意事项：无
******************************************************************************/
void FullOil(void)
{
    g_stC7.stOut.stoutCmd.byErr = 0;
    MotorCtrl.M5_FullOil();
}

/******************************************************************************
函数名：void Ageing(void)
参数描述：无
返回值描述：
无

说明：
AGING

特殊算法：无
注意事项：无
******************************************************************************/
void C7Aging(void)
{
    if(g_stC7.stIn.wParm[0] == 1) Ageing = 1;
    else Ageing = 0;    
}

/******************************************************************************
函数名：void CheckSlidBox(void)
参数描述：无
返回值描述：
无

说明：
充满镜油

特殊算法：无
注意事项：无
******************************************************************************/
void CheckSlidBox(void)
{
    g_stC7.stOut.stoutCmd.byErr = 0;
    if( DevIo->Read(PD,6) == 0)//判断试纸盒
    {
        for(__IO int i = 0;i<0xffff; i++);
        for(__IO int i = 0;i<0xffff; i++);
        if(DevIo->Read(PD,6) == 0)
        {
            g_stC7.stOut.byCardBox = 0;
            MotorCtrl.C7SetErr(ERR_C7_BOX);
        }
    }
    else
    {
        g_stC7.stOut.byCardBox = 1;
    }
}

/*******************************************************************************
函数功能：
*   @ 获取升级完成标志
参数描述：
*   @ 获取升级完成标志判断是否升级成功
返回值描述：
*   @ 无
说明：

*******************************************************************************/

//void GetUpdataFinish(void)
//{
//    if((*(__IO uint32_t*)USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS) == 0x55AA55AA)//用户程序下载成功标记有效才进入用户程序，避免烧写途中掉电导致不能启动用户程
//    {
//        g_stC7.stOut.byUpdataReady = 0;
//        g_stC7.stOut.byUpdataFinish = 1;
//    }
//    else
//    {
//        g_stC7.stOut.byUpdataReady = 0;
//        g_stC7.stOut.byUpdataFinish = 0;
//    }
//}

/*******************************************************************************
函数功能：
*   @ 获取升级完成标志
参数描述：
*   @ 获取升级完成标志判断是否升级成功
返回值描述：
*   @ 无
说明：

*******************************************************************************/

unsigned char GetUpdataFinish(void)
{
    unsigned char ret=0;
    if((*(__IO uint32_t*)USER_APP_DOWNLOAD_SUCCESS_FLAG_ADDRESS) == 0x55AA55AA)//用户程序下载成功标记有效才进入用户程序，避免烧写途中掉电导致不能启动用户程
    {
        ret = 1;
    }

    return ret;
}

/******************************************************************************
函数名：void TransferToC6(void)
参数描述：无
返回值描述：
无

说明：
传送载玻片到C6

特殊算法：无
注意事项：无
******************************************************************************/
void TransferToC6(void)
{
    uint8_t byErrLevel = 0;
    byErrLevel = MotorCtrl.Get_ErrLevel(g_stC7.stOut.stoutCmd.byErr);
    if(byErrLevel < 2)
    {
        MotorCtrl.C7TransferToC6();
    }
}

/******************************************************************************
函数名：void C7GetMpuData(void)
参数描述：无
返回值描述：
无

说明：获取陀螺仪数据

特殊算法：无
注意事项：无
******************************************************************************/
void C7GetMpuData(void)
{
//    DevMpu6050->MPU6050_Get_Angle(&data);
//
//    if(g_pstC7SysPare->wMpuX > 9000)
//    {
//        wMpuData_X = (uint16_t)(data.X_Angle*100) - (g_pstC7SysPare->wMpuX - 9000);//data.X_Angle>>8
//    }
//    else
//    {
//        wMpuData_X = (uint16_t)(data.X_Angle*100) + (9000 - g_pstC7SysPare->wMpuX);
//    }
//    if((wMpuData_X > 0) && (wMpuData_X < 18100))
//    {
//        g_stC7.stOut.wMpuSwap[0] = wMpuData_X;
//    }
//
//    if(g_pstC7SysPare->wMpuY > 9000)
//    {
//        wMpuData_Y = (uint16_t)(data.Y_Angle*100) - (g_pstC7SysPare->wMpuY - 9000);
//    }
//    else
//    {
//        wMpuData_Y = (uint16_t)(data.Y_Angle*100) + (9000 - g_pstC7SysPare->wMpuY);
//    }
//    if((wMpuData_Y > 0) && (wMpuData_Y < 18100))
//    {
//        g_stC7.stOut.wMpuSwap[1] = wMpuData_Y;
//    }
}

/******************************************************************************
函数名：void C7SetTemp(void)
参数描述：无
返回值描述：
无

说明：设置固化温度

特殊算法：无
注意事项：无
******************************************************************************/
void C7SetTemp(void)
{
    SetTemp(g_stC7.stIn.wParm[0]);
}

/******************************************************************************
函数名：void C7SetFanTemp(void)
参数描述：无
返回值描述：
无

说明：设置烘干温度

特殊算法：无
注意事项：无
******************************************************************************/
void C7SetFanTemp(void)
{
    SetFanTemp(g_stC7.stIn.wParm[0]);
}
           
/*******************************************************************
功能：打开/关闭染色位风扇
参数：无
返回值：无
其它说明：
*******************************************************************/
void DyeFunOpen(void)
{
    FunCtrl.OpendyeFan(1);
}

/*******************************************************************
功能：打开/关闭染色位风扇
参数：无
返回值：无
其它说明：
*******************************************************************/
void DyeFunClose(void)
{
    FunCtrl.OpendyeFan(0);
}

/*******************************************************************
功能：void SendDataTask(void *pvPare)
参数：无
返回值：无
其它说明：2013.12.18
*******************************************************************/
void SendDataTask(void *pvPare)
{
    while (1) {  
        vTaskDelay(500);
        if(bTareCal == false)
        {
            g_stC7.stOut.wTareVal = Get_Weight(g_pstC7SysPare->wTareVal); 
        }
        MotorCtrl.C7Ageing();       
    } 
}

/*******************************************************************
功能：void CreateSendDataTask(void)
参数：无
返回值：无
其它说明：2013.12.18
*******************************************************************/
void CreateSendDataTask(void)
{
    xTaskCreate( SendDataTask , ( char const * ) "SendDataTask", 128, NULL, tskIDLE_PRIORITY, NULL );//tskIDLE_PRIORITY
}
