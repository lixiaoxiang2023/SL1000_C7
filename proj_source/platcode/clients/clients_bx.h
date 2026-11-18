/*******************************************************************************
* Copyright URIT Co.,Ltd. All Right Reserved
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*******************************************************************************/

/*******************************************************************************
* File Name     : client_bx.h
* File Path     : platcode/clients/
* Project       : urit1 plat
* Author        : lixiaoxiang
* Date          : 2023.02.10
* Description   : mcu io base
*******************************************************************************/

/*******************************************************************************
* modify logs:
* 2023.02.10 lixiaoxiang create
*******************************************************************************/
#ifndef __CLIENTS_BX_H_
#define __CLIENTS_BX_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "clients/clients_comm.h"
#include "clients/clients_bx_cmds.h"

/*******************************************************************************
Constant and Macro Definition 常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
/******************************PLUS*B1 IO*******************************/
#if(false)
#pragma pack(1)
typedef union
{
    struct
    {
        unsigned int bLsrOfflineErr:1; // 激光器掉线错误
        unsigned int bLsrOverCurrErr:1;//激光器过流 超过二极管的最大电流
        unsigned int bLsrOverPowerErr:1;//  激光器过功率 实际功率超过设定的功率范围
        unsigned int bLsrBaseTempErr:1;// 激光器模块底板温度过高或过低
        unsigned int bLsrDiodeTempErr:1;//  激光二极管温度过高或过低

        unsigned int ucNA2:27;//预留

    }BIT;
    unsigned int uiBuf;

}ERR_ST;//

// 输出资源定义
typedef struct {
    short unsigned int wSetValue;                                             // PMT设置值
    short unsigned int wValue;                                                // PMT实际输出值*10
} stPmtOutTdf;                                                                // 高压包输出资源定义

typedef struct {
    unsigned char byState;                                                    // 状态 0关闭状态1开启状态2掉线
    unsigned char byMaxPwr;                                                   // 最大输出功率            ,50代表50mW
    short unsigned int wPwrSet;                                               // 设置激光器输出功率返回值 ,125代表12.5mW
    short unsigned int wPwr;                                                  // 实际输出功率             ,125代表12.5mW
    short unsigned int wBaseTemp;                                             // 底板温度
    short unsigned int wDiodeTemp;                                            // 二极管温度
} stLaserOutTdf;                                                                // 激光器输出资源定义

typedef struct
{
    // 输入资源
    struct {
        unsigned char  byCmd;
        unsigned char  byCmdCnt;
        unsigned int waSwap[5];                 // 用于命令的参数交互
    } stIn;

    // 输出资源
    struct {
        unsigned char byCmdDown;                                               // 上次命令执行代码
        unsigned char byCmdCntDown;                                            // 命令执行计数器，有变化是有命令完成执行
        ERR_ST byErr;                                                   // 错误代码

        short unsigned int wGainSet[6];                                       // 模拟增益值
        stPmtOutTdf    stPmt[4];                                                // 高压包输出
        stLaserOutTdf  stLsr[1];                                                // 激光器输出
        short unsigned int wFscBase;                                          // FSC_Base
        //增加一套参数
        unsigned char byParaVer;                                               // 参数版本，0，使用第一套参数，1，使用第二套参数
        unsigned char bAirChkSt:1;                                              // 0无气泡 1有气泡
        unsigned char byRe:7;
        short unsigned int wGainSet2[6];                                      // 模拟增益值,第二套参数
        short unsigned int wPmtSetValue[4];                                   // PMT设置值,第二套参数                                             // 高压包输出,第二套参数

        unsigned int  wHwVer;
        unsigned int  wSwVer;
        unsigned char ucHandFlg;//接收成功回馈标志
        unsigned char usUpdataFinish:1;                    // 固件升级完成标志
        unsigned char usUpdataReady:1;
        unsigned char ucNA1:6;
    } stOut;

    __StDownLoad stDownload;
} stPmtIoTdf;
#pragma pack()
#endif

