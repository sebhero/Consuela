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
#include "TwiComHandler.h"

arminfo_t theArm;
uint32_t SLAVE_ADDR;//twi slave address for arm

void handleCmd(uint8_t cmd);

void twi_comInit(void);

//Send data to Arm with TWI
//data holds the data, dataLength is how many bytes the data is.

uint8_t twiSendData(uint8_t* data,int dataLength);
uint8_t sendArmCmd(uint8_t cmd);


//Recive data from ARM with TWI
//recives and prints the data out on Uart.
//packageSize how many bytes expected to recive
//returns the data recived as array.
//uint8_t* reciveFromArm(uint8_t packageSize);
uint8_t twiReciveData(uint8_t* package,uint8_t packageSize);

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

//send command to nav
void twi_sendNavCmd(void);


#endif /* COM_H_ */