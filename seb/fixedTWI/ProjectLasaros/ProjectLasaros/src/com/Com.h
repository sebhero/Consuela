/*
 * Com.h
 *
 * Created: 2017-05-09 12:40:28
 *  Author: Sebastian Boreback
 */


#ifndef COM_H_
#define COM_H_

#include "../asf.h"
#include <stdint.h>
#include "TwiComHandler.h"

typedef struct {
	uint16_t x_pos;
	uint16_t y_pos;
	const char *name_p;
} object_pos_t;

arminfo_t theArm;
uint32_t SLAVE_ADDR;//twi slave address for arm

void handleCmd(uint8_t cmd);

void twi_comInit(void);

//Send data to Arm with TWI
//data holds the data, dataLength is how many bytes the data is.

uint8_t twiSendData(uint8_t *data, int dataLength);

uint8_t sendArmCmd(uint8_t cmd);


//Recive data from ARM with TWI
//recives and prints the data out on Uart.
//packageSize how many bytes expected to recive
//returns the data recived as array.
//uint8_t* reciveFromArm(uint8_t packageSize);
uint8_t twiReciveData(uint8_t *package, uint8_t packageSize);

arminfo_t twi_getArmInfo();

void twi_changeSlave(uint32_t slave_address);

//start pickup
uint8_t twi_pickupStart(void);

//get how many cm to move
uint8_t twi_pickupGetMoveCm(void);

//pickup send movement done
uint8_t twi_pickupSendMovementDone(void);

//get the pickup Status
PickupStatus twi_pickupGetStatus(void);


//set pickup status. when done driving
void twi_pickupSetMasterStatus(PickupStatus newStatus);

//get the current pickup status
PickupStatus twi_pickupGetMasterStatus(void);

//start dropoff
uint8_t twi_dropoffStart(void);

//get the dropoff status
DropoffStatus twi_dropoffGetStatus(void);

//send nav command
uint8_t twi_navSendCmd(TwiCmdNav cmd);

//get nav info
void twi_navRead(uint8_t *readedData);

//get object from nav system
//provide with pointer to object struct for each typ of object.x
uint8_t twi_navGetObjectsPos(objectinfo_t *ptr_sock, objectinfo_t *ptr_square, objectinfo_t *ptr_glass,
							 objectinfo_t *ptr_boxgoal);

//gets both xy 1 and xy 2
//send 1 or 2 for whichXY, then pointer to where to store the data
void twi_navGetXY(TwiCmdNav whichXY, uint16_t *ptr_xpos_1, uint16_t *ptr_ypos_1);


#endif /* COM_H_ */