/******************************PLUS*B2 IO*******************************/
#if(false)
#pragma pack(1)
typedef struct
{
    struct {                                                                    // 输入资源
        unsigned char  byCmd;                                                         // 命令
        unsigned char  byCmdCnt;                                                      // 命令计数器
        unsigned short int wRe;
        unsigned int waSwap[5];                                                     // 用于命令的参数交互
    } stIn;

    struct {
        unsigned char byCmdDown;                                                      // 上次命令执行代码
        unsigned char byCmdCntDown;                                                   // 命令执行计数器，有变化是有命令完成执行
        unsigned char byNA1;
        unsigned char byNA2;

        ERR_ST  dwErr;                                                          // 故障位
        unsigned char byPcmWorking;                                                   // 1:正在计数，          0:停止计数
        unsigned char byDoSamp;                                                       // 1:进行原始数据采集，  0:进行峰值检测
        unsigned char byTrsUsb;                                                       // 1:使用usb传输，       0:使用网络传输
        unsigned char byAdcDbusCheak;                                                 // 1:正在实时监控adc总线 0:不在监控总线

        unsigned short int wAdcDbus[5];                                                   // adc总线数据 最大值
        unsigned short int wAdcPcmCnt[5];                                                 // 各个通道的实时计数值,在进行检测的时候，回传中值的波峰最大值
        unsigned int dwPcmCnt;                                                      // fpga计数值
        unsigned int dwPeakTopBase;                                                 // 识别通道的高值限制 0-15bit 低值，16-31 高值

        unsigned short int wGate[5];                                                      // 识别阈值
        unsigned short int wMidVal[5];                                                    // 峰值的中值,使用最大值，即出现最多次数的粒子进行计算，在采样过程中实时计算
        unsigned short int wMidVal2[5];                                                   // 按照波形统计的中值，最大值/2后M，遍历所有数据，按照min（》M） + max（<M）/2 计算

        unsigned short int wSetGate1[5];                                                  // 返回设置阈值1给工控板 高低灵敏度调试用
        unsigned short int wSetGate2[5];                                                  // 返回设置阈值2给工控板 高低灵敏度调试用

        unsigned short int wGetWGate[HW_ADC_NUM];
        unsigned short int wSetWGate1[HW_ADC_NUM];                                        // 返回设置宽度阈值
        unsigned short int wSetWGate2[HW_ADC_NUM];                                        // 返回设置宽度阈值

        unsigned char  byParaVer;                                                     // 参数版本，0，使用第一套参数，1，使用第二套参数

        unsigned char  ucHandFlg;                                                     // 接收成功回馈标志
        unsigned char  usUpdataFinish:1;                                              // 固件升级完成标志
        unsigned char  usUpdataReady :1;                                              // 固件升级准备好
        unsigned char  ucNA1:6;

        unsigned char  ucNA111;

        unsigned int dwFpgaCellFormVer;                                             // FPGA细胞分类模板版本
        unsigned short int wCellResults[46];                                              // FPGA细胞分类结果

        unsigned int dwHwVer;
        unsigned int dwSwVer;
        unsigned int dwFpgaVer;
    } stOut;

    __StDownLoad stDownload;
} stIoMainTdf;
#pragma pack()
#endif

/******************************PLUS*B3 IO*******************************/
#if(false)
#pragma pack(1)
typedef union
{
  struct
  {
    unsigned char bit00:1;
    unsigned char bit01:1;
    unsigned char bit02:1;
    unsigned char bit03:1;
    unsigned char bit04:1;
    unsigned char bit05:1;
    unsigned char bit06:1;
    unsigned char bit07:1;
  }Field ;
  unsigned char uiBuf;
}Staus8BitType;

typedef union
{
  struct
  {
    unsigned int bit00:1;
    unsigned int bit01:1;
    unsigned int bit02:1;
    unsigned int bit03:1;
    unsigned int bit04:1;
    unsigned int bit05:1;
    unsigned int bit06:1;
    unsigned int bit07:1;
    unsigned int bit08:1;
    unsigned int bit09:1;
    unsigned int bit10:1;
    unsigned int bit11:1;
    unsigned int bit12:1;
    unsigned int bit13:1;
    unsigned int bit14:1;
    unsigned int bit15:1;
    unsigned int bit16:1;
    unsigned int bit17:1;
    unsigned int bit18:1;
    unsigned int bit19:1;
    unsigned int bit20:1;
    unsigned int bit21:1;
    unsigned int bit22:1;
    unsigned int bit23:1;
    unsigned int bit24:1;
    unsigned int bit25:1;
    unsigned int bit26:1;
    unsigned int bit27:1;
    unsigned int bit28:1;
    unsigned int bit29:1;
    unsigned int bit30:1;
    unsigned int bit31:1;
  }Field ;
  unsigned int ulBuf;
}StauintBitType;

