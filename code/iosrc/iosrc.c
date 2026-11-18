////@file name :ican_slave_src.c
////@author : xq
////@data :20111014
//
//#include "iosrc.h"
//#include "stm32f2xx.h"
//
//_a2_src_dev g_stA2;
//
//u8 *a2_running              = g_stA2.a2_state_rg.a2_running;
//u8 *a2_command_ack_number   = g_stA2.a2_state_rg.a2_command_ack_number;
//u8 *a2_error_flag           = g_stA2.a2_state_rg.a2_error_flag;
//u8 *a2_error_code           = g_stA2.a2_state_rg.a2_error_code;
//
//u8 *a2_drop_paper           = g_stA2.a2_paper_rg.a2_drop_paper;
//u8 *a2_strap_paper          = g_stA2.a2_paper_rg.a2_strap_paper;
//u8 *a2_paper_time           = g_stA2.a2_paper_rg.a2_paper_time;
//
//u8 *detect_timer_start      = g_stA2.paper_detect_info.detect_timer_start;
//u16 *paper_detect_time      = g_stA2.paper_detect_info.paper_detect_time;
//u8 *result_ok               = g_stA2.paper_detect_info.result_ok;
//u8 *result_tubeNO           = g_stA2.paper_detect_info.result_tubeNO;
//u8 *result_tuberackNO       = g_stA2.paper_detect_info.result_tuberackNO;
//u8 *paper_todetect_count    = g_stA2.paper_detect_info.paper_todetect_count;
//u8 *paper_result            = g_stA2.paper_detect_info.paper_result;
//u8 *result_nulity           = g_stA2.paper_detect_info.result_nulity;
//u8 *paper_nulity            = g_stA2.paper_detect_info.paper_nulity;
//
//u8 *a2_new_command_flag     = g_stA2.a2_command_rg.a2_new_command_flag;
//u8 *a2_command_number       = g_stA2.a2_command_rg.a2_command_number;
//u8 *a2_command_code         = g_stA2.a2_command_rg.a2_command_code;
//u8 *a2_command_param_len    = g_stA2.a2_command_rg.a2_command_param_len;
//u8 *a2_command_param        = g_stA2.a2_command_rg.a2_command_param;
//
//u16 *a2_Drop_steps          = g_stA2.a2_position_rg.a2_Drop_steps;
//u16 *a2_Detect_steps        = g_stA2.a2_position_rg.a2_Detect_steps;
//
//u8 *a2_paper_type           = g_stA2.a2_run_mode_rg.a2_paper_type;
//u8 *a2Aging                   = g_stA2.a2_run_mode_rg.a2Aging;
//
//u8 *a2_Opc_State            = g_stA2.a2_Optocoupler_rg.a2_Opc_State;
//
//u8 *a2_light_value          = g_stA2.a2_Light_value.a2_light_value;
//
//u8 *a2_CanRxBuf_byCanRx     = g_stA2.CanRxBuf.byCanRx;
////u8 *a2_err_code_num         = g_stA2.a2_err_code_imfo.a2_err_code_num;