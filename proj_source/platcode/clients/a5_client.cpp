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

/*******************************************************************************
Include file
*******************************************************************************/
#include <map>
#include "a5_client.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
class A5ClientD {
public:
    unsigned char mHasNewRack = 0;
    unsigned char mHasNewTube = 0;
};

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
static unsigned short int mMyIoUUID     = 0xa0a5;
static unsigned short int mRemoteDevID  = 0x1605;

static std::map<unsigned char, unsigned int> s_mapCmdTime = {
    { CMD_A5_RESET,                  120},   // 复位命令
    { CMD_A5_NEXT_TRUB,              120},   // 查找试管命令
    { CMD_A5_DEBUG,                  120},   // 调试命令
    { CMD_A5_SAVE_DEBUG,             120},   // 保存命令
    { CMD_A5_CANCEL_DEBUG,           120},   // 取消调试命令
    { CMD_A5_RACK_OUT_A5,            120}    // 进桥命令
};

/*!
 * \brief constructor
 * \retval no
 */
A5Client::A5Client() :ClientBase ()
{
    unsigned int dwComChnID = XcanNet::GetCommChnIdWithName("can0");
    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);
}

/*!
 * \brief constructor
 * \retval no
 */
A5Client::A5Client(unsigned short int wRemoteDevID, unsigned int dwComChnID) :ClientBase()
{
    mRemoteDevID = wRemoteDevID;
    dwComChnID = XcanNet::GetCommChnIdWithName("can0");

    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);
    in_byCmdCnt->Value(out_byCmdCnt->GetValueU8());
}

/*!
 * \brief destructor
 * \retval no
 */
A5Client::~A5Client()
{
    if(d != nullptr)
    {
        delete d;
        d = nullptr;
    }
}

/*!
 * \brief io init
 * \retval no
 */
