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
*******************************************************************************/

#ifndef __CLIENTS_CX_CMDS_H_
#define __CLIENTS_CX_CMDS_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
/**************************SL1000 C0*****************************************/
typedef enum
{
    ERR_C0_OK                     = 0,
    ERR_C0_STAININGFLUIDA_IS_NONE = 0xC001, // 染色液A耗尽
    ERR_C0_STAININGFLUIDB_IS_NONE = 0xC002, // 染色液B耗尽
    ERR_C0_CLEANOUTFLUIDA_IS_NONE = 0xC003, // 清洗液A耗尽
    ERR_C0_CLEANOUTFLUIDB_IS_NONE = 0xC004, // 清洗液B耗尽
    ERR_C0_DILUENT_IS_NONE        = 0xC005, // 稀释液耗尽
    ERR_C0_CEDAROIL_IS_NONE       = 0xC006, // 香柏油耗尽
    ERR_C0_SEND_MSG               = 0xC007, // 发送消息失败
    ERR_C0_RSP_OVERTIME           = 0xC008, // 响应超时
    ERR_C0_IPU_IS_NONE            = 0xC009, // IPU连接失败
    ERR_C0_CAMERA_IS_NONE         = 0xC00a, //摄像机连接失败
    ERR_C0_CIS_IS_NONE            = 0xC00b, // CIS连接失败
    ERR_C0_NO_DISK_SPACE          = 0xC00c, // 磁盘空间不足
    ERR_C0_RESULT_TIMEOUT         = 0xC00d, // 结果超时
    ERR_C0_FOUCS_FAIL             = 0xC00e, // 对焦失败
    ERR_C0_DropNoStripErr         = 0xC00f, // 滴样位无BV卡故障
    ERR_C0_CISLedValErr           = 0xC012, // CIS校准光亮值异常
    ERR_C0_CISCalValErr           = 0xC013, // CIS白平衡校准异常
    ERR_C0_C7RestErr              = 0xC014, // C7复位超时
    ERR_C0_FoucsErr               = 0xC015, //C6开机第一张对焦异常
    ERR_C0_CommunicationErr       = 0xC030, // C0 模块通信错误
    ERR_C0_NoRespondErr           = 0xC031, // C0 模块没有应答
    ERR_C0_C1CanLinkErr           = 0xC016, // C1 CAN连接异常
    ERR_C0_C2CanLinkErr           = 0xC017, // C2 CAN连接异常
    ERR_C0_C3CanLinkErr           = 0xC018, // C3 CAN连接异常
    ERR_C0_A5CanLinkErr           = 0xC019, // C5 CAN连接异常
    ERR_C0_C6CanLinkErr           = 0xC01a, // C6 CAN连接异常
    ERR_C0_C7CanLinkErr           = 0xC01b, // C7 CAN连接异常
    ERR_C0_C8CanLinkErr           = 0xC01c, // C8 CAN连接异常
    ERR_C0_A9CanLinkErr           = 0xC01d, // A9 CAN连接异常
    ERR_C0_CBCanLinkErr           = 0xC01e, // CB CAN连接异常
    ERR_C0_CAMERA_Cal_Err         = 0xC01f, //摄像机顏色校准失败
    ERR_C0_QCLOT_NULL_Err         = 0xC020, //质控样本无对应批号的靶值
    ERR_C0_DETECTMODE_MATCH_Err   = 0xC021, //样本检测模式与仪器当前检测模式不匹配
    ERR_C0_IMG_DELETED            = 0xC022, //样本图片已删除
    ERR_C0_QCSTRIP_MATCH_Err      = 0xC023, //当前试纸类型不支持此质控样本型号
    ERR_C0_NEEDLEBLOCKAGE         = 0xC024, //采样针堵塞(连续三次采样均堵塞才报此故障）
    ERR_C0_GRAPHICSCARD_ERR       = 0xC025, //仪器显卡异常（可能过热）
    ERR_C0_GPU_ONLINE_ERR         = 0xC026, //仪器显卡连接异常
    ERR_C0_DYEA_WILL_NONE         = 0xC027, // 染色液A即将耗尽
    ERR_C0_DYEB_WILL_NONE         = 0xC028, // 染色液B即将耗尽
    ERR_C0_CLEANA_WILL_NONE       = 0xC029, // 清洗液A即将耗尽
    ERR_C0_CLEANB_WILL_NONE       = 0xC02a, // 清洗液B即将耗尽
    ERR_C0_DILUENT_WILL_NONE      = 0xC02b, // 稀释液即将耗尽
    ERR_C0_CONCENTRATION_LOW      = 0xC02c, // 样本浓度过低
    ERR_C0_CONCENTRATION_HIGH     = 0xC02d, // 样本浓度过高
    ERR_C0_NOTubeErr              = 0xC02e, // 夹不到试管
    ERR_C0_TubeDropErr            = 0xC02f, // 试管掉落故障
    ERR_C0_OpenClapErr            = 0xC030, //打开试管夹异常
    ERR_C0_DYEC_WILL_NONE         = 0xC031, // 染色液C即将耗尽
    ERR_C0_DYED_WILL_NONE         = 0xC032, // 染色液D即将耗尽
    ERR_C0_DYEE_WILL_NONE         = 0xC033, // 染色液E即将耗尽
    ERR_C0_STAININGFLUIDC_IS_NONE = 0xC034, // 染色液C耗尽
    ERR_C0_STAININGFLUIDD_IS_NONE = 0xC035, // 染色液D耗尽
    ERR_C0_STAININGFLUIDE_IS_NONE = 0xC036, // 染色液E耗尽

} emC0ErrTdf;

