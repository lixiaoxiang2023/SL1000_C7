/*****************************************************************************
* Copyright URIT Co.,Ltd. All Right Reserved
*
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/

/******************************************************************************
* File Name     : clients_dx.h
* File Path     : ../clients_dx.h
* Project       : MP-1200
* Author        : huangfengquan
* Date          : 2025.04.07
* Description   : MP1200
*
* modify logs:
* 2025.04.07 huangfengquan    create
******************************************************************************/
#ifndef CLIENTS_DX_H_
#define CLIENTS_DX_H_

/******************************************************************************
*Include files
******************************************************************************/
#include "clients/clients_comm.h"
#include "clients/clients_dx_cmds.h"

/******************************************************************************
*Data define
******************************************************************************/
typedef union {
    unsigned short int wCmdisBusy; // 10个并行线程
    struct {
        unsigned short int wCmdisBusy0:1;
        unsigned short int wCmdisBusy1:1;
        unsigned short int wCmdisBusy2:1;
        unsigned short int wCmdisBusy3:1;
        unsigned short int wCmdisBusy4:1;
        unsigned short int wCmdisBusy5:1;
        unsigned short int wCmdisBusy6:1;
        unsigned short int wCmdisBusy7:1;
        unsigned short int wCmdisBusy8:1;
        unsigned short int wCmdisBusy9:1;
        unsigned short int wCmdisBusy10:1;
        unsigned short int wCmdisBusy11:1;
        unsigned short int wCmdisBusy12:1;
        unsigned short int wCmdisBusy13:1;
        unsigned short int wCmdisBusy14:1;
        unsigned short int wCmdisBusy15:1;
    } stCmdisBusy;
}unCmdisBusy;

typedef union {
    unsigned short int wErrSta; // 错误位置信息
    struct {
        unsigned short int wErrUpXSta:1;    //0 上层机械手X轴去夹玻片前复位   1完成夹取-放置后复位
        unsigned short int wErrUpYSta:2;    //0 上层机械手Y轴去夹玻片前复位   1完成夹取-放置后复位, 2 完成夹取后复位
        unsigned short int wErrDXSta:1;     //0 下层机械手X轴去夹玻片前复位   1完成夹取-放置后复位
        unsigned short int wErrDYSta:1;     //0 下层机械手Y轴去夹玻片前复位   1完成夹取-放置后复位，
        unsigned short int wErrDZSta:2;     //0 下层机械手Z轴去夹玻片前复位   1完成夹取-放置后复位  2 完成夹取后复位
        unsigned short int wErrUDYSta:1;    //0 上下层机械手去夹玻片前复位    1完成夹取-放置后复位
        unsigned short int wErrSta6:1;
        unsigned short int wErrSta7:1;
        unsigned short int wErrSta8:1;
        unsigned short int wErrSta9:1;
        unsigned short int wErrSta10:1;
        unsigned short int wErrSta11:1;
        unsigned short int wErrSta12:1;
        unsigned short int wErrSta13:1;
    } stErrSta;
}unErrSta;

typedef union {
    unsigned short int wD1StaSync; // 状态同步
    struct {
        unsigned short int  byHasSlideBox:1;        //有无玻片盒 1无 0有
        unsigned short int  byHasGlass:1;           //有无载玻片 1无 0有
        unsigned short int  byFrontHousingStatus:1; //前壳打开
        unsigned short int  byGlsssToD2carStatus:1; // 玻片上车  1到达 0未达
        unsigned short int  byTubeRemove:1;         // 是否需要手动移除试管 0 不需要 1 需要
        unsigned short int  bySlideLess:1;          // 玻片过少 1：充足   0：过少
        unsigned short int  byClawBottomPos:1;      // 夹取气爪光耦 底部
        unsigned short int  byClawTopPos:1;         // 夹取气爪光耦 顶部
        unsigned short int  byClawCarPos:1;         // 夹取气爪光耦 小车位置

        unsigned short int  res:7;
    } stD1StaSync;
}unD1StaSync;

