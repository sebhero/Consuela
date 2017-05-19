/*
 * TwiComHandler.h
 *
 * Created: 2017-05-12 09:59:23
 *  Author: Sebastian Boreback
 * Version 1.3
 * V1.3 added nav and rotate
 * v1.1 Changed so that the enums starts from 2
 * v1.2 added CMD for NAV and updates with new commands for rotate left and right
 */


#ifndef TWICOMHANDLER_H_
#define TWICOMHANDLER_H_


#include <stdint.h>

#define TWI_STANDARD_MODE_SPEED 100000U //normal speed of twi clk
#define TWI_FAST_MODE_SPEED 400000U // fast speed for twi, = more chance for NACK and errors

#define SLAVE_ADDR_NAV 0x03 //twi slave address for navigation
#define SLAVE_ADDR_ARM 0x02 //twi slave address for navigation

uint32_t SLAVE_ADDR;//twi slave address

#define TWI_SPEED TWI_STANDARD_MODE_SPEED
//set the twi port/pins to use
#define TWI_PORT TWI1

typedef enum {
	SOCK = 2,
	SQUARE = 3,
	GLASS = 4,
	BOXGOAL = 5
} Object;

typedef struct {
	//distances are in cm from center of robot
	uint8_t boxDistance;
	//angles in degrees
	uint8_t boxAngle;
	//distances are in cm from center of robot
	uint8_t objectDistance;
	uint8_t objectAngle;
	//Boolean. true that arm can collect all obj. then dropoff
	//false if need to dropoff each obj. after pickup
	uint8_t collectAll;
	//just to check if the struct is set
	uint8_t hasData;
} arminfo_t;

//Holds info about each object position
typedef struct {
	//the type of object
	Object theObject;
	// x position of object
	int16_t xpos;
	//y position of object
	int16_t ypos;
} objectinfo_t;


//TWI state
typedef enum  {

	TWI_CMD_ARM_INIT = 0x20,
	TWI_CMD_DROPOFF_START = 0x21,
	TWI_CMD_PICKUP_START = 0x22,
	TWI_CMD_DROPOFF_STATUS = 0x23,
	TWI_CMD_PICKUP_STATUS = 0x24,
	TWI_CMD_ERROR = 0x25,
} TwiCmd;

typedef enum  {
	//retuns distance to box goal, and angle to set robot for dropoff
	TWI_CMD_ARM_REQ_BOX_INFO = 2,
	//returns distance to object and angle for collect
	TWI_CMD_ARM_REQ_OBJ_INFO = 3,
	TWI_CMD_ARM_REQ_COLLECT_INFO = 4
} TwiCmdInitReq;

typedef enum  {
	PICKUP_DONE = 2,
	PICKUP_FORWARD = 3,
	PICKUP_BACKWARD = 4,
	PICKUP_RUNNING = 5,
	PICKUP_FAILED = 6,
	PICKUP_DONE_DRIVE = 7,
	PICKUP_IDLE = 8,
	PICKUP_ROTATE_L = 9,
	PICKUP_ROTATE_R = 10
} PickupStatus;

typedef enum  {
	DROPOFF_DONE = 2,
	DROPOFF_RUNNING = 3,
	DROPOFF_FAILED = 4,
	DROPOFF_IDLE = 5
} DropoffStatus;


//enum for communicationing with Navigation system
//we send 1 byte and recive 5x bytes
// [cmd][x][x][y][y]
// x,y is int16_t
typedef enum {
	XY1 = 0X50,
	XY2 = 0X51,
	SOCKETXY = 0X52,
	SQUAREXY = 0X53,
	GLASSXY = 0X54,
	BOXGOALXY = 0X55,
	TWI_ERROR=0x0
} TwiCmdNav;


#endif /* TWICOMHANDLER_H_ */