typedef enum
{
    CMD_C0_IDLE                 = 0,
    CMD_C0_RESET                = 0xC001,   //整机复位
    CMD_C0_START_DRY            = 0xC002,   //干化学检测
    CMD_C0_STOP                 = 0xC003,   //停止检测
    CMD_C0_CAPTURE              = 0xC004,   //一键拍照
    CMD_C0_START_TANGIBLE       = 0xC005,   //有形检测
    CMD_C0_FOCUS                = 0xC006,   //自动对焦
    CMD_C0_NORMALDECT           = 0xC007,   //正常检测(有形加干化学)
    CMD_C0_STOPNOW              = 0xC008,   //立即停止检测
    CMD_C0_READTEMP             = 0xC009,   //读取温度
    CMD_C0_ERRPROCES            = 0xC00a,   //错误处理
    CMD_C0_SENDTUBEMESSAGE      = 0xC00b,   //推送干化学样本信息
    CMD_C0_CAPTURE_END          = 0xC00c,   //拍照完成
    CMD_C0_GETMACVER            = 0xC00d,   //获取下位机版本号
    CMD_C0_GETCISCALSTA         = 0xC00e,   //获取CIS校准状态
    CMD_C0_GETEMCSTA            = 0xC00f,   //获取待机急诊状态
    CMD_C0_SENDERR              = 0xC010,   //获取待机急诊状态
    CMD_C0_READCARD             = 0XC011,   //获取试剂卡读取状态
    CMD_C0_GETANCSTA            = 0XC012,   //获取CAN状态
    CMD_C0_CAMERACAL            = 0XC013,   //相机校准流程
    CMD_C0_STLINETEST           = 0XC014,   //标准条测试流程
    CMD_C0_SL100TEST            = 0XC015,   //SL100测试流程
    CMD_C0_WAKEUP               = 0XC016,   //仪器唤醒
    CMD_C0_SLIDECLEAR           = 0XC017,   //清空制片系统残留载玻片
    CMD_C0_RESETBEFOREDETECT    = 0XC018,   //检测前复位
    CMD_C0_FOUNDNEWTUBE         = 0XC019,   //找到新样本
} emC0CmdTdf;

/**************************SL1000 C1*****************************************/
typedef enum
{
    ERR_C1_OK                = 0,
    ERR_C1_XMotor            = 0x01,     //  X轴复位故障
    ERR_C1_YMotor            = 0x02,     // Y轴复位故障
    ERR_C1_YHightRest        = 0x03,     // 急诊Y轴电机复位故障
    ERR_C1_RCmd              = 0x04,     // 发命令顺序错误
    ERR_C1_NOTube            = 0x05,     // 夹不到试管
    ERR_C1_TubeDrop          = 0x06,     // 试管掉落故障
    ERR_C1_CleanBDoor        = 0x07,
    ERR_C1_NOCleanB          = 0x08,
    ERR_C1_OpenClap          = 0x09,     //打开试管夹异常
    ERR_C1_Communication     = 0x30,     // C0 模块通信错误
    ERR_C1_NoRespond         = 0x31,     // C0 模块没有应答
} emC1ErrTdf;

typedef enum
{
    CMD_C1_IDLE                 = 0,
    CMD_C1_RESET                = 0x01,      //复位命令
    CMD_C1_TURBE_TO_ELUTIONT    = 0x02,      //：夹试管到洗脱位。
    CMD_C1_ELUTIONT             = 0x03,      //：洗脱
    CMD_C1_ELUTIONT_TO_TRACK    = 0x04,      //：洗脱位放回试管架
    CMD_C1_DEBUG                = 0X05,      //调试
    CMD_C1_GETOPC               = 0X06,      //读取光耦状态
    CMD_C1_UPDATA_PREPARE       = 0x07,      //未用
    CMD_C1_FULL_ACTIONS         = 0x08,      //
    CMD_C1_FASHMODE             = 0xa0,      //快速模式（0:SL1000运行模式; 1:SL100运行模式）
    CMD_C1_REFRESH              = 0xea,      //刷新参数。
    CMD_C1_LOAD                 = 0xeb,      //加载参数。
    CMD_C1_SAVE                 = 0xec,      //保存参数  ， 将调试好的参数进行保存。
    CMD_C1_GETVER               = 0xed,      //获取版本号命令
    CMD_C1_UPDATA_START         = 0xee,      //开始升级
} emC1CmdTdf;

