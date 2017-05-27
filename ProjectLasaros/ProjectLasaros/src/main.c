
/*
 * The robot's system functionalities require a real-time operative system for implementing 
 * time management and task scheduling. The application tasks running on the processor have 
 * specified priorities and period cycles. These tasks manage driving, communication and 
 * localization.
 *
 * In total, there are five tasks: vLocalizationTask, vDriveToObjectTask, vCommunicationTask
 * vModifyPositionTask and vLCDTask
 *
 * Made by: Danial Mahmoud 2017-05-15
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
#include "lcd/lcd.h"
#include "time measurement/RealTimeTimer.h"

xTaskHandle *pxTaskDriveToObject;
xTaskHandle *pxTaskLocalization;
xTaskHandle *pxTaskCommunication;
xTaskHandle *pxTaskModifyPosition;
xTaskHandle *pxTaskLCD;

void vLocalizationTask(void *pvParam);
void vDriveToObjectTask(void *pvParam);
void vCommunicationTask(void *pvParam);
void vModifyPositionTask(void *pvParam);
void vLCDTask(void *pvParam);

uint32_t sec; 

/*
 * TWI-communication states
 */
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

/*
 * Robot-system states
 */
enum robot_state{
	DRIVING,
	LOCALIZATION,
	MODIFY_POSITION,
	COMMUNICATION,
	DO_NOTHING
};

typedef enum robot_state Robot_states;
Robot_states current_robot_state;

static arminfo_t armInfo;
static void driveForwardDuringPickup(void);

/*
 * In order to function properly certain arms need to drive forward during pickup-action
 */

static void driveForwardDuringPickup(){
	forwardDrive(20); //20 cm may suffice
}

/*
 * LCD-task periodically updates and displays the current value in Real Time Timer-register
 */
void vLCDTask(void *pvParam){
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		lcd_set_cursor_pos(1, 0);    /* place cursor in second row, first column */
		sec = get_current_value();   /* retrieve current value in RTT-register */
		display_ascii_nbr(sec);      /* print out number on LCD-display */
		vTaskDelayUntil(&xLastWakeTime, pdMSTOTICKS(1000));
	}
	vTaskDelete(NULL);
}

/*
 * Task that administers entire functionality that concerns robot's automatic self-transportation and
 * movement-mobilization
 *
 */
