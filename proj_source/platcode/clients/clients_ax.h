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
* File Name     : client_ax.h
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
#ifndef __CLIENTS_AX_H_
#define __CLIENTS_AX_H_

/*******************************************************************************
Include files
*******************************************************************************/
#include "clients/clients_comm.h"
#include "clients/clients_ax_cmds.h"

/*******************************************************************************
Constant and Macro Definition
*******************************************************************************/

/*******************************************************************************
typedef
*******************************************************************************/

/******************************1600*NEW*CAN*A1 IO*******************************/
#if(true)
#pragma pack(1)

typedef struct _stA1StateTdf
{
    unsigned char byAdjChkFinish;                       // 选条完成
} stA1StateTdf;

typedef struct _stA1PaperTdf
{
    unsigned char byDetectNumber;                       // 光耦检测位是否有试纸
    unsigned char bySwerveNumber;                       // 转纸器是否有试纸
    unsigned char byPaperFinded;                        // 保存已经找到的试纸数
    unsigned char byPaperToFind;                        // 保存上位机要寻找的试纸数
    unsigned char byPaperAbandon;                       // 放弃掉缓存的试纸
} stA1PaperTdf;

volatile typedef struct _stPostionTdf
{
    unsigned short int wUpdownSteps;                    // 试纸抖动位置
    unsigned short int wCheckSteps;                     // 光纤检测位置
    unsigned short int wObverseSteps;                   // 转纸器出带位置
    unsigned short int wInverseSteps;                   // 转纸器进带位置
    unsigned short int wWideSlotSteps;                  // 宽槽翻试纸位置
    unsigned short int wPaperType;                      // 试纸类型   5:US1000
} stA1PostionTdf;

typedef struct _stA1OpStaTdf
{
    unsigned char abyOpcState[2];                       // abyOpcState[0]M1大滚筒复位光耦值，
    unsigned short int awFiberValue[2];                 // abyFiberValue[0]有无试纸光纤采样值
} stA1OpStaTdf;

typedef struct
{
    stCmdStaTdf             stoutCmd;

    stA1StateTdf            stState;
    stA1PaperTdf            stPaper;
    stA1PostionTdf          stPost;
    stA1OpStaTdf            stOpt;
    stVersionTdf            stVer;

    stUpgradeFlagTdf        stFwFlag;
    unsigned char           ucNA[25];                       // 预留字节提高协议兼容性
} stA1OutTdf;

typedef struct _stIoA1Tdf
{
    stCmdTdf stIn;
    stA1OutTdf stOut;
    stUpgradeCmdTdf stDownload;
} stIoA1Tdf;
#pragma pack()
#endif

/*****************************1600*NEW*CAN*A2 IO*******************************/
#if(true)
#pragma pack(1)

typedef struct _stA2RunStaTdf
{
    unsigned char byRunSta;
} stA2RunStaTdf;

typedef struct _stA2PaperTdf
{
    unsigned char byDropPaper;                          // 滴样位是否有试纸
    unsigned char byStrapPaper;                         // 皮带上试纸的条数
    unsigned char byPaperTime;                          // 皮带上试纸的在皮带上的时间
    unsigned char byRes;                                // 保留
} stA2PaperTdf;

typedef struct _stA2InfoTdf
{
    unsigned char abyDetectTimStart[10];                // 如果置1，则定时器累加paper_detect_time[0]
    unsigned short awPaperDetectTime[10];               // 该试纸被送上皮带的时间，如果皮带每0.005S走一格，则12000（60S/5MS)的时候测试
    unsigned char byResOk;                              // 结果正常计数
    unsigned char byResTubeNo;                          // 试管号
    unsigned char byResTuberackNo;                      // 试管架号
    unsigned char byPaperTodetectCnt;                   // 已经滴样、待检测的试纸条数
    unsigned char abyPaperRes[43];                      // 样本结果数据
    unsigned char byResNulity;                          // 试纸检测结果有效位，0有效，01检测位无试纸，02试纸反带或类型错误，03无样本，
    unsigned char byPaperNulity;                        // 试纸滴样有效位，1无效，0有效
    unsigned char abyRefGrade[16];                      // 反射率分档结果输出
} stA2InfoTdf;

