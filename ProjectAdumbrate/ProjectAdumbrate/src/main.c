
#include <asf.h>
#include <FreeRTOSConfig.h>
#include "TimerCounter.h"
#include "conf_board.h"
#include "pulseCounterHandler.h"
#include "pulse.h"
#include "Motorfunc.h"
#include "navigation.h"
#include <inttypes.h>
#include "Hjulreglering.h"
#include <queue.h>
#include "Com.h"
#include "ultra_servo.h"

#include "communication/communication.h"

xTaskHandle *pxTaskGotoObject;
xTaskHandle *pxTaskLocate;
xTaskHandle *pxTaskDump;
xTaskHandle *pxTaskPickup;
xTaskHandle *pxTaskController;
xTaskHandle *pxTaskTwi;

xQueueHandle xObjectQueue;
xQueueHandle xPickupStatusQueue;
xSemaphoreHandle xCheckSemaphore;
xSemaphoreHandle xSemaphoreTWI;
#define D7  IOPORT_CREATE_PIN(PIOC, 23)

static arminfo_t armInfo;
static uint8_t pickUpStarted = 0;
static uint8_t dropOffStarted = 0;

#define pulseh_ch 0
#define pulsev_ch 1

typedef struct {
	uint32_t object_ID;
	uint32_t done;	
	xSemaphoreHandle *pxSemaphore;
} gototask_t;

enum controller_states{
	INITIALIZE_ARM,
	POSITION_ONE,
	POSITION_TWO,
	POSITION_THREE,
	POSITION_FOUR,
	LOCATE,
	PICKING_UP,
	DROP_OFF,
	MODIFY_POSITION
};

typedef enum controller_states state_t;
state_t current_state;
state_t next_state;

enum twi_states{
	INIT_ARM,
	START_PICKUP,
	PICKUP_STATUS_RX,
	START_DROP_OFF,
	DROP_OFF_STATUS,
	IDLE
};

typedef enum twi_states TWI_state;
TWI_state current_twi_state;
TWI_state next_twi_state;

static uint32_t nextPos = 0;

void vGotoTask( void *pvParam) {
	uint32_t receivedValue;
	if(xQueueReceive(xObjectQueue, &receivedValue, 0) == pdPASS){
		printf("\nSuccessfully received value: %u", receivedValue);
		vTaskDelay(pdMSTOTICKS(100));
	}
	
	printf("\nGiving semaphore");
	xSemaphoreGive(xCheckSemaphore);
	
	/*
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);

	pulse_init();

	pulse_set_period(pulsev_ch, 1500);
	pulse_set_period(pulseh_ch, 1500);

	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
	
	int degreesToPos;
	double tempVariabel = 0;
	
	
	valuesCalc(receivedValue);
	degreesToPos = angleToPos();
	
	printf("\nRotating %d", degreesToPos);
	printf("\n");
	
	if (degreesToPos < 0){
		degreesToPos = abs(degreesToPos);
		rotateRightByDegrees(degreesToPos);
		} else{
		rotateLeftByDegrees(degreesToPos);
	}
	updateAngle();

	while (distanceToPosition(receivedValue)>30.0){
		
		vTaskDelay(pdMSTOTICKS(500));
		int ek = counterA-counterB;
		tempVariabel = counterA*1.355;
		reglerahjul3(ek);
		updatePos(tempVariabel);
		tempVariabel = 0;
		printf("... rollin' ...");

	}
	
	stop();
	
	printf("\nGiving semaphore");
	xSemaphoreGive(xCheckSemaphore);
	*/
	 /*
	uint32_t objects;
	
	gototask_t *todo_task = (gototask_t *) pvParam;
	
	printf("\nRobin trying to go to %lu", todo_task->object_ID);
	
	pulseCounter_configA(ID_PIOC, PIOC, PIO_PC28);
	pulseCounter_configB(ID_PIOC, PIOC, PIO_PC23);

	pulse_init();	

	pulse_set_period(pulsev_ch, 1500);
	pulse_set_period(pulseh_ch, 1500);

	pulse_start(pulseh_ch);
	pulse_start(pulsev_ch);
		
	int degreesToPos;
	double tempVariabel = 0;
	
	
	valuesCalc(todo_task->object_ID);	
	degreesToPos = angleToPos();
	
	printf("\nRotating %d", degreesToPos);
	
	if (degreesToPos < 0){
		degreesToPos = abs(degreesToPos);
		rotateRightByDegrees(degreesToPos);	
	} else{
		rotateLeftByDegrees(degreesToPos);		
	}			
	updateAngle();

	while (distanceToPosition(todo_task->object_ID)>30.0){
		
		vTaskDelay(pdMSTOTICKS(500));		
		printf("X");
		int ek = counterA-counterB;
		tempVariabel = counterA*1.355;
		reglerahjul3(ek);
		printf("Y");
		updatePos(tempVariabel);
		printf("Z");
		tempVariabel = 0;

	}
	
	stop();	
	
	todo_task->done = 1;
	xSemaphoreGive(*(todo_task->pxSemaphore));
	vTaskDelete(NULL);

	*/
	vTaskDelete(NULL);
}



