/*******************************************************************************
* Copyright URIT Co.,Ltd. All Right Reserved
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/*******************************************************************************
* File Name     : client_cx_cmds.h
* File Path     : platcode/clients/
* Project       : urit1 plat
* Author        : lixiaoxiang
* Date          : 2023.02.10
* Description   : mcu io base
*******************************************************************************/

/*******************************************************************************
* modify logs:
* 2023.02.10 lixiaoxiang create
* * * * modify logs:
* 2023.02.23 ps add sl000 cmds
* * * * * modify logs:
* 2023.09.19 ps add mp-1200 cmds
*******************************************************************************/

#ifndef __CLIENTS_DX_CMDS_H_
#define __CLIENTS_DX_CMDS_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/**************************MP1200 D0*****************************************/
typedef enum
{
    ERR_D0_DYE1_OUT_FAIL  = 0x01,    // 工位1放行后染色盒未移走
    ERR_D0_DYE2_OUT_FAIL  = 0x02,    // 工位2放行后染色盒未移走
    ERR_D0_DYE3_OUT_FAIL  = 0x03,    // 工位3放行后染色盒未移走
    ERR_D0_DYE4_OUT_FAIL  = 0x04,    // 工位4放行后染色盒未移走
    ERR_D0_RACK_TUBE      = 0x05,    // 检测试管架/位与实际不符
    ERR_D0_SLIDE_CAR_NONE = 0x06,    // 玻片小车无玻片
    ERR_D0_FRONT_COVER  = 0x07,      // 前盖打开
    ERR_D0_NO_BASKET_ID  = 0x08,     // 读取玻片篮号异常
    ERR_D0_DRY_TEMP_LOW = 0x09,     // 烘干温度偏低
    ERR_D0_DRY_TEMP_HIGH = 0x0A,    // 烘干温度偏高

    ERR_D0_DYE1_NO_BOX = 0X0B,      // 工位1未找到染色盒
    ERR_D0_DYE2_NO_BOX = 0X0C,      // 工位2未找到染色盒
    ERR_D0_DYE3_NO_BOX = 0X0D,      // 工位3未找到染色盒
    ERR_D0_DYE4_NO_BOX = 0X0E,      // 工位4未找到染色盒

    ERR_D0_INVALID_RACK = 0X0F,     // 无效试管架号
    ERR_D0_IPU_OFFLINE = 0x10,      // IPU断开连接
    ERR_D0_PHOTO_NOT_CLAMP = 0x11, // 拍照位玻片未夹走

    ERR_D0_BSK_ONLINE_NO_REPLY = 0x12, // 玻片篮请求出桥未收到回复

    ERR_D0_D1_OFFLINE = 0x71,   // D1控制板通信异常
    ERR_D0_D2_OFFLINE = 0x72,   // D2控制板通信异常
    ERR_D0_D3_OFFLINE = 0x73,   // D3控制板通信异常
    ERR_D0_D5_OFFLINE = 0x75,   // D5控制板通信异常
    ERR_D0_D6_OFFLINE = 0x76,   // D6控制板通信异常
    ERR_D0_D8_OFFLINE = 0x78,   // D8控制板通信异常

    ERR_D0_D1_TIMEOUT = 0x81,   // D1控制板命令超时
    ERR_D0_D2_TIMEOUT = 0x82,   // D2控制板命令超时
    ERR_D0_D3_TIMEOUT = 0x83,   // D3控制板命令超时
    ERR_D0_D5_TIMEOUT = 0x85,   // D5控制板命令超时
    ERR_D0_D6_TIMEOUT = 0x86,   // D6控制板命令超时
    ERR_D0_D8_TIMEOUT = 0x88,   // D8控制板命令超时

}emD0ErrTdf;

/**************************MP1200 D1*****************************************/
typedef enum
{
//    ERR_D1_OK                       = 0,
    ERR_D1_OK = 0,//0x00
    ERR_D1_M2_REST   = 0x01   ,//扫码夹持电机复位故障
    ERR_D1_M3_REST   = 0x02  ,//混匀Y轴电机复位故障
    ERR_D1_M4_REST   = 0x03  ,//混匀X轴电机复位故障
    ERR_D1_M5_REST   = 0x04  ,//玻片盒电机复位故障
    ERR_D1_M8_REST   = 0x05  ,//夹取试管电机复位故障
    ERR_D1_M9_REST   = 0x06  ,//急诊小车电机复位故障
    ERR_D1_M10_REST  = 0x07  ,//开盖电机复位故障
    ERR_D1_M11_REST  = 0x08  ,//混匀电机复位故障
    ERR_D1_CLAMP_TO_TOP    = 0X09  ,//夹取试管电机向上运行故障
    ERR_D1_RObOtCLIP08_NOCLAMP  = 0x0a,//夹紧试管机械手未关闭
    ERR_D1_CLAMP_TO_CAR = 0x0b,//夹取试管电机运行到急诊小车夹取位故障
    ERR_D1_M15_REST  = 0x0c  ,//拨载玻片电机复位故障
    ERR_D1_READ_BARCODE      = 0x0d, //读条码超时
    ERR_D1_RObOtCLIP08_OPEN  = 0x0e, //夹紧试管机械手打开故障
    ERR_D1_RObOtCLIP08_NOCLAMPTUBE  = 0x0f, //夹紧试管机械手未夹到试管
    ERR_D1_NOHASGLASS        = 0x10,     //无载玻片

    ERR_D1_NOHASlIDEBOX      = 0x12,   //无载玻片盒
    ERR_D1_TOO_FEW_SLIDES    = 0x14, //盒内载玻片过少
    ERR_D1_TUBETOCAR_RST     = 0x15, //夹取试管电机从急诊小车向上复位位置故障
    ERR_D1_NOMOVETOPRINT      = 0x16,//未能拨出载玻片到打印位故障

    ERR_D1_RObOtCLIP09_OPEN  = 0x17, //试管帽夹爪打开故障

    ERR_D1_AIRCLAW_NO_OPEN   = 0x19, //取试管气爪未打开故障
    ERR_D1_AIRCLAW_NO_CLOSE  = 0x1a, //取试管气爪未关闭故障
    ERR_D1_AIRCLAW_NOCLAMPTUBE = 0x1b,//取试管气爪未夹到试管
    ERR_D1_UPDATA            = 0x1c,//U盘升级失败
    ERR_D1_FORWARD_RST       = 0x1d,//混匀X轴电机向前复位光耦故障
    ERR_D1_UPWARD_RST        = 0x1e,//混匀Y轴电机向上复位光耦故障

    ERR_D1_CARRUNTORECTUBE   = 0x20,//急诊小车未能运行到接试管位置故障
    ERR_D1_CARRUNTOCoverLid  = 0x21,//急诊小车未能运行到开压盖位置故障
    ERR_D1_CARRUNTOSample    = 0x22,//急诊小车未能运行到采样位置故障
    ERR_D1_RObOtCLIP09_CLOSE  = 0x23,//试管帽夹爪关闭故障
    ERR_D1_RObOtCLIP09_NO_RESPONSE = 0x24,//开盖机械手未响应
    ERR_D1_RObOtCLIP09_RST   = 0x25,//开盖机械手复位故障
    ERR_D1_RObOtCLIP09_ROTAR   = 0x26,//开盖机械手旋转障
} emD1ErrTdf;

typedef enum
{
    CMD_D1_IDLE                     = 0,
    CMD_D1_0_RESET                    = 0x01,     //复位命令

    CMD_D1_1_SCANOPEN                 = 0x02,     //扫条码试管夹打开
    CMD_D1_1_SCANNING                 = 0x03,     //扫条码试管

    CMD_D1_2_MIXWEL                   = 0x04,     //混匀命令

    CMD_D1_3_TUBE_TO_SAMPLING         = 0x05,     //夹取试管到采样位命令，用于从进样机构夹取试管到滴样位，在此过程会完成试管内有无血液的判断、试管摇匀、开盖等动作。
    CMD_D1_3_SAMPLING_TO_TUBE         = 0x06,     //试管从采样位放回到试管架试管位命令，此命令过程中将完成试管盖盖子、夹回试管等动作
    CMD_D1_3_EMCTUBE                  = 0x07,     //急诊未稍血试管伸出接试管
    CMD_D1_3_EMCTUBERETERN            = 0x08,     //急诊未稍血试管伸出接到试管后小车复位到采样位

    CMD_D1_4_PUSH_OUT_SLIDE           = 0x09,     //拨出载玻片结构命令,此命令需要带参数

    CMD_D1_0_GETOPC                   = 0x0a,     //读取光耦状态 读取D1机构各光耦状态命令
    CMD_D1_0_DEBUG                    = 0x0b,     //调试命令

    CMD_D1_3_EMG_TO_SAMPLE            = 0x0c,     //急诊到采样位，带试管类型参数
    CMD_D1_3_EMG_END_OUT              = 0X0d,     //急诊采样结束推出来，带试管类型参数

    CMD_D1_0_ERR_NOTICE               = 0X0e,     //上位机通知接收到故障

    CMD_D1_3_TUBE_CLEAR               = 0x0F,     // 试管清理，一个参数 0：夹爪在上端，1：夹爪在下端。 夹爪按参数复位，开盖机构复位，小车通过急诊位送出
    CMD_D1_3_TUBE_CLEAR_RST           = 0x10,     // 试管清理后，小车和夹爪复位至待机位

    CMD_D1_3_TUBE_AUTO_CLEAR          = 0x11,     // 自动压盖，清理，用于初次修复故障 两个参数 试管类型、是否急诊

    CMD_D1_0_REFRESH                  = 0xea,     //刷新参数。
    CMD_D1_0_LOAD                     = 0xeb,     //加载参数。
    CMD_D1_0_SAVE                     = 0xec,     //保存参数  ， 将调试好的参数进行保存。
    CMD_D1_0_GETVER                   = 0xed,     //获取版本号命令
    CMD_D1_0_UPDATA_START             = 0xee,     //开始升级
} emD1CmdTdf;