typedef struct _stA2PostionTdf
{
    unsigned short wDropSteps;                          // 落带位置到滴样位置
    unsigned short wDetectSteps;                        // 落带位置到皮带位置
} stA2PostionTdf;

typedef struct _stA2ModeTdf
{
    unsigned char byPaperType;                          // 试纸类型，0：11F；   1：12F；   2：14G   3:线性带;   4;彩带
    unsigned char byAging;                              // 老练模式
} stA2ModeTdf;

typedef union _A2OptTdf
{
    unsigned char byA2Opt;
    struct
    {
        unsigned char bitOptPushMoRst : 1;              // M3试纸拨杆复位光耦值
        unsigned char bitOptStripExist : 1;             // 滴样位有无试纸检测光耦值
        unsigned char bitOptStripTurn : 1;              // 滴样位平台反带检测光耦值
        unsigned char bitOptWastepaper : 1;              // 废纸盒试纸检测光耦值
        unsigned char bitRes : 4;                       // 保留
    };
} A2OptTdf;

typedef struct _stA2OptTdf
{
    A2OptTdf unOptSta;                                  // 三个光耦状态
    unsigned short int wTemp;                           // 温度
} stA2OptTdf;

typedef struct _stA2LightTdf {
    unsigned short awLight[5];                          // CIS光值
} stA2LightTdf;

typedef struct _stA2HslTdf {
    unsigned short awHsl[18];                           // 各项目HSL值
} stA2HslTdf;

typedef struct _stFpgaVersionTdf {
    unsigned short dwFpgaVer;                           // FPGA版本
} stFpgaVersionTdf;

typedef struct _stA2CalRgbTdf {
    unsigned char abyRBG[3];                           // 空白块RGB值
} stA2CalRgbTdf;

typedef struct
{
    stCmdStaTdf         strOutCmd;                      // 命令状态

    stA2RunStaTdf		stA2RunSta;                     // 命令执行状态，非0：执行中，0：空闲
    stA2PaperTdf        stPaperRg;                      // 试纸状态
    stA2InfoTdf         stPaperInfo;                    // 试纸结果信息
    stA2PostionTdf      stPos;                          // 试纸拨动位置控制
    stA2ModeTdf         stMode;                         // 检测模式，试纸or老练
    stA2OptTdf          stOpt;                          // 光耦状态，温度值
    stA2LightTdf        stLight;                        // CIS光值
    stA2HslTdf          stHsl;                          // 各项目HSL值
    stVersionTdf        stVer;                          // 版本
    stUpgradeFlagTdf    stFwFlag;
    unsigned short      usSnID;                         // 试管SN号
    unsigned char       usPushMoRst;                    // 拨杆电机状态位置 0：复位 1：滴样位 2：皮带位
    stA2CalRgbTdf       stCalRgb;                       //空白块RGB
    stFpgaVersionTdf    stFpVer;                        //FPGA版本
    unsigned char       ucNA[17];                       // 预留字节提高协议兼容性
}stA2OutTdf;

typedef struct
{
    stCmdTdf stIn;
    stA2OutTdf stOut;
    stUpgradeCmdTdf stDownload;
} stA2IoTdf;

#pragma pack()
#endif

/******************************1600*NEW*CAN*A3 IO*******************************/
#define __stIn stCmdTdf
#define __StDownLoad stUpgradeCmdTdf
#pragma pack(1)

