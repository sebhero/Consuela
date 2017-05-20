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

void sendArmCmd(uint8_t cmd)
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
			puts("Write to slave");
			while (twi_master_write(TWI1, &packet) != TWI_SUCCESS);
			delay_ms(100);
			puts("Write to slave done");
		}
		else{
			puts("error on write to slave");
		}
}

//Send data to Arm with TWI
//data holds the data, dataLength is how many bytes the data is.
void sendArm(uint8_t* data,int dataLength)
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
		puts("Write to slave");
		while (twi_master_write(TWI1, &packet) != TWI_SUCCESS);
		delay_ms(100);
		puts("Write to slave done");
	}
	else{
		puts("error on write to slave");
	}
}

//Recive data from ARM with TWI
//recives and prints the data out on Uart.
//packageSize how many bytes expected to recive
//returns the data recived as array.
uint8_t* reciveFromArm(uint8_t packageSize)
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
		puts("Read from slave");
		// Perform a multi-byte read access then check the result.
		while (twi_master_read(TWI1, &pkt_rcv) != TWI_SUCCESS);
		delay_ms(10);
		puts("Done reading");
		//printf("got: %s\n",*packet_received.buffer);
		//int theSize = sizeof(data_received)/sizeof(data_received[0]);//figure out size
		for (xx;xx<packageSize;xx++)
		{
			printf("got DATA:%x\n",package[xx]);
			printf("got int:%d\n",package[xx]);
			printf("got char:%c\n",package[xx]);
			if(package[xx]==120)
			{
				puts("got x from arduino NO DATA");
			}
		}
		puts("");
		delay_ms(100);
		
	}
	else{
		puts("error on write to slave");
	}
	
	return package;
	
}

