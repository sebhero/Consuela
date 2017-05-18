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

//TWI state
typedef enum TWI_CMD {
	//commands from arm
	TWI_CMD_FROM_ARM_ID = 				0x10,
	TWI_CMD_FROM_ARM_OBJ_POS = 			0x11,
	TWI_CMD_FROM_ARM_DROPBOX_POS = 		0x12,
	TWI_CMD_FROM_ARM_MAXSPEED = 		0x13,
	TWI_CMD_FROM_ARM_SUCCESS_LIFT = 	0x14,
	TWI_CMD_FROM_ARM_FAILD_LIFT = 		0x15,
	TWI_CMD_FROM_ARM_SUCCESS_DELIVER = 	0x16,
	TWI_CMD_FROM_ARM_FAILED_DELIVER = 	0x17,
	TWI_CMD_FROM_ARM_ADJUST_POS = 		0x18,
	TWI_CMD_FROM_ARM_ABORT_ADJUST = 	0x19,

	//commands to arm
	TWI_CMD_TO_ARM_INIT = 				0x20,
	TWI_CMD_TO_ARM_LIFT_SQUARE = 		0x21,
	TWI_CMD_TO_ARM_LIFT_GLASS = 		0x22,
	TWI_CMD_TO_ARM_LIFT_SOCK = 			0x23,
	TWI_CMD_TO_ARM_ABORT_LIFT = 		0x24,
	TWI_CMD_TO_ARM_DELIVER_OBJ = 		0x25,

	//commands from positioning
	TWI_CMD_FROM_POS_BOX = 				0x30,
	TWI_CMD_FROM_POS_ROBOT = 			0x31,
	TWI_CMD_FROM_POS_SOCK = 			0x32,
	TWI_CMD_FROM_POS_SQUARE = 			0x33,
	TWI_CMD_FROM_POS_GLASS = 			0x34,

	//commands from positioning
	TWI_CMD_TO_POS_INIT = 				0x40,
	TWI_CMD_TO_POS_ROBOT = 				0x41,
	TWI_CMD_TO_POS_SOCK =				0x42,
	TWI_CMD_TO_POS_SQUARE =				0x43,
	TWI_CMD_TO_POS_GLASS = 				0x44,
	NOT_FOUND = 						0x45
} twi_cmd;

void handleCmd(uint8_t cmd);



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