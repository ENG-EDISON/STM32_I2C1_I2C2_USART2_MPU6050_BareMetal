/*
 * I2C.h
 *
 *  Created on: Mar 20, 2022
 *      Author: USER
 */
#include <stdint.h>
#include "stm32f4xx.h"
#include <stdlib.h>

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

void I2C_init(void);
int I2C1_byteRead(char saddr, char maddr, uint8_t* data);
int I2C1_byteWrite(char saddr, char maddr, uint8_t* data);


#endif /* SRC_I2C_H_ */
