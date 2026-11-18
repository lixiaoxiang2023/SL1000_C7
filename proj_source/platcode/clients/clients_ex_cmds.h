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
* File Name     : client_ex_cmds.h
* File Path     : platcode/clients/
* Project       : urit plat
* Author        : pengshen
* Date          : 2026.05.10
* Description   : mcu io base
*******************************************************************************/

/*******************************************************************************
* modify logs:
* 2023.02.10 pengshen create
*******************************************************************************/

#ifndef __CLIENTS_EX_CMDS_H_
#define __CLIENTS_EX_CMDS_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/**************************MP1200 D0*****************************************/
typedef enum
{
    ERR_E0_IPU_OFFLINE = 0x20,      // IPU断开连接
    ERR_E0_CAMERA_INIT_FAIL = 0x21, // 相机初始化失败
    ERR_E0_CAMERA_OFFLINE = 0x22,   // 相机断开连接

    ERR_E0_E1_OFFLINE = 0x71,   // E1控制板通信异常
    ERR_E0_E2_OFFLINE = 0x72,   // E2控制板通信异常
    ERR_E0_E3_OFFLINE = 0x73,   // E3控制板通信异常
    ERR_E0_E5_OFFLINE = 0x75,   // E5控制板通信异常
    ERR_E0_E6_OFFLINE = 0x76,   // E6控制板通信异常
    ERR_E0_E7_OFFLINE = 0x77,   // E7控制板通信异常
    ERR_E0_E9_OFFLINE = 0x79,   // E9控制板通信异常

    ERR_E0_E1_TIMEOUT = 0x81,   // E1控制板命令超时
    ERR_E0_E2_TIMEOUT = 0x82,   // E2控制板命令超时
    ERR_E0_E3_TIMEOUT = 0x83,   // E3控制板命令超时
    ERR_E0_E5_TIMEOUT = 0x85,   // E5控制板命令超时
    ERR_E0_E6_TIMEOUT = 0x86,   // E6控制板命令超时
    ERR_E0_E7_TIMEOUT = 0x87,   // E7控制板命令超时
    ERR_E0_E9_TIMEOUT = 0x89,   // E9控制板命令超时
}emE0ErrTdf;

/**************************MP1200 E1*****************************************/
typedef enum
{
//    ERR_E1_OK                       = 0,
    ERR_E1_OK = 0,//0x00
    ERR_E1_M1_REST   = 0x01   ,//液路X轴电机复位故障
    ERR_E1_M2_REST   = 0x02  ,//液路Y轴复位故障
    ERR_E1_M3_REST   = 0x03  ,//显微镜平台X复位故障
    ERR_E1_M4_REST   = 0x04  ,//显微镜平台Y复位故障
    ERR_E1_M5_REST   = 0x05  ,//显微镜平台Z复位故障
    ERR_E1_M6_REST   = 0x06  ,//注射泵复位故障
    ERR_E1_M7_REST   = 0x07  ,//D16控制电机复位故障
    ERR_E1_M8_REST   = 0x08  ,//
} emE1ErrTdf;

