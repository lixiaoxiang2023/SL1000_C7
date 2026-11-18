/*******************************************************************************
文件名称：   i2c.c
目录：       $PROJ_DIR$/../code/dev
项目名称：   事业一部技术平台
创建者：     胥强
创建日期：   2016.03.12
程序用途：   基于stm32的i2c驱动接口

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

#if(CONFIG_I2C1|CONFIG_I2C2|CONFIG_I2C3)
/*******************************************************************************
常量定义
*******************************************************************************/
#define I2C_DEV_NUM     3

/* I2C设备读写命令 *///模拟I2C
/* I2C操作定义 */
#define I2C_SCL_Reset(DEV)  DevIo->Write(s_staDev[DEV].Scl_Port, s_staDev[DEV].Scl_Pin, 0)
#define I2C_SCL_Set(DEV)    DevIo->Write(s_staDev[DEV].Scl_Port, s_staDev[DEV].Scl_Pin, 1)
#define I2C_SDA_Reset(DEV)  DevIo->Write(s_staDev[DEV].Sda_Port, s_staDev[DEV].Sda_Pin, 0)
#define I2C_SDA_Set(DEV)    DevIo->Write(s_staDev[DEV].Sda_Port, s_staDev[DEV].Sda_Pin, 1)
#define I2C_SDA_Read(DEV)   DevIo->Read (s_staDev[DEV].Sda_Port, s_staDev[DEV].Sda_Pin)
#define I2C_Delay(DEV)      s_staDev[DEV].delay()

/* I2C应答信号重定义 */
#define I2C_Send_ACK(DEV)   I2C_SendBit_0(DEV)
#define I2C_Send_NAK(DEV)   I2C_SendBit_1(DEV)
#define I2C_Check_Ack(DEV)  I2C_ReadBit(DEV)

/* I2C设备读写命令 */
#define I2C_DEV_W           (0x00)
#define I2C_DEV_R           (0x01)

/*******************************************************************************
全局变量定义
*******************************************************************************/

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef union {
    __IO unsigned short int wSR1;

    struct {
        __IO uint16_t SB:1;//                          ((uint16_t)0x0001)            /*!<Start Bit (Master mode) */
        __IO uint16_t ADDR:1;//                        ((uint16_t)0x0002)            /*!<Address sent (master mode)/matched (slave mode) */
        __IO uint16_t BTF:1;//                         ((uint16_t)0x0004)            /*!<Byte Transfer Finished */
        __IO uint16_t ADD10:1;//                       ((uint16_t)0x0008)            /*!<10-bit header sent (Master mode) */
        __IO uint16_t STOPF:1;//                       ((uint16_t)0x0010)            /*!<Stop detection (Slave mode) */
        __IO uint16_t Reserved5:1;
        __IO uint16_t RXNE:1;//                        ((uint16_t)0x0040)            /*!<Data Register not Empty (receivers) */
        __IO uint16_t TXE:1;//                         ((uint16_t)0x0080)            /*!<Data Register Empty (transmitters) */
        __IO uint16_t BERR:1;//                        ((uint16_t)0x0100)            /*!<Bus Error */
        __IO uint16_t ARLO:1;//                        ((uint16_t)0x0200)            /*!<Arbitration Lost (master mode) */
        __IO uint16_t AF:1;//                          ((uint16_t)0x0400)            /*!<Acknowledge Failure */
        __IO uint16_t OVR:1;//                         ((uint16_t)0x0800)            /*!<Overrun/Underrun */
        __IO uint16_t PECERR:1;//                      ((uint16_t)0x1000)            /*!<PEC Error in reception */
        __IO uint16_t Reserved13:1;
        __IO uint16_t TIMEOUT:1;//                     ((uint16_t)0x4000)            /*!<Timeout or Tlow Error */
        __IO uint16_t SMBALERT:1;//                    ((uint16_t)0x8000)            /*!<SMBus Alert */
    }BitSR1;
}I2cSrTdf;

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
static struct {
    I2C_TypeDef *pstDev;

    bool blErr;
#if(CONFIG_I2C_IO)
    uint8_t Scl_Port;
    uint8_t Scl_Pin;
    uint8_t Sda_Port;
    uint8_t Sda_Pin;
    void (*delay)(void);
#endif

#if(CONFIG_I2C_DMA)
    unsigned char byDmaNo;
    unsigned char byDmaChnTx;
#if(CONFIG_I2C_RX_DMA)
    unsigned char byDmaChnRx;
#endif
#endif

#if(CONFIG_I2C_IRP)
    bool blIrpDoing;
    unsigned int dwLen;
    unsigned int dwTrsCnt;
    unsigned char *pbyDat;
    unsigned char *pbyDatRx;


    I2cSrTdf unSr1;
    unsigned short int wSr2;

#endif
}s_staDev[I2C_DEV_NUM];

/*******************************************************************************
内部函数申明
*******************************************************************************/