typedef union {
    unsigned int wD2StaSync; // 状态同步
    struct {
        unsigned int  bySlid:1;                 // 有无玻片 0有 1无
        unsigned int  byCleanSlid:1;            // 清洗位有无玻片
        unsigned int  byCarbinolPos:1;          // 转盘甲醇位置状态
        unsigned int  byDyeAPos:1;              // 转盘染色A位置状态
        unsigned int  byDyeBPos:1;              // 转盘染色B位置状态
        unsigned int  byCleanPos:1;             // 转盘清洗位位置状态
        unsigned int  byBasketOpcIn1:1;         // 玻片篮反射光耦状态
        unsigned int  byBasketOpcIn2:1;         // 玻片篮反射光耦状态
        unsigned int  byBasketOpcIn3:1;         // 玻片篮反射光耦状态
        unsigned int  byBasketOpcOut1:1;        // 玻片篮反射光耦状态
        unsigned int  byBasketOpcOut2:1;        // 玻片篮反射光耦状态
        unsigned int  byBasketOpcOut3:1;        // 玻片篮反射光耦状态
        unsigned int  byHeatSlid:1;             // 烘干位有无载玻片 1有 0无
        unsigned int  byDownSildSta:1;          // 玻片是否已上升 0 1下降
        unsigned int  byUpSildSta:1;            // 玻片在风干位置状态是否已上升 0 1下降
        unsigned int  byAfterPush:1;            // 推刀到清洗槽上方状态标志
        unsigned int  bySlideIntoDye:1;         // 玻片是否下降至染色池 0:未放 1：已放入
        unsigned int  byBasketDown:1;           // 玻片篮下降到底部 0  上面1
        unsigned int  res:14;                   // 预留
    } stD2StaSync;
}unD2StaSync;

typedef union {
    unsigned short int wD3StaSync; // 状态同步
    struct {
        unsigned short int  byOutFsw:1;       //外置试剂浮子标志
        unsigned short int  byOutWasteFsw:1;     //外置废液浮子标志
        unsigned short int  byAfterDropState:1;     //滴样结束状态标志
        unsigned short int  byAfterPushWash:1;      //清洗槽加水完成
        unsigned short int  byAfterSampleWash:1;    //采样清洗完成
        unsigned short int  byAfterDropWash:1;      //滴样清洗完成
        unsigned short int  byDyeEmpty:1;           //染剂回收利用到达设置后排废,1正在运行，0结束
        unsigned short int  byWashEmpty:1;          //最后一次清洗抽废开始标志，1开始，0结束
	    unsigned short int  byPinPapeSample:1;      //采样针管路样本检测
		unsigned short int  byDiulentTankOut:1;     //稀释液外置浮子
		unsigned short int  byPurifiedTankOut:1;    //纯化水外置浮子
		unsigned short int  byBufferTankOut:1;      //缓冲液外置浮子
		unsigned short int  byCh3ohTankOut:1;      //甲醇外置浮子
		unsigned short int  byDyeATankOut:1;      //染剂A外置浮子
		unsigned short int  byDyeBTankOut:1;      //染剂A外置浮子
		unsigned short int  res:1;               //预留
    } stD3StaSync;
}unD3StaSync;
typedef union {
    unsigned int wD5StaSync; // 状态同步
    struct {
        unsigned int  byTubeSamplePos:1;          //试管检测位有无试管标致 1或0
        unsigned int  byTubeScanBarcodePos:1;     //条码扫描位有无试管  1或0
        unsigned int  byTubeMixPos:1;             //混匀位有无试管  1或0
        unsigned int  byTubeClampPos:1;           //夹试管位有无试管 1或0
        unsigned int  byOnlineModel:1;            //进样联机模式标志位  1联机 0单机
        unsigned int  bySampleState:1;            //进样类型使用标志位 1联机进样 0单机进样
        unsigned int  byRetestWaitPos:1;          //试管架是否在复测等待位 0不在   1在
        unsigned int  byFinsh:1;                  //试管架推到左平台结束标致
        unsigned int  byMedialBeltWorkFlag:1;     //内侧皮带工作标志位 1工作 0停止
        unsigned int  byOutsideBeltWorkFlag:1;    //外侧皮带工作标志位 1工作 0停止
        unsigned int  byOnlineInRackFlag:1;       //联机试管架进入标志位，1有 0无
        unsigned int  byRackBackFlag:1;           //出桥位有无试管架标志位 0无 1有
        unsigned int  byLeftPaddleEnable:1;       // 左拨杆能否往前推至出桥识别位 0：不能  1：能
        unsigned int  byLeftPlatHas:1;            // 左平台有无试管架 0：没有 1：有
        unsigned int  byRightPlatSig:1;           // 右平台试管架对射信号 0：没有 1：有
        unsigned int  byEnableTubeOp:1;           // 拨动试管架，使能是否可以操作试管 0：不能  1：能
        unsigned int  res:16;                     // 预留
    } stD5StaSync;
}unD5StaSync;

