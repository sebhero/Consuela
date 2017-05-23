
/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */

#include <asf.h>
#include <inttypes.h>
#include <FreeRTOSConfig.h>
#include "time measurement/TimerCounter.h"
#include "conf_board.h"
#include "pulse/pulseCounterHandler.h"
#include "pulse/pulse.h"
#include "driving/motorFunc.h"
#include "driving/navigation.h"
#include "detection/ultra_servo.h"
#include "com/TwiComHandler.h"
#include "com/Com.h"

xTaskHandle *pxTaskDriveToObject;
xTaskHandle *pxTaskUltraSensor;
xTaskHandle *pxTaskCommunication;

void vUltraSensorTask(void *pvParam);
void vDriveToObjectTask(void *pvParam);
void vCommunicationTask(void *pvParam);

#define D7  IOPORT_CREATE_PIN(PIOC, 23)

uint8_t booleanDriving = 0;
uint8_t booleanUltraSensor = 0;
uint8_t booleanCommunication = 0;
uint8_t booleanModifyPosition = 0;

#define pulseh_ch 0
#define pulsev_ch 1

double distanceToMove;

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
static void driveForwardDuringPickup(void);
static void setBitLevels(int, int, int, int);

static void setBitLevels(int driving, int ultrasensor, int modifyPosition, int communication){
	booleanDriving = driving;
	booleanUltraSensor = ultrasensor;
	booleanModifyPosition = modifyPosition;
	booleanCommunication = communication;
}

gotoNextDummy=1;
curObj=1;
/*
 * Task that administers entire functionality that concerns robot's automatic transportation and movement-mobilization
 *
 */
void vDriveToObjectTask(void *pvParam) {
	
	while (1)
	{
		if (booleanDriving == 1 && booleanUltraSensor == 0 && booleanCommunication == 0)
		{
						
 			printf("\n>>>>>>>>>>>>DRIVING TO OBJECT<<<<<<<<<<<<\n");
			//todo del
			uint8_t gotoVal=0;
			if(curObj < 3) 
			{
				gotoVal=1;
				curObj++;
			}
			else{
				gotoVal = 2;
			}
			vTaskDelay(pdMSTOTICKS(4000));
			//todo uncomment
// 			uint8_t gotoVal = goToNext();
			if(gotoVal == 1)
			{
				setBitLevels(0, 1, 0, 0); // ultrasensor = 1
				current_twi_state = START_PICKUP;
				puts("GOTO PICKUP FROM DRIVE");
			    printf("\nGotoVal = %u", gotoVal);
			}
			
			if(gotoVal == 2)
			{
				setBitLevels(0, 1, 0, 0); // ultrasensor = 1
				current_twi_state = START_DROP_OFF;
				puts("GOTO DROPOFF FROM DRIVE");
			    printf("\nGotoVal = %u", gotoVal);
			}
			
			if(gotoVal == 3)
			{
				// Mission accomplished!
				setBitLevels(0, 0, 0, 0); // do not enter any task!
				printf("\nGotoVal = %u", gotoVal);
			}
			portTickType xLastWakeTime = xTaskGetTickCount();
			vTaskDelayUntil(&xLastWakeTime, pdMSTOTICKS(250));
		}
		else // continue blocking
		{
			vTaskDelay(pdMSTOTICKS(200));
		}
	}
	vTaskDelete(NULL);
}

/*
 * The task below handles close-range localization
 *
 */
