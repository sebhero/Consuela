
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
xTaskHandle *pxTaskTwiNav;

xQueueHandle xObjectQueue;
xSemaphoreHandle xCheckSemaphore;
xSemaphoreHandle xSemaphoreTWI;
xSemaphoreHandle xSemaphoreTWINav;
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
	NAV_COM
};

typedef enum state state_t;
state_t current_state;
state_t next_state;


void vGotoTask( void *pvParam) {
	uint32_t receivedValue;
	if(xQueueReceive(xObjectQueue, &receivedValue, 0) == pdPASS){
		printf("\nSuccessfully received value: %u\n", receivedValue);
		vTaskDelay(pdMSTOTICKS(100));
	}
	
	printf("\nGiving semaphore");
	puts("");
	xSemaphoreGive(xCheckSemaphore);
	
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
	puts("TWI TASK Started");
	vTaskDelay(pdMSTOTICKS(20));
//	test send all cmds	
//	twi_testCmds();

//	test to cmds send 2 slaves
//	twi_test2slaves();

// 	puts("REQ arm info");
// 	vTaskDelay(pdMSTOTICKS(20));
// 	
// 	//test get armInfo
 	arminfo_t armInfo = twi_getArmInfo();
	if(armInfo.hasData)
 	{
		printf("armInfo: boxA: %d boxD: %d objA %d objD %d col: %d",armInfo.boxAngle,armInfo.boxDistance, armInfo.objectAngle, armInfo.objectDistance, armInfo.collectAll);
 		vTaskDelay(pdMSTOTICKS(20));
	 }
	
	//test pick cmds start and status
//	twi_test_pickup();
	
	//test dropoff cmd start and status
//	twi_test_dropoff();
	
	xSemaphoreGive(xSemaphoreTWI);
	vTaskDelete(NULL);
}


void vTaskTwiNav(void *pvParameter){
	puts("NAV TASK Started");
	vTaskDelay(pdMSTOTICKS(20));
	//maybe not needed
	twi_changeSlave(SLAVE_ADDR_NAV);
	//do send to NAV
	void twi_sendNavCmd();
	
	
	xSemaphoreGive(xSemaphoreTWINav);
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
				 
			     xTaskCreate(vTwiTask, "TWI", 1000, NULL, 1, pxTaskTwi);
			     vTaskDelay(pdMSTOTICKS(300));
			     if(xSemaphoreTake(xSemaphoreTWI, pdMSTOTICKS(10000)) == pdTRUE){
			     }
				 vTaskDelay(pdMSTOTICKS(100));
			     next_state = NAV_COM;
			break;
			case NAV_COM:
			//handle nav communication							     
				xTaskCreate(vTaskTwiNav, "TWI NAV", 1000, NULL, 1, pxTaskTwiNav);
				vTaskDelay(pdMSTOTICKS(300));
				if(xSemaphoreTake(xSemaphoreTWINav, pdMSTOTICKS(10000)) == pdTRUE){
				}
				vTaskDelay(pdMSTOTICKS(100));
				next_state = INITIALIZE_ARM;
			break;
			case POSITION_ONE:
			     xTaskCreate(vGotoTask, "GotoObject", 1000, NULL, 1, NULL);
			     xQueueSendToBack(xObjectQueue, (void *) &objectID[1], 0);
			     vTaskDelay(pdMSTOTICKS(100));
			     if (xSemaphoreTake(xCheckSemaphore, pdMSTOTICKS(10000)) == pdTRUE) //wait in Blocked state for semaphore max 10 s
			     {
				     //printf("\nSemaphore has been received");
			     }
				 next_state = INITIALIZE_ARM;
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
	//init TWI
	twi_comInit();
	ioport_set_pin_dir(D7, IOPORT_DIR_OUTPUT);
	current_state = INITIALIZE_ARM;
	next_state = INITIALIZE_ARM;
	xObjectQueue = xQueueCreate(4, sizeof(uint32_t));
	xTaskCreate(vController, "Controller", 1000, NULL, 2, pxTaskController);
	vSemaphoreCreateBinary(xCheckSemaphore);
	vSemaphoreCreateBinary(xSemaphoreTWI);
	vSemaphoreCreateBinary(xSemaphoreTWINav);
	/*
	xTaskCreate(vWheelRegulating, "WheelRegulating", 1000, NULL, 1, pxWheelRegulatingHandle);
	xTaskCreate(vTask2, "Task 2", 1000, NULL, 1, pxTask2Handle);
	xTaskCreate(vControllerTask, "Controller task", 1000, NULL, 2, pxControllerHandle);
	*/
	
	vTaskStartScheduler();
	
	while(1) {
		
	}
	
}