// 试管识别位
typedef union _unTubeIdentify
{
    unsigned int uData;
    // 0 无需处理 1 到位无试管 2：到位有试管
    struct {
        unsigned int uTube1:2;
        unsigned int uTube2:2;
        unsigned int uTube3:2;
        unsigned int uTube4:2;
        unsigned int uTube5:2;
        unsigned int uTube6:2;
        unsigned int uTube7:2;
        unsigned int uTube8:2;
        unsigned int uTube9:2;
        unsigned int uTube10:2;
        unsigned int uRes:12;
    } stTubeIdentify;
}unTubeIdentify;
/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出变量申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
/******************************MP1200*NEW*CAN*d1 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdisBusy     wCmdisBusy;
        unsigned char  byMixPosRecord;    //混匀位置状态记录
        unsigned char  byPinchPosRecord;  //夹取及开盖试管位置记录，此位上位机未用，下位机仍用到
        unsigned char  byGlassPosRecord;  //拨载玻片位置记录
        unsigned char  bySampleSizeStatus;//样本量状态
        unD1StaSync    wD1StaSync;        //状态
        unsigned char  byHandFlg;
        unsigned char  byUpdataFinish;
        unsigned char  byUpdataReady;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_d1_src_dev;
#pragma pack()
#endif

/******************************MP1200*NEW*CAN*d2 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf     stoutCmd;
        unCmdisBusy    wCmdisBusy;
        unD2StaSync    wD2StaSync;
        unsigned char  bySlidBasketOpc;     // IN有无玻片篮，累加
        unsigned char  bySlidBasketOutOpc;  // OUT有无玻片篮，累加
        unsigned short int wBasketSlid;    // 玻片篮有无载玻片 0无 1有或者1-10;
        unErrSta       wErrSta;
        unsigned char  byHandFlg;           //接收成功回馈标志
        unsigned char  byUpdataFinish;      // 固件升级完成标志
        unsigned char  byUpdataReady;
        unsigned char  bySwapOutLen;
        unsigned short int waSwap[32];      // 数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_d2_src_dev;
#pragma pack()
#endif

/******************************MP1200*NEW*CAN*d3 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdisBusy     wCmdisBusy;
        unD3StaSync     wD3StaSync;
        unsigned short int byLevelCheck;     //粘度等级
        unsigned char  bySwapOutLen;        //预留
		unsigned char  byDyeAEmpty;        //染剂A回收次数
        unsigned short int waSwap[32];      //数据交换部分
        unsigned char  byHandFlg;           //接收成功回馈标志
        unsigned char  byUpdataFinish;      // 固件升级完成标志
        unsigned char  byUpdataReady;
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_d3_src_dev;
#pragma pack()
#endif

/******************************MP1200*NEW*CAN*d5 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byEnableEmc;                 //按键急诊，按一次加一次
        unsigned char byRightNewRackCnt;        //右平台红外光耦信号,信号为累加值
        unsigned short int byRightRackID;      //右侧扫描位试管架号
        unsigned short int byLeftRackID;       //左侧扫描位试管架号
        unTubeIdentify unTubeid;                 //试管号
        unD5StaSync    wD5StaSync;
        unsigned char byHandFlg;                //接收成功回馈标志
        unsigned char byUpdataFinish;           // 固件升级完成标志
        unsigned char byUpdataReady;
//        unsigned char byRes;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[20];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_d5_src_dev;
#pragma pack()
#endif

/******************************MP1200*NEW*CAN*D6 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdisBusy     wCmdisBusy;
        unsigned short int wBasketNum;          //玻片篮号码
        unsigned short int wAht20Data[2];       // 0 湿度 1 温度
        unsigned short int wBmp280Data[3];      // 0 大气压 1 温度 2 海拔高度
        unsigned short int wNtcTemp;            // 烘干温度值
        unsigned char  byHandFlg;               //接收成功回馈标志
        unsigned char  byUpdataFinish;          // 固件升级完成标志
        unsigned char  byUpdataReady;
//        unsigned char  byPressureErr;           //气源异常标志位1异常，0恢复       
        unsigned char  bySwapOutLen;
        unsigned short int waSwap[20];          // 数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_d6_src_dev;
#pragma pack()
#endif

/******************************MP1200*NEW*CAN*D1toD6 IO****************************/
//#if(true)
//#pragma pack(1)
//// 输出资源定义
//typedef struct
//{
//    stCmdTdf stIn;
//    struct {
//        stCmdStaTdf stoutCmd;
//        unsigned char wPressureData;   // 气压
//    } stOut;
//}_d1_6_src_dev;
//#pragma pack()
//#endif