// 扫码机构调试功能
typedef enum
{
    DEBUG_D1_SCAN_OPEN = 0x01,     // 扫码打开
    DEBUG_D1_SCAN_CODE = 0x02,     // 扫码
    DEBUG_D1_SCAN_TIME = 0x03,     // 扫码时间调试
    DEBUG_D1_D2_AGED_TEST = 0x05,  // 机构老化
}emD1DebugScanTdf;

// 混匀机构调试功能
typedef enum
{
    DEBUG_D1_MIX_GET_TUBE = 0x01,  // 夹取试管步数调试
    DEBUG_D1_MIX_BACK = 0x02,      // 试管夹退回步数
    DEBUG_D1_MIX_ANGLE = 0x03,     // 混匀角度
    DEBUG_D1_MIX_IDENTIFY = 0x04,  // 试管识别位置步数
    DEBUG_D1_MIX_PROC = 0x05,      // 混匀流程
    DEBUG_D1_MIX_GET_RST = 0x06,   // 夹取试管电机快速复位
    DEBUG_D1_MIX_Y_RST = 0x07,     // Y轴电机快速复位
    DEBUG_D1_MIX_RST = 0x08,       // 混匀电机快速复位
    DEBUG_D1_MIX_AGED = 0x09,      // 老化测试
}emD1DebugMixTdf;

// 夹试管机构调试功能
typedef enum
{
    DEBUG_D1_CLAMP_GET_TO_TOP = 0x01,      // 取试管运行到最高位命令
    DEBUG_D1_CLAMP_PUT_TO_CAR = 0x02,      // 试管放置到急诊小车上
    DEBUG_D1_CLAMP_CLOSE = 0x03,           // 试管夹关闭
    DEBUG_D1_CLAMP_OPEN = 0x04,            // 试管夹打开
    DEBUG_D1_CLAMP_RST = 0x05,             // 夹取电机快速复位
    DEBUG_D1_CLAMP_AGED = 0x06,            // 老化
}emD1DebugClampTdf;

// 试管小车调试功能
typedef enum
{
    DEBUG_D1_TUBECAR_RECV_WHOLE = 0X01,         // 接全血试管步数
    DEBUG_D1_TUBECAR_WHOLE_OPEN = 0X02,         // 全血试管开盖步数
    DEBUG_D1_TUBECAR_WHOLE_SAM = 0X03,          // 全血试管采样位步数
    DEBUG_D1_TUBECAR_RECV_PERIPHERAL = 0X04,    // 接末梢血试管命令
    DEBUG_D1_TUBECAR_PERIPHERAL_SAM = 0X06,     // 末梢血采样
    DEBUG_D1_TUBECAR_PERIPHERAL_MIX = 0X07,     // 末梢血混匀
    DEBUG_D1_TUBECAR_RECV_MICRO = 0X08,         // 接微量血
    DEBUG_D1_TUBECAR_MICRO_SAM = 0X09,          // 微量血采样
    DEBUG_D1_TUBECAR_RST = 0X0a,                // 小车电机复位
    DEBUG_D1_TUBECAR_AGED = 0X10,               // 老化
}emD1DebugTubeCarTdf;

// 开盖机构调试功能
typedef enum
{
    DEBUG_D1_LID_OPEN_MAX = 0X01,           // 常规试管开盖步数
    DEBUG_D1_LID_COVER_HAT = 0X02,          // 常规试管压盖步数
    DEBUG_D1_LID_OPEN_SARSTEDT = 0X03,      // 欧洲试管开盖步数
    DEBUG_D1_LID_COVER_SARSTEDT = 0X04,     // 欧洲试管压盖步数

    DEBUG_D1_LID_RST        = 0X05,         // 开压盖电机快速复位
    DEBUG_D1_LID_GET_TUBE_PROC = 0X06,      // 夹取试管开盖流程  参数0：末梢  1：常规   2：SARSTEDT
    DEBUG_D1_LID_PUT_TUBE_PROC = 0X07,      // 放回试管压盖流程  参数0：末梢  1：常规   2：SARSTEDT
    DEBUG_D1_LID_TUBE_CLAMP_OPEN = 0X09,    // 试管夹打开命令
    DEBUG_D1_LID_TUBE_CLAMP_CLOSE = 0X0A,   // 试管夹关闭命令
    DEBUG_D1_LID_HAT_CLAMP_OPEN = 0X0B,     // 试管帽夹打开命令
    DEBUG_D1_LID_HAT_CLAMP_CLOSE = 0X0C,    // 试管帽夹关闭命令

    DEBUG_D1_LID_AGED = 0X08,               // 老化 参数0：末梢  1：常规   2：SARSTEDT
}emD1DebugLiqTdf;

// 玻片盒机构调试功能
typedef enum
{
    DEBUG_D1_SLIDEBOX_TO_PRINT = 0X01,      // 玻片打印位置步数
    DEBUG_D1_SLIDEBOX_TO_CAT = 0X02,        // 玻片到小车步数
    DEBUG_D1_SLIDEBOX_OUT = 0X05,           // 玻片盒推出
    DEBUG_D1_SLIDEBOX_IN = 0X06,            // 玻片盒进
    DEBUG_D1_SLIDEBOX_MOVE_MOT_RST = 0X07,  // 玻片移动电机快速复位
    DEBUG_D1_SLIDEBOX_AGED = 0X0B,          // 老化

}emD1DebugSlideBoxTdf;

typedef enum  //注：此处定义的命令为CMD_D1_PUSH_OUT_SLIDE的参数
{
    CMD_D1_SLIDE_RESET                      = 0x01,
    CMD_D1_SLIDE_TO_PRINTED                 = 0x02,    //载玻片推出到滴样位
    CMD_D1_PRINTED_TO_CAR                   = 0x03,   //载玻片从滴样位摔推到D2小车上
    CMD_D1_BOX_OUT                          = 0x04,    //载玻片盒退出
    CMD_D1_BOX_IN                           = 0x05,    //载玻片盒进入
    CMD_D1_PRINT_OPEN                       = 0x06,
    CMD_D1_PRINT_CLOSE                      = 0x07,
} emD1SlideCmdTdf;
/**************************MP1200 D2*****************************************/
typedef enum
{
    ERR_D2_OK                       = 0,
    ERR_D2_M1_REST                  = 0x01,     //上层机械手X轴电机复位故障
    ERR_D2_M2_REST                  = 0x02,     //上层机械手Y轴电机复位故障
    ERR_D2_M3_REST                  = 0x03,     //竖起电机复位故障
    ERR_D2_M4_REST                  = 0x04,     //运送小车电机复位故障
    ERR_D2_M5_REST                  = 0x05,     //拍照平台电机复位故障
    ERR_D2_M6_REST                  = 0x06,     //推片X轴电机复位故障
    ERR_D2_M7_REST                  = 0x07,     //推片Y轴电机复位故障
    ERR_D2_M8_REST                  = 0x08,     //玻片篮Y轴电机复位故障
    ERR_D2_M9_REST                  = 0x09,     //玻片篮X电机复位故障
    ERR_D2_M10_REST                 = 0x0a,     //玻片篮OUT传送带电机复位故障
    ERR_D2_M11_REST                 = 0x0b,     //玻片篮IN传送带电机复位故障
    ERR_D2_M12_REST                 = 0x0c,     //玻片篮联机电机复位故障
    ERR_D2_M13_REST                 = 0x0d,     //下层机械手X轴电机复位故障
    ERR_D2_M14_REST                 = 0x10,     //下层机械手Z轴电机复位故障
    ERR_D2_M15_REST                 = 0x0f,     //染色转盘电机复位故障
    ERR_D2_M16_REST                 = 0x0e,     //下层机械手Y轴电机复位故障
    ERR_D2_M17_REST                 = 0x11,     //染色加液电机复位故障
    ERR_D2_M18_REST                 = 0x12,     //上下层机械手电机复位故障
    ERR_D2_NOBASKETIN               = 0x13,     //入口无玻片篮
    ERR_D2_BASKETOUT                = 0x14,     //玻片篮OUT传送电机或内侧光耦故障
    ERR_D2_BASKECAL                 = 0x15,     //玻片篮校准失败
    ERR_D2_BASKETIN                 = 0x16,     //玻片篮IN传送电机或外侧光耦故障
    ERR_D2_BASKETINTOEND            = 0x17,     //玻片篮IN传送电机或内侧光耦故障
    ERR_D2_BASKETOUTTOEND           = 0x18,     //玻片篮OUT传送电机或联机光耦故障
    ERR_D2_BASKETFULL               = 0x19,     //玻片篮满
    ERR_D2_M12_ONLINEREST           = 0x1a,     //联机电机或光耦复位故障
    ERR_D2_CMD_D6                   = 0x1b,     //D2发送命令到D6错误
    ERR_D2_SLIDECARTODROP           = 0x1C,     // 玻片小车运行到滴液位故障
    ERR_D2_SLIDECARTOPUSH           = 0x1D,     // 玻片小车运行到推片位故障
    ERR_D2_SLIDECARTOTURN           = 0x1E,     // 玻片小车运行到翻转位故障
    ERR_D2_GRABTOFAN1               = 0x1F,     // 玻片夹取到风干位1故障
    ERR_D2_GRABTOFAN2               = 0x20,     // 玻片夹取到风干位2故障
    ERR_D2_XMOVETOPUSH              = 0x21,     // 推片X轴电机运行到推片起始位故障
    ERR_D2_YMOVETOCLEAN             = 0x22,     // 推片Y轴电机运行到清洗位故障
    ERR_D2_DOWNTOPIC                = 0x23,     // 上下层机械手到拍照夹取位故障
    ERR_D2_DOWNTODYE                = 0x24,     // 上下层机械手到染色盒位故障
    ERR_D2_MACHINE_X                = 0x25,     // 下层机械手到X轴位置故障
    ERR_D2_MACHINE_Y                = 0x26,     // 下层机械手到Y轴位置故障
    ERR_D2_MACHINE_Z                = 0x27,     // 下层机械手到Z轴位置故障
    ERR_D2_BASKET_IN_X              = 0x28,     // 玻片篮X轴到入口位置运行故障
    ERR_D2_DYE_NEEDLE_DOWN          = 0x29,     // 染色加液电机下降故障
    ERR_D2_BASKET_OUT_X             = 0x2A,     // 玻片篮X轴到出口位置运行故障
    ERR_D2_GRABTOCAMERA             = 0x2B,     // 玻片夹取到相机故障
    ERR_D2_UP_GRAB_CLAMP            = 0x2C,     // 上层机械手夹取失败故障
    ERR_D2_UP_GRAB_DROP             = 0x2D,     // 上层机械手放行失败故障
    ERR_D2_UPD_GRAB_CLAMP           = 0x2E,     // 上下层机械手夹取失败故障
    ERR_D2_UPD_GRAB_DROP            = 0x2F,     // 上下层机械手放行失败故障
    ERR_D2_DOWN_GRAB_CLAMP          = 0x30,     // 下层机械手夹取失败故障
    ERR_D2_DOWN_GRAB_DROP           = 0x31,     // 下层机械手放行失败故障
    ERR_D2_TURN_DOWN                = 0x32,     // 竖起电机放平故障
    ERR_D2_UP_GRAB_DROP_F           = 0x33,     // 上层机械手风干位卡玻片
    ERR_D2_UP_GRAB_DROP_P           = 0x34,     // 上层机械手卡拍照位玻片
    ERR_D2_UPD_GRAB_STUCK           = 0x35,     // 上下层机械手染色盒位卡玻片
    ERR_D2_DOWN_GRAB_DROP_H         = 0x36,     // 下层机械手烘干位卡玻片
    ERR_D2_DOWN_GRAB_DROP_B         = 0x37,     // 下层机械手玻片篮卡玻片
    ERR_D2_YMOVETOPUSH              = 0x38,     // 推刀下降到推片位置故障
    ERR_D2_PRESSURE                 = 0x39,     // D2 250KPA气压不足
} emD2ErrTdf;

