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
* File Name     : a9_client.h
* File Path     : DevLib
* Project       : u1600plus
* Author        : ...
* Date          : 2022.11.01
* Description   : barcode
*
* modify logs:  Copyright urit Co.,Ltd.All Right Reserved
* 2022.11.01 ...   create
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include <map>
#include "a15_client.h"
#include "platform.h"
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
static unsigned short int mMyIoUUID     = 0xc015;
static unsigned short int mRemoteDevID  = 0x1615;

static std::map<unsigned char, unsigned int> s_mapCmdTime = {
    { CMD_A15_SET_COARSE_FOCUS,                  5}
};

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
* 内部全局变量定义
*******************************************************************************/

/*!
 * \brief constructor
 * \retval no
 */
A15Client::A15Client() : ClientBase ()
{
    unsigned int dwComChnID = XcanNet::GetCommChnIdWithName("net0");
    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);}

/*!
 * \brief constructor
 * \retval no
 */
A15Client::A15Client(short unsigned int wRemoteDevID, unsigned int dwComChnID) : ClientBase ()
{
    mRemoteDevID = wRemoteDevID;

    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);
}

A15Client::A15Client(unsigned short wRemoteDevID, unsigned int dwComChnID, bool blIsMaster) : ClientBase ()
{
    mRemoteDevID = wRemoteDevID;

    Init(mRemoteDevID, dwComChnID, mMyIoUUID, blIsMaster);
}

/*!
 * \brief destructor
 * \retval no
 */
A15Client::~A15Client()
{
}

/*!
 * \brief io init
 * \retval no
 */
void A15Client::IoDescInit()
{
    // 命令输入
    SetIoInsertStart();
    SetIoSynInStart();

    // Command_
    XCAN_IOBASE_NEW_OBJECT(in_byCmd,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_byCmdCnt,       XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_awParm,         XcanIoDesc::IO_U32, 5);
    SetIoSynOutStart();

    // State
    XCAN_IOBASE_NEW_OBJECT(out_byCmd,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byCmdCnt,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCode,     XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCnt,      XcanIoDesc::IO_U8, 1);

    // data
    XCAN_IOBASE_NEW_OBJECT(out_gain,          XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_ip,            XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_mode,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_ms_delay_time, XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_explode_time,  XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_focus_value,   XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dirction,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_trig_mode,     XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_adjust_mode,   XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_white_balance, XcanIoDesc::IO_U32, 3);

    // Version_
    XCAN_IOBASE_NEW_OBJECT(out_dwArchVersion,         XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersion,             XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersionDate,         XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersion,     XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersionDate, XcanIoDesc::IO_U32, 1);

    SetIoDSyncEnd();
    SetIoSynEnd();
    SetIoDEnd();
}

/*!
 * \brief get client id
 * \retval id
 */
unsigned char A15Client::GetClientID()
{
    return 0xaf;
}

/*!
 * \brief get client cmd time out lenght
 * \param  [in]  unsigned char byCmd
 * \retval no
 */
unsigned int A15Client::GetCmdTimeout(unsigned char byCmd)
{
    if(s_mapCmdTime.count(byCmd) != 0) return s_mapCmdTime[byCmd];
    return 10;
}

/*!
 * \brief
 * \retval
 */
void A15Client::SetOutCmdCntRev(void(*pHandler)(void))
{
    out_byCmdCnt->SetRefreshNotice(pHandler);
}

/*!
 * \brief
 * \retval
 */
void A15Client::RegHandler(void(*pvHandler)(unsigned short))
{
    pfHandler = pvHandler;
}

/*!
 * \brief set gain
 * \retval no
 */
int A15Client::SetGain(unsigned int iGain)
{
    in_awParm->Value(iGain);
    return RunCommand(CMD_A15_SET_GAIN);
}

/*!
 * \brief set wthite balance
 * \retval no
 */
int A15Client::SetWB(unsigned int fRed, unsigned int fGreen, unsigned int fBlue)
{
    unsigned int param[3] = {fRed, fGreen, fBlue};
    in_awParm->Value(param, 3);

    return RunCommand(CMD_A15_SET_BLANCE);
}

/*!
 * \brief set Explode time
 * \retval no
 */
int A15Client::SetExplode(unsigned int iExposuretime)
{
    in_awParm->Value(iExposuretime);
    return RunCommand(CMD_A15_SET_EXPLODE_TIM);
}

/*!
 * \brief set ip
 * \retval no
 */
int A15Client::SetIP(unsigned int ip)
{
    in_awParm->Value(ip);
    return RunCommand(CMD_A15_SET_IP);
}

/*!
 * \brief set video mode
 * \retval no
 */
int A15Client::SetVideoMode(unsigned int iMode)
{
    in_awParm->Value(iMode);
    return RunCommand(CMD_A15_SET_MODE);
}

/*!
 * \brief get img
 * \retval no
 */
int A15Client::GetImg(bool bIsAutoFocus)
{
    emA15CmdTdf sendCmd = CMD_A15_NULL;

    if(!bIsAutoFocus) sendCmd = CMD_A15_GET_IMG;
    else sendCmd = CMD_A15_GET_IMG_FOCUE;

    return RunCommand(sendCmd);
}