/*****************************************************************************
参数描述: CMD_E1_0_Q_RESET
注意事项：命令_电路板_线程号_模块_简称
******************************************************************************/
typedef enum
{
    CMD_E1_IDLE  = 0,
    CMD_E1_1_Q_RESET = 0x01,               //液路机构复位命令
    CMD_E1_1_Q_RESETCLEAN = 0x02,          //液路复位清洗灌注命令
    CMD_E1_1_Q_FILLTUBING  = 0x03,         //灌注管路
    CMD_E1_1_Q_FULLRESBOTL  = 0x04,        //液路灌充命令 带参数1灌充清洗液2灌充稀释液3灌充维护液   
    CMD_E1_1_Q_EMPTYRESBOTL = 0x05,        //排空储液管  带参数1灌充清洗液2灌充稀释液3灌充维护液  
    CMD_E1_1_Q_SAMPLE  = 0x06,             //采样:混匀+采样+电导率+清洗针外壁+充池
    CMD_E1_1_Q_CLEANCH  = 0x07,            //采样前和检测后清洗,为计数池通道：通道1-4
    
    CMD_E1_2_M_RESET = 0x10,             //镜检模块复位 返回swap[0]x swap[1]y
    CMD_E1_2_M_ATOB  = 0x11,            //
    CMD_E1_2_M_CTOD   = 0x12,           //
    CMD_E1_2_M_ChangeChannel = 0x13,    //
    CMD_E1_2_M_Focus =  0x14,      //自动到对焦位，
    CMD_E1_2_M_ToGrid =  0x15,     //移动到网格，wParm[0]：1为1通道网格 ，2为2通道网格，3为3通道网格。返回swap[0]x swap[1]y
    CMD_E1_2_PTMOVETOXY =0x16,     //移动平台，带两个参数，wParm[0]：x轴坐标, wParm[1]：y轴坐标，返回swap[0]x swap[1]y
    CMD_E1_2_SET_CH_POS = 0x17,     // 设置通道坐标，wParm[0]通道名称(1,2,3)  wParm[1] x, wParm[2] y
    CMD_E1_2_Z_RESET = 0X18,        //Z轴复位
    CMD_E1_2_Z_MOVE =  0x19,        //发步进步数，wParm[0]方向（0上，1下），wParm[1]为步进步数。
    CMD_E1_2_M_PhotoTrack = 0x1A,   //计数池3通道拍照运动轨迹，返回swap[0]x swap[1]y
    
    CMD_E1_0_DEVICETEST = 0x2b,        //仪器电器件测试命令wParm[0] 模块，wParm[1]项目，wParm[2]参数
    CMD_E1_0_SETLOAD = 0x2c,        //加载设置参数。
    CMD_E1_0_SET = 0x2d,            //设置  
    CMD_E1_0_DEBUG = 0x2e,          //调试，命令wParm[0] 模块 0为液路模块 1为镜检模块 ，wParm[1]项目，wParm[2]参数 ，wParm[3]参数
    CMD_E1_0_GETOPC = 0x2f,         //获取光耦状态 
    CMD_E1_0_ERR_NOTICE = 0x30,     //错误接收完成通知
    CMD_E1_0_CLEARERR = 0x31,       //清除错误。
    CMD_E1_0_REFRESH  = 0xea,       //刷新参数。
    CMD_E1_0_LOAD = 0xeb,           //加载参数。
    CMD_E1_0_SAVE = 0xec,           //保存参数  ， 将调试好的参数进行保存。
    CMD_E1_0_GETVER  = 0xed,        //获取版本号命令
    CMD_E1_0_UPDATA_START = 0xee,   //开始升级
} emE1CmdTdf;

// E1子模块
typedef enum
{
    E1_FLUID = 1,   // 液路机构
    E1_MICRO = 2,   // 镜检平台
}emE1SubModule;

