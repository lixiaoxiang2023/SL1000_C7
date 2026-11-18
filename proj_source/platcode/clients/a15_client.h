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
* Author        : ...
* Date          : 2022.11.01
* Description   : barcode
*
* modify logs:  Copyright urit Co.,Ltd.All Right Reserved
* 2022.11.01 ...   create
*******************************************************************************/
#ifndef A15Client_H
#define A15Client_H

/*******************************************************************************
Include file
*******************************************************************************/
#include "xcan/xcaniobase.h"
#include "clients/clients_ax_cmds.h"

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class A15ClientD;
class XCAN_EXPORTS_API A15Client : public ClientBase
{
public:
    // io
    XcanIoDesc *out_gain = nullptr;
    XcanIoDesc *out_ip = nullptr;
    XcanIoDesc *out_mode = nullptr;
    XcanIoDesc *out_ms_delay_time = nullptr;
    XcanIoDesc *out_explode_time = nullptr;
    XcanIoDesc *out_focus_value = nullptr;
    XcanIoDesc *out_dirction = nullptr;
    XcanIoDesc *out_trig_mode = nullptr;
    XcanIoDesc *out_adjust_mode = nullptr;
    XcanIoDesc *out_white_balance = nullptr;

public:
    A15Client();
    A15Client(short unsigned int wRemoteDevID, unsigned int dwComChnID);
    A15Client(short unsigned int wRemoteDevID, unsigned int dwComChnID, bool blIsMaster);
    ~A15Client();
    /*!
     * \brief SetOutCmdCntRev 设置输出命令回调函数
     * \param  pHandler [out] 当输出命令计数器发生改变时，回调此函数
     */
    void SetOutCmdCntRev(void(*pHandler)(void));

    /*!
     * \brief RegHandler
     */
    void RegHandler(void(*pvHandler)(unsigned short));

    /*!
     * \brief RegHandler
     */
    void(*pfHandler)(unsigned short);

    /*!
     * \brief set gain
     * \retval no
     */
    int SetGain(unsigned int iGain);

    /*!
     * \brief set wthite balance
     * \retval no
     */
    int SetWB(unsigned int fRed, unsigned int fGreen, unsigned int fBlue);

    /*!
     * \brief set Explode time
     * \retval no
     */
    int SetExplode(unsigned int iExposuretime);

    /*!
     * \brief set ip
     * \retval no
     */
    int SetIP(unsigned int ip);

    /*!
     * \brief set video mode
     * \retval no
     */
    int SetVideoMode(unsigned int iMode);

    /*!
     * \brief get img
     * \retval no
     */
    int GetImg(bool bIsAutoFocus);

    /*!
     * \brief GetImgWithTrig
     * \param [in] iSampleId 样本号
     * \param [in] wArea 区域数
     * \param [in] wImageNum 拍照单个区域的张数
     * \param [in] wNormalOrQc 0:正常测试 1:质控
     * \param [in] wSpeedMode 0:快速模式（45秒） 1:慢速模式（90秒） 2:对焦模式（每个小图对焦）
     * \return
     */
    int GetImgWithTrig(unsigned int iSampleId, unsigned short wArea, unsigned short wImageNum, unsigned short wNormalOrQc, unsigned short wSpeedMode);

    /*!
     * \brief WetPhotography 湿法拍照（在未使用回调函数的情况下，可使用此函数读取图像信息）
     * \param [in] iSampleId 样本号
     * \param [in] wArea 区域数
     * \param [in] wImageNum 拍照单个区域的张数
     * \param [in] wNormalOrQc 0:正常测试 1:质控
     * \param [in] wSpeedMode 0:快速模式（45秒） 1:慢速模式（90秒）
     * \return
     */
     int WetPhotography(unsigned int iSampleId, unsigned short wArea, unsigned short wImageNum, unsigned short wTestMode, unsigned short wSpeedMode);

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
      int WetVideoPhoto(unsigned int iSampleId, unsigned short wArea, unsigned short wLayerNum, unsigned short wFrame, unsigned short wTestMode, unsigned short wSpeedMode);

    /*!
     * \brief get focus
     * \retval no
     */
    int SetFocus(unsigned int dwFocusValue);

    /*!
     * \brief SetCoarseFocus
     * \retval no
     */
    int SetCoarseFocus(unsigned int iDirection, unsigned int iFocusValue);

    /*!
     * \brief SetAutoFocus
     * \retval no
     */
    int SetAutoFocus(bool bIsReturnImage);

    /*!
     * \brief SetAutoCoarseFocus
     * \retval no
     */
    int SetAutoCoarseFocus(bool bIsReturnImage);

    /*!
     * \brief SetTriggerMode
     * \retval no
     */
    int SetTriggerMode(unsigned int nTriggerMode);

    /*!
     * \brief SetTriggerMode
     * \retval no
     */
    int SetColorAdjust(unsigned int iTestMode);

    /*!
     * \brief SetTriggerMode
     * \retval no
     */
    int SetColorAdjust(unsigned int iTestMode, unsigned int iRed, unsigned int iGreen, unsigned int iBlue);

    /*!
     * \brief SetZMotorCalibration
     * \retval no
     */
    int SetZMotorCalibration();

    /*!
     * \brief get config
     * \retval no
     */
    int GetConfig();

    /*!
     * \brief get version
     * \retval no
     */
    static string Version();


private:
    void IoDescInit(void)  override;
    unsigned char GetClientID() override;
    unsigned int GetCmdTimeout(unsigned char byCmd) override;
};

#endif // DEVSCANCODE_H
