/*******************************************************************************
文件名称：   ps2.c
文件目录：   $PROJ_DIR$/usr/Drive
项目名称：   事业一部技术平台
创建者：     陆作沛
创建日期：   2013-08-25
程序用途:    PS/2驱动程序

版权声明：   Copyright urit Co.,Ltd.All Right Reserved

日期：********  陆作沛
初稿

日期：20160422 胥强
加入技术平台

*******************************************************************************/

/*******************************************************************************
Include files头文件
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_PS2)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define BARCODE_BUF_MAX 20
#define BARCODE_RX_BUFF 90

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
　　内部全局变量定义
*******************************************************************************/

static unsigned char BARCODE_CHAR[59][2] =
{
	0x1C, 'a', 0x32, 'b', 0x21, 'c', 0x23, 'd', 0x24, 'e', 0x2B, 'f',
	0x34, 'g', 0x33, 'h', 0x43, 'i', 0x3B, 'j', 0x42, 'k', 0x4B, 'l',
	0x3A, 'm', 0x31, 'n', 0x44, 'o', 0x4D, 'p', 0x15, 'q', 0x2D, 'r',
	0x1B, 's', 0x2C, 't', 0x3C, 'u', 0x2A, 'v', 0x1D, 'w', 0x22, 'x',
	0x35, 'y', 0x1A, 'z', 0x45, '0', 0x16, '1', 0x1E, '2', 0x26, '3',
	0x25, '4', 0x2E, '5', 0x36, '6', 0x3D, '7', 0x3E, '8', 0x46, '9',
	0x0E, '`', 0x4E, '-', 0x55, '=', 0x5D, '\\', 0x29, ' ', 0x54, '[',
	0x5B, ']', 0x4C, ';', 0x52, '\'', 0x41, ',', 0x49, '.', 0x4A, '/',
	0x71, '.', 0x70, '0', 0x69, '1', 0x72, '2', 0x7A, '3', 0x6B, '4',
	0x73, '5', 0x74, '6', 0x6C, '7', 0x75, '8', 0x7D, '9',
};

static struct {
    unsigned char byIntCnt;        // 中断次数计数
	unsigned char byCodeCnt;       // 总共接收到的字符数
    unsigned char byCode;

    void (*pfPS2Bckhandle) (const char *p, unsigned int dwLen);     // 扫描一次条码后的回调函数

    unsigned int *pwdClkGpio;      //时钟的位定义地址
    unsigned int *pwdSdaGpio;      //数据的位定义地址

    unsigned char byIrpChn;

    char abyCodeStr[BARCODE_RX_BUFF];
}s_DevCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 设定ps2的中断回调函数
参数描述：
*   @ void (*PS2IRQHander)(const char *p, unsigned int)：中断回调函数类型
返回值描述：
*   @ 无
说明：
*   @ PS2接收到数据，将调用此注册的函数，注意此函数将在中断中执行，不能有太久的延时
*******************************************************************************/
static void SetIrq( void (*PS2IRQHander)(const char *p, unsigned int))
{
	s_DevCtr.pfPS2Bckhandle = PS2IRQHander;
	if(PS2IRQHander == 0)
	{
		DevIo->IrpDown( s_DevCtr.byIrpChn, 1);		
	}
	else
	{
		DevIo->IrpUp( s_DevCtr.byIrpChn, 1);		
	}
}