typedef struct {
    unsigned char bPinchOptoNoCailErr:1;              //夹管光耦未校准
    unsigned char bPinchOptoCailErr:1;               //夹管光耦校准失败
    unsigned char bDecompreOverErr:1;                //泄压过压故障
    unsigned char bDecompreLowErr:1;                //泄压欠压故障
    unsigned char bRiHErr:1;                       //RI上限位浮子异常
    unsigned char bCeramicsFLAOptErr:1;           //FLA陶瓷泵光耦状态异常
    unsigned char bCeramicsFLBOptErr:1;           //FLB陶瓷泵光耦状态异常
    unsigned char bYDyeOptoEmptyErr_NA:1;                 //FLA染色液管空

    unsigned char bSheathEmtyErr:1;                     // 鞘液灌注异常
    unsigned char bWasteFullErr:1;                     // 废液满
    unsigned char dwSampNeedleBlockErr:1;              //采样针堵塞
    unsigned char bFlashErr:1;                         //存储异常
    unsigned char bMotorStepSaveErr:1;                 //采样针定位步数未存储
    unsigned char bTempErr1:1;                         //反应杯温度传感器故障
    unsigned char bTempErr2:1;                         //鞘液加热杯温度传感器故障
    unsigned char bTempErr3:1;                         //环境温度传感器故障

    unsigned char bEleHotErr1:1;                       //反应杯加热故障
    unsigned char bEleHotErr2:1;                       //鞘液加热杯加热故障
    unsigned char bReactOverTemp:1;                         //反应杯过温故障
    unsigned char bSheathOverTemp:1;                         //鞘液加热杯过温故障
    unsigned char bPresErr3_NA:1;                         //PS3压力传感器故障
    unsigned char bPresErr4_NA:1;                         //PS4压力传感器故障
    unsigned char bPresErr5_NA:1;                         //PS5压力传感器故障
    unsigned char ubPS1PesOverLimt:1;//PS1超限故障

    unsigned char ubPS2PesOverLimt:1;//PS2压超限故障
    unsigned char ubPS3PesOverLimt:1;//PS3压超限故障
    unsigned char ubPS4PesOverLimt:1;//PS4压超限故障
    unsigned char ubPS5PesOverLimt:1;//PS5压超限故障
    unsigned char ubPS1PesLowLimt:1;//PS1欠压故障
    unsigned char ubPS2PesLowLimt:1;//PS2欠压限故障
    unsigned char ubPS3PesLowLimt:1;//PS3欠压限故障
    unsigned char ubPS4PesLowLimt:1;//PS4欠压限故障

    unsigned char ubPS5PesLowLimt:1;//PS5欠压限故障
    unsigned char dMALostStepErr:1;//MA失步故障
    unsigned char dMBLostStepErr:1;//MB失步故障
    unsigned char dMCLostStepErr:1;//MC失步故障
    unsigned char dMDLostStepErr:1;//MD失步故障
    unsigned char dMERestStepErr:1;//ME复位故障
    unsigned char dMELostStepErr:1;//ME定位故障
    unsigned char ucOverTimeCpu0:1;//CPU0等待超时

    unsigned char ucOverTimeCpu1:1;//CPU1等待超时
    unsigned char ucOverTimeCpu2:1;//CPU2等待超时
    unsigned char ulShBlockageErr:1;//鞘流器堵塞
    unsigned char ulNeedleBlockageErr:1;//采样针堵塞
    unsigned char ulVmcmdErr:1;//VM指令错误
    unsigned char ulKeepFluidErr:1;//保养液不足
    unsigned char ulSamplErr:1;//样本量不足
    unsigned char ulSamplChkErr:1;//样本量检测异常

    unsigned char ulConductCailErr:1;//电导率校准失败
    unsigned char ucAmbientTempOverRun :1;//环温超限
    unsigned char ucOverTimeCpu3:1;//CPU3等待超时
    unsigned char ulNA5:5;//预留
} ST_FAULT;

