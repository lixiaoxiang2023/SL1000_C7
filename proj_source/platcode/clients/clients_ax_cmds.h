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
* File Name     : client_ax_cmds.h
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
#ifndef __CLIENTS_AX_CMDS_H_
#define __CLIENTS_AX_CMDS_H_

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition 常量以及宏定义
*******************************************************************************/

/**************************1600PLUS A0*****************************************/
typedef enum _emA0CmdTdf {
    CMD_A0_NORMAL_DETECT         = 0x01,    //正常测试
    CMD_A0_MANUAL_DETECT         = 0x02,    //手动测试
    CMD_A0_RNA_DETECT            = 0x03,    //质控测试
    CMD_A0_CALIBRATE_DETECT      = 0x04,    //定标测试
    CMD_A0_STANDARD_DETECT       = 0x05,    //标准条测试
    CMD_A0_EMERGENCY_DETECT      = 0x06,    //急诊测试
    CMD_A0_LINEAR_DETECT         = 0x07,    //线性带测试
    CMD_A0_RIBBON_DETECT         = 0x08,    //彩带测试
    CMD_A0_SG_CALIBRATE_DETECT   = 0x09,    //比重浊度计校准测试
    CMD_A0_SG_RNA_DETECT         = 0x0a,    //SG质控测试
    CMD_A0_DEBUG_DETECT_TIME     = 0x0b,    //检测时间
    CMD_A0_DEBUG_LIGHT_TIME      = 0x0c,    //灯光检测时间
    CMD_A0_DEBUG_COLLECT_IMAGE   = 0x0d,    //图像收集
    CMD_A0_AGING_DETECT          = 0x0e,    //老练检测
    CMD_A0_STOP_DETECT           = 0x0f,    //停止测试
    CMD_A0_ALL_RESET             = 0x30,    //复位所有模块
    CMD_A0_ERROR_RECOVERY        = 0x50,    //错误恢复
    CMD_A0_SET_RUN_MODE          = 0x60,    //设置运行模式
    CMD_A0_MAINTAIN_FUNC         = 0x70,    //维护功能
    CMD_A0_PROGRAM_UPDATE_READY  = 0x80,    //准备程序升级
    CMD_A0_PROGRAM_UPDATE_DATA   = 0x81,    //传输需要升级的程序数据
    CMD_A0_PROGRAM_UPDATE_START  = 0x82,    //开始程序升级
    CMD_A0_PWR_SYS_SHUTDOWN      = 0x83,    //整机断电
    CMD_A0_READ_A3_SG_CALIBRATE  = 0xe5,    //SG carebration
    CMD_A0_READ_VERSION          = 0xff,    //读取版本号
    CMD_A0_READ_RUN_MODE         = 0xdf,    //读取下位机运行模式
    CMD_A0_READ_RUNNING_STATE    = 0xfe,    //读取下位机运行状态
    CMD_A0_READ_RESULT           = 0xfb,    //读取检测结果

} emA0CmdTdf;

typedef enum _emA0ErrTdf {
    ERR_A0_SUCCESS                  = 0x00,
    MS_DETECT_DATA_UNREAD           = 0x8a,//上个检测数据未读取
    ERR_A0_ICAN_ERR_PARAM           = 0x04,//检测试管号与实际试管号不符
    ERR_A0_ICAN_ERR_TRANS			= 0x07, //传输出错
    ERR_A0_NOT_PRINT_PAPER          = 0x08, //无打印纸
    ERR_A0_AGAIN_PAPER_TYPE         = 0x09, //重新选择试纸类型
    ERR_A0_ERROR_TUBE_NO            = 0x66,//检测试管号与实际试管号不符
    ERR_A0_A1_DISCONNECT            = 0x91,//A1模块断开连接
    ERR_A0_A2_DISCONNECT            = 0x92,//A2模块断开连接
    ERR_A0_A3_DISCONNECT            = 0x93,//A3模块断开连接
    ERR_A0_A5_DISCONNECT            = 0x94,//A5模块断开连接
    ERR_A0_A6_DISCONNECT            = 0x95,//A6模块断开连接
    ERR_A0_A7_DISCONNECT            = 0x96,//A7模块断开连接
    ERR_A0_A9_DISCONNECT            = 0x97,//A9模块断开连接
    ERR_A0_CONSUMABLEINSUFFD21      = 0x98,//清洗液D21不足
    ERR_A0_CONSUMABLEINSUFFD22      = 0x99,//清洗液D22不足
    ERR_A0_PAPERINSUFF              = 0x9A,//试纸不足
    ERR_A0_RUNOUTD21                = 0x9B,//清洗液D21已耗尽
    ERR_A0_RUNOUTD22                = 0x9C,//清洗液D22已耗尽
    ERR_A0_RUNOUTPAPER              = 0x9D,//检测试纸已耗尽

} emA0ErrTdf;

