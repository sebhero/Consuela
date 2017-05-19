////
//// Created by trevl on 2017-05-11.
////
//
//#include "TwiCom.h"
//#include "../com/Com.h"
////#include "Com.h"
//
//
//void printData(uint8_t* p_data)
//{
//	puts("data is");
//	printf("Data0: %x\n",*(p_data+0));
//	printf("Data1: %d\n",*(p_data+1));
//	printf("Data2: %d\n",*(p_data+2));
//
//
//};
//
////initalize twi
//void twiCom_init(void){
//	initTwi();
//}
//
////returns success/fail (1/0)
////and saves result in theArmInfo
//uint8_t twi_requestArmInfo(void){
//
//	//does 3x request to arm to get info
//
//	//get Box info
//	//send data
//	uint8_t data[3]={TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_BOX_INFO,0};
//	puts("send 1st");
//	printData(data);
//
//	//recive data holder
//	uint8_t recv[3]={0};
//
//	//the TWI get data funtion
//	uint8_t result=twi_writeCmd(data);
//	if(0== result)
//	{
//		//failed
//		return 0;
//	}
//	delay_ms(50);
//	result = twi_read(recv);
//	if(0== result)
//	{
//		//failed
//		return 0;
//	}
//	//all good
//	//save box info
//	theArmInfo.boxDistance = 	recv[1];
//	theArmInfo.boxAngle = 		recv[2];
//
//	//2nd request
//	data[1]=TWI_CMD_ARM_REQ_OBJ_INFO;
//
//	puts("send 2nd");
//	printData(data);
//	//clean up revc data holer
//	recv[0]=0;
//	recv[1]=0;
//	recv[2]=0;
//
//
//	//the TWI get data funtion
//	result=twi_writeCmd(data);
//	if(0== result)
//	{
//		//failed
//		return 0;
//	}
//
//	delay_ms(50);
//	result = twi_read(recv);
//	if(0== result)
//	{
//		//failed
//		return 0;
//	}
//	//save object info
//	theArmInfo.objectDistance=	recv[1];
//	theArmInfo.objectAngle= 	recv[2];
//
//	//3rd req
//	data[1]=TWI_CMD_ARM_REQ_COLLECT_INFO;
//	puts("send 3rd");
//	printData(data);
//
//	//clean up revc data holer
//	recv[0]=0;
//	recv[1]=0;
//	recv[2]=0;
//	//the TWI get data funtion
//	result=twi_writeCmd(data);
//	if(0== result)
//	{
//		//failed
//		return 0;
//	}
//
//	delay_ms(50);
//	result = twi_read(recv);
//	if(0== result)
//	{
//		//failed
//		return 0;
//	}
//	//save object info
//	theArmInfo.collectAll =		recv[1];
//
//
//	return 1;
//}
//
//armInfo twi_getArmInfo(void){
//	return theArmInfo;
//}
//
////send command for picking up to arm
/////returns success/fail (1/0)
//uint8_t twi_sendStartPickup(OBJECT whatObject){
//	uint8_t data[3]={TWI_CMD_PICKUP_START,whatObject,0};
//	return twi_writeCmd(data);
//}
//
////send command for continue pickingup
/////returns success/fail (1/0)
//uint8_t twi_sendContinuePickup(void){
//	uint8_t data[3]={TWI_CMD_PICKUP_STATUS,PICKUP_DONE_DRIVE,0};
//	return twi_writeCmd(data);
//}
//
//
////request status of arm while doing pickup
//PICKUP_STATUS twi_reqPickupStatus(void){
//	uint8_t recived[3]={0};
//	uint8_t result = twi_read(recived);
//	if (result) {
//		puts("twi_reqPickupStatus Success fulle read");
//		printf("twi_reqPickupStatus [%d, %d, %d]\n",recived[0],recived[1],recived[2]);
//		return recived[1];
//
//		} else {
//		puts("twi_reqPickupStatus faild to read");
//	}
//
//}
//
////send command for dropoff to arm
/////returns success/fail (1/0)
//uint8_t twi_sendStartDropoff(){
//	uint8_t data[3]={TWI_CMD_DROPOFF_START,0,0};
//	return twi_writeCmd(data);
//}
//
//
////request status of arm while doing dropoff
//DROPOFF_STATUS twi_reqDropoffStatus(void){
//	uint8_t recived[3]={0};
//	uint8_t result = twi_read(recived);
//	if (result) {
//		puts("twi_reqPickupStatus Success fulle read");
//		printf("twi_reqPickupStatus [%d, %d, %d]\n",recived[0],recived[1],recived[2]);
//		return recived[1];
//
//		} else {
//		puts("twi_reqPickupStatus faild to read");
//	}
//}
//
//
//void handleCmd(uint8_t cmd) {
//
//	printf("got in hex: %x\n",cmd);
//	printf("got in dec: %d\n",cmd);
//
//	switch (cmd) {
//		case TWI_CMD_ARM_INIT:
//		printf("TWI_CMD_ARM_INIT\n");
//		break;
//		case TWI_CMD_ARM_REQ_BOX_INFO:
//		printf("TWI_CMD_ARM_REQ_BOX_INFO\n");
//		break;
//		case TWI_CMD_ARM_REQ_OBJ_INFO:
//		printf("TWI_CMD_ARM_REQ_OBJ_INFO\n");
//		break;
//		case TWI_CMD_ARM_REQ_COLLECT_INFO:
//		printf("TWI_CMD_ARM_REQ_COLLECT_INFO\n");
//		break;
//		case TWI_CMD_PICKUP_START:
//		printf("TWI_CMD_PICKUP_START\n");
//		break;
//		case TWI_CMD_PICKUP_STATUS:
//		printf("TWI_CMD_PICKUP_STATUS\n");
//		break;
//		case TWI_CMD_DROPOFF_START:
//		printf("TWI_CMD_DROPOFF_START\n");
//		break;
//		case TWI_CMD_DROPOFF_STATUS:
//		printf("TWI_CMD_DROPOFF_STATUS\n");
//		break;
//		case TWI_CMD_ERROR:
//		printf("TWI_CMD_ERROR\n");
//		break;
//		default:
//		printf("error\n");
//	}
//}