/******************************************************************************
参数描述：CMD+模块名称+子模块+线程号+命令描述
          例   CMD+  D2    +  F1  + 0    + RESET

无子模块  参数描述：CMD+模块名称+线程号+命令描述
         例   CMD+  D2  + 0    + RESET
******************************************************************************/
typedef enum
{
    CMD_D2_IDLE                         = 0,
    CMD_D2_F1_0_RESET                   = 0x01,   //下层复位命令
    CMD_D2_F1_4_GRIPPERDYE_RES          = 0x02,   //上下层机械手复位到顶
    CMD_D2_F1_4_GRIPPERDYE_DOWN2DYE     = 0x03,   //上下层机械手下降到染色位
    CMD_D2_F1_4_GRIPPERDYE_DOWN2GRAB    = 0x04,   //上下层机械手下降到相机抓取位
    CMD_D2_F1_3_TURNTABLE_RUN           = 0x05,   //转盘转动
    CMD_D2_F1_3_TURNTABLE_STOP          = 0x06,   //转盘停止
    CMD_D2_F1_1_GRIPPERHEAT_RES         = 0x07,   //烘干位机械手复位
    CMD_D2_F1_1_GRIPPER_CLEAN2DRY       = 0x08,   //烘干位机械手清洗位到烘干位（带位置信息参数1,2,3....）
    CMD_D2_F1_1_GRIPPER_DRY2BASKET      = 0x09,   //烘干位到玻片篮（带2个位置信息参数；参数1为夹取烘干位置信息，参数2位送到玻片篮位置信息）
    CMD_D2_F1_1_GRIPPER_CLEAN2BASKET    = 0x0a,   //清洗位到玻片篮（带位置信息参数1,2,3....）
    CMD_D2_F1_1_GRIPPER_BASKET2CLEAN    = 0x0b,   //预留：玻片篮到清洗位（带位置信息参数1,2,3....
    CMD_D2_F1_10_BASKET_IN              = 0x0c,   //传送带空玻片篮进到射频位置
    CMD_D2_F1_10_BASKET_OUT             = 0x0d,   //传送带玻片篮出
    CMD_D2_F1_10_BASKET_ONLINE_PUSH     = 0x0e,   //玻片篮推出联机
    CMD_D2_F1_2_BASKETREC_RES           = 0x0f,   //玻片篮接收架复位
    CMD_D2_F1_2_BASKETREC_IN            = 0x10,   //玻片篮接收架到进口位置
    CMD_D2_F1_2_BASKETREC_OUT           = 0x11,   //玻片篮接收架到出口位置
    CMD_D2_F1_2_BASKETREC_DOWN2HEAT     = 0x12,   //玻片篮接收架到下降到烘干收集位置
    CMD_D2_F1_3_ADD_LIQ_RES             = 0x13,   //添加液体针复位
    CMD_D2_F1_3_ADD_LIQ_DOWN            = 0x14,   //添加液体针下降(用于添加染色液/清洗液等)
    CMD_D2_F1_0_DEBUG                   = 0x15,   //调试
    CMD_D2_F1_0_GETOPC                  = 0x16,   //获取下层光耦状态
    CMD_D2_F1_2_BASKET_IN_BOX           = 0x17,   //传送带空玻片篮进到篮
    CMD_D2_F1_2_BASKET_MOVEONCE         = 0x18,   //玻片篮移动一格
    CMD_D2_F1_10_READBASKETNUM          = 0x19,   //读取玻片篮号码
    CMD_D2_F1_0_SET                     = 0x1a,   //设置命令，带2个参数，0：参数项目 ；1：参数设置模式

    CMD_D2_F1_2_BASKETREC_RST2HEAT     = 0x1b,   //玻片篮接收架  复位位置到烘干收集位置

//    CMD_D2_F1_1_HEATCTRL                = 0x1c,   //控制加热 带一个参数：0 关闭，1 打开
//    CMD_D2_F1_1_SETHEATTEMP             = 0x1d,   //设置加热温度 带一个参数：设置温度值
    CMD_D2_F1_0_AGED_TEST               = 0x1e,   //老练
    CMD_D2_F1_0_Res2Dye                 = 0x1f,   //测试到染色池位
    CMD_D2_F1_3_TURNTABLE_RUNSTEPS      = 0x20,   //转盘按步数转动
    CMD_D2_F1_1_RES2BASKET              = 0x21,   //下层机械手复位位置到玻片篮位置
    CMD_D2_F1_3_LIQ_UP_STEP             = 0x22,   //添加液体针上抬固定步数
    CMD_D2_F1_3_LIQ_DOWN_STEP           = 0x23,   //添加液体针下降固定步数
    CMD_D2_F1_0_MOTOR_RES               = 0x24,   //单电机复位
    CMD_D2_F1_3_ADD_LIQ_END             = 0x25,   //添加液体针下降到最低
    CMD_D2_F1_2_BASKET_OUT2IN           = 0x26,   //玻片篮出口到入口
    CMD_D2_F1_10_BASKETCYCLE            = 0x27,   //玻片篮单机/老练循环推送

    CMD_D2_F2_5_RESET                   = 0x31,   //载玻片运送平台复位
    CMD_D2_F2_5_TRANSPORTTOPUSH         = 0x32,   //载玻片滴样推片位置
    CMD_D2_F2_6_PUSHSLIDES_RESET        = 0x33,   //推片模块复位到待机位（推片位上方）
    CMD_D2_F2_5_PUSH_SLIDES             = 0x34,   //推片：玻片到推片位置、完成推片、推刀清洗、玻片起竖、玻片小车复位

    CMD_D2_F2_5_SILDES_FLIP             = 0x35,   //竖起载玻片
    CMD_D2_F2_5_SILDES_FLIP_RESET       = 0x36,   //放平载玻片
    CMD_D2_F2_7_GRIPPER_RESET           = 0x37,   //上层机械手模块复位，确认各工位是否有玻片
    CMD_D2_F2_7_GRIPPER_MOVE            = 0x38,   //移动机械手到4个位置，带一个位置参数
    CMD_D2_F2_8_CAMERA_BOX_RESET        = 0x39,   //拍照盒复位
    CMD_D2_F2_8_MOVETOCAMERA            = 0x3A,   //拍照盒移动到拍照位
    CMD_D2_F2_8_CAMERATODOWN            = 0x3B,   //拍照位运动到玻片下降位置
    CMD_D2_F2_6_PUSH_CLEAN              = 0x3C,   //推刀清洗
    CMD_D2_F2_0_GETOPC                  = 0x3D,   //获取上层光耦状态
//    CMD_D2_F2_10_FANCTRL                = 0x3E,   //上层风扇控制 带参数 0：风扇1 1：风扇2  2：风扇3
    CMD_D2_F2_9_ULTRASONICCTRL          = 0x3F,   //控制超声波开关 带一个参数：0 关闭，1 打开
    CMD_D2_F2_6_PUSH_WASH               = 0x40,   //????

    CMD_D2_F2_7_RES_POSITIONS           = 0x41,   // 强制将玻片放至相机位
    CMD_D2_F2_5_SLIDE_TO_UPRIGHT        = 0x42,   // 竖起玻片：玻片推到竖起位、玻片起竖、玻片小车复位
    CMD_D2_F2_6_PUSH_CLEAN_MOVE         = 0x43,   //推刀清洗摆动
    CMD_D2_F2_6_PUSH_DOWN_TANK          = 0x44,   // 推刀下降清洗槽
    CMD_D2_F1_1_RST_TO_DRY              = 0x45,   // 烘干位机械手 复位位置到烘干位（带位置信息参数1,2,3....）
    CMD_D2_F2_0_DEBUG                   = 0x4F,    //F2调试

    CMD_D2_7_GRIPPER_MOVE_RES           = 0x50,   //移动机械手到4个位置后复位，带一个位置参数
    CMD_D2_9_GRIPPER_DISABLE            = 0x51,   //IO机械手失能
    CMD_D2_9_GRIPPER_OPEN               = 0x52,   //IO机械手开
    CMD_D2_9_GRIPPER_CLOSE              = 0x53,   //IO机械手关

    CMD_D2_F2_5_MOVE_PUSH               = 0x54,   //推刀移动到推片位置上方
    CMD_D2_F2_5_MOTOR_RES               = 0x55,   //单电机复位
    CMD_D2_0_ERR_NOTICE                 = 0x56,   //错误接收完成通知

    CMD_D2_0_CLEARERR                   = 0xe8,   //清除错误。
    CMD_D2_0_F1_REFRESH                 = 0xe9,   //刷新F1参数。
    CMD_D2_0_F2_REFRESH                 = 0xea,   //刷新F2参数。
    CMD_D2_0_LOAD                       = 0xeb,   //加载参数。
    CMD_D2_0_SAVE                       = 0xec,   //保存参数  ， 将调试好的参数进行保存。
    CMD_D2_0_GETVER                     = 0xed,   //获取版本号命令
    CMD_D2_0_UPDATA_START               = 0xee,   //开始升级
} emD2CmdTdf;

