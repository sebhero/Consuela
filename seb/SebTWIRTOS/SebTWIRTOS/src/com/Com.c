/*
 * Com.c
 *
 * Created: 2017-05-09 13:17:17
 *  Author: trevl
 */ 
#include "Com.h"
#include <stdio.h>




void initTwi(void){
	// TWI master initialization options.
	twi_master_options_t opt;
	opt.speed = TWI_SPEED;
	opt.chip  = SLAVE_ADDR;

	// Initialize the TWI master driver.
	twi_master_setup(TWI1, &opt);
}

uint8_t sendArmCmd(uint8_t cmd)
{
		twi_package_t packet = {

			.addr[0]      = 0, // TWI slave memory address data MSB
			.addr[1]      = 0,// TWI slave memory address data LSB
			.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
			.chip         = SLAVE_ADDR,      // TWI slave bus address
			.buffer       = &cmd, // transfer data source buffer
			.length       = 3   // transfer data size (bytes)
		};


		if(twi_probe(TWI1,SLAVE_ADDR)==TWI_SUCCESS)
		{
			//puts("Write to slave");
			while (twi_master_write(TWI1, &packet) != TWI_SUCCESS);
			//delay_ms(100);
			//puts("Write to slave done");
			return 1;
		}
		else{
			//puts("error on write to slave");
			return 0;
		}
}

//Send data to Arm with TWI
//data holds the data, dataLength is how many bytes the data is.
uint8_t sendArm(uint8_t* data,int dataLength)
{

	twi_package_t packet = {

		.addr[0]      = 0, // TWI slave memory address data MSB
		.addr[1]      = 0,// TWI slave memory address data LSB
		.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
		.chip         = SLAVE_ADDR,      // TWI slave bus address
		.buffer       = data, // transfer data source buffer
		.length       = dataLength   // transfer data size (bytes)
	};


	if(twi_probe(TWI1,SLAVE_ADDR)==TWI_SUCCESS)
	{
		//puts("Write to slave");
		while (twi_master_write(TWI1, &packet) != TWI_SUCCESS);
		//delay_ms(100);
		//puts("Write to slave done");
		return 1;
	}
	else{
		//puts("error on write to slave");
		return 0;
	}
}

//Recive data from ARM with TWI
//recives and prints the data out on Uart.
//packageSize how many bytes expected to recive
//returns the data recived as array.
uint8_t reciveFromArm(uint8_t* recv,uint8_t packageSize)
{
	int xx=0;
	//setup reciving package
	//data recived
	uint8_t package[packageSize];//setup data package to recive
	//	uint8_t recv[packageSize] = {0};
	//package recived
	twi_package_t pkt_rcv = {

		.addr[0]      = 0,//EEPROM_MEM_ADDR, // TWI slave memory address data MSB
		.addr[1]      = 0,//EEPROM_MEM_ADDR,      // TWI slave memory address data LSB
		.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
		.chip         = SLAVE_ADDR,      // TWI slave bus address
		.buffer       = package, // transfer data source buffer
		.length       = packageSize   // transfer data size (bytes)
	};
	
	if(twi_probe(TWI1,SLAVE_ADDR)==TWI_SUCCESS)
	{
		//puts("Read from slave");
		// Perform a multi-byte read access then check the result.
		while (twi_master_read(TWI1, &pkt_rcv) != TWI_SUCCESS);
		*(recv)=package[0];
		*(recv+1)=package[1];
		*(recv+2)=package[2];
		
		//delay_ms(10);
		//puts("Done reading");
		//printf("got: %s\n",*packet_received.buffer);
		//int theSize = sizeof(data_received)/sizeof(data_received[0]);//figure out size
		
// 		for (xx;xx<packageSize;xx++)
// 		{
// 			//printf("got DATA:%x\n",package[xx]);
// 			printf("got int:%d\n",package[xx]);
// 			//printf("got char:%c\n",package[xx]);
// 			if(package[xx]==120)
// 			{
// 				puts("got x from arduino NO DATA");
// 			}
// 		}
		/*
		
		puts("");
		delay_ms(100);
		*/
		return 1;
		
	}
	else{
		//puts("error on write to slave");
		return 0;
	}
	
	//return package;
	
}


