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
//#include <uart_serial.h>
//#include <arduino_due_x.h>
//#include <cycle_counter.h>
//#include <ioport_pio.h>
//#include "semphr.h"
//#include "FreeRTOSConfig.h"
#include "asf.h"
#include "counters/TimerCounter.h"
#include "conf_board.h"
#include "driving/pulseCounterHandler.h"
#include "driving/pulse.h"
#include "driving/Motorfunc.h"
#include "driving/Hjulreglering.h"
#include "driving/navigation.h"
#include "ultrasound/ultra_servo.h"
#include "com/TwiComHandler.h"
#include "com/Com.h"
#include "mockups/dummyFunc.h"



xTaskHandle *pxTaskDriveToObject;
xTaskHandle *pxTaskUltraSensor;
xTaskHandle *pxTaskCommunication;

#define D7  IOPORT_CREATE_PIN(PIOC, 23)

uint8_t booleanDriving = 0;
uint8_t booleanUltraSensor = 0;
uint8_t booleanCommunication = 0;
uint8_t booleanModifyPosition = 0;

#define pulseh_ch 0
#define pulsev_ch 1

// uint8_t nextPos = 0;
// uint8_t prevPos = 0;

typedef enum  {
	INIT,
	DRIVE,
	LOCATE,
	PICKUP,
	DROPOFF
}MAINFSM_state;

MAINFSM_state currentFSM;
MAINFSM_state nexttFSM;


typedef enum  {
	INIT_ARM,
	START_PICKUP,
	GET_STATUS_PICKUP,
	START_DROP_OFF,
	GET_STATUS_DROP_OFF,
	IDLE
}TWI_state;

//twi commands
//typedef enum twi_states TWI_state;
TWI_state current_twi_state;
TWI_state next_twi_state;

//previouse twi pickup status
arminfo_t armInfo;

void vDriveToObjectTask2(void *pVoid);

PickupStatus prevTwiPickupStatus;

//static arminfo_t armInfo;

static uint8_t adjustPositionDuringPickup(void);
static uint8_t adjustPosition(void);
void vDriveToObjectTask(void *pvParam);
void vUltraSensorTask(void *pvParam);
void vCommunicationTask(void *pvParam);

static void configure_console(void);


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