//D15-玻片下降机构
typedef enum
{
    DEBUG_D2_GRIPPER_DOWN2PHOTO_STEP        = 0x01,    //调试上下层机械手夹取相机拍照位置；
    DEBUG_D2_GRIPPER_DOWN2DYE_STEP          = 0x02,    //调试上下层机械手到染色池位置；
    DEBUG_D2_GRIPPER_TEST                   = 0x03,    //测试机械手 1开
//    DEBUG_D2_GRIPPER_CLOSE_TEST             = 0x04,    //测试机械手 关
    DEBUG_D2_DOWN2PHOTO_TEST                = 0x04,    //测试到相机拍照位置
    DEBUG_D2_DOWN2DYE_TEST                  = 0x05,    //测试到染色位置
    DEBUG_D2_D15_AGED_TEST                  = 0x06,    //老化
} emD2DebugDownTdf;

//D16-染色机构
typedef enum
{
    DEBUG_D2_ADD_LIQ_DOWN_STEP              = 0x01,    //调试染色转盘清洗/加样真下降到染色池位置；
    DEBUG_D2_TRUN_STEP                      = 0x02,    //调试转盘电机位置；
    DEBUG_D2_ELECTROMAGNET1_TEST            = 0x03,    //电磁铁1
    DEBUG_D2_ELECTROMAGNET2_TEST            = 0x04,    //电磁铁2
    DEBUG_D2_ELECTROMAGNET3_TEST            = 0x05,    //电磁铁3
    DEBUG_D2_ELECTROMAGNET4_TEST            = 0x06,    //电磁铁4
    DEBUG_D2_D16_AGED_TEST                  = 0x07,    //老化
    DEBUG_D2_ADD_LIQ_DOWN_END_STEP          = 0x08,    //调试染色转盘清洗/加样真下降到染色池最低位置；
} emD2DebugDyeTdf;

//D17-玻片下层转移机构
typedef enum
{
    DEBUG_D2_GRIPPERHEAT2CLEAN_X_STEP       = 0x01,    //调试夹取机械手到染色池玻片X轴位置；
    DEBUG_D2_GRIPPERHEAT2CLEAN_Y_STEP       = 0x02,    //调试夹取机械手到染色池玻片Y轴位置
    DEBUG_D2_GRIPPERHEAT2CLEAN_Z_STEP       = 0x03,    //调试夹取机械手到染色池玻片Z轴位置
    DEBUG_D2_GRIPPERHEAT2HEAT_X_STEP        = 0x04,    //调试夹取机械手到第一片烘干位X轴位置；
    DEBUG_D2_GRIPPERHEAT2HEAT_Y_STEP        = 0x05,    //调试夹取机械手到第一片烘干位Y轴位置；
    DEBUG_D2_GRIPPERHEAT2HEAT_Z_STEP        = 0x06,    //调试夹取机械手到第一片烘干位Z轴位置；
    DEBUG_D2_GRIPPERHEAT2BASKET_X_STEP      = 0x07,    //调试夹取机械手到玻片篮第一片X轴位置；
    DEBUG_D2_GRIPPERHEAT2BASKET_Y_STEP      = 0x08,    //调试夹取机械手到玻片篮第一片Y轴位置；
    DEBUG_D2_GRIPPERHEAT2BASKET_Z_STEP      = 0x09,    //调试夹取机械手到玻片篮第一片Z轴位置；
    DEBUG_D2_GRIPPERHEAT_CAL_STEP           = 0x0a,    //机械手旋转90度调试补偿
    DEBUG_D2_ROBOTCLOP_OPEN_CLOSE           = 0x0b,    //调试机械手开关；
    DEBUG_D2_HEAT_POS_TEST                  = 0x0c,    //测试烘干1-10位置间隔
    DEBUG_D2_TO_CLEAN_TEST                  = 0x0d,    //测试到染色池位
    DEBUG_D2_TO_HEAT_TEST                   = 0x0e,    //测试到烘干位
    DEBUG_D2_TO_BASKET_TEST                 = 0x0f,    //测试到玻片篮位
    DEBUG_D2_D17_AGED_TEST                  = 0x10,    //老化
} emD2DebugTransferTdf;

//D19-玻片篮输送机构
typedef enum
{
    DEBUG_D2_BASKETREC2IN_X_STEP            = 0x01,    //调试放置玻片篮进口位X轴位置；
    DEBUG_D2_BASKETREC2IN_Y_STEP            = 0x02,    //调试放置玻片篮进口位Y轴位置；
    DEBUG_D2_BASKETREC2HEAT_X_STEP          = 0x03,    //调试放置玻片篮进口到接收烘干X位置；
    DEBUG_D2_BASKETREC2HEAT_Y_STEP          = 0x04,    //调试放置玻片篮进口到接收烘干Y位置；
    DEBUG_D2_BASKETREC2OUT_X_STEP           = 0x05,    //调试放置玻片篮接收烘干位到出口X轴位置；
    DEBUG_D2_BASKETREC2ONLINE_STEP          = 0x06,    //调试玻片篮推送联机推位置
    DEBUG_D2_BASKETREC_FRISTSLIDE_STEP      = 0x07,    //调试玻片篮第一个玻片光耦检测位
    DEBUG_D2_BASKETREC_ENDSLIDE_STEP        = 0x08,    //调试玻片篮第十个玻片光耦检测位
    DEBUG_D2_READBASKETNUM_TEST             = 0x09,    //读取玻片篮号码
    DEBUG_D2_ELECTROMAGNET_TEST             = 0x0a,    //测试电磁铁
    DEBUG_D2_D19_AGED_TEST                  = 0x0b,    //老化
    DEBUG_D2_D19_UP_REC                     = 0x0c,    //调试玻片篮上抬到接收玻片位置
    DEBUG_D2_BASKET_POS_TEST                = 0x0d,    //测试玻片篮1-10位置间隔
} emD2DebugBasketTdf;

//D9-推片机构
typedef enum
{
    DEBUG_D2_PUSH_SLIDE_INIT                = 0x00,    //推片机构初始化
    DEBUG_D2_PUSH_SLIDE                     = 0x01,    //推片
    DEBUG_D2_PUSH_SLIDE_ANGLE_STEP          = 0x02,    //推片角度校准步数
    DEBUG_D2_PUSH_SLIDE_Y_STEP              = 0x03,    //Y轴推片位置步数
    DEBUG_D2_PUSH_SLIDE_ANGLE_15            = 0x04,    //15°测试
    DEBUG_D2_PUSH_SLIDE_AGING               = 0x05,    //老练
} emD2DebugPushSlideTdf;


//D10-推刀清洗
typedef enum
{
    DEBUG_D2_KNIFE_CLEAN_INIT               = 0x00,    //初始化
    DEBUG_D2_KNIFE_TO_CLEAN_STEP            = 0x01,    //载波片插入清洗槽步数
    DEBUG_D2_KNIFE_CLEAN_TIME               = 0x02,    //清洗时间
    DEBUG_D2_KNIFE_TO_CLEAN                 = 0x03,    //载波片插入清洗槽
    DEBUG_D2_KNIFE_UP                       = 0x04,    //抬升波片
    DEBUG_D2_KNIFE_CLEAN_AGING              = 0x05,    //老练
    DEBUG_D2_FAN_STEP                       = 0x06,    //吹干波片位置步数
    DEBUG_D2_CLEAN                          = 0x10,    //清洗
} emD2DebugKnifeCleanTdf;

