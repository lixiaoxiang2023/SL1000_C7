/******************************************************************************
文件名称：ram.h
文件目录：$PROJ_DIR$\code\User\Drive\inc
项目名称：
创建者：胥强
创建日期：2012、10、10
程序用途：
版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
******************************************************************************/
#ifndef __RAM_H
#define __RAM_H
/******************************************************************************
Include files头文件
******************************************************************************/


/******************************************************************************
Constant and Macro Definition常量以及宏定义
******************************************************************************/
/*
REG_CPLD_RST：    CPLD复位寄存器，CPLD上电的时候，系统处于复位状态，所有的硬件状
态处于清零状态，需要将上电复位状态保持100MS以上，在将此寄存器写入1，清除复位状态，
CPLD系统开始正常运行。
1：系统退出复位状态
0：系统处于复位状态
*/
#define         REG_CPLD_RST                            *((uint16_t *)(0x60000000 + (0x400000 << 1)))

/*
REG_CPLD_LCD_PWR:       LCD背光使能
1：开启LCD背光
0：关闭LCD背光
*/
//#define         REG_CPLD_LCD_PWR                        *((uint16_t *)(0x60000000 + (0x400001 << 1)))

/*
REG_CPLD_LCD_EN：       LCD使能
1：LCD开始工作
0：LCD停止工作
*/
//#define         REG_CPLD_LCD_EN                         *((uint16_t *)(0x60000000 + (0x400002 << 1)))

/*
REG_CPLD_LCD_BKP_CONST：       LCD背光级别
参数范围：
7：
6：
5：
4：LCD 4 级背光，同时背光最亮
3：LCD 3 级背光
2：LCD 2 级背光
1：LCD 1 级背光
0：LCD 0 级背光，同时背光最弱
*/
//#define         REG_CPLD_LCD_BKP_CONST                  *((uint16_t *)(0x60000000 + (0x400003 << 1)))

/*
REG_CPLD_BEEP：       蜂鸣器寄存器
1：蜂鸣器工作（蜂鸣器发声）
0：蜂鸣器停止工作（蜂鸣器停止发声）
*/
//#define         REG_CPLD_BEEP                           *((uint16_t *)(0x60000000 + (0x400004 << 1)))

/*
REG_CPLD_SDRAM_PRECHARGE：     SDRAM预充电
1：启动SDRAM预充电
0：停止SDRAM预充电
*/
#define         REG_CPLD_SDRAM_PRECHARGE                *((uint16_t *)(0x60000000 + (0x400005 << 1)))
/*
REG_CPLD_SDRAM_REFRESH：       SDRAM刷新使能
1：启动SDRAM刷新
0：停止SDRAM刷新
*/
#define         REG_CPLD_SDRAM_REFRESH                  *((uint16_t *)(0x60000000 + (0x400006 << 1)))

/*
REG_CPLD_SDRAM_LMR：       SDRAM模式设置使能
1：启动SDRAM模式设置
0：停止SDRAM模式设置
*/
#define         REG_CPLD_SDRAM_LMR                      *((uint16_t *)(0x60000000 + (0x400007 << 1)))

/*
REG_CPLD_CIS_CLR：       CIS重置寄存器
使用：
将此寄存器写1后在清零，将重置CIS模块，CIS的开始存储数据地址将清零
*/
#define         REG_CPLD_CIS_CLR                        *((uint16_t *)(0x60000000 + (0x400008 << 1)))

/*
REG_CPLD_AD9822_CLK：    AD9822的时钟寄存器
此寄存器直接控制AD9822的CLK设置端口
*/
#define         REG_CPLD_AD9822_CLK                     *((uint16_t *)(0x60000000 + (0x400009 << 1)))

/*
REG_CPLD_AD9822_SDA：       AD9822数据总线寄存器
此寄存器直接控制AD9822的SDA设置端口
*/
#define         REG_CPLD_AD9822_SDA                     *((uint16_t *)(0x60000000 + (0x40000a << 1)))

/*
REG_CPLD_AD9822_ADEN：       AD9822数据使能寄存器
此寄存器直接控制AD9822的数据使能设置端口
*/
#define         REG_CPLD_AD9822_ADEN                    *((uint16_t *)(0x60000000 + (0x40000b << 1)))

/*
REG_CPLD_LED_MODE：       tlc5922 mode寄存器
此寄存器直接控制tlc5922的mode设置端口
*/
#define         REG_CPLD_LED_MODE                       *((uint16_t *)(0x60000000 + (0x40000c << 1)))

