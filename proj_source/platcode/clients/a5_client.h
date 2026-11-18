/*******************************************************************************
* Copyright URIT Co.,Ltd. All Right Reserved
*
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*******************************************************************************/

/*******************************************************************************
* File Name     : a5_client.h
* File Path     : DevLib
* Project       : u1600plus
* Author        : qinzhifu
* Date          : 2022.11.01
* Description   : 进样机构模块
*
* modify logs:  Copyright urit Co.,Ltd.All Right Reserved
* 2022.11.01 qinzhifu   create
*******************************************************************************/
#ifndef A5Client_H
#define A5Client_H

/*******************************************************************************
Include file
*******************************************************************************/
#ifdef XCANBASE_INCLUDE
    #include "LibXcan/xcaniobase.h"
#else
    #include "xcan/xcaniobase.h"
#endif
#include "clients/clients_def.h"

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class A5ClientD;
class XCAN_EXPORTS_API A5Client : public ClientBase
{
public:
    //command_2
    XcanIoDesc *in_byCmd2 = nullptr;                // 联机命令码
    XcanIoDesc *in_byCmdCnt2 = nullptr;             // 联机命令码
    XcanIoDesc *in_wParam2 = nullptr;               // 联机命令参数 长度5

    //cmd_State_2
    XcanIoDesc *out_byCmd2 = nullptr;               // 联机命令
    XcanIoDesc *out_byCmdCnt2 = nullptr;            // 联机命令计数器
    XcanIoDesc *out_byErrCode2 = nullptr;           // 联机错误码
    XcanIoDesc *out_byErrorCnt2 = nullptr;          // 联机错误计数器

    // error bits
    XcanIoDesc *out_dwErrBits = nullptr;

    //opt
    XcanIoDesc *out_wOPT = nullptr;                 // 光耦表示状态，按位取，详见结构体

    //state_a5
    XcanIoDesc *out_dwState= nullptr;               // 运行状态，按字节取，详见结构体

    //_SampleData_
    XcanIoDesc *out_bySampleCnt = nullptr;          // 递增代表有采样试管
    XcanIoDesc *out_bySampleFlag = nullptr;         // 代表有采样试管有无(检测位）
    XcanIoDesc *out_bySampleRackid = nullptr;       // 采样位试管架号
    XcanIoDesc *out_bySampleTubeid = nullptr;       // 采样位试管号，试管架试管为1-10
    XcanIoDesc *out_abyBarCode = nullptr;           // 条码，虽然不用，先占位，给1600 长度20
    XcanIoDesc *out_byEmerCnt = nullptr;            // 递增代表有急诊
    XcanIoDesc *out_byReadyTube= nullptr;           // 试管检测的结果，0无试管，1有试管，试管号为 byBarCodeTubeid-1
    XcanIoDesc *out_byNewRackCnt= nullptr;          // 递增代表找到新试管架
    XcanIoDesc *out_byNewRackid= nullptr;           // 新试管架的架号
    XcanIoDesc *out_byBarCodeCnt= nullptr;          // 条码计数器
    XcanIoDesc *out_byBarCodeTubeFlag= nullptr;     // 扫码位是否有试管号,上位机判断是否需要转试管扫码
    XcanIoDesc *out_byBarCodeTubeid= nullptr;       // 扫码位试管号，1-10表示扫码位在试管架上的试管号，11,12可代表推试管架的Step，0表示试管架不在扫码位下方，其他数字非法
    XcanIoDesc *out_abyRe0= nullptr;                // 预计信息位 长度5

    //RackDataRight
    XcanIoDesc *out_byDataCntR = nullptr;           // 递增代码代表信息更新,
    XcanIoDesc *out_byOutA5ReqR = nullptr;          // 左侧出A5请求，1代表有请求,
                                                    // 这里是联合体 byInA5State 右侧允许进A5状态，0，代表允许下发emTubyRack_In_A5请求，
    XcanIoDesc *out_byRackNameR = nullptr;          // 试管架计数
    XcanIoDesc *out_byReR = nullptr;                // 预留
    XcanIoDesc *out_abyRackidR = nullptr;           // 试管架号容器，一侧最多放6个试管架，方便管理，直接开8个试管架的空间 长度为8

    //RackDataLeft
    XcanIoDesc *out_byDataCntL = nullptr;           // 递增代码代表信息更新,
    XcanIoDesc *out_byOutA5ReqL = nullptr;          // 左侧出A5请求，1代表有请求,
                                                    // 这里是联合体 byInA5State 右侧允许进A5状态，0，代表允许下发emTubyRack_In_A5请求，
    XcanIoDesc *out_byRackNameL = nullptr;          // 试管架计数
    XcanIoDesc *out_byReL = nullptr;                // 预留
    XcanIoDesc *out_abyRackidL = nullptr;           // 试管架号容器，一侧最多放6个试管架，方便管理，直接开8个试管架的空间 长度为8