/**************************SL1000 C2*****************************************/
typedef enum
{
    ERR_C2_OK                   = 0,
    ERR_C2_M1_Op1Rest        = 0x01, // 拨杆电机光耦1复位故障
    ERR_C2_M1_Op2Rest        = 0x02, // 拨杆电机光耦2复位故障
    ERR_C2_M2_OpRest         = 0x03,  // 拨一格光耦复位故障
    ERR_C2_StripType         = 0x04,  // 试纸类型错误
    ERR_C2_TempSensor        = 0x05, // 温度传感器异常
    ERR_C2_CisCal            = 0x06,     // 校准检测步数失败
    ERR_C2_Box               = 0x07,        // BV盒被取出
    ERR_C2_NoBvCard          = 0x08,      // 无BV卡
    ERR_C2_Temp              = 0x09,       // 温育温度异常
    ERR_C2_Warmming          = 0x0a,    // 加热异常
    ERR_C2_TempLow           = 0x10,    //温育温度过低
    ERR_C2_TempHight         = 0x11,  //温育温度过高
    ERR_C2_Communication     = 0x30,
    ERR_C2_NoRespond         = 0x31,
    ERR_C2_Calibrate         = 0x32, //检测步数校准失败
} emC2ErrTdf;

typedef enum
{
    CMD_C2_IDLE                 = 0,
    CMD_C2_RESET                = 0x01,       //复位命令
    CMD_C2_STRIPTODROP          = 0x02,     //试纸条至滴样位lib置（返回无参）/
    CMD_C2_MOVEONE              = 0x03,     //传送一格（返回无参）/
    CMD_C2_DETEST               = 0x04,     //检测（返回检测结果，BV检测项目反射率）/
    CMD_C2_GETPIC               = 0x05,     //获取图片（返回一张图片）/
    CMD_C2_GETOPC               = 0x06,     //读取光耦信号命令
    CMD_C2_DEBUG                = 0x07,     //调试
    CMD_C2_UPDATA_PREPARE       = 0x0b,     //升级准备
    CMD_C2_UPDATA_RECEVE        = 0x0C,     //升级数据接收
    CMD_C2_GETLIGHEVAL          = 0x0F,     //自动获取光亮值
    CMD_C2_GETPAPERCARD         = 0x10,     //获取试纸卡
    CMD_C2_PAPERDROTOHEAT       = 0x11,     //试纸卡滴样位置到加热模块位置
    CMD_C2_LINECOLOR_DETECT     = 0x12,     //线性带彩带检测
    CMD_C2_SENDBALANCEDATA      = 0x14,     //发送黑白平衡数据
    CMD_C2_TEMP_RECOVER         = 0x15,     //加热
    CMD_C2_TEMP_CLOSE           = 0x16,     //停止加热
    CMD_C2_FAN_OPEN             = 0x17,     //C2风扇开
    CMD_C2_FAN_CLOSE            = 0x18,     //C2风扇关
    CMD_C2_AGING                = 0x19,     //老化模式 带参数 0:停止老化; 1:开始老化
    CMD_C2_REFRESH              = 0xea,    //刷新参数。
    CMD_C2_LOAD                 = 0xeb,    //加载参数。
    CMD_C2_SAVE                 = 0xec,    //保存参数  ， 将调试好的参数进行保存。
    CMD_C2_GETVER               = 0xed,    //获取版本号命令
    CMD_C2_UPDATA_START         = 0xee,    //开始升级
} emC2CmdTdf;

/**************************SL1000 C3*****************************************/
typedef enum
{
    ERR_C3_OK                = 0,
    ERR_C3_YMotor            = 0x01,       // Y轴电机复位异常。
    ERR_C3_XMotor            = 0x02,       // X轴电机复位异常。
    ERR_C3_DPMotor           = 0x03,       // 染色液A泵复位异常
    ERR_C3_WPMotor           = 0x04,       // 清洗泵复位异常。
    ERR_C3_SampleMoto        = 0x05,       // 滴液泵复位异常。
    ERR_C3_T_MOTOR           = 0x06,       // 旋转电机复位错误
    ERR_C3_S_MOTOR           = 0x07,       // 细胞稀释液泵复位异常
    ERR_C3_N_MOTOR           = 0x08,       // 细胞稀释液定量泵复位异常
    ERR_C3_Db_MOTOR          = 0x09,       // 染剂B泵复位异常
    ERR_C3_NODyeA            = 0x0a,       // 无染色剂A
    ERR_C3_NODyeB            = 0x0b,       // 无染色剂B
    ERR_C3_NOPURIFIEDWATER   = 0x0c,       // 0xc30c：无纯化水
    ERR_C3_NOCLEANWATER      = 0x0d,       // 0xc30d：无清洗液
    ERR_C3_THICKSAMPLE       = 0x0e,       // 浓稠样本
    ERR_C3_WASTEWARTERFULL   = 0x0f,       // 0xc30e：废液满
    ERR_C3_WASTERFULL        = 0x10,       // 采样为空
    ERR_C3_NONACL            = 0x11,       // 无细胞稀释液
    ERR_C3_PINCLOG           = 0x12,       // 采样针采样堵塞
    ERR_C3_WASTEFULL         = 0x13,       // 废液满
    ERR_C3_WASTEPOTFULL      = 0x14,       // 废液罐满
    ERR_C3_XMOTORSTUCK       = 0x15,
    ERR_C3_YMOTORSTUCK       = 0x16,
    ERR_C3_DropVal           = 0x17,
    ERR_C3_SampleDown        = 0x18,       // 采样下降异常
    ERR_C3_PIPECLOG          = 0x19,       // 采样针管路堵塞
    ERR_C3_NODyeE            = 0x1a,       // 无染色剂E
    ERR_C3_NODyeD            = 0x1b,       // 无染色剂D
    ERR_C3_Communication     = 0x30,
    ERR_C3_NoRespond         = 0x31,
    ERR_C3_NODyeC            = 0x32,       // 無甲醇
} emC3ErrTdf;