/*******************************************************************************
函数功能：
*   @ 初始化I2c设备时钟
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
static void RccInit(void)
{
#if(CONFIG_I2C1)
    RCC->BitAPB1ENR.I2C1EN = 1;
#endif
#if(CONFIG_I2C2)
    RCC->BitAPB1ENR.I2C2EN = 1;
#endif
#if(CONFIG_I2C3)
    RCC->BitAPB1ENR.I2C3EN = 1;
#endif
}

/*******************************************************************************
函数功能：
*   @ 初始化I2c设备参数
参数描述：
*   @ unsigned int dwDev：设备号0，1
*   @ unsigned int dwBaud：波特率
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int Config ( unsigned int dwDev, unsigned int dwBaud, bool blIsDutyCycle2)
{
    unsigned short int tmpreg = 0;
    unsigned short int result = 0x04;
    unsigned int dwClk = 0;
    unsigned int dwDiv = 0;

    if(dwDev >= I2C_DEV_NUM) return -1;

#ifdef PLAT_STM32_F1
    dwClk = 36000000;
#endif

#ifdef PLAT_STM32_F2
    dwClk = 30000000;
#endif

#ifdef PLAT_STM32_F4
    dwClk = 42000000;
#endif
    //初始化寄存器 否则硬件中断出错无法初始化 20210716 14:01 lixx
   // s_staDev[dwDev].pstDev->BitCR1.SWRST = 1;
    //__ASM volatile ("nop");
   // s_staDev[dwDev].pstDev->BitCR1.SWRST = 0;
#if 1
#if(CONFIG_I2C_IRP)
#if(CONFIG_I2C1)
        I2C1->BitCR2.ITERREN = 1;
        DevIo->NvicUp(I2C1_EV_IRQn, 11, 0);
        DevIo->NvicUp(I2C1_ER_IRQn, 11, 0);
#endif
#if(CONFIG_I2C2)
        I2C2->BitCR2.ITERREN = 1;
        DevIo->NvicUp(I2C2_EV_IRQn, 11, 0);
        DevIo->NvicUp(I2C2_ER_IRQn, 11, 0);
#endif
#if(CONFIG_I2C3)
        I2C3->BitCR2.ITERREN = 1;
        DevIo->NvicUp(I2C3_EV_IRQn, 11, 0);
        DevIo->NvicUp(I2C3_ER_IRQn, 11, 0);
#endif

#endif
#if(CONFIG_I2C1)
        I2C1->BitCR2.ITERREN = 1;
        DevIo->NvicUp(I2C1_ER_IRQn, 11, 0);
#endif
#if(CONFIG_I2C2)
        I2C2->BitCR2.ITERREN = 1;
        DevIo->NvicUp(I2C2_ER_IRQn, 11, 0);
#endif
#if(CONFIG_I2C3)
        I2C3->BitCR2.ITERREN = 1;
        DevIo->NvicUp(I2C3_ER_IRQn, 11, 0);
#endif
#endif
    dwDiv = dwClk / 1000000;
    s_staDev[dwDev].pstDev->BitCR2.FREQ = dwDiv;

    //波特率
    s_staDev[dwDev].pstDev->BitCR1.PE1 = 0;
    tmpreg = 0;
    if (dwBaud <= 100000) {
		if(dwBaud == 0 ) {return -1;}
        result = (unsigned short int)(dwClk / (dwBaud << 1));
        if (result < 0x04) result = 0x04;
        tmpreg |= result;
        s_staDev[dwDev].pstDev->TRISE = dwDiv + 1;
    }
    else {
        if (blIsDutyCycle2) result = (uint16_t)(dwClk / (dwBaud * 3));
        else {
            result = (uint16_t)(dwClk / (dwBaud * 25));
            result |= ((uint16_t)0x4000);
        }

        if ((result & ((unsigned short int)0x0FFF)) == 0) result |= (unsigned short int)0x0001;
        tmpreg |= (unsigned short int)(result | ((unsigned short int)0x8000));
        s_staDev[dwDev].pstDev->TRISE = (unsigned short int)(((dwDiv * (unsigned short int)300) / (unsigned short int)1000) + (unsigned short int)1);
    }

    s_staDev[dwDev].pstDev->CCR = tmpreg;
    s_staDev[dwDev].pstDev->BitCR1.PE1 = 1;

    //模式与ack
    s_staDev[dwDev].pstDev->BitCR1.SMBUS = 0;
    s_staDev[dwDev].pstDev->BitCR1.SMBTYPE = 0;
    s_staDev[dwDev].pstDev->BitCR1.ENARP = 0;
    s_staDev[dwDev].pstDev->BitCR1.ACK = 1;


    s_staDev[dwDev].pstDev->OAR1 = (((unsigned short int)0x4000) | 0);

    return 0;
}

#if(CONFIG_I2C_IO)
/*******************************************************************************
函数功能：
*       @ 延时，用来配置I2C默认速度
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：
*       @ 无
*******************************************************************************/
static void I2C_delay(void)
{
     for (__IO uint8_t i = 0; i < 2; i++) ;
}