typedef struct _stA3PostionTdf
{
    unsigned short int WashSteps;                       // 清洗孔位置
    unsigned short int NeedleWashSteps;                 // 针头清洗位置
    unsigned short int WashDownSteps;                   // 清洗下降高度
    unsigned short int SampleSteps;                     // 正常加样位置
    unsigned short int EmcSteps;                        // 急症位
    unsigned short int SampleDownSteps;                 // 采样深度
    unsigned short int P12F_DropDebug;                  // 12F开始滴样位置
    unsigned short int DropDownSteps;                   // 滴样高度
    unsigned short int P12F_Drop_Interval;              // 滴样间隔
    unsigned short int P14F_DropDebug;                  // 14F开始滴样位置
    unsigned short int P14F_Drop_Interval;              // 滴样间隔
} stA3PostionTdf;

typedef struct _stA3DropVal
{
    unsigned short  w11F[12];
    unsigned short  w12F[13];
    unsigned short  w14F[15];
    unsigned short  w11FA[12];
    unsigned short  w12FA[13];
    unsigned short  w14FA[15];
} stA3DropVal;

typedef struct _stA3OptTdf
{
    union _unOpt {
        unsigned char strucOpt[2];
        struct _stOptBit {
            unsigned char ubMYOpt:1;                    // 采样针向上限位光耦
            unsigned char ubMXOpt:1;                    // 采样针平移限位光耦
            unsigned char ubMBOpt:1;                    // 小柱塞泵限位光耦
            unsigned char ubMAOpt:1;                    // 大柱塞泵限位光耦
            unsigned char ubClearUpOpt:1;               // 清洗液灌上浮子
            unsigned char ubClearDownOpt:1;             // 清洗液灌下浮子
            unsigned char ubKeepFitUpOpt:1;             // 保养液瓶上浮子
            unsigned char ubKeepFitDownOpt:1;           // 保养液瓶下浮子
            unsigned char ubWasterBottleOpt:1;          // 废液桶浮子
            unsigned char ubWasterJamOpt:1;             // 清洗液槽漏液检测
        } stOptBit;
    } unOpt;

    // unsigned char  OpcState[10];
} stA3OptTdf;

typedef struct _stA3ModeTdf
{
    unsigned char PaperType;                            // 试纸类型，0：11F；   1：12F；   2：14G
    unsigned char StrongWash;                           // 清洗方式，0，普通清洗方式；   1，加强清洗方式
    unsigned char Blend;                                // 混匀模式开关，0：关闭混匀；   1开启混匀
    unsigned char Liquid;                               // 液面感应开关， 0：开启液面感应；  1：关闭液面感应
    unsigned char SampleDetection;                      // 样本检测， 0：打开样本检测    1：关闭样本检测     // 样本检测功能为，单纯检测试管内是否有液体，采样针还是插到试管底部采样
    unsigned char Sg:1;                                   // 尿比重开关模式，0：关闭尿比重  1：打开尿比重
    unsigned char Cond:1;                                   // 尿比重开关模式，0：关闭电导率  1：打开电导率
    unsigned char ucCollisionOptEn:1;                                   // 防撞光耦使能 0 关闭 1开启
    unsigned char ucNA:5;                                   // 预留
} stA3ModeTdf;

typedef struct _stA3SgTdf
{
    unsigned char ColorData[3];                         // 颜色检测
    unsigned char ColorCalib[3];                        // 颜色校准
    unsigned short ColorLev;                            // 颜色代码
    unsigned short TurbLev;                             // 浊度检测
    unsigned short SgrLev;                              // 比重检测
    short TurbCalib[2];                                 // 浊度校准
    short SgCalib[2];                                   // 比重校准
    unsigned short wSysVer;
} stA3SgTdf;
/*
V1.0SG颜色分档：0x21    红色       V2.0SG颜色分档：0x21    红色
0x22    棕色                       0x22    棕色
0x23	黄色                       0x23	   黄色
0x24	绿色                       0x24	   绿色
0x13	浅黄色                     0x27	   浅黄色
0x33	深黄色                     0x28	   稻黄色
0x27	琥珀色                     0x29	   琥珀色
0xfe	其他                       0xfe	   其他
0x00	无色                       0x00	   无色
*/
typedef struct _stA3SgCalibTdf
{
    unsigned char    ColorCalib;                        // 颜色校准
    unsigned char    SgCalib;                           // 比重校准
    unsigned char    TurbCalib;                         // 浊度校准
    unsigned char    res;                               // 保留用于对齐
} stA3SgCalibTdf;

