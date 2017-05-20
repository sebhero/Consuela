/*
 * Com.h
 *
 * Created: 2017-05-09 12:40:28
 *  Author: trevl
 */ 


#ifndef COM_H_
#define COM_H_

#include "asf.h"
#include <stdint.h>

#define TWI_STANDARD_MODE_SPEED 100000U
#define TWI_FAST_MODE_SPEED 400000U

#define SLAVE_ADDR 0x02 //twi slave
//#define TWI_SPEED 100000
#define TWI_SPEED TWI_STANDARD_MODE_SPEED

void initTwi(void);

//Send data to Arm with TWI
//data holds the data, dataLength is how many bytes the data is.
void sendArm(uint8_t* data,int dataLength);
void sendArmCmd(uint8_t cmd);


//Recive data from ARM with TWI
//recives and prints the data out on Uart.
//packageSize how many bytes expected to recive
//returns the data recived as array.
uint8_t* reciveFromArm(uint8_t packageSize);


#endif /* COM_H_ */