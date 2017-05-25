/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "conf_uart_serial.h"
#include "com/TwiComHandler.h"
#include "com/Com.h"

#include "pulse/pulse.h"
#include "pulse/pulseCounterHandler.h"
#include "driving/motorFunc.h"
//#include "seb/SebWire.h"
// #include <stdio.h>
// #include <stdio_serial.h>

#define TWI_STANDARD_MODE_SPEED 100000U
#define TWI_FAST_MODE_SPEED 400000U

#define SLAVE_ADDR 0x02 //twi slave
//#define TWI_SPEED 100000
#define TWI_SPEED TWI_STANDARD_MODE_SPEED



void initTwiMaster(void);
void sendToArduino(void);
void readSlave(void);
void configureConsole(void);
void readWrite(void);

//test data to send
#define PATTERN_TEST_LENGTH     sizeof(test_pattern)
const uint8_t test_pattern[] = {
	0x68,
	0x65,
	0x6c,
	0x6c,
	0x6f
};


twi_package_t packet = {

	.addr[0]      = 0,//EEPROM_MEM_ADDR, // TWI slave memory address data MSB
	.addr[1]      = 0,//EEPROM_MEM_ADDR,      // TWI slave memory address data LSB
	.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
	.chip         = SLAVE_ADDR,      // TWI slave bus address
	.buffer       = (void *)test_pattern, // transfer data source buffer
	.length       = PATTERN_TEST_LENGTH   // transfer data size (bytes)
};

// TWI state
// typedef enum TWI_CMD {
// 	//commands from arm
// 	TWI_CMD_FROM_ARM_ID = 				0x10,
// 	TWI_CMD_FROM_ARM_OBJ_POS = 			0x11,
// 	TWI_CMD_FROM_ARM_DROPBOX_POS = 		0x12,
// 	TWI_CMD_FROM_ARM_MAXSPEED = 		0x13,
// 	TWI_CMD_FROM_ARM_SUCCESS_LIFT = 	0x14,
// 	TWI_CMD_FROM_ARM_FAILD_LIFT = 		0x15,
// 	TWI_CMD_FROM_ARM_SUCCESS_DELIVER = 	0x16,
// 	TWI_CMD_FROM_ARM_FAILED_DELIVER = 	0x17,
// 	TWI_CMD_FROM_ARM_ADJUST_POS = 		0x18,
// 	TWI_CMD_FROM_ARM_ABORT_ADJUST = 	0x19,
//
// 	//commands to arm
// 	TWI_CMD_TO_ARM_INIT = 				0x20,
// 	TWI_CMD_TO_ARM_LIFT_SQUARE = 		0x21,
// 	TWI_CMD_TO_ARM_LIFT_GLASS = 		0x22,
// 	TWI_CMD_TO_ARM_LIFT_SOCK = 			0x23,
// 	TWI_CMD_TO_ARM_ABORT_LIFT = 		0x24,
// 	TWI_CMD_TO_ARM_DELIVER_OBJ = 		0x25,
//
// 	//commands from positioning
// 	TWI_CMD_FROM_POS_BOX = 				0x30,
// 	TWI_CMD_FROM_POS_ROBOT = 			0x31,
// 	TWI_CMD_FROM_POS_SOCK = 			0x32,
// 	TWI_CMD_FROM_POS_SQUARE = 			0x33,
// 	TWI_CMD_FROM_POS_GLASS = 			0x34,
//
// 	//commands from positioning
// 	TWI_CMD_TO_POS_INIT = 				0x40,
// 	TWI_CMD_TO_POS_ROBOT = 				0x41,
// 	TWI_CMD_TO_POS_SOCK =				0x42,
// 	TWI_CMD_TO_POS_SQUARE =				0x43,
// 	TWI_CMD_TO_POS_GLASS = 				0x44,
// 	NOT_FOUND = 						0x45
// } twi_cmd;


