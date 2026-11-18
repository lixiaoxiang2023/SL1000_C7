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
#ifndef A9Client_H
#define A9Client_H

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

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
class A9ClientD;
class XCAN_EXPORTS_API A9Client : public ClientBase
{
public:
    // io
    XcanIoDesc *out_barcode = nullptr;
    XcanIoDesc *out_optc = nullptr; 
    XcanIoDesc *OutSampleTubeReadyCnt  = nullptr;
    XcanIoDesc *OutSwapLen  = nullptr;
    XcanIoDesc *OutSwap = nullptr;
    XcanIoDesc *outScanEn = nullptr;
    XcanIoDesc *OutNa = nullptr;

public:
    /*!
     * \brief A9Client 构造函数
     */
    A9Client();

    /*!
     * \brief A9Client
     * \param dwRemoteDevID [in] 远程设备ID
     * \param dwComChnID    [in] 通讯方式 0xf0 can  0xf1 net
     */
    A9Client(short unsigned int wRemoteDevID, unsigned int dwComChnID);

    /*!
     * \brief A9Client
     * \param dwRemoteDevID [in] 远程设备ID
     * \param dwComChnID    [in] 通讯方式 0xf0 can  0xf1 net
     * \param blIsMaster    [in] 设置主从模块， true:主设备模式，false：从设备模式
     */
    A9Client(short unsigned int wRemoteDevID, unsigned int dwComChnID, bool blIsMaster);
    /*!
     * \brief A9Client 析造函数
     */
    ~A9Client();

    /*!
     * \brief SetOutCmdCntRev 设置输出命令回调函数
     * \param  pHandler [out] 当输出命令计数器发生改变时，回调此函数
     */
    void SetOutCmdCntRev(void(*pHandler)(void));

    /*!
     * \brief RegHandler 此函数功能我也不太清楚，没有调用。
     */
    void RegHandler(void(*pvHandler)(unsigned short));

    /*!
     * \brief RegHandler 此函数功能我也不太清楚，没有调用。
     */
    void(*pfHandler)(unsigned short);

    /*!
     * \brief GetDeviceErrCode 获取模块的错误码，双字节带设备ID前缀
     * \return 如果有错误返回对应的错误码，否则返加0表示正常
     */
    unsigned char GetDeviceErrCode(void);

    /*!
     * \brief ScancodeCommand 扫描命令封装，（没必要封装，不利于命令接口统一性)
     * \param byCmd [in] 输入命令码
     * \return 成功返回0,失败返回对应的错误码
     */
    int ScancodeCommand(unsigned char byCmd);

    /*!
     * \brief ScancodeRst 复位命令，(建议使用Reset()即可，即不接口统一，Rst简称不是很通用)
     * \return 成功返回0,失败返回对应的错误码
     */
    int ScancodeRst();

    /*!
     * \brief ScancodeOpen 条码扫描机构打开操作
     * \return 成功返回0,失败返回对应的错误码
     */
    int ScancodeOpen();

    /*!
     * \brief ScancodeRead 条码扫描机构读取条码操作
     * \return 成功返回0,失败返回对应的错误码
     */
    int ScancodeRead();

    /*!
     * \brief ScanEnSta 条码扫描机构全能状态(读取类型接口建议使用Get开头)
     * \return 成功返回0,失败返回对应的错误码
     */
    int ScanEnSta();

    /*!
     * \brief GetBarcode 获取条码信息
     * \param pOut  [out] 读取条码缓存区
     * \param dwLen [in]  读取条码缓存区长度
     * \return 成功返回0,失败返回对应的错误码
     */
    int GetBarcode(char *pOut, unsigned int dwLen);

    /*!
     * \brief GetBarcode 以字符串方式获取条码信息
     * \param pOut  [out] 读取条码缓存区
     * \param dwLen [in]  读取条码缓存区长度
     * \return 成功返回对应的条码，失败返回"noread"
     */
    string GetBarcode(void);

    /*!
     * \brief RelodeCntSta (目前直接写死返回1,不清楚此函数作用)
     * \return 固定返回1，
     */
    unsigned short int RelodeCntSta(void);

    /*!
     * \brief GetDevBarScanTube (目前直接写死返回1,不清楚此函数作用)
     * \return 固定返回1，
     */
    unsigned char GetDevBarScanTube();

    /*!
     * \brief GetCanSta 获取can的状态，（Base函数里有，没必要再封装一次了）
     * \return 返回设备状态，0成功，1设备忙，-1设备断开，-2参数未定义
     */
    int GetCanSta(void);

    /*!
     * \brief GetSwap 获取交换区的内容，
     * \param pRecData [out] 交换区缓存区 （最好传入缓存区长度，防止溢出)
     * \return IO资源缓冲区的个数
     */
    unsigned char GetSwap(unsigned short *pRecData);

    /*!
     * \brief GetVer 获取版本号 (建议统一使用 基类中统一获取版本函数)
     * \return int型的版本号
     */
    int GetVer();

    /*!
     * \brief SetScanSw 设置条码模式开关
     * \param byModel  0关， 1开
     * \return 成功返回0,失败返回错误码
     */
    int SetScanSw(unsigned char byModel);

    /*!
     * \brief GetClientID 获取本模块的ID号(重写)
     * \return  返回模块的ID号
     */
    unsigned char GetClientID() override;
private:
    A9ClientD *d = nullptr;
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
};

#endif // DEVSCANCODE_H
