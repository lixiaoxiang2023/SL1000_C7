/*******************************************************************************
文件名称： tcs230.c
文件目录： $PROJ_DIR$/usr/Drive/inc
项目名称： 事业一部通用技术平台
创建者：   胥强
创建日期：
程序用途： 用于颜色传感器驱动

版权声明： Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/

/******************************************************************************
Include files 头文件
******************************************************************************/
#include "devplat.h"

#if(CONFIG_TCS230)
/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/

/*******************************************************************************
Global Variables Definition全局变量定义
*******************************************************************************/

/*******************************************************************************
Local Variables Definition内部全局变量定义
*******************************************************************************/

/*******************************************************************************
外部全局变量申明
*******************************************************************************/

/*******************************************************************************
内部数据结构与数据类型定义
*******************************************************************************/
static Tcs230InitTdf stTcs230;
static struct {
    unsigned int adwColorDat[3];
    unsigned short int wColorDatTmp;


    unsigned char byCnt;
    bool blIsBusy;
}s_stCtr;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 初始化Tcs230颜色通道
参数描述：
*   @ 所需要的颜色
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void ChooseColor(unsigned char byColor)
{
    switch(byColor)
    {
    case TCS230_R :
        if(stTcs230.pwdS2) *stTcs230.pwdS2 = 0;
        if(stTcs230.pwdS3) *stTcs230.pwdS3 = 0;
        break;
    case TCS230_G :
        if(stTcs230.pwdS2) *stTcs230.pwdS2 = 1;
        if(stTcs230.pwdS3) *stTcs230.pwdS3 = 1;
        break;
    case TCS230_B :
        if(stTcs230.pwdS2) *stTcs230.pwdS2 = 0;
        if(stTcs230.pwdS3) *stTcs230.pwdS3 = 1;
        break;
    case TCS230_W :
        if(stTcs230.pwdS2) *stTcs230.pwdS2 = 1;
        if(stTcs230.pwdS3) *stTcs230.pwdS3 = 0;
        break;
    default :
        if(stTcs230.pwdS2) *stTcs230.pwdS2 = 1;
        if(stTcs230.pwdS3) *stTcs230.pwdS3 = 0;
        break;
    }
}

/*******************************************************************************
函数功能：
*   @ 定时器捕获通道中断回调接口
参数描述：
*   @ 发生捕获的定时器捕获通道号
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void TimIRQHandler(unsigned int byCcrChn)
{
    s_stCtr.byCnt ++;

    if(s_stCtr.byCnt == 2) {
        s_stCtr.wColorDatTmp = DevTim->ReadCcrm(stTcs230.byTimNO, stTcs230.byTimCcmrChn);
    }
    else if(s_stCtr.byCnt == 9) {
        DevTim->StopCcmr(stTcs230.byTimNO, stTcs230.byTimCcmrChn); //关闭捕获
        s_stCtr.wColorDatTmp = DevTim->ReadCcrm(stTcs230.byTimNO, stTcs230.byTimCcmrChn) - s_stCtr.wColorDatTmp;
        s_stCtr.wColorDatTmp >>= 3;
        s_stCtr.blIsBusy = false;
    }
}

/*******************************************************************************
函数功能：
*   @ 颜色处理将RGB转成HSL
参数描述：
*   @ const unsigned char* abyRGB：RGB数据输入
*   @ unsigned short int *pwHsl：HSL数据输入
返回值描述：
*   @ 通过输入地址返回hsl数据
说明：
*   @ 无
*******************************************************************************/
static void Rgb2Hsl(const unsigned char* abyRGB, unsigned short int *pwHsl)
{
    unsigned char abyTemp[3];
    unsigned char byTemp = 0;
    short anHSL[3];

    memcpy(abyTemp, abyRGB, 3);

    //RGB大小排序
    if(abyTemp[0] < abyTemp[1]) {
        byTemp = abyTemp[1];
        abyTemp[1] = abyTemp[0];
        abyTemp[0] = byTemp;
    }

    if(abyTemp[1] < abyTemp[2]) {
        byTemp = abyTemp[2];
        abyTemp[2] = abyTemp[1];
        abyTemp[1] = byTemp;
    }

    if(abyTemp[0] < abyTemp[1]) {
        byTemp = abyTemp[1];
        abyTemp[1] = abyTemp[0];
        abyTemp[0] = byTemp;
    }

    anHSL[2] = ((abyTemp[0] + abyTemp[2]) * 100 + 255) / 510;

    //计算S
    if(abyTemp[0] == abyTemp[2]) {  //最大最小相同
        anHSL[0] = 0;
        anHSL[1] = 0;
    }
    else {
        if(anHSL[2] < 50) {
            anHSL[1] = (short)((float)(abyTemp[0] - abyTemp[2]) * 100 / (abyTemp[0] + abyTemp[2]) + 0.5);       // L<0.5, S=(maxcolor-mincolor)/(maxcolor + mincolor)
        }
        else {
            anHSL[1] = (short)((float)(abyTemp[0] - abyTemp[2]) * 100 / (510 - abyTemp[0] - abyTemp[2]) + 0.5); // L>=0.5, S=(maxcolor-mincolor)/(2.0-maxcolor-mincolor)
        }
    }

    //计算H   H = H * 60;
    if(abyRGB[0] == abyTemp[0]) {
        anHSL[0] = (short)((float)(abyRGB[1] - abyRGB[2]) * 60 / (abyTemp[0] - abyTemp[2]) + 0.5); //If R=maxcolor, H=(G-B)/(maxcolor-mincolor)
    }
    else if(abyRGB[1] == abyTemp[0]) {
        anHSL[0] = 120 + (abyRGB[2] - abyRGB[0]) * 60  / (abyTemp[0] - abyTemp[2]); //If G=maxcolor, H=2.0+(B-R)/(maxcolor-mincolor)
    }
    else anHSL[0] = 240 + (abyRGB[0] - abyRGB[1]) * 60 / (abyTemp[0] - abyTemp[2]); //If B=maxcolor, H=4.0+(R-G)/(maxcolor-mincolor)

    if(anHSL[0] < 0) anHSL[0] += 360; //负数+360
    memcpy(pwHsl, anHSL, 6);
}

