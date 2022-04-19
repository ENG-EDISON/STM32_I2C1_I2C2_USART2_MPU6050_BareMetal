/*
 * MPU6050.c
 *
 *  Created on: Mar 20, 2022
 *      Author: USER
 */
#include "MPU6050.h"
#include "I2C.h"
#include <stdint.h>
#include "math.h"
#include <stdlib.h>

#define Square(X)  X*X

#define RAD_TO_DEG        57.295779513082320876798154814105
#define MPU6050_ADDR      0x68
#define WHO_AM_I_REG      0x75
#define PWR_MGMT_1_REG    0x6B
#define SMPLRT_DIV_REG    0x19
#define ACCEL_CONFIG_REG  0x1C
#define ACCEL_XOUT_H_REG  0x3B
#define ACCEL_YOUT_H_REG  0x3D
#define ACCEL_ZOUT_H_REG  0x3F
#define TEMP_OUT_H_REG    0x41
#define GYRO_CONFIG_REG   0x1B
#define GYRO_XOUT_H_REG   0x43
#define GYRO_YOUT_H_REG   0x45
#define GYRO_ZOUT_H_REG   0X47



MPU6050_GetData MPU6050_Data;
static MPU6050_Raw MPU6050_RawData;

typedef enum Mpu_Choice
{
	Acc_Data=0,
	Gyr_Data,
}Mpu_Choice;


void MPU6050_Init(void)
{
	uint8_t Configure=0;
	I2C1_byteWrite(MPU6050_ADDR,PWR_MGMT_1_REG, &Configure);
	Configure=0x7;
	I2C1_byteWrite(MPU6050_ADDR,SMPLRT_DIV_REG, &Configure);
	Configure=0x00;
	I2C1_byteWrite(MPU6050_ADDR,ACCEL_CONFIG_REG,&Configure);
	Configure=0x00;
	I2C1_byteWrite(MPU6050_ADDR,GYRO_CONFIG_REG, &Configure);
}
void Get_Accelerometer(MPU6050_GetData *Gyroscope_Data)
{

	MPU6050_RawData.X_axis= (uint16_t)MP6050_I2cRead(ACCEL_XOUT_H_REG);
	MPU6050_RawData.Y_axis= (uint16_t)MP6050_I2cRead(ACCEL_YOUT_H_REG);
	MPU6050_RawData.Z_axis= (uint16_t)MP6050_I2cRead(ACCEL_ZOUT_H_REG);
	MPU6050_ConvertData(&MPU6050_RawData,Acc_Data);
	MPU6050_Angles(Gyroscope_Data);

}
void Get_Gyroscope(void)
{
	MPU6050_RawData.GX_axis= (uint16_t)MP6050_I2cRead(GYRO_XOUT_H_REG);
	MPU6050_RawData.GY_axis= (uint16_t)MP6050_I2cRead(GYRO_YOUT_H_REG);
	MPU6050_RawData.GZ_axis= (uint16_t)MP6050_I2cRead(GYRO_ZOUT_H_REG);
	MPU6050_ConvertData(&MPU6050_RawData,Gyr_Data);
}
uint16_t MP6050_I2cRead(char address)
{
	uint8_t I2cData[2]={0};
	I2C1_byteRead(MPU6050_ADDR,address,&I2cData[0]);
	I2C1_byteRead(MPU6050_ADDR,address+1,&I2cData[1]);
	return ((uint8_t)I2cData[0]<<8|I2cData[1]);
}

void MPU6050_ConvertData(MPU6050_Raw *SensorData,int Data)
{
	if(Data==Acc_Data)
	{
		MPU6050_Data.X_Axis= (int16_t)SensorData->X_axis/16384.0;
		MPU6050_Data.Y_Axis= (int16_t)SensorData->Y_axis/16384.0;
		MPU6050_Data.Z_Axis= (int16_t)SensorData->Z_axis/16384.0;
	}
	else
	{
		MPU6050_Data.GX_Axis= (int16_t)SensorData->GX_axis/131.0;
		MPU6050_Data.GY_Axis= (int16_t)SensorData->GY_axis/131.0;
		MPU6050_Data.GZ_Axis= (int16_t)SensorData->GZ_axis/131.0;
	}
}
void MPU6050_Angles(MPU6050_GetData *Accelerometer_Data)
{
	Accelerometer_Data->Roll  =atan (Accelerometer_Data->Y_Axis/sqrt(Square(Accelerometer_Data->X_Axis) + Square(Accelerometer_Data->Z_Axis)))*RAD_TO_DEG;
	Accelerometer_Data->Pitch =atan (Accelerometer_Data->X_Axis/sqrt(Square(Accelerometer_Data->Y_Axis) + Square(Accelerometer_Data->Z_Axis)))*RAD_TO_DEG;
	Accelerometer_Data->Yaw   = atan(Accelerometer_Data->Z_Axis/sqrt(Square(Accelerometer_Data->X_Axis) + Square(Accelerometer_Data->Z_Axis)))*RAD_TO_DEG;
}