typedef enum
{
    CMD_C3_IDLE                 = 0,
    CMD_C3_RESET                = 0x01,      //复位命令
    CMD_C3_D1WASH               = 0x02,      //清洗管路
    CMD_C3_COLLECT_SAMPLE       = 0x03,      //采集样本命令
    CMD_C3_DROP_SAMPLE          = 0x04,      //滴样命令
    CMD_C3_DROP_WASH            = 0x05,      //滴样后清洗
    CMD_C3_DROP_SET             = 0x06,      //滴液量设置
    CMD_C3_D2WASH               = 0x07,      //保养液路（开机保养/待机保养）
    CMD_C3_PINWASH              = 0x08,      //针头清洗命令
    CMD_C3_UPDATA_PREPARE       = 0x09,      //准备擦除FLASH
    CMD_C3_UPDATA_RECEVE        = 0x0a,      //数据接收
    CMD_C3_DROP_CH3OH           = 0x0b,      //滴加甲醇(染剂C)
    CMD_C3_GETOPC               = 0x0c,      //读取光耦信号
    CMD_C3_DEBUG                = 0x0d,      //調試
    CMD_C3_REFRESHDROP          = 0x11,      //刷新滴样液量参数
    CMD_C3_SETMODLE             = 0x12,      //设置命令
    CMD_C3_ADD_DILUTION         = 0x13,      //前处理加稀释液
    CMD_C3_EMPTY_WASTE_F        = 0x14,      //采样针废液排废
    CMD_C3_DROP_DYE             = 0x15,      //滴加染剂
    CMD_C3_DROP_PURIFIEDWATER   = 0x16,      //滴加纯化水
    CMD_C3_DRY_DROP             = 0x17,      //干化学滴样
    CMD_C3_Tangible_DROP        = 0x18,      //有形成分滴样
    CMD_C3_BACK_TO_DROP         = 0x19,      //回到滴样起始位
    CMD_C3_EMPTY_DYEA           = 0x1a,      //排空染剂A
    CMD_C3_EMPTY_DYEB           = 0x1b,      //排空染剂B
    CMD_C3_EMPTY_PURIFIEDWATER  = 0x1c,      //排空纯化水
    CMD_C3_EMPTY_DILU           = 0x1d,      //排空细胞稀释液
    CMD_C3_PURIFIEDWATER_MATIN  = 0x1e,      //纯化水保养管路（关机保养）
    CMD_C3_EMPTY_CLEAN          = 0x1f,      //排空清洗液
    CMD_C3_EMPITY_PIP           = 0x20,      //排空管路
    CMD_C3_FULL_DYEA            = 0x21,      //灌充染剂A
    CMD_C3_FULL_WATER           = 0x22,      //灌充纯化水
    CMD_C3_FULL_CLEANA          = 0x23,      //灌充清洗液A
    CMD_C3_FULL_DYEB            = 0x24,      //灌充染剂B
    CMD_C3_START_REST           = 0x25,      //开机复位
    CMD_C3_PURIFIEDWATER_FULL   = 0x26,      //纯化水清洗管路（关机灌注纯化水）
    CMD_C3_SECOND_SAMPLE        = 0x27,      //质控检测采样
    CMD_C3_FULL_ALL             = 0x28,      //灌充所有试剂
    CMD_C3_CHECK_WASTER         = 0x29,      //读取废液满状态
    CMD_C3_DROP_DYEA            = 0x2a,      //滴加染剂A
    CMD_C3_DROP_DYEB            = 0x2b,      //滴加染剂B
    CMD_C3_DROP_DYED            = 0x2c,      //滴加染剂D
    CMD_C3_FULL_DILU            = 0x2d,      //灌充稀释液
    CMD_C3_DRAIN_WASTE          = 0x2e,      //排废液
    CMD_C3_THICK_WASH           = 0x2f,      //采样堵针清洗
    CMD_C3_FULL_RESERVOIR       = 0x30,      //灌充储液罐
    CMD_C3_FULL_FRONT           = 0x32,      //灌充前端管路
    CMD_C3_EMPTY_CH3OH          = 0x33,      //排空甲醇
    CMD_C3_MATIN_DYEA           = 0x34,      //保养染剂A罐
    CMD_C3_MATIN_DYEB           = 0x35,      //保养染剂B罐
    CMD_C3_MATIN_SILICONE       = 0x36,      //保养硅胶套
    CMD_C3_FULL_DYED            = 0x37,      //灌充染剂D
    CMD_C3_FULL_DYEE            = 0x38,      //灌充染剂E
    CMD_C3_DROP_DYEE            = 0x39,      //滴加染剂E
    CMD_C3_EMPTY_DYEE           = 0x3a,      //排空染剂E
    CMD_C3_SLIDE_WASH           = 0x3b,      //玻片冲洗 （携带参数:冲洗时间，单位毫秒）
    CMD_C3_EMPTY_DYED           = 0x3c,      //排空染剂D
	CMD_C3_WASH_SLIDE_F         = 0x3d,      //玻片冲洗（清洗槽废液泵抽废）
    CMD_C3_FASHMODE             = 0xa0,      //快速模式（0:SL1000运行模式; 1:SL100运行模式）
    CMD_C3_REFRESH              = 0xea,      //刷新参数。
    CMD_C3_LOAD                 = 0xeb,      //加载参数。
    CMD_C3_SAVE                 = 0xec,      //保存参数  ， 将调试好的参数进行保存。
    CMD_C3_GETVER               = 0xed,      //获取版本号命令
    CMD_C3_UPDATA_START         = 0xee,      //开始升级
} emC3CmdTdf;