#if(0)
/*******************************************************************************
函数功能：
*   @ 颜色处理将HSL转成RGB
参数描述：
*   @ cconst short* anHSL：HSL数据输入
*   @ unsigned char* byRGB：RGB数据输入
返回值描述：
*   @ 通过输入地址返回hsl数据
说明：
*   @ 无
*******************************************************************************/
static void Hsl2Rgb(const short* anHSL, unsigned char* byRGB)
{
    unsigned char byTemp2;               //q
    unsigned char byTemp1;               //p
    unsigned char abyHk[3];

    if(anHSL[1] == 0) {                    //If S=0,表示灰色，定义R,G和B都为L.
        byRGB[0] = anHSL[2];
        byRGB[1] = anHSL[2];
        byRGB[2] = anHSL[2];
        return;
    }
    else if(anHSL[2] < 50) byTemp2 = (anHSL[2] * (100 + anHSL[1]) + 50)/100;   //If L<0.5,temp2=L*(1.0+S)
    else byTemp2 = anHSL[2] + anHSL[1] - (anHSL[2] * anHSL[1] + 50) / 100;     //If L>=0.5,temp2=L+S-L*S

    byTemp1 = 2 * anHSL[2] - byTemp2;
    abyHk[1] = (anHSL[0] * 100 + 180) / 360;                                    //Hk = h / 360;
    abyHk[0] = abyHk[1] + 33;

    if(abyHk[0] > 100) abyHk[0] = abyHk[0] - 100;

    if(abyHk[1] < 33) abyHk[2] = 67 + abyHk[1];
    else abyHk[2] = abyHk[1] - 33;

    for(unsigned char a = 0; a < 3; a++) {
        if(abyHk[a] < 17) byRGB[a] = byTemp1 + ((byTemp2 - byTemp1) * 6 * abyHk[a] + 50) / 100;
        else if(abyHk[a] < 50) byRGB[a] = byTemp2;
        else if(abyHk[a] < 67) byRGB[a] = byTemp1 + ((byTemp2 - byTemp1) * 6 * (67-abyHk[a]) + 50) / 100;
        else byRGB[a] = byTemp1;

        byRGB[a] = (byRGB[a] * 255 + 50) / 100;
    }
}
#endif

