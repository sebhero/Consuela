
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

#include "communication/communication.h"

xTaskHandle *pxTaskGotoObject;
xTaskHandle *pxTaskLocate;
xTaskHandle *pxTaskDump;
xTaskHandle *pxTaskPickup;
xTaskHandle *pxTaskController;
xTaskHandle *pxTaskTwi;

xQueueHandle xObjectQueue;
xSemaphoreHandle xCheckSemaphore;
xSemaphoreHandle xSemaphoreTWI;
#define D7  IOPORT_CREATE_PIN(PIOC, 23)


#define pulseh_ch 0
#define pulsev_ch 1

typedef struct {
	uint32_t object_ID;
	uint32_t done;	
	xSemaphoreHandle *pxSemaphore;
} gototask_t;

enum state{
	INITIALIZE_ARM,
	POSITION_ONE,
	POSITION_TWO,
	POSITION_THREE,
	POSITION_FOUR,
	LOCATE,
	PICKING_UP,
	DROP_OFF,
};

typedef enum state state_t;
state_t current_state;
state_t next_state;


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
	
	gototask_t *todo_task = (gototask_t *) pvParam;
	vTaskDelay(200);
	todo_task->done = 1;
	xSemaphoreGive(*(todo_task->pxSemaphore));	
	printf("\nLocating!!!!!");
	vTaskDelete(NULL);

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

void vTwiTask(void *pvParameter){
	uint8_t dum[3];
	dum[0] = TWI_CMD_FROM_ARM_ID;
	dum[1] = 0;
	dum[2] = 0;
	switch ()
	{
	case :
		break;
	}
	uint8_t result = sendArm(dum, 3);
	if (result)
	{
		//puts("Send successful");
	}else{
		//puts("Fail");
	}
	vTaskDelay(pdMSTOTICKS(40));
	uint8_t rec[3] = {0};
	result = reciveFromArm(rec,3);
	if (result)
	{
		printf("Receive successful: %u, %u, %u\n", rec[0], rec[1], rec[2]);
		//vTaskDelay(pdMSTOTICKS(10));
	}else{
		puts("Receive failed");
		//vTaskDelay(pdMSTOTICKS(10));
	}
	xSemaphoreGive(xSemaphoreTWI);
	vTaskDelete(NULL);
}

void vController(void *pvParam) {
	uint32_t objectID[] = {0, 1, 2, 3};	
	/*
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
	*/
	while (1)
	{
		switch (current_state)
		{
			case INITIALIZE_ARM:
			     //get arm info
			     //get object pos info 1-4 (with goal box)
				 ioport_set_pin_level(D7, 1);
			     xTaskCreate(vTwiTask, "TWI", 1000, NULL, 1, pxTaskTwi);
			     vTaskDelay(pdMSTOTICKS(300));
			     if(xSemaphoreTake(xSemaphoreTWI, pdMSTOTICKS(10000)) == pdTRUE){
			     }
				 ioport_set_pin_level(D7, 0);
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
				next_state = LOCATE;
			case POSITION_THREE:
				xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
				xQueueSendToBack(xObjectQueue, (void *) &objectID[3], 0);
				vTaskDelay(pdMSTOTICKS(100));
				if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
				{
					printf("\nSemaphore has been received");
				}
				next_state = LOCATE;
			case POSITION_FOUR:
				xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
				xQueueSendToBack(xObjectQueue, (void *) &objectID[0], 0);
				vTaskDelay(pdMSTOTICKS(100));
				if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
				{
					printf("\nSemaphore has been received");
				}
				next_state = DROP_OFF;
			case LOCATE:
		        vTaskDelete(NULL);    
			case PICKING_UP:
				vTaskDelete(NULL);
			case DROP_OFF:
				vTaskDelete(NULL); 
		}
		current_state = next_state; 
	}

	/*
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
	initTwi();
	ioport_set_pin_dir(D7, IOPORT_DIR_OUTPUT);
	current_state = INITIALIZE_ARM;
	next_state = INITIALIZE_ARM;
	xObjectQueue = xQueueCreate(4, sizeof(uint32_t));
	xTaskCreate(vController, "Controller", 1000, NULL, 2, pxTaskController);
	vSemaphoreCreateBinary(xCheckSemaphore);
	vSemaphoreCreateBinary(xSemaphoreTWI);
	/*
	xTaskCreate(vWheelRegulating, "WheelRegulating", 1000, NULL, 1, pxWheelRegulatingHandle);
	xTaskCreate(vTask2, "Task 2", 1000, NULL, 1, pxTask2Handle);
	xTaskCreate(vControllerTask, "Controller task", 1000, NULL, 2, pxControllerHandle);
	*/
	
	vTaskStartScheduler();
	
	while(1) {
		
	}
	
}
