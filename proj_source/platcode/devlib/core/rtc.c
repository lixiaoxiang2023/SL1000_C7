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

#if(PLAT_STM32_F1|PLAT_GD32_F30X)
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
static const unsigned char c_byDaysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static struct {
    void (*RtcHandler) ();
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

    if(pstTim->byDay > c_byDaysOfMonth[pstTim->byMonth - 1]) return false;
    return true;
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
	unsigned short int wTemp;
	unsigned int dwSecCount = 0;

    if(!IsTime(pstTim)) return -1; //输入时间格式不对没问题

    for (wTemp = 2000; wTemp < pstTim->wYear; wTemp++) {  //把2000年作为起点，所有年份的秒钟相加
        if (IsLeapYear(wTemp)) dwSecCount += 31622400;	  //闰年的秒钟数
        else dwSecCount += 31536000; //平年的秒钟数
    }

    if(pstTim->byMonth > 1) {
        for (wTemp = 0; wTemp < pstTim->byMonth - 1; wTemp ++) { //把前面月份的秒钟数相加
            if(wTemp == 1) { //闰年2月份增加一天的秒钟数
                if( IsLeapYear(pstTim->wYear) ) dwSecCount += (86400 * 29);
                else dwSecCount += (86400 * 28);
                continue;
            }

            dwSecCount += (86400 * c_byDaysOfMonth[wTemp]);
        }
    }

    dwSecCount += (unsigned int)(pstTim->byDay - 1) * 86400;        //把前面日期的秒钟数相加
    dwSecCount += (unsigned int)pstTim->byHour * 3600;	          //小时秒钟数
    dwSecCount += (unsigned int)pstTim->byMinute * 60;      	      //分钟秒钟数
    dwSecCount += pstTim->bySecond;			                          //最后的秒钟加上去

    //设置计数器
    while(RTC->BitCRL.RTOFF == 0) { ; } // WaitForLastTask
    RTC->BitCRL.CNF = 1;                // 进入配置模式
    RTC->CNTH = dwSecCount >> 16;
    RTC->CNTL = dwSecCount & 0x0000ffff;
    RTC->BitCRL.CNF = 0;                // 推出配置模式
    while(RTC->BitCRL.RTOFF == 0) { ; }  // WaitForLastTask

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
	unsigned int dwTimeCnt = 0;
	unsigned int dwDays = 0;
	unsigned int dwForCnt = 0;

    if(!pstTim) return -1;

    dwTimeCnt = RTC->CNTL;
	dwTimeCnt += ((unsigned int)RTC->CNTH << 16);
	dwDays = dwTimeCnt / 86400;   // 得到天数(秒钟数对应的)

    dwForCnt = 2000;  // 从2000年开始
    while (dwDays >= 365) {
        if (IsLeapYear(dwForCnt)) { // 是闰年
            if (dwDays >= 366) dwDays -= 366;
            else break;
        }
        else dwDays -= 365;       // 平年

        dwForCnt ++;
    }
    pstTim->wYear = dwForCnt;       // 得到年份

	dwForCnt = 0;
    while (dwDays >= 28) {        // 超过了一个月
        if (IsLeapYear(pstTim->wYear) && dwForCnt == 1) { //当年是不是闰年/2月份
            if(dwDays >= 29) dwDays -= 29;               //闰年的秒钟数
            else break;
        }
        else {
            if(dwDays >= c_byDaysOfMonth[dwForCnt]) dwDays -= c_byDaysOfMonth[dwForCnt];//平年
            else break;
        }

        dwForCnt ++;
    }

    pstTim->byMonth = dwForCnt + 1;  //得到月份
    pstTim->byDay = dwDays  + 1;     //得到日

	dwForCnt = dwTimeCnt % 86400;     	              //得到秒钟数
	pstTim->byHour   = dwForCnt / 3600;     	  //小时
	pstTim->byMinute = (dwForCnt % 3600) / 60; //分钟
	pstTim->bySecond = (dwForCnt % 3600) % 60; //秒钟

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
	unsigned int dwTimeCnt = 0;

	dwTimeCnt = RTC->CNTL;
	dwTimeCnt += ((unsigned int)RTC->CNTH << 16);
    return dwTimeCnt;
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
函数功能：
*   @ 设置秒中断使能
参数描述：
*   @ 无
返回值描述：
*   @  无
说明：
*   @ 无
*******************************************************************************/
static void SecItUp( void )
{
    RTC->BitCRL.SECF = 0;
    RTC->BitCRH.SECIE = 1;

    DevIo->NvicUp(RTC_IRQn, 10, 0);
}

/*******************************************************************************
函数功能：
*   @ 设置秒中断关闭
参数描述：
*   @ 无
返回值描述：
*   @  无
说明：
*   @ 无
*******************************************************************************/
static void SecItDown( void )
{
    RTC->BitCRL.SECF = 0;
    RTC->BitCRH.SECIE = 0;

    DevIo->NvicDown(RTC_IRQn);
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
    // Reset Backup Domain
    RCC->BitBDCR.BDRST = 1;
    RCC->BitBDCR.BDRST = 0;

    // Enable LSE
    RCC->BitBDCR.LSEON = 1;
    // Wait till LSE is ready
    while(RCC->BitBDCR.LSERDY == 0) {;}

    // Select LSE as RTC Clock Source
    RCC->BitBDCR.RTCSEL = 1; //RCC_RTCCLKSource_LSE
    // Enable RTC Clock
    RCC->BitBDCR.RTCEN = 1;

    // Wait for RTC registers synchronization
    while(RTC->BitCRL.RSF == 0) {;}    // 等待同步
    /* Wait until last write operation on RTC registers has finished */
    while(RTC->BitCRL.RTOFF == 0) {;}  // 等待操作完成

    RTC->PRLH = (32767 >> 16);
    RTC->PRLL = (32767 & 0xffff);

    while(RTC->BitCRL.RSF == 0) {;}    // 等待同步
    while(RTC->BitCRL.RTOFF == 0) {;}  // 等待操作完成
}

/*******************************************************************************
函数功能：
*   @ 写后备寄存器
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void WrBkpReg(unsigned int dwBkpDr, unsigned short int wDat)
{
    __IO uint32_t tmp = 0;

    tmp = (unsigned int)BKP_BASE;
    tmp += dwBkpDr;

    *(__IO uint16_t *) (BKP_BASE + dwBkpDr) = wDat;
}

/*******************************************************************************
函数功能：
*   @ 读后备寄存器
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static unsigned short int RdBkpReg(unsigned int dwBkpDr)
{
    return (*(__IO uint16_t *) (BKP_BASE + dwBkpDr));
}

/*******************************************************************************
函数功能：
*   @ RTC实时时钟中断
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
void RTC_IRQHandler(void)
{
	if (RTC->BitCRL.SECF == 1) {
        RTC->BitCRL.SECF = 0;
        if(s_DevCtr.RtcHandler) s_DevCtr.RtcHandler();
	}
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
    RCC->BitAPB1ENR.BKPEN = 1;
    RCC->BitAPB1ENR.PWREN = 1;

    PWR->BitCR.DBP = 1;                 // 启动可读写

    /*
    // 2017.11.7 取消RTC初始化里面的检查初始化，应用独立使用
    if(RdBkpReg(RTC_BKP_DR1) == 0) {
        Config();
        WrBkpReg(RTC_BKP_DR1, 1);
    }

    while(RTC->BitCRL.RSF == 0) {;}    // 等待同步
    while(RTC->BitCRL.RTOFF == 0) {;}  // 等待操作完成
    */

    return &s_Dev;
}
#endif
#endif