/******************************E2********************************************/
typedef enum
{
    CMD_E2_IDLE                     = 0,
    CMD_E2_HPLS                     = 0x01,     //高倍镜光源亮度调节,参数0 亮度范围为0——4095
    CMD_E2_LPLS                     = 0x02,     //低倍镜光源亮度调节，参数0 亮度范围为0——4095
    CMD_E2_HPLSCTR                  = 0x03,     //高倍镜光源开关控制  参数0 1为开 0为关
    CMD_E2_LPLSCTR                  = 0x04,     //低倍镜光源开关控制  参数0 1为开 0为关
    CMD_E2_READ_BRIGHTNESS          = 0x05,     //读取亮度  命令执行完成，swaplen = 2, swap[0]高倍镜 swap[1]低倍镜
    
    CMD_E2_GETVER                   = 0xed,    //获取版本号命令
    CMD_E2_UPDATA_START             = 0xee,    //开始升级
} emE2CmdTdf;
/******************************E3********************************************/
typedef enum
{
    CMD_E3_IDLE                     = 0,
    CMD_E3_ALLPOWERCTRL             = 0x01,     //所有电源控制  参数1  1为开 0为关
    CMD_E3_POWER5VCTRL              = 0x02,     //5V电源控制 参数1 1为开 0为关
    CMD_E3_POWER12VCTRL             = 0x03,     //12V电源控制 参数1 1为开 0为关
    CMD_E3_POWER24VCTRL             = 0x04,     //24V电源控制 参数1 1为开 0为关
    CMD_E3_BUZZERCTRL               = 0x05,
    CMD_E3_GETOPC                   = 0x06,     //读取触摸按键状态

    CMD_E3_GETVER                   = 0xed,    //获取版本号命令
    CMD_E3_UPDATA_START             = 0xee,    //开始升级
} emE3CmdTdf;
/**************************UD1290 E5*****************************************/
typedef enum
{
    ERR_E5_OK                       = 0x00,
    ERR_E5_RMOTORERR                = 0x01,// 右平台拨杆驱动电机复位故障         M3
    ERR_E5_LMOTORERR                = 0x02,// 左平台拨杆驱动电机复位故障         M6
    ERR_E5_MMOTORERR                = 0x03,// 找试管拨杆驱动电机复位故障         M4
    ERR_E5_RACKFULLERR              = 0x04,// 左平台试管架满
    ERR_E5_LEFTMOTORERR             = 0x05,// 未用
    ERR_E5_NORACKERR                = 0x06,// 右平台上没有可测试的试管架
    ERR_E5_NOTUBEERR                = 0x07,// 未用，试管架上没有试管
    ERR_E5_READRACKCODENMBERR       = 0x08,// 读试管架号错信误
    ERR_E5_ONLINEMOTOERR            = 0x09,// 未用
    //联机新增故障
    ERR_E5_RINBELTMOTOERR           = 0x0a,// 右内侧皮带电机故障                 M1
    ERR_E5_INTERCEPTMOTOERR         = 0x0b,// 拦截电机故障                       M2
    ERR_E5_LEFTFINDMOTORERR         = 0x0c,// 左侧试管架复测电机故障             M5
    ERR_E5_LEFTRACKOUTMOTOERR       = 0x0d,// 左侧平台推试管架至皮带电机故障      M7
    ERR_E5_LIFTINGMOTOERR           = 0x0e,// 左侧平台升降电机故障               M8
    ERR_E5_LINBELTMOTOERR           = 0x0f,// 左内侧皮带电机故障                 M9
    ERR_E5_OUTBELTMOTOERR           = 0x10,// 外侧皮带电机故障                   M10
} emE5ErrTdf;

typedef enum
{
    CMD_E5_IDLE                     = 0,
    CMD_E5_RESET                    = 0x01,     //复位命令
    CMD_E5_INTERCEPT                = 0x02,     //拦截试管命令，联机时需要执行此命令，以防试管从皮带直接传递过去
    CMD_E5_TUBETODETECT             = 0x03,     //查找下一个试管命令，用于把试管拨到夹试管位上等动作，此命令每发一次就拨动一格
    CMD_E5_MOTOR_TEST               = 0x04,     // 电机测试 参数0：电机号    需要在swap返回测试状态，0：异常 1：正常
    CMD_E5_GETOPC                   = 0x05,     //读取D5机构各光耦状态命令
    CMD_E5_DEBUG                    = 0x06,     //调试
    //联机新增命令
    CMD_E5_LEFTCLEAR                = 0x07,     //左侧平台拨杆清空
    CMD_E5_TO_OUTSIDE_BELT          = 0x08,     //试管架至外侧皮带位

    CMD_E5_BELT_ROLL                = 0x0a,     //皮带滚动

    CMD_E5_SETMODE                  = 0x0c,     //模式设置
    CDM_E5_CANCLE_INTERCEPT         = 0x0e,     //拦截电机取消拦截命令
    CDM_E5_CLEARRACK                = 0x0f,     //试管架清空命令
    CMD_E5_INTERCEPT_IN_RIGHT       = 0x10,     //拦截电机进右侧平台命令
    CMD_E5_DEBUG_READRACK           = 0x11,     // 测试试管架（swap[0]返回试管架号。调试功能）
    CMD_E5_DEBUG_READTUBE           = 0x12,     // 测试试管有无（swap[0-9]返回相应位置的试管有无状态。调试功能）

    CMD_E5_CLEARERR                 = 0xe9,     //清除故障
    CMD_E5_REFRESH                  = 0xea,    //刷新参数
    CMD_E5_LOAD                     = 0xeb,    //加载参数
    CMD_E5_SAVE                     = 0xec,    //保存参数 将调试好的参数进行保存
    CMD_E5_GETVER                   = 0xed,    //获取版本号命令
    CMD_E5_UPDATA_START             = 0xee,    //开始升级
} emE5CmdTdf;

