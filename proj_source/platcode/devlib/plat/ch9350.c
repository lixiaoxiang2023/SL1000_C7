/*******************************************************************************
文件名称：     ch9350.c
目录：         plat/code/stdlib
项目名称：     事业一部技术平台
创建者：       xuqiang
创建日期：     20151123
程序用途：

版权声明：
Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
20200914：
第一次由马德嘉创建初始程序。
由 修改 标准化并加入技术平台
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_CH9350)
/*******************************************************************************
常量定义
*******************************************************************************/
#define CH9350_RX_BUFF_SIZE  600
#define CODE_BUFF_SIZE  100
/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
const u8 c_byaPS2KeyCode[][3] =      // 键盘通码表，与键盘按键表一一对应
{
  {0x04,	'a', 'A'},
  {0x05,	'b', 'B'},
  {0x06,	'c', 'C'},
  {0x07,	'd', 'D'},
  {0x08,	'e', 'E'},
  {0x09,	'f', 'F'},
  {0x0a,	'g', 'G'},
  {0x0b,	'h', 'H'},
  {0x0c,	'i', 'I'},
  {0x0d,	'j', 'J'},
  {0x0e,	'k', 'K'},
  {0x0f,	'l', 'L'},
  {0x10,	'm', 'M'},
  {0x11,	'n', 'N'},
  {0x12,	'o', 'O'},
  {0x13,	'p', 'P'},
  {0x14,	'q', 'Q'},
  {0x15,	'r', 'R'},
  {0x16,	's', 'S'},
  {0x17,	't', 'T'},
  {0x18,	'u', 'U'},
  {0x19,	'v', 'V'},
  {0x1a,	'w', 'W'},
  {0x1b,	'x', 'X'},
  {0x1c,	'y', 'Y'},
  {0x1d,	'z', 'Z'},
  {0x1e,	'1', '!'},
  {0x1f,	'2', '@'},
  {0x20,	'3', '#'},
  {0x21,	'4', '$'},
  {0x22,	'5', '%'},
  {0x23,	'6', '^'},
  {0x24,	'7', '&'},
  {0x25,	'8', '*'},
  {0x26,	'9', '('},
  {0x27,	'0', ')'},
  {0x2d,	'-', '_'},
  {0x2e,	'=', '+'},
  {0x2f,	'[', '{'},
  {0x30,	']', '}'},
  {0x33,	';', ':'},
  {0x35,	'`', '~'},
  {0x36,	',', '<'},
  {0x37,	'.', '>'},
  {0x38,	'/', '?'},

};


static struct {
  unsigned char byUartSn;//串口号
  unsigned char byTimNO;
  bool blRxEn;//允许接收
  unsigned char abyRxbuf[CH9350_RX_BUFF_SIZE];
  u8  abyDeDat[CODE_BUFF_SIZE];//最终解码的数据
  u16 wRxCnt;  //接收计数器
  u8  byCnt;//
  u32 dwTimeCnt;
  bool blStarTime;
  bool blBusy;
}s_DevCtr;

