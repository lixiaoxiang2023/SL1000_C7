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
* File Name     : C8_client.h
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
#include "C8_client.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
class C8ClientD {
public:
    unsigned char mCurrentErrCnt = 0;    // 错误计数

};

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
static unsigned short int mMyIoUUID     = 0xC0C8;
static unsigned short int mRemoteDevID  = 0x1608;

static std::map<unsigned char, unsigned int> s_mapCmdTime = {
    { CMD_C8_IDLE,                       5},
    { CMD_C8_RESET,                     15}, //复位命令
    { CMD_C8_OPENALLPOWER,               5},   //打开所有通道
    { CMD_C8_CLOSEALLPOWER,              5},   //关闭所有通道
    { CMD_C8_OPENPOWER1,                 5},  //
    { CMD_C8_CLOSEPOWER1,                5},   //
    { CMD_C8_OPENPOWER2,                 5},  //
    { CMD_C8_CLOSEPOWER2,                5},  //
    { CMD_C8_OPENPOWER3,                 5},  //
    { CMD_C8_CLOSEPOWER3,                5},  //
    { CMD_C8_OPENPOWER4,                 5},   //
    { CMD_C8_CLOSEPOWER4,                5},   //
    { CMD_C8_OPENPOWER5,                 5},   //
    { CMD_C8_CLOSEPOWER5,                5},   //
    { CMD_C8_OPENPOWER6,                 5},   //
    { CMD_C8_CLOSEPOWER6,                5}, //
    { CMD_C8_OPENPOWER7,                 5},   //
    { CMD_C8_CLOSEPOWER7,                5},  //
    { CMD_C8_OPENPOWER8,                 5},  //
    { CMD_C8_CLOSEPOWER8,                5},   //
    { CMD_C8_READICCARD,                 5},  //
    { CMD_C8_WRITEICCARD,                5},   //
    { CMD_C8_LED_R,                      5},   //红色闪烁
    { CMD_C8_LED_G,                      5},
    { CMD_C8_LED_B,                      5},
    { CMD_C8_LED_ON_R,                   5},  //红色常亮
    { CMD_C8_LED_ON_G,                   5},
    { CMD_C8_LED_ON_B,                   5},
    { CMD_C8_UPDATA_START,               5}, //开始升级
    { CMD_C8_CANCLPWOEROFF,              5}, //取消关机
    { CMD_C8_GETVER,                     5},
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
C8Client::C8Client() : ClientBase ()
{
    unsigned int dwComChnID = XcanNet::GetCommChnIdWithName("can0");
    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);
}

/*!
 * \brief constructor
 * \retval no
 */
C8Client::C8Client(short unsigned int wRemoteDevID, unsigned int dwComChnID) : ClientBase ()
{
    mRemoteDevID = wRemoteDevID;
    dwComChnID = XcanNet::GetCommChnIdWithName("can0");

    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);
}

/*!
 * \brief destructor
 * \retval no
 */