int main(void) {
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();
	board_init();
	configure_console();
	TC0_init();
	//init twi communication
	twi_comInit();	

	//armInfo = twi_getArmInfo();

	uint32_t value = 0;

	current_twi_state = INIT_ARM;
	currentFSM = INIT;
	nexttFSM = INIT;

	ioport_init();
	ioport_set_pin_dir(D7, IOPORT_DIR_OUTPUT);

	ioport_set_pin_dir(trig, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(servo, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(servo, LOW);

	if (xTaskCreate(vDriveToObjectTask, (const signed char*)"DriveToObject", 1000, NULL, 1, pxTaskDriveToObject) != pdPASS) {
		//todo del
		printf("Failed to create DriveToObject-task");
	}

	if (xTaskCreate(vUltraSensorTask, (const signed char*)"UltraSensor", 1000, NULL, 1, pxTaskUltraSensor) != pdPASS) {
		//todo del
		printf("Failed to create UltraSensor-task");
	}

	if (xTaskCreate(vCommunicationTask,(const signed char*) "Communication", 1000, NULL, 1, pxTaskCommunication) != pdPASS) {
		//todo del
		printf("Failed to create Communication-task");
	}


	booleanDriving = 0;
	booleanUltraSensor = 0;
	booleanModifyPosition = 0;
	booleanCommunication = 1;

	vTaskStartScheduler();

	while (1) {
		/*
		value = read_counter_value();
		printf("Counter value: %u\n", value);
		*/
	}
	/* Insert application code here, after the board has been initialized. */
}


//handles driving the robot to next object/goalbox
void vDriveToObjectTask(void *pvParam)
{

	while (1) {
		if (booleanDriving == 1 && booleanUltraSensor == 0 && booleanCommunication == 0) {
			//todo del
			printf("\n>>>>>>>>>>>>DRIVING TO OBJECT<<<<<<<<<<<<\n");
			//void twi_navGetXY(TwiCmdNav whichXY, uint16_t *ptr_xpos_1, uint16_t *ptr_ypos_1) {
			int16_t x1_pos;
			int16_t y1_pos;

			int16_t x2_pos;
			int16_t y2_pos;
			//todo implement
			//Get the cords from navigation
			//twi_navGetXY(XY1,&x1_pos,&y1_pos);
			//twi_navGetXY(XY2,&x2_pos,&y2_pos);

			//do moving
			uint8_t gotoVal = goToNext();
			if (gotoVal == 1) {

				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 1;
				booleanModifyPosition = 0;
				current_twi_state = START_PICKUP;
				//todo del
				puts("GOTO PICKUP FROM DRIVE");
				delay_ms(2000);

			} else if (gotoVal == 2) {


				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 1;
				booleanModifyPosition = 0;
				current_twi_state = START_DROP_OFF;
				//todo del
				puts("GOTO DROPOFF FROM DRIVE");
				delay_ms(2000);

			}
			//todo del
			printf("\nGotoVal = %u", gotoVal);
			delay_ms(1000);

		} else {
			vTaskDelay(pdMSTOTICKS(200));
		}

	}
	vTaskDelete(NULL);
}

//When driving is done do Locate object
void vUltraSensorTask(void *pvParam) {

	while (1) {
		if (currentFSM == LOCATE)
		{
			puts("DOING LOCATE");
		}
		if (booleanUltraSensor == 1 && booleanCommunication == 0 && booleanDriving == 0) {
			

			//todo del
			puts("Ultrasound found object distance\n");
			printf("Distance to object: %lu\n", distanceUltraSensor);
			printf("Angle to object: %lu\n", angleUltraSensor);
			//todo end del

			//todo uncomment real code
// 			for (uint8_t i = 0; i < 180; i++)
// 			{
// 				testingUltraSound();
// 				if (WITHIN_RANGE_FLAG == 1)
// 				{
// 					printf("Detected object");
// 					booleanModifyPosition = 1;
// 				}
// 			}

			//When still locating
			//booleanUltraSensor == 1 && booleanCommunication == 0 && booleanDriving == 0
			
			if(adjustPosition())
			{
				if(current_twi_state==START_PICKUP)
				{
					puts("LOCATE DONE, PICKUP");
					currentFSM=PICKUP;
				}
				if(current_twi_state==START_DROP_OFF)
				{
					puts("LOCATE DONE, DROPOFF");
					currentFSM=DROPOFF;
				}
			}
			//when located
			booleanModifyPosition = 1;
			if (booleanModifyPosition == 1) {
				//TODO: rotate, drive backwards/forwards
				//activate twi communication
				//todo del
				printf("Modifying driving\n");

				booleanDriving = 0;
				booleanUltraSensor = 0;
				booleanModifyPosition = 0;
				//start pickup. goto communication
				booleanCommunication = 1;
				//todo was here

			}
		} else {
			vTaskDelay(pdMSTOTICKS(100));
		}
	}
	vTaskDelete(NULL);
}

//adjust position during pickup
static uint8_t adjustPositionDuringPickup() {
	printf("\nAdjusting position during pickup!!!\n");
}

//adjust position
//return 1 if success
static uint8_t adjustPosition(){
	printf("\nAdjusting position during LOCATE!!!\n");
	return 1;
}

//TWI communication
void vCommunicationTask(void *pvParam) {

	while (1) {
		if (booleanCommunication == 1 && booleanUltraSensor == 0 && booleanDriving == 0 && booleanModifyPosition == 0) {
			//check state for communication
			switch (current_twi_state) {
				case INIT_ARM:
					//todo del
					puts("INIT_ARM");
					//get the info about the arm

					armInfo = twi_getArmInfo();
					if (armInfo.hasData) {
						setGetAll(armInfo.collectAll);
						booleanCommunication = 0;
						booleanDriving = 1;

						//todo remove
						printf("init arm done\n");
						printf("arminfo: %u %u %u %u all: %u", armInfo.boxAngle, armInfo.boxDistance,
							   armInfo.objectAngle, armInfo.objectDistance, armInfo.collectAll);


					} else {
						//todo remove
						puts("INIT ARM NO DATA");
					}

					//todo implement
//					//get Info about objects
//					objectinfo_t sock;
//					sock.theObject = SOCK;
//					objectinfo_t square;
//					square.theObject = SQUARE;
//					objectinfo_t glass;
//					glass.theObject = GLASS;
//					objectinfo_t boxgoal;
//					boxgoal.theObject = BOXGOAL;
//					twi_navGetObjectsPos(&sock, &square, &glass, &boxgoal);

					break;
				case START_PICKUP:

					//start pickup after modify position
					if (twi_pickupStart() == 1) {
						//todo del
						puts("STARTED PICKUP");
						//Could start pickup
						current_twi_state = GET_STATUS_PICKUP;

					} else {

						//todo del
						puts("FAILED TO START_PICKUP");

						//failed to start pickup!! try again!
						current_twi_state = START_PICKUP;
					}
					break;
					//pick up is started, getting currentStatus
				case GET_STATUS_PICKUP:
					//todo del
					printf("");

					//get current arm currentStatus about pickup
					PickupStatus currentStatus = twi_pickupGetStatus();

					switch (currentStatus) {
						case PICKUP_FAILED:
							//todo del
							puts("PICKUP_FAILED");
							prevTwiPickupStatus=currentStatus;
							break;
						case PICKUP_DONE_DRIVE:
							//todo del
							puts("PICKUP_DONE_DRIVE");
							prevTwiPickupStatus=currentStatus;
							break;
						case PICKUP_IDLE:
							//arm is doing notting
							prevTwiPickupStatus=currentStatus;
							break;
						case PICKUP_RUNNING:
							//puts("pickup RUNNING");
							prevTwiPickupStatus=currentStatus;
							break;
						case PICKUP_DONE:
							//TODO: done picking up
							//done with pickup, continue to drive
							//stop communicating

							//current_twi_state = IDLE;

							puts("PICKUP_DONE");
							//tell (set) movement that pickup is done
							setDonePickup();

							//TODO UNCOMMENT

							booleanUltraSensor = 0;
							booleanModifyPosition = 0;
							booleanCommunication = 0;
							booleanDriving = 1;
							//reset twi pickup status
							prevTwiPickupStatus=PICKUP_IDLE;

							break;
						case PICKUP_FORWARD:
						case PICKUP_BACKWARD:
							//todo del
							puts("go forward or back");
							//TODO: call function that drives forward/backwards based on cm
							//todo del
							printf("Driving forward/backward");
							if (prevTwiPickupStatus == PICKUP_DONE_DRIVE) {
								puts("HAVE Already DROVEN!!");
							}
														
							//if we needed to drive during pickup, check if driving is done
							adjustPositionDuringPickup();
							twi_pickupSendMovementDone();
							//save previouse state
							prevTwiPickupStatus=currentStatus;

							//prevTwiPickupStatus = PICKUP_DONE_DRIVE;

							break;
						default:
							//printf("UNHANDLED PICKUP STATUS: %x\n",currentStatus);
							break;
							//end of handling currentStatus
					}
					//end of get currentStatus pickup
					break;


				case START_DROP_OFF:
					if (twi_dropoffStart() == 1) {
						//done starting pickup
						current_twi_state = GET_STATUS_DROP_OFF;
					} else {
						//todo del
						puts("FAILD TO START DROPOFF");

						//failed to start DROPOFF
						current_twi_state = START_DROP_OFF;
					}
					break;

				case GET_STATUS_DROP_OFF:
					//puts("STATUS DROP OFF");
					switch (twi_dropoffGetStatus()) {
						case DROPOFF_DONE:
							//tell (set)drive that pickup is done.
							setDoneDropoff();
							//todo del
							printf("DROPOFF_DONE\n");

							booleanUltraSensor = 0;
							booleanModifyPosition = 0;
							booleanCommunication = 0;
							booleanDriving = 0;
							break;
						case DROPOFF_RUNNING:
							//todo del
							printf("DROPOFF_RUNNING\n");

							break;
						case DROPOFF_FAILED:
							//todo del
							printf("DROPOFF_FAILED\n");

							break;
						case DROPOFF_IDLE:
							//todo del
							printf("DROPOFF_IDLE\n");

							break;
					}
					break;
				case IDLE:
					//todo del
					puts("IDLE");

					break;
				default:
					//todo del maybe..
					printf("failed twi switch %d\n", current_twi_state);
			}
			//end of current_twi_state
		} else {
			vTaskDelay(pdMSTOTICKS(100));

		}
	}
	vTaskDelete(NULL);
}

static void configure_console(void)
/* Enables feedback through the USB-cable back to terminal within Atmel Studio */
{
	const usart_serial_options_t uart_serial_options = {
			.baudrate = CONF_UART_BAUDRATE,
			.paritytype = CONF_UART_PARITY
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);

	//todo del
	printf("Console ready\n");
	printf("=============\n");

}