void A5Client::IoDescInit()
{
    d = new A5ClientD();
    XCAN_IOBASE_INIT();

    // 命令输入
    SetIoInsertStart();
    SetIoSynInStart();

    // Command_
    XCAN_IOBASE_NEW_OBJECT(in_byCmd,                XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_byCmdCnt,             XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_awParm,               XcanIoDesc::IO_U16, 5);

    //Command_2
    XCAN_IOBASE_NEW_OBJECT(in_byCmd2,               XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_byCmdCnt2,            XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_wParam2,              XcanIoDesc::IO_U16, 5);

    SetIoSynOutStart();

    // State
    XCAN_IOBASE_NEW_OBJECT(out_byCmd,               XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byCmdCnt,            XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCode,           XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCnt,            XcanIoDesc::IO_U8, 1);

    // cmd_State_2
    XCAN_IOBASE_NEW_OBJECT(out_byCmd2,              XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byCmdCnt2,           XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCode2,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrorCnt2,         XcanIoDesc::IO_U8, 1);

    // opt
    XCAN_IOBASE_NEW_OBJECT(out_wOPT,                XcanIoDesc::IO_U16, 1);

    // _SampleData_
    XCAN_IOBASE_NEW_OBJECT(out_bySampleCnt,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_bySampleFlag,        XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_bySampleRackid,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_bySampleTubeid,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_abyBarCode,          XcanIoDesc::IO_U8, 20);
    XCAN_IOBASE_NEW_OBJECT(out_byEmerCnt,           XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byReadyTube,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byNewRackCnt,        XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byNewRackid,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byBarCodeCnt,        XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byBarCodeTubeFlag,   XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byBarCodeTubeid,     XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_abyRe0,              XcanIoDesc::IO_U8, 5);

    // state_a5
    XCAN_IOBASE_NEW_OBJECT(out_dwState,             XcanIoDesc::IO_U32, 1);

    // RackData
    XCAN_IOBASE_NEW_OBJECT(out_byDataCntR,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byOutA5ReqR,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byRackNameR,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byReR,               XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_abyRackidR,          XcanIoDesc::IO_U8, 8);

    XCAN_IOBASE_NEW_OBJECT(out_byDataCntL,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byOutA5ReqL,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byRackNameL,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byReL,               XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_abyRackidL,          XcanIoDesc::IO_U8, 8);

    // _set_
    XCAN_IOBASE_NEW_OBJECT(out_byOnlineModel,       XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byCycleOnOff,        XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byTubeRackNoOnOff,   XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byBarcodeOnOff,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byA5Type,            XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byA6Online,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_abySetRe,            XcanIoDesc::IO_U8, 1);

    // Postion
    XCAN_IOBASE_NEW_OBJECT(out_wRightStPos,         XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wRightIntvl,         XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wLeftStPos,          XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wLeftIntvl,          XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wOnlinePos,          XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wCycSteps,           XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wTube1Pos,           XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wTube10Pos,          XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wRack2LeftPos,       XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wRetestCompenPos,    XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wThreshold,          XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wPoints,             XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wCurValue,           XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_awDebugRe,           XcanIoDesc::IO_U16, 2);

    //A5UVAdcTdf
    XCAN_IOBASE_NEW_OBJECT(out_wCalValue,           XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wLightValue,         XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wValidPoints,        XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(out_wResult,             XcanIoDesc::IO_U16, 1);

    // Version_
    XCAN_IOBASE_NEW_OBJECT(out_dwArchVersion,       XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersion,           XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersionDate,       XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersion,   XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersionDate,XcanIoDesc::IO_U32, 1);

    // upgrade
    XCAN_IOBASE_NEW_OBJECT(out_byHandFlg,           XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byUpdataFinish,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byUpdataReady,       XcanIoDesc::IO_U8, 1);

    // error bits
    XCAN_IOBASE_NEW_OBJECT(out_dwErrBits,           XcanIoDesc::IO_U32, 1);

    XCAN_IOBASE_NEW_OBJECT(out_strNA,               XcanIoDesc::IO_U8, 21);

    SetIoDSyncEnd();
    SetIoSynEnd();

    // UpdateDataBuff_
    XCAN_IOBASE_NEW_OBJECT(inout_dwFrameID,         XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(inout_dwFilesize,        XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(inout_adwUpdateData,     XcanIoDesc::IO_U8, 512);
    XCAN_IOBASE_NEW_OBJECT(inout_wCrcChk,           XcanIoDesc::IO_U16, 1);
    SetIoDEnd();
}

/*!
 * \brief get client id
 * \retval true/false
 */
unsigned char A5Client::GetClientID()
{
    return 0xa5;
}

/*!
 * \brief get client cmd time out lenght
 * \param  [in]  unsigned char byCmd
 * \retval no
 */
unsigned int A5Client::GetCmdTimeout(unsigned char byCmd)
{
    if(s_mapCmdTime.count(byCmd) != 0) return s_mapCmdTime[byCmd];
    return 120;
}

/*!
 * \brief cheak, new rack in.
 * \retval true/false
 */
bool A5Client::HasNewRack(unsigned char &byRack)
{
    unsigned char byRackNew = out_byNewRackCnt->GetValueU8();

    if(d->mHasNewRack != byRackNew) {
        d->mHasNewRack = byRackNew;
        byRack = out_byNewRackid->GetValueU8();
        if(byRackNew != 0) return true;
    }

    return false;
}

/*!
 * \brief cheak, new emrgency.
 * \retval true/false
 */
bool A5Client::HasEmgTube(void)
{
    unsigned char ucSampFlg = out_byEmerCnt->GetValueU8();
    static unsigned char s_ucEmerCntLast = out_byEmerCnt->GetValueU8();

    if(ucSampFlg != s_ucEmerCntLast)
    {
        s_ucEmerCntLast = ucSampFlg;
        return true;
    }

    return false;
}

/*!
 * \brief cheak, new emrgency.
 * \retval true/false
 */
bool A5Client::HasEmgSt(void)
{
    unsigned char ucSampFlg = out_bySampleFlag->GetValueU8();
    unsigned char ucRackID = out_bySampleRackid->GetValueU8();
    unsigned char ucTubeID = out_bySampleTubeid->GetValueU8();

    if(ucSampFlg == 1 && ucRackID == 0 && ucTubeID == 0)
    {
        return true;
    }

    return false;
}

/*!
 * \brief cheak, new tube.
 * \retval true/false
 */
bool A5Client::HasNewTube(unsigned char &byTubeID, unsigned char &byRackID)
{
    unsigned char ucSampleFlg = out_bySampleFlag->GetValueU8();
    byTubeID = out_bySampleTubeid->GetValueU8();
    byRackID = out_bySampleRackid->GetValueU8();

    if(ucSampleFlg == 1) {
        return true;
    }

    return false;
}

/*!
 * \brief SetFindTubeMode
 * \param  [in]  unsigned char tubeMode
 * \retval no
 */
int A5Client::SetFindTubeMode(unsigned char tubeMode)
{
    unsigned char param[2];

    param[0] = 0x03;
    param[1] = tubeMode;

    in_awParm->Value(param, 2);
    return RunCommand(CMD_A5_SET_RUN_MODE);
}

/*!
 * \brief SetRunMode 设置A3运行模式
 * \param wRunCmd   [in] 运行的命令码，非执行命令码
 * \param wRunParam [in] 运行的参数，
 * \return 执行成功返回0,否则返回对应的错误码
 */
int A5Client::SetRunMode(unsigned short wRunCmd, unsigned short wRunParam)
{
    unsigned short awParam[2] = {wRunCmd, wRunParam};
    return RunCommand(CMD_A5_SET_RUN_MODE, awParam, 2);
}

/*!
 * \brief get run status
 * \retval struct run status
 */
stA5StateTdf A5Client::GetA5RunState()
{
    stA5StateTdf strRunSt;

    strRunSt.unStste.dwState  = out_dwState->GetValueU32();
    return strRunSt;
}

/*!
 * \brief get opts status
 * \retval struct opts status
 */
stA5OptTdf A5Client::GetA5OptState()
{
    stA5OptTdf strOPtSt;

    strOPtSt.unOpt.wOPT = out_wOPT->GetValueU16();
    return strOPtSt;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptMoRight()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x01) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptMoLeft()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x02) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptMoMid()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x04) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptBridgeLeft()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x08) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptBridgeRight()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x0100) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptRightUp()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x10) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptRightDm()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x20) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptLeftUp()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x40) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptLeftDm()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x80) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptExistRightRack()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x0200) == 0)? false:true;
}

