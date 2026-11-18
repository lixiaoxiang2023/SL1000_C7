/*******************************************************************************
文件名称：   I2c_io.c
文件目录：   $PROJ_DIR$/../code/usr/dev
项目名称：
创建者：
创建日期：   xxxxx
程序用途：   模拟i2c驱动射频卡

版权声明：Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修 改 人：
修改内容：
修改日期：
*******************************************************************************/

/*******************************************************************************
包含头文件
*******************************************************************************/
#include "devinit.h"
#include "I2c_io.h"

/*******************************************************************************
常量及宏定义
*******************************************************************************/
#define I2C_DEV_NUM         4
/* I2C操作定义 */
#define I2C_SCL_Reset(DEV)  DevIo->Write(stDevCtr[DEV].Scl_Port, stDevCtr[DEV].Scl_Pin, 0)
#define I2C_SCL_Set(DEV)    DevIo->Write(stDevCtr[DEV].Scl_Port, stDevCtr[DEV].Scl_Pin, 1)
#define I2C_SDA_Reset(DEV)  DevIo->Write(stDevCtr[DEV].Sda_Port, stDevCtr[DEV].Sda_Pin, 0)
#define I2C_SDA_Set(DEV)    DevIo->Write(stDevCtr[DEV].Sda_Port, stDevCtr[DEV].Sda_Pin, 1)
#define I2C_SDA_Read(DEV)   DevIo->Read (stDevCtr[DEV].Sda_Port, stDevCtr[DEV].Sda_Pin)
#define I2C_Delay(DEV)      stDevCtr[DEV].delay()

/* I2C应答信号重定义 */
#define I2C_Send_ACK(DEV)   I2C_SendBit_0(DEV)
#define I2C_Send_NAK(DEV)   I2C_SendBit_1(DEV)
#define I2C_Check_Ack(DEV)  I2C_ReadBit(DEV)

/* I2C设备读写命令 */
#define I2C_DEV_W           (0x00)
#define I2C_DEV_R           (0x01)

/*******************************************************************************
内部函数申明
*******************************************************************************/
static void I2C_Config(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, void (*delay)(void));
static uint8_t I2C_WriteReg(uint8_t byDev, uint8_t byAddr8, uint8_t byReg, uint8_t data);
static uint8_t I2C_ReadReg(uint8_t byDev, uint8_t byAddr8, uint8_t byReg);
static uint8_t I2C_WriteMultiRegs(uint8_t byDev, uint8_t byAddr8, \
                                   uint8_t byStReg, uint8_t *buf, uint8_t bytes);
static uint8_t I2C_ReadMultiRegs(uint8_t byDev, uint8_t byAddr8, uint8_t byStReg, uint8_t *buf, uint8_t bytes);

/*******************************************************************************
全局变量定义
*******************************************************************************/
const struct stI2C_Dev_t stI2C_Dev = {
    .Config     = I2C_Config,
    .WriteReg   = I2C_WriteReg,
    .ReadReg    = I2C_ReadReg,
    .WriteMultiRegs = I2C_WriteMultiRegs,
    .ReadMultiRegs  = I2C_ReadMultiRegs,
};

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
static struct {
    uint8_t Scl_Port;
    uint8_t Scl_Pin;
    uint8_t Sda_Port;
    uint8_t Sda_Pin;
    void (*delay)(void);
} stDevCtr[I2C_DEV_NUM] = {0};

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
    for (__IO uint8_t i = 0; i < 16; i++) {
        ;
    }
}

/*******************************************************************************
函数功能：
*       @ 配置I2C
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：
*       @ 无
*******************************************************************************/
static void I2C_Config(uint8_t byDev, uint8_t Scl_Port, uint8_t Scl_Pin, uint8_t Sda_Port, uint8_t Sda_Pin, void (*delay)(void))
{
    stDevCtr[byDev].Scl_Port = Scl_Port;
    stDevCtr[byDev].Scl_Pin  = Scl_Pin;
    stDevCtr[byDev].Sda_Port = Sda_Port;
    stDevCtr[byDev].Sda_Pin  = Sda_Pin;

    DevIo->Config(stDevCtr[byDev].Scl_Port, stDevCtr[byDev].Scl_Pin, 1, Mode_Out_OD);
    DevIo->Config(stDevCtr[byDev].Sda_Port, stDevCtr[byDev].Sda_Pin, 1, Mode_Out_OD);
    stDevCtr[byDev].delay = (delay == NULL) ? I2C_delay : delay;

    I2C_SDA_Set(byDev);
    I2C_SCL_Set(byDev);
}

