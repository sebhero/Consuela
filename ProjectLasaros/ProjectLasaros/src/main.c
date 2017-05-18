
/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#include <asf.h>
#include <inttypes.h>
#include <FreeRTOSConfig.h>
#include "TimerCounter.h"
#include "conf_board.h"
#include "pulseCounterHandler.h"
#include "pulse.h"
#include "Motorfunc.h"
#include "Hjulreglering.h"
#include "navigation.h"
#include "ultra_servo.h"
#include "TwiComHandler.h"
#include "Com.h"
#include "dummyFunc.h"

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

enum twi_states{
	INIT_ARM,
	START_PICKUP,
	GET_STATUS_PICKUP,
	START_DROP_OFF,
	GET_STATUS_DROP_OFF,
	IDLE
};

typedef enum twi_states TWI_state;
TWI_state current_twi_state;
TWI_state next_twi_state;

static arminfo_t armInfo;
static void adjustPositionDuringPickup(void);

//handles driving the robot to next object/goalbox
void vDriveToObjectTask(void *pvParam) {
	
	while (1)
	{
		if (booleanDriving == 1 && booleanUltraSensor == 0 && booleanCommunication == 0)
		{
			printf("\n>>>>>>>>>>>>DRIVING TO OBJECT<<<<<<<<<<<<\n");
			//do moving
			uint8_t gotoVal = goToNext();
			if(gotoVal==1)
			{
				
				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 1;
				booleanModifyPosition = 0;
				current_twi_state = START_PICKUP; 
				puts("GOTO PICKUP FROM DRIVE");
				delay_ms(2000);

			}
			else if(gotoVal == 2)
			{
				

				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 1;
				booleanModifyPosition = 0;
				current_twi_state = START_DROP_OFF; 
				puts("GOTO DROPOFF FROM DRIVE");
				delay_ms(2000);

			}
			printf("\nGotoVal = %u", gotoVal);
			delay_ms(1000);
			
		}
		else
		{
			vTaskDelay(pdMSTOTICKS(200));
		}

	}
	vTaskDelete(NULL);
}

//When driving is done do Locate object
void vUltraSensorTask(void *pvParam) {

	while (1)
	{
		if (booleanUltraSensor == 1 && booleanCommunication == 0 && booleanDriving == 0)
		{
			
			//todo del
			puts("Ultrasound found object distance\n");
			printf("Distance to object: %u\n", distanceUltraSensor);
			printf("Angle to object: %u\n", angleUltraSensor);
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
			
			//when located
			booleanModifyPosition=1;
			if (booleanModifyPosition == 1)
			{
				//TODO: rotate, drive backwards/forwards
				//activate twi communication
				printf("Modifying driving\n");
				booleanDriving=0;
				booleanUltraSensor=0;
				booleanModifyPosition = 0;
				//start pickup. goto communication
				booleanCommunication = 1;
				
			}
		} 
		else
		{
			vTaskDelay(pdMSTOTICKS(100));
		}
	}
	vTaskDelete(NULL);
}