/**************************SL1000 C5*****************************************/
typedef enum
{
    ERR_C5_OK                   = 0,
    ERR_C5_NoErr                = 0x00,   //：无错误
    ERR_C5_RMotorErr            = 0x01,   //：右进样机构电机复位光耦故障
    ERR_C5_LMotorErr            = 0x02,   //：左进样机构电机复位光耦故障
    ERR_C5_MMotorErr            = 0x03,   //：选试管电机复位光耦故障
    ERR_C5_RackFullErr          = 0x04,   //：试管架满
    ERR_C5_LeftMotorErr         = 0x05,   //:进样平台左侧试管架拨出异常
    ERR_C5_NoRackErr            = 0x06,   //:无试管架
    ERR_C5_NoTubeErr            = 0x07,   //:当前试管架无试管
    ERR_C5_ReadTubeNmbErr       = 0x08,   //试管架号扫描超时。
    ERR_C5_OnlineMotoErr        = 0x09,   //联机电机复位异常。
    ERR_C5_FbAdjErr             = 0x0A,
    ERR_C5_CommunicationErr     = 0x30,
    ERR_C5_NoRespondErr         = 0x31,
} emC5ErrTdf;

typedef enum
{
    CMD_C5_IDLE                 = 0,
    CMD_C5_RESET                = 0x01,    //复位命令
    CMD_C5_FINDNEXT             = 0x02,    //查找下一个试管
    CMD_C5_FINDTRACK            = 0x03,    //查询是否有试管架
    CMD_C5_LEFTRESET            = 0x04,    // A5左机构复位
    CMD_C5_DEBUG                = 0x05,    //调试
    CMD_C5_READTRACK            = 0x06,    //读试管架号
    CMD_C5_READSCAN             = 0x07,    //读条形码
    CMD_C5_CRASHTUBE            = 0x08,    //混淆样本
    CMD_C5_GETOPC               = 0x09,    //读取光耦状态
    CMD_C5_READLIGHTVALUE       = 0x0b,    //读取光纤值
    CMD_C5_UPDATA_RECEVE        = 0x0c,    //就收数据命令
    CMD_C5_REFRESH              = 0xea,    //刷新参数。
    CMD_C5_LOAD                 = 0xeb,    //加载参数。
    CMD_C5_SAVE                 = 0xec,    //保存参数  ， 将调试好的参数进行保存。
    CMD_C5_GETVER               = 0xed,    //获取版本号命令
    CMD_C5_UPDATA_START         = 0xee,    //开始升级
} emC5CmdTdf;

/**************************SL1000 C6*****************************************/
typedef enum
{
    ERR_C6_OK                = 0,
    ERR_C6_XMotor            = 0x01,  //：X轴电机复位故障。
    ERR_C6_YMotor            = 0x02,  //：Y轴电机复位故障。
    ERR_C6_ZMotor            = 0x03,  //：Z轴电机复位故障。
    ERR_C6_OutRange          = 0x04,  //：XY平台电机移动到边界。
    ERR_C6_PushMotor         = 0x05,  //：载玻片移动电机复位故障。
    ERR_C6_LMotor            = 0x06,  //：L轴电机复位故障。
    ERR_C6_ZMOTORCHECK       = 0x0A,  //：Z轴平台电机校准故障。
    ERR_C6_PicTimeout        = 0x23,  //：拍照超时。
    ERR_C6_Communication     = 0x30,
    ERR_C6_NoRespond         = 0x31,
    ERR_C6_PhoNoRespond      = 0x32,
    ERR_C6_TackPictureFail   = 0x33,
} emC6ErrTdf;