/*******************************************************************************
函数功能：
*   @ 设定模拟I2C的管脚
参数描述：
*   @ unsigned int dwDev：设备号0，1
*   @ unsigned int *pwdEn ： 使能线
*   @ unsigned int *pwdClk ： 时钟线
*   @ unsigned int *pwdSda ： 数据线
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 通过模拟I2c接口，
*******************************************************************************/
static int SetPin (uint8_t byDev, uint8_t Scl_Port, uint8_t Scl_Pin, uint8_t Sda_Port, uint8_t Sda_Pin, void (*delay)(void))
{
    if(byDev > I2C_DEV_NUM) return -1;

    s_staDev[byDev].Scl_Port = Scl_Port; s_staDev[byDev].Scl_Pin  = Scl_Pin;
    s_staDev[byDev].Sda_Port = Sda_Port; s_staDev[byDev].Sda_Pin  = Sda_Pin;

    DevIo->Config(s_staDev[byDev].Scl_Port, s_staDev[byDev].Scl_Pin, 1, Mode_Out_OD);
    DevIo->Config(s_staDev[byDev].Sda_Port, s_staDev[byDev].Sda_Pin, 1, Mode_Out_OD);
    s_staDev[byDev].delay = (delay == NULL) ? I2C_delay : delay;

    I2C_SDA_Set(byDev);
    I2C_SCL_Set(byDev);

    return 0;
}