/*******************************************************************************
函数功能：
*   @ 解码
参数描述：
*   @ unsigned int byLenth：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static unsigned int DeCode(unsigned int byLenth)
{
    int iIndex = 0;
    bool blUpper = false;

    for(int i = 0 ; i < byLenth ;i ++) {
        if(s_DevCtr.abyCodeStr[i] == 0x12) {
            // SHITF
            blUpper = true;
            i++;
        }
        else if((s_DevCtr.abyCodeStr[i] == 0xF0) && (s_DevCtr.abyCodeStr[i+1] == 0x12)) {
            // SHITF断码
            blUpper = false;
            i++;
            continue;
        }

        for(int j = 0 ; j < 59; j ++ ) {
            if( s_DevCtr.abyCodeStr[i] == BARCODE_CHAR[j][0]) {
                if( iIndex < BARCODE_BUF_MAX ) {
                    if(blUpper == true ) {
                        if(( BARCODE_CHAR[j][1] >= 'a') && (BARCODE_CHAR[j][1] <= 'z')) {
                            // 字母 要改为大写
                            s_DevCtr.abyCodeStr[iIndex ++] = BARCODE_CHAR[j][1]-0x20;
                        }
                        else {
                            s_DevCtr.abyCodeStr[iIndex ++] = BARCODE_CHAR[j][1];
                        }
                    }
                    else {
                        s_DevCtr.abyCodeStr[iIndex ++] = BARCODE_CHAR[j][1];
                    }
                }

                break;
            }
        }

        i += 2;
    }

    return iIndex;
}

/*******************************************************************************
函数功能：
*   @ PS2用于接收总线数据
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void KernelProg(unsigned int dwPare)
{
    unsigned int dwLen = 0;
    unsigned char byLastCode = 0;

    if(*s_DevCtr.pwdClkGpio) return;

    if((s_DevCtr.byIntCnt > 0) && (s_DevCtr.byIntCnt < 9)) {
        s_DevCtr.byCode >>= 1;                                   // 因键盘数据是低>>高，结合上一句所以右移一位
        if(*s_DevCtr.pwdSdaGpio == 1) s_DevCtr.byCode |= 0x80;   //若数据引脚为高电平输入
    }

    s_DevCtr.byIntCnt ++;
    if(s_DevCtr.byIntCnt > 10) {
        // 当中断10次后表示一帧数据收完，清变量准备下一次接收
        s_DevCtr.byIntCnt = 0;

        if(s_DevCtr.byCodeCnt == 0) memset(s_DevCtr.abyCodeStr, 0, BARCODE_RX_BUFF);
        if(s_DevCtr.byCodeCnt) byLastCode = s_DevCtr.abyCodeStr[s_DevCtr.byCodeCnt - 1];
        s_DevCtr.abyCodeStr[s_DevCtr.byCodeCnt ++] = s_DevCtr.byCode;

        if(s_DevCtr.byCodeCnt > BARCODE_RX_BUFF - 2) {
            s_DevCtr.byCodeCnt = 0;
            return;
        }

        if(s_DevCtr.byCode == 0x5A && byLastCode == 0xf0) {
            // 5A:ENTER, 14:L CTRL//结束
            // 换行
            if(s_DevCtr.byCodeCnt >= 3) s_DevCtr.byCodeCnt -= 3;

            DevIo->IrpDown( s_DevCtr.byIrpChn, 1);
            dwLen = DeCode(s_DevCtr.byCodeCnt);

            if(s_DevCtr.pfPS2Bckhandle) s_DevCtr.pfPS2Bckhandle (s_DevCtr.abyCodeStr, dwLen);
            //清除标志
            s_DevCtr.byCodeCnt = 0;
            s_DevCtr.byIntCnt = 0;
            DevIo->IrpUp( s_DevCtr.byIrpChn, 1);
        }
    }
}

/*******************************************************************************
*函数功能：
*   @ 直接触发ps2数据
*参数描述：
*   @ 初始化参数
*返回值描述：
*   @ 设备的操作集合
*说明：
*   @ 无
*******************************************************************************/
static void Report(unsigned char *pbyIn)
{
    unsigned char abyChar[24];
    int iLen = 0;

    if(s_DevCtr.pfPS2Bckhandle) {
        memset(abyChar, 24, 0);
        for( iLen = 0; iLen < 24 && pbyIn[iLen] != 0; iLen ++) {
            abyChar[iLen] = pbyIn[iLen];
        }

        s_DevCtr.pfPS2Bckhandle((const char*)abyChar, iLen);
    }
}

#if(CONFIG_PS2_CTR)
/*******************************************************************************
*函数功能：
*   @ 关闭PS2设备
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void Down(void)
{
    DevIo->IrpDown( s_DevCtr.byIrpChn, 1);
}

/*******************************************************************************
*函数功能：
*   @ 启动PS2设备
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void Up(void)
{
    s_DevCtr.byCodeCnt = 0;
    s_DevCtr.byIntCnt = 0;

    memset(s_DevCtr.abyCodeStr, 0, BARCODE_RX_BUFF);

    DevIo->IrpUp( s_DevCtr.byIrpChn, 1);
}
#endif

static const DevPs2Tdf s_Dev = {
    SetIrq,
    Report,
#if(CONFIG_PS2_CTR)
    Down,
    Up,
#endif
};

/*******************************************************************************
函数功能：
*   @ 初始化
参数描述：
*   @ 初始化参数
返回值描述：
*   @ 设备的操作集合
说明：
*   @ 无
*******************************************************************************/
const DevPs2Tdf *Ps2Init(DevPs2InitTdf *pstInit)
{
    memset(&s_DevCtr, 0, sizeof(s_DevCtr));

    s_DevCtr.pwdClkGpio = pstInit->pwdClkGpio;
    s_DevCtr.pwdSdaGpio = pstInit->pwdSdaGpio;
    s_DevCtr.byIrpChn = pstInit->byClkGpioChn;
    DevIo->AddIrp(pstInit->byClkGpioNo, pstInit->byClkGpioChn, Trigger_Falling, KernelProg);

#if(CONFIG_PS2_CTR)
    Down();
#endif

	return &s_Dev;
}

#endif