typedef union
{
    struct
    {
      unsigned int ubValFlg1:1;      //
      unsigned int ubValFlg2:1;  //
      unsigned int ubValFlg3:1;    //
      unsigned int ubValFlg4:1;    //
      unsigned int ubValFlg5:1;    //
      unsigned int ubValFlg6:1;    //
      unsigned int ubValFlg7:1;   //
      unsigned int ubValFlg8:1;  //

      unsigned int ubValFlg9:1;      //
      unsigned int ubValFlg10:1;  //
      unsigned int ubValFlg11:1;    //
      unsigned int ubValFlg12:1;    //
      unsigned int ubValFlg13:1;    //
      unsigned int ubValFlg14:1;    //
      unsigned int ubValFlg15:1;   //
      unsigned int ubValFlg16:1;  //

      unsigned int ubValFlg17:1;      //
      unsigned int ubValFlg18:1;  //
      unsigned int ubValFlg19:1;    //
      unsigned int ubValFlg20:1;    //
      unsigned int ubValFlg21:1;    //
      unsigned int ubValFlg22:1;    //
      unsigned int ubValFlg23:1;   //
      unsigned int ubValFlg24:1;  //

      unsigned int ubValFlg25:1;      //
      unsigned int ubValFlg26:1;  //
      unsigned int ubValFlg27:1;    //
      unsigned int ubValFlg28:1;    //
      unsigned int ubValFlg29:1;    //
      unsigned int ubValFlg30:1;    //
      unsigned int ubValFlg31:1;   //
      unsigned int ubValFlg32:1;  //
    }BIT;
    unsigned int ulBuf;
}VAR_ST;//阀状态

typedef union
{
    struct
    {
      unsigned int ubMAOpt1:1;      //
      unsigned int ubMBOpt2:1;  //
      unsigned int ubMCOpt3:1;    //
      unsigned int ubMDOpt4:1;    //
      unsigned int ubMEOpt5:1;    //
      unsigned int ubMFOpt6:1;    //
      unsigned int ubHotTankOpt7:1;   //加热罐溢出检测夹管光耦
      unsigned int YDyeOpt8:1;  //黄色染色液检测夹管光耦

      unsigned int ubMEUPOpt9:1;      //
      unsigned int bsheathUpLim10:1;  //
      unsigned int bsheathDownLim11:1;    //
      unsigned int bCleanerH12:1;    //
      unsigned int bCleanerL13:1;    //
      unsigned int RDyeOpt14:1;    //紫色染色液检测夹管光耦
      unsigned int bDiluentPinchOpto15:1;   //稀释液夹管光耦
      unsigned int ubFLAOpt16:1;    //FLA陶瓷泵光耦
      unsigned int ubFLBOpt17:1;    //FLB陶瓷泵光耦
      unsigned int ubWasteOpt18:1;  //
      unsigned int ubOpt19:1;    //
      unsigned int ubOpt20:1;    //
      unsigned int ubOpt21:1;    //
      unsigned int ubOpt22:1;    //
      unsigned int ubOpt23:1;   //
      unsigned int ubOpt24:1;  //

      unsigned int ubOpt25:1;      //
      unsigned int ubOpt26:1;  //
      unsigned int ubOpt27:1;    //
      unsigned int ubOpt28:1;    //
      unsigned int ubOpt29:1;    //
      unsigned int ubOpt30:1;    //
      unsigned int ubOpt31:1;   //
      unsigned int ubOpt32:1;  //

    }BIT;
    unsigned int ulBuf;
}OPT_ST;//光耦状态

typedef struct{
    unsigned char sw:1;
    unsigned char bySped:7;
}PUMP_SET;

typedef struct{
    unsigned int sdir:1;
    unsigned int strucMotorKeyFlg:1;
    unsigned int sStep:30;
}MOTOR_CALIB;