typedef enum
{
    CMD_C6_IDLE                 = 0,
    CMD_C6_RESET                = 0x01, //复位,人工镜检
    CMD_C6_ZFOCUS               = 0x02, //手动对焦，,人工镜检
    CMD_C6_RESTATE              = 0x03, //状态返回
    CMD_C6_SETSTATE             = 0x04,
    CMD_C6_MOVEPT               = 0x05, //,人工镜检，移动平台
    CMD_C6_SETDOT               = 0x06, //,人工镜检，设置闪光灯值
    CMD_C6_FINDDOT              = 0x07,
    CMD_C6_MOVESLIDE            = 0x08, //,人工镜检,停止运动
    CMD_C6_TOXY                 = 0x09,
    CMD_C6_DISCARDSLIDE         = 0x0a,
    CMD_C6_AUTOFOCUS            = 0x0b,
    CMD_C6_TAKEPIC              = 0x0c,  //,人工镜检，拍照
    CMD_C6_MOVECLEARPIC         = 0x0d,  //,对焦移动到最清晰照片
    CMD_C6_DEBUG                = 0x0f,  //调试
    CMD_C6_singlepic            = 0x14,  //拍一张
    CMD_C6_UPDATA_PREPARE       = 0x15,  //升级预处理命令
    CMD_C6_UPDATA_RECEVE        = 0x16,  //就收数据命令
    CMD_C6_GETOPC               = 0x18,
    CMD_C6_ResetDelayY          = 0x19,      //复位延时Y轴
    CMD_C6_FlashLight           = 0x1a,      //闪光灯控制
    CMD_C6_LENSCLEAN            = 0x1b,      //镜头清洗
    CMD_C6_AGING                = 0x1c,      //老化模式 带参数 0:停止老化; 1:开始老化
    CMD_C6_WET_VIDEOPHOTO       = 0x1d,      //湿法录制视频加拍照
    CMD_C6_REFRESH              = 0xea,      //刷新参数。
    CMD_C6_LOAD                 = 0xeb,      //加载参数。
    CMD_C6_SAVE                 = 0xec,      //保存参数  ， 将调试好的参数进行保存。
    CMD_C6_GETVER               = 0xed,      //获取版本号命令
    CMD_C6_UPDATA_START         = 0xee,      //开始升级
} emC6CmdTdf;

/**************************SL1000 C7*****************************************/
typedef enum
{
    ERR_C7_OK                = 0,      //：无错误
    
    ERR_C7_M1OPC2            = 0x01,   //M1电机光耦2复位异常
    ERR_C7_M2MOTOR           = 0x02,   //：M2轴电机复位故障
    ERR_C7_M3MOTOR           = 0x03,   //：M3轴电机复位故障
    ERR_C7_M4MOTOR           = 0x04,   //：M4轴电机复位故障_情况1
    ERR_C7_TEMPFAN           = 0x05,   //：烘干加热故障
    ERR_C7_NOOIL             = 0x06,   //：镜油不足
    ERR_C7_ADDOIL            = 0x07,   //：添加镜油故障
    ERR_C7_TEMP              = 0x08,   //：加热异常
    ERR_C7_BOX               = 0x09,   //载玻片盒被取出
    ERR_C7_M5MOTOR           = 0x0a,   //M5滴镜油电机复位故障
    ERR_C7_M1MOTOR           = 0x0b,   //：M1轴电机复位故障
    ERR_C7_TEMPLOW           = 0x0c,   //制片温度过低
    ERR_C7_TEMPHIGH          = 0x0d,   //制片温度过高
    ERR_C7_DYEOPC            = 0x0e,   //染剂有无载波片计数
    ERR_C7_OILOPC            = 0x0f,   //镜油有无载波片计数
    ERR_C7_CARDOPC           = 0x10,   //滴样有无载波片计数
    ERR_C7_SLIDECAR          = 0x11,   //拨动载玻片卡
    ERR_C7_FANTEMPLOW        = 0x12,   //染色后烘干温度过低
    ERR_C7_FANTEMPHIGH       = 0x13,   //染色后烘干温度过高
    ERR_C7_OIL_ISABOUTTONULL = 0x14,   //镜油即将耗尽
    ERR_C7_M4MOTOR_ERR2      = 0x15,   //：M4轴电机复位故障_情况2
    ERR_C7_M4MOTOR_ERR3      = 0x16,   //：M4轴电机复位故障_情况3
    ERR_C7_M4MOTOR_ERR4      = 0x17,   //：M4轴电机复位故障_情况4
    ERR_C7_TEMP_CURING_ERR   = 0x18,   //：固化温度传感器异常
    ERR_C7_TEMP_DRYING_ERR   = 0x19,   //：烘干温度传感器异常
    ERR_C7_COMMUNICATION     = 0x30,
    ERR_C7_NORESPOND         = 0x31,
    
} emC7ErrTdf;