typedef enum  //此处的定义命令为D5调试命令的参数
{
    DEBUG_E5_NONE                                       = 0x00,                 // 无调试命令
    DEBUG_E5_RIGHT_ST_POS                               = 0x01,                 // 右拨杆起始位置
    DEBUG_E5_RIGHT_INTVL                                = 0x02,                 // 右拨杆间隔位
    DEBUG_E5_LEFT_ST_POS                                = 0x03,                 // 左拨杆起始位置
    DEBUG_E5_LEFT_INTVL                                 = 0x04,                 // 左拨杆间隔位
    DEBUG_E5_TUBE_1_POS                                 = 0x05,                 // 第1个试管位置调试
    DEBUG_E5_TUBE_10_POS                                = 0x06,                 // 第10个试管位置调试
    DEBUG_E5_START_FIND_ELIMINATE_GAPS                  = 0x07,                 //找试管电机起始位消隙步数

 //   DEBUG_E5_RETEST                                     = 0x10,                 // 复测功能
    DEBUG_E5_FINGDTUBE_TEST                             = 0x11,                 // 找试管测试
    DEBUG_E5_READRACK_NUM                               = 0x12,                 // 读试管架号测试
    DEBUG_E5_ELTMTEST                                   = 0x13,                 // 电磁铁开关
    DEBUG_E5_D5MODULEAGING                              = 0x14,                 // 老化测试

    //联机新增调试命令
    DEBUG_E5_RIGHT_INTERCEPT_POS                        = 0x21,                 // 拦截电机拦截位
    DEBUG_E5_RIGHT_INTERCEPT_INTOPLAT_POS               = 0x22,                 // 拦截电机推试管架位置
    DEBUG_E5_RIGHT_FINGTUBE_INTVL                       = 0x23,                 // 右侧找试管间隔步数
    DEBUG_E5_RACKMOV_LEFT_INTVL                         = 0x24,                 // 右侧试管架移至左侧剩余步数
    DEBUG_E5_LEFT_BACKRACK_INTVL                        = 0x25,                 // 左侧找试管复测位步数
    DEBUG_E5_LEFT_ONLINE_INTVL                          = 0x26,                 // 左侧联机出桥位步数
    DEBUG_E5_OUTSIDE_NEXTTEST                           = 0x27,                 // 出桥位至下一台步数
    DEBUG_E5_OUTSIDE_BACK                               = 0x28,                 // 出桥位至回传皮带步数
    DEBUG_E5_MOTORLIFTING_INTVL                         = 0x29,                 // 升降电机步数
    DEBUG_E5_FINDTUBELAST_INTVL                         = 0x2a,                 // 试管架至左侧剩余步数
//    DEBUG_E5_RIGHT_INTERCEPT_TUBE_STEP                  = 0x01,    //右进样机构拦截试管位调试命令
//    DEBUG_E5_RIGHT_INTERCEPT_LEVER_STEP                 = 0x02,    //右进样机构拦截拨杆拨试管命令
//    DEBUG_E5_RIGHT_INSIDE_LEVER_START_STEP              = 0x03,    //右进样机构内拨杆起始位调试命令
//    DEBUG_E5_RIGHT_INSIDE_LEVER_INTERVAL_STEP           = 0x04,    //右进样机构内拨杆拨动间隔调试命令
//    DEBUG_E5_FIND_TUBE_INTERVAL_STEP                    = 0x05,    //选试管拨动间隔调试命令
//    DEBUG_E5_RETESTING_LEVER_INTERVAL_STEP              = 0x06,    //复测拨杆拨动间隔调试命令
//    DEBUG_E5_LEFT_INSIDE_LEVER_START_STEP               = 0x07,    //左进样机构内拨杆起始位调试命令
//    DEBUG_E5_LEFT_INSIDE_LEVER_INTERVAL_STEP            = 0x08,    //左进样机构内拨杆拨动间隔调试命令
//    DEBUG_E5_LEFT_MIDDLE_LEVER_INTERVAL_STEP            = 0x09,    //左中拨杆拨动步数调试命令
//    DEBUG_E5_LIFTING_MOTOR_STEP                         = 0x0a,    //升降电机步数调试命令
//    DEBUG_E5_STAT_STEP                                  = 0x0b,    //急诊步数调试命令
} emE5DebugCmdTdf;
///////////////////////////E6////////////////////////////////////////
typedef enum
{
    ERR_E6_OK               = 0,
    ERR_E6_Read_Ic          = 0x01,   // IC卡读卡错误
    ERR_E6_No_Ic            = 0x02,   //无IC卡
    ERR_E6_RecvICData       = 0x04,
    ERR_E6_Communication    = 0x30,
    ERR_E6_NoRespond        = 0x31,
} emE6ErrTdf;