//D10-推刀清洗
typedef enum
{
    DEBUG_D2_TRANSPORT_SLIDE_INIT           = 0x00,    //初始化
    DEBUG_D2_TRANSPORT_TO_DRIP_STEP         = 0x01,    //推玻片到滴样位置步数
    DEBUG_D2_TRANSPORT_TO_PUSH_STEP         = 0x02,    //推玻片到推片位置步数
    DEBUG_D2_TRANSPORT_TO_RUFFLED_STEP      = 0x03,    //推玻片到起竖位置步数
    DEBUG_D2_TRANSPORT_SLIDE_AGING          = 0x04,    //老练
    DEBUG_D2_VERTICAL_FLAT_STEP             = 0x05,    //起诉平台放平步数
    DEBUG_D2_VERTICAL_ERECTED               = 0x06,    //起诉平台竖起
    DEBUG_D2_VERTICAL_FLAT                  = 0x07,    //起诉平台放平
} emD2DebugTransportSlideTdf;

//D12波片干燥
typedef enum
{
    DEBUG_D2_DRY_SLIDE_INIT           = 0x00,    //初始化
    DEBUG_D2_FAN1_CTR                 = 0x01,    //风扇1开关
    DEBUG_D2_FAN2_CTR                 = 0x02,    //风扇1开关
    DEBUG_D2_DRY_SLIDE_AGING          = 0x03,    //老练
} emD2DebugDrySlideTdf;


//D13-玻片上层机械手机构
typedef enum
{
    DEBUG_D2_UPPER_ROBOT_INIT            = 0x00,    //初始化
    DEBUG_D2_ROBOT_TO_RUFFLED_STEP       = 0x01,    //夹持波片到起竖位步数
    DEBUG_D2_ROBOT_TO_DRY1_STEP          = 0x02,    //夹持波片到烘干位1步数
    DEBUG_D2_ROBOT_TO_DRY2_STEP          = 0x03,    //夹持波片到拍照架 放开机械爪位置
    DEBUG_D2_ROBOT_TO_PHOTO_STEP         = 0x04,    //夹持波片到拍照夹步数
    DEBUG_D2_ROBOT_DOWN_STEP             = 0x05,    //机械爪下降步数

    DEBUG_D2_RUFFLED_TO_DRY1             = 0x06,    //从起竖位夹持波片到烘干位1
    DEBUG_D2_RUFFLED_TO_DRY2             = 0x07,    //从起竖位夹持波片到烘干位2
    DEBUG_D2_DRY1_TO_DOWN                = 0x08,    //从烘干位1夹持波片到拍照夹
    DEBUG_D2_DRY2_TO_DOWN                = 0x09,    //从烘干位2夹持波片到拍照夹

    DEBUG_D2_UPPER_ROBOT_AGING            = 0x0a,    //老练
    DEBUG_D2_UPPER_ROBOT_OPEN_CLOSE       = 0x0b,    //机械爪打开/关闭
} emD2DebugUpperRobotTdf;

//D14-拍照机构
typedef enum
{
    DEBUG_D2_PHOTO_INIT                  = 0x00,    //初始化
    DEBUG_D2_PHOTO_TO_RUFFLED_STEP       = 0x01,    //拍照夹持位置调试
    DEBUG_D2_PHOTO_STEP                  = 0x02,    //拍照位置调试（需要加视频）
    DEBUG_D2_PHOTO_TO_DOWN_STEP          = 0x03,    //上下层转移位置调试
    DEBUG_D2_PHOTO_AGING                 = 0x06,    //老练
} emD2DebugPhotoTdf;

//机构
typedef enum
{
    D2_PUSH_SLIDE                           = 0x09,    //D9-推片机构
    D2_KNIFE_CLEAN                          = 0x10,    //D10-推刀清洗
    D2_TRANSPORT_SLIDE                      = 0x11,    //D11-玻片水平转运机构
    D2_DRY_SLIDE                            = 0x12,    //D12波片干燥
    D2_UPPER_ROBOT                          = 0x13,    //D13-玻片上层机械手机构
    D2_PHOTO_SLIDE                          = 0x14,    //D14-拍照机构
    D2_MACHINE_DOWN                         = 0x15,    //D15-玻片下降机构
    D2_MACHINE_DYE                          = 0x16,    //D16-染色机构
    D2_MACHINE_TRANSFER                     = 0x17,    //D17-玻片下层转移机构
    D2_MACHINE_BASKET                       = 0x19,    //D19-玻片篮输送机构
} emD2DebugMachineTdf;

typedef enum
{
    //推刀玻片上推血膜
   DEBUG_D2_F2_PUSH_ANGLE_DEBUG             =  0x30,//:推片角度调试(校准)命令。          Push Angle Debugging
   DEBUG_D2_F2_PUSH_STEP_ADJ                =  0x31,//:推片位置步数调试命令。            Push position step number adjustment
   DEBUG_D2_F2_KNIFE_TO_CLEAN_HEIGHT        =  0x32,//:推刀清洗下降高度。                Push knife to clean the drop height
   DEBUG_D2_F2_KNIFE_TO_CLEAN               =  0x33,//:推刀清洗位置
    //推刀超声清洗
   DEBUG_D2_F2_CLEAN_POWER                  =  0x34,//:清洗功率调试命令。                Cleaning power
   DEBUG_D2_F2_BLOW_TIME                    =  0x35,//:吹干时间                          Blowing time
    //玻片传送到滴样、推片、及起竖位置，完成推片、起竖
   DEBUG_D2_F2_SLIDE_TO_TRANS               =  0x36,//:玻片传送到滴样位置步数调试命令。  The slides are transferred to the titling position
   DEBUG_D2_F2_SLIDE_TO_PUSH                =  0x37,//:玻片传送到推片位置步数调试命令。
   DEBUG_D2_F2_SLIDE_TO_UPRIGHT             =  0x38,//玻片传送到起竖位置步数调试命令。
    //夹持玻片在起竖位、烘干位、拍照位转移
   DEBUG_D2_F2_CLAMPSLIDE_AT_UPRGHT         =  0x39,//夹持玻片在起竖位步数调试命令。
   DEBUG_D2_F2_CLAMPSLIDE_AT_DRY1           =  0x3a,//夹持玻片在烘干位1调试命令
   DEBUG_D2_F2_CLAMPSLIDE_AT_DRY2           =  0x3b,//夹持玻片在烘干位2调试命令
   DEBUG_D2_F2_Y_DOWN                       =  0x3C,//Y轴下降高度
   DEBUG_D2_F2_CLAMPSLIDE_AT_PHOTO          =  0x3D,//夹持玻片在拍照位调试命令
    //完成血膜拍照，移动玻片到上下层转移位置
   DEBUG_D2_F2_SLIDE_TO_UPPER               =  0x3E,//移动玻片到上层转移位置步数调试命令。
   DEBUG_D2_F2_PHOTO_PLACE                  =  0x3F,//拍照位置
   DEBUG_D2_F2_PHOTO_BRIGHTNESS             =  0x40,//拍照亮度
   DEBUG_D2_F2_EXP_TIME                     =  0x41,//曝光时间
   DEBUG_D2_F2_WHITE_BALANCE                =  0x42,//白平衡

} emD2F2DebugCmdTdf;

/**************************MP1200 D3*****************************************/
typedef enum
{
    ERR_D3_OK                       = 0,
    ERR_D3_NO_DIULENT_TANK          = 0x01,       //储液罐内无稀释液
    ERR_D3_NO_PURIFIED_TANK         = 0x02,       //储液罐内无纯化水
    ERR_D3_NO_CH3OH_TANK            = 0x03,       //储液罐内无甲醇
    ERR_D3_WASHING_PUMP             = 0x04,       //清洗泵复位错误
    ERR_D3_SAMPLING_PUMP            = 0x05,       //采样泵复位错误
    ERR_D3_X_LEFT_MOTOR             = 0x06,       //采样针X轴电机左限位复位错误
    ERR_D3_NO_DYEA_TANK             = 0x08,       //储液罐内无染剂A
    ERR_D3_Y_UP_MOTOR               = 0x07,       //采样针Y轴电机上限位复位错误
    ERR_D3_NO_DYEB_TANK             = 0x09,       //储液罐内无染剂B
    ERR_D3_NO_BUFFER_TANK           = 0x0a,       //储液罐内无缓冲液
    ERR_D3_NO_DYEA_BUCKET           = 0x0b,       //试剂桶内无染剂A
    ERR_D3_NO_DYEB_BUCKET           = 0x0c,       //试剂桶内无染剂B
    ERR_D3_NO_BUFFER_BUCKET         = 0x0d,       //试剂桶内无缓冲液
    ERR_D3_NO_CH3OH_BUCKET          = 0x0e,       //试剂桶内无甲醇
    ERR_D3_NO_DIULENT_BUCKET        = 0x0f,       //试剂桶内无稀释液
    ERR_D3_NO_PURIFIED_BUCKET       = 0x10,       //试剂桶内无纯化水
    ERR_D3_SAMPLE_DOWN              = 0x11,       //未到达采样深度
    ERR_D3_DROP_DOWN                = 0x12,       //未到达滴样深度
    ERR_D3_WASTE_DYE_TANK1          = 0x13,       //染色位1抽废异常
    ERR_D3_WASTE_DYE_TANK2          = 0x14,       //染色位2抽废异常
    ERR_D3_WASTE_DYE_TANK3          = 0x15,       //染色位3抽废异常
    ERR_D3_WASTE_DYE_TANK4          = 0x16,       //染色位4抽废异常
    ERR_D3_WASTE_FULL               = 0x17,       //废液桶满
    ERR_D3_EMPTY_TANK_UP            = 0x18,       //废液罐满
    ERR_D3_NO_SAMPLE                = 0x19,       //采样针采样异常/没有样本
    ERR_D3_PIPE_BLOCK               = 0x1a,       //采样针管路堵塞
    ERR_D3_VACUUM_HIGH              = 0x1b,       //负压压力值过高
    ERR_D3_VACUUM_LOW               = 0x1c,       //负压压力值过低
    ERR_D3_VOLUMEL_HIGH             = 0x1d,       //正低压压力值过高
    ERR_D3_VOLUMEL_LOW              = 0x1e,       //正低压压力值过低
    ERR_D3_VOLUMEH_HIGH             = 0x1f,       //正高压压力值过高
    ERR_D3_VOLUMEH_LOW              = 0x20,       //正高压压力值过低
    ERR_D3_VISCISITY_LOW            = 0x21,       //采样样本粘度过低
    ERR_D3_VISCISITY_HIGH           = 0x22,       //采样样本粘度过高
    ERR_D3_CALIBRATION_LOW          = 0x23,       //低粘度校准异常
    ERR_D3_CALIBRATION_NORMAL       = 0x24,       //常规粘度校准异常
    ERR_D3_TEM_DIULENT_LOW          = 0x25,       //稀释液温度过高
    ERR_D3_TEM_DIULENT_HIGH         = 0x26,       //稀释液温度过低
    ERR_D3_NO_PIN_SAMPLE            = 0x27,       //采样针不在采样位
    ERR_D3_NO_PIN_WASH              = 0x28,       //采样针不在清洗位
    ERR_D3_NO_PIN_DROP              = 0x29,       //采样针不在滴样位
    ERR_D3_D_LEFT_MOTOR             = 0x2a,       //烘干位电机左复位故障
    ERR_D3_EMPTY_TANK_DOWN          = 0x2b,       //下层废液罐满
} emD3ErrTdf;