typedef enum
{
    CMD_C7_IDLE                 = 0,
    CMD_C7_RESET                = 0x01,    //复位命令
    CMD_C7_CARTODROP            = 0x02,    //试纸卡到滴样位置
    CMD_C7_OPEN_FAN_HEAT        = 0x03,    //打開吹風加熱
    CMD_C7_TRANONCE             = 0x04,    //传送一格。
    CMD_C7_CLOSE_FAN_HEAT       = 0x05,    //關閉吹風加熱
    CMD_C7_AIRDRY               = 0x06,    //吹干风扇PWM
    CMD_C7_M3DOWNTOWASH         = 0x07,    // M3下降到冲洗位置
    CMD_C7_GETOPC               = 0x08,    //读取光耦信号命令
    CMD_C7_DROPOIL              = 0x09,    //滴加镜油
    CMD_C7_DEBUG                = 0x0A,    // 0xC70A：调试
    CMD_C7_M3_UP_TO_WASH        = 0x0E,    //上升到染剂位置
    CMD_C7_GET_FINISH           = 0x0F,    //获取升级完成状态
    CMD_C7_MOTO_UP              = 0x11,    //上升
    CMD_C7_MOTO_DWON            = 0x12,    //下降
    CMD_C7_TRANC_TO_DMP         = 0x13,    //传送到C6
    CMD_C7_REST_TO_DMP          = 0x14,    //复位M4
    CMD_C7_RESE_TO_DROP         = 0x15,    //复位到滴样位置
    CMD_C7_TEMP_DERAT           = 0X16,    //制片温度降额
    CMD_C7_TEMP_RECOVER         = 0X17,    //制片温度恢复
    CMD_C7_FULL_OIL             = 0x18,    //充满镜油
    CMD_C7_CHECK_SLIDBOX        = 0X19,    //检查载玻片盒
    CMD_C7_COLSEFUNPWM          = 0X1a,    //关闭风扇PWM
    CMD_C7_CAR_TO_OPC           = 0x1b,    //试纸卡到滴样光藕检测位置
    CMD_C7_OPC_TO_DROP          = 0x1c,    //载波片盒到滴样位置
    CMD_C7_FULL_OIL_RE          = 0x1e,    //充满镜油,用于流程
    CMD_C7_TEMP_CLOSE           = 0X1f,    //制片温度关闭
    CMD_C7_TRANSFER_TO_C6       = 0X20,    //传送载玻片到C6
    CMD_C7_GETMPUDATA           = 0x21,    //获取陀螺仪数据
    CMD_C7_SETTEMP              = 0x22,    //设置固化温度
    CMD_C7_TARE                 = 0x23,    //去皮
    CMD_C7_STAINFAN_OPEN        = 0x24,    //染色风扇打开
    CMD_C7_STAINFAN_CLOSE       = 0x25,    //染色风扇关闭
    CMD_C7_M3DOWNTODRY          = 0x26,    //M3下降到吹干位置
    CMD_C7_SETFANTEMP           = 0x27,    //设置烘干温度
    CMD_C7_M3UPTODRY            = 0x28,    //M3上升到染色位位置
    CMD_C7_AGING                = 0x29,     //老化模式 带参数 0:停止老化; 1:开始老化
    CMD_C7_M1_STEP              = 0x2A,     //M1移动一小段
    CMD_C7_REFRESH              = 0xea,    //刷新参数。
    CMD_C7_LOAD                 = 0xeb,    //加载参数。
    CMD_C7_SAVE                 = 0xec,    //保存参数  ， 将调试好的参数进行保存。
    CMD_C7_GETVER               = 0xed,    //获取版本号命令
    CMD_C7_UPDATA_START         = 0xee,    //开始升级
} emC7CmdTdf;

typedef enum
{
    C7_D_TODROP_STEP          =  0x01,//拨杆电机拨到滴样位步数调试。
    C7_D_UP_STEP              =  0x02,//染剂上升步数调试。
    C7_D_M2_TAKE_A_STEP       =  0x03,//M2电机将玻片拨动一格步数调试
    C7_D_DROPOIL              =  0x04,//调试滴油量
    C7_D_REST_TO_TODROP_STEP  =  0x05,//M1复位到滴样位置
    C7_D_M3_DOWN_STEP         =  0x06,//M3下降到冲洗位置
    C7_D_MPU_CAL              =  0x07,//校准陀螺儀
    C7_D_FAN_DOWN_STEP        =  0x08,//吹风下降调试位置
    C7_D_SETTEMP              =  0x09,    //设置固化温度
    C7_D_SETFANTEMP           =  0x0a,    //设置烘干温度
    C7_D_M4SETPS              =  0x0b,//调试M4电机步数
    C7_M2_STEP2               =  0x0C,//滴样一小步位置
    C7_M3_UP_TO_WASH          =  0x0E,//M3向上到染剂位置
    C7_GET_UPDATE_FINISH      =  0x0F,//
} emC7DebugCmdTdf;



/**************************SL1000 C8*****************************************/
typedef enum
{
    ERR_C8_OK               = 0,
    ERR_C8_Read_Ic          = 0x01,   // IC卡读卡错误
    ERR_C8_No_Ic            = 0x02,   //无IC卡
    ERR_C8_Door_Open        = 0x03,   //前盖打开
    ERR_C8_RecvICData       = 0x04,
    ERR_C8_Communication    = 0x30,
    ERR_C8_NoRespond        = 0x31,
} emC8ErrTdf;

