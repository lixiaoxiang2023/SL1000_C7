/*******************************************************************************
文件名称：    mpu6050.c
目录：        $PROJ_DIR$/../code/dev
项目名称：    事业一部技术平台
创建者：      ps
创建日期：    2022.10.14
程序用途：    用于cat3626的LED驱动

版权声明：
Copyright urit Co.,Ltd.
All Right Reserved

修改及维护信息

*******************************************************************************/

/*******************************************************************************
Include file
*******************************************************************************/
#include "devplat.h"
#include "I2c_io.h"
#include <math.h>
#include "mpu6050.h"

#if(CONFIG_MPU6050)
/*******************************************************************************
常量定义
*******************************************************************************/
/* 传感器数据修正值（消除芯片固定误差，根据硬件进行调整） */
#define X_ACCEL_OFFSET 0//-600
#define Y_ACCEL_OFFSET 0//-100
#define Z_ACCEL_OFFSET 0//2900
#define X_GYRO_OFFSET 32
#define Y_GYRO_OFFSET -11
#define Z_GYRO_OFFSET 1

/*******************************************************************************
内部全局变量申明
*******************************************************************************/
//static struct {
//    unsigned int *pwdI2cEn;
//    unsigned char byI2cNo;
//}s_stDev6050;

MPU6050_Angle data;
static struct {
    uint8_t (*ReadReg)(uint8_t byReg);
    uint8_t (*WriteReg)(uint8_t byReg, uint8_t byData);
    uint8_t (*ReadMulti)(uint8_t byStReg, uint8_t *buf, uint8_t bytes);
    uint8_t (*WriteMulti)(uint8_t byStReg, uint8_t *buf, uint8_t bytes);
} stDevCtr[MAX_MPU];

/*******************************************************************************
函数功能：
*   @ 配置底层寄存器读写
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：无
*******************************************************************************/
static void Mpu6050_Config(uint8_t byDev, uint8_t (*ReadReg)(uint8_t), uint8_t (*WriteReg)(uint8_t, uint8_t),
                        uint8_t (*ReadMulti)(uint8_t byStReg, uint8_t *buf, uint8_t bytes),
                        uint8_t (*WriteMulti)(uint8_t byStReg, uint8_t *buf, uint8_t bytes))
{
    stDevCtr[byDev].ReadReg     = ReadReg;
    stDevCtr[byDev].WriteReg    = WriteReg;
    stDevCtr[byDev].ReadMulti   = ReadMulti;
    stDevCtr[byDev].WriteMulti  = WriteMulti;
}

/******************************************************************************
* 函数介绍： MPU6050 初始化函数
* 输入参数： 无
* 输出参数： 无
* 返回值 ： 无
* 备 注： 配置 MPU6050 测量范围：± 2000 °/s ± 2g
******************************************************************************/
static void MPU6050_Init(uint8_t byDev)
{
//    unsigned char abySet[3] = {0, 0, 0};
//    abySet[0] = DEV_ADDR;
//    abySet[1] = PWR_MGMT_1;
//    abySet[2] = 0;
//    DevI2c->Trs(s_stDev6050.byI2cNo, abySet, NULL, 3);
//    abySet[0] = DEV_ADDR;
//    abySet[1] = SMPLRT_DIV;
//    abySet[2] = 0x07;
//    DevI2c->Trs(s_stDev6050.byI2cNo, abySet, NULL, 3);
//    abySet[0] = DEV_ADDR;
//    abySet[1] = CONFIG;
//    abySet[2] = 0x06;
//    DevI2c->Trs(s_stDev6050.byI2cNo, abySet, NULL, 3);
//    abySet[0] = DEV_ADDR;
//    abySet[1] = GYRO_CONFIG;
//    abySet[2] = 0x18;
//    DevI2c->Trs(s_stDev6050.byI2cNo, abySet, NULL, 3);
//    abySet[0] = DEV_ADDR;
//    abySet[1] = ACCEL_CONFIG;
//    abySet[2] = 0x01;
//    DevI2c->Trs(s_stDev6050.byI2cNo, abySet, NULL, 3);
    //I2C_Init();  // I2C 初始化
    stDevCtr[byDev].WriteReg(PWR_MGMT_1, 0x00); //解除休眠状态
    stDevCtr[byDev].WriteReg(SMPLRT_DIV, 0x07); //陀螺仪采样率，典型值：0x07(125Hz)
    stDevCtr[byDev].WriteReg(CONFIG, 0x06);     //低通滤波频率，典型值：0x06(5Hz)
    stDevCtr[byDev].WriteReg(GYRO_CONFIG, 0x18);//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
    stDevCtr[byDev].WriteReg(ACCEL_CONFIG, 0x01);//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
}