typedef union
{
    struct
    {
        unsigned short bRunningSta:2;                   // A3采样针位置状态,0清洗位置，1采样完成回到清洗位，2到滴样位
        unsigned short bRst:1;                          //
        unsigned short bCollect:1;                      //
        unsigned short bDrop:1;                         //
        unsigned short bWashPipe:1;
        unsigned short bMaintainPipe:1;
        unsigned short bWashPinTop:1;
        unsigned short bClearDroppipe:1;
        unsigned short running:1;
        unsigned short bSampleDetection:1;
        unsigned short usNA:5;
    }BIT;
    unsigned short  usRunSt;
}RunSt;                                                 // 光耦状态

/*电导率调试*/
typedef struct
{
    unsigned short wCondAmpl;                           // 振幅
    unsigned short wEncourage;                          // 激励
    unsigned short usOsm;                               // 渗透压
    unsigned short wDuctVal;                            // 检测值
    unsigned short wDuctTemp;                           // 检测温度
    unsigned short wCalTemp;                            // 校准温度
}stA3CondDebug;

typedef struct {
    stCmdStaTdf         strOutCmd;
    stA3PostionTdf      strOutPosition;
    stA3DropVal         strOutDropValue;
    stA3OptTdf          strOutOpt;
    stA3ModeTdf         strOutMode;
    stA3SgTdf           strOutSg;
    stA3SgCalibTdf      strOutSgCalib;
    stVersionTdf        strOutVer;
    RunSt               RunSta;
    stUpgradeFlagTdf    stFwFlag;
    stA3CondDebug       strCondDebug;                   // 电导率调试、
    unsigned char       ucMaintenanPumpCx; //保养泵圈数
    unsigned char       ucNA[12];                       // 预留字节提高协议兼容性
} __stOut;

typedef struct
{
    __stIn              stIn;
    __stOut             stOut;
    __StDownLoad        stDownload;
}A3IoTdf;

#pragma pack()

/******************************PLUS*A5 IO*******************************/
#if(true)
#pragma pack(1)
#define __StDownLoad stUpgradeCmdTdf
typedef struct _stA5OptTdf
{
    // 光耦
    // 右边拨杆电机光耦  左边拨杆电机光耦  中间找试管电机光耦  联机电机左光耦
    // 右边内侧光耦      右边外侧光耦       左边内侧光耦       左边外侧光耦
    // 联机电机右光耦      红外对射光耦      急诊位光耦
    union __unOpt {
        unsigned short int wOPT;                        // 按位 ,1S刷新
        struct __stOptBit {
            unsigned short int wMoRightOpt : 1;
            unsigned short int wMoLeftOpt  : 1;
            unsigned short int wMoUpOpt    : 1;
            unsigned short int wOpt_L_Online: 1;        // 联机电机左光耦
            unsigned short int wOpt_RightU : 1;         // 右上，右内
            unsigned short int wOpt_RightD : 1;         // 右下, 右外
            unsigned short int wOpt_LeftU  : 1;         // 左上，左内
            unsigned short int wOpt_LeftD  : 1;         // 左下，左外
            unsigned short int wOpt_R_Online:1;         // 联机电机右光耦
            unsigned short int wOpt_IR     : 1;         // 红外对射光耦, 右侧是否有试管架
            unsigned short int wOpt_Emer   : 1;         // 急诊位光耦
            unsigned short int wOptRes     : 5;
        } stOptBit;
    } unOpt;
} stA5OptTdf;