void vLocateTask( void *pvParam) {
	while (1)
	{
		testingUltraSound();
		printf("\nLocating ...");
		vTaskPrioritySet(NULL, 1); // set priority lower than Controller-task
		vTaskDelay(pdMSTOTICKS(1));
	}
	vTaskDelete(NULL);
	/*
	gototask_t *todo_task = (gototask_t *) pvParam;
	vTaskDelay(200);
	todo_task->done = 1;
	xSemaphoreGive(*(todo_task->pxSemaphore));	
	printf("\nLocating!!!!!");
	vTaskDelete(NULL);
	*/

}

void vPickupTask(void *pvParam) {
	gototask_t *todo_task = (gototask_t *) pvParam;
	vTaskDelay(200);
	todo_task->done = 1;
	xSemaphoreGive(*(todo_task->pxSemaphore));		
	printf("\nPICKING UP!!!!!");
	
	vTaskDelete(NULL);
}

void vDumpTask(void *pvParam) {
	gototask_t *todo_task = (gototask_t *) pvParam;
	vTaskDelay(200);
	todo_task->done = 1;
	xSemaphoreGive(*(todo_task->pxSemaphore));
	printf("\nDUMPING!!!!!");
	vTaskDelete(NULL);
}


uint8_t test=TWI_CMD_ARM_INIT;


//TWI test send all cmds. loops through the commands
// and recives data from them
void twi_testCmds()
{
	uint8_t dum[3];
	dum[0] = test;
	dum[1] = 0;
	dum[2] = 0;

	uint8_t result = twiSendData(dum, 3);
	
	if (result)
	{
		puts("Send successful");
		}else{
		puts("Fail");
	}
	
	if(test < TWI_CMD_ERROR)
	{
		test++;
	}
	else
	{
		test = TWI_CMD_ARM_INIT;
	}
	
	
	vTaskDelay(pdMSTOTICKS(60));
	uint8_t rec[3] = {0};
	result = twiReciveData(rec,3);
	if (result)
	{
		printf("Receive successful: %u, %u, %u\n", rec[0], rec[1], rec[2]);
		vTaskDelay(pdMSTOTICKS(10));
		}else{
		puts("Receive failed");
		vTaskDelay(pdMSTOTICKS(10));
	}
}

int testslave =0;

void twi_test2slaves(void)
{
	
	
	
	if(testslave){
		puts("slave is NAV");
		testslave=0;
		twi_changeSlave(SLAVE_ADDR_ARM);
	}
	else{
		testslave=1;
		puts("slave is ARM");
		twi_changeSlave(SLAVE_ADDR_NAV);
	}
	vTaskDelay(pdMSTOTICKS(10));
	twi_testCmds();
}

void twi_test_getArminfo()
{
	puts("REQ arm info");
	vTaskDelay(pdMSTOTICKS(20));
	
	//test get armInfo
	arminfo_t armInfo = twi_getArmInfo();
	if(armInfo.hasData)
	{
		printf("armInfo: boxA: %d boxD: %d objA %d objD %d col: %d",armInfo.boxAngle,armInfo.boxDistance, armInfo.objectAngle, armInfo.objectDistance, armInfo.collectAll);
		vTaskDelay(pdMSTOTICKS(20));
	}
}