/******************************************************************************
* 函数介绍： 连续读两个寄存器并合成 16 位数据
* 输入参数： regAddr：数据低位寄存器地址
* 输出参数： 无
* 返回值 ： data：2 个寄存器合成的 16 位数据
******************************************************************************/
static int16_t MPU6050_Get_Data(uint8_t byDev,uint8_t regAddr)
{
    uint8_t Data_H, Data_L;
    uint16_t data;
//    DevI2c->Read(s_stDev6050.byI2cNo,&regAddr,2,&Data_H,1);
//    DevI2c->Read(s_stDev6050.byI2cNo,&regAddr+1,2,&Data_L,1);
    Data_H = stDevCtr[byDev].ReadReg(regAddr);
    Data_L = stDevCtr[byDev].ReadReg(regAddr+1);
    data = (Data_H << 8) | Data_L;  // 合成数据
    return data;
}

/******************************************************************************
* 函数介绍： 计算 x, y, z 三轴的倾角
* 输入参数： 无
* 输出参数： data：角度结构体
* 返回值 ： 无
******************************************************************************/
static void MPU6050_Get_Angle(MPU6050_Angle *data)
{
    /* 计算x, y, z 轴倾角，返回弧度值*/
    data->X_Angle = acos((MPU6050_Get_Data(0,ACCEL_XOUT_H) + X_ACCEL_OFFSET) / 16384.0);
    data->Y_Angle = acos((MPU6050_Get_Data(0,ACCEL_YOUT_H) + Y_ACCEL_OFFSET) / 16384.0);
    data->Z_Angle = acos((MPU6050_Get_Data(0,ACCEL_ZOUT_H) + Z_ACCEL_OFFSET) / 16384.0);

    /* 弧度值转换为角度值 */
    data->X_Angle = data->X_Angle * 57.29577;
    data->Y_Angle = data->Y_Angle * 57.29577;
    data->Z_Angle = data->Z_Angle * 57.29577;
}

const static DevMpu6050Tdf s_Dev = {
    MPU6050_Init,
    MPU6050_Get_Data,
    MPU6050_Get_Angle,
};

/*******************************************************************************
函数功能：
*   @ 用底层通信接口实现mfrc设备通信接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static uint8_t Mpu_ReadReg(uint8_t byReg)
{
    uint8_t byData = stI2C_Dev.ReadReg(0, DEV_ADDR, byReg);
    return byData;
}

/*******************************************************************************
函数功能：
*   @ 用底层通信接口实现mfrc设备通信接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static uint8_t  Mpu_WriteReg(uint8_t byReg, uint8_t byData)
{
    return stI2C_Dev.WriteReg(0, DEV_ADDR, byReg, byData);
}

/*******************************************************************************
函数功能：
*   @ 用底层通信接口实现mfrc设备通信接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static uint8_t Mpu_ReadMulti(uint8_t byStReg, uint8_t *buf, uint8_t bytes)
{
    return stI2C_Dev.ReadMultiRegs(0, DEV_ADDR, byStReg, buf, bytes);
}

/*******************************************************************************
函数功能：
*   @ 用底层通信接口实现mfrc设备通信接口
参数描述：
*   @ 无
返回值描述：
*   @ 无
说明：
*   @ 无
*******************************************************************************/
static uint8_t Mpu_WriteMulti(uint8_t byStReg, uint8_t *buf, uint8_t bytes)
{
    return stI2C_Dev.WriteMultiRegs(0, DEV_ADDR, byStReg, buf, bytes);
}

/*******************************************************************************
函数功能：
*   @ 初始化MPU6050
参数描述：
*   @ 无
返回值描述：
*   @ MPU6050的操作方法集合
说明：
*******************************************************************************/
const DevMpu6050Tdf * Mpu6050Init( Mpu6050InitTdf *pstInit )
{
//    s_stDev6050.byI2cNo = pstInit->byI2cNo;
//    DevI2c->Config(pstInit->byI2cNo, 400000, true);
//    if(pstInit->pwdI2cEn) *pstInit->pwdI2cEn = 1;
    stI2C_Dev.Config (0, PE,  8, PE,  9, NULL);//PB8/9
    Mpu6050_Config(0, Mpu_ReadReg, Mpu_WriteReg, Mpu_ReadMulti, Mpu_WriteMulti);
    return &s_Dev;
}

#endif