typedef struct _stA5StateTdf
{
    union __unStste {
        struct __stState {
            unsigned int byResetEnd  :1;                // 只有2000在用
            unsigned int byRightRack :1;                // 右侧试管架,红外对射检测1右侧有试管架，0，右侧无试管架
            unsigned int byEmcState  :1;                     // 急诊位信息，1有急诊，0没有急诊，设计成命令B5_EmerRead查询
            unsigned int bitRes      :29;
        } stState;

        unsigned int dwState;
    } unStste;
} stA5StateTdf;

//out
typedef struct _stSampleDataTdf                         // 中间采样位的信息
{
    unsigned char  bySampleCnt;                         // 递增代表有采样试管
    unsigned char  bySampleFlag;                        // 采样位有无试管标志，1，代表找到新的试管，0没有试管
    unsigned char  bySampleRackid;                      // 采样位试管架号
    unsigned char  bySampleTubeid;                      // 采样位试管号，试管架试管为1-10

    unsigned char  abyBarCode[20];                      // 条码，虽然不用，先占位，给1600
    unsigned char  byEmerCnt;                           // 递增代表有急诊

    unsigned char  byReadyTube;                         // 下一个试管状态，0无试管，1有试管，试管号为 byBarCodeTubeid+1

    unsigned char  byNewRackCnt;                        // 递增代表找到新试管架
    unsigned char  byNewRackid;                         // 新试管架的架号

    // 扫码位信息
    unsigned char  byBarCodeCnt;
    unsigned char  byBarCodeTubeFlag;                   // 扫码位是否有试管号,上位机判断是否需要转试管扫码
    unsigned char  byBarCodeTubeid;                     // 扫码位试管号，1-10表示扫码位在试管架上的试管号，11,12可代表推试管架的Step，0表示试管架不在扫码位下方，其他数字非法

    // 预留信息位置
    unsigned char  byRe0[5];
} stSampleDataTdf;

typedef union _unReq {
    unsigned char byOutA5Req;                           // 左侧出A5模块请求,1代表有请求
    unsigned char byInA5State;                          // 右侧允许进A5状态，0，代表允许下发emTubyRack_In_A5请求，
}unReqTdf;

typedef struct _stA5RackDataTdf                         // 左右两侧试管架信息，联机的时候需要
{
    unsigned char byDataCnt;                            // 递增代表信息更新

                                                        //  1，代表收到emTubyRack_In_A5，并且已经准备好了，等待管架进入，开始计时60秒
    unReqTdf   unReq;                                   //  2，代表忙，不允许emTubyRack_In_A5请求, 不允许试管架进入
    unsigned char byRackNum;                            // 试管架计数
    unsigned char byRe;
    unsigned char abyRackid[8];                         // 试管架号容器，一侧最多放6个试管架，方便管理，直接开8个试管架的空间
} stA5RackDataTdf;

typedef struct _stA5SetTdf
{
    unsigned char byOnlineModel;                        // 设置联机模式 0x00：单机
                                                        //  0x01:1200联机模式  //  0x02:1280联机模式
                                                        //  0x03：IQ200联机    //  0x04：爱威
                                                        //  0x05:希森美康      //  0x06:迈瑞
                                                        //  0x11：IPU联机。    //  0x09:UD2000

    unsigned char byCycle_OnOff;                        // 设置循环模式 0x00: 关闭循环 0x01:开启循环(自动关闭联机模式)
    unsigned char byTubeRackNo_OnOff;                   // 试管架号功能开关，0,关闭试管架号检测； 1，开启试管架号检测
    unsigned char byBarCode_OnOff;                      // 条码扫描开关,0,关闭； 1，开启
    unsigned char byA5Type;                             // 进样机构所在联机位置 0：非末端 1：末端
    unsigned char byA6Online;                           // A6联机标志位
    unsigned char bySetRes;                             // reserve
} stA5SetTdf;                                           // 运行模式