typedef struct{
    short dwDuctTemp;                                                             // 电导温度
    short dwCailTemp;                                                             // 校准温度
    unsigned short int dwDuctVaul;                                                             // 电导值
    unsigned short int bSample:1;                                                              // 样本量判断,0,正常，                                                                          // 1,无样本或者样本电导率值低
    unsigned short int bRe:3;
    unsigned short int wAmplSet:12;                                                            // 激励
    unsigned short int wAmplGet;                                                               // 采样到的波形振幅
    unsigned short int wCondCal;                                                               // 校准液电导率值
}CONDUCT_CONFG;

typedef struct
{
    signed int iMaStepNow;
    signed int iMaStepAim;

    signed int iMbStepNow;
    signed int iMbStepAim;

    signed int iMcStepNow;
    signed int iMcStepAim;

    signed int iMdStepNow;
    signed int iMdStepAim;

    signed int iMeStepNow;
    signed int iMeStepAim;


} MotorStepMonitor;

typedef struct
{
    unsigned int uiUFLACnt;
    unsigned int uiUFLBCnt;
    unsigned int uiClearCnt;
    unsigned int uiSheathUsage;//鞘液使用量ml
    unsigned int uiDiluentCnt;
} ReagentMonitor;

typedef struct
{
    unsigned short usOptAdc;//光耦AD值显示
    unsigned short usOptEmptyAdc;//光耦检测空管ADC值
    unsigned short usOptFullAdc;//光耦检测液体满ADC值
}OptCfg;//夹管光耦配置

typedef struct {
    // 输入资源
    unsigned char  byCmdCnt;
    unsigned char  byCmd;
    StauintBitType waSwap[5];                 // 用于命令的参数交互
} __stIn;