typedef enum
{
    CMD_C8_IDLE                 = 0,
    CMD_C8_RESET                = 0x01,   //复位命令
    CMD_C8_OPENALLPOWER         = 0x02,   //打开所有通道
    CMD_C8_CLOSEALLPOWER        = 0x03,   //关闭所有通道
    CMD_C8_OPENPOWER1           = 0x04,   //J20
    CMD_C8_CLOSEPOWER1          = 0x05,   //J20
    CMD_C8_OPENPOWER2           = 0x06,   //J19
    CMD_C8_CLOSEPOWER2          = 0x07,   //J19
    CMD_C8_OPENPOWER3           = 0x08,   //J18
    CMD_C8_CLOSEPOWER3          = 0x09,   //J18
    CMD_C8_OPENPOWER4           = 0x0a,   //J17
    CMD_C8_CLOSEPOWER4          = 0x0b,   //J17
    CMD_C8_OPENPOWER5           = 0x0c,   //J16
    CMD_C8_CLOSEPOWER5          = 0x0d,   //J16
    CMD_C8_OPENPOWER6           = 0x0e,   //J15
    CMD_C8_CLOSEPOWER6          = 0x0f,   //J15
    CMD_C8_OPENPOWER7           = 0x10,   //J14
    CMD_C8_CLOSEPOWER7          = 0x11,   //J14
    CMD_C8_OPENPOWER8           = 0x12,   //J13
    CMD_C8_CLOSEPOWER8          = 0x13,   //J13
    CMD_C8_READICCARD           = 0x14,   //
    CMD_C8_WRITEICCARD          = 0x15,   //
    CMD_C8_LED_R                = 0x16,   //红色闪烁
    CMD_C8_LED_G                = 0x17,
    CMD_C8_LED_B                = 0x18,
    CMD_C8_LED_ON_R             = 0x19,   //红色常亮
    CMD_C8_LED_ON_G             = 0x1a,
    CMD_C8_LED_ON_B             = 0x1b,
    CMD_C8_B_BREATH             = 0x1c,
    CMD_C8_CANCLPWOEROFF        = 0x1e,  //取消关机
    CMD_C8_CRTLBEEP             = 0x1f,  //蜂鸣器控制
    CMD_C8_POWER_CONTROL        = 0x20,  //下位机电源控制  0：关闭下位机电源 1：打开下位机电源
    CMD_C8_GETVER               = 0xed,      //获取版本号命令
    CMD_C8_UPDATA_START         = 0xee,      //开始升级
} emC8CmdTdf;

/**************************SL1000 C10*****************************************/
typedef  enum
{
    ERR_C10_Y_MOTOR                          =   0x01,//Y轴电机向上复位错误
    ERR_C10_X_MOTOR                          =   0x02,//X轴电机向右复位错误
    ERR_C10_T_MOTOR                          =   0x03,//旋转电机复位错误
    ERR_C10_SLIDE_FULL                       =   0x04,//载玻片满
    ERR_C10_CommunicationErr                 = 0x30,
    ERR_C10_NoRespondErr                     = 0x31,
}emC10ErrTdf;

typedef enum
{
    CMD_C10_RESET                =   0x01,//复位
    CMD_C10_Y_TO_NEXT            =   0x02,//下一片玻片准备，Y上升一格
    CMD_C10_PUSH_SLIDE           =   0x03,//推玻片
    CMD_C10_BOX_OUT              =   0x04,//玻片盒弹出
    CMD_C10_GET_OP               =   0x05,//获取光耦状态
    CMD_C10_DEBUG                =   0x06,//调试
    CMD_C10_REFRESH              =   0xea,//刷新调试参数
    CMD_C10_READ                 =   0xeb,//加载数据
    CMD_C10_SAVE                 =   0xec,//保存数据
    CMD_C10_GETVER               =   0xed,//读取版本号
    CMD_C10_UPDATA_START         =   0xee,//升级
}emC10CmdTdf;

/**************************SL1000 CB*****************************************/
typedef enum
{
    ERR_CB_OK               = 0,
    ERR_CB_Read_Ic          = 0x01,   // IC卡读卡错误
    ERR_CB_No_Ic            = 0x02,   //无IC卡
    ERR_CB_RecvICData       = 0x04,
    ERR_CB_Communication    = 0x30,
    ERR_CB_NoRespond        = 0x31,
} emCBErrTdf;

typedef enum
{
    CMD_CB_IDLE                 = 0,
    CMD_CB_RESET                = 0x01,   //复位命令
    CMD_CB_READICCARD           = 0x14,   //
    CMD_CB_WRITEICCARD          = 0x15,   //
    CMD_CB_GETVER               = 0xed,   //获取版本号命令
    CMD_CB_UPDATA_START         = 0xee,   //开始升级
} emCBCmdTdf;

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