static void adjustPositionDuringPickup(){
	printf("\nAdjusting position during pickup!!!");
}
//TWI communcation
void vCommunicationTask(void *pvParam)
{
	
	while(1)
	{
		if (booleanCommunication == 1 && booleanUltraSensor == 0 && booleanDriving == 0 && booleanModifyPosition == 0)
		{
			//check state for communication
			switch (current_twi_state)
			{
				case INIT_ARM:
					puts("INIT_ARM");
					armInfo = twi_getArmInfo();	
					if(armInfo.hasData)
					{
						booleanCommunication = 0;
						booleanDriving = 1;						
						//todo remove						
						printf("init arm done\n");
						printf("arminfo: %u %u %u %u all: %u",armInfo.boxAngle, armInfo.boxDistance, armInfo.objectAngle, armInfo.objectDistance,armInfo.collectAll);
						setGetAll(armInfo.collectAll);
					}
					else
					{
						puts("INIT ARM NO DATA");
					}					
				break;
				case START_PICKUP:
					
					//start pickup after modify position
					if (twi_pickupStart() == 1)
					{
						puts("STARTED PICKUP");
						//Could start pickup
						
						current_twi_state=GET_STATUS_PICKUP;
						
					}
					else
					{
						//failed to start pickup!! try again!
						puts("FAILED TO START_PICKUP");
						current_twi_state=START_PICKUP;
					}
				break;
				//pick up is started, getting status
				case GET_STATUS_PICKUP:
					printf("");
					//get current arm status about pickup
					uint8_t status = twi_pickupGetStatus();
					
					switch(status)
					{
						case PICKUP_FAILED:
						puts("PICKUP_FAILED");
						break;
						case PICKUP_DONE_DRIVE:
						puts("PICKUP_DONE_DRIVE");
						break;
						case PICKUP_IDLE:
						break;
						case PICKUP_RUNNING:
							//puts("pickup RUNNING");
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
							
							booleanUltraSensor=0;
							booleanModifyPosition=0;
							booleanCommunication=0;
							booleanDriving=1;
							
						break;						
						case PICKUP_FORWARD:
						case PICKUP_BACKWARD:
							puts("go forward or back");
							//TODO: call function that drives forward/backwards based on cm
							printf("Driving forward/backward");
							//if we needed to drive during pickup, check if driving is done
							adjustPositionDuringPickup();
							twi_pickupSendMovementDone();
							current_twi_state = PICKUP_DONE_DRIVE;
						break;
						default:
						//printf("UNHANDLED PICKUP STATUS: %x\n",status);
						break;
						//end of handling status
					}
					//end of get status pickup
				break;
				
				
				case START_DROP_OFF:
					if (twi_dropoffStart() == 1)
					{
						//done starting pickup
						current_twi_state=GET_STATUS_DROP_OFF;
					}
					else
					{
						//failed to start DROPOFF
						puts("FAILD TO START DROPOFF");
						current_twi_state=START_DROP_OFF;
					}
				break;
				
				case GET_STATUS_DROP_OFF:
					//puts("STATUS DROP OFF");
					switch(twi_dropoffGetStatus())
					{
						case DROPOFF_DONE:
						//tell (set)drive that pickup is done.
							setDoneDropoff();
							printf("DROPOFF_DONE\n");
							booleanUltraSensor=0;
							booleanModifyPosition=0;
							booleanCommunication=0;
							booleanDriving=0;
						break;
						case DROPOFF_RUNNING:
							printf("DROPOFF_RUNNING\n");
						break;
						case DROPOFF_FAILED:
							printf("DROPOFF_FAILED\n");
						break;
						case DROPOFF_IDLE:
							printf("DROPOFF_IDLE\n");
						break;
					}				
				break;
				case IDLE:
				puts("IDLE");
				break;
				default:
				printf("failed twi switch %d\n",current_twi_state);
			}
			//end of current_twi_state
		} 
		else
		{
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
	
	printf("Console ready\n");
	printf("=============\n");
}

int main (void)
{
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
	
	ioport_init();
	ioport_set_pin_dir(D7, IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(trig, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(servo, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(servo, LOW);
	
	if(xTaskCreate(vDriveToObjectTask, "DriveToObject", 1000, NULL, 1, pxTaskDriveToObject) != pdPASS){
		printf("Failed to create DriveToObject-task");
	}
	
	if(xTaskCreate(vUltraSensorTask, "UltraSensor", 1000, NULL, 1, pxTaskUltraSensor) != pdPASS){
		printf("Failed to create UltraSensor-task");
	}
	
	if(xTaskCreate(vCommunicationTask, "Communication", 1000, NULL, 1, pxTaskCommunication) != pdPASS){
		printf("Failed to create Communication-task");
	}
	
// 	nextPos = 0;
// 	prevPos = 0;
// 	
	booleanDriving = 0;
	booleanUltraSensor = 0;
	booleanModifyPosition = 0;
	booleanCommunication = 1;
	
	vTaskStartScheduler();
	
	while (1)
	{
		/*
		value = read_counter_value();
		printf("Counter value: %u\n", value);
		*/
	}
	/* Insert application code here, after the board has been initialized. */
}
