/*
 * TwiComHandler.h
 *
 * Created: 2017-05-12 09:59:23
 *  Author: Sebastian Boreback
 * Version 1.1
 * Changed so that the enums starts from 2
 */ 


#ifndef TWICOMHANDLER_H_
#define TWICOMHANDLER_H_


#define TWI_STANDARD_MODE_SPEED 100000U //normal speed of twi clk
#define TWI_FAST_MODE_SPEED 400000U // fast speed for twi, = more chance for NACK and errors

#define SLAVE_ADDR_NAV 0x03 //twi slave address for navigation
#define SLAVE_ADDR_ARM 0x02 //twi slave address for navigation

uint32_t SLAVE_ADDR;//twi slave address

#define TWI_SPEED TWI_STANDARD_MODE_SPEED
//set the twi port/pins to use
#define TWI_PORT TWI1

typedef struct ArmInfo{
	//distances are in cm
	uint8_t boxDistance;
	//angles in degrees
	uint8_t boxAngle;
	uint8_t objectDistance;
	uint8_t objectAngle;
	//Boolean. true that arm can collect all obj. then dropoff
	//false if need to dropoff each obj. after pickup
	uint8_t collectAll;
	//just to check if the struct is set
	uint8_t hasData;

}arminfo_t;

/** Definitions for PWM channel number */
// typedef enum _pwm_ch_t {
// 	PWM_CHANNEL_0 = 0,
// 	PWM_CHANNEL_1 = 1,
// 	PWM_CHANNEL_2 = 2,
// 	PWM_CHANNEL_3 = 3,
// 	#if (SAM3XA)
// 	PWM_CHANNEL_4 = 4,
// 	PWM_CHANNEL_5 = 5,
// 	PWM_CHANNEL_6 = 6,
// 	PWM_CHANNEL_7 = 7
// 	#endif /* (SAM3XA) */
// } pwm_ch_t;


//TWI state
typedef enum _twi_cmd_t {

	TWI_CMD_ARM_INIT = 0x20,
	TWI_CMD_DROPOFF_START = 0x21,
	TWI_CMD_PICKUP_START = 0x22,
	TWI_CMD_DROPOFF_STATUS = 0x23,
	TWI_CMD_PICKUP_STATUS = 0x24,
	TWI_CMD_ERROR = 0x25,
} TwiCmd;

typedef enum _twi_cmd_init_req_t {
	TWI_CMD_ARM_REQ_BOX_INFO = 2,
	TWI_CMD_ARM_REQ_OBJ_INFO = 3,
	TWI_CMD_ARM_REQ_COLLECT_INFO = 4
} TwiCmdInitReq;

typedef enum _pickup_status_t {
	PICKUP_DONE = 2,
	PICKUP_FORWARD = 3,
	PICKUP_BACKWARD = 4,
	PICKUP_RUNNING = 5,
	PICKUP_FAILED = 6,
	PICKUP_DONE_DRIVE = 7,
	PICKUP_IDLE = 8
} PickupStatus;

typedef enum _dropoff_status_t {
	DROPOFF_DONE = 2,
	DROPOFF_RUNNING = 3,
	DROPOFF_FAILED = 4,
	DROPOFF_IDLE = 5
} DropoffStatus;

typedef enum _object_t {
	SOCK = 2,
	SQUARE = 3,
	GLASS = 4
} Object;




#endif /* TWICOMHANDLER_H_ */