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
#ifndef C8Client_H
#define C8Client_H

/*******************************************************************************
Include file
*******************************************************************************/
#include "xcan/xcaniobase.h"
#include "clients/clients_def.h"
/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/******************************************************************************
*Callback function
******************************************************************************/
/*!
 * \brief 参数变化
 */
typedef void(*ParamChangedCb)(void);

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class C8ClientD;
class XCAN_EXPORTS_API C8Client : public ClientBase
{
public:
    // io
    XcanIoDesc *OutWriteIcSuccess = nullptr;
    XcanIoDesc *OutDoorSta = nullptr;        //1关上，0打开
    XcanIoDesc *OutPwerDown = nullptr;

    /*!
     * \brief 触摸上电标志
     */
    XcanIoDesc *m_pTouchPowerOn = nullptr;

    XcanIoDesc *OutSwapLen = nullptr;
    XcanIoDesc *OutSwap = nullptr;
public:
    C8Client();
    C8Client(short unsigned int wRemoteDevID, unsigned int dwComChnID);
    C8Client(short unsigned int wRemoteDevID, unsigned int dwComChnID, bool blIsMaster);
    ~C8Client();

    void SetOutCmdCntRev(void(*pHandler)(void));
    void RegHandler(void(*pvHandler)(unsigned short));
    void(*pfHandler)(unsigned short);

    int C8Command(unsigned char byCmd);
    int RelodeCntSta(void);
    int Reset();
    int Close_Hot_Powr();
    int Open_Hot_Powr();

    /*!
     * \brief 关机
     * \param byTime 秒
     * \return
     */
    int Close_AllPowr(uint8_t byTime);

    int Read_Ic_Card();
    int Write_Ic_Card();
    int GetVer();
    unsigned char GetIcSta();
    unsigned char GetRes();
    int GetCanSta(void);
    unsigned char GetCleanDoorSta(void);
    unsigned char GetCleanBoxSta(void);
    unsigned char GetSwap(unsigned short *pRecData) override;
    unsigned char GetPwerDown();
    unsigned char GetDoorSta();

    /*!
     * \brief 控制第5路开关
     * \param bOpen
     * \return
     */
    int CtrlPower5(bool bOpen);

    /*!
     * \brief 读取缓冲区数据
     * \param pwBuf 缓冲区
     * \param pwLen 缓冲区数据长度
     * \return 读取成功或者失败
     */
    bool ReadSwap(uint16_t* pwBuf, uint16_t* pwLen);

    /*!
     * \brief 下位机供电控制
     * \param bOpen
     * \return
     */
    int PowerSupplyCtrl(bool bOpen);

    /*!
     * \brief 注册触摸上电通知
     * \param cb
     */
    void RegTouchPowerOnNotice(ParamChangedCb cb);

    /*!
     * \brief 读取掉线故障码
     * \return
     */
    uint16_t ReadOfflineErr();

    /*!
     * \brief 控制报警 参数2
     * \return
     */
    int CtrlBeep_2();
private:
    C8ClientD *d = nullptr;
    void IoDescInit(void)  override;
    unsigned char GetClientID() override;
    unsigned int GetCmdTimeout(unsigned char byCmd) override;

    /*!
     * \brief 触摸上电通知
     */
    ParamChangedCb m_TouchPowerOnNotice = nullptr;
};

#endif // DEVSCANCODE_H