    //Set
    XcanIoDesc *out_byOnlineModel= nullptr;         // 设置联机模式 0x00：单机
                                                    //  0x01:1200联机模式  //  0x02:1280联机模式
                                                    //  0x03：IQ200联机    //  0x04：爱威
                                                    //  0x05:希森美康       //  0x06:迈瑞
                                                    //  0x11：IPU联机。
    XcanIoDesc *out_byCycleOnOff = nullptr;         // 设置循环模式 0x00: 关闭循环 0x01:开启循环(自动关闭联机模式)
    XcanIoDesc *out_byTubeRackNoOnOff = nullptr;    // 试管架号功能开关，0,关闭试管架号检测； 1，开启试管架号检测
    XcanIoDesc *out_byBarcodeOnOff = nullptr;       // 条码扫描开关,0,关闭； 1，开启
    XcanIoDesc *out_byA5Type = nullptr;             // 进样机构所在联机位置 0：非末端 1：末端
    XcanIoDesc *out_byA6Online = nullptr;           // A6联机 0,关闭； 1，开启
    XcanIoDesc *out_abySetRe = nullptr;             // 设置缓存区  长度2

    //Postion
    XcanIoDesc *out_wRightStPos = nullptr;          // 右拨杆起始位置
    XcanIoDesc *out_wRightIntvl = nullptr;          // 右拨杆间距
    XcanIoDesc *out_wLeftStPos = nullptr;           // 左拨杆起始位置
    XcanIoDesc *out_wLeftIntvl = nullptr;           // 左拨杆间距

    XcanIoDesc *out_wOnlinePos = nullptr;           // 试管架进桥位置
    XcanIoDesc *out_wCycSteps = nullptr;            // 右侧传送试管架位置
    XcanIoDesc *out_wTube1Pos = nullptr;            // 第一试管检测位
    XcanIoDesc *out_wTube10Pos = nullptr;           // 第十试管检测位

    XcanIoDesc *out_wRack2LeftPos = nullptr;        // 试管架移至左侧步数
    XcanIoDesc *out_wRetestCompenPos = nullptr;     // 复测杆试管架光耦补偿位

    XcanIoDesc *out_wThreshold = nullptr;           // 阈值  //紫外采样资源这三个变量做固件写入
    XcanIoDesc *out_wPoints = nullptr;              // 判断点数
    XcanIoDesc *out_wCurValue = nullptr;            // 电流值
    XcanIoDesc *out_awDebugRe = nullptr;            // 调试预留 长度2

    XcanIoDesc *out_wCalValue = nullptr;            // 标准值
    XcanIoDesc *out_wLightValue = nullptr;          // 光亮值
    XcanIoDesc *out_wValidPoints = nullptr;         // 有效点数
    XcanIoDesc *out_wResult = nullptr;              // 结果值  (紫外采样位试管)

public:
    /*!
     * \brief A5Client
     */
    A5Client();

    /*!
     * \brief A5Client
     * \param dwRemoteDevID [in] 远程设备ID
     * \param dwComChnID    [in] 通讯方式 0xf0 can  0xf1 net
     */
    A5Client(unsigned short int dwRemoteDevID, unsigned int dwComChnID);

    /*!
     * \brief ~A5Client
     */
    ~A5Client();

    /*!
     * \brief cheak, new rack in.
     * \brief  [out] unsigned char &byRack.
     * \retval true/false
     */
    bool HasNewRack(unsigned char &byRack);

    /*!
     * \brief cheak, new emrgency.
     * \retval true/false
     */
    bool HasEmgTube(void);

    /*!
     * \brief cheak, new tube.
     * \retval true/false
     */
    bool HasNewTube(unsigned char &byTubeID, unsigned char &byRackID);

    /*!
     * \brief cheak, new tube.
     * \retval true/false
     */
    bool HasEmgSt(void);

    /*!
     * \brief SetFindTubeMode
     * \param  [in]  unsigned char tubeMode
     * \retval no
     */
    int SetFindTubeMode(unsigned char tubeMode);

    /*!
     * \brief SetRunMode 设置A3运行模式
     * \param wRunCmd   [in] 运行的命令码，非执行命令码
     * \param wRunParam [in] 运行的参数，
     * \return 执行成功返回0,否则返回对应的错误码
     */
    int SetRunMode(unsigned short wRunCmd, unsigned short wRunParam);

