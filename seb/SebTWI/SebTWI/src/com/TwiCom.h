////
//// Created by trevl on 2017-05-11.
////
//
//#include <stdint.h>
//
//
//#ifndef DUEIO_TWICOM_H
//#define DUEIO_TWICOM_H
//
////TWI state
//typedef enum TWI_CMD {
//
//	//INIT REQ
//	TWI_CMD_ARM_INIT = 					0x20,
//	TWI_CMD_ARM_REQ_BOX_INFO = 			1,
//	TWI_CMD_ARM_REQ_OBJ_INFO = 			2,
//	TWI_CMD_ARM_REQ_COLLECT_INFO = 		3,
//
//	TWI_CMD_PICKUP_START= 				0x22,
//	TWI_CMD_PICKUP_STATUS= 				0x24,
//	TWI_CMD_DROPOFF_START= 				0x21,
//	TWI_CMD_DROPOFF_STATUS=				0x23,
//	TWI_CMD_ERROR=						0x25,
//} twi_cmd;
//
//typedef struct ArmInfo{
//	uint8_t boxDistance;
//	uint8_t boxAngle;
//	uint8_t objectDistance;
//	uint8_t objectAngle;
//	//Boolean. true that arm can collect all obj. then dropoff
//	//false if need to dropoff each obj. after pickup
//	uint8_t collectAll;
//
//}armInfo;
//
//typedef enum PICKUPSTATUS{
//	PICKUP_DONE,
//	PICKUP_FORWARD,
//	PICKUP_BACKWARD,
//	PICKUP_RUNNING,
//	PICKUP_FAILED,
//	PICKUP_DONE_DRIVE
//}PICKUP_STATUS;
//
//typedef enum DROPOFFSTATUS{
//	DROPOFF_DONE,
//	DROPOFF_RUNNING,
//	DROPOFF_FAILED
//}DROPOFF_STATUS;
//
//typedef enum object{
//	SOCK,
//	SQUARE,
//	GLASS
//}OBJECT;
//
//armInfo theArmInfo;
//
////initalize twi
//void twiCom_init(void);
//
////returns success/fail (1/0)
////and saves result in theArmInfo
//uint8_t twi_requestArmInfo(void);
//
//armInfo twi_getArmInfo(void);
//
////send command for picking up to arm
/////returns success/fail (1/0)
//uint8_t twi_sendStartPickup(OBJECT whatObject);
//
////send command for continue pickingup
/////returns success/fail (1/0)
//uint8_t twi_sendContinuePickup(void);
//
////request status of arm while doing pickup
//PICKUP_STATUS twi_reqPickupStatus(void);
//
//
////send command for dropoff to arm
/////returns success/fail (1/0)
//uint8_t twi_sendStartDropoff();
//
//
////request status of arm while doing dropoff
//DROPOFF_STATUS twi_reqDropoffStatus(void);
//
////prints out the command
//void handleCmd(uint8_t cmd);
//
//#endif //DUEIO_TWICOM_H