/*!
 * \brief get op status
 * \retval struct true/false
 */
bool A5Client::GetOptEmerg()
{
    unsigned short int wOpt = out_wOPT->GetValueU16();
    return ((wOpt & 0x0400) == 0)? false:true;
}

/*!
 * \brief get set status
 * \retval struct sets status
 */
stA5SetTdf A5Client::GetA5SetState()
{
    stA5SetTdf strSetSt;

    strSetSt.byOnlineModel = out_byOnlineModel->GetValueU8();
    strSetSt.byCycle_OnOff = out_byCycleOnOff->GetValueU8();
    strSetSt.byTubeRackNo_OnOff = out_byTubeRackNoOnOff->GetValueU8();
    strSetSt.byBarCode_OnOff = out_byBarcodeOnOff->GetValueU8();
    strSetSt.byA5Type = out_byA5Type->GetValueU8();

    return strSetSt;
}

/*!
 * \brief 获取联机模式
 * \return
 */
uint8_t A5Client::GetOnlineModel()
{
    return out_byOnlineModel->GetValueU8();
}

/*!
 * \brief GetA5UVAdcState A5紫外采样资源
 * \return
 */
stA5UVAdcTdf A5Client::GetA5UVAdcState()
{
    stA5UVAdcTdf stGetUVAdc;
    stGetUVAdc.CalValue = out_wCalValue->GetValueU16();
    stGetUVAdc.LightValue = out_wLightValue->GetValueU16();
    stGetUVAdc.ValidPoints = out_wValidPoints->GetValueU16();
    stGetUVAdc.TubeResult = out_wResult->GetValueU16();

    return stGetUVAdc;
}

/*!
 * \brief GetBarCode
 * \param  [out]  char *pRecData
 * \param  [in]   unsigned int iRecDataLen
 * \retval no
 */
int A5Client::GetBarCode(char *pRecData, unsigned int iRecDataLen)
{
    unsigned char cBarCode[32] = {0};

    memset(cBarCode, 0, 32);
    out_abyBarCode->GetValueU8(cBarCode);

    unsigned int len = out_abyBarCode->mArrayLen > iRecDataLen ? out_abyBarCode->mArrayLen : iRecDataLen;
    for (unsigned int i = 0; i < len; i++) {
        if(cBarCode[i] == 0) break;
        pRecData[i] = cBarCode[i] & 0x7f;
    }

    return 0;
}

/*!
 * \brief GetBarCode
 * \retval string barcode
 */
