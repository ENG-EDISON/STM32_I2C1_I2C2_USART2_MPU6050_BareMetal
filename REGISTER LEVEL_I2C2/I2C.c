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
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; 															/* Enable I2C1 AND I2Cclock */
	GPIOB->AFR[0] &=~(0x9000);      																	
	GPIOB->AFR[0] |=  0x9000;      																		/* Enable PB3 TO AF9  */
	
	GPIOB->AFR[1] &=~ 0x400;
	GPIOB->AFR[1] |= 0x400;																						/* Enable PB10 TO AF4 */
	GPIOB->MODER  &= ~(GPIO_MODER_MODER3_1|GPIO_MODER_MODE10_1);			/* PB2, PB10 use alternate function */
	GPIOB->MODER  |= (GPIO_MODER_MODER3_1|GPIO_MODER_MODE10_1);
	GPIOB->OTYPER |= (GPIO_OTYPER_OT3|GPIO_OTYPER_OT10);        			/* output open-drain for PB3 AND PB10*/
	GPIOB->PUPDR  &=~(GPIO_PUPDR_PUPD3_0|GPIO_PUPDR_PUPD10_0);        /* with pull-ups for PB3 AND PB10 */
	GPIOB->PUPDR  |= (GPIO_PUPDR_PUPD3_0|GPIO_PUPDR_PUPD10_0);
	I2C2->CR1      = I2C_CR1_SWRST;     															/* software reset I2C1 */
	I2C2->CR1     &= ~I2C_CR1_SWRST;           												/* out of reset */
	I2C2->CR2      = (I2C_CR2_FREQ & 0x2a);  													/* peripheral clock is 16 MHz*/
	I2C2->CCR      = (I2C_CCR_CCR & 0x8023);               						/* standard mode, 400kHz clock */
	I2C2->TRISE    =(I2C_TRISE_TRISE & 0xd0);                 				/* maximum rise time */
	I2C2->CR1     |= I2C_CR1_PE;            													/* enable I2C1module */
}


/* this funtion reads a byte of data from the memory location
maddr of a device with I2C slave device address saddr. * For
simplicity, no error checking or error report is done. */


int I2C1_byteRead(char saddr, char maddr,uint8_t* data)
{
	volatile uint32_t tmp=0;
	while (I2C2->SR2 & I2C_SR2_BUSY);       /* wait until bus not busy */
	I2C2->CR1 |= I2C_CR1_START;             /* generate start */
	while (!(I2C2->SR1 & I2C_SR1_SB));      /* wait until start flag is set */
	I2C2->DR = (uint8_t)saddr<<1;           /* transmit slave address + Write */
	while (!(I2C2->SR1 & I2C_SR1_ADDR));    /* wait until addr flag is set */
	tmp = I2C2->SR1;                				/* clear addr flag */
	tmp = I2C2->SR2;  
	while (!(I2C2->SR1 & I2C_SR1_TXE));   	/* wait until data register empty */
	I2C2->DR = maddr;               				/* send memory address */
	while (!(I2C2->SR1 & I2C_SR1_TXE));    	/* wait until data register empty */
	I2C2->CR1 |= 0x100;             				/* generate restart */
	while (!(I2C2->SR1 & I2C_SR1_SB));      /* wait until start flag is set */
	I2C2->DR =(uint8_t)(saddr<< 1|0x1);     /*transmit slave address + Read */
	while (!(I2C2->SR1 & I2C_SR1_ADDR));    /* wait until addr flag is set */
	I2C2->CR1 &= ~I2C_CR1_ACK;            	/* Disable Acknowledge */
	tmp = I2C2->SR1;                				/* clear addr flag */
	tmp = I2C2->SR2;                				/* clear addr flag */
	while (!(I2C2->SR1 & I2C_SR1_RXNE));    /* Wait until RXNE flag is set */
	I2C2->CR1 |= I2C_CR1_STOP;            	/* generate stop after data received */
	*data++ = (uint8_t)I2C2->DR;            /* Read data from DR */
	return 0;
}
int I2C1_byteWrite(char saddr, char maddr, uint8_t* data)
{
	volatile int tmp=0;
	while (I2C2->SR2 & I2C_SR2_BUSY);     	/* wait until bus not busy */
	I2C2->CR1 |= I2C_CR1_START; 						/* generate start */
	while (!(I2C2->SR1 & I2C_SR1_SB));			/* wait until start flag is set */
	I2C2->DR = saddr << 1; 									/* transmit slave address */
	while(!(I2C2->SR1 & I2C_SR1_ADDR));			/* wait until addr flag is set */
	tmp = I2C2->SR1; 												/*clear addr flag */
	tmp = I2C2->SR2; 												/*clear addr flag */
	while (!(I2C2->SR1 & I2C_SR1_TXE)); 		/* wait until data register empty */
	I2C2->DR = maddr; 											/* send memory address */
	while (!(I2C2->SR1 & I2C_SR1_TXE)); 		/* wait until data register empty */
	I2C2->DR = *data; 											/* transmit data */
	while (!(I2C2->SR1 & I2C_SR1_BTF)); 		/* wait until transfer finished */
	I2C2->CR1 |= 0x200; 										/* generate stop */
	return 0;
}