/*
REG_CPLD_LED_BLANK：       tlc5922 blank寄存器
此寄存器直接控制tlc5922的blank设置端口
*/
#define         REG_CPLD_LED_BLANK                      *((uint16_t *)(0x60000000 + (0x40000d << 1)))

/*
REG_CPLD_LED_XLAT：       tlc5922 xlat寄存器
此寄存器直接控制tlc5922的xlat设置端口
*/
#define         REG_CPLD_LED_XLAT                       *((uint16_t *)(0x60000000 + (0x40000e << 1)))

/*
REG_CPLD_LED_CLK：       tlc5922 CLK寄存器
此寄存器直接控制tlc5922的clk设置端口
*/
#define         REG_CPLD_LED_CLK                        *((uint16_t *)(0x60000000 + (0x40000f << 1)))

/*
REG_CPLD_LED_IN：        tlc5922 IN寄存器
此寄存器直接控制tlc5922的in设置端口
*/
#define         REG_CPLD_LED_IN                         *((uint16_t *)(0x60000000 + (0x400010 << 1)))

/*
REG_CPLD_CIS_START：     CIS 启动寄存器
将此寄存器写1后在清零，将启动一次CIS扫描
*/
#define         REG_CPLD_CIS_START                      *((uint16_t *)(0x60000000 + (0x400011 << 1)))

/*
REG_CPLD_CIS_EN：       CIS 使能寄存器
1：CIS模块时钟使能
0：CIS模快时钟关闭
*/
#define         REG_CPLD_CIS_EN                         *((uint16_t *)(0x60000000 + (0x400012 << 1)))

/*
REG_CPLD_CIS_LEDR_ON_TIM：       CIS LED R开灯时间寄存器
注意此寄存器接受的范围是5~78之间的偶数
*/
#define         REG_CPLD_CIS_LEDR_ON_TIM                *((uint16_t *)(0x60000000 + (0x400013 << 1)))

/*
REG_CPLD_CIS_LEDR_ON_TIM：       CIS LED R开灯时间寄存器
注意此寄存器接受的范围是5~78之间的偶数
*/
#define         REG_CPLD_CIS_LEDG_ON_TIM                *((uint16_t *)(0x60000000 + (0x400014 << 1)))

/*
REG_CPLD_CIS_LEDR_ON_TIM：       CIS LED R开灯时间寄存器
注意此寄存器接受的范围是5~78之间的偶数
*/
#define         REG_CPLD_CIS_LEDB_ON_TIM                *((uint16_t *)(0x60000000 + (0x400015 << 1)))

/*
REG_CPLD_CIS_LEDR_ON_TIM：       CIS LED R开灯时间寄存器
注意此寄存器接受的范围是5~78之间的偶数
*/
#define         REG_CPLD_CIS_LEDIR_ON_TIM               *((uint16_t *)(0x60000000 + (0x400016 << 1)))

/*
REG_CPLD_CIS_LEDR_ON_TIM：       CIS LED R开灯时间寄存器
注意此寄存器接受的范围是5~78之间的偶数
*/
#define         REG_CPLD_CIS_LEDYG_ON_TIM               *((uint16_t *)(0x60000000 + (0x400017 << 1)))
/*
REG_CPLD_LCD_CUR_CTR       LCD Cur on/off
注意此寄存器接受的范围是5~78之间的偶数
*/
//#define         REG_CPLD_LCD_CUR_CTR                    *((uint16_t *)(0x60000000 + (0x400018 << 1)))

/*
REG_CPLD_LCD_CURX       X坐标
注意此寄存器接受的范围是5~78之间的偶数
*/
//#define         REG_CPLD_LCD_CURX                      *((uint16_t *)(0x60000000 + (0x400019 << 1)))

/*
REG_CPLD_LCD_CURX       Y坐标
注意此寄存器接受的范围是5~78之间的偶数
*/
//#define         REG_CPLD_LCD_CURY                      *((uint16_t *)(0x60000000 + (0x40001a << 1)))

/*
REG_CPLD_LCD_CURX       Y坐标
注意此寄存器接受的范围是5~78之间的偶数
*/
//#define         REG_CPLD_LCD_CUR_COLOR                 *((uint16_t *)(0x60000000 + (0x40001b << 1)))

/*
外部存储器的起始地址
*/
#define         EXT_MEMERY_START                        0x60000000
#define         EXT_MEMERY_CIS_START                    0x60400000
/******************************************************************************
数据结构与数据类型定义
******************************************************************************/
/***************************************************************************
宏定义函数申明
******************************************************************************/
/*****************************************************************************
输出函数申明
******************************************************************************/
int RamInit(void);
int SramTest(void);
#endif