typedef struct _stA5PostionTdf
{
    //    DebugCmdNone;
    unsigned short int right_st_pos;                    // 右拨杆起始位置
    unsigned short int right_intvl;                     // 右拨杆间距
    unsigned short int left_st_pos;                     // 左拨杆起始位置
    unsigned short int left_intvl;                      // 左拨杆间距

    unsigned short int wOnline_pos;                     // 试管架进桥位置
    unsigned short int wCycSteps;                       // 右侧传送试管架位置
    unsigned short int tube_1_pos;                      // 第一试管检测位
    unsigned short int tube_10_pos;                     // 第十试管检测位

    unsigned short int rack2left_pos;                   // 试管架移至左侧步数
    unsigned short int RetestCompen_pos;                // 复测杆试管架光耦补偿位
    unsigned short int Threshold;                       // 阈值
    unsigned short int Points;                          // 判断点数
    unsigned short int CurValue;                        // 电流值
    unsigned short int wDebugRe[2];
} stA5PostionTdf;                                       // 调试信息

typedef struct _stA5UVAdcTdf
{
    unsigned short int CalValue;                        // 标准值
    unsigned short int LightValue;                      // 实际光亮值
    unsigned short int ValidPoints;                     // 有效点数
    unsigned short int TubeResult;                      // 试管有无结果值
}stA5UVAdcTdf ;                                         // A5紫外采样资

typedef union
{
    struct
    {
        unsigned int bM1RightRstErr:1;                  // 右侧M1电机复位故障
        unsigned int bM2LeftRstErr:1;                   // 左侧M2电机复位故障
        unsigned int bM3UpRstErr:1;                     // 找试管M3电机复位故障
        unsigned int bLeftNoSpace:1;                    // 左侧试管架满
        unsigned int bNoTuberack:1;	                    // 无待测试管架  （未使用）
        unsigned int bTuberackNumberTimeout:1;          // 试管架号扫描超时

        unsigned int bOverTimeOnBridgeErr:1;            // 进样机构进桥超时
        unsigned int bM6ResetErr:1;                     // M6复位故障
        unsigned int bM4DownRightRstErr:1;
        unsigned int bM4DownLeftRstErr:1;
        unsigned int bRightInsideOpcErr:1;              // 进样机构右内侧光耦故障
        unsigned int bRightOutsideOpcErr:1;             // 进样机构右外侧光耦故障 (未使用)
        unsigned int bLeftInsideOpcErr:1;               // 进样机构左内侧光耦故障
        unsigned int bLeftOutsideOpcErr:1;              // 进样机构左内侧光耦故障
        unsigned int bFbadjerr:1;
        unsigned int ucNA2:17;                          // 预留
    }BIT;

    unsigned int uiBuf;
}ERR_B5PLUS;//

typedef union
{
    struct
    {
        unsigned int bA7_Rst_Err:1;                     // 收容复位故障
        unsigned int bA7_Plat_Full_Err:1;               // 收容机构试管架满
        unsigned int bA7_Time_Out:1;                    // 收容机构联机超时
        unsigned int bA7_Offline:1;                     // 收容机构掉线

        unsigned int ucNA2:28;                          // 预留
    }BIT;
    unsigned int uiBuf;
}ERR_A7PLUS;

typedef struct _stIoA5Tdf
{
    stCmdTdf                stIn;
    stCmdTdf                inCmd2;                     // 联机用
    stCmdStaTdf             stOut;
    stCmdStaTdf             outCmd2;                    // 联机用

    stA5OptTdf              outOpt;
    stSampleDataTdf         outSampleData;              // 中间采样位信息
    stA5StateTdf            outState;

    stA5RackDataTdf         outRightRackData;           // 右侧试管架信息
    stA5RackDataTdf         outLeftRackData;            // 左侧试管架信息

    stA5SetTdf              outSet;
    stA5PostionTdf          outDebug;
    stA5UVAdcTdf            outUVadc;

    stVersionTdf            outVer;
    stUpgradeFlagTdf        stFwFlag;
    ERR_B5PLUS              outErrBits;			        // 故障位
    ERR_A7PLUS              outOnlineErrBits;			// 联机故障位
    unsigned char           ucNA[17];                   // 预留字节提高协议兼容性
    __StDownLoad            stDownload;
} stIoA5Tdf;

