/*
 * Com.c
 *
 * Created: 2017-05-09 13:17:17
 *  Author: Sebastian Boreback
 */
#include "Com.h"

PickupStatus twi_masterPickupStatus = PICKUP_RUNNING;
uint8_t twi_move_cm = 0;//cm to move




void buildObject(uint8_t data[5], objectinfo_t *ptr_object);

void buildXY(uint8_t data[5], int16_t *pInt, int16_t *ypos_1);

void twi_comInit(void) {
	// TWI master initialization options.
	//set that there is no data;
	theArm.hasData = 0;
	SLAVE_ADDR = SLAVE_ADDR_ARM;//twi slave address for arm

	twi_master_options_t opt;
	opt.speed = TWI_SPEED;
	opt.chip = (uint8_t) SLAVE_ADDR;

	// Initialize the TWI master driver.
	twi_master_setup(TWI_PORT, &opt);
}

uint8_t sendArmCmd(uint8_t cmd) {
	twi_package_t packet = {

			.addr[0]      = 0, // TWI slave memory address data MSB
			.addr[1]      = 0,// TWI slave memory address data LSB
			.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
			.chip         = (uint8_t) SLAVE_ADDR,      // TWI slave bus address
			.buffer       = &cmd, // transfer data source buffer
			.length       = 3   // transfer data size (bytes)
	};


	if (twi_probe(TWI_PORT, (uint8_t) SLAVE_ADDR) == TWI_SUCCESS) {
		//puts("Write to slave");
		while (twi_master_write(TWI_PORT, &packet) != TWI_SUCCESS);
		//delay_ms(100);
		//puts("Write to slave done");
		return 1;
	} else {
		//puts("error on write to slave");
		return 0;
	}
}

//Send data to Arm with TWI
//data holds the data, dataLength is how many bytes the data is.
uint8_t twiSendData(uint8_t *data, uint32_t dataLength) {

	twi_package_t packet = {

			.addr[0]      = 0, // TWI slave memory address data MSB
			.addr[1]      = 0,// TWI slave memory address data LSB
			.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
			.chip         = (uint8_t) SLAVE_ADDR,      // TWI slave bus address
			.buffer       = data, // transfer data source buffer
			.length       = dataLength   // transfer data size (bytes)
	};


	if (twi_probe(TWI_PORT, (uint8_t) SLAVE_ADDR) == TWI_SUCCESS) {
		while (twi_master_write(TWI_PORT, &packet) != TWI_SUCCESS);
		return 1;
	} else {
		return 0;
	}
}

//Recive data from ARM with TWI
//recives and prints the data out on Uart.
//packageSize how many bytes expected to recive
//returns the data recived as array.
uint8_t twiReciveData(uint8_t *recv, uint8_t packageSize) {
	//package recived
	twi_package_t pkt_rcv = {

			.addr[0]      = 0,//EEPROM_MEM_ADDR, // TWI slave memory address data MSB
			.addr[1]      = 0,//EEPROM_MEM_ADDR,      // TWI slave memory address data LSB
			.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
			.chip         = (uint8_t) SLAVE_ADDR,      // TWI slave bus address
			.buffer       = recv, // transfer data source buffer
			.length       = packageSize   // transfer data size (bytes)
	};

// 	if(twi_probe(TWI_PORT,SLAVE_ADDR)==TWI_SUCCESS)
// 	{
	while (twi_master_read(TWI_PORT, &pkt_rcv) != TWI_SUCCESS);
	//return 1 success and recv has gotten the twi data.
	return 1;

// 	}
// 	else{
// 		return 0;
// 	}

}