typedef enum
{
    CMD_D3_00_IDLE                     = 0,
    CMD_D3_14_RESET                    = 0x01,      //复位命令
    CMD_D3_03_WASH_NEEDLE              = 0x02,      //滴血针维护
    CMD_D3_00_PRESSURE_REFRESH         = 0x03,      //读取压力值
    CMD_D3_03_SAMPLING                 = 0x04,      //采样，用于控制采样针完成样本采样
    CMD_D3_03_DROP_SAMPLE              = 0x05,      //滴样命令，用于滴加采集的样本
    CMD_D3_03_WASH_NEEDL_SURFACE       = 0x06,      //采样后清洗，用于采样后清洗针外壁
    CMD_D3_03_WASH_NEEDL_WALL          = 0x07,      //滴样后清洗，用于滴样后清洗采样针内外壁
    CMD_D3_14_RESET_TO_WAIT            = 0x08,      //普通复位，用于采样针复位到等待位
    CMD_D3_07_DROP_METHANOL            = 0x09,      //滴加甲醇，用于单独控制向回收罐内加注甲醇，染色位1
    CMD_D3_06_DROP_STAINA              = 0x0a,      //滴加染剂A，用于向染色池内滴加染剂A，染色位2
    CMD_D3_06_EMPTY_DYE2               = 0x37,
    CMD_D3_14_VISCISITY_CALIBRATION    = 0x0b,      //采样黏度校准
    CMD_D3_00_GETOPC                   = 0x0c,      //读取光耦信号
    CMD_D3_14_DEBUG                    = 0x0d,      //調試
    CMD_D3_14_VACUUM_CALIBRATION       = 0x0e,      //-40Kpa气压校准
    CMD_D3_14_VOLUMEL_CALIBRATION      = 0x0f,      //70Kpa气压校准
    CMD_D3_14_VOLUMEH_CALIBRATION      = 0x10,      //250Kpa气压校准
    CMD_D3_06_DRAIN_TANK               = 0x11,      //排空回收罐，用于排空回收罐内液体
    CMD_D3_00_SETMODLE                 = 0x12,      //设置命令
    CMD_D3_05_DROP_STAINB              = 0x13,      //滴加染剂B，用于向染色池内滴加染剂B
    CMD_D3_05_EMPTY_DYE3               = 0x38,
    CMD_D3_00_D7_EXPERIENCED           = 0x14,      //D7采滴血机构老练
    CMD_D3_03_MAINTAIN_FLUID           = 0x15,      //保养液路，抽取甲醇保养管路1
    CMD_D3_00_D20_EXPERIENCED          = 0x16,      //D20液路机构老练
    CMD_D3_04_PURIFIED_WASH            = 0x17,      //滴加纯化水
    CMD_D3_14_DILUENT_TEM              = 0x18,      //稀释液温度校准
    CMD_D3_08_PUSH_WASH                = 0x19,      //推刀玻片清洗
    CMD_D3_01_PUSH_DRYING              = 0x1a,      //推刀玻片吹干
    CMD_D3_09_EMPTY_WASTE_UP           = 0x1b,      //上层废液
    CMD_D3_14_FULL_PIPE                = 0x1c,      //灌充管路
    CMD_D3_14_EMPTY_PIPE               = 0x1d,      //排空管路
    CMD_D3_08_TANK_WASTE               = 0x1e,      //清洗槽抽废
    CMD_D3_07_CH3OH_WASTE              = 0x1f,      //染色位1抽废
    CMD_D3_02_FULL_DILUENT             = 0x20,      //灌充稀释液
    CMD_D3_02_FULL_PURIFIED            = 0x21,      //灌充纯化水
    CMD_D3_02_FULL_DYEA                = 0x22,      //灌充染剂A
    CMD_D3_02_FULL_CH3OH               = 0x23,      //灌充甲醇
    CMD_D3_02_FULL_BUFFER              = 0x24,      //灌充缓冲液
    CMD_D3_02_FULL_DYEB                = 0x25,      //灌充染剂B
    CMD_D3_14_EMPTY_DILUENT            = 0x26,      //排空稀释液
    CMD_D3_14_EMPTY_PURIFIED           = 0x27,      //排空纯化水
    CMD_D3_14_EMPTY_DYEA               = 0x28,      //排空染剂A
    CMD_D3_14_EMPTY_CH3OH              = 0x29,      //排空甲醇
    CMD_D3_14_EMPTY_BUFFER             = 0x2a,      //排空缓冲液
    CMD_D3_14_EMPTY_DYEB               = 0x2b,      //排空染剂B
    CMD_D3_02_FULL_TANK                = 0x2c,      //灌充储液罐
    CMD_D3_00_OPEN_AIR                 = 0x2d,      //开气源
    CMD_D3_00_CLOSE_AIR                = 0x2e,      //关气源
    CMD_D3_11_SLIDE_DYE                = 0x2f,      //打印前玻片吹干
    CMD_D3_00_EXP_PUMP                 = 0x30,      //气动定量泵老练
    CMD_D3_14_AIR_LINE_GET             = 0x31,      //气压基准值获取
    CMD_D3_00_DYENEXT_CLEAN            = 0x32,      //染剂A管路保养之甲醇灌注
    CMD_D3_00_DYEATANK_CLEAN           = 0x33,      //染剂A管路保养之染剂排空
    CMD_D3_00_VISCISITY_DROPSAMPLE     = 0x34,      //采样针滴液量校准
    CMD_D3_03_PREDROP                  = 0x35,      //滴样后预排
    CMD_D3_00_ERRREV                   = 0x36,      //错误修复
    CMD_D3_00_DYEEND_CLEAN             = 0x39,      //染剂A管路保养之染剂灌充
    CMD_D3_12_DYE_RESET                = 0x3a,      //烘干机构电机复位
    CMD_D3_12_DYE_TO_SET               = 0x3b,      //移动到指定位置
    CMD_D3_13_EMPTY_WASTE_DOWN         = 0x3c,      //下层排废
    CMD_D3_0_ERR_NOTICE                = 0x3d,      //错误接收
    CMD_D3_13_TURN_WASTE               = 0x3e,      //下层转盘抽废
    CMD_D3_12_SHAKE_WASTE              = 0x3f,      //烘干机构小车抖动

    CMD_D3_00_REFRESH                  = 0xea,      //刷新参数。
    CMD_D3_00_LOAD                     = 0xeb,      //加载参数。
    CMD_D3_00_SAVE                     = 0xec,      //保存参数  ， 将调试好的参数进行保存。
    CMD_D3_00_GETVER                   = 0xed,      //获取版本号命令
    CMD_D3_00_UPDATA_START             = 0xee,      //开始升级
} emD3CmdTdf;

//D7采滴血机构调试
typedef enum
{
    DEBUG_D3_PIN_TO_WASH_PLACE              = 0x01,    //采样针清洗位
    DEBUG_D3_PIN_TO_WASH_DOWN               = 0x02,    //采样针清洗深度
    DEBUG_D3_PIN_TO_SAMPLE_PLACE            = 0x03,    //采样针采样位
    DEBUG_D3_PIN_TO_SAMPLE_DOWN             = 0x04,    //采样针采样深度
    DEBUG_D3_PIN_TO_DROP_PLACE              = 0x05,    //采样针滴样位
    DEBUG_D3_PIN_TO_DROP_DOWN               = 0x06,    //采样针滴样深度
    DEBUG_D3_ONE_SAMPLE_TEXT                = 0x07,    //单个玻片采滴样测试
    DEBUG_D3_07_AGEING                      = 0x08,    //机构老化
    DEBUG_D3_PIN_TO_PERIPHERALSAMPLE_DOWN   = 0x09,    //末梢血采样深度
    DEBUG_D3_PIN_TO_TRACESAMPLE_DOWN        = 0x0a,    //微量血采样深度
} emD3DebugPinTdf;