extern stIoA5Tdf g_stA5;
#pragma pack()
#endif

#if(true)
#pragma pack(1)
typedef struct _stA6OptTdf
{
    union __unA6Opt {
        unsigned int  uiOPT;                      // 按位 ,1S刷新
        struct __stA6OptBit {
            unsigned int wMoRightOpt        : 1;
            unsigned int wMoInOpt           : 1;
            unsigned int wDetectRackOpt     : 1;
            unsigned int wOptRes            : 29;
        } stA6OptBit;
    } unA6Opt;
} stA6OptTdf;

typedef struct _stA6SetTdf
{
    unsigned short          usSetStartPos;              // 扩容模块上拨杆调试步数  //不使用，预留
    unsigned short          usSetIntvlPos;              // 扩容模块试管架间隔步数
    unsigned short          usSetToA5Pos;               // 扩容模块推到A5模块步数
} stA6SetTdf;

typedef struct {
    stCmdStaTdf             strOutCmd;
    stA6OptTdf              strOpt;
    stA6SetTdf              strSetTdf;
    stVersionTdf            outVer;
    stUpgradeFlagTdf        stFwFlag;
    unsigned char           ucNA[25];                   // 预留字节提高协议兼容性
} __A6stOut;

typedef struct
{
    __stIn                  stIn;
    __A6stOut               stOut;
    __StDownLoad            stDownload;
}stIoA6Tdf;
#pragma pack()
#endif

#if(true)
#pragma pack(1)
typedef struct
{
    unsigned char           bySta;                      // 灯状态
}LedSta_;

typedef struct
{
    unsigned char           byKeyEven[3];               // 按键事件
}KeyEven_;

typedef struct {
    stCmdStaTdf             strOutCmd;
    KeyEven_                KeyEven;
    LedSta_                 LedSta;
    stVersionTdf            outVer;

    stUpgradeFlagTdf        stFwFlag;
    unsigned char           ucNA[25];                       // 预留字节提高协议兼容性
} __A8stOut;

typedef struct
{
    __stIn                  stIn;
    __A8stOut               stOut;
    __StDownLoad            stDownload;
}stIoA8Tdf;

#pragma pack()
#endif

/******************************PLUS*A9 IO*******************************/
#if(true)
#pragma pack(1)
typedef union
{
    struct
    {
        unsigned short bM1RestErr:1;                    // m1复位故障
        unsigned short bScancodeReadErr:1;              // 读条码失败
        unsigned short bMotorStepErr:1;                 // 垂直深度设置失败
        unsigned short ucNA2:13;                        // 预留
    }BIT;

    unsigned short uiBuf;
}SCANCODE_FAULT;

typedef struct _stA9DataTdf {
    unsigned char abyBarCode[20];                       // 条码
    unsigned char abyOptc[2];                           // 光耦
    unsigned char byTubeCnt;                            // 识别试管位，累加1
    unsigned char ucHandFlg;                            // 接收成功回馈标志
    unsigned char usUpdataFinish;                       // 固件升级完成标志
    unsigned char usUpdataReady;                        // 固件升级准备完成标志
    unsigned char ucScanEn;                             // 扫描使能
    SCANCODE_FAULT byErr;                               // UD2000故障
    unsigned char  bySwapOutLen;                        // 输出数据交换区数据长度
    unsigned short int waSwap[6];                       // 输出数据交换区
} stA9DataTdf;

typedef struct _stIoA9Tdf
{
    stCmdTdf               stIn;
    stCmdStaTdf            stOut;
    stA9DataTdf            outDat;
    stVersionTdf           outVer;
    stUpgradeCmdTdf        outUpgradeCmd;
} stIoA9Tdf;
#pragma pack()
#endif

#endif
