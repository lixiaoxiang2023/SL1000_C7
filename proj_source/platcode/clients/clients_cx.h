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
* File Name     : clients_cx.h
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
#ifndef __CLIENTS_CX_H_
#define __CLIENTS_CX_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "clients/clients_comm.h"
#include "clients/clients_cx_cmds.h"
/*******************************************************************************
Constant and Macro Definition 常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出变量申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/
/******************************SL1000*NEW*CAN*c1 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned short int wATRH[2];
        unsigned char byOpenDoorOP;   //  打开门光耦
        unsigned char byHaveBoxOP;    //  有保养液光耦
        unsigned char byHandFlg;       //接收成功回馈标志
        unsigned char byUpdataFinish;  // 固件升级完成标志
        unsigned char byUpdataReady;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[32];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_c1_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c2 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned short int wTemp[2];
        unsigned char  byDropOpc;               // 滴样位光耦状态
        unsigned char  byPaperCard;             // 试纸卡 1有，0 没有
        unsigned char  byStipBox;
        unsigned char  byCisAdjFag;
        unsigned char  byHandFlg;         //接收成功回馈标志
        unsigned char  byUpdataFinish;  // 固件升级完成标志
        unsigned char  byUpdataReady;
        unsigned char  bySwapOutLen;
        unsigned short int waSwap[20];             // 数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_c2_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c3 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char  byCmdisBusy[4];   //命令忙
        unsigned char  bySampleSta;      //采样状态 采样前为1，采样结束为2，滴样前为3，干化学滴完样为0，有形滴完样后清洗后为0
        unsigned char  byPreSampleSta;   //前处理 添加细胞稀释液前为0，加液结束后为1
        unsigned char  byDropDyeSta;     //滴染剂状态 加液前为0，滴完染剂为1，滴完纯化水为2
        unsigned char  byHandFlg;         //接收成功回馈标志
        unsigned char  byUpdataFinish;  // 固件升级完成标志
        unsigned char  byUpdataReady;
        unsigned char  byRes;
        unsigned char  bySwapOutLen;     //预留
		unsigned char  byCh3ohSta;       //甲醇无状态标志，1为无，0为有
        unsigned short int waSwap[32];      //数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_c3_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c5 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byPtube;                 //上位机判断是否需要转试管扫码
        unsigned char byReadySample;           //累加
        unsigned char byRackid;
        unsigned char byTubeid;
        unsigned char byEmcSta;                //累加
        unsigned char byIrState;
        unsigned char byFinsh;
        unsigned char byHandFlg;         //接收成功回馈标志
        unsigned char byUpdataFinish;  // 固件升级完成标志
        unsigned char byUpdataReady;
        unsigned char byRes;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[20];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_c5_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c6 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byCmdisBusy;
        unsigned short int wTemp;           //温度
        unsigned char byHandFlg;               //接收成功回馈标志
        unsigned char byUpdataFinish;          // 固件升级完成标志
        unsigned char byUpdataReady;
        unsigned char byRes;
        unsigned char byRes1;
        unsigned char byRes2;
        unsigned char bySwapOutLen;
        unsigned short int waSwap[20];
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_c6_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c7 IO****************************/
#if(true)
#pragma pack(1)
// 输出资源定义
typedef struct
{
    stCmdTdf stIn;
    struct {
        stCmdStaTdf stoutCmd;
        unsigned char byCmdisBusy[6];
        unsigned char byDropOpc;               //光耦状态 ++
        unsigned char bySlideCard;             // 染色位++
        signed short int byTemp[2];
        unsigned char byDropOilCard;           //滴油位玻片 ++
        unsigned char byCardBox;               //试纸盒光耦 1有，0 没有
        unsigned short int wMpuSwap[2];    //陀螺仪信息校准【0】X，【1】Y，当前【2】【3】
        unsigned char byHandFlg;         //接收成功回馈标志
        unsigned char byUpdataFinish;  // 固件升级完成标志
        unsigned char byUpdataReady;
        unsigned char byRet1;
        unsigned char byRet2;
        unsigned char bySwapOutLen;
        unsigned short int wTareVal;
        unsigned short int waSwap[32];             // 数据交换部分
        stVersionTdf stVer;
    } stOut;
    stUpgradeCmdTdf stDownload;
}_c7_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c8 IO****************************/
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
}_c8_src_dev;
#pragma pack()
#endif

/******************************SL1000*NEW*CAN*c24 IO****************************/
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
}_c26_src_dev;
#pragma pack()
#endif

#endif