//D7采滴血机构调试
typedef enum
{
    DEBUG_D3_DRY_TO_SET                     = 0x02,    //烘干电机运动到设定位置
    DEBUG_D3_DRY_TO_START                   = 0x01,    //烘干电机运动到起始位置
    DEBUG_D3_12_AGEING                      = 0x03,    //烘干位机构老化
} emD3DebugDryTdf;

//D20液路机构调试
typedef enum
{
    DEBUG_D3_DROP_VALUE                     = 0x01,    //滴液量
    DEBUG_D3_V11                            = 0x02,    //电磁阀V1~V38
    DEBUG_D3_V12                            = 0x03,
    DEBUG_D3_V13                            = 0x04,
    DEBUG_D3_V14                            = 0x05,
    DEBUG_D3_V15                            = 0x06,
    DEBUG_D3_V16                            = 0x07,
    DEBUG_D3_V17                            = 0x08,
    DEBUG_D3_V18                            = 0x09,
    DEBUG_D3_V19                            = 0x0a,
    DEBUG_D3_V20                            = 0x0b,
    DEBUG_D3_V21                            = 0x0c,
    DEBUG_D3_V22                            = 0x0d,
    DEBUG_D3_V23                            = 0x0e,
    DEBUG_D3_V24                            = 0x0f,
    DEBUG_D3_V25                            = 0x10,
    DEBUG_D3_V26                            = 0x11,
    DEBUG_D3_V27                            = 0x12,
    DEBUG_D3_V28                            = 0x13,
    DEBUG_D3_V29                            = 0x14,
    DEBUG_D3_V30                            = 0x15,
    DEBUG_D3_V31                            = 0x16,
    DEBUG_D3_V32                            = 0x17,
    DEBUG_D3_V33                            = 0x18,
    DEBUG_D3_V34                            = 0x19,
    DEBUG_D3_V35                            = 0x1a,
    DEBUG_D3_V36                            = 0x1b,
    DEBUG_D3_V37                            = 0x1c,
    DEBUG_D3_V38                            = 0x1d,
    DEBUG_D3_V39                            = 0x1e,
    DEBUG_D3_V40                            = 0x1f,
    DEBUG_D3_V41                            = 0x20,
    DEBUG_D3_V42                            = 0x21,
    DEBUG_D3_V43                            = 0x22,
    DEBUG_D3_V44                            = 0x23,
    DEBUG_D3_V45                            = 0x24,
    DEBUG_D3_V46                            = 0x25,
    DEBUG_D3_V47                            = 0x26,
    DEBUG_D3_V48                            = 0x27,    //电磁阀V1~V38
    DEBUG_D3_OPEN_AIR                       = 0x28,    //开气源
    DEBUG_D3_CLOSE_AIR                      = 0x29,    //关气源
    DEBUG_D3_WASH_PUMP_STEPS                = 0x2a,    //清洗泵下拉步数
    DEBUG_D3_SAMPLE_PUMP_STEPS              = 0x2b,    //滴样泵下拉步数
    DEBUG_D3_EMPTY_DILUENT_TANK             = 0x2c,    //排空稀释液罐
    DEBUG_D3_EMPTY_PURIFIED_TANK            = 0x2d,    //排空纯化水罐
    DEBUG_D3_EMPTY_DYEA_TANK                = 0x2e,    //排空染剂A罐
    DEBUG_D3_EMPTY_DYEB_TANK                = 0x2f,    //排空染剂B罐
    DEBUG_D3_EMPTY_BUFFER_TANK              = 0x30,    //排空缓冲液罐
    DEBUG_D3_EMPTY_CH3OH_TANK               = 0x31,    //排空甲醇罐
    DEBUG_D3_EMPTY_WASTE_TANK_UP            = 0x32,    //排空上层废液罐
    DEBUG_D3_FULL_DILUENT_TANK              = 0x33,    //灌充稀释液罐
    DEBUG_D3_FULL_PURIFIED_TANK             = 0x34,    //灌充纯化水罐
    DEBUG_D3_FULL_DYEA_TANK                 = 0x35,    //灌充染剂A罐
    DEBUG_D3_FULL_DYEB_TANK                 = 0x36,    //灌充染剂B罐
    DEBUG_D3_FULL_BUFFER_TANK               = 0x37,    //灌充缓冲液罐
    DEBUG_D3_FULL_CH3OH_TANK                = 0x38,    //灌充甲醇罐
    DEBUG_D3_EMPTY_PIPE                     = 0x39,    //排空管路
    DEBUG_D3_FULL_PIPE                      = 0x3a,    //灌充管路
    DEBUG_D3_CH3OH_PUMP                     = 0x3b,    //甲醇定量泵
    DEBUG_D3_DYEA_PUMP_6                    = 0x3c,    //染剂A定量泵6ml
    DEBUG_D3_DYEA_PUMP_0_3                  = 0x3d,    //染剂A定量泵0.3ml
    DEBUG_D3_DYEB_PUMP                      = 0x3e,    //染剂B定量泵
    DEBUG_D3_PURIFIED_PUMP                  = 0x3f,    //纯化水定量泵
    DEBUG_D3_DYE_PLACE_1                    = 0x40,    //染色位1滴加
    DEBUG_D3_DYE_PLACE_2                    = 0x41,    //染色位2滴加
    DEBUG_D3_DYE_PLACE_3                    = 0x42,    //染色位3滴加
    DEBUG_D3_DYE_PLACE_4                    = 0x43,    //染色位4滴加
    DEBUG_D3_BUFFER_PUMP_5_4                = 0x44,    //缓冲液定量泵5.4ml
    DEBUG_D3_20_AGEING                      = 0x45,    //机构老化
    DEBUG_D3_DYEING_POOL                    = 0x46,    //染色转盘抽废
    DEBUG_D3_EMPTY_WASTE_TANK_DOWN          = 0x47,    //排空下层废液罐
    DEBUG_D3_V49                            = 0x48,
    DEBUG_D3_V50                            = 0x49,
} emD3DebugWaterTdf;

/**************************MP1200 D5*****************************************/
typedef enum
{
    ERR_D5_OK                       = 0x00,
    ERR_D5_RMOTORERR                = 0x01,// 右平台拨杆驱动电机复位故障         M3
    ERR_D5_LMOTORERR                = 0x02,// 左平台拨杆驱动电机复位故障         M6
    ERR_D5_MMOTORERR                = 0x03,// 找试管拨杆驱动电机复位故障         M4
    ERR_D5_RACKFULLERR              = 0x04,// 左平台试管架满
    ERR_D5_LEFTMOTORERR             = 0x05,// 未用
    ERR_D5_NORACKERR                = 0x06,// 右平台上没有可测试的试管架
    ERR_D5_NOTUBEERR                = 0x07,// 未用，试管架上没有试管
    ERR_D5_READRACKCODENMBERR       = 0x08,// 读试管架号错信误
    ERR_D5_ONLINEMOTOERR            = 0x09,// 未用
    //联机新增故障
    ERR_D5_RINBELTMOTOERR           = 0x0a,// 右内侧皮带电机故障                 M1
    ERR_D5_INTERCEPTMOTOERR         = 0x0b,// 拦截电机故障                       M2
    ERR_D5_LEFTFINDMOTORERR         = 0x0c,// 左侧试管架复测电机故障             M5
    ERR_D5_LEFTRACKOUTMOTOERR       = 0x0d,// 左侧平台推试管架至皮带电机故障      M7
    ERR_D5_LIFTINGMOTOERR           = 0x0e,// 左侧平台升降电机故障               M8
    ERR_D5_LINBELTMOTOERR           = 0x0f,// 左内侧皮带电机故障                 M9
    ERR_D5_OUTBELTMOTOERR           = 0x10,// 外侧皮带电机故障                   M10
} emD5ErrTdf;

typedef enum
{
    CMD_D5_IDLE                     = 0,
    CMD_D5_RESET                    = 0x01,     //复位命令
    CMD_D5_INTERCEPT                = 0x02,     //拦截试管命令，联机时需要执行此命令，以防试管从皮带直接传递过去
    CMD_D5_TUBETODETECT             = 0x03,     //查找下一个试管命令，用于把试管拨到夹试管位上等动作，此命令每发一次就拨动一格
    CMD_D5_RETEST                   = 0x04,     //复测命令，预留命令
    CMD_D5_GETOPC                   = 0x05,     //读取D5机构各光耦状态命令
    CMD_D5_DEBUG                    = 0x06,     //调试
    //联机新增命令
    CMD_D5_LEFTCLEAR                = 0x07,     //左侧平台拨杆清空
    CMD_D5_RACKBACK                 = 0x08,     //试管架至外侧皮带位
    CMD_D5_RACKFORWARD              = 0x09,     //试管架至内侧皮带位
    CMD_D5_NEXTBRIGEMOV             = 0x0a,     //试管架出桥至下一台
    CMD_D5_BACKMOV                  = 0x0b,     //试管架返回收容
    CMD_D5_SETMODE                  = 0x0c,     //模式设置
    CDM_D5_CANCLE_INTERCEPT         = 0x0e,     //拦截电机取消拦截命令 禁用
    CDM_D5_CLEARRACK                = 0x0f,     //试管架清空命令
    CMD_D5_INTERCEPTINRIGHT         = 0x10,     //拦截电机进右侧平台命令
    CMD_D5_INTERCEPTRIGHT_CLEAR     = 0x11,     //拦截电机进桥位清空命令
    CMD_D5_MOTORSINGLERST           = 0x12,     //单个电机复位命令
    CMD_D5_RETESTMODESET            = 0x13,     //复测设置命令
    CMD_D5_CTREMCLED                = 0x14,     //急诊LED命令
    CMD_D5_CTRERRLED                 = 0xe9,     //控制故障LED灯
    CMD_D5_REFRESH                  = 0xea,    //刷新参数
    CMD_D5_LOAD                     = 0xeb,    //加载参数
    CMD_D5_SAVE                     = 0xec,    //保存参数 将调试好的参数进行保存
    CMD_D5_GETVER                   = 0xed,    //获取版本号命令
    CMD_D5_UPDATA_START             = 0xee,    //开始升级
} emD5CmdTdf;