void handleCmd(uint8_t cmd) {

	switch (cmd) {
		case TWI_CMD_ARM_INIT:
			printf("TWI_CMD_ARM_INIT\n");
			break;
		case TWI_CMD_ARM_REQ_BOX_INFO:
			printf("TWI_CMD_ARM_REQ_BOX_INFO\n");
			break;
		case TWI_CMD_ARM_REQ_OBJ_INFO:
			printf("TWI_CMD_ARM_REQ_OBJ_INFO\n");
			break;
		case TWI_CMD_ARM_REQ_COLLECT_INFO:
			printf("TWI_CMD_ARM_REQ_COLLECT_INFO\n");
			break;
		case TWI_CMD_PICKUP_START:
			printf("TWI_CMD_PICKUP_START\n");
			break;
		case TWI_CMD_PICKUP_STATUS:
			printf("TWI_CMD_PICKUP_STATUS\n");
			break;
		case TWI_CMD_DROPOFF_START:
			printf("TWI_CMD_DROPOFF_START\n");
			break;
		case TWI_CMD_DROPOFF_STATUS:
			printf("TWI_CMD_DROPOFF_STATUS\n");
			break;
		case TWI_CMD_ERROR:
			printf("TWI_CMD_ERROR\n");
			break;
		default:
			printf("error\n");
	}
}

arminfo_t twi_getArmInfo() {
	//if the data is allready collected
// 	if(theArm.hasData)
// 	{
// 		return theArm;
// 	}

	uint8_t data[3] = {0};
	uint8_t recv[3] = {0};
	uint8_t result;


	data[0] = TWI_CMD_ARM_INIT;
	data[1] = TWI_CMD_ARM_REQ_BOX_INFO;

	//TWI_CMD_ARM_REQ_COLLECT_INFO;
	//TWI_CMD_ARM_REQ_OBJ_INFO

	result = twiSendData(data, 3);
	if (result) {

//		vTaskDelay(pdMSTOTICKS(20));
		twiReciveData(recv, 3);
		//printf("111 Receive successful: %u, %u, %u\n", recv[0], recv[1], recv[2]);
		//vTaskDelay(pdMSTOTICKS(10));
		theArm.boxDistance = recv[1];
		theArm.boxAngle = recv[2];

		//todo remove
		theArm.hasData = 1;
	} else {
		puts("Fail 111");
		//vTaskDelay(pdMSTOTICKS(10));
		theArm.hasData = 0;
		return theArm;
	}

	//vTaskDelay(pdMSTOTICKS(60));

	//setup for next send
	data[0] = TWI_CMD_ARM_INIT;
	data[1] = TWI_CMD_ARM_REQ_OBJ_INFO;
	recv[0] = 0;
	recv[1] = 0;
	recv[2] = 0;

	result = twiSendData(data, 3);
	if (result) {
		//vTaskDelay(pdMSTOTICKS(20));
		twiReciveData(recv, 3);
// 			//printf("222 Receive successful: %u, %u, %u\n", recv[0], recv[1], recv[2]);
// 			//vTaskDelay(pdMSTOTICKS(10));
		theArm.objectDistance = recv[1];
		theArm.objectAngle = recv[2];
		theArm.hasData = 1;
	} else {
		puts("Fail 222");
		//vTaskDelay(pdMSTOTICKS(10));
		theArm.hasData = 0;
		return theArm;
	}


	//data[0] = TWI_CMD_ARM_INIT;
	data[1] = TWI_CMD_ARM_REQ_COLLECT_INFO;
	recv[0] = 0;
	recv[1] = 0;
	recv[2] = 0;


	result = twiSendData(data, 3);
	if (result) {

		//delay_ms(20);

		twiReciveData(recv, 3);
		//printf("333 Receive successful: %u, %u, %u\n", recv[0], recv[1], recv[2]);
		//vTaskDelay(pdMSTOTICKS(10));
		theArm.collectAll = recv[1];
		theArm.hasData = 1;
	} else {
		puts("Fail 333");
		//vTaskDelay(pdMSTOTICKS(10));
		theArm.hasData = 0;
		return theArm;
	}

	//return the armInfo
	return theArm;
}


//change slave. with new slave address
void twi_changeSlave(uint32_t slave_address) {
	SLAVE_ADDR = slave_address;
	twi_set_slave_addr(TWI_PORT, SLAVE_ADDR);

}