uint8_t startpickup=0;
uint8_t drive= 0;
void twi_test_pickup(void)
{
	puts("twi pick");
	if(startpickup==0)
	{
		
		//RETURN 1 IF success
		startpickup=twi_pickupStart();
		if (startpickup ==0)
		{
			puts("failed tot start pickup!");
		}
		
	}
	else{

		
		
		if (twi_pickupGetMasterStatus()==PICKUP_DONE_DRIVE)
		{
			puts("DONE MOVE");
			//tell slave that we are done moving.
			twi_pickupSendMovementDone();
		}
		
		PickupStatus pickupStatus=twi_pickupGetStatus();
		
		switch(pickupStatus){
			case PICKUP_BACKWARD:
			puts("PICKUP_STATUS.PICKUP_BACKWARD:");
			printf("back: %u\n",twi_pickupGetMoveCm());
			//should be handle by driving.. to tell that its done
			twi_pickupSetMasterStatus(PICKUP_DONE_DRIVE);
			break;
			case PICKUP_DONE:
			puts("PICKUP_DONE");
			startpickup=0;
			break;
			case PICKUP_DONE_DRIVE:
			puts("PICKUP_DONE_DRIVE:");
			break;
			case PICKUP_FAILED:
			puts("PICKUP_FAILED");
			break;
			case PICKUP_FORWARD:
			puts("PICKUP_STATUS.PICKUP_FORWARD:");
			printf("forward: %u\n",twi_pickupGetMoveCm());
			//should be handle by driving.. to tell that its done
			twi_pickupSetMasterStatus(PICKUP_DONE_DRIVE);
			
			break;
			case PICKUP_RUNNING:
			puts("PICKUP_RUNNING");
			break;
			default:
			puts("pickup status error");
			startpickup=0;
			break;
		}
	}
	
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

void vTwiTask(void *pvParameter){
	uint8_t stateNumber = (uint8_t) pvParameter;
	current_twi_state = stateNumber;
	printf("\nState number: %u", stateNumber);
	switch (current_twi_state)
	{
	case INIT_ARM:
	    armInfo = twi_getArmInfo();
		break;
	case START_PICKUP:
		if (twi_pickupStart() == 1)
		{
			pickUpStarted = 1;
		} 
		else
		{
			pickUpStarted = 0;
		}
		break;
	case PICKUP_STATUS_RX:
		if (twi_pickupGetMasterStatus()==PICKUP_DONE_DRIVE)
		{
			//tell slave that we are done moving.
			twi_pickupSendMovementDone();
		}
		twi_pickupGetStatus();
		break;
	case START_DROP_OFF:
		if (twi_dropoffStart() == 1)
		{
			dropOffStarted = 1;
		}
		else
		{
			dropOffStarted = 0;
		}
		break;
	case DROP_OFF_STATUS:
	
		break;
	}
	xSemaphoreGive(xSemaphoreTWI);
	vTaskDelete(NULL);
}


void vController(void *pvParam) {
	uint32_t objectID[] = {0, 1, 2, 3};	
	while (1)
	{
		switch (current_state)
		{
			case INITIALIZE_ARM:
			     //get arm info
			     //get object pos info 1-4 (with goal box)
			     xTaskCreate(vTwiTask, "TWI", 1000, INIT_ARM, 1, pxTaskTwi);
			     vTaskDelay(pdMSTOTICKS(300));
			     if(xSemaphoreTake(xSemaphoreTWI, pdMSTOTICKS(10000)) == pdTRUE){
			     }
			     next_state = POSITION_ONE;
			break;
			case POSITION_ONE:
			     xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
			     xQueueSendToBack(xObjectQueue, (void *) &objectID[1], 0);
			     vTaskDelay(pdMSTOTICKS(100));
			     if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
			     {
				     printf("\nSemaphore has been received");
			     }
				 nextPos = 2;
				 next_state = LOCATE;
			break;
			case POSITION_TWO:
				xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
				xQueueSendToBack(xObjectQueue, (void *) &objectID[2], 0);
				vTaskDelay(pdMSTOTICKS(100));
				if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
				{
					printf("\nSemaphore has been received");
				}
				nextPos = 3;
				next_state = LOCATE;
			break;
			case POSITION_THREE:
				xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
				xQueueSendToBack(xObjectQueue, (void *) &objectID[3], 0);
				vTaskDelay(pdMSTOTICKS(100));
				if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
				{
					printf("\nSemaphore has been received");
				}
				nextPos = 4;
				next_state = LOCATE;
			break;
			case POSITION_FOUR:
				xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
				xQueueSendToBack(xObjectQueue, (void *) &objectID[0], 0);
				vTaskDelay(pdMSTOTICKS(100));
				if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
				{
					printf("\nSemaphore has been received");
				}
				next_state = DROP_OFF;
			break;
			case LOCATE:
			    /*
				xTaskCreate(vLocateTask, "LocateObject", 1000, NULL, 1, pxTaskLocate);
				for (uint32_t i = 0; i < 180; i++)
				{
					vTaskPrioritySet(pxTaskLocate, 3); // set priority of vLocatetask higher than priority of Controller-task
					printf("\nLoop: %u", i);
					vTaskDelay(pdMSTOTICKS(100));
					vTaskPrioritySet(pxTaskLocate, 1); 
				}
				vTaskDelete(pxTaskLocate);
				//få ut antal cm till objekt, samt vinkel till objekt
				//sänd info till MODIFY_POSITION 
				printf("\nExited for-loop");
				*/
				printf("\nDone Locating");
				next_state = MODIFY_POSITION;
			break;
			case MODIFY_POSITION:
			    // räkna ut och kör/rotera mha ultraljud/openCV och arm-info
				printf("\nBox angle: %u", armInfo.boxAngle);
				printf("\nBox distance: %u", armInfo.boxDistance);
				printf("\nObject angle: %u", armInfo.objectAngle);
				printf("\nObject distance: %u", armInfo.objectDistance);
				printf("\nCollect all: %u", armInfo.collectAll);
				
				if (twi_pickupGetMasterStatus() == PICKUP_FORWARD)
				{
					//TODO: drive forward, call task
					printf("KÖR FRAMÅÅÅÅÅÅÅÅÅÅÅT! %u", twi_pickupGetMoveCm());
					twi_pickupSetMasterStatus(PICKUP_DONE_DRIVE);
				}
				else if (twi_pickupGetMasterStatus() == PICKUP_BACKWARD)
				{
					//TODO: drive backwards, call task
					printf("KÖR BAKÅÅÅÅÅÅÅÅÅÅT! %u", twi_pickupGetMoveCm());
					twi_pickupSetMasterStatus(PICKUP_DONE_DRIVE);
				}
				
				next_state = PICKING_UP;
				
				//vTaskDelay(NULL);
			break;
			case PICKING_UP:
			    if (pickUpStarted == 0)
			    {
					xTaskCreate(vTwiTask, "TWI", 1000, START_PICKUP, 1, pxTaskTwi);
					vTaskDelay(pdMSTOTICKS(500));
			    }else{

					xTaskCreate(vTwiTask, "TWI", 1000, PICKUP_STATUS_RX, 1, pxTaskTwi);
					vTaskDelay(100);
					switch(twi_pickupGetMasterStatus())
					{
						case PICKUP_BACKWARD:
						case PICKUP_FORWARD:
							next_state = MODIFY_POSITION;
							break;
						case PICKUP_DONE:
						    pickUpStarted = 0;
						//collect all objects
							if(armInfo.collectAll == 1)
							{
								if (nextPos == 2)
								{
									next_state = POSITION_TWO;
								}
								else if (nextPos == 3)
								{
									next_state = POSITION_THREE;
								}
								else if (nextPos == 4)
								{
									next_state = POSITION_FOUR;
								}
								else
								{
									next_state = PICKING_UP;
								}
							}
							else
							{
								//can only collect one object a time
								next_state = POSITION_FOUR;
							}
							
						break;
						case PICKUP_RUNNING:
							next_state = PICKING_UP;
							break;
					}
				}
				//vTaskDelay(pdMSTOTICKS(1000));
					
			break;
			case DROP_OFF:
			    
			    while (1)
			    {
					printf("\nWell laa di daa");
					delay_ms(500);
			    }
				vTaskDelete(NULL);	
			break;	 
		}
		current_state = next_state; 
	}

	/*
	/*
	-----------------------------------------------
	for(uint32_t i = 0; i < 4; i++){
		xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
		xQueueSendToBack(xObjectQueue, (void *) &objectID[i], 0);
		vTaskDelay(pdMSTOTICKS(100));
		if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
		{
			printf("\nSemaphore has been received");
		}
		vTaskDelay(pdMSTOTICKS(300));
		xTaskCreate(vTwiTask, "TWI", 1000, NULL, 1, pxTaskTwi);
		if(xSemaphoreTake(xSemaphoreTWI, pdMSTOTICKS(10000)) == pdTRUE){
		}
	}
	printf("\n");
	printf("\n");
	printf("\nDRIVING AND TWI FINISHED");
	-----------------------------------------
	
	xSemaphoreHandle xSemaphore;
	vSemaphoreCreateBinary( xSemaphore );
	gototask_t task = {
		.object_ID = 0,
		.done = 0,
		.pxSemaphore = &xSemaphore
	};

	
	// Pick up objects
	for(uint32_t i = 0; i < 3; i++) {
		task.object_ID = i;		
		task.done = 0;
		// Goto object i
		xTaskCreate(vGotoTask, "GotoObject", 1000, &task, 1, pxTaskGotoObject);				
		while(task.done != 1) {
			if(xSemaphoreTake(*(task.pxSemaphore), 1000 ) == pdPASS){
				printf("\nRolling...");
			}else{
				printf("\nNot taking semaphore...");
			}
		}
		printf("\nDone Rolling");
		
		// Locate object
		task.done = 0;
		xTaskCreate(vLocateTask, "LocateObject", 1000, &task, 1, pxTaskLocate);
		while(task.done != 1) {
			xSemaphoreTake(*(task.pxSemaphore), 1000 );
			printf("\nLocating...");
		}
		printf("\nDone Locating");
		
		// Pick up object
		task.done = 0;
		xTaskCreate(vPickupTask, "PickupObject", 1000, &task, 1, pxTaskPickup);				
		while(task.done != 1) {
			xSemaphoreTake(*(task.pxSemaphore), 1000 );
			printf("\nPicking up...");
		}		
		printf("\nDone going to object: %lu", i);
	}
	

	
	// Goto dump
	task.object_ID = 3;		
	task.done = 0;
	xTaskCreate(vGotoTask, "GotoDeliver", 1000, &task, 3, pxTaskGotoObject);			
	while(task.done != 1) {
		xSemaphoreTake(*(task.pxSemaphore), 1000 );
		printf("\Rolling to dump...");
	}	
	// Dump objects
	task.done = 0;
	xTaskCreate(vDumpTask, "Deliver", 1000, &task, 3, pxTaskDump);
	while(task.done != 1) {
		xSemaphoreTake(*(task.pxSemaphore), 1000 );
		printf("\DumpinG..");
	}
	
	ioport_set_pin_dir(PIO_PA15_IDX, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIO_PA15_IDX, IOPORT_PIN_LEVEL_LOW);
	while (1) {
		ioport_toggle_pin_level(PIO_PA15_IDX);
		vTaskDelay(500);
	}
	
	*/
	vTaskDelete(NULL);
}


int main (void)
{
	
	sysclk_init();
	board_init();
	configure_console();
	ioport_set_pin_dir(D7, IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(trig, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(echo, IOPORT_DIR_INPUT);
	ioport_set_pin_dir(servo, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(servo, LOW);
	
	twi_comInit();
	current_state = INITIALIZE_ARM;
	next_state = INITIALIZE_ARM;
	
	current_twi_state = INIT_ARM;
	next_twi_state = INIT_ARM;
	
	xObjectQueue = xQueueCreate(4, sizeof(uint32_t));
	xPickupStatusQueue = xQueueCreate(1, sizeof(uint8_t));
	if(xTaskCreate(vController, "Controller", 2000, NULL, 2, pxTaskController) != pdPASS){
		printf("Failed to create task");
	}
	//xTaskCreate(vController, "Controller", 2000, NULL, 2, pxTaskController);
	//xTaskCreate(vLocateTask, "LocateObject", 1000, NULL, 1, pxTaskLocate);
	vSemaphoreCreateBinary(xCheckSemaphore);
	vSemaphoreCreateBinary(xSemaphoreTWI);
	
	vTaskStartScheduler();


	
	while(1) {
		
	}
	
}