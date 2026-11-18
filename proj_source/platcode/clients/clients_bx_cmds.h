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
* File Name     : client_bx_cmds.h
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
#ifndef __CLIENTS_BX_CMDS_H_
#define __CLIENTS_BX_CMDS_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition 常量以及宏定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
/**************************2000   B1 PMT*************************************/
typedef enum _emB1CmdTdf
{
    CMD_B1_IDLE                     = 0x00,
    CMD_B1_SET_BLUE_LEA             = 0x20, // 激光功率
    CMD_B1_PMT_SET                  = 0x21, // 高压设置
    CMD_B1_GAIN_SET                 = 0x22, // 增益设置
    CMD_B1_BLUE_ON                  = 0x24, // 开启激光器
    CMD_B1_BLUE_OFF                 = 0x26, // 关闭激光器
    CMD_B1_PMT2_SET                 = 0x27, // 高压设置,第二套参数
    CMD_B1_GAIN2_SET                = 0x28, // 增益设置,第二套参数
    CMD_B1_PARA_VER_SET             = 0x29, // 参数版本，0，使用第一套参数，1，使用第二套参数

    CMD_B1_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_B1_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_B1_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_B1_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emB1CmdTdf;

typedef enum emB1ErrTdf {
    ERR_B1_NO      					= 0x00,
    ERR_B1_LSR_B_OFFLINE            = 0x01, // 激光器掉线错误
    ERR_B1_LSR_B_OVER_CURR          = 0x02, // 激光器过流 超过二极管的最大电流
    ERR_B1_LSR_B_OVER_POWER         = 0x03, // 激光器过功率 实际功率超过设定的功率范围
    ERR_B1_LSR_B_BASE_TEMP          = 0x04, // 激光器模块底板温度过高或过低
    ERR_B1_LSR_B_DIODE_TEMP         = 0x05, // 激光二极管温度过高或过低

    ERR_B1_TIME_OUT                 = 0xfd, // 超时
    ERR_B1_OFFLINE                  = 0xfe  // 掉线
} emB1ErrTdf;

/**************************2000   B2 MAIN************************************/
typedef enum _emB2CmdTdf {
    CMD_B2_NO                       = 0x00,
    CMD_B2_CountStart               = 0x01, // 开始计数流程,主板开始pcm计数，并开始使用网络传输细胞数据。
    CMD_B2_CountStop                = 0x02, // 停止计数，主板停止细胞数据的发送。
    CMD_B2_SampDbusStart            = 0x03, // 开始原始数据采集
    CMD_B2_SetUsbTrs                = 0x04, // 设置usb传输模式
    CMD_B2_SetNetTrs                = 0x05, // 设置网络传输模式
    CMD_B2_SetGeta                  = 0x06, // 设置识别识别阈值，带参数，参数在swap区，4字节：设置第1细胞识别通道信息，高24位为阈值，低8位为通道地址
    CMD_B2_SetMidValGeta            = 0x07, // 设置最小峰值输出阈值，4字节：设置第1细胞识别通道信息，高24位为阈值，低8位为通道地址
    CMD_B2_SetPeakGeta              = 0x08, // 设置最大峰值输出阈值，4字节：设置第1细胞识别通道信息，高24位为阈值，低8位为通道地址
    CMD_B2_SetAdcBusMonitorOn       = 0x09, // 启动adc的总线实时数据返回
    CMD_B2_SetAdcBusMonitorOff      = 0x0a, // 关闭adc的总线实时数据返回
    CMD_B2_FpgaPeakReset            = 0x0b, // 复位检测模块和中值检测模块
    CMD_B2_SetGeta2                 = 0x0c, // 阈值设置第二段，高低灵敏度调试用
    CMD_B2_PARA_VER_SET             = 0x0d, // 阈值选择指令，高低灵敏度调试用

    CMD_B2_SetWGeta                 = 0x0e, // 宽度阈值设置
    CMD_B2_SetWGeta2                = 0x0f,

    /* 获取空闲计数 */
    CMD_B2_GetPcmCntIdle            = 0x10,
    CMD_B2_COUNT_ADD                = 0x11, // 设置计数时间增量

    CMD_B2_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_B2_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_B2_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_B2_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emB2CmdTdf;

typedef enum _emB2ErrTdf {
    ERR_B2_OK                       = 0x00,
    ERR_B2_PARA                     = 0x01, // 参数错误，未有初始化参数
    ERR_B2_FLASH                    = 0x02, // 存储数据异常
    ERR_B2_NO_CMD_EXIST             = 0x30, // 命令不存在

    ERR_B2_TIME_OUT                 = 0xfd, // 超时
    ERR_B2_OFFLINE                  = 0xfe  // 掉线
} emB2ErrTdf;

/**************************2000   B3 LIQ ************************************/
typedef enum {
    CMD_B3_IDLE                     = 0x00,
    CMD_B3_GET_VER                  = 0x01, // 获取版本号等相关参数
    CMD_B3_CHANGE_SHEATH_T          = 0x02, // 更改鞘流时间
    CMD_B3_WRITE_PAR                = 0x03, // 将输入资源中的参数写入到系统参数中并保存到Flash中
    CMD_B3_RST                      = 0x04, // 液路复位
    CMD_B3_EMPTY_CHK                = 0x05, // 空白检测
    CMD_B3_FLOOD_SHEATH             = 0x06, // 自动灌注鞘液
    CMD_B3_QUALIT_CONTL_CHK         = 0x07, // 质控检测
    CMD_B3_WASH_S                   = 0x08, // 深入清洗
    CMD_B3_WASTE_FULL_EN            = 0x09, // 废液故障使能
    CMD_B3_KEEP_FIT                 = 0x0a, // 液路保养
    CMD_B3_FLA_EMPTYING             = 0x0b, // FLA排空
    CMD_B3_SHEATH_HOT               = 0x0c, // 鞘液反应杯加热
    CMD_B3_BUILD_PS1_PRESS          = 0x0d, // 建立PS1压力
    CMD_B3_FLOOD_SHEATH_HEAT        = 0x0e, // 向鞘液加热杯灌注鞘液
    CMD_B3_AIR_OUT                  = 0x0f, // 排管路气泡
    CMD_B3_Packup_And_Shutdown      = 0x10, // 运输前关机流程
    CMD_B3_BUILD_PRESS              = 0x11, // 建立负压
    CMD_B3_REMOVE_PRESS             = 0x12, // 解除负压
    CMD_B3_ME_WASH                  = 0x13, // 清洗槽定位
    CMD_B3_ME_SAMP                  = 0x14, // 采样位定位
    CMD_B3_ME_ST                    = 0x15, // 急诊位定位
    CMD_B3_ME_REACT                 = 0x16, // 反应杯位定位
    CMD_B3_CHAGE_YDYE               = 0x17, // 更换黄色染色液
    CMD_B3_CHAGE_RDYE               = 0x18, // 更换紫色染色液
    CMD_B3_CHAGE_DILUENT            = 0x19, // 更换稀释液
    CMD_B3_QUALIT_ST_CHK            = 0x1A, // 原样本检测
    CMD_B3_CLEAN_SIMPL              = 0x1B, // 简单清洗
    CMD_B3_DRAIN                    = 0x1C, // 液路排空
    CMD_B3_DILUENT_FLOOD            = 0x1D, // 灌注稀释剂
    CMD_B3_Y_DYE_AGNT_FLOOD         = 0x1E, // 灌注黄染色剂
    CMD_B3_STOP_FLOOD_SHEATH        = 0x1F, // 停止灌注鞘液
    CMD_B3_END_RUN                  = 0x20, // 结束运行     2019.10.29 8:39 lixx
    CMD_B3_MORE_SAMP_FST            = 0x21, // 连续进样检测开始
    CMD_B3_MORE_SAMP_CHK            = 0x22, // 连续进样检测
    CMD_B3_MORE_SAMP_END            = 0x23, // 连续进样检测结束
    CMD_B3_CHAGE_SHEAT              = 0x24, // 更换鞘液
    CMD_B3_SINGLE_ST_CHK            = 0x25, // 急诊
    CMD_B3_SHEATH_HOT_STOP          = 0x26, // 鞘液反应杯加热停止
    CMD_B3_SHUTDOWN_MOTOR_EN        = 0x27, // 关闭电机使能
    CMD_B3_OPEN_MOTOR_EN            = 0x28, // 开启电机使能
    CMD_B3_ADJ_LEVEL                = 0x29, // 调整液面
    CMD_B3_BUILD_PS2_PRESS          = 0x2A, // 建立PS2压力
    CMD_B3_BUILD_PS5_PRESS          = 0x2B, // 建立PS5压力
    CMD_B3_R_DYE_AGNT_FLOOD         = 0x2C, // 灌注紫染色剂
    CMD_B3_CAIL_10CV                = 0x2D, // 10CV校准流程
    CMD_B3_REAT_WASH                = 0x2E, // 反应杯清洗
    CMD_B3_R_DYE_ANALY              = 0x2F, // 分析紫染色剂
    CMD_B3_Y_DYE_ANALY              = 0x30, // 分析黄染色剂
    CMD_B3_DILUENT_ANALY            = 0x31, // 分析稀释剂
    CMD_B3_ERR_END_RUN              = 0x32, // 故障停止运行流程
    CMD_B3_RUN_STOP                 = 0x33, // 急停
    CMD_B3_CANCEL_STOP              = 0x34, // 取消急停
    CMD_B3_SHEAT_CHK	            = 0x35, // 鞘流检测
    CMD_B3_DORMANCY                 = 0x36, // 休眠
    CMD_B3_AWAKEN	                = 0x37, // 唤醒
    CMD_B3_SHEATH_EMPTYING          = 0x38, // 鞘液排空

    CMD_B3_FLB_EMPTYING             = 0x39, // FLB排空

    CMD_B3_NEELD_BLOCK              = 0x3A, // 样品针排堵
    CMD_B3_SH_BLOCK                 = 0x3B, // 鞘流排堵
    CMD_B3_MA_TEST                  = 0x3C, // MA泵测试
    CMD_B3_MB_TEST                  = 0x3D, // MB泵测试
    CMD_B3_MC_TEST                  = 0x3E, // MC泵测试
    CMD_B3_DUNCT_CHK                = 0x3F, // 电导率检测

    CMD_B3_DBUG_ON                  = 0x40, // 进入调试模式
    CMD_B3_DBUG_OFF                 = 0x41, // 退出调试模式

    CMD_B3_SET_VAR_EN               = 0X50, // 设置阀使能
    CMD_B3_SET_OPT_EN               = 0X51, // 设置光耦使能
    CMD_B3_SET_VAR                  = 0X52, // 设置阀动作
    CMD_B3_SET_PUMP                 = 0X53, // 设置泵动作
    CMD_B3_SET_MOTOR                = 0X54, // 设置电机动作
    CMD_B3_SET_Y_CALI               = 0X55, // X电机定位校准
    CMD_B3_SET_X_CALI               = 0X56, // Y电机定位校准
    CMD_B3_SET_CONDCAL              = 0x57, // 电导率校准指令
    CMD_B3_SET_SAVE                 = 0x58, // 保存
    CMD_B3_SET_CANCEL               = 0x59, // 取消
    CMD_B3_SET_AUTO_MIX             = 0x5A, // 设置自动混匀
    CMD_B3_ME_KEEP                  = 0x5B, // 清洗液位置

    CMD_B3_UFL_A_CRL                = 0x5C, // 清UFL_A计数
    CMD_B3_UFL_B_CRL                = 0x5D, // 清UFL_B计数
    CMD_B3_CLEAR_CRL                = 0x5E, // 清清洗液计数
    CMD_B3_DILUENT_CRL              = 0x5F, // 清稀释液计数
    CMD_B3_SHEATH_CRL               = 0x60, // 清鞘液计数
    CMD_B3_FACT_REST                = 0x61, // 恢复出厂值
    CMD_B3_ME_LOST_REST             = 0x62, // Me失步故障恢复
    CMD_B3_SAMP_REST                = 0x63, // 采样针复位
    CMD_B3_MORE_CNT_CHK             = 0x64, // 连续进样计数流程
    CMD_B3_ERR_CEALR                = 0x65, // 故障清除
    CMD_B3_OPT_CAIL                 = 0x66, // 夹管光耦校准指令
    CMD_B3_REAGENT_DILUTION         = 0x67, // 试剂稀释量分析
    CMD_B3_DILUENT_EMPTYING         = 0x68, // 稀释液排空
    CMD_B3_DIAM_CALIB_CHK           = 0x69, // 定标检测流程
    CMD_B3_CLEAN_CNT_DATA           = 0x6A, // 清计数
    CMD_B3_BATCH_SET                = 0x6B, // 批量设置（预留）
    CMD_B3_CLEAR_CMD                = 0xFF, //

    CMD_B3_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_B3_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_B3_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_B3_UPDATE_STOP              = 0xEF  // 进入固件升级退出
}emB3CmdTdf;

typedef enum {
    ERR_B3_NO                       = 0x00,
    ERR_B3_PINCH_OPT_CAIL_ERR       = 0x01, // 夹管光耦校准失败
    ERR_B3_DECOMPRE_OVER_ERR        = 0x02, // 泄压过压故障
    ERR_B3_DECOMPRE_LOW_ERR         = 0x03, // 泄压欠压故障
    ERR_B3_RI_H_OPT_ERR             = 0x04, // 清洗液灌上限位浮子故障
    ERR_B3_FLA_OPT_ERR              = 0x05, // FLA陶瓷泵光耦状态异常
    ERR_B3_FLB_OPT_ERR              = 0x06, // FLB陶瓷泵光耦状态异常
    ERR_B3_FLA_OPT_EMTY_NA          = 0x07, // FLA染色液管空
    ERR_B3_CLEAN_EMTY               = 0x08, // 鞘液灌注异常
    ERR_B3_WASTE_FULL               = 0x09, // 废液满
    ERR_B3_SAMP_NEEDLE_BLOCKAGE     = 0x0A, // 采样针堵塞

    ERR_B3_FLASH                    = 0x0B, // 存储异常
    ERR_MOTOR_SAVE_STEP             = 0X0C, // 采样针定位步数未存储
    ERR_B3_TEMP_1                   = 0x0D, // 反应杯温度传感器故障
    ERR_B3_TEMP_2                   = 0x0E, // 鞘液加热杯温度传感器故障
    ERR_B3_TEMP_3                   = 0x0F, // 环境温度传感器故障
    ERR_B3_ELE_HOT_1                = 0x10, // 反应杯加热故障
    ERR_B3_ELE_HOT_2                = 0x11, // 鞘液加热杯加热故障
    ERR_B3_REACT_OVER_TEMP          = 0x12, // 反应杯过温故障
    ERR_B3_SHEATH_OVER_TEMP         = 0x13, // 鞘液加热杯过温故障
    ERR_B3_PRES_3_NA                = 0x14, // 压力传感器故障3
    ERR_B3_PRES_4_NA                = 0x15, // 压力传感器故障4
    ERR_B3_PRES_5_NA                = 0x16, // 压力传感器故障5

    ERR_B3_PS1_OVER                 = 0x17, // PS1过压
    ERR_B3_PS2_OVER                 = 0x18, // PS2过压
    ERR_B3_PS3_OVER                 = 0x19, // PS3过压
    ERR_B3_PS4_OVER                 = 0x1A, // PS4过压
    ERR_B3_PS5_OVER                 = 0x1B, // PS5过压
    ERR_B3_PS1_LOW                  = 0x1C, // PS1欠压
    ERR_B3_PS2_LOW                  = 0x1D, // PS2欠压
    ERR_B3_PS3_LOW                  = 0x1E, // PS3欠压
    ERR_B3_PS4_LOW                  = 0x1F, // PS4欠压
    ERR_B3_PS5_LOW                  = 0x20, // PS5欠压

    ERR_B3_MA_LOST_STEP             = 0x21, // MA失步故障
    ERR_B3_MB_LOST_STEP             = 0x22, // MB失步故障
    ERR_B3_MC_LOST_STEP             = 0x23, // MC失步故障
    ERR_B3_MD_LOST_STEP             = 0x24, // MD失步故障
    ERR_B3_ME_REST_STEP             = 0x25, // ME复位故障
    ERR_B3_ME_LOST_STEP             = 0x26, // ME定位故障

    ERR_B3_CPU0_WAIT_OVER_TIME      = 0x27, // 液路流程0等待超时
    ERR_B3_CPU1_WAIT_OVER_TIME      = 0x28, // 液路流程1等待超时
    ERR_B3_CPU2_WAIT_OVER_TIME      = 0x29, // 液路流程2等待超时

    ERR_B3_SH_BLOCKAGE              = 0x2A, // 鞘流器堵塞
    ERR_B3_SAMP_BLOCKAGE            = 0x2B, // 样品管堵塞

    ERR_B3_VM_CMD                   = 0x2C, // VM指令错误
    ERR_B3_KEEPFLUID                = 0x2D, // 清洗液不足
    ERR_B3_SAMP_SHORTAGE            = 0X2E, // 样本量不足
    ERR_B3_SAMP_CHK                 = 0X2F, // 样本量检测异常
    ERR_B3_CONDUCT_CAIL             = 0x30, // 电导率校准失败
    ERR_B3_AMBIENT_TEMP_OVERRUN     = 0x31, // 环温超限
    ERR_B3_CPU3_WAIT_OVER_TIME      = 0x32, // 液路流程3等待超时
    ERR_B3_PINCH_OPT_NO_CAIL        = 0x33, // 夹管光耦未校准

    ERR_B3_TIME_OUT                 = 0xfd, // 超时
    ERR_B3_OFFLINE                  = 0xfe  // 掉线
} emB3ErrTdf;

typedef enum _emB3SheathUsageTdf {
    LIQ_EMPTY_USAGE                   = 23, // 空白检测 ok
    LIQ_FLOOD_USAGE                   = 0,  // 自动灌注鞘液  其他流程计算 ok
    LIQ_WASH_S_USAGE                  = 33, // 深入清洗     OK
    LIQ_KEEP_FIT_USAGE                = 33, // 液路保养
    LIQ_BLOCKAGE_USAGE                = 0,  // 采样针排堵流程
    LIQ_FLOOD_SHEATH_USAGE            =337, //  空 管路灌液
    LIQ_AIR_OUT_USAGE                 = 10, // 排管路气泡
    LIQ_QUALIT_ST_USAGE               = 35, // 急诊位质控检测
    LIQ_CLEAN_SIMPL_USAGE             = 25, // 简单清洗
    LIQ_DRAIN_USAGE                   = 0,  // 液路排空   灌满已经计算
    LIQ_MORE_SAMP_FST_USAGE           = 33, // 连续进样检测开始 ok
    LIQ_MORE_SAMP_CHK_USAGE           = 32, //= 30, // 连续进样检测
    LIQ_MORE_SAMP_END_USAGE           = 0,  // 连续进样检测结束
    LIQ_CHAGE_SHEAT_USAGE             = 40, // 更换鞘液
    LIQ_SINGLE_ST_USAGE               = 32, // 急诊位单进样检测  OK
    LIQ_ADJ_LEVEL_USAGE               = 10, //70 // 调整液面ok
    LIQ_CAIL_10CV_USAGE               = 32, // 10CV校准流程ok
    LIQ_REAT_WASH_USAGE               = 33, // 反应杯清洗OK
    LIQ_AWAKEN_USAGE                  = 25, // 唤醒OK
    LIQ_SAMP_BLOCK_USAGE              = 3,  // 样品管排堵0k
    LIQ_NEELD_BLOCK_USAGE             = 0,  // 采样针排堵
    LIQ_SH_BLOCK_USAGE                = 57, // 鞘流排堵ok
    LIQ_BUILD_PRSS_USAGE              = 10, // 建立压力 OK
    LIQ_QUALIT_USAGE                  = 32, // 质控检测 ok
} emB3SheathUsageTdf;//鞘流使用量 ml

typedef enum _emB3DiluentUsageTdf {
    D_WASH_S_USAGE                    = 50, // 深入清洗     OK
    D_NORMAL_CHK_USAGE                =345, // 正常检测
    D_KEEPFIT_CHK_USAGE               =300, // 液路保养
    D_CHANGE_CHK_USAGE                =500, // 稀释液更换
} emB3DiluentUsageTdf;//稀释剂使用量 ml

/**************************2000   B6*****************************************/
typedef enum _emB6ErrTdf {
    ERR_B6_OK                       = 0x00,
    ERR_B6_NO_CMD                   = 0x20,

    ERR_B6_TIME_OUT                 = 0xfd, // 超时
    ERR_B6_OFFLINE                  = 0xfe  // 掉线
} emB6ErrTdf;

/**************************2000   B7*****************************************/
typedef enum _emB7CmdTdf
{
    CMD_B7_IDLE                     = 0x00,
    CMD_B7_SYS_RUN_ST               = 0x20, //系统运行状态 0 表示不运行 1表示运行
    CMD_B7_Rfid_Read                = 0x21, // 加密模块读取数据
    CMD_B7_Rfid_Erase               = 0x22, // 擦除(未使用)
    CMD_B7_Rfid_Write               = 0x23, // 写 // 关，byFind置0，等待上位机命令

    CMD_B7_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_B7_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_B7_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_B7_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emB7CmdTdf;

typedef enum _emB7ErrTdf {
    ERR_B7_OK                       = 0x00,
    ERR_B7_LEAKAGE                  = 0x01, // 漏液故障
    ERR_B7_YDYEH                    = 0x02, // FLA染色液未放入
    ERR_B7_YDYEV                    = 0x03, // FLA染色液门未关
    ERR_B7_RDYEH                    = 0x04, // FLB染色液未放入
    ERR_B7_RDYEV                    = 0x05, // FLB染色液门未关
    ERR_B7_KEEPFITH                 = 0x06, // 保养液未放入

    ERR_B7_TIME_OUT                 = 0xfd, // 超时
    ERR_B7_OFFLINE                  = 0xfe  // 掉线
} emB7ErrTdf;

/*******************************************************************************
宏定义函数申明
*******************************************************************************/

/*******************************************************************************
输出变量申明
*******************************************************************************/

/*******************************************************************************
输出函数申明
*******************************************************************************/

#endif