void vUltraSensorTask(void *pvParam) {

	while (1)
	{
		if (booleanUltraSensor == 1 && booleanCommunication == 0 && booleanDriving == 0)
		{
			puts("ULTRA SOUND find obj");
			vTaskDelay(pdMSTOTICKS(5000));
			booleanDriving=0;
			booleanUltraSensor=0;
			booleanModifyPosition = 0;
			booleanCommunication = 1;
			
			//todo uncomment
// 			for (int i = 0; i <= 360; i++)
// 			{
// 				testingUltraSound();
// 				printf("Searching ...\n");
// 				if (WITHIN_RANGE_FLAG == 1)
// 				{
// 					printf("\nObject has been detected");
// 					booleanModifyPosition = 1;
// 				}
// 			}
// 	
// 			if (booleanModifyPosition == 1)
// 			{
// 				if (angleUltraSensor == 90)
// 				{
// 					if ((distanceUltraSensor + 32) > armInfo.objectDistance)
// 					{
// 						int travelDistToObj = (distanceUltraSensor + 32) - armInfo.objectDistance;
// 						forwardDrive(travelDistToObj);
// 						printf("\nModifying driving: driving forward %i \n", travelDistToObj);
// 						booleanDriving=0;
// 						booleanUltraSensor=0;
// 						booleanModifyPosition = 0;
// 						booleanCommunication = 1;
// 					}
// 					if ((distanceUltraSensor + 32) < armInfo.objectDistance)
// 					{
// 						int travelDistToObj = armInfo.objectDistance - (distanceUltraSensor + 32);
// 						reverseDrive(travelDistToObj);
// 						printf("\nModifying driving: driving backward %i \n", travelDistToObj);
// 						booleanDriving=0;
// 						booleanUltraSensor=0;
// 						booleanModifyPosition = 0;
// 						booleanCommunication = 1;
// 					}
// 				}
// 				else //rotation required
// 				{
// 					if (angleUltraSensor > 90)
// 					{
// 						rotateRightByDegrees(angleUltraSensor-90);
// 						printf("\n----------------Modifying driving: rotating right %i \n", (angleUltraSensor-90) );
// 						if ((distanceUltraSensor + 32) > armInfo.objectDistance)
// 						{
// 							int travelDistToObj = (distanceUltraSensor + 32) - armInfo.objectDistance;
// 							forwardDrive(travelDistToObj); 
// 							printf("\nModifying driving: driving forward %i \n", travelDistToObj);
// 							booleanDriving=0;
// 							booleanUltraSensor=0;
// 							booleanModifyPosition = 0;
// 							booleanCommunication = 1;
// 						}
// 						if ((distanceUltraSensor + 32) < armInfo.objectDistance)
// 						{
// 							int travelDistToObj = armInfo.objectDistance - (distanceUltraSensor + 32);
// 							reverseDrive(travelDistToObj);
// 							printf("\n-----------------Modifying driving: driving backward %i \n", travelDistToObj);
// 							booleanDriving=0;
// 							booleanUltraSensor=0;
// 							booleanModifyPosition = 0;
// 							booleanCommunication = 1;
// 						}
// 					}
// 					if (angleUltraSensor < 90)
// 					{
// 						rotateLeftByDegrees(90-angleUltraSensor);
// 						printf("\n----------------Modifying driving: rotating left %i \n", (90-angleUltraSensor) );
// 						if ((distanceUltraSensor + 32) > armInfo.objectDistance)
// 						{
// 							int travelDistToObj = (distanceUltraSensor + 32) - armInfo.objectDistance;
// 							forwardDrive(travelDistToObj);
// 							printf("\nModifying driving: driving forward %i \n", travelDistToObj);
// 							booleanDriving=0;
// 							booleanUltraSensor=0;
// 							booleanModifyPosition = 0;
// 							booleanCommunication = 1;
// 						}
// 						if ((distanceUltraSensor + 32) < armInfo.objectDistance)
// 						{
// 							int travelDistToObj = armInfo.objectDistance - (distanceUltraSensor + 32);
// 							reverseDrive(travelDistToObj);
// 							printf("\n-----------------Modifying driving: driving backward %i \n", travelDistToObj);
// 							booleanDriving=0;
// 							booleanUltraSensor=0;
// 							booleanModifyPosition = 0;
// 							booleanCommunication = 1;
// 						}
// 					}
// 				}
// 			}
			distanceUltraSensor = 0;
			angleUltraSensor = 0;
			WITHIN_RANGE_FLAG = 0;
		}
		else // continue blocking
		{
			vTaskDelay(pdMSTOTICKS(100));
		}
	}
	vTaskDelete(NULL);
}