string A5Client::GetBarCode()
{
    string strBarCode = "noread";
    char cBarCode[32] = {0};

    GetBarCode(cBarCode, 32);
    if(cBarCode[0] == 0) return strBarCode;

    strBarCode = string(cBarCode);
    if(strBarCode.compare("NO READ") == 0)
    {
        strBarCode = "noread";
    }

    return strBarCode;
}

/*!
 * \brief reset
 * \retval no
 */
int A5Client::Reset()
{
    return RunCommand(CMD_A5_RESET);
}

/*!
 * \brief save debug
 * \retval no
 */
int A5Client::SaveDebug()
{
    return RunCommand(CMD_A5_SAVE_DEBUG);
}

/*!
 * \brief cancel debug
 * \retval no
 */
int A5Client::CancelDebug()
{
    return RunCommand(CMD_A5_CANCEL_DEBUG);
}

/*!
 * \brief get optc
 * \retval no
 */
int A5Client::GetOptc()
{
    return RunCommand(CMD_A5_DEBUG_OPTO);
}

/*!
 * \brief debug
 * \retval no
 */
int A5Client::Debug()
{
    return RunCommand(CMD_A5_DEBUG);
}

/*!
 * \brief next tube
 * \retval no
 */
int A5Client::NextTube()
{
    return RunCommand(CMD_A5_NEXT_TRUB);
}

/*!
 * \brief pass bridge
 * \retval no
 */
int A5Client::PassBridge()
{
    return RunCommand(CMD_A5_RACK_OUT_A5);
}

/*!
 * \brief Request pass bridge
 * \retval no
 */
int A5Client::RequestPassBridge()
{
    return RunCommand(CMD_A5_REQUEST_PASS_BRIDGE);
}

/*!
 * \brief Tube detect
 * \retval no
 */
int A5Client::RackIdDetect()
{
    return RunCommand(CMD_A5_RACK_ID_DETECT);
}

/*!
 * \brief Debug
 * \param wDebugCmd debugcmd
 * \param wParam parameter
 * \return
 */
int A5Client::Debug(unsigned short wDebugCmd, unsigned short wFirstParam,unsigned short wSecondParamTwo)
{
    unsigned short arrParam[3] = {wDebugCmd, wFirstParam, wSecondParamTwo};
    return RunCommand(CMD_A5_DEBUG, arrParam, 3);
}

/*!
 * \brief SetOnlineMode 设置A5联机模式
 * \param bIsOn T 开启 + 联机类型 F 关闭联机
 * \param wParam 开启时传入联机类型
 * \return 执行成功返回0,失败返回对应的错误码
 */
int A5Client::SetOnlineMode(bool bIsOn, unsigned short wParam)
{
    int iResult = 0;
    unsigned short arrParam[2] = {0};
    if(bIsOn)
    {
        arrParam[0] = CMD_A5_ONLINE_MODEL;
        arrParam[1] = wParam;
        iResult = RunCommand(CMD_A5_SET_RUN_MODE, arrParam, 2);
    }
    else
    {
        arrParam[0] = CMD_A5_ONLINE_MODEL;
        arrParam[1] = 0;
        iResult = RunCommand(CMD_A5_SET_RUN_MODE, arrParam, 2);
    }
    return iResult;
}

/*!
 * \brief 联机命令
 * \param byCmd 命令码
 * \return
 */
int A5Client::OnlineRunCommand(unsigned char byCmd)
{
    if (LinkToRemoteDevice() == false) return -3;
    if (RemoteDeviceExist() == false) return -1;

    unsigned char byInCmdCnt = in_byCmdCnt2->GetValueU8();
    byInCmdCnt += 1;

    ForceResetErrStatus();
    in_byCmd2->Value(byCmd);
    in_byCmdCnt2->Value(byInCmdCnt);

    return 0;
}

/*!
 * \brief 试管架出
 * \return
 */
int A5Client::RackOut()
{
    return OnlineRunCommand(CMD_A5_RACK_OUT_A5);
}

/*!
 * \brief 获取出桥请求状态
 * \return
 */
uint8_t A5Client::GetOutReqStatus()
{
    return out_byOutA5ReqR->GetValueU8();
}

/*!
 * \brief 判断联机命令是否结束
 * \return
 */
bool A5Client::CheckOnlineCmdExecEnd()
{
    return in_byCmdCnt2->GetValueU8() == out_byCmdCnt2->GetValueU8();
}