/*!
 * \brief GetImgWithTrig
 * \param [in] iSampleId 样本号
 * \param [in] wArea 区域数
 * \param [in] wImageNum 拍照单个区域的张数
 * \param [in] wNormalOrQc 0:正常测试 1:质控
 * \param [in] wSpeedMode 0:快速模式（45秒） 1:慢速模式（90秒） 2:对焦模式（每个小图对焦）
 * \return
 */
int A15Client::GetImgWithTrig(unsigned int iSampleId, unsigned short wArea, unsigned short wImageNum, unsigned short wNormalOrQc, unsigned short wSpeedMode)
{
    unsigned int param[5] = {iSampleId, wArea, wImageNum, wNormalOrQc, wSpeedMode};
    in_awParm->Value(param, 5);

    return RunCommand(CMD_A15_GET_IMG_JOINT_START);
}

/*!
 * \brief WetPhotography 湿法拍照（在未使用回调函数的情况下，可使用此函数读取图像信息）
 * \param [in] iSampleId 样本号
 * \param [in] wArea 区域数
 * \param [in] wImageNum 拍照单个区域的张数
 * \param [in] wNormalOrQc 0:正常测试 1:质控
 * \param [in] wSpeedMode 0:快速模式（45秒） 1:慢速模式（90秒）
 * \return
 */
int A15Client::WetPhotography(unsigned int iSampleId, unsigned short wArea, unsigned short wImageNum, unsigned short wNormalOrQc, unsigned short wSpeedMode)
{
    unsigned int param[5] = {iSampleId, wArea, wImageNum, wNormalOrQc, wSpeedMode};
    in_awParm->Value(param, 5);

    return RunCommand(CMD_A15_WET_PHOTOGRAPHY);
}

/*!
 * \brief WetVideoPhoto 湿法录制视频加拍照
 * \param [in] iSampleId 样本号
 * \param [in] wArea 区域数
 * \param [in] wLayerNum 单个区域拍照的层数
 * \param [in] wFrame 视频帧数（视频固定拍三个区域）
 * \param [in] wNormalOrQc 0:正常测试 1:质控
 * \param [in] wSpeedMode 0:快速模式（45秒） 1:慢速模式（90秒）
 * \return
 */
int A15Client::WetVideoPhoto(unsigned int iSampleId, unsigned short wArea, unsigned short wLayerNum, unsigned short wFrame, unsigned short wTestMode, unsigned short wSpeedMode)
{
    unsigned int param[6] = {iSampleId, wArea, wLayerNum, wFrame, wTestMode, wSpeedMode};
    in_awParm->Value(param, 6);

    return RunCommand(CMD_A15_WET_PHOTOGRAPHY);
}

/*!
 * \brief get focus
 * \retval no
 */
int A15Client::SetFocus(unsigned int dwFocusValue)
{
    in_awParm->Value(dwFocusValue);
    return RunCommand(CMD_A15_SET_FOCUS);
}

/*!
 * \brief SetCoarseFocus
 * \retval no
 */
int A15Client::SetCoarseFocus(unsigned int iDirection, unsigned int iFocusValue)
{
    unsigned int param[2] = {iDirection, iFocusValue};
    in_awParm->Value(param, 2);

    return RunCommand(CMD_A15_SET_COARSE_FOCUS);
}

/*!
 * \brief SetAutoFocus
 * \retval no
 */
int A15Client::SetAutoFocus(bool bIsReturnImage)
{
    emA15CmdTdf sendCmd = CMD_A15_NULL;

    if(!bIsReturnImage) sendCmd = CMD_A15_SET_AUTO_FOCUS;
    else sendCmd = CMD_A15_SET_AUTO_FOCUS_WITH_IMAGE;

    return RunCommand(sendCmd);
}

/*!
 * \brief SetAutoCoarseFocus
 * \retval no
 */
int A15Client::SetAutoCoarseFocus(bool bIsReturnImage)
{
    emA15CmdTdf sendCmd = CMD_A15_NULL;
    if(bIsReturnImage) sendCmd = CMD_A15_SET_AUTO_COARSE_FOCUS_WITH_IMAGE;
    else sendCmd = CMD_A15_SET_AUTO_COARSE_FOCUS;

    return RunCommand(sendCmd);
}

/*!
 * \brief SetTriggerMode
 * \retval no
 */
int A15Client::SetTriggerMode(unsigned int nTriggerMode)
{
    in_awParm->Value(nTriggerMode);
    return RunCommand(CMD_A15_SET_FOCUS);
}

/*!
 * \brief SetTriggerMode
 * \retval no
 */
int A15Client::SetColorAdjust(unsigned int iTestMode)
{
    in_awParm->Value(iTestMode);
    return RunCommand(CMD_A15_SET_ADJUST_MODE);
}

int A15Client::SetColorAdjust(unsigned int iTestMode, unsigned int iRed, unsigned int iGreen, unsigned int iBlue)
{
    unsigned int param[4] = {iTestMode, iRed, iGreen, iBlue};
    in_awParm->Value(param, 4);

    return RunCommand(CMD_A15_SET_ADJUST_MODE);
}

/*!
 * \brief SetZMotorCalibration
 * \retval no
 */
int A15Client::SetZMotorCalibration()
{
    return RunCommand(CMD_A15_SET_Z_CALIBRATION);
}

int A15Client::GetConfig()
{
    return RunCommand(CMD_A15_GET_CONFIG);
}

/*!
 * \brief get version
 * \retval no
 */
string A15Client::Version()
{
    return std::string(LIB_VERSION);
}