void vDriveToObjectTask(void *pvParam) {
	portTickType xLastWakeTime = xTaskGetTickCount();
	
	while (1)
	{
		if (current_robot_state == DRIVING)
		{
			uint8_t gotoVal = goToNext();
			if(gotoVal == 1)
			{
				current_robot_state = LOCALIZATION;
				current_twi_state = START_PICKUP;
			}
			else if(gotoVal == 2)
			{
				current_robot_state = LOCALIZATION;
				current_twi_state = START_DROP_OFF;
			}
			else if(gotoVal == 3)
			{
				// Mission accomplished!
				current_robot_state = DO_NOTHING;
			}
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
 * If the robot detects an object it might need to adjust its position in relation to the object's position
 * in order to facilitate pick-up. We want the robot to be strategically placed at the desired 
 * distance and angle for a successful pick-up. 
 *
 * The task uses its handle to eliminate itself after the work has been accomplished
 *
 */
void vModifyPositionTask(void *pvParam) {
	
	while(1){
		
		if (current_robot_state == MODIFY_POSITION)
		{
			if (angleUltraSensor == 90) // object is right in front of robot
			{
				/* Target farther away than reaching-ability of arm */
				if ((distanceUltraSensor + 32) > armInfo.objectDistance) 
				{
					int travelDistToObj = (distanceUltraSensor + 32) - armInfo.objectDistance;
					forwardDrive(travelDistToObj); 
				}
				/* Robot is too close to object */
				if ((distanceUltraSensor + 32) < armInfo.objectDistance)
				{
					int travelDistToObj = armInfo.objectDistance - (distanceUltraSensor + 32);
					reverseDrive(travelDistToObj);	
				}
				current_robot_state = COMMUNICATION;
				distanceUltraSensor = 0;
				angleUltraSensor = 0;
				WITHIN_RANGE_FLAG = 0;
				vTaskDelete(NULL);
			}
			else //rotation required
			{
				if (angleUltraSensor > 90)
				{
					rotateRightByDegrees(angleUltraSensor-90);
					if ((distanceUltraSensor + 32) > armInfo.objectDistance)
					{
						int travelDistToObj = (distanceUltraSensor + 32) - armInfo.objectDistance;
						forwardDrive(travelDistToObj);
					}
					if ((distanceUltraSensor + 32) < armInfo.objectDistance)
					{
						int travelDistToObj = armInfo.objectDistance - (distanceUltraSensor + 32);
						reverseDrive(travelDistToObj);
					}
				}
				if (angleUltraSensor < 90)
				{
					rotateLeftByDegrees(90-angleUltraSensor);
					printf("\n----------------Modifying driving: rotating left %i \n", (90-angleUltraSensor) );
					if ((distanceUltraSensor + 32) > armInfo.objectDistance)
					{
						int travelDistToObj = (distanceUltraSensor + 32) - armInfo.objectDistance;
						forwardDrive(travelDistToObj);
					}
					if ((distanceUltraSensor + 32) < armInfo.objectDistance)
					{
						int travelDistToObj = armInfo.objectDistance - (distanceUltraSensor + 32);
						reverseDrive(travelDistToObj);
					}
				}
				current_robot_state = COMMUNICATION;
				distanceUltraSensor = 0;
				angleUltraSensor = 0;
				WITHIN_RANGE_FLAG = 0;
				vTaskDelete(NULL);
			}
		}
		else
		{
			vTaskDelay(pdMSTOTICKS(200));
		}
	}
	vTaskDelete(NULL);
}

/*
 * The task below handles close-range localization. An ultrasonic sensor is used to detect an object
 * within a pre-defined target distance. Detection of object is indicated by raised flag in servoControll().
 * 
 */
void vLocalizationTask(void *pvParam) {
	portTickType xLastWakeTime = xTaskGetTickCount();
	static int rotationCount = 0;
	int boolean = 0;
	while (1)
	{
		if (current_robot_state == LOCALIZATION)
		{	
			testingUltraSound();
			rotationCount++;
			
			/* If true, an object has been detected */
			if (WITHIN_RANGE_FLAG == 1)
			{
				/* Create task that modifies robot position relative to object-position */
				xTaskCreate(vModifyPositionTask, "Modify Position", 1000, NULL, 1, pxTaskModifyPosition);
				boolean = 1;
			}
			
			/* Maximum servomotor-rotation: 360 degrees */
			if (rotationCount > 360)
			{
				if (boolean == 1)
				{
					current_robot_state = MODIFY_POSITION;
					boolean = 0;
				}
				rotationCount = 0;
			}
			/* Max-time for one-degree-sweep is 92 ms, restart task after 100 ms*/
			vTaskDelayUntil(&xLastWakeTime, pdMSTOTICKS(100)); 
		}
		else // continue blocking
		{
			vTaskDelay(pdMSTOTICKS(100));
		}
	}
	vTaskDelete(NULL);
}

/*
 * This task manages TWI-communication. When robot-start launches, this is the first task to be entered
 * To begin with, information from the robot-arm is queried and gathered if present. This start-up takes
 * highest amount of time. Furthermore the task is responsible for managing the robot-arm before, furing 
 * and after pick-up of an object.  
 */
void vCommunicationTask(void *pvParam)
{
	portTickType xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if (current_robot_state == COMMUNICATION)
		{
			//check state for communication
			switch (current_twi_state)
			{
				/* Gather necessary information about arm, such as desired distance to object, and position of objects */
				case INIT_ARM:
					arminfo_t armInfo;
					armInfo.boxAngle=0;
					armInfo.boxDistance=0;
					armInfo.collectAll=0;
					armInfo.hasData=0;
					armInfo.objectAngle=0;
					armInfo.objectDistance=0;
					armInfo=twi_getArmInfo();
					if(armInfo.hasData)
					{
						//get nav info
						//get object pos
						//get Info about objects
						objectinfo_t sock;
						sock.theObject = SOCK;
						sock.xpos=0;
						sock.ypos=0;
						
						uint8_t res =twi_navGetSockPos(&sock);
						
						objectinfo_t square;
						square.theObject = SQUARE;
						square.xpos=0;
						square.ypos=0;
						
						twi_navGetSquarePos(&square);
						objectinfo_t glass;
						glass.theObject = GLASS;
						glass.xpos=0;
						glass.ypos=0;
						
						twi_navGetGlassPos(&glass);
						printf("glass: x=%d, y=%d.\n",glass.xpos,glass.ypos);
						objectinfo_t boxgoal;
						boxgoal.theObject = BOXGOAL;
						boxgoal.xpos=50;
						boxgoal.ypos=0;

						if(res == 1)
						{	
							setObjectSimple(sock);
							setObjectSimple(square);
							setObjectSimple(glass);
							setObjectSimple(boxgoal);
							setCollectAll(armInfo.collectAll);
							current_robot_state = DRIVING;
						}
						else
						{
							printf("INIT NAV NO DATA");
							twi_master_disable(TWI_PORT);
							vTaskDelay(pdMSTOTICKS(20));
							twi_master_enable(TWI_PORT);
							vTaskDelay(pdMSTOTICKS(100));
							current_robot_state = COMMUNICATION;
						}
					}
					else
					{
						printf("INIT ARM NO DATA");			
 						twi_master_disable(TWI_PORT);
 						vTaskDelay(pdMSTOTICKS(20));
 						twi_master_enable(TWI_PORT);
					}
				break;
				case START_PICKUP:
					
					//start pickup after modifying position
					if (twi_pickupStart() == 1)
					{
						// Able to start pickup
						current_twi_state = GET_STATUS_PICKUP;
						
					}
					else
					{
						//failed to start pickup!! try again!
						puts("FAILED TO START_PICKUP");
						current_twi_state=START_PICKUP;
					}
				break;
				
				/* Pick-up has been initiated, monitor status during this course */
				
				case GET_STATUS_PICKUP:
					;
					
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
						break;
						case PICKUP_DONE:
							//done with pickup, continue to drive
							//stop communicating
							//tell (set) movement that pickup is done
							setDonePickup();
							current_robot_state = DRIVING;
						break;						
						case PICKUP_FORWARD:
						case PICKUP_BACKWARD:
							driveForwardDuringPickup();
							twi_pickupSendMovementDone();
						break;
						
						default:
						    printf("UNHANDLED PICKUP STATUS: %x\n",status);
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
							if(armInfo.collectAll)
							{
								/* If objects were gathered in one drive, do not enter any task */
								current_robot_state = DO_NOTHING; 
							}
							else
							{
								current_robot_state = DRIVING;
							}
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
					printf("Failed TWI switch %d\n",current_twi_state);
				break;
			}
			
			vTaskDelayUntil(&xLastWakeTime, pdMSTOTICKS(65));
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
	/* Initiate system-functionalities */
	sysclk_init();
	board_init();
	configure_console();
	TC0_init();
	twi_comInit();
	pulse_init();
	lcd_init();
	
	lcd_write_str("RTT Time:");
	lcd_set_cursor_pos(1, 0);
	start_RTT(); 
	sec = 0;
	
	/* Initiate pulse-counter handlers */
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);
	
	current_twi_state = INIT_ARM;
	current_robot_state = COMMUNICATION;
	
	ioport_init();
	
	/* Pins of Ultrasonic sensor */
	ioport_set_pin_dir(trig, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(servo, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(servo, LOW);
	
	/* Create robot-application tasks */
	if(xTaskCreate(vCommunicationTask, "Communication", 1000, NULL, 2, pxTaskCommunication) != pdPASS){
		printf("Failed to create Communication-task");
	}
	
	if(xTaskCreate(vDriveToObjectTask, "DriveToObject", 1000, NULL, 2, pxTaskDriveToObject) != pdPASS){
		printf("Failed to create DriveToObject-task");
	}

	if(xTaskCreate(vLocalizationTask, "Localization", 1000, NULL, 2, pxTaskLocalization) != pdPASS){
		printf("Failed to create Localization-task");
	}

	if(xTaskCreate(vLCDTask, "LCD", 1000, NULL, 1, pxTaskLCD) != pdPASS){
		printf("Failed to create LCD-task");
	}
	
	vTaskStartScheduler();
	
	while (1)
	{
		
	}
}