typedef enum
{
    CMD_E6_IDLE                 = 0,
    CMD_E6_RESET                = 0x01,   //复位命令
    CMD_E6_READICCARD           = 0x14,   //
    CMD_E6_WRITEICCARD          = 0x15,   //
    CMD_E6_GETVER               = 0xed,   //获取版本号命令
    CMD_E6_UPDATA_START         = 0xee,   //开始升级
} emE6CmdTdf;
//////////////////////E7////////////////////////////////
typedef enum
{
    CMD_E7_DO_RS            	= 0x01,//完整测量 — 测量阻抗 → 电导率 → 渗透压，同时采集内外温度、判断导管状态
    CMD_E7_DO_ADJ_R         	= 0x02,//读取校准电阻 — 导通校准通道，测量 30K 参考电阻值
    CMD_E7_DO_TEMP          	= 0x03,//AD5933 内部温度 — 读取芯片内部温度传感器
    CMD_E7_DO_DS18B20       	= 0x04,//外部温度 — 读取 DS18B20 环境温度（超限时返回默认 25℃）
    CMD_E7_DO_ADJ           	= 0x05,//系统校准 — 自动校准 AD5933 增益系数（30K 标准电阻）
    CMD_E7_DO_TUBE           	= 0x0e,//导管状态检测 — 读取 AD5933 特殊状态判断管路是否为空（>60000Ω 判空）

    CMD_E7_GET_RS           	= 0x06,
//    Get_Adj_R        	        = 0x07,
//    Get_Temp         	        = 0x08,
//    Get_DS18B20          	= 0x09,
//    Get_Adj_State    	        = 0x0a,
//
//    Get_L                	= 0x0b,
//    Get_C            	        = 0x0c,
//    Get_Rads         	        = 0x0d,
    CMD_E7_REFRESH              = 0xea,      //刷新参数。
    CMD_E7_LOAD                 = 0xeb,      //加载参数。
    CMD_E7_SAVE                 = 0xec,      //保存参数  ， 将调试好的参数进行保存。
    CMD_E7_GETVER               = 0xed,      //获取版本号命令
    CMD_E7_UPDATA_START         = 0xee,      //开始升级
} emE7CmdTdf;






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