/******************************MP1200*NEW*CAN*D3toD6 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned short int wPressureData[3];   // 0  1  2
        unsigned short int wTemp;             // 温度值
        unsigned char  byOpcVal;               //返回当前光耦状态
    } stOut;
}_d3_6_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*D2toD6 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdisBusy     wCmdisBusy;
        unsigned char  byHeatSlide;            //烘干位有无载玻片 0无 1有
        unsigned char  byBasketSlide;          //玻片篮有无载玻片 0无 1有或者1-10
        union {
            unsigned short int wSensor;        // 传感器信号S0-S7
            struct {
                unsigned short int wSensor0:1;
                unsigned short int wSensor1:1;
                unsigned short int wSensor2:1;
                unsigned short int wSensor3:1;
                unsigned short int wSensor4:1;
                unsigned short int wSensor5:1;
                unsigned short int wSensor6:1;
                unsigned short int wSensor7:1;
                unsigned short int wSensor8:1;
                unsigned short int wSensor9:1;
                unsigned short int wSensor10:1;
                unsigned short int wSensor11:1;
                unsigned short int wSensor12:1;
                unsigned short int wSensor13:1;
                unsigned short int wSensor14:1;
                unsigned short int wSensor15:1;
            } stSensor;
        } unSensor;
      //  unsigned short int wSWSencerVal;      //返回当前磁性开关状态
        union {
            unsigned short int wSw;// 磁性开关信号
            struct {
                unsigned short int wSw0:1;
                unsigned short int wSw1:1;
                unsigned short int wSw2:1;
                unsigned short int wSw3:1;
                unsigned short int wSw4:1;
                unsigned short int wSw5:1;
                unsigned short int wSw6:1;
                unsigned short int wSw7:1;
                unsigned short int wSw8:1;
                unsigned short int wSw9:1;
                unsigned short int wSw10:1;
                unsigned short int wSw11:1;
                unsigned short int wSw12:1;
                unsigned short int wSw13:1;
                unsigned short int wSw14:1;
                unsigned short int wSw15:1;
            } stSw;
        }unSWSencerVal;
       unsigned char  bySlidOpVal;            //玻片篮玻片检测光耦状态
    } stOut;
}_d2_6_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*D11 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char  byHandFlg;
        unsigned char  byUpdataFinish;   //  打开门光耦
        unsigned char  byUpdataReady;    //  有保养液光耦
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_d11_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*D11_D2 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
    } stOut;
}_d11_d2_src_dev;
#pragma pack()
#endif

#endif