/*******************************************************************************
函数功能：
*       @ Start
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：SDA ---\______
          SCL -----\____
*******************************************************************************/
static void I2C_Start(uint8_t byDev)
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
*       @ Stop
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：SDA ______/----
          SCL ___/-------
*******************************************************************************/
static void I2C_Stop(uint8_t byDev)
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
static inline
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
static inline
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
*       @ 读8bit数据
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：
*       @ 无
*******************************************************************************/
static uint8_t I2C_ReadByte(uint8_t byDev)
{
    uint8_t byReadData = 0;

    for (uint8_t i = 0; i < 8; i++) {
        byReadData <<= 1;
        byReadData |= I2C_ReadBit(byDev) ? 0x01 : 0x00;
    }
    return byReadData;
}

/*******************************************************************************
函数功能：
*       @ I2C读寄存器
参数描述：
*       @ 无
返回描述：
*       @ 无
说    明：
*       @ 无
*******************************************************************************/
static uint8_t I2C_ReadReg(uint8_t byDev, uint8_t byAddr8, uint8_t byReg)
{
    uint8_t data;

    I2C_Start(byDev);
    I2C_SendByte(byDev, I2C_DEV_W | byAddr8);
    I2C_Check_Ack(byDev);
    I2C_SendByte(byDev, byReg);
    I2C_Check_Ack(byDev);

    I2C_Start(byDev);
    I2C_SendByte(byDev, I2C_DEV_R | byAddr8);
    I2C_Check_Ack(byDev);
    data = I2C_ReadByte(byDev);
    I2C_Send_NAK(byDev);
    I2C_Stop(byDev);

    return data;
}

/*******************************************************************************
函数功能：
*       @ I2C读多个寄存器
参数描述：
*       @ 无
返回描述：
*       @ 0,写成功；1，失败
说    明：
*       @ 无
*******************************************************************************/
static uint8_t I2C_ReadMultiRegs(uint8_t byDev, uint8_t byAddr8, uint8_t byStReg, uint8_t *buf, uint8_t bytes)
{
    I2C_Start(byDev);
    I2C_SendByte(byDev, I2C_DEV_W | byAddr8);
    if (I2C_Check_Ack(byDev)) {
        I2C_Stop(byDev);
        return 1;
    }
    I2C_SendByte(byDev, byStReg);
    I2C_Check_Ack(byDev);
    I2C_Delay(byDev);

    I2C_Start(byDev);
    I2C_SendByte(byDev, I2C_DEV_R | byAddr8);
    I2C_Check_Ack(byDev);

    *buf++ = I2C_ReadByte(byDev);
    for (uint8_t i = 1; i < bytes; i++) {
        I2C_Send_ACK(byDev);
        *buf++ = I2C_ReadByte(byDev);
    }
    I2C_Send_NAK(byDev);
    I2C_Stop(byDev);

    return 0;
}

/*******************************************************************************
函数功能：
*       @ I2C写寄存器
参数描述：
*       @ 无
返回描述：
*       @ 0,写成功；1，失败
说    明：
*       @ 无
*******************************************************************************/
static uint8_t I2C_WriteReg(uint8_t byDev, uint8_t byAddr8, uint8_t byReg, uint8_t data)
{
    uint8_t byRe = 0;

    I2C_Start(byDev);
    I2C_SendByte(byDev, I2C_DEV_W | byAddr8);
    I2C_Check_Ack(byDev);
    I2C_SendByte(byDev, byReg);
    I2C_Check_Ack(byDev);
    I2C_SendByte(byDev, data);
    byRe = I2C_Check_Ack(byDev);
    I2C_Stop(byDev);

    return byRe;
}

/*******************************************************************************
函数功能：
*       @ I2C写多个寄存器
参数描述：
*       @ 无
返回描述：
*       @ 0,写成功；1，失败
说    明：
*       @ 无
*******************************************************************************/
static uint8_t I2C_WriteMultiRegs(uint8_t byDev, uint8_t byAddr8, uint8_t byStReg, uint8_t *buf, uint8_t bytes)
{
    I2C_Start(byDev);
    I2C_SendByte(byDev, I2C_DEV_W | byAddr8);
    I2C_Check_Ack(byDev);
    I2C_SendByte(byDev, byStReg);
    I2C_Check_Ack(byDev);

    for (uint8_t i = 0; i < bytes; i++) {
        I2C_SendByte(byDev, *buf++);
        if (I2C_Check_Ack(byDev)) {
            I2C_Stop(byDev);
            return 1;
        }
    }

    I2C_Stop(byDev);

    return 0;
}