typedef struct {
    // 输出资源
    //unsigned char  byErrCnt;
    unsigned char  byCmdDown;
    unsigned char  byCmdCntDown;
    unsigned short int usNA2;

    ST_FAULT byErr;           //故障位

    unsigned int dwPS1;                // 气压传感器实时压力值
    int dwPS2;                // 液压传感器实时压力值

    short wTemp[12];            // 0-7 熱電偶温度，8 电导率温度， 9保留
    unsigned char  ucNA1;                // 预留
    unsigned char  ucNA2;                // 预留
    unsigned char  ucNA3;                // 预留
    unsigned char  ucNA4;                // 预留
    OPT_ST strOpt;            //光耦状态位显示

    unsigned char  bQiaoliu:1;                   // 鞘流状态          0 无鞘流     1 有鞘流
    unsigned char  bGainType:1;                  //增益类型 0：取第一个缓冲 1：取第二个缓冲
    unsigned char  bSysRunSt:2;                  //系统运行状态 0:待机 1：运行       //用于判断流程是否运行完成 切入 ST检测
    unsigned char  bSheath_empty:1;              // 鞘液罐状态：        0 还有       1 鞘液已用完
    unsigned char  bWasteLiquid_full:1;          // 废液桶状态：        0 未满 		1 满
    unsigned char  bBuildPress:1;                // 鞘液负压状态        0 无压力     1 有压力
    unsigned char  bSheathHot:1;                 // 鞘液温度状态        0 未达到要求 1 达到要求

    unsigned char  bAdjLiqLevel:1;               //调整液路流程 0:未完成 1：完成
    unsigned char  bRemovPress:1;                // 释放压力流程：      0 未完成     1 完成
    unsigned char  bEmptyChk:1;                  // 空白检测：          0 未完成     1 完成
    unsigned char  bQualitContlChk:1;            // 质控检测：          0 未完成     1 完成
    unsigned char  bDiluentEmptying:1;                 // 稀释液排空：          0 未完成     1 完成
    unsigned char  bKeepFitFluid:1;              // 保养液路：          0 未完成     1 完成
    unsigned char  bFLAEmptying:1;             // FLA排空：        0 未完成     1 完成
    unsigned char  bTranPwrOff:1;                // 运输前关机流程：    0 未完成     1 完成

    unsigned char  bFloodSheath:1;               // 灌注鞘液流程：       0 未完成     1 完成
    unsigned char  bWashEnd:1;                   // 清洗液流程：        0 未完成     1 完成
    unsigned char  bLiqRst:1;                    // 液路复位：         0 未完成     1 完成
    unsigned char  bSheathEmptying:1;              //鞘液排空         0 未完成     1 完成
    unsigned char  bHeatCupFlood:1;              // 灌注鞘液流程：       0 未完成     1 完成
    unsigned char  bPipeAirOut:1;                // 排管路气泡流程：      0 未完成     1 完成
    unsigned char  bCleanSheath:1;               // 清空鞘液流程：       0 未完成     1 完成
    unsigned char  bLiquidDrain:1;               // 液路排空流程：       0 未完成     1 完成

    unsigned char  bDiluentFlood:1;              // 灌注稀释剂流程：      0 未完成     1 完成
    unsigned char  bYDyeAgntFlood:1;              // 灌注黄染色剂流程：      0 未完成     1 完成
    unsigned char  bChageSheath:1;               // 鞘液更换流程：       0 未完成     1 完成
    unsigned char  bXYConfigOK:1;                //XY位置设定         0：未设置 1：设置完成
    unsigned char  bCleanFull:1;                 // 清洗液瓶灌注流程：      0 未完成     1 完成
    unsigned char  bChageDiluent:1;              //更换稀释液
    unsigned char  bChageYDye:1;                 //更换黄色染色液
    unsigned char  bChageRDye:1;                 //更换紫色染色液

    unsigned short int bQualitSTChk:1;               //急诊位质控检测流程：0 未完成 1完成
    unsigned short int bSingleSTChk:1;               //急诊位单进样检测流程：0 未完成 1完成
    unsigned short int bRDyeAgntFlood:1;              // 灌注紫染色剂流程：      0 未完成     1 完成
    unsigned short int bConDuctCmdEnd:1;              //电导率校准结束
    unsigned short int bCailEnd:1;                     // 10CV校准结束流程 0 未完成 1完成
    unsigned short int bCailCxEnd:1;                   // 10CV校准计数延时流程 0 未完成 1完成
    unsigned short int bBaseDiluent:1;              //打一次稀释剂
    unsigned short int bBaseYDye:1;                 //打一次黄色染色液
    unsigned short int bBaseRDye:1;                 //打一次紫色染色液
    unsigned short int bWashReatflg:1;             // 清洗反应杯 0未完成 1 完成
    unsigned short int bEleHotEn:1;              //加热开关 0关闭加热 1开启加热
    unsigned short int bErrRunEndFlg:1;            // 故障运行流程 0：未完成 1完成
    unsigned short int bSheatchkFlg:1;            // 鞘流检测流程 0：未完成 1：完成
    unsigned short int bDormancyEndFlg:1;				//休眠流程 0：未完成 1：完成
    unsigned short int bAwakenEndFlg:1;			//唤醒流程0：未完成 1：完成
    unsigned short int bSampBlockEndFlg:1;            // 采样针排堵流程 0：未完成 1：完成

    unsigned short int bFLAOptSt:1;                   // FLA液路管空状态输出 0不空 1空
    unsigned short int bFLBOptSt:1;                   // FLB液路管空状态输出 0不空 1空
    unsigned short int bDiluentOptSt:1;               // 稀释液液路管空状态输出 0不空 1空
    unsigned short int bSheathOptSt:1;               //  鞘液液路管空状态输出 0不空 1空
    unsigned short int bGenMixSt:1;                   //一般混匀
    unsigned short int bStrMixSt:1;                   // 加强混匀
    unsigned short int bCloseMixSt:1;                   //不混匀
    unsigned short int wCountTime:9;                   // 计数时长

    unsigned short int bDbugEn:1;          //调试状态 0：正常状态 1：调试状态
    unsigned short int bShBlockagePipiEndflg:1;       // 鞘流器排堵流程：         0 未完成     1 完成
    unsigned short int usMeLostStepFlg:1;                   // ME失败复位完成标志
    unsigned short int usMAtestFlg:1;                      //MA测试流程：         0 未完成     1 完成
    unsigned short int usMBtestFlg:1;                      //MB测试流程：         0 未完成     1 完成
    unsigned short int usMCtestFlg:1;                      //MC测试流程：         0 未完成     1 完成
    unsigned short int usUpdataFinish:1;                   // 固件升级完成标志
    unsigned short int usUpdataReady:1;                    // 固件升级准备好
    unsigned short int usDuctChkFlg:1;                     //电导率检测标志
    unsigned short int usSampRestFlg:1;                    //采样针复位完成标志
    unsigned short int usReagentDilutioFlg:1;              //尿样加样分析完成标志
    unsigned short int bFLBEmptying:1;                     //FLB排空      0 未完成     1 完成
    unsigned short int bMixMode:2;                         // 混匀模式 0：不混匀 1：标准混匀 2、加强混匀
    unsigned short int wWashTime:2;                        // 清洗时长

    CONDUCT_CONFG dwDuct;                   //电导率值

    unsigned int dwHwVer;
    unsigned int dwSwVer;
    unsigned char  ucContinuSampFstEnd;         //连续进样首样结束标志位
    unsigned char  ucContinuSampEnd;            //连续进样结束标志位
    unsigned char  ucContinuSampALLEnd;         //连续进样结束标志位
    unsigned char  ucMoreSimpChkEnd;            //多进样检测结束
    VAR_ST strDis;                   //阀状态显示
    PUMP_SET strPumpSt[8];           //泵状态显示
    MOTOR_CALIB strMotorX[5];        //获取校准步进电机
    MOTOR_CALIB strMotorY[3];        //获取校准步进电机
    int dwPS3;                       // 液压传感器实时压力值
    int dwPS4;                       // 液压传感器实时压力值
    unsigned int dwPS5;                       // 气压传感器实时压力值
    unsigned int uiV23Cx;//定量泵使用次数
    unsigned int uiV24Cx;//定量泵使用次数
    unsigned int usV25Cx;//定量泵使用次数
    OPT_ST strOptEn;          //光耦使能
    unsigned int  uiMaCx;
    unsigned int  uiMbCx;
    ReagentMonitor strReagentCnt;//试剂使用次数
    MotorStepMonitor strMotorStep;//点击步数监控测试用
    unsigned int ucHandFlg;//接收成功回馈标志
    OptCfg strOptCfg[4];//夹管光耦参数
    unsigned char ucNA[4];//预留
} __stOut;