//send command for arm to start pickup
uint8_t twi_pickupStart() {

	uint8_t data[3] = {TWI_CMD_PICKUP_START, 0, 0};
	//send pickup start cmd
	return twiSendData(data, 3);
}


void twi_pickupSetCm(uint8_t cm) {
	twi_move_cm = cm;
}

//set arm in send pickup status
PickupStatus twi_pickupGetStatus() {
	uint8_t data[3] = {TWI_CMD_PICKUP_STATUS, 0, 0};
	uint8_t recv[3] = {0};
	//send pickup start cmd
	if (twiSendData(data, 3)) {
		//get status
		twiReciveData(recv, 3);
		if (recv[0] == TWI_CMD_PICKUP_STATUS) {
			//printf("pickupstatus %x %u",recv[0],recv[1]);
			//vTaskDelay(pdMSTOTICKS(10));

			//when slave want to move
			if (recv[1] == PICKUP_FORWARD || recv[1] == PICKUP_BACKWARD) {
				twi_pickupSetMasterStatus((PickupStatus) recv[1]);
				twi_pickupSetCm(recv[2]);
			}
			return (PickupStatus) recv[1];
		} else {
			puts("ERROR pickup status");
			printf("got: %x %u\n", recv[0], recv[0]);
			//vTaskDelay(pdMSTOTICKS(10));
			handleCmd(recv[0]);
			//vTaskDelay(pdMSTOTICKS(10));
		}

	} else {
		puts("Error on send status pickup");
	}
	return PICKUP_FAILED;

}

//send done driving
uint8_t twi_pickupSendMovementDone() {
	uint8_t data[3] = {TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE, 0};
	//send pickup start cmd
	if (twiSendData(data, 3)) {
		twi_masterPickupStatus = PICKUP_RUNNING;
		//success
		return 1;
	} else {
		//failed
		return 0;
	}

}

//start dropoff
uint8_t twi_dropoffStart(void) {

	uint8_t data[3] = {TWI_CMD_DROPOFF_START, 0, 0};
	//send pickup start cmd
	return twiSendData(data, 3);
}

//get the dropoff status
DropoffStatus twi_dropoffGetStatus(void) {
	uint8_t data[3] = {TWI_CMD_DROPOFF_STATUS, 0, 0};
	uint8_t recv[3] = {0};
	//send pickup start cmd
	if (twiSendData(data, 3)) {
		//get status
		twiReciveData(recv, 3);
		if (recv[0] == TWI_CMD_DROPOFF_STATUS) {
			//printf("pickupstatus %x %u",recv[0],recv[1]);
			//vTaskDelay(pdMSTOTICKS(10));
			//wait here for arm to finish..

			return (DropoffStatus) recv[1];
		} else {
			puts("ERROR pickup status");
			printf("got: %x %u\n", recv[0], recv[0]);
			//vTaskDelay(pdMSTOTICKS(10));
			handleCmd(recv[0]);
			//vTaskDelay(pdMSTOTICKS(10));
		}

	} else {
		puts("Error on send status pickup");
	}
	//error
	return DROPOFF_FAILED;
}

//set pickup status. when done driving
void twi_pickupSetMasterStatus(PickupStatus newStatus) {
	twi_masterPickupStatus = newStatus;
}

//get the current pickup status
PickupStatus twi_pickupGetMasterStatus(void) {
	return twi_masterPickupStatus;
}

uint8_t twi_pickupGetMoveCm(void) {
	return twi_move_cm;
}