/**************************1600PLUS A1*****************************************/
typedef enum _emA1CmdTdf {
    CMD_A1_IDLE                     = 0x00,
    CMD_A1_RESET                    = 0x41, // 复位命令
    CMD_A1_PAPER_PREPARE            = 0x42, // 选条命令
    CMD_A1_DOWN_PAPER               = 0x43, // 落带命令
    CMD_A1_DEBUG                    = 0x44, // 调试命令
    CMD_A1_SAVE_DEBUG               = 0x45, // 保存命令
    CMD_A1_CANCEL_DEBUG             = 0x46, // 取消调试命令
    CMD_A1_DEBUG_OPTOCOUPLER        = 0x4a, // 读取光耦信号命令
    CMD_A1_SET_RUN_MODE             = 0x4b, // 设置运行模式（是否老练模式）
    CMD_A1_GET_STD_STRIP            = 0x4c, // set line color coll
    CMD_A1_DEBUG_OPT_READ           = 0x4d, //单独读光耦状态命令
    CMD_A1_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A1_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_A1_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A1_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA1CmdTdf;

typedef enum _emA1ErrTdf {
    ERR_A1_NO                       = 0x00,
    ERR_A1_ROLL_RESET               = 0x11, // 大滚筒复位故障
    ERR_A1_SWERVE_RESET             = 0x12, // 转纸器复位故障
    ERR_A1_SHAKE_JAM                = 0x13, // 滚筒复位位置至试纸抖动位置过程卡条
    ERR_A1_PAPER_NO                 = 0x14, // /缺试纸故障
    ERR_A1_RIBBON_PAPER_NO          = 0x15, // 缺RIBBON试纸故障
    ERR_A1_ENCODER                  = 0x16, // 编码器异常
    ERR_A1_ADJ_CHECKPOS             = 0x17, // CIS检测位置异常
    ERR_A1_ADJ_LIGHT                = 0x18, // CIS光值校准异常
    ERR_A1_OC_DECT_JAM              = 0x1b, // 试纸抖动位置到光纤检测位过程卡条故障
    ERR_A1_DROP_JAM                 = 0x1e, // 滚筒下带过程卡条

    ERR_A1_TIME_OUT                 = 0xfd, // 超时
    ERR_A1_OFFLINE                  = 0xfe  // 掉线
} emA1ErrTdf;

typedef enum _emA1DebugTdf {
    DEBUG_A1_IDLE                   = 0x00, // 无操作
    DEBUG_A1_SHAKEPOS               = 0x01, // 试纸抖动位置调试
    DEBUG_A1_FIBERDISTNGUISHPOS     = 0x02, // 光纤检测位置调试
    DEBUG_A1_OPPOSITESTIPOUTPOS     = 0x03, // 转纸器反带出带位置调试函数
    DEBUG_A1_POSITIVESTIPINPOS      = 0x04, // 转纸器正带进带位置调试
    DEBUG_A1_DROPSIPPOS             = 0x05, // 滚筒落带位置调试
    DEBUG_A1_PAPERTYE               = 0x06  // 设置选条试纸类型
} emA1DebugTdf;

/**************************1600PLUS A2*****************************************/
typedef enum
{
    A2_STRIP_11F = 0x00,
    A2_STRIP_12F = 0x01,
    A2_STRIP_14F = 0x02,
    A2_STRIP_LINER = 0x03, //线性带
    A2_STRIP_COLOR = 0x04, //彩带
    A2_STRIP_WHITE = 0x05, //标准条
    A2_STRIP_11FA   = 0x06,
    A2_STRIP_12FA   = 0x07,
    A2_STRIP_14FA   = 0x08,
    A2_STRIP_14FLINER =0x09,
    A2_STRIP_14FCOLOR =0x0a,
}A2StripTypeDef;

typedef enum _emA2CmdTdf {
    CMD_A2_GETPIC                   = 0x05, //获取图片
    CMD_A2_RESET                    = 0x51, // 复位命令
    CMD_A2_TO_DROP                  = 0x52, // 拨至滴样位命令
    CMD_A2_TO_DETECT                = 0x53, // 送至检测位命令
    CMD_A2_STRAP_START              = 0x54, // 皮带传送开始
    CMD_A2_STRAP_STOP               = 0x55, // 皮带传送结束
    CMD_A2_DEBUG                    = 0x56, // 调试命令
    CMD_A2_SAVE_DEBUG               = 0x57, // 保存命令
    CMD_A2_CANCEL_DEBUG             = 0x58, // 取消调试命令
    CMD_A2_SET_RUN_MODE             = 0x59, // 设置运行模式（是否老练模式）
    CMD_A2_RIBBON_TO_DROP           = 0x5a, // 将线性带、彩带送到滴样位
    CMD_A2_DEBUG_OPTOCOUPLER        = 0x5e, // 读取光耦信号命令
    CMD_A2_TO_DROP_2                = 0x5f, // 到滴样位调试命令

    CMD_A2_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A2_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_A2_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A2_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA2CmdTdf;

typedef enum _emA2ErrTdf {
    ERR_A2_NO                       = 0x00,
    ERR_A2_LEVER                    = 0x21, // 试纸拨杆复位故障
    ERR_A2_DROP_PAPER               = 0x22, // 滴样位无试纸故障
    ERR_A2_CHECK_PAPER              = 0x23, // 检测位无试纸故障
    ERR_A2_TYPE_PAPER               = 0x24, // 试纸反面或类型错误 (UI show T-02 strip type is error, T-01 test postion is no strip)
    //    ERR_A2_COMMUNICATE              = 0x25, // A2板与检测板通信故障
    ERR_A2_PSRAM_WR                 = 0x26, // Flash数据写入失败
    ERR_A2_CIS_FILE_OPEN            = 0x27, // CIS文件打开错误
    ERR_A2_CIS_NO_ADJ               = 0x28, // CIS未校准
    ERR_A2_CIS_NO_LINK              = 0x29, // CIS未连接
    ERR_A2_CIS_NO_LIGHT             = 0x2a, // CIS无光亮值
    ERR_A2_FULL_PAPER               = 0x2b, // 废纸盒满
    //    ERR_A2_DROP_REVERSE_PAPER       = 0x2C, // 滴样平台试纸反带故障
    ERR_A2_DEBUG_SPEED              = 0x2d, // A2板传送皮带速度调试失败
    ERR_A2_CIS_FILE                 = 0x2e, // CIS校准文件保存错误
    ERR_A2_ADJ_LIGHT                = 0x2f, // CIS光值自动校准失败
    ERR_A2_SKEW_PAPER               = 0x30, // 试纸歪带错误
    ERR_A2_TIME_OUT                 = 0xfd, // 超时
    ERR_A2_OFFLINE                  = 0xfe  // 掉线
} emA2ErrTdf;

typedef enum _emA2DebugTdf {
    CMD_A2_DETECT                   = 0x00, // 正常检测
    CMD_A2_D_TODROP_STEP            = 0x01, // 落带位置到滴样位置，步数调试
    CMD_A2_D_TOBELT_STEP            = 0x02, // 拨杆电机拨上皮带，步数调试
    CMD_A2_D_CIS_CAL                = 0x03, // CIS白面白平衡校准
    CMD_A2_D_OUTOM_LIGHT            = 0x04, // 自动校准CIS光亮值
    CMD_A2_D_GET_LIGHT              = 0x05, // 自动获取光亮值
    CMD_A2_D_DETECT_RESET           = 0x06, // 检测复位
    CMD_A2_D_M2_FREQ                = 0x07, // 试纸运动时间调试
    CMD_A2_D_M2_TO_DETECT           = 0x08, // 试纸传送到检测
    CMD_A2_D_CIS_TIME               = 0x09, // CIS开灯间隔时间调试
    CMD_A2_D_CLR_CNT                = 0x0A, // 清线性带彩带次数计数
    CMD_A2_D_M1REST                 = 0x0c, // M1电机（拨杆电机）复位至光耦位置命令
    CMD_A2_D_M2RUN                  = 0x0d, // M2电机（圆带电机）运动命令
    CMD_A2_D_M2STOP                 = 0x0e, // M2电机（圆带电机）停止命令
    CMD_A2_D_TO_DROP                = 0x0f, // M1电机从滴样到复位位置调试
    CMD_A2_D_TO_TOBELT              = 0x10, // M1电机从皮带到复位位置调试
    CMD_A2_D_CIS_W                  = 0x11, // CIS白面白平衡校准
    CMD_A2_D_CIS_B                  = 0x12, // CIS黑面白平衡校准
    CMD_A2_D_CLEAN_M2RUN            = 0x13, // 传送带清洗命令
    CMD_A2_D_OPEN_LIGHT             = 0x14, // 根据通道单独开灯
    CMD_A2_D_GET_TIM                = 0x15, // 获取开灯时间
} emA2DebugTdf;

/**************************1600PLUS A3*****************************************/
typedef enum
{
    emPaperType_11F                 = 0,   //11F 试纸类型
    emPaperType_12F                 = 1,   //12F 试纸类型
    emPaperType_14F                 = 2,   //14F 试纸类型
    emPaperType_11FA                = 6,   //11FA试纸类型
    emPaperType_12FA                = 7,   //12FA试纸类型
    emPaperType_14FA                = 8,   //14FA试纸类型
}emPAPER_TYPE;//试纸类型 滴样位置判断使用

typedef enum _emA3CmdTdf {
    CMD_A3_IDLE                     = 0x00, // 空闲指令不做相应

    CMD_A3_PREHEAT                  = 0x01, // 蠕动泵，夹管发 预热
    CMD_A3_CLEAR                    = 0x02, // 故障清除
    CMD_A3_CLEAR_EMPY               = 0x03, // 清空管路
    CMD_A3_PWR_OFF                  = 0x04, // 关机流程
    CMD_A3_SG_RNA                   = 0x60, // 比重浊度计质控命令
    CMD_A3_RESET                    = 0x61, // 复位命令,采样针复位，液路执行复位清洗
    CMD_A3_COLLECT_SAMPLE           = 0x62, // 采样流程
    CMD_A3_COLLECT_SAMPLE_EMG       = 0x70, // 采样流程
    CMD_A3_DROP_SAMPLE              = 0x63, // 滴样流程
    CMD_A3_DEBUG                    = 0X64, // 调试进入调试命令，一般与输入参数配合使用
    CMD_A3_SAVE_DEBUG               = 0X65, // 调试保存，保存调试参数
    CMD_A3_CANCEL_DEBUG             = 0X66, // 取消保存，调试参数取消保存
    CMD_A3_SET_RUN_MODE             = 0x67, // 设置运行模式（是否老练模式）
    CMD_A3_WASH_PIPE                = 0x68, // 管路清洗
    CMD_A3_SAMPLE_RESET             = 0x69, // 采样针复位
    CMD_A3_DEBUG_OPTOCOUPLER        = 0x6c, // 读取光耦信号命令
    CMD_A3_CLEAN_DROPPIPE           = 0x6d, // 滴完样本后清洗
    CMD_A3_SG_CALIBRATE             = 0x6e, // 比重浊度计校准测试
    CMD_A3_SG_END_CALIBRATE         = 0x6F, // 校准结束

    CMD_A3_MAINTAIN_PIPE            = 0x81, // 保养液路命令
    CMD_A3_WASHPIN                  = 0x82, // 针头清洗
    CMD_A3_MAINTAIN_STRONG_CLEAR    = 0X83, //加强保养命令
    CMD_A3_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A3_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_A3_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A3_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA3CmdTdf;

typedef enum _emA3ErrTdf {
    ERR_A3_MC_MOTOR_REST            = 0x01, // MC夹管阀复位错误
    ERR_A3_MD_MOTOR_REST            = 0x02, // MD蠕动泵复位错误
    ERR_A3_Y_SAMP_ANT                = 0x03, //采样针防撞光耦故障
    ERR_A3_MY_DIAN                   = 0x04, //Y轴电机堵转
    ERR_A3_MX_DIAN                   = 0x05, //X轴电机堵转
    ERR_A3_MA_DIAN                   = 0x06, //MA轴电机堵转
    ERR_A3_MB_DIAN                   = 0x07, //MB轴电机堵转
    ERR_A3_MC_DIAN                   = 0x08, //MC轴电机堵转
    ERR_A3_MD_DIAN                   = 0x09, //MD轴电机堵转
    ERR_A3_MC_MOTOR_UP               = 0x0A, // MC夹管阀上限位故障
    ERR_A3_MC_MOTOR_DOWN             = 0x0B, // MC夹管阀下限位故障

    ERR_A3_Y_MOTOR                  = 0x31, // 电机Y轴电机复位错误
    ERR_A3_X_MOTOR                  = 0X32, // 电机X轴电机复位错误
    ERR_A3_MA_MOTOR_REST            = 0x33, // MA泵复位异常
    ERR_A3_MB_MOTOR_REST            = 0x34, // MB泵复位错误
    ERR_A3_SAMP_LESS_ERR            = 0x35, // 样本量不足
    ERR_A3_NotClear                 = 0x36, // 清洗液没有了
    ERR_A3_WasteWater               = 0x37, // 液路堵塞
    ERR_A3_NoSample                 = 0x38, // 检测不到样本（停止检测）
    ERR_A3_WaseFull                 = 0x39, // 废液瓶满
    ERR_A3_LipRst                   = 0x3a, // 液面感应复位失败
    ERR_A3_NoSolWater               = 0x3b, // 保养液不足
    ERR_A3_SG_COLOUR_ADJ    		= 0xd1,/*颜色校准调整光亮值失败*/
    ERR_A3_SG_COLOUR_READ   		= 0xd2,/*颜色校准读取数据出错*/
    ERR_A3_SG_SG_LIGHT_ADJ  		= 0xd3,/*比重校准调整光亮值失败*/
    ERR_A3_SG_SG_READ       		= 0xd4,/*比重校准读取数据出错*/
    ERR_A3_SG_SG_RESULT     		= 0xd5,/*高低比重校准数据不合理，划档不成功*/
    ERR_A3_SG_TURB_LIGHT_ADJ 		= 0xd6,/*浊度校准调整光亮值失败*/
    ERR_A3_SG_TURB_READ     		= 0xd7,/*浊度校准读取数据失败*/
    ERR_A3_SG_TURB_RESULT   		= 0xd8,/*高低浊度校准数据不合理，划档不成功*/
    ERR_A3_SG_TIMEOUT       		= 0xdb,/*超时*/
    ERR_A3_SG_COLOUR_TEST   		= 0xdd,/*颜色检测不成功*/
    ERR_A3_SG_SG_TEST       		= 0xde,/*比重检测不成功*/
    ERR_A3_SG_TURB_TEST     		= 0xdf,/*浊度检测不成功*/
    ERR_A3_SG_COND_TEST     		= 0x12,/*电导率检测不成功*/

    ERR_A3_TIME_OUT                 = 0xfd, // 超时
    ERR_A3_OFFLINE                  = 0xfe  // 掉线
} emA3ErrTdf;

typedef enum _emA3DebugTdf {
    DEBUG_A3_CLEAN_POS_DATA             = 0x01,     // 清洗位置
    DEBUG_A3_NEEDLE_CLEAN_POS_DATA      = 0x02,     // 针头清洗位置
    DEBUG_A3_CLEAN_DECLINE_HEIGHT_DATA  = 0x03,     // 清洗下降高度
    DEBUG_A3_SAMPLE_POS_DATA            = 0x04,     // 正常加样位置
    DEBUG_A3_STAT_POS_DATA              = 0x05,     // 急诊位置
    DEBUG_A3_SAMPLE_DEPTH_DATA          = 0x06,     // 加样深度
    DEBUG_A3_START_DROP_POS_12F_DATA    = 0x07,     // 11/12F开始滴样位置
    DEBUG_A3_DROP_HEIGHT_DATA           = 0x08,     // 滴样高度
    DEBUG_A3_START_DROP_POS_14F_DATA    = 0x09,     // 14F开始滴样位置

    //drop volume 11F
    DEBUG_DROP_VOLUME_VC_11F               = 0x0a,     // 11F VC维生素
    DEBUG_DROP_VOLUME_WBC_11F              = 0x0b,     // 11F WBC白细胞
    DEBUG_DROP_VOLUME_KET_11F              = 0x0c,     // 11F KET酮体
    DEBUG_DROP_VOLUME_NIT_11F              = 0x0d,     // 11F NIT亚硝酸盐
    DEBUG_DROP_VOLUME_URO_11F              = 0x0e,     // 11F URO尿胆原
    DEBUG_DROP_VOLUME_BIL_11F              = 0x0f,     // 11F BIL胆红素
    DEBUG_DROP_VOLUME_PRO_11F              = 0x10,     // 11F PRO蛋白质
    DEBUG_DROP_VOLUME_GLU_11F              = 0x11,     // 11F GLU葡萄糖
    DEBUG_DROP_VOLUME_SG_11F               = 0x12,     // 11F SG尿比重
    DEBUG_DROP_VOLUME_BLD_11F              = 0x13,     // 11F BLD隐血
    DEBUG_DROP_VOLUME_PH_11F               = 0x14,     // 11F PH酸碱度
    DEBUG_DROP_VOLUME_CAL_11F              = 0x15,     // 11F CAL空白

    //drop volume 12F
    DEBUG_DROP_VOLUME_NIT_12F              = 0x16,     // NIT亚硝酸盐
    DEBUG_DROP_VOLUME_WBC_12F              = 0x17,     // WBC白细胞
    DEBUG_DROP_VOLUME_KET_12F              = 0x18,     // KET酮体
    DEBUG_DROP_VOLUME_CR_12F               = 0x19,     // CR肌酐
    DEBUG_DROP_VOLUME_URO_12F              = 0x1a,     // URO尿胆原
    DEBUG_DROP_VOLUME_BIL_12F              = 0x1b,     // BIL胆红素
    DEBUG_DROP_VOLUME_PRO_12F              = 0x1c,     // PRO蛋白质
    DEBUG_DROP_VOLUME_GLU_12F              = 0x1d,     // GLU葡萄糖
    DEBUG_DROP_VOLUME_SG_12F               = 0x1e,     // SG尿比重
    DEBUG_DROP_VOLUME_BLD_12F              = 0x1f,     // BLD隐血
    DEBUG_DROP_VOLUME_PH_12F               = 0x20,     // PH酸碱度
    DEBUG_DROP_VOLUME_MA_12F               = 0x21,     // MA微白蛋白
    DEBUG_DROP_VOLUME_CAL_12F              = 0x22,     // CAL空白

    //drop volume 14F
    DEBUG_DROP_VOLUME_WBC_14F              = 0x23,     // WBC 白细胞
    DEBUG_DROP_VOLUME_KET_14F              = 0x24,     // KET 酮体
    DEBUG_DROP_VOLUME_NIT_14F              = 0x25,     // NIT 亚硝酸盐
    DEBUG_DROP_VOLUME_URO_14F              = 0x26,     // URO 尿胆原
    DEBUG_DROP_VOLUME_BIL_14F              = 0x27,     // BIL 胆红素
    DEBUG_DROP_VOLUME_GLU_14F              = 0x28,     // GLU 葡萄糖
    DEBUG_DROP_VOLUME_PRO_14F              = 0x29,     // PRO 蛋白质
    DEBUG_DROP_VOLUME_SG_14F               = 0x2a,     // SG  尿比重
    DEBUG_DROP_VOLUME_PH_14F               = 0x2b,     // PH  酸碱度
    DEBUG_DROP_VOLUME_BLD_14F              = 0x2c,     // BLD 隐血
    DEBUG_DROP_VOLUME_VC_14F               = 0x2d,     // VC 维生素
    DEBUG_DROP_VOLUME_MA_14F               = 0x2e,     // MA 微白蛋白
    DEBUG_DROP_VOLUME_CA_14F               = 0x2f,     // CA 尿钙
    DEBUG_DROP_VOLUME_CR_14F               = 0x30,     // CR 肌酐
    DEBUG_DROP_VOLUME_CAL_14F              = 0x31,     // CAL 空白


    DEBUG_A3_SOLENOID1_COMMAND          = 0x32,         // 电磁阀1
    DEBUG_A3_SOLENOID2_COMMAND          = 0x33,         // 电磁阀2
    DEBUG_A3_SOLENOID3_COMMAND          = 0x34,         // 电磁阀3
    DEBUG_A3_SOLENOID4_COMMAND          = 0x35,         // 夹管阀
    DEBUG_A3_MONITOR1_COMMAND           = 0x36,         // 大柱塞泵
    DEBUG_A3_MONITOR2_COMMAND           = 0x37,         // 小柱塞泵

    CMD_A3_DEBUG_DROP_CMD                  = 0x6D,//滴样量分析命令
    CMD_A3_CON_CAIL                        = 0x6F,//电导率校准
    CMD_A3_CON_SET                         = 0x71,//设置电导率校准值
    CMD_A3_CON_CHK                         = 0x72,//电导率检测
    CMD_A3_DEBUG_TIG_RESET                 = 0x73,//气密性调试复位并移动至急诊位流程
    CMD_A3_DEBUG_TIG_DROP                  = 0x74,//气密性调试采样流程
    CMD_A3_DEBUG_TIG_SAMP                  = 0x75,//气密性调试滴样流程
    CMD_A3_DEBUG_TIG_CLEAN_DROPPIPE        = 0x76,//气密性调试滴样后排出剩余样本流程
    CMD_A3_DEBUG_TIG_DRAIN_DROPPIPE        = 0x77,//气密性调试滴样后清洗流程
    CMD_A3_DEBUG_PERISTALTIC_TURNS         = 0x78,//蠕动泵圉数

    DEBUG_A3_OPEN_COLOR_LIGHT           = 0x39,          // 颜色检测灯
    DEBUG_A3_OPEN_TURBIDITY_LIGHT       = 0x3a,          // 浊度检测灯
    DEBUG_A3_OPEN_SG_LIGHT              = 0x3b,          // 比重检测灯
    DEBUG_A3_MEASURE_COLOR              = 0x3c,          // 颜色检测
    DEBUG_A3_MEASURE_TURBIDITY          = 0x3d,          // 浊度检测
    DEBUG_A3_MEASURE_SG                 = 0x3e,          // 比重检测
    DEBUG_A3_CALIBRATE_COLOR            = 0x3f,          // 颜色校准
    DEBUG_A3_CALIBRATE_TURBIDITY        = 0x40,          // 浊度校准
    DEBUG_A3_CALIBRATE_SG               = 0x41,          // 比重校准

    //11FA
    DEBUG_DROP_VOLUME_VC_11FA               = 0x42,     // VC维生素
    DEBUG_DROP_VOLUME_WBC_11FA              = 0x43,     // WBC白细胞
    DEBUG_DROP_VOLUME_KET_11FA              = 0x44,     // KET酮体
    DEBUG_DROP_VOLUME_NIT_11FA              = 0x45,     // NIT亚硝酸盐
    DEBUG_DROP_VOLUME_URO_11FA              = 0x46,     // URO尿胆原
    DEBUG_DROP_VOLUME_BIL_11FA              = 0x47,     // BIL胆红素
    DEBUG_DROP_VOLUME_PRO_11FA              = 0x48,     // PRO蛋白质
    DEBUG_DROP_VOLUME_GLU_11FA              = 0x49,     // GLU葡萄糖
    DEBUG_DROP_VOLUME_SG_11FA               = 0x4a,     // SG尿比重
    DEBUG_DROP_VOLUME_BLD_11FA              = 0x4b,     // BLD隐血
    DEBUG_DROP_VOLUME_PH_11FA               = 0x4c,     // PH酸碱度
    DEBUG_DROP_VOLUME_CAL_11FA              = 0x4d,     // CAL空白

    //drop volume 12FA
    DEBUG_DROP_VOLUME_NIT_12FA              = 0x4e,     // NIT亚硝酸盐
    DEBUG_DROP_VOLUME_WBC_12FA              = 0x4f,     // WBC白细胞
    DEBUG_DROP_VOLUME_KET_12FA              = 0x50,     // KET酮体
    DEBUG_DROP_VOLUME_CR_12FA               = 0x51,     // CR肌酐
    DEBUG_DROP_VOLUME_URO_12FA              = 0x52,     // URO尿胆原
    DEBUG_DROP_VOLUME_BIL_12FA              = 0x53,     // BIL胆红素
    DEBUG_DROP_VOLUME_PRO_12FA              = 0x54,     // PRO蛋白质
    DEBUG_DROP_VOLUME_GLU_12FA              = 0x55,     // GLU葡萄糖
    DEBUG_DROP_VOLUME_SG_12FA               = 0x56,     // SG尿比重
    DEBUG_DROP_VOLUME_BLD_12FA              = 0x57,     // BLD隐血
    DEBUG_DROP_VOLUME_PH_12FA               = 0x58,     // PH酸碱度
    DEBUG_DROP_VOLUME_MA_12FA               = 0x59,     // 微白蛋白
    DEBUG_DROP_VOLUME_CAL_12FA              = 0x5a,     // CAL空白

    //drop volume 14FA
    DEBUG_DROP_VOLUME_WBC_14FA              = 0x5c,     // WBC白细胞
    DEBUG_DROP_VOLUME_KET_14FA              = 0x5d,     // KET酮体
    DEBUG_DROP_VOLUME_NIT_14FA              = 0x5e,     // NIT亚硝酸盐
    DEBUG_DROP_VOLUME_URO_14FA              = 0x5f,     // URO尿胆原
    DEBUG_DROP_VOLUME_BIL_14FA              = 0x60,     // BIL胆红素
    DEBUG_DROP_VOLUME_GLU_14FA              = 0x61,     // GLU葡萄糖
    DEBUG_DROP_VOLUME_PRO_14FA              = 0x62,     // PRO蛋白质
    DEBUG_DROP_VOLUME_SG_14FA               = 0x63,     // SG尿比重
    DEBUG_DROP_VOLUME_PH_14FA               = 0x64,     // PH酸碱度
    DEBUG_DROP_VOLUME_BLD_14FA              = 0x65,     // BLD隐血
    DEBUG_DROP_VOLUME_VC_14FA               = 0x66,     // VC维生素
    DEBUG_DROP_VOLUME_MA_14FA               = 0x67,     // MA微白蛋白
    DEBUG_DROP_VOLUME_CA_14FA               = 0x68,     // CA尿钙
    DEBUG_DROP_VOLUME_CR_14FA               = 0x69,     // CR肌酐
    DEBUG_DROP_VOLUME_CAL_14FA              = 0x6a,     // CAL空白
    DEBUG_A3_PERISTALTIC_PUMP               = 0x6b,     // 蠕动泵
    DEBUG_A3_PUMP_TURNS                     = 0x78,     // 蠕动泵圈数

} emA3DebugTdf;

typedef enum _emA3RunModeTdf {
    RUN_MODE_A3_CLEANING_FLUID              = 0x01,     // 设置A3板为清洗模式, 其中参数为0表示普通清洗，参数为1表示加强清洗
    RUN_MODE_A3_MIX                         = 0x02,     // 设置A3板混匀模式, 其中参数为0表示非混匀模式，参数为1表示混匀模式
    RUN_MODE_A3_LIQUID_LEVEL_SENSING        = 0x03,     // 设置A3板液面感应， 0：开启液面感觉 ，1：关闭液面感觉
    RUN_MODE_A3_DETECT_SAMPLE_TEST          = 0x04,     // 设置A3板样本量检测 0：开启样本量检测， 1：关闭样本量检测
    RUN_MODE_A3_SET_SG                      = 0x05,     // 设置A3板SG，      1：开启SG模式，   0：关闭SG 模式  （需要判断是否关闭成功)
    RUN_MODE_A3_INJECTION_PUMP              = 0x06,     // 设置注射泵容量     0:250ul  1:600ul  2:1000ul
    RUN_MODE_A3_SET_COND                    = 0x07,     // 设置A3板电导率，      1：电导率开模式，   0：关闭电导率 模式  （需要判断是否关闭成功)
    RUN_MODE_A3_SET_PreventCollision        = 0x08,     // 设置A3板防撞，      1：开启，   0：关闭  （需要判断是否关闭成功)
} emA3RunModeTdf;

/**************************1600PLUS A5*****************************************/
typedef enum _emA5CmdTdf {
    CMD_A5_IDLE                     = 0x00,
    CMD_A5_RESET                    = 0x51, // 复位
    CMD_A5_NEXT_TRUB                = 0x52, // 找下一个试管，（为兼容1600plus需带，0继续找下一个试管架
    //  1 测试完本次试管架(质控或校准)）
    CMD_A5_SET_RUN_MODE             = 0x53, // 设置指令,需带参数emB5SetCmdTdf
    CMD_A5_DEBUG                    = 0x54, // 调试指令,需带参数emB5DebCmdTdf
    CMD_A5_SAVE_DEBUG               = 0x55, // 调试参数保存
    CMD_A5_CANCEL_DEBUG             = 0x56, // 取消保存
    CMD_A5_RETEST                   = 0x57, // 复测功能
    CMD_A5_EMERREAD                 = 0x58, // 待机读急诊按钮

    CMD_A5_RACK_IN_A5               = 0x81, //进桥命令
    CMD_A5_RACK_OUT_A5              = 0x82, //出桥命令
    CMD_A5_ERR_CLEAR                = 0x83, //无错误命令
    CMD_A5_REQUEST_PASS_BRIDGE      = 0x84, //请求出桥命令

    CMD_A5_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A5_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_A5_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A5_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA5CmdTdf;

typedef enum _emA5ErrTdf {
    ERR_A5_NO                       = 0x00,
    ERR_A5_Rack_ID_Timeout          = 0x06, // 试管架号扫描超时
    ERR_A5_M6_Push_RESE             = 0x08, // M6复位故障，  (SS模块的，未使用)
    ERR_A5_M1_RIGHT_RESET           = 0x51, // 右侧M1电机复位故障
    ERR_A5_M2_LEFT_RESET            = 0x52, // 左侧M2电机复位故障
    ERR_A5_M3_UP_RESET              = 0x53, // 找试管M3电机复位故障
    ERR_A5_M4_DOWN_RIGHT_RESET      = 0x54, // 联机电机右侧复位故障
    ERR_A5_M4_DOWN_LEFT_RESET       = 0x55, // 联机电机左侧复位故障
    ERR_A5_LEFT_NO_SPACE            = 0x56, // 左侧试管架满
    ERR_A5_NO_tuberack              = 0x57, // 无待测试管架  （未使用）
    ERR_A5_OVERTIME_ON_BRIDGE       = 0x58, // 进样机构进桥超时

    ERR_A5_RIGHT_INSIDE_OPC         = 0x5d, // 进样机构右内侧光耦故障
    ERR_A5_RIGHT_OUTSIDE_OPC        = 0x5c, // 进样机构右外侧光耦故障
    ERR_A5_LEFT_INSIDE_OPC          = 0x5b, // 进样机构左内侧光耦故障
    ERR_A5_LEFT_OUTSIDE_OPC         = 0x5a, // 进样机构左外侧光耦故障
    ERR_A5_FbAdjErr                 = 0x6b, // 光纤校准失败

    ERR_A5_MS_ERROR_TUBE_NO         = 0x66, // 检测试管号与实际试管号不符
    ERR_A5_BARCODE_TIMEOUT          = 0x68, // 条形码扫描超时，请查看条形码接线是否完好
    ERR_A5_SCAN_TEST_TUBES_ERR      = 0x69, // 扫描试管架错误，请查看线路是否接好，扫描接口板上电源指示灯是否亮
    ERR_A5_BRIDGE_CHOCK_ERR         = 0x6a, // 联机时，由于速度不匹配，进桥堵塞，导致检测暂停

    ERR_A5_TIME_OUT                 = 0xfd, // 超时
    ERR_A5_OFFLINE                  = 0xfe,  // 掉线
    ERR_A5_UPDATA_FILE                 = 0xff, // 升级失败
} emA5ErrTdf;

typedef enum _emA5DebugTdf {
    CMD_A5_RIGHT_ST_POS             = 0x01, // 右拨杆起始位
    CMD_A5_RIGHT_INTVL              = 0x02, // 右拨杆间距
    CMD_A5_LEFT_ST_POS              = 0x03, // 左拨杆起始位置
    CMD_A5_LEFT_INTVL               = 0x04, // 左拨杆间距
    CMD_A5_ONLINE_POS               = 0x05, // 试管架进桥步数,1600
    CMD_A5_CYCLEPLACE_POS           = 0x06, // 右侧传送试管架位置,循环模式,1600
    CMD_A5_TUBE_1_POS               = 0x09, // 第一试管检测位
    CMD_A5_TUBE_10_POS              = 0x12, // 第十试管检测位
    CMD_A5_RACK2LEFT_POS            = 0x13, // 试管架移至左侧步数
    CMD_A5_TUBECOMPENSE_POS         = 0x14, // 光耦补偿位,1600
    CMD_A5_SCAN_BAR_CODE            = 0x15, // 条码扫描
    CMD_A5_RACK_ID_DETECT           = 0x16, // 读取一次试管架号
    CMD_A5_DEBUG_THRESHOLD_POINTS   = 0x1f, // 紫外调试界面设置阈值命令
    CMD_A5_DEBUG_DECTTUBE           = 0x20, // 紫外调试界面设置判断点数命令
    //以下是新增量产16没有的命令
    CMD_A5_DEBUG_UVTUBE             = 0x21, // 读取光亮值命令
    CMD_A5_TUBE_POS                 = 0x26, // 试管中心位置,弃用
    CMD_A5_POSDEBUGRE               = 0x27, // 预留一个调试命令
    CMD_A5_RACKID_DEBUG             = 0x28, // 试管架号调试
    CMD_A5_WRTUBERACKID             = 0x29, // 写试管架号
    CMD_A5_DEBUG_OPTO               = 0x2a, // 获取一次光耦的状态
    CMD_A5_UVLED_ON_OFF             = 0x2b, // 紫外灯常亮常关
    CMD_A5_UVLED_ADJ                = 0X2C, // 紫外灯校准
} emA5DebugTdf;

typedef enum _emA5RunModeTdf {

    /*
    0x01 代表与1200联机
    0x02 代表与1280联机
    0x03 代表与IQ200联机
    0x04 代表与爱威联机
    0x05 代表与西斯美康联机
    0x06 待变与EH-2080联机
    0x07 代表A6A7联机
    */
    CMD_A5_ONLINE_MODEL           = 0x01,// 设置联机模式 0x00：单机= 0x01：联机
    CMD_A5_CYCLEONOFF             = 0x02,// 设置循环模式 0x00: 关闭循环 0x01:开启循环
    CMD_A5_TUBERACKID_ONOFF       = 0x03,// 试管架号扫描开关 00 关 01开
    CMD_A5_BARCODE_ONOFF          = 0x04,// 条码扫描开关  00 关 01开
    CMD_A5_TYPE                   = 0x05,// 进样机构类型 0 前端 1末端
    CMD_A5_A6ONLINE               = 0x06, // 设置A6是否联机
} emA5RunModeTdf;

/**************************1600PLUS A6*****************************************/
typedef enum _emA6CmdTdf {
    CMD_A6_RESET                    = 0xa1, // 复位命令
    CMD_A6_NEXT_TRUB                = 0xa2, // 查找试管命令
    CMD_A6_PASS_BRIDGE              = 0xa3, // 进桥命令
    CMD_A6_DEBUG                    = 0xa4, // 调试命令
    CMD_A6_SAVE_DEBUG               = 0xa5, // 保存命令
    CMD_A6_CANCEL_DEBUG             = 0xa6, // 取消调试命令
    CMD_A6_DEBUG_OPTOCOUPLER        = 0xaa, // 调试光耦状态

    CMD_A6_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A6_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_A6_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A6_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA6CmdTdf;

typedef enum _emA6DebugTdf {
    CMD_A6_START_POS                = 0x01, // 扩容模块上拨杆调试步数
    CMD_A6_INTVL_POS                = 0x02, // 扩容模块试管架间隔步数
    CMD_A6_TOA5_POS                 = 0x03, // 扩容模块推到A5模块步数
    CMD_A6_UPDATA                   = 0x04, // 刷新参数命令
//    CMD_A6_CANCELDATA               = 0x05, // 取消保存
//    CMD_A6_SAVEDATA                 = 0x06, // 保存设置参数

}emA6DebugTdf;

typedef enum _emA6ErrTdf {
    ERR_A6_NO                       = 0x00,
    ERR_A6_Y_MOTOR_ERR              = 0xa1, // 电机M1向上复位错误
    ERR_A6_X_MOTOR_ERR              = 0xa2, // 电机M2向右复位错误
    ERR_A6_PLAT_NOTUBE_ERR          = 0xa3, // 已无试管架
    ERR_A6_PLAT_EMCSTOP             = 0xa4, // 急停按钮打开

    ERR_A6_TIME_OUT                 = 0xfd, // 超时
    ERR_A6_OFFLINE                  = 0xfe  // 掉线
} emA6ErrTdf;

/**************************1600PLUS A7*****************************************/
typedef enum _emA7CmdTdf {
    CMD_A7_RESET                    = 0xb1, // 复位命令
    CMD_A7_QUERY_CROSS_BRIGDE       = 0xb2, // 查询是否可以进桥
    CMD_A7_DEBUG                    = 0xb3, // 调试命令
    CMD_A7_SAVE_DEBUG               = 0xb4, // 保存命令
    CMD_A7_CANCEL_DEBUG             = 0xb5, // 取消调试命令
    CMD_A7_DEBUG_OPTOCOUPLER        = 0xb9, // 调试光耦状态

    CMD_A7_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A7_UPDATE_DATA              = 0xFC, // 升级接收数据命令
    CMD_A7_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A7_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA7CmdTdf;

typedef enum _emA7ErrTdf {
    ERR_A7_NO                       = 0x00,
    ERR_A7_RESET_ERR                = 0xb1, // 电机复位错误
    ERR_A7_PLAT_FULL_ERR            = 0xb2, // 平台试管满

    ERR_A7_TIME_OUT                 = 0xfd, // 超时
    ERR_A7_OFFLINE                  = 0xfe  // 掉线
} emA7ErrTdf;

/**************************2000   A8 PWR*************************************/
typedef enum _emA8SysStateTdf {
    SYS_A8_ON                       = 0x00, // 开机，蓝闪
    SYS_A8_STANDBY                  = 0x01, // 待机，常绿
    SYS_A8_RUN                      = 0x02, // 运行，绿闪
    SYS_A8_ERR                      = 0x03, // 故障，红闪
    SYS_A8_OFF                      = 0x04
} emA8SysStateTdf;

typedef enum _emA8KeyEvenTdf {
    KEY_A8_Click                    = 0x00, // 点击
    KEY_A8_Press                    = 0x01, // 短按
    KEY_A8_LPress                   = 0x02, // 长按
    KEY_A8_EvenNum                  = 0x03
} emA8KeyEvenTdf;

typedef enum __emA8Cmd {
    CMD_A8_SYS_SHUTDOWN             = 0x83, // 整机断电
    CMD_A8_SYS_ON                   = 0x84, // 开机，蓝闪
    CMD_A8_SYS_STANDBY              = 0x85, // 待机，常绿
    CMD_A8_SYS_RUN                  = 0x86, // 运行，绿闪
    CMD_A8_SYS_ERR                  = 0x87, // 故障，红闪
    CMD_A8_BOOT_ALL_INIT            = 0x88, // 仪器开机
    CMD_A8_SYS_FQ                   = 0x89, // 蜂鸣器

    CMD_A8_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A8_UPDATE_STOP              = 0xEF  // 进入固件升级退出
} emA8CmdTdf;

typedef enum __emA8Err {
    ERR_A8_NO                       = 0x00,

    ERR_A8_TIME_OUT                 = 0xfd, // 超时
    ERR_A8_OFFLINE                  = 0xfe  // 掉线
} emA8ErrTdf;

/**************************sl   C9/A9 barcode******************************/
typedef enum _emA9CmdTdf
{
    CMD_A9_IDLE                     = 0x00,
    CMD_A9_Rst                      = 0x01, // 复位
    CMD_A9_Open                     = 0x02, // 张开
    CMD_A9_Read                     = 0x03, // 读条码
    CMD_A9_SCAN_EN                  = 0x04, // 设置条码开关
    CMD_A9_GETOPC                   = 0x05, // 读条码,读取条码失败时返回Barcode_Timeout，适合调试页面用
    CMD_A9_Debug                    = 0x06, // 预留
    CMD_A9_SAVE                     = 0x09, // 保存参数
    CMD_A9_RdTimeSet                = 0x0a, // 设置汇像条码扫描时间
    CMD_A9_Check                    = 0x0b, // 自检光耦及扫码器状态
    CMD_A9_REFRESH                  = 0xea, // 刷新参数
    CMD_A9_LOAD                     = 0xeb, // 加载参数
    CMD_A9_GETVER                   = 0xed, // 获取版本号命令

    CMD_A9_UPDATE_START             = 0xEE, // 进入固件升级模式
    CMD_A9_UPDATE_STOP              = 0xEF, // 进入固件升级退出
    CMD_A9_UPDATE_READY             = 0xFB, // 升级准备命令
    CMD_A9_UPDATE_DATA              = 0xFC  // 升级接收数据命令
} emA9CmdTdf;

typedef enum _emA9ErrTdf {
    ERR_A9_OK                       = 0x00,
    ERR_A9_M1REST                   = 0xc1, // M1电机复位故障
    ERR_A9_SCANCODE_READ            = 0xc2, // 读条码超时
    ERR_A9_MotorStep                = 0xc3, // 垂直深度设置失败
    ERR_A9_CommunicationErr         = 0xc4, // 通讯错误
    ERR_A9_NoRespondErr             = 0xc5, // 无应答

    ERR_A9_TIME_OUT                 = 0xfd, // 超时
    ERR_A9_OFFLINE                  = 0xfe  // 掉线
} emA9ErrTdf;

/************************** sl1000 A15 camera ******************************/
typedef enum _emA15CmdTdf
{
    CMD_A15_NULL                              = 0x00, // 空命令
    CMD_A15_SET_GAIN                          = 0x01, // 设置增益
    CMD_A15_SET_EXPLODE_TIM                   = 0x02, // 设置曝光
    CMD_A15_SET_BLANCE                        = 0x03, // 设置白平衡
    CMD_A15_SET_IP                            = 0x04, // 设置IP
    CMD_A15_SET_MODE                          = 0x05, // 设置相机触发模式（软，硬，外部触发)
    CMD_A15_SET_FOCUS                         = 0x06, // 设置细焦点
    CMD_A15_SET_COARSE_FOCUS                  = 0x07, // 设置精焦点
    CMD_A15_SET_AUTO_FOCUS                    = 0x08, // 设置自动细对焦
    CMD_A15_SET_AUTO_FOCUS_WITH_IMAGE         = 0x09, // 设置自动细对焦返回图片
    CMD_A15_SET_AUTO_COARSE_FOCUS             = 0x0a, // 设置自动粗对焦
    CMD_A15_SET_AUTO_COARSE_FOCUS_WITH_IMAGE  = 0x0b, // 设置自动粗对焦返回图片
    CMD_A15_SET_TRIG_MODE                     = 0x0c, // 设置触发模式
    CMD_A15_SET_CONFIG                        = 0x0d, // 设置配置
    CMD_A15_SET_ADJUST_MODE                   = 0x0e, // 设置自动颜色平衡
    CMD_A15_SET_Z_CALIBRATION                 = 0x0f, // 设置Z轴自动对焦
    CMD_A15_SET_IMAGE_FILE                    = 0x10, // 发送图片数据
    CMD_A15_SET_IMAGE_VIDEO                   = 0x11, // 发送视频数据
    CMD_A15_SET_HEARTBEAT                     = 0x12, // 设置心跳数据
    CMD_A15_GET_CONFIG                        = 0x13, // 获取配置
    CMD_A15_GET_IMG                           = 0x14, // 获取单张图片
    CMD_A15_GET_IMG_FOCUE                     = 0x15, // 获取单张融合图片
    CMD_A15_GET_IMG_JOINT_START               = 0x16, // 获取图片拼图开始
    CMD_A15_GET_IMG_JOINT_STOP                = 0x17, // 获取图片拼图停止
    CMD_A15_SET_SYSTIME                       = 0x18, // 设置系统时间
    CMD_A15_GET_VER                           = 0x19, // 获取版本号
    CMD_A15_SAVE_PARAM                        = 0x1a, // 保存白平衡参数
    CMD_A15_RESET_PARAM                       = 0x1b, // 重置白平衡参数
    CMD_A15_WET_PHOTOGRAPHY                   = 0x1c, // 湿法拍照
    CMD_A15_WET_VIDEOPHOTO                    = 0x1d, // 湿法录制视频加拍照

} emA15CmdTdf;

typedef enum _emA15ErrTdf
{
    ERR_A15_OK                                = 0x00,
    ERR_A15_AUTO_COARSE_FOCUS_FAIL            = 0x01, //自动粗对焦失败
    ERR_A15_WHITEBALANCE_CORRECTION_FAIL      = 0x02, //白平衡校正失败

}emA15ErrTdf;


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