    /*!
     * \brief GetA5RunState 获取A5模块的运行状态
     * \return 返回运行状态结构体，按字节取
     */
    stA5StateTdf GetA5RunState();

    /*!
     * \brief GetA5OptState
     * \return data
     */
    stA5OptTdf GetA5OptState();

    bool GetOptMoRight();
    bool GetOptMoLeft();
    bool GetOptMoMid();
    bool GetOptBridgeLeft();
    bool GetOptBridgeRight();
    bool GetOptRightUp();
    bool GetOptRightDm();
    bool GetOptLeftUp();
    bool GetOptLeftDm();
    bool GetOptExistRightRack();
    bool GetOptEmerg();

    /*!
     * \brief GetA5SetState 获取A5设置状态
     * \return 返回设置状态结构体，
     */
    stA5SetTdf GetA5SetState();

    /*!
     * \brief 获取联机模式
     * \return
     */
    uint8_t GetOnlineModel();

    /*!
     * \brief GetA5UVAdcState A5紫外采样资源
     * \return 返回紫光状态结构体
     */
    stA5UVAdcTdf GetA5UVAdcState();

    /*!
     * \brief GetBarCode
     * \param pRecData    [out] 获取条码缓存结构体
     * \param iRecDataLen [in]  接收条码缓存的的长度
     * \retval 成功返回0,失败返回错误码
     */
    int GetBarCode(char *pRecData, unsigned int iRecDataLen);

    /*!
     * \brief GetBarCode 获取条码
     * \retval 返回条码字符串，如果仪器离线返回"",如果读取失败返回"NO READ"
     */
    string GetBarCode();

    /*!
     * \brief Reset 复位模块
     * \retval 执行成功返回 0,执行失败返回对应的错误码
     */
    int Reset();

    /*!
     * \brief Reset  保存调试参数
     * \retval no
     */
    int SaveDebug();

    /*!
     * \brief cancel debug
     * \retval no
     */
    int CancelDebug();

    /*!
     * \brief get optc
     * \retval no
     */
    int GetOptc();

    /*!
     * \brief debug
     * \retval no
     */
    int Debug();

    /*!
     * \brief next tube
     * \retval no
     */
    int NextTube();

    /*!
     * \brief pass bridge
     * \retval no
     */
    int PassBridge();

    /*!
     * \brief Request pass bridge
     * \retval no
     */
    int RequestPassBridge();

    /*!
     * \brief Request Correlation
     * \retval no
     */
    int RequestCorrelation();

    /*!
     * \brief Request st status
     * \retval no
     */
    int RequestStStatus();

    /*!
     * \brief Tube detect
     * \retval no
     */
    int RackIdDetect();

    /*!
     * \brief Debug 模块调试
     * \param wDebugCmd [in] 调试子命令，非模块命令
     * \param wParam [in] 调试的参数
     * \return 执行成功返回0,失败返回对应的错误码
     */
    int Debug(unsigned short wDebugCmd, unsigned short wFirstParam = 0, unsigned short wSecondParamTwo = 0);

    /*!
     * \brief SetOnlineMode 设置A5联机模式
     * \param bIsOn T 开启 + 联机类型 F 关闭联机
     * \param wParam 开启时传入联机类型
     * \return 执行成功返回0,失败返回对应的错误码
     */
    int SetOnlineMode(bool bIsOn,unsigned short wParam = 0x01);

    /*!
     * \brief 联机命令
     * \param byCmd 命令码
     * \return
     */
    int OnlineRunCommand(unsigned char byCmd);

    /*!
     * \brief 试管架出
     * \return
     */
    int RackOut();

    /*!
     * \brief 获取出桥请求状态
     * \return
     */
    uint8_t GetOutReqStatus();

    /*!
     * \brief 判断联机命令是否结束
     * \return
     */
    bool CheckOnlineCmdExecEnd();

    /*!
     * \brief GetClientID 获取本模块的ID号(重写)
     * \return  返回模块的ID号
     */
    unsigned char GetClientID() override;
private:
    A5ClientD *d = nullptr;
    /*!
     * \brief IoDescInit 初始化资源，将CAN 结构体的资源进行同步区（输入，输出）和非同步区设置(重写）
     */
    void IoDescInit(void) override;

    /*!
     * \brief GetCmdTimeout 获取指定命令码的超时时间
     * \param byCmd [in] 命令码
     * \return 返回对应命令码的超时时间
     */
    unsigned int GetCmdTimeout(unsigned char byCmd) override;

public:
    unsigned short Variable_command[5] = {0};   //临时命令缓存区
};

#endif /* A5Client_H */