//send nav command
uint8_t twi_navSendCmd(TwiCmdNav cmd) {
	twi_package_t packet = {

			.addr[0]      = 0, // TWI slave memory address data MSB
			.addr[1]      = 0,// TWI slave memory address data LSB
			.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
			.chip         = SLAVE_ADDR_NAV,      // TWI slave bus address
			.buffer       = &cmd, // transfer data source buffer &cmd
			.length       = 1   // transfer data size (bytes)
	};


/*	if (twi_probe(TWI_PORT, SLAVE_ADDR_NAV) == TWI_SUCCESS) {*/
	//puts("Write to slave");
	while (twi_master_write(TWI_PORT, &packet) != TWI_SUCCESS);
	//delay_ms(100);
	//puts("Write to slave done");
	return 1;
// 	} else {
// 		//puts("error on write to slave");
// 		return 0;
// 	}
}

//get nav info
void twi_navRead(uint8_t *readedData) {
	//package recived
	twi_package_t pkt_rcv = {

			.addr[0]      = 0,//EEPROM_MEM_ADDR, // TWI slave memory address data MSB
			.addr[1]      = 0,//EEPROM_MEM_ADDR,      // TWI slave memory address data LSB
			.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
			.chip         = SLAVE_ADDR_NAV,      // TWI slave bus address
			.buffer       = (void *) readedData, // transfer data source buffer (
			.length       = 5   // transfer data size (bytes)
	};

//TODO FIX TWI0 TO TWI_PORT

/*	if (twi_probe(TWI_PORT, SLAVE_ADDR_NAV) == TWI_SUCCESS) {*/
	while (twi_master_read(TWI_PORT, &pkt_rcv) != TWI_SUCCESS);
	//return 1 success and recv has gotten the twi data.
	//return 1;
// 	} else {
// 		puts("Error on NAV READ");
// 	}
}


//get object info
void twi_navGetObjectsPos(objectinfo_t *ptr_sock, objectinfo_t *ptr_square, objectinfo_t *ptr_glass,
							 objectinfo_t *ptr_boxgoal) {
	TwiCmdNav cmdSock = SOCKETXY;
	TwiCmdNav cmdSquare = SQUAREXY;
	TwiCmdNav cmdGlass = GLASSXY;
	TwiCmdNav cmdGoal = BOXGOALXY;
	//raw data
	uint8_t dataSock[5] = {0};
	uint8_t dataSquare[5] = {0};
	uint8_t dataGlass[5] = {0};
	uint8_t dataGoal[5] = {0};

	puts("SEND SOCKETXY");
	twi_navSendCmd(cmdSock);
	twi_navRead(dataSock);

	puts("SEND SQUAREXY");
	twi_navSendCmd(cmdSquare);
	twi_navRead(dataSquare);

	puts("SEND GLASSXY");
	twi_navSendCmd(cmdGlass);
	twi_navRead(dataGlass);

	puts("SEND BoxGoalxy");
	twi_navSendCmd(cmdGoal);
	twi_navRead(dataGoal);

	buildObject(dataSock, ptr_sock);
	buildObject(dataSquare, ptr_square);
	buildObject(dataGlass, ptr_glass);
	buildObject(dataGoal, ptr_boxgoal);

}

void buildObject(uint8_t data[5], objectinfo_t *ptr_object) {
	int16_t x = (data[1] << 8) | (data[2] << 0);
	int16_t y = (data[3] << 8) | (data[4] << 0);
	(*ptr_object).xpos = x;
	(*ptr_object).ypos = y;
	//ptr_object->xpos = x;
	//ptr_object->ypos = y;

}

void buildXY(uint8_t data[5], int16_t *ptr_xpos, int16_t *ptr_ypos) {
	int16_t x = (data[1] << 8) | (data[2] << 0);
	int16_t y = (data[3] << 8) | (data[4] << 0);
	*ptr_xpos = x;
	*ptr_ypos = y;

}


//gets both xy for one xy cord (either xy1 or xy2)
void twi_navGetXY(TwiCmdNav whichXY, int16_t *ptr_xpos_1, int16_t *ptr_ypos_1) {
	
	uint8_t data[5] = {0};
		


	twi_navSendCmd(whichXY);
	twi_navRead(data);

	buildXY(data, ptr_xpos_1, ptr_ypos_1);

}
