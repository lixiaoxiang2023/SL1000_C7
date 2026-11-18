/*******************************************************************************
文件名称：    cat3626.c
目录：        $PROJ_DIR$/../code/dev
项目名称：    事业一部技术平台
创建者：      胥强
创建日期：    2016.03.14
程序用途：    用于cat3626的LED驱动

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息
20151123：
第一次由XQ创建初始程序。
2016.03.14：
整理驱动模块，通用调用核心驱动层
*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"

#if(CONFIG_CAT3626)
/*******************************************************************************
常量定义
*******************************************************************************/
#define CAT3626_ADDR        0xcc
#define CAT3626_REG_A       0
#define CAT3626_REG_B       1
#define CAT3626_REG_C       2
#define CAT3626_REG_EN      3

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static struct {
    unsigned int *pwdI2cEn;

    unsigned char byI2cNo;
    unsigned char byChn[6];
    unsigned char byCur[6];
}s_stDev3626;

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 打开Led电流，并开灯
参数描述：
*   @ byChn:0:对用相应通道的打开与关闭状态，
*   @ bool blReWrCur ：是否需要重新写入驱动器电流,true：重新写入电流值，false:不用重新写电流值
返回值描述：
*   @ -1：未找到相应通道的电流值
*   @  0：成功
说明：
*******************************************************************************/
static int SetDevCurrent(unsigned char byChn, bool blWait)
{
    unsigned char abySet[3] = {0, 0, 0};

    abySet[0] = CAT3626_ADDR;
    if(byChn & (CAT3626_CHN_LEDA1 | CAT3626_CHN_LEDA2)) abySet[1] = CAT3626_REG_A;
    else if(byChn & (CAT3626_CHN_LEDB1 | CAT3626_CHN_LEDB2)) abySet[1] = CAT3626_REG_B;
    else if(byChn & (CAT3626_CHN_LEDC1 | CAT3626_CHN_LEDC2)) abySet[1] = CAT3626_REG_C;

    for(int i = 0; i < 6; i ++) {
        if(s_stDev3626.byChn[i] == byChn) {
            abySet[2] = s_stDev3626.byCur[i];
            break;
        }
    }

#if(CONFIG_CAT3626_DMA)
    DevI2c->TrsDma(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IRQ)
    DevI2c->TrsIrq(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IO)
    DevI2c->TrsIo(s_stDev3626.byI2cNo, abySet, 3);
#else
    DevI2c->Trs(s_stDev3626.byI2cNo, abySet, NULL, 3);
#endif

    return 0;
}

/*******************************************************************************
函数功能：
*   @ 打开Led电流，并开灯
参数描述：
*   @ byChn:0:对用相应通道的打开与关闭状态，
*   @ bool blReWrCur ：是否需要重新写入驱动器电流,true：重新写入电流值，false:不用重新写电流值
返回值描述：
*   @ -1：未找到相应通道的电流值
*   @  0：成功
说明：
*******************************************************************************/
static int TurnOn(unsigned char byChn, bool blReWrCur, bool blWait)
{
    unsigned char abySet[3] = {0, 0, 0};

    if(blReWrCur) {
        abySet[0] = CAT3626_ADDR;
        if(byChn & (CAT3626_CHN_LEDA1 | CAT3626_CHN_LEDA2)) abySet[1] = CAT3626_REG_A;
        else if(byChn & (CAT3626_CHN_LEDB1 | CAT3626_CHN_LEDB2)) abySet[1] = CAT3626_REG_B;
        else if(byChn & (CAT3626_CHN_LEDC1 | CAT3626_CHN_LEDC2)) abySet[1] = CAT3626_REG_C;

        for(int i = 0; i < 6; i ++) {
            if(s_stDev3626.byChn[i] == byChn) {
                abySet[2] = s_stDev3626.byCur[i];
                break;
            }
        }

        if(!abySet[2]) return -1;
#if(CONFIG_CAT3626_DMA)
        DevI2c->TrsDma(s_stDev3626.byI2cNo, abySet, NULL, 3, true);
#elif(CONFIG_CAT3626_IRQ)
        DevI2c->TrsIrq(s_stDev3626.byI2cNo, abySet, NULL, 3, true);
#elif(CONFIG_CAT3626_IO)
        DevI2c->TrsIo(s_stDev3626.byI2cNo, abySet, 3);
#else
        DevI2c->Trs(s_stDev3626.byI2cNo, abySet, NULL, 3);
#endif
    }

    //led turn on
    abySet[0] = CAT3626_ADDR;
    abySet[1] = CAT3626_REG_EN;
    abySet[2] = byChn;

#if(CONFIG_CAT3626_DMA)
    return DevI2c->TrsDma(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IRQ)
    return DevI2c->TrsIrq(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IO)
    return DevI2c->TrsIo(s_stDev3626.byI2cNo, abySet, 3);
#else
    return DevI2c->Trs(s_stDev3626.byI2cNo, abySet, NULL, 3);
#endif
}