void handleCmd(uint8_t cmd) {

	printf("got in hex: %x\n",cmd);
	printf("got in dec: %d\n",cmd);

	switch (cmd) {
		case TWI_CMD_FROM_ARM_ID :
		printf("cmd is TWI_CMD_FROM_ARM_ID: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_OBJ_POS :
		printf("cmd is TWI_CMD_FROM_ARM_OBJ_POS: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_DROPBOX_POS :
		printf("cmd is TWI_CMD_FROM_ARM_DROPBOX_POS: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_MAXSPEED :
		printf("cmd is TWI_CMD_FROM_ARM_MAXSPEED: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_SUCCESS_LIFT :
		printf("cmd is TWI_CMD_FROM_ARM_SUCCESS_LIFT: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_FAILD_LIFT :
		printf("cmd is TWI_CMD_FROM_ARM_FAILD_LIFT: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_SUCCESS_DELIVER :
		printf("cmd is TWI_CMD_FROM_ARM_SUCCESS_DELIVER: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_FAILED_DELIVER :
		printf("cmd is TWI_CMD_FROM_ARM_FAILED_DELIVER: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_ADJUST_POS :
		printf("cmd is TWI_CMD_FROM_ARM_ADJUST_POS: %x \n",cmd);
		break;
		case TWI_CMD_FROM_ARM_ABORT_ADJUST :
		printf("cmd is TWI_CMD_FROM_ARM_ABORT_ADJUST: %x \n",cmd);
		break;
		case TWI_CMD_TO_ARM_INIT :
		printf("cmd is TWI_CMD_TO_ARM_INIT: %x \n",cmd);
		break;
		case TWI_CMD_TO_ARM_LIFT_SQUARE :
		printf("cmd is TWI_CMD_TO_ARM_LIFT_SQUARE: %x \n",cmd);
		break;
		case TWI_CMD_TO_ARM_LIFT_GLASS :
		printf("cmd is TWI_CMD_TO_ARM_LIFT_GLASS: %x \n",cmd);
		break;
		case TWI_CMD_TO_ARM_LIFT_SOCK :
		printf("cmd is TWI_CMD_TO_ARM_LIFT_SOCK: %x \n",cmd);
		break;
		case TWI_CMD_TO_ARM_ABORT_LIFT :
		printf("cmd is TWI_CMD_TO_ARM_ABORT_LIFT: %x \n",cmd);
		break;
		case TWI_CMD_TO_ARM_DELIVER_OBJ :
		printf("cmd is TWI_CMD_TO_ARM_DELIVER_OBJ: %x \n",cmd);
		break;
		case TWI_CMD_FROM_POS_BOX :
		printf("cmd is TWI_CMD_FROM_POS_BOX: %x \n",cmd);
		break;
		case TWI_CMD_FROM_POS_ROBOT :
		printf("cmd is TWI_CMD_FROM_POS_ROBOT: %x \n",cmd);
		break;
		case TWI_CMD_FROM_POS_SOCK :
		printf("cmd is TWI_CMD_FROM_POS_SOCK: %x \n",cmd);
		break;
		case TWI_CMD_FROM_POS_SQUARE :
		printf("cmd is TWI_CMD_FROM_POS_SQUARE: %x \n",cmd);
		break;
		case TWI_CMD_FROM_POS_GLASS :
		printf("cmd is TWI_CMD_FROM_POS_GLASS: %x \n",cmd);
		break;
		case TWI_CMD_TO_POS_INIT :
		printf("cmd is TWI_CMD_TO_POS_INIT: %x \n",cmd);
		break;
		case TWI_CMD_TO_POS_ROBOT :
		printf("cmd is TWI_CMD_TO_POS_ROBOT: %x \n",cmd);
		break;
		case TWI_CMD_TO_POS_SOCK :
		printf("cmd is TWI_CMD_TO_POS_SOCK: %x \n",cmd);
		break;
		case TWI_CMD_TO_POS_SQUARE :
		printf("cmd is TWI_CMD_TO_POS_SQUARE: %x \n",cmd);
		break;
		case TWI_CMD_TO_POS_GLASS :
		printf("cmd is TWI_CMD_TO_POS_GLASS: %x \n",cmd);
		break;
		case NOT_FOUND:
		printf("cmd is NOT_FOUND: %x \n",cmd);
		break;
		default:
		printf("error\n");

		
	}
}
