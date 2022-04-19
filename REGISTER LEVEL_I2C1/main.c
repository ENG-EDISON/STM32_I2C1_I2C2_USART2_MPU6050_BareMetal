#include "stm32f4xx.h"
#include "UART.h"
#include <stdio.h>
#include <string.h>
#include "MPU6050.h"
#include "I2C.h"
#include <stdint.h>
#include <stdlib.h>

static char Buffer[100];
extern MPU6050_GetData MPU6050_Data;

int main(void)
{
	I2C_init();
	MPU6050_Init();
	Usart2_Init();
	/* configure PA5 for the green LED (LD2) */
	RCC->AHB1ENR |= 1; /* enable GPIOA clock */
	GPIOA->MODER &=~ 0x00000C00; /* clear pin mode */
	GPIOA->MODER |= 0x00000400; /* Set pin to output mode */
	while (1)
	{
		Get_Accelerometer(&MPU6050_Data);
		Get_Gyroscope();
		sprintf(Buffer,"X=%f		Y=%f	 	Z=%f\t\t",MPU6050_Data.X_Axis,MPU6050_Data.Y_Axis,MPU6050_Data.Z_Axis);
		Uart2_Send(Buffer);
		sprintf(Buffer,"GX=%f		GY=%f 	GZ=%f\t\t",MPU6050_Data.GX_Axis,MPU6050_Data.GY_Axis,MPU6050_Data.GZ_Axis);
		Uart2_Send(Buffer);
		sprintf(Buffer,"Roll=%f		Pitch=%f  Yaw=%f\r\n\n",MPU6050_Data.Roll,MPU6050_Data.Pitch,MPU6050_Data.Yaw);
		Uart2_Send(Buffer);
	}

}