typedef struct
{
    __stIn stIn;
    __stOut stOut;
    __StDownLoad stDownload;
}stIoB3Tdf;
#pragma pack()
#endif

/******************************PLUS*B7 IO*******************************/
#if(false)
#pragma pack(1)
typedef struct {
    unsigned char bLeakageErr:1;              //漏液故障
    unsigned char bYdyeHErr:1;    				//FLA染色液未放入
    unsigned char bYdyeVErr:1;    				//FLA染色液门未关
    unsigned char bRdyeHErr:1; 	 			//FLB染色液未放入
    unsigned char bRdyeVErr:1;  				//FLB染色液门未关
    unsigned char bKeepfitHErr:1;	 			 //保养液未放入
    unsigned char bNA1:2;	  //

    union {
        struct {
            unsigned char bFLA_OffLine:1;              // FLA射频模块掉线                  // 0:在线，1:通信异常，掉线
            unsigned char bFLB_OffLine:1;              // FLB射频模块掉线
            unsigned char bCLEAR_OffLine:1;            // 清洗液射频模块掉线
            unsigned char bSHEAT_OffLine:1;            // 鞘液射频模块掉线
            unsigned char bDILUENT_OffLine:1;          // 稀释液射频模块掉线
            unsigned char bRe:3;
        };
        unsigned char byOffLineErr;
    };

    unsigned char ucNA1;//预留
    unsigned char ucNA2;//预留
 } ST_FAULT_B7;

typedef union _OPT_ST
{
    struct
    {
      unsigned char bRdyeHOpt1:1;      //
      unsigned char bRdyeVOpt2:1;  //
      unsigned char bYdyeHOpt3:1;    //
      unsigned char bYdyeVOpt4:1;    //
      unsigned char bKeepfitHOpt5:1;    //
      unsigned char bSheathOpt6:1;    //
      unsigned char bThinnerOpt7:1;   //加热罐溢出检测夹管光耦
      unsigned char bOpt8:1;  //黄色染色液检测夹管光耦

    }BIT;
    unsigned char ucBuf;

} OPT_ST_B7;//光耦状态

