/*
 * I2C.c
 *
 *  Created on: Mar 20, 2022
 *      Author: USER
 */
#include "I2C.h"
#include "stm32f4xx.h"


void I2C_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;														 	/* Enable GPIOB clock */
	RCC->APB1ENR |=RCC_APB1ENR_I2C1EN; 																/* Enable I2C1 AND I2Cclock */
	GPIOB->AFR[1] &=~(GPIO_AFRH_AFSEL8_2|GPIO_AFRH_AFSEL9_2);      		/* PB8, PB9 I2C1 SCL, SDA */
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_2|GPIO_AFRH_AFSEL9_2;
	GPIOB->MODER  &= ~(GPIO_MODER_MODE8_1|GPIO_MODER_MODE9_1);				/* PB8, PB9 use alternate function */
	GPIOB->MODER  |= (GPIO_MODER_MODE8_1|GPIO_MODER_MODE9_1);
	GPIOB->OTYPER |= (GPIO_OTYPER_OT8|GPIO_OTYPER_OT9);        				/* output open-drain */
	GPIOB->PUPDR  &=~(GPIO_PUPDR_PUPD8_0|GPIO_PUPDR_PUPD9_0);        	/* with pull-ups */
	GPIOB->PUPDR  |= (GPIO_PUPDR_PUPD8_0|GPIO_PUPDR_PUPD9_0);
	I2C1->CR1      = I2C_CR1_SWRST;     															/* software reset I2C1 */
	I2C1->CR1     &= ~I2C_CR1_SWRST;           												/* out of reset */
	I2C1->CR2      = (I2C_CR2_FREQ & 0x2a);  													/* peripheral clock is 16 MHz*/
	I2C1->CCR      = (I2C_CCR_CCR & 0x8023);               						/* standard mode, 100kHz clock */
	I2C1->TRISE    =(I2C_TRISE_TRISE & 0xd0);                 				/* maximum rise time */
	I2C1->CR1     |= I2C_CR1_PE;            													/* enable I2C1module */
}


/* this funtion reads a byte of data from the memory location
maddr of a device with I2C slave device address saddr. * For
simplicity, no error checking or error report is done. */


int I2C1_byteRead(char saddr, char maddr,uint8_t* data)
{
	volatile uint32_t tmp=0;
	while (I2C1->SR2 & I2C_SR2_BUSY);       /* wait until bus not busy */
	I2C1->CR1 |= I2C_CR1_START;             /* generate start */
	while (!(I2C1->SR1 & I2C_SR1_SB));      /* wait until start flag is set */
	I2C1->DR = (uint8_t)saddr<<1;           /* transmit slave address + Write */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));    /* wait until addr flag is set */
	tmp = I2C1->SR1;                				/* clear addr flag */
	tmp = I2C1->SR2;  
	while (!(I2C1->SR1 & I2C_SR1_TXE));   	/* wait until data register empty */
	I2C1->DR = maddr;               				/* send memory address */
	while (!(I2C1->SR1 & I2C_SR1_TXE));    	/* wait until data register empty */
	I2C1->CR1 |= 0x100;             				/* generate restart */
	while (!(I2C1->SR1 & I2C_SR1_SB));      /* wait until start flag is set */
	I2C1->DR =(uint8_t)(saddr<< 1|0x1);     /*transmit slave address + Read */
	while (!(I2C1->SR1 & I2C_SR1_ADDR));    /* wait until addr flag is set */
	I2C1->CR1 &= ~I2C_CR1_ACK;            	/* Disable Acknowledge */
	tmp = I2C1->SR1;                				/* clear addr flag */
	tmp = I2C1->SR2;                				/* clear addr flag */
	while (!(I2C1->SR1 & I2C_SR1_RXNE));    /* Wait until RXNE flag is set */
	I2C1->CR1 |= I2C_CR1_STOP;            	/* generate stop after data received */
	*data++ = (uint8_t)I2C1->DR;            /* Read data from DR */
	return 0;
}
int I2C1_byteWrite(char saddr, char maddr, uint8_t* data)
{
	volatile int tmp=0;
	while (I2C1->SR2 & I2C_SR2_BUSY);      /* wait until bus not busy */
	I2C1->CR1 |= 0x100; 										/* generate start */
	while (!(I2C1->SR1 & 1));/* wait until start flag is set */
	I2C1->DR = saddr << 1; /* transmit slave address */
	while(!(I2C1->SR1 & 2)); /* wait until addr flag is set */
	tmp = I2C1->SR2; /*clear addr flag */
	while (!(I2C1->SR1 & 0x80)); /* wait until data register empty */
	I2C1->DR = maddr; /* send memory address */
	while (!(I2C1->SR1 & 0x80)); /* wait until data register empty */
	I2C1->DR = *data; /* transmit data */
	while (!(I2C1->SR1 & 4)); /* wait until transfer finished */
	I2C1->CR1 |= 0x200; /* generate stop */
	return 0;
}