// void handleCmd(uint8_t cmd) {
//
// 		printf("got in hex: %x\n",cmd);
// 		printf("got in dec: %d\n",cmd);
//
// 		switch (cmd) {
// 			case TWI_CMD_FROM_ARM_ID :
// 			printf("cmd is TWI_CMD_FROM_ARM_ID: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_OBJ_POS :
// 			printf("cmd is TWI_CMD_FROM_ARM_OBJ_POS: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_DROPBOX_POS :
// 			printf("cmd is TWI_CMD_FROM_ARM_DROPBOX_POS: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_MAXSPEED :
// 			printf("cmd is TWI_CMD_FROM_ARM_MAXSPEED: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_SUCCESS_LIFT :
// 			printf("cmd is TWI_CMD_FROM_ARM_SUCCESS_LIFT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_FAILD_LIFT :
// 			printf("cmd is TWI_CMD_FROM_ARM_FAILD_LIFT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_SUCCESS_DELIVER :
// 			printf("cmd is TWI_CMD_FROM_ARM_SUCCESS_DELIVER: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_FAILED_DELIVER :
// 			printf("cmd is TWI_CMD_FROM_ARM_FAILED_DELIVER: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_ADJUST_POS :
// 			printf("cmd is TWI_CMD_FROM_ARM_ADJUST_POS: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_ARM_ABORT_ADJUST :
// 			printf("cmd is TWI_CMD_FROM_ARM_ABORT_ADJUST: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_ARM_INIT :
// 			printf("cmd is TWI_CMD_TO_ARM_INIT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_ARM_LIFT_SQUARE :
// 			printf("cmd is TWI_CMD_TO_ARM_LIFT_SQUARE: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_ARM_LIFT_GLASS :
// 			printf("cmd is TWI_CMD_TO_ARM_LIFT_GLASS: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_ARM_LIFT_SOCK :
// 			printf("cmd is TWI_CMD_TO_ARM_LIFT_SOCK: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_ARM_ABORT_LIFT :
// 			printf("cmd is TWI_CMD_TO_ARM_ABORT_LIFT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_ARM_DELIVER_OBJ :
// 			printf("cmd is TWI_CMD_TO_ARM_DELIVER_OBJ: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_POS_BOX :
// 			printf("cmd is TWI_CMD_FROM_POS_BOX: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_POS_ROBOT :
// 			printf("cmd is TWI_CMD_FROM_POS_ROBOT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_POS_SOCK :
// 			printf("cmd is TWI_CMD_FROM_POS_SOCK: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_POS_SQUARE :
// 			printf("cmd is TWI_CMD_FROM_POS_SQUARE: %x \n",cmd);
// 			break;
// 			case TWI_CMD_FROM_POS_GLASS :
// 			printf("cmd is TWI_CMD_FROM_POS_GLASS: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_POS_INIT :
// 			printf("cmd is TWI_CMD_TO_POS_INIT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_POS_ROBOT :
// 			printf("cmd is TWI_CMD_TO_POS_ROBOT: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_POS_SOCK :
// 			printf("cmd is TWI_CMD_TO_POS_SOCK: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_POS_SQUARE :
// 			printf("cmd is TWI_CMD_TO_POS_SQUARE: %x \n",cmd);
// 			break;
// 			case TWI_CMD_TO_POS_GLASS :
// 			printf("cmd is TWI_CMD_TO_POS_GLASS: %x \n",cmd);
// 			break;
// 			case NOT_FOUND:
// 			printf("cmd is NOT_FOUND: %x \n",cmd);
// 			break;
// 			default:
// 			printf("error\n");
//
//
// 	}
// }


void sendToArduino2(int cmd)
{
	int xx =0;
	uint8_t recv[2] = {0};
	twi_package_t pkt_rcv = {

		.addr[0]      = 0,//EEPROM_MEM_ADDR, // TWI slave memory address data MSB
		.addr[1]      = 0,//EEPROM_MEM_ADDR,      // TWI slave memory address data LSB
		.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
		.chip         = SLAVE_ADDR,      // TWI slave bus address
		.buffer       = recv, // transfer data source buffer
		.length       = 1   // transfer data size (bytes)
	};

	//char* hello = "sebastian";
	//int hello =;
	packet.buffer=&cmd;
	packet.length= 1;

 	if(twi_probe(TWI1,SLAVE_ADDR)==TWI_SUCCESS)
 	{
		puts("Write to slave");
		while (twi_master_write(TWI1, &packet) != TWI_SUCCESS);
		delay_ms(100);
		puts("Write to slave done");

		puts("Read from slave");
		// Perform a multi-byte read access then check the result.
		while (twi_master_read(TWI1, &pkt_rcv) != TWI_SUCCESS);
		delay_ms(10);
		puts("Done reading");
		//printf("got: %s\n",*packet_received.buffer);
		//int theSize = sizeof(data_received)/sizeof(data_received[0]);//figure out size
		for (xx;xx<1;xx++)
		{
			printf("got DATA:%d",recv[xx]);
		}
		puts("");
		delay_ms(100);

 	}
	else{
 		puts("error on write to slave");
 	}
}

// void testCmds(void)
// {
// 	int i=0;
// 		for(i = TWI_CMD_FROM_ARM_ID; i<NOT_FOUND; i++){
// 			//send a command and recive it.
// 			sendArmCmd(i);
// 			reciveFromArm(1);
// //			sendArm("hello",5);
//
//
// 			//printf("sent cmd: %x\n",i);
// 			//handleCmd(i);
// 			delay_ms(500);
// 		}
// 		//restart
// 		i = TWI_CMD_FROM_ARM_ID;
// }