// 输出资源定义
typedef struct {
    unsigned char byCard:1;                                                    // 是否有卡片
    unsigned char byCnt :5;                                                    // 刷卡动作计数
    unsigned char byState:1;                                                   // 是否就绪，0 就绪，1，未就绪（门未关好或者没有检测到试剂瓶）
    unsigned char byFlag :1;                                                   // 写卡是否成功，0成功，1失败
    unsigned char abyUUID[5];                                                  // 卡号
    unsigned char abyData[64];                                                 // 数据
} stRfidTdf;                                                                    // 射频卡输出定义


typedef struct
{
    // 输入资源
    struct {
        emRfidCmdTdf    byCmd;
        unsigned char  byCmdCnt;
        short unsigned int waSwap[39];                                        // 数据交换部分
    } stIn;

    // 输出资源
    struct {
        emRfidCmdTdf byCmdDown;                                                 // 上次命令执行代码
        unsigned char byCmdCntDown;                                            // 命令执行计数器，有变化是有命令完成执行
        ST_FAULT byErr;                                                         // 错误代码
        OPT_ST strLimOpt;
        stRfidTdf stRfid[5];                                                    // 4路射频卡输出

        unsigned int  wHwVer;
        unsigned int  wSwVer;
        unsigned char ucHandFlg;//接收成功回馈标志
        unsigned char usUpdataFinish:1;                    // 固件升级完成标志
        unsigned char usUpdataReady:1;
        unsigned char ucNA1:6;
    } stOut;
    __StDownLoad stDownload;
} stRfidIoTdf;
#pragma pack()
#endif

/******************************PLUS*B9 IO*******************************/
#if(false)
#pragma pack(1)
typedef union
{
    struct
    {
        unsigned int bM1RestErr:1;                                            //M1复位故障
        unsigned int bScancodeReadErr:1;                                      //读条码失败
        unsigned int bMotorStepErr:1;                                         //垂直深度设置失败
        unsigned int ucNA2:29;                                                //预留
    }BIT;
    unsigned int uiBuf;

}ST_FAULT_B9;//

typedef struct
{
    // 输入资源
    struct {
        unsigned char  byCmd;
        unsigned char  byCmdCnt;
        unsigned int waSwap[5];                                               // 用于命令的参数交互
    } stIn;

    // 输出资源
    struct {
        unsigned char byCmdDown;                                               // 上次命令执行代码
        unsigned char byCmdCntDown;                                            // 命令执行计数器，有变化是有命令完成执行
        ST_FAULT byErr;                                                         // 错误代码

        unsigned char abyBarCode[18];                                          // 条码
        unsigned char abyOptc[2];                                              // 光耦

        unsigned int  wHwVer;
        unsigned int  wSwVer;

        unsigned char ucHandFlg;//接收成功回馈标志
        unsigned char usUpdataFinish:1;                    // 固件升级完成标志
        unsigned char usUpdataReady:1;
        unsigned char ucNA1:6;
        unsigned short usMotorStep;//扫码电机当前步数
        unsigned char ucNA2[10];

    } stOut;

    __StDownLoad stDownload;
} stScanCodeIoTdf;
#pragma pack()
#endif

/******************************PLUS*B6 IO*******************************/
#if(false)
#pragma pack(1)
typedef struct
{
    // 输入资源
    struct {
        unsigned char  byCmd;
        unsigned char  byCmdCnt;
        unsigned int waSwap[5];                 // 用于命令的参数交互
    } stIn;

    // 输出资源
    struct {
        unsigned char  byErr;                                                  // 错误代码
        unsigned char  byErrCnt;                                               // 错误计数器，有变化是有错误发生
        unsigned char  byCmdDown;                                              // 上次命令执行代码
        unsigned char  byCmdCntDown;                                           // 命令执行计数器，有变化是有命令完成执行

        unsigned char  bySta;
        unsigned char  byKeyEven[3];                                           // 按键事件
        unsigned int wHwVer;
        unsigned int wSwVer;
        unsigned char ucHandFlg;//接收成功回馈标志
        unsigned char usUpdataFinish:1;                    // 固件升级完成标志
        unsigned char usUpdataReady:1;
        unsigned char ucNA1:6;
        short sInLightTemp;//光路内部温度检测
        short sOutLightTemp;//光路外部温度检测
        char ucNA2[10];
    } stOut;

    __StDownLoad stDownload;
} stB6IoTdf;
#pragma pack()
#endif

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出变量申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/


#endif