/*
In order to function properly certain arms need to drive forward during pickup-action
*/

static void driveForwardDuringPickup(){
	printf("\Forward drive during pickup!");
	//todo uncom
	//forwardDrive(20); //20 cm may suffice
	vTaskDelay(pdMSTOTICKS(2000));
}

/*
 * Task manages TWI-communication
 */
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
						//todo remove						
						setCollectAll(armInfo.collectAll);
						
						
						
						
						
					}
					else
					{
						puts("INIT ARM NO DATA");
						vTaskDelay(pdMSTOTICKS(100));
						//try again 
						booleanCommunication = 1;
						booleanDriving = 0;
						delay_ms(1000);
					}
					
					//get object pos
					//get Info about objects
					objectinfo_t sock;
					sock.theObject = SOCK;
					sock.xpos=0;
					sock.ypos=0;
					twi_navGetSockPos(&sock);
					//printf("sock: x=%d, y=%d.\n",sock.xpos,sock.ypos);
					
					objectinfo_t square;
					square.theObject = SQUARE;
					square.xpos=0;
					square.ypos=0;
					twi_navGetSquarePos(&square);
					//printf("square: x=%d, y=%d.\n",square.xpos,square.ypos);
					objectinfo_t glass;
					glass.theObject = GLASS;
					glass.xpos=0;
					glass.ypos=0;
					int res=twi_navGetGlassPos(&glass);
					//printf("glass: x=%d, y=%d.\n",glass.xpos,glass.ypos);
					objectinfo_t boxgoal;
					boxgoal.theObject = BOXGOAL;
					boxgoal.xpos=0;
					boxgoal.ypos=0;
					res=twi_navGetBoxPos(&boxgoal);
					//printf("boxgoal: x=%d, y=%d.\n",boxgoal.xpos,boxgoal.ypos);
					
					//twi_navGetObjectsPos(&sock, &square, &glass, &boxgoal)
					if(res)
					{
											
						
						
						setObjectSimple(sock);
						setObjectSimple(square);
						setObjectSimple(glass);
						setObjectSimple(boxgoal);
						
						//todo del
						// 							setObject(SQUARE,100,300);
						// 							setObject(SOCK, 300, 300);
						// 							setObject(GLASS, 300, 100);
						printf("so x=%d, y=%d. sq x=%d y=%d. gl x=%d y=%d bo x=%d y=%d",sock.xpos,sock.ypos,square.xpos,square.ypos,
						glass.xpos,glass.ypos,boxgoal.xpos,boxgoal.ypos);
						printf("init arm done\n");
						printf("arminfo: %u %u %u %u all: %u\n",armInfo.boxAngle, armInfo.boxDistance, armInfo.objectAngle, armInfo.objectDistance,armInfo.collectAll);

						
						//booleanCommunication = 0;
						//booleanDriving = 1;
					}
					else
					{
						puts("INIT NAV NO DATA");
						twi_reset(TWI_PORT);
						vTaskDelay(pdMSTOTICKS(100));
						booleanCommunication = 1;
						booleanDriving = 0;
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
					;//due to c code no statement after case
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
							puts("PICKUP_DONE");
							//tell (set) movement that pickup is done
							setDonePickup();
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
							driveForwardDuringPickup();							
							twi_pickupSendMovementDone();
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
					puts("GET_STATUS_DROP_OFF");
					switch(twi_dropoffGetStatus())
					{
						case DROPOFF_DONE:
						//tell (set)drive that pickup is done.
							setDropoffDone();
							printf("DROPOFF_DONE\n");
							if(armInfo.collectAll)
							{
								booleanDriving=0;
							}
							else
							{
								booleanDriving=1;
							}
							booleanUltraSensor=0;
							booleanModifyPosition=0;
							booleanCommunication=0;
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
					printf("FAILED twi switch %d\n",current_twi_state);
				break;
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
	
	//navigering screwed up. there system doesnt work good!
	delay_ms(5000);
	//armInfo = twi_getArmInfo();
	
	uint32_t value = 0;
	
	
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
	pulse_init();
	
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
