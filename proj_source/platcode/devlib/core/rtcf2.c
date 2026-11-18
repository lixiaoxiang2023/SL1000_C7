/*******************************************************************************
文件名称：   rtc.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.12
程序用途：   用于STM32的RTC时钟驱动

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
2016.03.12：
第一次由XQ创建初始程序。
*******************************************************************************/
/*******************************************************************************
Include file
*******************************************************************************/
#include "dev.h"

#ifdef PLAT_STM32_F2
#if(CONFIG_RTC)
/*******************************************************************************
常量定义
*******************************************************************************/

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static const unsigned char c_byDaysOfMonth[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static struct {
    void (*RtcHandler) (void);

#if(CONFIG_RTC_ALARM)
    void (*RtcAlarmAHandler) (void);
    void (*RtcBlarmAHandler) (void);
#endif
#if(CONFIG_RTC_TAMPER)
    void (*RtcTamperHandler) (void);
#endif

}s_DevCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 判断是否是闰年
参数描述：
*   @ unsigned int dwYear：年份
返回值描述：
*   @ true：是
*   @ false ：否
说明：
*   @ 无
*******************************************************************************/
static bool IsLeapYear(unsigned int dwYear)
{
    if(dwYear % 400 == 0) return true;
    if(dwYear % 4 != 0) return false;
    if(dwYear % 100 == 0) return false;

    return true;
}

/*******************************************************************************
*函数功能：
*   @ 转成bcd码
*参数描述：
*   @ unsigned char byIn：输入
*返回值描述：
*   @ bcd
*说明：
*   @ 无
*******************************************************************************/
static unsigned char Byte2Bcd(unsigned char byIn)
{
    unsigned char bcdhigh = 0;

    while (byIn >= 10)
    {
        bcdhigh++;
        byIn -= 10;
    }

    return  ((unsigned char)(bcdhigh << 4) | byIn);
}

/*******************************************************************************
*函数功能：
*   @ bcd成值
*参数描述：
*   @ unsigned char byIn：输入
*返回值描述：
*   @ 数值
*说明：
*   @ 无
*******************************************************************************/
static unsigned char Bcd2ToByte(unsigned char byIn)
{
  unsigned char tmp = 0;

  tmp = ((unsigned char)(byIn & (unsigned char)0xF0) >> (unsigned char)0x4) * 10;
  return (tmp + (byIn & (unsigned char)0x0F));
}

/*******************************************************************************
函数功能：
*   @ 判断输入的RTC时间是否正确
参数描述：
*   @ stRtcTimTdf *pstTim：时间数据
返回值描述：
*   @ true：正确
*   @ false ：错误
说明：
*   @ 无
*******************************************************************************/
static bool IsTime( stRtcTimTdf *pstTim )
{
    if(pstTim->wYear < 2000 || pstTim->wYear > 2099) return false;
    if(pstTim->byMonth > 12 || pstTim->byMonth == 0) return false;
    if(pstTim->byDay > 31 || pstTim->byDay == 0) return false;

    if(pstTim->byHour > 23) return false;
    if(pstTim->byMinute > 59) return false;
    if(pstTim->bySecond > 59) return false;

    if(pstTim->byMonth == 2) {
        if(IsLeapYear(pstTim->wYear)) {
            if(pstTim->byDay > 29) return false;
        }
        else {
            if(pstTim->byDay > 28) return false;
        }

        return true;
    }

    if(pstTim->byDay > c_byDaysOfMonth[pstTim->byMonth]) return false;
    return true;
}

/*******************************************************************************
*函数功能：
*   @ 根据日期计算周几
*参数描述：
*   @ 年月日
*返回值描述：
*   @ 1 周1，2周2
*说明：
*   无
*******************************************************************************/
static unsigned int WeekDay( unsigned int iYear, unsigned int iMonth, unsigned int iDay )
{
    int iWeek = 0;
    unsigned int y = 0, c = 0, m = 0, d = 0;

    if ( iMonth == 1 || iMonth == 2 ) {
        c = ( iYear - 1 ) / 100;
        y = ( iYear - 1 ) % 100;
        m = iMonth + 12;
        d = iDay;
    }
    else {
        c = iYear / 100;
        y = iYear % 100;
        m = iMonth;
        d = iDay;
    }

    iWeek = y + y / 4 + c / 4 - 2 * c + 26 * ( m + 1 ) / 10 + d - 1;    //蔡勒公式
    iWeek = iWeek >= 0 ? ( iWeek % 7 ) : ( iWeek % 7 + 7 );    //iWeek为负时取模

    if ( iWeek == 0 ) iWeek = 7;   //星期日不作为一周的第一天
    return iWeek;
}

/*******************************************************************************
*函数功能：
*   @ 保护模式的开关
*参数描述：
*   @ 无
*返回值描述：
*   @  无
*说明：
*   @ 无
*******************************************************************************/
static void ProtectionCmd(bool blProtOn)
{
    if (!blProtOn) RTC->WPR = 0xFF;
    else {
        RTC->WPR = 0xCA;
        RTC->WPR = 0x53;
    }
}

/*******************************************************************************
*函数功能：
*   @ 进入初始化模式
*参数描述：
*   @ 无
*返回值描述：
*   @  无
*说明：
*   @ 无
*******************************************************************************/
static int EnterInitMode(void)
{
    unsigned int dwTimOut = 0;

    if(RTC->BitISR.INITF != 0) return 0;
    RTC->BitISR.INIT = 1;

    while(RTC->BitISR.INITF == 0) {
        if(++ dwTimOut == 0x8000) break;
    }

    if (dwTimOut == 0x8000) return -1;
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 退出初始化模式
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static int ExitInitMode(void)
{
    RTC->BitISR.INIT = 0;
    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 同步
*参数描述：
*   @ 无
*返回值描述：
*   @  无
*说明：
*   @ 无
*******************************************************************************/
static int Synchro( void )
{
    unsigned int dwTimOut = 0;

    ProtectionCmd(true);

    RTC->BitISR.RSF = 0;
    RTC->BitISR.INIT = 0;

    while(RTC->BitISR.RSF == 0) {
        if(++ dwTimOut == 0x8000) break;
    }

    ProtectionCmd(false);
    if (dwTimOut == 0x8000) return -1;

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 设置RTC时间
参数描述：
*   @ stRtcTimTdf *pstTim：时间数据
返回值描述：
*   @  0 ：成功
*   @ -1 ：时间格式错误
说明：
*   @ 无
*******************************************************************************/
static int SetTime(stRtcTimTdf *pstTim)
{
    stRtcTimTdf stTim = *pstTim;
    unsigned int dwReg = 0;

    if(!IsTime(pstTim)) return -1; //输入时间格式不对没问题

    stTim.wYear     = Byte2Bcd(stTim.wYear-2000);
    stTim.byMonth   = Byte2Bcd(stTim.byMonth);
    stTim.byDay     = Byte2Bcd(stTim.byDay);

    stTim.byHour    = Byte2Bcd(stTim.byHour);
    stTim.byMinute  = Byte2Bcd(stTim.byMinute);
    stTim.bySecond  = Byte2Bcd(stTim.bySecond);

    stTim.byWeek = WeekDay(pstTim->wYear, pstTim->byMonth, pstTim->byDay);
    stTim.byWeek  = Byte2Bcd(stTim.byWeek);

    ProtectionCmd(true);
    EnterInitMode();

    dwReg = (unsigned int)(((unsigned int)stTim.wYear << 16) | \
              ((unsigned int)stTim.byMonth << 8) | \
              ((unsigned int)stTim.byDay) | \
              ((unsigned int)stTim.byWeek << 13));

    RTC->DR = dwReg;

    dwReg = (unsigned int)(((unsigned int)stTim.byHour << 16) | \
              ((unsigned int)stTim.byMinute << 8) | \
              ((unsigned int)stTim.bySecond));

    RTC->TR = dwReg;
    ExitInitMode();
    ProtectionCmd(false);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取RTC 并将时间信息
参数描述：
*   @ stRtcTimTdf *pstTim：时间数据数据输出
返回值描述：
*   @  0 ：成功
*   @ -1 ：出错
说明：
*   @ 无
*******************************************************************************/
static int GetTim(stRtcTimTdf *pstTim)
{
    if(!pstTim) return -1;

    Synchro();

    pstTim->byHour   = Bcd2ToByte((unsigned char)RTC->BitTR.H);     	  //小时
	pstTim->byMinute = Bcd2ToByte((unsigned char)RTC->BitTR.MN);        //分钟
	pstTim->bySecond = Bcd2ToByte((unsigned char)RTC->BitTR.S);         //秒钟

    pstTim->wYear   = Bcd2ToByte((unsigned char)RTC->BitDR.Y)+2000;
    pstTim->byMonth = Bcd2ToByte((unsigned char)RTC->BitDR.M);
    pstTim->byDay   = Bcd2ToByte((unsigned char)RTC->BitDR.D);
    pstTim->byWeek  = Bcd2ToByte((unsigned char)RTC->BitDR.WDU);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取RTC 并将时间信息计数
参数描述：
*   @ 无
返回值描述：
*   @  0 ：成功
*   @ -1 ：出错
说明：
*   @ 无
*******************************************************************************/
static unsigned int GetTimCnt(void)
{
    stRtcTimTdf stTim;
    unsigned short int wTemp;
	unsigned int dwSecCount = 0;

    GetTim(&stTim);

    for (wTemp = 2000; wTemp < stTim.wYear; wTemp++) {  //把2000年作为起点，所有年份的秒钟相加
        if (IsLeapYear(wTemp)) dwSecCount += 31622400;	    //闰年的秒钟数
        else dwSecCount += 31536000; //平年的秒钟数
    }

    if(stTim.byMonth) stTim.byMonth --;
    if(stTim.byMonth > 1) {
        for (wTemp = 0; wTemp < stTim.byMonth - 1; wTemp ++) { //把前面月份的秒钟数相加
            if(wTemp == 1) { //闰年2月份增加一天的秒钟数
                if( IsLeapYear(stTim.wYear) ) dwSecCount += (86400 * 29);
                else dwSecCount += (86400 * 28);
                continue;
            }

            dwSecCount += (86400 * c_byDaysOfMonth[wTemp]);
        }
    }

    if(stTim.byDay) stTim.byDay --;
    dwSecCount += (unsigned int)stTim.byDay * 86400;        //把前面日期的秒钟数相加
    dwSecCount += (unsigned int)stTim.byHour * 3600;	      //小时秒钟数
    dwSecCount += (unsigned int)stTim.byMinute * 60;        //分钟秒钟数
    dwSecCount += stTim.bySecond;			                  //最后的秒钟加上去

    return dwSecCount;
}

/*******************************************************************************
函数功能：
*   @ 读取RTC 并将时间信息，并转换成相应格式的字符串
参数描述：
*   @ emTimTypeTdf enType：时间格式
*   @ stRtcTimTdf *pstTim,：为外部数据进行转换
*   @ char *pcOut ： 数据输出缓冲
返回值描述：
*   @  0 ：成功
*   @ -1 ：出错
说明：
*   @ 无
*******************************************************************************/
static int GetTimStr(emTimTypeTdf enType, stRtcTimTdf *pstTim, char *pcOut)
{
    stRtcTimTdf stTim;

    if(pstTim) stTim = *pstTim;
    else GetTim(&stTim);
    if(!pcOut) return -1;

    switch(enType) {
    case HHMMSS:
        sprintf(pcOut,"%02d:%02d:%02d", stTim.byHour, stTim.byMinute, stTim.bySecond);
        break;

    case HHMM:
        sprintf(pcOut,"%02d:%02d", stTim.byHour, stTim.byMinute);
        break;

    case TTHHMM:
        if(stTim.byHour < 12) sprintf(pcOut,"AM %02d:%02d", stTim.byHour, stTim.byMinute);
        else if(stTim.byHour == 12) sprintf(pcOut,"PM %02d:%02d", stTim.byHour, stTim.byMinute);
        else sprintf(pcOut,"PM %02d:%02d", stTim.byHour - 12, stTim.byMinute);
        break;

    default: break;
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 读取RTC 并将日期信息，并转换成相应格式的字符串
参数描述：
*   @ emDateTypeTdf enType：日期格式
*   @ stRtcTimTdf *pstTim,：为外部数据进行转换
*   @ char *pcOut ： 数据输出缓冲
返回值描述：
*   @  0 ：成功
*   @ -1 ：出错
说明：
*   @ 无
*******************************************************************************/
static int GetDateStr(emDateTypeTdf enType, stRtcTimTdf *pstTim, char *pcOut)
{
    stRtcTimTdf stTim;

    if(pstTim) stTim = *pstTim;
    else GetTim(&stTim);
    if(!pcOut) return -1;

    switch(enType) {
    case YYMMDD:
        sprintf(pcOut,"%04d-%02d-%02d", stTim.wYear, stTim.byMonth, stTim.byDay);
        break;

    case DDMMYY:
        sprintf(pcOut,"%02d-%02d-%04d", stTim.byDay, stTim.byMonth, stTim.wYear);
        break;

    default: break;
    }

    return 0;
}

/*******************************************************************************
*函数功能：
*   @ 设置秒中断使能
*参数描述：
*   @ 无
*返回值描述：
*   @  无
*说明：
*   @ 无
*******************************************************************************/
static void SecItUp( void )
{
    ProtectionCmd(true);
    RTC->BitCR.WUTE = 1;
    RTC->BitCR.WUTIE = 1;
    ProtectionCmd(false);

    RTC->BitISR.WUTF = 0;
    DevIo->NvicUp(RTC_WKUP_IRQn, 10, 0);
    DevIo->AddIrp(NULL, 22, Trigger_Rising, 0);
}

/*******************************************************************************
*函数功能：
*   @ 设置秒中断关闭
*参数描述：
*   @ 无
*返回值描述：
*   @  无
*说明：
*   @ 无
*******************************************************************************/
static void SecItDown( void )
{
    RTC->BitCR.WUTE = 0;
    RTC->BitCR.WUTIE = 0;

    RTC->BitISR.WUTF = 0;
    DevIo->NvicDown(RTC_WKUP_IRQn);
}

/*******************************************************************************
函数功能：
*   @ 设置秒中断回调函数
参数描述：
*   @ void (* pHandler) (void) ：中断回调函数接口
返回值描述：
*   @  无
说明：
*   @ 无
*******************************************************************************/
static int SetSecHandler(void (* pHandler) (void))
{
    s_DevCtr.RtcHandler = pHandler;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ RTC配置初始化硬件，设定为外部震荡时钟，分频为32767
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void Config(void)
{
    stRtcTimTdf stTim ;

    RCC->BitBDCR.LSEON = 0;
    RCC->BitBDCR.LSEBYP = 0;

    RCC->BitBDCR.LSEON = 1;         //使能外部低速时钟
    while(RCC->BitBDCR.LSERDY == 0) {;}

    RCC->BitBDCR.TRCSEL = 1;        //RTC时钟选择为LSE
    RCC->BitBDCR.RTCEN = 1;         //使能RTC时钟
    Synchro();

    ProtectionCmd(true);
    EnterInitMode();

    // 分频
    RTC->BitPRER.PREDIV_S = 0xFF;
    RTC->BitPRER.PREDIV_A = 0x7F;
    RTC->BitCR.FMT = 0;

    RTC->WUTR = 0x07ff;
    RTC->BitCR.OSEL = 0; // 使用16分屏 32768 = 0x07ff * 16,刚好为1HZ

    ExitInitMode();
    ProtectionCmd(false);
       
    // bkp sram
    PWR->BitCSR.BRE = 1;
    while(PWR->BitCSR.BRR == 0) {;}

    //初始化日期设置 15年01月01日 星期五
    stTim.wYear     =  2015;
    stTim.byMonth   =  1;
    stTim.byDay     =  1;
    //初始化时间设置  00:00:00
    stTim.byHour    =  0;
    stTim.byMinute  =  0;
    stTim.bySecond  =  0;
    
    SetTime(&stTim); //设置初始化时间
}

/*******************************************************************************
*函数功能：
*   @ 写后备寄存器
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static void WrBkpReg(unsigned int dwBkpDr, unsigned int wDat)
{
    __IO unsigned int tmp = 0;

    tmp = RTC_BASE + 0x50;
    tmp += (dwBkpDr * 4);

    *(__IO unsigned int *)tmp = (unsigned int)wDat;
}

/*******************************************************************************
*函数功能：
*   @ 读后备寄存器
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
static unsigned int RdBkpReg(unsigned int dwBkpDr)
{
    __IO unsigned int tmp = 0;

    tmp = RTC_BASE + 0x50;
    tmp += (dwBkpDr * 4);

    return (*(__IO unsigned int *)tmp);
}

/*******************************************************************************
*函数功能：
*   @ RTC实时时钟中断
*参数描述：
*   @ 无
*返回值描述：
*   @ 无
*说明：
*   @ 无
*******************************************************************************/
void RTC_WKUP_IRQHandler(void)
{
    // 秒中断
    if(RTC->BitCR.WUTE == 1 || RTC->BitCR.WUTIE == 1) {
        if(RTC->BitISR.WUTF) {
            RTC->BitISR.WUTF = 0;
            EXTI->BitPR.PR22 = 1;
            if(s_DevCtr.RtcHandler) s_DevCtr.RtcHandler();
        }
    }

#if(CONFIG_RTC_ALARM)
    // 闹钟
    if(RTC->BitCR.ALRAIE == 1 || RTC->BitCR.ALRAE == 1 || RTC->BitCR.ALRBIE == 1 || RTC->BitCR.ALRBE == 1) {
        if(RTC->BitISR.ALRAF) {
            RTC->BitISR.WUTF = 0;
            EXTI->BitPR.PR17 = 1;
            if(s_DevCtr.RtcHandler) s_DevCtr.RtcAlarmAHandler();
        }

        if(RTC->BitISR.ALRBF) {
            RTC->BitISR.ALRBF = 0;
            EXTI->BitPR.PR17 = 1;
            if(s_DevCtr.RtcHandler) s_DevCtr.RtcBlarmAHandler();
        }
    }
#endif

#if(CONFIG_RTC_TAMPER)
    // 时间戳
    if(RTC->BitCR.TSE == 1 || RTC->BitCR.TSIE == 1) {
        if(RTC->BitISR.TSF) {
            RTC->BitISR.TSF = 0;
            EXTI->BitPR.PR21 = 1;
            if(s_DevCtr.RtcHandler) s_DevCtr.RtcTamperHandler();
        }
    }
#endif
}

static const DevRtcTdf s_Dev = {
    IsLeapYear,
    IsTime,
    SetTime,
    GetTim,
    GetTimCnt,
    GetTimStr,
    GetDateStr,
    SecItUp,
    SecItDown,
    SetSecHandler,
    Config,
    WrBkpReg,
    RdBkpReg,
};

/*******************************************************************************
函数功能：
*   @ RTC设备初始化
参数描述：
*   @ 无
返回值描述：
*   @ RTC操作方法集合
说明：
*   @ 无
*******************************************************************************/
const DevRtcTdf *RtcInit(void)
{
    RCC->BitAPB1ENR.PWREN = 1;
    PWR->BitCR.DBP = 1;             // 启动可读写
    RCC->BitAHB1ENR.BKPSRAMEN = 1;  // 使能BKPSRAM时钟

    if(RTC->BitISR.INITS==0) Config();

    Synchro();
    return &s_Dev;
}
#endif
#endif