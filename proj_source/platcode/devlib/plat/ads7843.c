/*******************************************************************************
文件名称：    ads7843.c
目录：        $PROJ_DIR$/../code/dev
项目名称：    事业一部技术平台
创建者：      mdj
创建日期：    2016.03.14
程序用途：    用于ads7843驱动

版权声明：
Copyright urit Co.,Ltd. All Right Reserved

修改及维护信息
2016.03.14：
整理驱动模块，通用调用核心驱动层
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_ADS7843)
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
static struct
{
    unsigned short int wAdcX;
    unsigned short int wAdcY;

    unsigned short int wCnt;
    bool blIsGet;
    bool blIsDelay;
    bool blTouchUp;

    KeyMsgTdf stMsg;
    stCalibTdf stCalib;

    void (*PressHandler)(KeyMsgTdf *pstMsg);
} s_stTs;

static Ads7843InitTdf s_stConfig;

/*for DMA opration*/
static const unsigned char c_byaGetCmd[35] = {
    0xd0, 0x00, 0xd0,
    0x00, 0xd0, 0x00, 0xd0, 0x00, 0xd0, 0x00, 0xd0,
    0x00, 0xd0, 0x00, 0xd0, 0x00, 0xd0, 0x00, 0x90,
    0x00, 0x90, 0x00, 0x90, 0x00, 0x90, 0x00, 0x90,
    0x00, 0x90, 0x00, 0x90, 0x00, 0x90, 0x00, 0x00};
static unsigned char s_byaGet[35];

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 数据排序
参数描述：
*   @ unsigned short int *pwDatIn ： 需要排序的数据
*   @ unsigned int dwLenght ：数据长度
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void Sort(unsigned short int *pwDatIn, unsigned int dwLenght)
{
    unsigned short int dattmp = 0;
    unsigned short int *dataddtmp;
    unsigned char swap = 0;

    for (unsigned int k = 0; (k < dwLenght) && (!swap); k++)
    {
        //冒泡法排序 stdata数组大到小  最大值在ssdata[0]
        swap = 1;
        dataddtmp = pwDatIn;
        for (unsigned int j = 0; j < dwLenght - 1; j++, dataddtmp++)
        {
            if (*dataddtmp < *(dataddtmp + 1))
            {
                //大的往左移
                dattmp = *dataddtmp;
                *dataddtmp = *(dataddtmp + 1);
                *(dataddtmp + 1) = dattmp;
                swap = 0; //判断是否有排序 如果没有则退出排序 优化排序时间
            }
        }
    }
}