/*******************************************************************************
函数功能：
*   @ 读取一个颜色的值
参数描述：
*   @ 颜色分类：0 R；1 G；2 B；3 W
返回值描述：
*   @ 传感器输出的频率
说明：
*   @ 无
*******************************************************************************/
static unsigned int Read(unsigned char byColor)
{
    unsigned int dwTimOut = 0x2ffffff;
    double dfCal = 0;
    ChooseColor(byColor);                       //选择各滤光片
    if(stTcs230.pwdNOE) *stTcs230.pwdNOE = 0;   //低电平有效,开始输出方波

    s_stCtr.byCnt = 0;
    s_stCtr.blIsBusy = true;
    DevTim->StartCcmr(stTcs230.byTimNO, stTcs230.byTimCcmrChn, TimIRQHandler, 0);

    while(s_stCtr.blIsBusy) {
        dwTimOut --;
        if(dwTimOut == 0) break;
    }

    if(stTcs230.pwdNOE) *stTcs230.pwdNOE = 1;
    if(dwTimOut == 0) return 0;

    dfCal = 1 / ((double)s_stCtr.wColorDatTmp);
    dfCal *= 1000000;
    return (unsigned int )dfCal;
}

/*******************************************************************************
函数功能：
*   @ 读取颜色的值，包括rgb、hsl
参数描述：
*   @ unsigned int *pdwWrgb ： 白平衡的值
*   @ unsigned char *pbyRgb ： 需要输出的rgb的值，地址为0，不输出
*   @ unsigned short int *pwHsl ：需要输出的Hsl的值，地址为0，不输出
返回值描述：
*   @ -1 ： 硬件超时出错
*   @ 0  ： 成功
说明：
*   @ 无
*******************************************************************************/
static unsigned int ReadColor(unsigned int *pdwWrgb, unsigned char *pbyRgb, unsigned short int *pwHsl)
{
    unsigned int adwRgb[3];
    unsigned char abyRgb[3];

    adwRgb[0] = Read(TCS230_R);
    for(__IO int i = 0; i < 255; i++) {;}
    adwRgb[1] = Read(TCS230_G);
    for(__IO int i = 0; i < 255; i++) {;}
    adwRgb[2] = Read(TCS230_B);

    for(int i = 0; i < 3; i ++) {
        if(adwRgb[i] == 0) return -1;
        if(pdwWrgb[i] != 0) adwRgb[i] = (adwRgb[i] * 255) / pdwWrgb[i];
        else adwRgb[i] = 0;

        if(adwRgb[i] > 255) abyRgb[i] = 255;
        else abyRgb[i] = adwRgb[i];
    }

    if(pbyRgb) memcpy(pbyRgb, abyRgb, 3);
    if(!pwHsl) return 0;

    Rgb2Hsl((const unsigned char*)abyRgb, pwHsl);
    return 0;
}

const static DevTcs230Tdf s_Dev = {
    Rgb2Hsl,
    /*Hsl2Rgb,*/
    Read,
    ReadColor,
};

/*******************************************************************************
函数功能：
*   @ 初始化Tcs230
参数描述：
*   @ 无
返回值描述：
*   @ 颜色设备提供的操作接口
说明：
*   @ 无
*******************************************************************************/
const DevTcs230Tdf *Tcs230Init(Tcs230InitTdf *pstInit)
{
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    DevRccGetClocksFreq(&stRccFreq);

    if(pstInit->dwTimNo == 0 || pstInit->dwTimNo == 7) dwClk = stRccFreq.PCLK2;
    else dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000*1000) - 1;

    stTcs230 = *pstInit;
    memset(&s_stCtr, 0, sizeof(s_stCtr));
    DevTim->Config(pstInit->byTimNO, dwPre, 0xffff);
    DevTim->ConfigCcmr(pstInit->byTimNO, pstInit->byTimCcmrChn, true, 0, ICPSC_DIV1);

    ChooseColor(TCS230_W);                       //选择各滤光片

    if(stTcs230.pwdNOE) *stTcs230.pwdNOE = 1;   //TCS230_OE_H 低电平有效
    for(__IO int i = 0; i < 0x5ffff; i++) {;}
    if(stTcs230.pwdNOE) *stTcs230.pwdNOE = 0;   //TCS230_OE_H 低电平有效
    for(__IO int i = 0; i < 0x5ffff; i++) {;}
    if(stTcs230.pwdNOE) *stTcs230.pwdNOE = 1;   //TCS230_OE_H 低电平有效

    return &s_Dev;
}
#endif