C8Client::~C8Client()
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
void C8Client::IoDescInit()
{
    d = new C8ClientD();
    XCAN_IOBASE_INIT();
    // 命令输入
    SetIoInsertStart();
    SetIoSynInStart();

    // Command_
    XCAN_IOBASE_NEW_OBJECT(in_byCmd,                  XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_byCmdCnt,               XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_awParm,                 XcanIoDesc::IO_U16, 5);
    SetIoSynOutStart();

    // State
    XCAN_IOBASE_NEW_OBJECT(out_byCmd,                 XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byCmdCnt,              XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCode,             XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCnt,              XcanIoDesc::IO_U8, 1);
    // data
    XCAN_IOBASE_NEW_OBJECT(OutWriteIcSuccess,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(OutDoorSta,                XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(OutPwerDown,               XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byHandFlg,             XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byUpdataFinish,        XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byUpdataReady,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(m_pTouchPowerOn,           XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(OutSwapLen,                XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(OutSwap,                   XcanIoDesc::IO_U16, 32);
    // Version_
    XCAN_IOBASE_NEW_OBJECT(out_dwArchVersion,         XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersion,             XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersionDate,         XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersion,     XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersionDate, XcanIoDesc::IO_U32, 1);
    SetIoDSyncEnd();
    SetIoSynEnd();

    // UpdateDataBuff_
    XCAN_IOBASE_NEW_OBJECT(inout_dwFrameID,           XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(inout_dwFilesize,          XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(inout_adwUpdateData,       XcanIoDesc::IO_U8, 512);
    XCAN_IOBASE_NEW_OBJECT(inout_wCrcChk,             XcanIoDesc::IO_U16, 1);
    SetIoDEnd();
}

/*!
 * \brief get client id
 * \retval id
 */
unsigned char C8Client::GetClientID()
{
    return 0xC8;
}

/*!
 * \brief get client cmd time out lenght
 * \param  [in]  unsigned char byCmd
 * \retval no
 */
unsigned int C8Client::GetCmdTimeout(unsigned char byCmd)
{
    if(s_mapCmdTime.count(byCmd) != 0) return s_mapCmdTime[byCmd];
    return 0;
}

/*!
 * \brief
 * \retval
 */
void C8Client::SetOutCmdCntRev(void(*pHandler)(void))
{
    out_byCmdCnt->SetRefreshNotice(pHandler);
}

/*!
 * \brief
 * \retval
 */
void C8Client::RegHandler(void(*pvHandler)(unsigned short))
{
    pfHandler = pvHandler;
}

/*!
 * \brief Scancode Command
 * \param  [in]  unsigned char byCmd
 * \retval no
 */
int C8Client::C8Command(unsigned char byCmd)
{
    return RunCommand(byCmd);
}

/*!
 * \brief relode errcnt
 * \retval no
 */
int C8Client::RelodeCntSta()
{
    d->mCurrentErrCnt = out_byErrCnt->GetValueU8(); // 错误计数
    return 1;
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int C8Client::Reset()
{
    return RunCommand(CMD_C8_RESET);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int C8Client::Close_Hot_Powr()
{
    return RunCommand(CMD_C8_CLOSEPOWER1);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int C8Client::Open_Hot_Powr()
{
    return RunCommand(CMD_C8_OPENPOWER1);
}

/*!
 * \brief 关机
 * \param byTime 秒
 * \return
 */
int C8Client::Close_AllPowr(uint8_t byTime)
{
    uint16_t awData[1] = {byTime};
    return RunCommand(CMD_C8_CLOSEALLPOWER, awData, 1);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int C8Client::Read_Ic_Card()
{
    return RunCommand(CMD_C8_READICCARD);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int C8Client::Write_Ic_Card()
{
    return RunCommand(CMD_C8_WRITEICCARD);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
unsigned char C8Client::GetIcSta()
{
    return OutWriteIcSuccess->GetValueU8();
}

/*!
 * \brief get rhtrmp
 * \retval no
 */
int C8Client::GetVer()
{
    return RunCommand(CMD_C8_GETVER);
}

/*!
 * \brief get rhtrmp
 * \retval no
 */
unsigned char C8Client::GetSwap(unsigned short *pData)
{
    unsigned char swapCount = OutSwapLen->GetValueU8();
    unsigned int *dwGet = OutSwap->Value();

    for (unsigned char i = 0; i < swapCount; i++)
    {
        pData[i] = static_cast<unsigned short>(dwGet[i]);
    }

    return swapCount;
}

/*!
 * \brief get rhtrmp
 * \retval no
 */
int C8Client::GetCanSta(void)
{
    return  GetState();
}

/*!
 * \brief get rhtrmp
 * \retval no
 */
unsigned char C8Client::GetPwerDown()
{
    return  OutPwerDown->GetValueU8();
}

/*!
 * \brief get rhtrmp
 * \retval no
 */
unsigned char C8Client::GetDoorSta()
{
    return  OutDoorSta->GetValueU8();
}

/*!
 * \brief 控制第5路开关
 * \param bOpen
 * \return
 */
int C8Client::CtrlPower5(bool bOpen)
{
    if (bOpen)
    {
        return RunCommand(CMD_C8_OPENPOWER5);
    }
    else
    {
        return RunCommand(CMD_C8_CLOSEPOWER5);
    }
}

/*!
 * \brief 读取缓冲区数据
 * \param pwBuf 缓冲区
 * \param pwLen 缓冲区数据长度
 * \return 读取成功或者失败
 */
bool C8Client::ReadSwap(uint16_t *pwBuf, uint16_t *pwLen)
{
    if (pwBuf == nullptr || pwLen == nullptr)
    {
        return false;
    }

    *pwLen = OutSwapLen->GetValueU8();
    OutSwap->GetValueU16(pwBuf, *pwLen);
    return true;
}

/*!
 * \brief 下位机供电控制
 * \param bOpen
 * \return
 */
int C8Client::PowerSupplyCtrl(bool bOpen)
{
    uint16_t awParam[1] = {bOpen};
    return RunCommand(CMD_C8_POWER_CONTROL, awParam, 1);
}

/*!
 * \brief 注册触摸上电通知
 * \param cb
 */
void C8Client::RegTouchPowerOnNotice(ParamChangedCb cb)
{
    if (m_pTouchPowerOn != nullptr)
    {
        m_pTouchPowerOn->SetRefreshNotice(cb);
    }
}

/*!
 * \brief 读取掉线故障码
 * \return
 */
uint16_t C8Client::ReadOfflineErr()
{
    uint16_t wErr = GetErrorCode();
    if (wErr == ERROR_CLIENT_OFFLINE)
    {
        return static_cast<uint16_t>((GetClientID() << 8) + ERROR_CLIENT_OFFLINE);
    }
    else
    {
        return GetNoErrorCode();
    }
}

/*!
 * \brief 控制报警 参数2
 * \return
 */
int C8Client::CtrlBeep_2()
{
    uint16_t awParam[1] = {2};
    return RunCommand(CMD_C8_CRTLBEEP, awParam, 1);
}