void twi_test_getArminfo()
{
	puts("REQ arm info");
	//vTaskDelay(pdMSTOTICKS(20));

	//test get armInfo
	arminfo_t armInfo = twi_getArmInfo();
	if(armInfo.hasData)
	{
		printf("armInfo: boxA: %d boxD: %d objA %d objD %d col: %d",armInfo.boxAngle,armInfo.boxDistance, armInfo.objectAngle, armInfo.objectDistance, armInfo.collectAll);
		//vTaskDelay(pdMSTOTICKS(20));
	}
}

int countObj=0;
Object theObj=GLASS;
int superDone =0;
uint8_t startpickup=0;
uint8_t drive= 0;
void twi_test_pickup(void)
{

	if(superDone)
	{
		return;
	}
	puts("twi pick");
	if(startpickup==0)
	{

		//RETURN 1 IF success
		startpickup=twi_pickupStart();
		if (startpickup ==0)
		{
			puts("failed tot start pickup!");
		}
		else
		{
			puts("Started pickup");
		}

		delay_ms(80);

	}
	else{

		puts("do tWi status");




		PickupStatus pickupStatus=twi_pickupGetStatus();

		switch(pickupStatus){
			case PICKUP_BACKWARD:
			puts("PICKUP_STATUS.PICKUP_BACKWARD:");
			printf("back: %u\n",twi_pickupGetMoveCm());
			//should be handle by driving.. to tell that its done
			twi_pickupSetMasterStatus(PICKUP_DONE_DRIVE);
			delay_ms(2000);
			break;
			case PICKUP_DONE: // Arm is up.
			puts("PICKUP_DONE");
			//startpickup=0;
			superDone=1;
			break;
			case PICKUP_DONE_DRIVE: // Sent done drive to arm, arm is going up...
			puts("PICKUP_DONE_DRIVE:");

			break;
			case PICKUP_FAILED:
			puts("PICKUP_FAILED");
			break;
			case PICKUP_FORWARD: // Arm has told platform to move forward.
			puts("PICKUP_STATUS.PICKUP_FORWARD:");
			printf("forward: %u\n",twi_pickupGetMoveCm());
			//should be handle by driving.. to tell that its done


			break;
			case PICKUP_RUNNING: // Arm is going down...
			puts("PICKUP_RUNNING");
			break;
			case PICKUP_IDLE:
			puts("PICKUP_IDLE");
			break;
			default:
			puts("pickup status error");
			startpickup=0;
			break;
		}
		//end of check slave state
		if(pickupStatus == PICKUP_BACKWARD || pickupStatus == PICKUP_FORWARD)
		{
			puts("MOVING FRWD");
			forwardDrive(40);
			twi_pickupSetMasterStatus(PICKUP_DONE_DRIVE);

			if (twi_pickupGetMasterStatus()==PICKUP_DONE_DRIVE)
			{
				puts("DONE MOVE");
				//tell slave that we are done moving.
				twi_pickupSendMovementDone();
				twi_pickupSetMasterStatus(PICKUP_IDLE);
			}
		}
	}
	//vTaskDelay(pdMSTOTICKS(80));

}

uint8_t startDropoff =0;
void twi_test_dropoff(void){
	puts("twi test dropoff");
	if (startDropoff==0)
	{
		//RETURN 1 IF success
		startDropoff=twi_dropoffStart();
		if(startDropoff==0){
			puts("error start dropoff");
		}
	}
	else
	{
		DropoffStatus dropoffStatus=twi_dropoffGetStatus();
		switch(dropoffStatus){
			case DROPOFF_RUNNING:
			puts("DROPOFF_RUNNING");
			break;
			case DROPOFF_FAILED:
			puts("DROPOFF_FAILED:");
			break;
			case DROPOFF_DONE:
			puts("DROPOFF_DONE:");
			break;
			default:
			//something went wrong restart.
			puts("dropoff unknown");
			startDropoff=0;
			break;
		}
	}
}