/*******************************************************************************
函数功能：
*   @ 数据校准转换
参数描述：
*   @ unsigned short int wFromX ：
*   @ unsigned short int wFromY ：
*   @ unsigned short int *pwToX ：
*   @ unsigned short int *pwToY ：
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int TscConverse(unsigned short int wFromX, unsigned short int wFromY, unsigned short int *pwToX, unsigned short int *pwToY)
{
    *pwToX = (unsigned short int)(((float)s_stTs.stCalib.a[0] +
                                   (float)s_stTs.stCalib.a[1] * (float)wFromX + (float)s_stTs.stCalib.a[2] * (float)wFromY) /
                                  (float)s_stTs.stCalib.a[6]);

    *pwToY = (unsigned short int)(((float)s_stTs.stCalib.a[3] +
                                   (float)s_stTs.stCalib.a[4] * (float)wFromX + (float)s_stTs.stCalib.a[5] * (float)wFromY) /
                                  (float)s_stTs.stCalib.a[6]);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 用于校正触摸屏函数算法
参数描述:
*   @ stCalibTdf *pstCalibrate：触摸屏校准需要的参数
返回值描述：
*   @ -1：校准失败
*   @ 0：校准正确
说明：
2.3 三点校准法
    触摸屏常和点阵式液晶显示(LCD)屏叠加在一起配套使用,构成一个矩形的实际物理平面;
    而由用户触摸的触摸点集合经过 A/D 转换器,得到具体显示坐标的集合,这个集合构成了
    一个逻辑平面。 由于存在误差,这两个平面并不重合,校准的作用就是要将逻辑平面映射
    到物理平面上,即得到触点在液晶屏上的位置坐标。 校准算法的中心思想也就是要建立
    这样一个映射函数现有的校准算法大多是基于线性校准, 即首先假定物理平面和逻辑平
    面之间的误差是线性误差,由旋转和偏移形成 。如果已知触摸屏上一点 A,其物理坐标
    为（xa,ya） ,相应的显示坐标为(xa',ya')   ,根据假定的线性关系,可以得到:

    Xa'=k1*xa + k2*ya+k3  ---------  2-1
    Ya'=k4*ya + k5*ya+k6  ---------  2-2

    只要能够求出线性变换的参数（k1,k2….）就可以来校正从触摸屏坐标得到显示坐标，
    显然要求参数，至少需要三个点的坐标，那么我们在触摸屏上取三个点
    （注意：不要取边界点，并且三点的覆盖面要大）

    Xa'=k1*xa + k2*ya+k3  ---------  2-1
    Ya'=k4*ya + k5*ya+k6  ---------  2-2

    Xb'=k1*xb + k2*yb+k3  ---------  2-1
    Yb'=k4*yb + k5*yb+k6  ---------  2-2

    Xc'=k1*xc + k2*yc+k3  ---------  2-1
    Yc'=k4*yc + k5*yc+k6  ---------  2-2

    其中参数中的前三点用于计算数据，最后的第四点可以用于校验触摸屏是否正常
******************************************************************************/
static int Calibrate(stCalibTdf *pstCalib)
{
    int j;
    float n, x, y, x2, y2, xy, z, zx, zy;
    float det, a, b, c, e, f, i;
    float scaling = 65536.0;

    // Get sums for matrix
    n = x = y = x2 = y2 = xy = 0;
    for (j = 0; j < 5; j++)
    {
        n += 1.0;
        x += (float)pstCalib->x[j];
        y += (float)pstCalib->y[j];
        x2 += (float)(pstCalib->x[j] * pstCalib->x[j]);
        y2 += (float)(pstCalib->y[j] * pstCalib->y[j]);
        xy += (float)(pstCalib->x[j] * pstCalib->y[j]);
    }

    // Get determinant of matrix -- check if determinant is too small
    det = n * (x2 * y2 - xy * xy) + x * (xy * y - x * y2) + y * (x * xy - y * x2);
    if (det < 0.1 && det > -0.1)
    {
        //printf("ts_calibrate: determinant is too small -- %f\n",det);
        return -1;
    }

    // Get elements of inverse matrix
    a = (x2 * y2 - xy * xy) / det;
    b = (xy * y - x * y2) / det;
    c = (x * xy - y * x2) / det;
    e = (n * y2 - y * y) / det;
    f = (x * y - n * xy) / det;
    i = (n * x2 - x * x) / det;

    // Get sums for x calibration
    z = zx = zy = 0;
    for (j = 0; j < 5; j++)
    {
        z += (float)pstCalib->xfb[j];
        zx += (float)(pstCalib->xfb[j] * pstCalib->x[j]);
        zy += (float)(pstCalib->xfb[j] * pstCalib->y[j]);
    }

    // Now multiply out to get the calibration for framebuffer x coord
    pstCalib->a[0] = ((a * z + b * zx + c * zy) * (scaling));
    pstCalib->a[1] = ((b * z + e * zx + f * zy) * (scaling));
    pstCalib->a[2] = ((c * z + f * zx + i * zy) * (scaling));

    // Get sums for y calibration
    z = zx = zy = 0;
    for (j = 0; j < 5; j++)
    {
        z += (float)pstCalib->yfb[j];
        zx += (float)(pstCalib->yfb[j] * pstCalib->x[j]);
        zy += (float)(pstCalib->yfb[j] * pstCalib->y[j]);
    }

    // Now multiply out to get the calibration for framebuffer y coord
    pstCalib->a[3] = ((a * z + b * zx + c * zy) * (scaling));
    pstCalib->a[4] = ((b * z + e * zx + f * zy) * (scaling));
    pstCalib->a[5] = ((c * z + f * zx + i * zy) * (scaling));

    // If we got here, we're OK, so assign scaling to a[6] and return
    pstCalib->a[6] = scaling;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 从采样数据中取得采样值
参数描述：
*   @ 无
返回值描述：
*   @ -1：校准失败
*   @ 0：校准正确
说明：
*   @ 无
*******************************************************************************/
static int TsGetADC(u16 *wAdcXTmp, u16 *wAdcYTmp)
{
    unsigned short int wDat1, wDat2;
    unsigned short int waTmp[16];
    unsigned short int *waTmpX;
    unsigned short int *waTmpY;
    
    waTmpX = &waTmp[9];
    waTmpY = &waTmp[0];
    for (int i = 0; i < 35; i++)
    {
        if (i % 2 == 1)
        {
            wDat1 = s_byaGet[i];
            wDat2 = s_byaGet[i + 1];
            waTmp[i / 2] = wDat1 * 256 + wDat2;
            waTmp[i / 2] = (waTmp[i / 2] >> 3) & 0xfff;
        }
    }

    Sort(waTmpX, 7);
    Sort(waTmpY, 9);
    *wAdcXTmp = (*(waTmpX + 1) + *(waTmpX + 2)) / 2;
    *wAdcYTmp = (*(waTmpY + 1) + *(waTmpY + 2)) / 2;
    
    if ((*(waTmpX + 1) - *(waTmpX + 2)) < 20 && (*(waTmpY + 1) - *(waTmpY + 2)) < 20)
        return 1;
    else
        return 0;
}

/*******************************************************************************
函数功能：
*   @ 按键处理函数由定时器调用spi的dma采样
参数描述：
*   @ 无
返回值描述：
*   @ -1：校准失败
*   @ 0：校准正确
说明：
*   @ 无
*******************************************************************************/
static void TscProcess(unsigned int dwPare)
{
    static unsigned short int wAdcK_tempX, wAdcK_tempY;

    if ((*s_stConfig.pwdPenIn == 0) && (s_stTs.wCnt == 0))
    {
        memset(s_byaGet, 0, 35);
        DevSpi->TrsDmaRet(s_stConfig.bySpiNo, c_byaGetCmd, s_byaGet, 35, s_stConfig.pwdSpiNcs);
    }
    else if ((*s_stConfig.pwdPenIn == 0) && (s_stTs.wCnt == 1))
    {
        if (!TsGetADC(&s_stTs.stMsg.wAdcKeyX, &s_stTs.stMsg.wAdcKeyY))
        {
            s_stTs.wCnt = 0;
            return;
        }
        memset(s_byaGet, 0, 35);
        DevSpi->TrsDmaRet(s_stConfig.bySpiNo, c_byaGetCmd, s_byaGet, 35, s_stConfig.pwdSpiNcs);
    }
    else if ((*s_stConfig.pwdPenIn == 0) && (s_stTs.wCnt == 3))
    {
        if (!TsGetADC(&wAdcK_tempX, &wAdcK_tempY))
        {
            s_stTs.wCnt = 0;
            return;
        }
        if (abs(s_stTs.stMsg.wAdcKeyX - wAdcK_tempX) < 10 && abs(s_stTs.stMsg.wAdcKeyY - wAdcK_tempY) < 10)
        {
            if (!TscConverse(s_stTs.stMsg.wAdcKeyX, s_stTs.stMsg.wAdcKeyY, &s_stTs.stMsg.wKeyX, &s_stTs.stMsg.wKeyY))
            {
                s_stTs.stMsg.emKeyState = KEY_PRESS;
                s_stTs.blIsGet = TRUE;
                if (s_stTs.PressHandler)
                    s_stTs.PressHandler(&s_stTs.stMsg);
            }
        }
        else
        {
            s_stTs.wCnt = 0;
            return;
        }
    }

    if ((*s_stConfig.pwdPenIn == 1))
    {
        s_stTs.wCnt = 0;
        s_stTs.blIsDelay = TRUE;
        if ((s_stTs.wCnt > 0) && (s_stTs.wCnt < 3000))
        {
            if (s_stTs.blIsGet == TRUE)
            {
                s_stTs.stMsg.emKeyState = KEY_UP;
            }
        }
        return;
    }
    else if ((s_stTs.wCnt < 5000) && (*s_stConfig.pwdPenIn == 0))
    {
        s_stTs.wCnt++;
    }
}

/*******************************************************************************
函数功能：
*   @ 用于触摸屏中断函数
参数描述：
*   @ 无
返回值描述：
*   @ -1：校准失败
*   @ 0：校准正确
说明：
*   @ 无
*******************************************************************************/
static void TsItHandle(unsigned int dwPare)
{
    DevIo->IrpDown(s_stConfig.byPenChn, 1);
    if (s_stTs.blTouchUp == false)
        return;

    //按键状态
    s_stTs.wCnt = 0;
    s_stTs.blIsGet = FALSE;
    s_stTs.blIsDelay = FALSE;
    s_stTs.stMsg.emKeyState = KEY_DM;

    memset(s_byaGet, 0, 35);
    //DevSpi->TrsDmaRet(s_stConfig.bySpiNo, c_byaGetCmd, s_byaGet, 35, s_stConfig.pwdSpiNcs);
    DevTim->TimEvent(s_stConfig.byTimNO, 10000, TscProcess, 0);
}

/*******************************************************************************
函数功能：
*   @ 注册得到按键的回掉函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void RegTsHandler(void (*TsHandler)(KeyMsgTdf *pstMsg))
{
    if (TsHandler)
        s_stTs.PressHandler = TsHandler;
}

/*******************************************************************************
函数功能：
*   @ 注册得到按键的回掉函数
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static int SetCalibrate(stCalibTdf *pstCalib)
{
    if (pstCalib)
        s_stTs.stCalib = *pstCalib;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 关闭触摸功能
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void TouchDown(void)
{
    s_stTs.blTouchUp = false;
    DevIo->IrpDown(s_stConfig.byPenChn, 1);
}

/*******************************************************************************
函数功能：
*   @ 启动触摸功能
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static void TouchUp(void)
{
    s_stTs.blTouchUp = true;
    DevIo->IrpUp(s_stConfig.byPenChn, 1);
}

/*******************************************************************************
函数功能：
*   @ 取得是否触摸屏在按下的状态
参数描述：
*   @ 无
返回值描述：
*   @ true 按下
*   @ false 未按下
说明：
*   @ 无
*******************************************************************************/
static bool IsTouch(void)
{
    if (*s_stConfig.pwdPenIn == 0)
        return true;
    else
        return false;
}

static const DevAds7843Tdf s_Dev = {
    RegTsHandler,
    Calibrate,
    SetCalibrate,
    IsTouch,
    TouchUp,
    TouchDown};

/*******************************************************************************
函数功能：
*   @ 初始化
参数描述：
*   @ 无
返回值描述：
*   @ 方法集合
说明：
*   @ 无
*******************************************************************************/
const DevAds7843Tdf *Ads7843Init(Ads7843InitTdf *pstInit)
{
    const uint8_t abyInitPenCmd[2] = {0x90, 0xd0};
    ClocksTypeDef stRccFreq;
    unsigned int dwPre = 0;
    unsigned int dwClk = 0;

    DevRccGetClocksFreq(&stRccFreq);

    if (pstInit->byTimNO == 0 || pstInit->byTimNO == 7)
        dwClk = stRccFreq.PCLK2;
    else
        dwClk = stRccFreq.PCLK1;

    dwPre = dwClk / (1000 * 1000) - 1;

    memset(&s_stTs, 0, sizeof(s_stTs));

    s_stConfig = *pstInit;
    *s_stConfig.pwdSpiNcs = 1;
    DevSpi->Config(pstInit->bySpiNo, pstInit->dwSpiBaud);
    DevSpi->Trs(pstInit->bySpiNo, abyInitPenCmd, NULL, 2, s_stConfig.pwdSpiNcs);

#if (PLAT_STM32_F2 | PLAT_STM32_F4)
    DevSpi->ConfigDmaTx(pstInit->bySpiNo, pstInit->bySpiTxDmaNO, pstInit->bySpiTxDmaChn, pstInit->bySpiTxDmaStream);
    DevSpi->ConfigDmaRx(pstInit->bySpiNo, pstInit->bySpiRxDmaNO, pstInit->bySpiRxDmaChn, pstInit->bySpiRxDmaStream);
#else
    DevSpi->ConfigDmaTx(pstInit->bySpiNo, pstInit->bySpiTxDmaNO, pstInit->bySpiTxDmaChn);
    DevSpi->ConfigDmaRx(pstInit->bySpiNo, pstInit->bySpiRxDmaNO, pstInit->bySpiRxDmaChn);
#endif

    DevIo->AddIrp(pstInit->byPenGpioNO, pstInit->byPenChn, Trigger_Falling, TsItHandle);
    DevTim->Config(pstInit->byTimNO, dwPre, 5000);

    *s_stConfig.pwdSpiNcs = 0;
    for (__IO int delay = 0; delay < 0xfffff; delay++)
    { ; }
    *s_stConfig.pwdSpiNcs = 1;

    // 初始化时关闭
    DevIo->IrpDown(s_stConfig.byPenChn, 1);
    s_stTs.blTouchUp = false;
    return &s_Dev;
}
#endif