typedef enum  //此处的定义命令为D5调试命令的参数
{
    DEBUG_D5_NONE                                       = 0x00,                 // 无调试命令
    DEBUG_D5_RIGHT_ST_POS                               = 0x01,                 // 右拨杆起始位置
    DEBUG_D5_RIGHT_INTVL                                = 0x02,                 // 右拨杆间隔位
    DEBUG_D5_LEFT_ST_POS                                = 0x03,                 // 左拨杆起始位置
    DEBUG_D5_LEFT_INTVL                                 = 0x04,                 // 左拨杆间隔位
    DEBUG_D5_TUBE_1_POS                                 = 0x05,                 // 第1个试管位置调试
    DEBUG_D5_TUBE_10_POS                                = 0x06,                 // 第10个试管位置调试
    DEBUG_D5_START_FIND_ELIMINATE_GAPS                  = 0x07,                 //找试管电机起始位消隙步数

 //   DEBUG_D5_RETEST                                     = 0x10,                 // 复测功能
    DEBUG_D5_FINGDTUBE_TEST                             = 0x11,                 // 找试管测试
    DEBUG_D5_READRACK_NUM                               = 0x12,                 // 读试管架号测试
    DEBUG_D5_ELTMTEST                                   = 0x13,                 // 电磁铁开关
    DEBUG_D5_D5MODULEAGING                              = 0x14,                 // 老化测试

    //联机新增调试命令
    DEBUG_D5_RIGHT_INTERCEPT_POS                        = 0x21,                 // 拦截电机拦截位
    DEBUG_D5_RIGHT_INTERCEPT_INTOPLAT_POS               = 0x22,                 // 拦截电机推试管架位置
    DEBUG_D5_RIGHT_FINGTUBE_INTVL                       = 0x23,                 // 右侧找试管间隔步数
    DEBUG_D5_RACKMOV_LEFT_INTVL                         = 0x24,                 // 右侧试管架移至左侧剩余步数
    DEBUG_D5_LEFT_BACKRACK_INTVL                        = 0x25,                 // 左侧找试管复测位步数
    DEBUG_D5_LEFT_ONLINE_INTVL                          = 0x26,                 // 左侧联机出桥位步数
    DEBUG_D5_OUTSIDE_NEXTTEST                           = 0x27,                 // 出桥位至下一台步数
    DEBUG_D5_OUTSIDE_BACK                               = 0x28,                 // 出桥位至回传皮带步数
    DEBUG_D5_MOTORLIFTING_INTVL                         = 0x29,                 // 升降电机步数
    DEBUG_D5_FINDTUBELAST_INTVL                         = 0x2a,                 // 试管架至左侧剩余步数
    DEBUG_D5_READ_LEFT_RACK                             = 0x2b,                 // 读左侧试管架号
    DEBUG_D5_READ_RACK_OPEN                             = 0X2c,
} emD5DebugCmdTdf;


/******************************************************************************
参数描述：CMD+模块名称+子模块+线程号+命令描述
     例   CMD+  D2    +  F1  + 0    + RESET
******************************************************************************/
typedef enum
{
    ERR_D6_OK                       = 0x00,     //：无错误
    ERR_D6_READBASKENNOM            = 0x01,     //：读取玻片篮故障
    ERR_D6_TEMP                     = 0x02,     //：加热异常
    ERR_D6_STATION1_PASS            = 0x03,     //：工位1放行异常
    ERR_D6_STATION1_OFF             = 0x04,     //：工位1拦截异常
    ERR_D6_STATION2_PASS            = 0x05,     //: 工位2放行异常
    ERR_D6_STATION2_OFF             = 0x06,     //: 工位2拦截异常
    ERR_D6_STATION3_PASS            = 0x07,     //: 工位3放行异常
    ERR_D6_STATION3_OFF             = 0x08,     //：工位3拦截异常
    ERR_D6_STATION4_PASS            = 0x09,     //: 工位4放行异常
    ERR_D6_STATION4_OFF             = 0x0a,     //: 工位4拦截异常
    ERR_D6_BASKET_STATION_PASS      = 0x0b,     //: 玻片篮放行异常
    ERR_D6_BASKET_STATION_OFF       = 0x0c,     //：玻片篮拦截异常
    ERR_D6_PRESSURE                 = 0x0d,     //：D6 250KPA气压不足    
} emD6ErrTdf;

/******************************************************************************
参数描述：CMD+模块名称+子模块+线程号+命令描述
     例   CMD+  D2    +  F1  + 0    + RESET
******************************************************************************/
typedef enum
{
    CMD_D6_IDLE                         = 0,
    CMD_D6_0_RESET                      = 0x01,   //复位命令
    CMD_D6_3_READBASKETNUM              = 0x02,   //读取玻片篮号码
    CMD_D6_0_LED_OPEN                   = 0x03,   //血膜拍照照明开 带参数1开，0关
    CMD_D6_0_LED_CLOSE                  = 0x04,   //关闭血膜拍照照明
    CMD_D6_1_GRIPPER1                   = 0x05,   //UP机械手1开0关
    CMD_D6_2_GRIPPER2                   = 0x06,   //UPDOWN机械手1开0关
    CMD_D6_1_ELECTROMAGNET              = 0x07,   //电磁铁控制，到2个参数，0参数传送控制位置信息 0：甲醇位上 1：甲醇位下 2：A染色位上 3：A染色位下 4：B染色位上 5：B染色位下 6：清洗位上 7：清洗位下;，1参数传送控制状态
    CMD_D6_2_FANCTRL                    = 0x08,   //上层风扇控制 带参数 0：0上层风扇J25；1下层风扇J28；2下层风扇J29  参数:1：0关 1开
    CMD_D6_3_BASKETLOCK                 = 0x09,   //控制玻片篮传送带卡扣升起/下降 带一个参数：0 下降，1 升起
    CMD_D6_0_HEATCTRL                   = 0x0a,   //控制加热 带2个参数：0,设备1，1设备2，0 关闭，1 打开
    CMD_D6_0_SETHEATTEMP                = 0x0b,   //设置加热温度 带一个参数：设置温度值
    CMD_D6_3_AIRPUMCTRL                 = 0x0c,   //控制气泵开关 带一个参数：0 关闭，1 打开
    CMD_D6_4_ULTRASONICCTRL             = 0x0d,   //控制超声波开关 带一个参数：0 关闭，1 打开
    CMD_D6_0_READONEOPC                 = 0x0e,   //返回光耦状态
    CMD_D6_3_GRIPPER3                   = 0x0f,   //UOWN机械手1开0关
    CMD_D6_4_AIRCYLINDER                = 0x10,   //下降到推片位置
    CMD_D6_4_PUSHER                     = 0x11,   //推片
    CMD_D6_0_DEBUG                      = 0x12,   //调试
    CMD_D6_4_ULTRASONICAUTOCTRL         = 0x13,   //控制超声波带时间，自动启停
    CMD_D6_1_GRIPPER_DISABLE            = 0x14,   //IO机械手失能
    CMD_D6_0_GETOPC                     = 0x15,   //返回所有光耦状态
    CMD_D6_0_READSWSENCER               = 0x16,   //返回磁性开关状态
    CMD_D6_2_ERR_NOTICE                 = 0x17,   //错误计数器通知
    CMD_D6_2_SOUNDCTRL                  = 0x18,   //喇叭控制，带控制
    CMD_D6_0_READMFRC                   = 0x19,   //连续读取射频命令 1开 0关
    CMD_D6_0_REFRESH                    = 0xea,   //刷新参数。
    CMD_D6_0_LOAD                       = 0xeb,   //加载参数。
    CMD_D6_0_SAVE                       = 0xec,   //保存参数 将调试好的参数进行保存。
    CMD_D6_0_GETVER                     = 0xed,   //获取版本号命令
    CMD_D6_0_UPDATA_START               = 0xee,   //开始升级
} emD6CmdTdf;

/**************************sl   D11******************************/
typedef enum
{
    ERR_D11_OK                       = 0x00,     //无错误
    ERR_D11_REST                     = 0x01,     //复位故障
    ERR_D11_OPEN                     = 0x02,     //打开故障
    ERR_D11_CLAMP                    = 0x03,     //夹紧故障

} emD11ErrTdf;

typedef enum _emD11CmdTdf
{
    CMD_D11_IDLE                     = 0x00,
    CMD_D11_RST                      = 0x01, //复位
    CMD_D11_OPEN                     = 0x02, //打开
    CMD_D11_CLAMP                    = 0x03, //关闭
    CMD_D11_RATATE                   = 0x04, //旋转
    CMD_D11_GETOPC                   = 0x05, //获取光耦
    CMD_D11_DEBUG                    = 0x06, //调试
    CMD_D11_AGETEST                  = 0x07, //老练
    CMD_D11_REFRESH                  = 0xea, //刷新参数。
    CMD_D11_LOAD                     = 0xeb, //加载参数。
    CMD_D11_SAVE                     = 0xec, //保存参数
    CMD_D11_GETVER                   = 0xed, //获取版本号命令
    CMD_D11_UPDATA_START             = 0xee, //开始升级
} emD11CmdTdf;
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

#endif
