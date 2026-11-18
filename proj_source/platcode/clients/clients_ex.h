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
* Project       : UD-1290
* Author        : pengshen
* Date          : 2026.05.13
* Description   : UD-1290
*
* modify logs:
* 2026.04.07 pengshen    create
******************************************************************************/
#ifndef CLIENTS_EX_H_
#define CLIENTS_EX_H_

/******************************************************************************
*Include files
******************************************************************************/
#include "clients/clients_comm.h"
#include "clients/clients_ex_cmds.h"

/******************************************************************************
*Data define
******************************************************************************/
typedef union {
    unsigned short int wD1StaSync; // 状态同步
    struct {
        unsigned short int  MicroRuning:1;        //显微镜运行中状态
        unsigned short int  res:15;
    } stE1StaSync;
}unE1StaSync;

typedef union {
    unsigned int wE5StaSync; // 状态同步
    struct {
        unsigned int  byTubeSamplePos:1;          //试管检测位有无试管标致 1或0
        unsigned int  byTubeScanBarcodePos:1;     //条码扫描位有无试管  1或0
        unsigned int  byTubeSamplingPos:1;         //采样位有无试管  1或0
        unsigned int  byOnlineModel:1;            //进样联机模式标志位  1联机 0单机
        unsigned int  byFinsh:1;                  //试管架推到左平台结束标致
        unsigned int  byOutsideBeltWorkFlag:1;    //外皮带工作标志位 1工作 0停止
        unsigned int  byOnlineInRackFlag:1;       //联机试管架进入标志位，1有 0无
        unsigned int  byLeftPlatHas:1;            // 左平台有无试管架 0：没有 1：有
        unsigned int  byRightPlatSig:1;           // 右平台试管架对射信号 0：没有 1：有
        unsigned int  res:23;                     // 预留
    } stE5StaSync;
}unE5StaSync;

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
}unCmdIsBusy;
/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出变量申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
/******************************UD1290*NEW*CAN*E1 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdIsBusy     wCmdisBusy;
        unE1StaSync    wE1StaSync;        //状态
        unsigned char  byHandFlg;
        unsigned char  byUpdataFinish;
        unsigned char  byUpdataReady;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_e1_src_dev;
#pragma pack()
#endif
/******************************UD1290*NEW*CAN*E2 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdIsBusy     wCmdisBusy;
        unsigned char  byHandFlg;
        unsigned char  byUpdataFinish;
        unsigned char  byUpdataReady;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_e2_src_dev;
#pragma pack()
#endif
/******************************UD1290*NEW*CAN*E3 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unCmdIsBusy     wCmdisBusy;
        unsigned char  byHandFlg;
        unsigned char  byUpdataFinish;
        unsigned char  byUpdataReady;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_e3_src_dev;
#pragma pack()
#endif
/*******************************NEW*CAN*E5 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byEmcNum;                 //急诊计数器
        unsigned char byRightNewRackCnt;        //右平台红外光耦信号,信号为累加值
        unsigned short int byRightRackID;      //右侧扫描位试管架号
        unsigned char  byTubeid;                 //试管号
        unE5StaSync    wE5StaSync;
        unsigned char byHandFlg;                //接收成功回馈标志
        unsigned char byUpdataFinish;           // 固件升级完成标志
        unsigned char byUpdataReady;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_e5_src_dev;
#pragma pack()
#endif
/******************************E6 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byWriteIcSuccess;       //用于写卡成功
        unsigned char byDoorSta;              //1关上，0打开
        unsigned char byPwerDown;
        unsigned char byHandFlg;             //接收成功回馈标志
        unsigned char byUpdataFinish;        // 固件升级完成标志
        unsigned char byUpdataReady;
        unsigned char byRes;
		unsigned char bySwapOutLen;
        unsigned short int  waSwap[32];          // 数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_e6_src_dev;
#pragma pack()
#endif

/******************************E7 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byHandFlg;             //接收成功回馈标志
        unsigned char byUpdataFinish;        // 固件升级完成标志
        unsigned char byUpdataReady;
        uint16_t iTEMP;                      //内部温度值
	uint16_t eTEMP;                      //外部温度值
        uint8_t  Adj_state;                  //校准状态
	uint8_t  Tube_state;                 //导管状态
	uint32_t Conductivity;               //电导率
	uint32_t RS_Adj;                     //校准电阻值
	uint32_t RS;                         //测量电参量
	uint16_t OSM;                        //渗透压
	unsigned char bySwapOutLen;
        unsigned short int  waSwap[32];      // 数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_e7_src_dev;
#pragma pack()
#endif
#endif
