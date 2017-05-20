
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
static void adjustPositionDuringPickup(void);


//handles driving the robot to next object/goalbox
void vDriveToObjectTask(void *pvParam) {
	
	while (1)
	{
		if (booleanDriving == 1 && booleanUltraSensor == 0 && booleanCommunication == 0)
		{
			printf("\n>>>>>>>>>>>>DRIVING TO OBJECT<<<<<<<<<<<<\n");
			uint8_t gotoVal = goToNext();
			if(gotoVal==1)
			{
				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 1;
				booleanModifyPosition = 0;
				current_twi_state = START_PICKUP;
				puts("GOTO PICKUP FROM DRIVE");
			    printf("\nGotoVal = %u", gotoVal);
			}
			
			if(gotoVal == 2)
			{
				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 1;
				booleanModifyPosition = 0;
				current_twi_state = START_DROP_OFF;
				puts("GOTO DROPOFF FROM DRIVE");
			    printf("\nGotoVal = %u", gotoVal);
			}
			
			if(gotoVal == 3)
			{
				booleanDriving = 0;
				booleanCommunication = 0;
				booleanUltraSensor = 0;
				booleanModifyPosition = 0;
				printf("\nGotoVal = %u", gotoVal);
			}
			portTickType xLastWakeTime = xTaskGetTickCount();
			vTaskDelayUntil(&xLastWakeTime, pdMSTOTICKS(250));
			
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
			
			puts("\nUltrasound found object distance\n");
			printf("\nDistance to object: %u\n", distanceUltraSensor);
			printf("\nAngle to object: %u\n", angleUltraSensor);
			
			/*
			for (uint8_t i = 0; i <= 180; i++)
			{
				testingUltraSound();
				if (WITHIN_RANGE_FLAG == 1)
				{
					printf("Object has been detected");
					booleanModifyPosition = 1;
				}
			}
			*/
			booleanModifyPosition = 1; //TODO: instant transition from LOC-task to COM-task, is to be removed
			if (booleanModifyPosition == 1)
			{
				forwardDrive(distanceUltraSensor); 
				printf("Modifying driving\n");
				booleanDriving=0;
				booleanUltraSensor=0;
				booleanModifyPosition = 0;
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
						//todo set to 0
						booleanCommunication = 0;
						//todo set to 1
						booleanDriving = 1;						
						//todo remove						
						printf("init arm done\n");
						printf("arminfo: %u %u %u %u all: %u",armInfo.boxAngle, armInfo.boxDistance, armInfo.objectAngle, armInfo.objectDistance,armInfo.collectAll);
						setCollectAll(armInfo.collectAll);
						
						//todo del
						current_twi_state = START_PICKUP;
					}
					else
					{
						puts("INIT ARM NO DATA");
					}	
					setObject(SQUARE,100,300);
					setObject(SOCK, 300, 300);
					setObject(GLASS, 300, 100);		
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

						
							puts("PICKUP_DONE");
							//tell (set) movement that pickup is done
							setDonePickup();
							
							//TODO UNCOMMENT
							
							booleanUltraSensor=0;
							booleanModifyPosition=0;
							//todo set to 0
							booleanCommunication=0;
							//todo set to 1
							booleanDriving=1;

							//todo del
							current_twi_state=START_DROP_OFF;
							
						break;						
						case PICKUP_FORWARD:
						case PICKUP_BACKWARD:
							puts("go forward or back");
							//TODO: call function that drives forward/backwards based on cm
							printf("Driving forward/backward");
							//if we needed to drive during pickup, check if driving is done
							adjustPositionDuringPickup();
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
					printf("failed twi switch %d\n",current_twi_state);
					char xx[255];
					scanf(xx);
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
