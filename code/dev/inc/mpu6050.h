/*******************************************************************************
文件名称：    cat3626.h
文件目录：    $PROJ_DIR$/usr/Drive/inc
项目名称：    事业一部技术平台
创建者：      PS
创建日期：    2022.10.14
程序用途：    Mpu6050驱动程序头文件

版权声明：    Copyright urit Co.,Ltd.All Right Reserved
第一次修改：
修改人：
修改内容：
日期：
*******************************************************************************/
#ifndef __MPU6050_H
#define __MPU6050_H

/*******************************************************************************
Include files头文件
*******************************************************************************/

/*******************************************************************************
Constant and Macro Definition常量以及宏定义
*******************************************************************************/
#define DEV_ADDR 0xD0 // 6050 器件地址
//-----------------------------------------
// 定义MPU6050内部地址
//-----------------------------------------
#define SMPLRT_DIV 0x19 //陀螺仪采样率，典型值：0x07(125Hz)
#define CONFIG 0x1A //低通滤波频率，典型值：0x06(5Hz)
#define GYRO_CONFIG 0x1B //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG 0x1C //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

/* 加速度相关寄存器地址 */
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

/* 温度相关寄存器地址 */
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42

/* 陀螺仪相关寄存器地址 */
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1 0x6B  //电源管理，典型值：0x00(正常启用)
#define WHO_AM_I 0x75 //IIC地址寄存器(默认数值0x68，只读)
#define SlaveAddress 0xD0 //IIC写入时的地址字节数据，+1为读取

/*******************************************************************************
数据结构与数据类型定义
*******************************************************************************/
typedef struct {
    uint8_t byI2cNo;
    unsigned int *pwdI2cEn;
}Mpu6050InitTdf;

/* 坐标角度结构体 */
typedef struct Angle
{
    double X_Angle;
    double Y_Angle;
    double Z_Angle;
} MPU6050_Angle;

typedef struct {
    void (*MPU6050_Init)(uint8_t byDev);
    int16_t (*MPU6050_Get_Data)(uint8_t byDev,uint8_t byChn);
    void(*MPU6050_Get_Angle)(MPU6050_Angle *data);
}DevMpu6050Tdf;

extern MPU6050_Angle data;
/*******************************************************************************
宏定义函数申明
*******************************************************************************/
void Mpu6050_Config(uint8_t byDev, uint8_t (*ReadReg)(uint8_t), uint8_t (*WriteReg)(uint8_t, uint8_t),
                        uint8_t (*ReadMulti)(uint8_t byStReg, uint8_t *buf, uint8_t bytes),
                        uint8_t (*WriteMulti)(uint8_t byStReg, uint8_t *buf, uint8_t bytes));
/*******************************************************************************
输出函数申明
*******************************************************************************/
const DevMpu6050Tdf * Mpu6050Init( Mpu6050InitTdf *pstInit );

#endif