static void (*pfBckhandle) (const char *p );     //  回调函数

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
*函数功能：
*   @ 开始接收数据
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void StartRx(void)
{
  memset(&s_DevCtr.abyDeDat, 0 , sizeof(s_DevCtr.abyDeDat));
  memset(&s_DevCtr.abyRxbuf, 0 , sizeof(s_DevCtr.abyRxbuf));
  s_DevCtr.wRxCnt = 0;  //接收计数器
  s_DevCtr.blRxEn = true;
  s_DevCtr.blStarTime = false;
  s_DevCtr.dwTimeCnt = 0;
}
/*******************************************************************************
函数功能：
*   @ 对ds18b20复位时需要的定时器中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void TimeHandler(unsigned int Hwnd)
{
  s_DevCtr.dwTimeCnt ++;
  if(s_DevCtr.dwTimeCnt >=0x2fff){
    s_DevCtr.blRxEn = false;
    DevTim->Stop(s_DevCtr.byTimNO);
    s_DevCtr.byCnt++;
    s_DevCtr.wRxCnt = 0;
  }
}
/*******************************************************************************
*函数功能：
*   @ 蓝牙数据接收处理模块
*参数描述：
*   @ unsigned char byIn 底层驱动传入的数据
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void RxHandler(unsigned char byIn)
{
  s_DevCtr.blBusy = true;
  if(s_DevCtr.wRxCnt < CH9350_RX_BUFF_SIZE && s_DevCtr.blRxEn == true)
  {
    if(!s_DevCtr.blStarTime ){//打开定时器
      s_DevCtr.blStarTime = true;
      DevTim->TimEvent(s_DevCtr.byTimNO, 60, TimeHandler, 0);//限定接收时间，并增加接收计数
    }
    s_DevCtr.abyRxbuf[s_DevCtr.wRxCnt++] = byIn;

  }
}
/*******************************************************************************
*函数功能：
*   @ 蓝牙数据接收处理模块
*参数描述：
*   @ unsigned char byIn 底层驱动传入的数据
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static u8 ReturnCnt(void)
{
  return s_DevCtr.byCnt ;
}
/*******************************************************************************
参数描述  ：扫码处理函数
@ u8 byFrameNum -- 当前帧序号
返回值描述：
@ 0  -- 已接收到结束字符
@ -1 -- 未接收到结束字符
说明      ：
@ 在帧间隔时间内处理 UART 数据，将 UART 帧数据转换为 PS/2 数据
返回值用于标识键值是否为 Enter 键值（结束字符）
特殊算法  ：
@ 根据 CH9350 的数据格式， ，故正常情况下
 比如  0x02 0x02 0x00 0x10  M
       大写           M解码
@ 特殊情况：由于最后一帧数据固定为 Enter 键值（结束字符），且在倒数第二帧
就可以收到结束字符，为了提高程序处理效率，结束字符的处理放在倒数第二帧和
最后一帧的间隔里。
注意事项  ：
@ 本函数对无意义的数据帧不做处理（如表示左 Shift 键值的帧）
*******************************************************************************/
bool DecodeProcess(u8 *pby)
{
    u8  *pbyRxbuf;
    u8  *pbyCode;
    char *pCode;
    char abyCode[20]={0};
    char cChar = 0;
    bool blFind = false;
    u16 wKeyCodeLen =0;
    u16 wCodeLen =0;
    u32 cnt =0;
    wKeyCodeLen = sizeof(c_byaPS2KeyCode)/sizeof(c_byaPS2KeyCode[0]);//数组长度

    pbyCode  = s_DevCtr.abyDeDat;
    pbyRxbuf = s_DevCtr.abyRxbuf;
    pCode = abyCode;

    for(int i=0;i<CH9350_RX_BUFF_SIZE;i++)
    {
        if(*pbyRxbuf==0x57){//提取键码
            pbyRxbuf +=5;
            *pbyCode++ = *pbyRxbuf;
            pbyRxbuf +=2;
            *pbyCode++ = *pbyRxbuf;
            wCodeLen +=2;
        }
        else pbyRxbuf++;

        if(*pbyRxbuf== 0x28&& *(pbyRxbuf+1)==0){//结束符
            i++;
            break;
        }
    }

    pbyCode  = s_DevCtr.abyDeDat;

    for(int i=0;i<wCodeLen;){//解码 转化为字符串
        blFind = false;
        if(*(pbyCode+i) == 0 &&*(pbyCode+i+1) == 0\
            &&*(pbyCode+i+2) == 0x00 &&*(pbyCode+i+3)== 0x00)break;

        if(*(pbyCode+i) == 0x02 &&*(pbyCode+i+1) == 0x00\
            &&*(pbyCode+i+2) == 0x00 &&*(pbyCode+i+3) == 0x00 ){ //SHIFT
                i+=4;
                continue;
            }

        if(*(pbyCode+i) == 0x02 &&*(pbyCode+i+2) == 0x02){ //大写
            for(int j=0;j<wKeyCodeLen;j++){
                if(c_byaPS2KeyCode[j][0] == *(pbyCode+i+3)){//查表
                    cChar =  c_byaPS2KeyCode[j][2];
                    blFind = true;
                    break;
                }
            }
        }
        else {//
            for(int j=0;j<wKeyCodeLen;j++){
                if(c_byaPS2KeyCode[j][0] == *(pbyCode+i+1)){
                    cChar = c_byaPS2KeyCode[j][1];
                    blFind = true;
                    break;
                }
            }
        }

        if(blFind) {
            *pby++ = cChar;
            *pCode++ = cChar;
        }
        i+=4;
    }

    if( pfBckhandle)  pfBckhandle((char const*)abyCode );//插入队列

    while(1){//等待空闲再接收数据
        if(s_DevCtr.blBusy){
            s_DevCtr.blBusy = false;
            cnt = 0;
        }

        cnt++;
        if(cnt>0x1ffff)break;
    }

    StartRx();
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 发送禁止应答指令
*参数描述：
*   @ 无
*返回值描述：
*   @ true 设备存在
*   @ false 设备不存在
*说明：
*   @ 无
*******************************************************************************/
void SendNoAck(void)
{
  const u8 abyNoAck[]={0x57, 0xAB, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x20};//取消应答指令
  DevUart->SendString(s_DevCtr.byUartSn,(unsigned char const*)abyNoAck,11  );
  for(int i=0;i<0xfffff;i++);
  StartRx();
}
/*******************************************************************************
*函数功能：
*   @ 发送禁止应答指令
*参数描述：
*   @ 无
*返回值描述：
*   @ true 设备存在
*   @ false 设备不存在
*说明：
*   @ 无
*******************************************************************************/
static void SetHander( void (*pHander)(const char *p ))
{
  pfBckhandle = pHander;
}


static const DevCh9350Tdf s_Dev = {
  StartRx ,
  SendNoAck,
  DecodeProcess,
  SetHander,
  ReturnCnt,
};

/*******************************************************************************
*函数功能：
*   @ 初始化Bt模块硬件，只支持设备，不支持ap
*参数描述：
*   @ 初始化参数
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
const DevCh9350Tdf *Ch9350DevInit(DevCh9350InitTdf* pstInit)
{
  unsigned int dwPre = 0;
  ClocksTypeDef stRcc;

  DevRccGetClocksFreq(&stRcc);
  if(pstInit->byTimNO == 0 || pstInit->byTimNO == 7) dwPre = stRcc.PCLK2 / (1000*1000);
  dwPre = stRcc.PCLK1 / (1000*1000);
  dwPre  = dwPre*2-1;

  memset(&s_DevCtr, 0 , sizeof(s_DevCtr));
  s_DevCtr.byUartSn  = pstInit->byUartSn;
  s_DevCtr.byTimNO = pstInit->byTimNO;
  DevUart->Config(s_DevCtr.byUartSn, 115200, 1, 0, true);
  DevUart->RegHandler(pstInit->byUartSn, RxHandler);
  DevTim->Config(pstInit->byTimNO, dwPre, 15);

  for(__IO int i = 0; i < 0xfffff; i ++) {;}
  return &s_Dev;
}
#endif