int main (void)
{
	  /* Initialize the common clock service, board-specific initialization, and
   * interrupt vector support prior to using the TWI master interfaces.
   */
  sysclk_init();

	board_init();

	configureConsole();

	 // TWI master initialization options.
	 //initTwiMaster();
	 //initTwi();
	 twi_comInit();


     	//TC0_init();

     	//armInfo = twi_getArmInfo();

     	uint32_t value = 0;


     	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
     	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);

     	pulse_init();

     	ioport_init();

     	forwardDrive(40);
     	rotateRightByDegrees(90);
        forwardDrive(20);
        superDone = 0;
        startpickup=0;
        while(!superDone) {
            delay_ms(200);
            twi_test_pickup();
        }
        rotateLeftByDegrees(90);
        forwardDrive(40);
        superDone = 0;
        startpickup=0;
        while(!superDone) {
            delay_ms(200);
            twi_test_pickup();
        }
        rotateLeftByDegrees(90);
        forwardDrive(20);
        superDone = 0;
        startpickup=0;
        while(!superDone) {
            delay_ms(200);
            twi_test_pickup();
        }
        rotateLeftByDegrees(90);
        forwardDrive(120);

        twi_test_dropoff();
        while(!startDropoff) {
        }

        while(1) {
        }


	 while(!superDone)
	 {
//		twi_test_getArminfo();
		twi_test_pickup();
//		twi_test_dropoff();

		delay_ms(200);
	 }


// 	 send to arduino conf
// 		 //sendToArduino();
// 		 //delay_s(5);
// 		 //readSlave();
// 		//readWrite();
// 		uint8_t data[1];
// 		uint8_t i;
// 		uint8_t dum[3];
// 		dum[0]=TWI_CMD_ARM_INIT;
// 		dum[1]=0;
// 		dum[2]=0;
// 		while(1)
// 		{
// 			//delay_ms(1000);
//
// 			sendArm(dum,3);
// 			//sendArmCmd(TWI_CMD_FROM_ARM_ID);
// 			delay_ms(100);
// 			uint8_t recieved[3]={0};
// 			reciveFromArm(recieved,3);
//
//
// 			if(twi_requestArmInfo())
// 			{
// 				puts("success req data");
// 				armInfo info=twi_getArmInfo();
// 				printf("arminfo boxdist: %d",info.boxDistance);
// 			}
// 			else{
// 				puts("faild to req");
// 			}
//
//
// 		}



}

void configureConsole()
{
	const usart_serial_options_t uart_serial_options =
	{
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);
}

	//uint8_t data_received[PATTERN_TEST_LENGTH] = {0};
	uint8_t data_received[PATTERN_TEST_LENGTH] = {0};
	twi_package_t packet_received = {

		.addr[0]      = 0,//EEPROM_MEM_ADDR, // TWI slave memory address data MSB
		.addr[1]      = 0,//EEPROM_MEM_ADDR,      // TWI slave memory address data LSB
		.addr_length  = 0, //sizeof (uint16_t),    // TWI slave memory address data size
		.chip         = SLAVE_ADDR,      // TWI slave bus address
		.buffer       = data_received, // transfer data source buffer
		.length       = PATTERN_TEST_LENGTH   // transfer data size (bytes)
		//.length       = PATTERN_TEST_LENGTH   // transfer data size (bytes)
	};

void readSlave()
{


	int xx=0;

	while(1)
	{
 		if(twi_probe(TWI1,SLAVE_ADDR)==TWI_SUCCESS)
 		{
			xx=0;
			puts("Read from slave");
			// Perform a multi-byte read access then check the result.
			while (twi_master_read(TWI1, &packet_received) != TWI_SUCCESS);
			delay_ms(10);
			puts("Done reading");
			//printf("got: %s\n",*packet_received.buffer);
			int theSize = sizeof(data_received)/sizeof(data_received[0]);
			for (xx;xx<PATTERN_TEST_LENGTH;xx++)
			{
				printf("%c",data_received[xx]);
			}
			puts("");
			//delay_s(1);
		}
		else{
			puts("TWI error on probe");
		}


	}
}

void sendToArduino()
{


	while(1)
	{
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
}

void initTwiMaster()
{
	 // TWI master initialization options.
	 twi_master_options_t opt;
	 opt.speed = TWI_SPEED;
	 opt.chip  = SLAVE_ADDR;

	 // Initialize the TWI master driver.
	 twi_master_setup(TWI1, &opt);

}

//test read and write commands to arduino slave
void readWrite(void){
	int xx=0;

	while(1)
	{
		if(twi_probe(TWI1,SLAVE_ADDR)==TWI_SUCCESS)
		{
			xx=0;
			puts("Read from slave");
			// Perform a multi-byte read access then check the result.
			while (twi_master_read(TWI1, &packet_received) != TWI_SUCCESS);
			delay_ms(10);
			puts("Done reading");
			//printf("got: %s\n",*packet_received.buffer);
			int theSize = sizeof(data_received)/sizeof(data_received[0]);
			for (xx;xx<PATTERN_TEST_LENGTH;xx++)
			{
				printf("%c",data_received[xx]);
			}
			puts("");
			delay_ms(100);
			puts("Write to slave");
			while (twi_master_write(TWI1, &packet) != TWI_SUCCESS);
			delay_ms(100);
			puts("Write to slave done");
		}
		else{
			puts("TWI error on probe");
		}


	}

}
