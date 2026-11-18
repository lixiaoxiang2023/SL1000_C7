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
#include "a9_client.h"

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
class A9ClientD {
public:
    unsigned char  mErrCnt = 0;
    unsigned char  mCurrentErrCnt = 0;    // 错误计数
    unsigned char  mSampleTubeReadyCnt=0;    // 采样光耦计算
};

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/
static unsigned short int mMyIoUUID     = 0xa0a9;
static unsigned short int mRemoteDevID  = 0x1609;

static std::map<unsigned char, unsigned int> s_mapCmdTime = {
    { CMD_A9_IDLE,                  120},
    { CMD_A9_Rst,                   120},   // 复位
    { CMD_A9_Open,                  120},   // 张开
    { CMD_A9_Read,                  120},   // 读条码
    { CMD_A9_REFRESH,               120},   // 刷新参数
    { CMD_A9_LOAD,                  120},   // 加载参数
    { CMD_A9_SAVE,                  120},   // 保存参数
    { CMD_A9_GETVER,                120}    // 获取版本号命令
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
A9Client::A9Client() : ClientBase ()
{
    unsigned int dwComChnID = XcanNet::GetCommChnIdWithName("can0");
    Init(mRemoteDevID, dwComChnID, mMyIoUUID, true);
}

/*!
 * \brief constructor
 * \retval no
 */
A9Client::A9Client(short unsigned int wRemoteDevID, unsigned int dwComChnID) : ClientBase ()
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
A9Client::~A9Client()
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
void A9Client::IoDescInit()
{
    d = new A9ClientD();
    XCAN_IOBASE_INIT();

    // 命令输入
    SetIoInsertStart();
    SetIoSynInStart();

    // Command_
    XCAN_IOBASE_NEW_OBJECT(in_byCmd,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_byCmdCnt,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(in_awParm,        XcanIoDesc::IO_U16, 5);
    SetIoSynOutStart();

    // State
    XCAN_IOBASE_NEW_OBJECT(out_byCmd,        XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byCmdCnt,         XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCode,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byErrCnt,       XcanIoDesc::IO_U8, 1);

    // data
    XCAN_IOBASE_NEW_OBJECT(out_barcode,         XcanIoDesc::IO_U8, 20);
    XCAN_IOBASE_NEW_OBJECT(out_optc,            XcanIoDesc::IO_U8, 2);
    XCAN_IOBASE_NEW_OBJECT(OutSampleTubeReadyCnt, XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byHandFlg,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byUpdataFinish,     XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(out_byUpdataReady,      XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(outScanEn,           XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(OutNa,               XcanIoDesc::IO_U16, 1);
    XCAN_IOBASE_NEW_OBJECT(OutSwapLen,          XcanIoDesc::IO_U8, 1);
    XCAN_IOBASE_NEW_OBJECT(OutSwap,             XcanIoDesc::IO_U16, 6);

    // Version_
    XCAN_IOBASE_NEW_OBJECT(out_dwArchVersion,            XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersion,                 XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwVersionDate,            XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersion,        XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(out_dwHardwareVersionDate,   XcanIoDesc::IO_U32, 1);
    SetIoDSyncEnd();
    SetIoSynEnd();

    // UpdateDataBuff_
    XCAN_IOBASE_NEW_OBJECT(inout_dwFrameID,     XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(inout_dwFilesize,    XcanIoDesc::IO_U32, 1);
    XCAN_IOBASE_NEW_OBJECT(inout_adwUpdateData,    XcanIoDesc::IO_U8, 512);
    XCAN_IOBASE_NEW_OBJECT(inout_wCrcChk,       XcanIoDesc::IO_U16, 1);
    SetIoDEnd();
}

/*!
 * \brief get client id
 * \retval id
 */
unsigned char A9Client::GetClientID()
{
    return 0xa9;
}

/*!
 * \brief get client cmd time out lenght
 * \param  [in]  unsigned char byCmd
 * \retval no
 */
unsigned int A9Client::GetCmdTimeout(unsigned char byCmd)
{
    if(s_mapCmdTime.count(byCmd) != 0) return s_mapCmdTime[byCmd];
    return 120;
}

/*!
 * \brief
 * \retval
 */
void A9Client::SetOutCmdCntRev(void(*pHandler)(void))
{
    out_byCmdCnt->SetRefreshNotice(pHandler);
}

/*!
 * \brief
 * \retval
 */
void A9Client::RegHandler(void(*pvHandler)(unsigned short))
{
    pfHandler = pvHandler;
}

/*!
 * \brief Scancode Command
 * \param  [in]  unsigned char byCmd
 * \retval no
 */
int A9Client::ScancodeCommand(unsigned char byCmd)
{
    return RunCommand(byCmd);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int A9Client::ScancodeRst()
{
    return RunCommand(CMD_A9_Rst);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int A9Client::ScancodeOpen()
{
    return RunCommand(CMD_A9_Open);
}

/*!
 * \brief Scancode Command
 * \retval no
 */
int A9Client::ScancodeRead()
{
    return RunCommand(CMD_A9_Read);
}

/*!
 * \brief a9 set steps
 * \param  [in]  unsigned short int wData
 * \retval no
 */
int A9Client::ScanEnSta()
{
    return outScanEn->GetValueU8();
}

/*!
 * \brief a9 set steps
 * \param  [in]  unsigned short int wData
 * \retval no
 */
int A9Client::GetBarcode(char *pOut, unsigned int dwLen)
{
    unsigned char byaBarCode[32]={0};

    out_barcode->GetValueU8(byaBarCode);

    unsigned int len = out_barcode->mArrayLen > dwLen ? out_barcode->mArrayLen : dwLen;
    for (unsigned int i = 0; i < len; i++) {
        if(byaBarCode[i] == 0) break;
        pOut[i] = byaBarCode[i] & 0x7f;
    }

    return 0;
}

/*!
 * \brief a9 get barcode
 * \retval string barcode.
 */
string A9Client::GetBarcode(void)
{
    char byaTmp[32] = {0};
    string strBarCode = "noread";

    GetBarcode(byaTmp, 32);
    if(byaTmp[0] == 0) return strBarCode;

    strBarCode = string(byaTmp);

    // 在某种情况，返回READ而非NO READ; 所以这里判断READ可以判断READ和NO READ两种情况
    if (strBarCode.find("READ") != std::string::npos)
    {
        strBarCode = "noread";
    }
    return strBarCode;
}
/*!
 * \brief RelodeCntSta (目前直接写死返回1,不清楚此函数作用)
 * \return 固定返回1，
 */
unsigned short int A9Client::RelodeCntSta(void)
{
    d->mCurrentErrCnt = static_cast<unsigned char>(out_byErrCnt->Value()[0]); // 错误计数
    d->mSampleTubeReadyCnt = static_cast<unsigned char>(OutSampleTubeReadyCnt->Value()[0]);

    return 1;
}

/*!
 * \brief GetDevBarScanTube (目前直接写死返回1,不清楚此函数作用)
 * \return 固定返回1，
 */
unsigned char A9Client::GetDevBarScanTube()
{
    unsigned char wSampleTubeReady = OutSampleTubeReadyCnt->GetValueU8();

    if (wSampleTubeReady == d->mSampleTubeReadyCnt) return 0;
    d->mSampleTubeReadyCnt = wSampleTubeReady;

    return 1;
}

/*!
 * \brief GetCanSta 获取can的状态，（Base函数里有，没必要再封装一次了）
 * \return 返回设备状态，0成功，1设备忙，-1设备断开，-2参数未定义
 */
int A9Client::GetCanSta()
{
    return  GetState();
}

/*!
 * \brief GetSwap 获取交换区的内容，
 * \param pRecData [out] 交换区缓存区 （最好传入缓存区长度，防止溢出)
 * \return IO资源缓冲区的个数
 */
unsigned char A9Client::GetSwap(unsigned short *pData)
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
 * \brief GetVer 获取版本号 (建议统一使用 基类中统一获取版本函数)
 * \return int型的版本号
 */
int A9Client::GetVer()
{
    return RunCommand(CMD_A9_GETVER);
}

/*!
 * \brief SetScanSw 设置条码模式开关
 * \param byModel  0关， 1开
 * \return 成功返回0,失败返回错误码
 */
int A9Client::SetScanSw(unsigned char byModel)
{
    in_awParm->Value(byModel);
    return RunCommand(CMD_A9_SCAN_EN);
}