/*******************************************************************************
*函数功能：
*   @ 打开、关闭设备
*参数描述：
*   @ bool blCmd  true:打开， false：关闭
*返回值描述：
*   @ -1：未找到相应通道的电流值
*   @  0：成功
*说明：
*******************************************************************************/
static void DevOn(bool blCmd)
{
    if(!s_stDev3626.pwdI2cEn) return ;

    if(blCmd) *s_stDev3626.pwdI2cEn = 1;
    else *s_stDev3626.pwdI2cEn = 0;
}

/*******************************************************************************
函数功能：
*   @ 所有的灯打开
参数描述：
*   @ 无
返回值描述：
*   @ -1：未找到相应通道的电流值
*   @  0：成功
说明：
*******************************************************************************/
static int TurnAllOn(bool blWait)
{
    unsigned char abySet[3] = {0, 0, 0};

    //led turn on
    abySet[0] = CAT3626_ADDR;
    abySet[1] = CAT3626_REG_EN;
    abySet[2] = 0x3f;

#if(CONFIG_CAT3626_DMA)
    return DevI2c->TrsDma(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IRQ)
    return DevI2c->TrsIrq(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IO)
    return DevI2c->TrsIo(s_stDev3626.byI2cNo, abySet, 3);
#else
    return DevI2c->Trs(s_stDev3626.byI2cNo, abySet, NULL, 3);
#endif
}

/*******************************************************************************
函数功能：
*   @ 所有的灯关闭
参数描述：
*   @ 无
返回值描述：
*   @ -1：未找到相应通道的电流值
*   @  0：成功
说明：
*******************************************************************************/
static int TurnAllOff(bool blWait)
{
    unsigned char abySet[3] = {0, 0, 0};

    //led turn on
    abySet[0] = CAT3626_ADDR;
    abySet[1] = CAT3626_REG_EN;
    abySet[2] = 0;

#if(CONFIG_CAT3626_DMA)
    return DevI2c->TrsDma(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IRQ)
    return DevI2c->TrsIrq(s_stDev3626.byI2cNo, abySet, NULL, 3, blWait);
#elif(CONFIG_CAT3626_IO)
    return DevI2c->TrsIo(s_stDev3626.byI2cNo, abySet, 3);
#else
    return DevI2c->Trs(s_stDev3626.byI2cNo, abySet, NULL, 3);
#endif
}

/*******************************************************************************
函数功能：
*   @ 清除缓存的电流
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*******************************************************************************/
static void ClrCurrent(void)
{
    for(int i = 0; i < 6; i ++) {
        s_stDev3626.byChn[0] = 0;
        s_stDev3626.byCur[0] = 0;
    }
}

/*******************************************************************************
函数功能：
*   @ 设定缓存电流，但是不会写入驱动器
参数描述：
*   @ unsigned char byChn ： 通道值，0-5
*   @ unsigned char byCur ： 通道的电流
返回值描述：
*   @ -1：未找到相应通道的电流值
*   @  0：成功
说明：
*******************************************************************************/
static int SetCurrent (unsigned char byChn, unsigned char byCur)
{
    int i = 0;
    for(i = 0; i < 6; i ++) {
        if(s_stDev3626.byChn[i] == byChn) {
            s_stDev3626.byCur[i] = byCur;
            return 0;
        }
    }

    for(i = 0; i < 6; i ++) {
        if(!s_stDev3626.byChn[i]) {
            s_stDev3626.byChn[i] = byChn;
            s_stDev3626.byCur[i] = byCur;
            break;
        }
    }

    return (i == 6)? -1:0;
}

const static DevCat3626Tdf s_Dev = {
    SetDevCurrent,
    TurnOn,
    TurnAllOn,
    TurnAllOff,
    ClrCurrent,
    SetCurrent,
    DevOn,
};

/*******************************************************************************
函数功能：
*   @ 初始化CAT3626
参数描述：
*   @ 无
返回值描述：
*   @ 3626的操作方法集合
说明：
*******************************************************************************/
const DevCat3626Tdf * Cat3626Init( Cat3626InitTdf *pstInit )
{
    for(int i = 0; i < 6; i ++) {
        s_stDev3626.byChn[i] = 0;
        s_stDev3626.byCur[i] = 0;
    }

    s_stDev3626.byI2cNo = pstInit->byI2cNo;
   // DevI2c->Config(pstInit->byI2cNo, 400000, true);

#if(CONFIG_CAT3626_IO)
    DevI2c->SetPin(pstInit->byI2cNo,  pstInit->byGpioNo,pstInit->byI2cClk, pstInit->byGpioNo,pstInit->byI2cSda,0);
#else
    DevI2c->Config(pstInit->byI2cNo, 400000, true);
#endif

#if(CONFIG_CAT3626_DMA)
    DevSpi->ConfigDmaTx(pstInit->byI2cNo, pstInit->byI2cTxDmaNO, pstInit->byI2cTxDmaChn);
#if(CONFIG_I2C_RX_DMA)
    DevSpi->ConfigDmaRx(pstInit->bySpiNo, pstInit->byI2cRxDmaNO, pstInit->byI2cRxDmaChn);
#endif
#endif

    if(pstInit->pwdI2cEn) *pstInit->pwdI2cEn = 1;
    return &s_Dev;
}
#endif