/*******************************************************************************
函数功能：
*   @ IIC启动传输函数
参数描述：
*   @ unsigned int dwDev：设备号0，1
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
static void I2cStart(unsigned int byDev)
{
    I2C_SDA_Set(byDev);
    I2C_SCL_Set(byDev);
    I2C_Delay(byDev);
    I2C_SDA_Reset(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Reset(byDev);
    I2C_Delay(byDev);
}

/*******************************************************************************
函数功能：
*   @ IIC停止传输函数
参数描述：
*   @ unsigned int dwDev：设备号0，1
返回值描述：
*   @ 无
说明：
*   @ 无。
*******************************************************************************/
static void I2cStop(unsigned int byDev)
{
    I2C_SDA_Reset(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Set(byDev);
    I2C_Delay(byDev);
    I2C_SDA_Set(byDev);
    I2C_Delay(byDev);
}
 /*******************************************************************************
函数功能：
*       @ 发送比特0
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：SDA XXX________________
          SCL X____/---------\___
*******************************************************************************/
static inline
void I2C_SendBit_0(uint8_t byDev)
{
    I2C_SDA_Reset(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Set(byDev);
    I2C_Delay(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Reset(byDev);
    I2C_Delay(byDev);
}

/*******************************************************************************
函数功能：
*       @ 发送比特1
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：SDA XXX----------------
          SCL X____/---------\___
*******************************************************************************/
void I2C_SendBit_1(uint8_t byDev)
{
    I2C_SDA_Set(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Set(byDev);
    I2C_Delay(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Reset(byDev);
    I2C_Delay(byDev);
}
/*******************************************************************************
函数功能：
*       @ 读1Bit
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：SDA ---XXXXXXXXXXXXXXXX
          SCL ______/---↑---\___
*******************************************************************************/
uint8_t I2C_ReadBit(uint8_t byDev)
{
    uint8_t byRead = 0;

    I2C_SDA_Set(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Set(byDev);
    I2C_Delay(byDev);
    byRead = I2C_SDA_Read(byDev);
    I2C_Delay(byDev);
    I2C_SCL_Reset(byDev);
    I2C_Delay(byDev);

    return byRead;
}
/*******************************************************************************
函数功能：
*       @ 发送8bit数据
参数描述：
*       @ byData,发送的数据
返回描述：
*       @ 无
说    明：
*       @ 无
*******************************************************************************/
static void I2C_SendByte(uint8_t byDev, uint8_t byData)
{
    for (uint8_t i = 0; i < 8; i++) {
        (byData & 0x80) ? I2C_SendBit_1(byDev) : I2C_SendBit_0(byDev);
        byData <<= 1;
    }
}
///*******************************************************************************
//函数功能：
//*   @ I软件模拟IIC写一字节并判断ACK
//参数描述：
//*   @ unsigned int dwDev：设备号0，1
//*   @ uint8_t byDat：要写入的字节
//返回值描述：
//    无
//说明：
//*   @ 无。
//*******************************************************************************/
//static void I2cWriteByte(unsigned int dwDev, unsigned char byDat)
//{
//	for(int i = 0; i < 9; i++) {
//        if(i == 8) *s_staDev[dwDev].pdwSda = 0;
//        else *s_staDev[dwDev].pdwSda = (byDat & 0x80)? 1: 0; //取MSB
//
//        DevTim->Delay(s_staDev[dwDev].byTim, 2);
//        *s_staDev[dwDev].pdwClk = 1;
//        DevTim->Delay(s_staDev[dwDev].byTim, 4);
//		*s_staDev[dwDev].pdwClk = 0;
//		DevTim->Delay(s_staDev[dwDev].byTim, 2);
//		byDat <<= 1;
//	}
//}

/*******************************************************************************
函数功能：
*   @ I2c设备模拟据传输
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int TrsIo(unsigned int dwDev,unsigned char *pbyIn, unsigned int dwLen)
{
    if(dwDev > I2C_DEV_NUM) return -1;

    uint8_t byRe = 0;

    I2cStart(dwDev);
    for(int i=0;i<dwLen;i++){
        I2C_SendByte(dwDev, *pbyIn++);
        I2C_Check_Ack(dwDev);
    }
    byRe = I2C_Check_Ack(dwDev);
    I2cStop(dwDev);

    return byRe;
}
#endif

//#define EV5     0x00030001
//#define EV6     0x00070082
//#define EV6_1   0x00030002

//#define EV8_1   0x00070082
//#define EV8     0x00070084
/*
EVX=事件x
*          This parameter can be one of the following values:
*            @arg I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED: EV1
*            @arg I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED: EV1
*            @arg I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED: EV1
*            @arg I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED: EV1
*            @arg I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED: EV1
*            @arg I2C_EVENT_SLAVE_BYTE_RECEIVED: EV2
*            @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF): EV2
*            @arg (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL): EV2
*            @arg I2C_EVENT_SLAVE_BYTE_TRANSMITTED: EV3
*            @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF): EV3
*            @arg (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL): EV3
*            @arg I2C_EVENT_SLAVE_ACK_FAILURE: EV3_2
*            @arg I2C_EVENT_SLAVE_STOP_DETECTED: EV4
*            @arg I2C_EVENT_MASTER_MODE_SELECT: EV5
*            @arg I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED: EV6
*            @arg I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED: EV6
*            @arg I2C_EVENT_MASTER_BYTE_RECEIVED: EV7
*            @arg I2C_EVENT_MASTER_BYTE_TRANSMITTING: EV8
*            @arg I2C_EVENT_MASTER_BYTE_TRANSMITTED: EV8_2
*            @arg I2C_EVENT_MASTER_MODE_ADDRESS10: EV9
*/

/* --EV1  (all the events below are variants of EV1) */
/* 1) Case of One Single Address managed by the slave */
#define  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED          ((uint32_t)0x00020002) /* BUSY and ADDR flags */
#define  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED       ((uint32_t)0x00060082) /* TRA, BUSY, TXE and ADDR flags */

/* 2) Case of Dual address managed by the slave */
#define  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    ((uint32_t)0x00820000)  /* DUALF and BUSY flags */
#define  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)  /* DUALF, TRA, BUSY and TXE flags */

/* 3) Case of General Call enabled for the slave */
#define  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED        ((uint32_t)0x00120000)  /* GENCALL and BUSY flags */

/* Slave RECEIVER mode --------------------------*/
/* --EV2 */
#define  I2C_EVENT_SLAVE_BYTE_RECEIVED                     ((uint32_t)0x00020040)  /* BUSY and RXNE flags */

/* Slave TRANSMITTER mode -----------------------*/
/* --EV3 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTED                  ((uint32_t)0x00060084)  /* TRA, BUSY, TXE and BTF flags */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTING                 ((uint32_t)0x00060080)  /* TRA, BUSY and TXE flags */
/* --EV3_2 */
#define  I2C_EVENT_SLAVE_ACK_FAILURE                       ((uint32_t)0x00000400)  /* AF flag */
/* --EV4  */
#define  I2C_EVENT_SLAVE_STOP_DETECTED                     ((uint32_t)0x00000010)  /* STOPF flag */
/* --EV5 */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
/* --EV6 */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
/* --EV9 */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */
/* Master RECEIVER mode -----------------------------*/
/* --EV7 */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */
/* Master TRANSMITTER mode --------------------------*/
/* --EV8 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
/* --EV8_2 */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */


#define EV5    I2C_EVENT_MASTER_MODE_SELECT
#define EV6    I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED
#define EV6_1  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED
#define EV8_1   (((I2C_SR2_MSL|I2C_SR2_BUSY|I2C_SR2_TRA)<<16)|I2C_SR1_TXE|I2C_SR1_ADDR)
#define EV8     I2C_EVENT_MASTER_BYTE_TRANSMITTED

#define EV1       I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED
#define EV2       I2C_EVENT_SLAVE_BYTE_RECEIVED
#define EV4       I2C_EVENT_SLAVE_STOP_DETECTED
#define EV1_1     I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED
#define EV3_1     I2C_EVENT_SLAVE_BYTE_TRANSMITTED
#define EV3       I2C_EVENT_SLAVE_BYTE_TRANSMITTING
#define EV3_2     I2C_EVENT_SLAVE_ACK_FAILURE

static int EVx(I2C_TypeDef *pstI2c, unsigned int dwDev, unsigned int dwEvx, bool blAck)
{
    unsigned short int wSr1;
    unsigned short int wSr2;
    unsigned int wDatReg;

    do{
        wSr1 = pstI2c->SR1;
        wSr2 = pstI2c->SR2;
        wDatReg = (unsigned int)wSr1 | ((unsigned int)wSr2 << 16);

        if(s_staDev[dwDev].blErr) return -1;
    }while((wDatReg & dwEvx) != dwEvx);

    if(blAck) {
        while((pstI2c->SR1 & 0x0400) == 0x0400) {
            if(s_staDev[dwDev].blErr) return -1;    // ack
        }
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ I2c设备硬件数据传输
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int Trs(unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen)
{
    I2C_TypeDef *pstI2c;

    if(dwDev >= I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;
    if(dwLen <= 1) return -1;

    s_staDev[dwDev].blErr = false;

    // 产生起始条件
    while(pstI2c->BitSR2.BUSY == 1) {;}
    pstI2c->BitCR1.START = 1;
    if( EVx(pstI2c, dwDev, EV5, false) ) return -1;

    // 发送地址
    pstI2c->DR = *pbyIn ++;
    if( EVx(pstI2c, dwDev, EV6, true) ) return -1;

    // 发送数据
    for(int i = 0; i < dwLen - 1; i ++) {
        pstI2c->DR = *pbyIn ++;
        if( EVx(pstI2c, dwDev, EV8, true) ) return -1;
    }

    // 停止
    pstI2c->BitCR1.STOP = 1;
    return 0;
}
/*******************************************************************************
函数功能：
*   @ I2c设备硬件数据传输
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int SaTrs(unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen)
{
    I2C_TypeDef *pstI2c;

    if(dwDev >= I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;
    if(dwLen <= 1) return -1;

    s_staDev[dwDev].blErr = false;

    // 产生起始条件
    while(pstI2c->BitSR2.BUSY == 1) {;}
    pstI2c->BitCR1.START = 1;

    // 发送地址
    pstI2c->DR = *pbyIn ++;
    if( EVx(pstI2c, dwDev, EV1, true) ) return -1;

    // 发送数据
    for(int i = 0; i < dwLen - 1; i ++) {
        pstI2c->DR = *pbyIn ++;
        if( EVx(pstI2c, dwDev, EV3, true) ) return -1;
    }

    // 停止
    pstI2c->BitCR1.STOP = 1;
    return 0;
}

/*******************************************************************************
函数功能：
*   @ 检测IIC设备是否在线
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char byAdrr：地址
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int CheckOnline(unsigned int dwDev,unsigned char byAdrr)
{
	I2C_TypeDef *pstI2c;
	int ucAck;

    if(dwDev >= I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;

	s_staDev[dwDev].blErr = false;

	// 产生起始条件
    while(pstI2c->BitSR2.BUSY == 1) {;}
    pstI2c->BitCR1.START = 1;
    if( EVx(pstI2c, dwDev, EV5, false) ) return -1;

	// 发送地址
	pstI2c->DR = byAdrr;
	if( EVx(pstI2c, dwDev, EV6, true) )
	{
		ucAck = 1;
	}
	else
	{
		ucAck = 0;
	}
	// 停止
    pstI2c->BitCR1.STOP = 1;

	return ucAck;
}

#if(CONFIG_I2C_READ)
/*******************************************************************************
函数功能：
*   @ I2c设备硬件数据传输读取
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyOut：数据输出地址,第一个数据为设备地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int Read(unsigned int dwDev,unsigned char *pbyOut, unsigned int dwLen)
{
    I2C_TypeDef *pstI2c;

    if(dwDev > I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;

    s_staDev[dwDev].blErr = false;
    (void) pstI2c->DR;

    // 产生起始条件
    while(pstI2c->BitSR2.BUSY == 1) {;}
    pstI2c->BitCR1.START = 1;
    if( EVx(pstI2c, dwDev, EV5, false) ) return -1;

    // 发送设备地址
    pstI2c->DR = (pbyOut[0] | 0x01);
    if( EVx(pstI2c, dwDev, EV6_1, true) ) return -1;

    for(int i = 0; i < dwLen; i ++)
	{
        if( i == (dwLen - 1))
		{
            pstI2c->BitCR1.ACK = 0;
            pstI2c->BitCR1.STOP = 1;
        }

        while(pstI2c->BitSR1.RXNE == 0) {;}
        *pbyOut ++ = pstI2c->DR;

        if( i == (dwLen - 1))
		{
            while(pstI2c->BitCR1.STOP == 1) {;}
            pstI2c->BitCR1.ACK = 1;
        }
    }

    return 0;
}

/*******************************************************************************
函数功能：
*   @ I2c设备硬件数据传输主设备连续读
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyIn ：数据输入地址,器件地址，写寄存器地址，器件地址
*   @ unsigned char *pbyOut：数据输出地址,读取数据
*   @ unsigned int dwLen：需要读取数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @主 //S AD+W      RA     S  AD+R             NACK   P
    @从 //       ACK     ACK          ACK   DATA
*******************************************************************************/
static int ReadData(unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen)
{
    I2C_TypeDef *pstI2c;
    unsigned char ucDevAdrr=0;
    unsigned int uiCx=0;

    if(dwDev > I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;

    s_staDev[dwDev].blErr = false;
    (void) pstI2c->DR;

    // 产生起始条件
   // while(pstI2c->BitSR2.BUSY == 1) {;}
    while(pstI2c->BitSR2.BUSY == 1 && uiCx<600000)
    {
       __NOP();
       uiCx++;
    }
    if(uiCx>=600000)
    {
        return -1;
    }
    uiCx = 0;
    pstI2c->BitCR1.START = 1;
    if( EVx(pstI2c, dwDev, EV5, false) ) return -1;

    // 发送地址
    pstI2c->DR = ucDevAdrr =*pbyIn ++;
    if( EVx(pstI2c, dwDev, EV6, true) ) return -1;

    // 发送数据
    pstI2c->DR = *pbyIn ++;
    if( EVx(pstI2c, dwDev, EV8, true) ) return -1;

    // 产生起始条件
    //while(pstI2c->BitSR2.BUSY == 1) {;}
    pstI2c->BitCR1.START = 1;
    if( EVx(pstI2c, dwDev, EV5, false) ) return -1;

    // 发送设备地址
    pstI2c->DR = (ucDevAdrr| 0x01);
    if( EVx(pstI2c, dwDev, EV6_1, true) ) return -1;

    for(int i = 0; i < dwLen; i ++)
    {
        if( i == (dwLen - 1))
        {
            pstI2c->BitCR1.ACK = 0;
            pstI2c->BitCR1.STOP = 1;
        }

        while(pstI2c->BitSR1.RXNE == 0 && uiCx<600000)
        {
           __NOP();
           uiCx++;
        }
        if(uiCx>=600000)
        {
            return -1;
        }
        uiCx = 0;
        *pbyOut ++ = pstI2c->DR;

        if( i == (dwLen - 1))
        {
            while(pstI2c->BitCR1.STOP == 1 && uiCx<600000)
            {
               __NOP();
               uiCx++;
            }
            
            if(uiCx>=600000)
            {
                return -1;
            }
            uiCx = 0;
            pstI2c->BitCR1.ACK = 1;
        }
    }

    return 0;
}


static int SaRead(unsigned int dwDev,unsigned char *pbyOut, unsigned int dwLen)
{
    I2C_TypeDef *pstI2c;

    if(dwDev > I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;

    s_staDev[dwDev].blErr = false;
    (void) pstI2c->DR;

    // 产生起始条件
    while(pstI2c->BitSR2.BUSY == 1) {;}
    pstI2c->BitCR1.START = 1;

    // 发送设备地址
    pstI2c->DR = (pbyOut[0] | 0x01);
    while((pstI2c->SR1 & 0x0400) == 0x0400) {
        ;    // ack
    }
    if( EVx(pstI2c, dwDev, EV1, FALSE) ) return -1;

    for(int i = 0; i < dwLen; i ++)
	{
        *pbyOut ++ = pstI2c->DR;
        if( EVx(pstI2c, dwDev, EV2, TRUE) ) return -1;

    }
    pstI2c->BitCR1.STOP = 1;
    if( EVx(pstI2c, dwDev, EV2, FALSE) ) return -1;
    while(pstI2c->BitCR1.STOP == 1) {;}
    if( EVx(pstI2c, dwDev, EV4, FALSE) ) return -1;

    return 0;
}

#endif

#if(CONFIG_I2C_DMA)
/*******************************************************************************
*函数功能：
*   @ 配置I2C的发送DMA接口
*参数描述：
*   @ unsigned int dwDev：I2C设备号
*   @ unsigned int dwDmaDev： I2CDMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
*返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
*说明：
*   DAM 优势不是很高，数据量段，操作复杂，取消使用。
*******************************************************************************/
static int ConfigDmaTx ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn)
{
    if(dwDev > I2C_DEV_NUM) return -1;
    DevDma->ConfigM2P(dwDmaDev, dwDmaChn, 0, 0); //数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChnTx = dwDmaChn;

    return 0;
}

#if(CONFIG_I2C_RX_DMA)
/*******************************************************************************
函数功能：
*   @ 配置I2C的发送DMA接口
参数描述：
*   @ unsigned int dwDev：I2C设备号
*   @ unsigned int dwDmaDev： I2CDMA设备号
*   @ unsigned int dwDmaChn ： dma使用通道号
返回值描述：
*   @ -1：失败，设备号错误
*   @  0：成功
说明：
*   @ 无。
*******************************************************************************/
static int ConfigDmaRx ( unsigned int dwDev, unsigned int dwDmaDev, int dwDmaChn)
{
    if(dwDev > I2C_DEV_NUM) return -1;
    DevDma->ConfigP2M(dwDmaDev, dwDmaChn, 0, 0); //数据长度为Byte

    s_staDev[dwDev].byDmaNo = dwDmaDev;
    s_staDev[dwDev].byDmaChnRx = dwDmaChn;

    return 0;
}
#endif

/*******************************************************************************
函数功能：
*   @ I2c设备硬件数据传输
参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
返回值描述：
*   @ -1：失败
*   @  0：成功
说明：
*   @ 无
*******************************************************************************/
static int TrsDma(unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, bool blWait)
{
    unsigned char byDmaNo;
    unsigned char byChnTx;
#if(CONFIG_I2C_RX_DMA)
    unsigned char byChnRx;
#endif

    I2C_TypeDef *pstI2c;

    if(dwDev > I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;

    byDmaNo = s_staDev[dwDev].byDmaNo;
    byChnTx = s_staDev[dwDev].byDmaChnTx;
#if(CONFIG_I2C_RX_DMA)
    byChnRx = s_staDev[dwDev].byDmaChnRx;
#endif

    pstI2c->BitCR2.DMAEN = 0;
    //clear data
    while (pstI2c->BitSR1.TXE == 0);
    __IO unsigned short int wClr = pstI2c->DR;

    DevDma->Trs(byDmaNo, byChnTx, (unsigned int)&pstI2c->DR, (unsigned int)pbyIn, dwLen);
#if(CONFIG_I2C_RX_DMA)
    DevDma->Trs(byDmaNo, byChnRx, (unsigned int)&pstI2c->DR, (unsigned int)pbyOut, dwLen);
    byChnTx = byChnRx;
#endif

    pstI2c->BitCR2.DMAEN = 1;

    if( blWait ) {
        while(DevDma->IsBusy(byDmaNo, byChnTx)){;}
        pstI2c->BitCR2.DMAEN = 0;
    }

    return 0;
}
#endif

#if(CONFIG_I2C_IRP)
/*******************************************************************************
*函数功能：
*   @ I2c设备硬件数据传输
*参数描述：
*   @ unsigned int dwDev：I2c设备号
*   @ unsigned char *pbyIn：数据输入地址
*   @ unsigned char *pbyOut：数据输出地址
*   @ unsigned int dwLen：需要操作的数据长度
*   @ ubool blWait：是否需要等待操作完成
*返回值描述：
*   @ -1：失败
*   @  0：成功
*说明：
*   @ 提升i2c操作效率
*******************************************************************************/
static int TrsIrq(unsigned int dwDev,unsigned char *pbyIn, unsigned char *pbyOut, unsigned int dwLen, bool blWait)
{
    I2C_TypeDef *pstI2c;
    unsigned int uiCx=0;

    if(dwDev > I2C_DEV_NUM) return -1;
    pstI2c = s_staDev[dwDev].pstDev;

    if(s_staDev[dwDev].blIrpDoing) return -1;
    if(pstI2c->BitSR2.BUSY == 1) return -1;

    s_staDev[dwDev].blIrpDoing = true;
    s_staDev[dwDev].pbyDat = pbyIn;
    s_staDev[dwDev].pbyDatRx = pbyOut;
    s_staDev[dwDev].dwLen = dwLen;
    s_staDev[dwDev].dwTrsCnt = 0;

    // 产生起始条件
    pstI2c->BitCR2.ITBUFEN = 1;
    pstI2c->BitCR2.ITEVTEN = 1;
    pstI2c->BitCR1.START = 1;

    // 开始触发中断 序列
    if(!blWait) return 0;

    // 等待执行
   // while(s_staDev[dwDev].blIrpDoing) {;}
    while(s_staDev[dwDev].blIrpDoing == 1 && uiCx<600000)
    {
       __NOP();
       uiCx++;
    }
    if(uiCx>=600000)
    {
        return -1;
    }
    uiCx = 0;
    //while(pstI2c->BitSR2.BUSY == 1) {;}
    while(pstI2c->BitSR2.BUSY == 1 && uiCx<600000)
    {
       __NOP();
       uiCx++;
    }
    if(uiCx>=600000)
    {
        return -1;
    }
    uiCx = 0;

    return 0;
}

#if(CONFIG_I2C1)
void I2C1_EV_IRQHandler(void)
{
    s_staDev[0].unSr1.wSR1 = I2C1->SR1;
    s_staDev[0].wSr2 = I2C1->SR2;

    // 产生起始条件
    if(s_staDev[0].unSr1.BitSR1.SB) {
        I2C1->DR = s_staDev[0].pbyDat[0];
        s_staDev[0].dwTrsCnt = 1;
        return ;
    }

    if(s_staDev[0].unSr1.BitSR1.ADDR) {
        I2C1->DR = s_staDev[0].pbyDat[1];
        s_staDev[0].dwTrsCnt = 2;
        return;
    }

    if(s_staDev[0].unSr1.BitSR1.TXE) {
        if(s_staDev[0].dwTrsCnt >= s_staDev[0].dwLen) {
            I2C1->BitCR1.STOP = 1;
            I2C1->BitCR2.ITBUFEN = 0;
            I2C1->BitCR2.ITEVTEN = 0;
            s_staDev[0].blIrpDoing = false;
        }
        else {
            I2C1->DR = s_staDev[0].pbyDat[s_staDev[0].dwTrsCnt];
            s_staDev[0].dwTrsCnt ++;
        }
    }
}
#endif

#if(CONFIG_I2C2)
void I2C2_EV_IRQHandler(void)
{
    s_staDev[1].unSr1.wSR1 = I2C2->SR1;
    s_staDev[1].wSr2 = I2C2->SR2;

    // 产生起始条件
    if(s_staDev[1].unSr1.BitSR1.SB) {
        I2C2->DR = s_staDev[1].pbyDat[0];
        s_staDev[1].dwTrsCnt = 1;
        return ;
    }

    if(s_staDev[1].unSr1.BitSR1.ADDR) {
        I2C2->DR = s_staDev[1].pbyDat[1];
        s_staDev[1].dwTrsCnt = 2;
        return;
    }

    if(s_staDev[1].unSr1.BitSR1.TXE) {
        if(s_staDev[1].dwTrsCnt >= s_staDev[1].dwLen) {
            I2C2->BitCR1.STOP = 1;
            I2C2->BitCR2.ITBUFEN = 0;
            I2C2->BitCR2.ITEVTEN = 0;
            s_staDev[1].blIrpDoing = false;
        }
        else {
            if(s_staDev[1].pbyDatRx) *s_staDev[1].pbyDatRx ++ = I2C2->DR;
            I2C2->DR = s_staDev[1].pbyDat[s_staDev[1].dwTrsCnt];
            s_staDev[1].dwTrsCnt ++;
        }
    }
}
#endif

#if(CONFIG_I2C3)
void I2C3_EV_IRQHandler(void)
{
    s_staDev[2].unSr1.wSR1 = I2C3->SR1;
    s_staDev[2].wSr2 = I2C3->SR2;

    // 产生起始条件
    if(s_staDev[2].unSr1.BitSR1.SB) {
        I2C3->DR = s_staDev[2].pbyDat[0];
        s_staDev[2].dwTrsCnt = 1;
        return ;
    }

    if(s_staDev[2].unSr1.BitSR1.ADDR) {
        I2C3->DR = s_staDev[2].pbyDat[1];
        s_staDev[2].dwTrsCnt = 2;
        return;
    }

    if(s_staDev[2].unSr1.BitSR1.TXE) {
        if(s_staDev[2].dwTrsCnt >= s_staDev[2].dwLen) {
            I2C3->BitCR1.STOP = 1;
            I2C3->BitCR2.ITBUFEN = 0;
            I2C3->BitCR2.ITEVTEN = 0;
            s_staDev[2].blIrpDoing = false;
        }
        else {
            if(s_staDev[2].pbyDatRx) *s_staDev[2].pbyDatRx ++ = I2C3->DR;
            I2C3->DR = s_staDev[2].pbyDat[s_staDev[2].dwTrsCnt];
            s_staDev[2].dwTrsCnt ++;
        }
    }
}
#endif
#endif

#if(CONFIG_I2C1)
void I2C1_ER_IRQHandler(void)
{
    // 解除锁定
    I2C1->BitCR1.STOP = 1;
    I2C1->BitCR2.ITBUFEN = 0;
    I2C1->BitCR2.ITEVTEN = 0;
    I2C1->SR1 &= 0x00FF;
#if(CONFIG_I2C_IRP)
    s_staDev[0].blIrpDoing = false;
#endif

    s_staDev[0].blErr = true;
}
#endif

#if(CONFIG_I2C2)
void I2C2_ER_IRQHandler(void)
{
        // 解除锁定
    I2C2->BitCR1.STOP = 1;
    I2C2->BitCR2.ITBUFEN = 0;
    I2C2->BitCR2.ITEVTEN = 0;
    I2C2->SR1 &= 0x00FF;
#if(CONFIG_I2C_IRP)
    s_staDev[1].blIrpDoing = false;
#endif

    s_staDev[1].blErr = true;
}
#endif

#if(CONFIG_I2C3)
void I2C3_ER_IRQHandler(void)
{
        // 解除锁定
    I2C3->BitCR1.STOP = 1;
    I2C3->BitCR2.ITBUFEN = 0;
    I2C3->BitCR2.ITEVTEN = 0;
    I2C3->SR1 &= 0x00FF;
#if(CONFIG_I2C_IRP)
    s_staDev[2].blIrpDoing = false;
#endif

    s_staDev[2].blErr = true;
}
#endif


static const DevI2cTdf s_Dev = {
    Config,
    Trs,
    SaTrs,
    CheckOnline,

#if(CONFIG_I2C_READ)
    SaRead,
    Read,
    ReadData,
#endif

#if(CONFIG_I2C_IO)
    SetPin,
    TrsIo,
#endif

#if(CONFIG_I2C_DMA)
    TrsDma,
    ConfigDmaTx,
#if(CONFIG_I2C_RX_DMA)
    ConfigDmaRx,
#endif
#endif

#if(CONFIG_I2C_IRP)
    TrsIrq,
#endif
};

/*******************************************************************************
函数功能：
*   @ I2c设备初始化
参数描述：
*   @ 无
返回值描述：
*   @ I2C操作方法集合
说明：
*   @ 无
*******************************************************************************/
const DevI2cTdf * I2cInit( void )
{
    RccInit();
#if(CONFIG_I2C1)
    s_staDev[0].pstDev = I2C1;
#endif
#if(CONFIG_I2C2)
    s_staDev[1].pstDev = I2C2;
#endif
#if(CONFIG_I2C3)
    s_staDev[2].pstDev = I2C3;
#endif

//中断配置放置config中，未配置IO口就打开中断当硬件异常是会直接进中断导致错误。
//所以位置完IO口再配置中断比较稳妥 其他外设应该也有同样问题，遇到再改 20210716 14:21 lixx
#if 0
#if(CONFIG_I2C_IRP)
    #if(CONFIG_I2C1)
    I2C1->BitCR2.ITERREN = 1;
    DevIo->NvicUp(I2C1_EV_IRQn, 11, 0);
   /DevIo->NvicUp(I2C1_ER_IRQn, 11, 0);
    #endif
    #if(CONFIG_I2C2)
    I2C2->BitCR2.ITERREN = 1;
    DevIo->NvicUp(I2C2_EV_IRQn, 11, 0);
    DevIo->NvicUp(I2C2_ER_IRQn, 11, 0);
    #endif
    #if(CONFIG_I2C3)
    I2C3->BitCR2.ITERREN = 1;
    DevIo->NvicUp(I2C3_EV_IRQn, 11, 0);
    DevIo->NvicUp(I2C3_ER_IRQn, 11, 0);
    #endif
#endif

#if(CONFIG_I2C1)
    I2C1->BitCR2.ITERREN = 1;
    DevIo->NvicUp(I2C1_ER_IRQn, 11, 0);
#endif
#if(CONFIG_I2C2)
    I2C2->BitCR2.ITERREN = 1;
    DevIo->NvicUp(I2C2_ER_IRQn, 11, 0);
#endif
#if(CONFIG_I2C3)
    I2C3->BitCR2.ITERREN = 1;
    DevIo->NvicUp(I2C3_ER_IRQn, 11, 0);
#endif
#endif
    return &s_Dev;
}
#endif
