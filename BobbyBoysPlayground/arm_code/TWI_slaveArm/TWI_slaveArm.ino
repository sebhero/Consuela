//
// Created by Sebastian Boreback on 2017-05-12.
//

#include <Arduino.h>
#include <Wire.h>


typedef enum {
  SOCK = 2,
  SQUARE = 3,
  GLASS = 4,
  BOXGOAL = 5
} Object;

typedef struct {
  //distances are in cm from center of robot
  uint8_t boxDistance;
  //angles in degrees
  uint8_t boxAngle;
  //distances are in cm from center of robot
  uint8_t objectDistance;
  uint8_t objectAngle;
  //Boolean. true that arm can collect all obj. then dropoff
  //false if need to dropoff each obj. after pickup
  uint8_t collectAll;
  //just to check if the struct is set
  uint8_t hasData;
} arminfo_t;

//Holds info about each object position
typedef struct {
  //the type of object
  Object theObject;
  // x position of object
  int16_t xpos;
  //y position of object
  int16_t ypos;
} objectinfo_t;


//TWI state
typedef enum {

  TWI_CMD_ARM_INIT = 0x20,
  TWI_CMD_DROPOFF_START = 0x21,
  TWI_CMD_PICKUP_START = 0x22,
  TWI_CMD_DROPOFF_STATUS = 0x23,
  TWI_CMD_PICKUP_STATUS = 0x24,
  TWI_CMD_ERROR = 0x25,
  IDLE = 0x0
} TwiCmd;

typedef enum {
  //retuns distance to box goal, and angle to set robot for dropoff
      TWI_CMD_ARM_REQ_BOX_INFO = 2,
  //returns distance to object and angle for collect
      TWI_CMD_ARM_REQ_OBJ_INFO = 3,
  TWI_CMD_ARM_REQ_COLLECT_INFO = 4
} TwiCmdInitReq;

typedef enum {
  PICKUP_DONE = 2,
  PICKUP_FORWARD = 3,
  PICKUP_BACKWARD = 4,
  PICKUP_RUNNING = 5,
  PICKUP_FAILED = 6,
  PICKUP_DONE_DRIVE = 7,
  PICKUP_IDLE = 8,
  PICKUP_ROTATE_L = 9,
  PICKUP_ROTATE_R = 10
} PickupStatus;

typedef enum {
  DROPOFF_DONE = 2,
  DROPOFF_RUNNING = 3,
  DROPOFF_FAILED = 4,
  DROPOFF_IDLE = 5
} DropoffStatus;


//todo remove
typedef enum FSMSTATES {
  ARM_IDLE = 2,
  ARM_PICKUP = 3,
  ARM_DROPOFF = 4
} STATE;

//PickupStatus currentPickupState = PICKUP_IDLE;
//PickupStatus nextPickupState= PICKUP_IDLE;
//DropoffStatus currentDropOffState = DROPOFF_IDLE;
//DropoffStatus nextDropOffState= DROPOFF_IDLE;

//FSM state for the arm
TwiCmd rxCurrent;
TwiCmd rxNext;

/***********************/
//variables
uint8_t recivedData[3] = {0};
uint8_t txBuff[3] = {0};
arminfo_t theArm;
//send to master
PickupStatus txPickupStatus;
//recive from master
PickupStatus rxPickupStatus;
DropoffStatus rxDropoffStatus;
DropoffStatus txDropoffStatus;
uint8_t run;


void handleReadCmd() {

  //data to send
  //uint8_t data[3] = {};
Serial.println("handleReadCmd");


  uint8_t cmd = recivedData[0];
  

  switch (cmd) {
    case TWI_CMD_ARM_INIT:
      Serial.println("TWI_CMD_ARM_INIT");
      txBuff[0] = cmd;
      switch (recivedData[1]) {
        case TWI_CMD_ARM_REQ_BOX_INFO:
          //Serial.println("TWI_CMD_ARM_REQ_BOX_INFO");
          txBuff[1] = theArm.boxDistance;
          txBuff[2] = theArm.boxAngle;
          break;
        case TWI_CMD_ARM_REQ_OBJ_INFO:
          //Serial.println("TWI_CMD_ARM_REQ_OBJ_INFO:");
          txBuff[1] = theArm.objectDistance;
          txBuff[2] = theArm.objectAngle;
          break;
        case TWI_CMD_ARM_REQ_COLLECT_INFO:
          //Serial.println("TWI_CMD_ARM_REQ_COLLECT_INFO:");
          txBuff[1] = theArm.collectAll;
          break;
        default:
          Serial.println("ERR Unhandle armInfo");
          break;
      }
      break;
    case TWI_CMD_DROPOFF_START:
      //todo implement
//      //Serial.println("TWI_CMD_DROPOFF_START");
//      //set nextstate to dropoff in FSM
//      rxCurrent = (TwiCmd) cmd;
//      //rxDropoffStatus = DROPOFF_RUNNING;
//      txBuff[0] = cmd;
//      txBuff[1] = rxDropoffStatus;
      break;
    case TWI_CMD_PICKUP_START:
      Serial.println("TWI_CMD_PICKUP_START");
      //start pickup in FSM, mainloop
      rxNext = (TwiCmd) cmd;
      //txPickupStatus = PICKUP_RUNNING;
      rxPickupStatus = PICKUP_RUNNING;
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = txPickupStatus;
      break;

    case TWI_CMD_PICKUP_STATUS:
      Serial.println("TWI_CMD_PICKUP_STATUS");
      if (recivedData[1] == PICKUP_DONE_DRIVE) {
        rxPickupStatus = (PickupStatus) recivedData[1];
      }
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = txPickupStatus;
      break;

    case TWI_CMD_DROPOFF_STATUS:
      //todo implement
//      //Serial.println("TWI_CMD_DROPOFF_STATUS");
//      txBuff[0] = cmd;
//      txBuff[1] = rxDropoffStatus;

      break;

    case TWI_CMD_ERROR:
      Serial.println("TWI_CMD_ERROR");
      break;
    default:
      printf("error\n");
      break;
  }
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  //todo del
  // Serial.println("sendREQ");
  //Serial.println("sending");
  //Serial.println(txBuff[0],HEX);
  //Serial.println(txBuff[1]);
  //Serial.println(txBuff[2]);
//todo del

  Wire.write(txBuff, 3);

  //reset states
  if (txBuff[0] == PICKUP_DONE) {
    Serial.println("Sent to master = PICKUP_DONE");
    txPickupStatus = PICKUP_IDLE;
    rxDropoffStatus = DROPOFF_IDLE;
    rxCurrent = IDLE;
  }
  //reset states
  if (txBuff[0] == DROPOFF_DONE) {
    Serial.println("Sent to master = DROPOFF_DONE");
    txDropoffStatus = DROPOFF_IDLE;
    rxPickupStatus = PICKUP_IDLE;
    rxCurrent = IDLE;
  }

  txBuff[0] = 0;
  txBuff[1] = 0;
  txBuff[2] = 0;


//end of request
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
//  Serial.println("Handling new cmd Req");
   Serial.print("how many: ");
   Serial.println(howMany);

  if (3 <= howMany) {
    int i = 0;
    while (Wire.available()) {
      recivedData[i] = (uint8_t) Wire.read();
      i++;
    }

    handleReadCmd();
  }

  //if its a command from master.
  if (howMany <= 1) {
    Wire.read();    // receive byte as an integer
    //Serial.println("on or less");

  }
//end of receive
}

//todo dummy
int movedArmDown;
int movedArmUp;


void setup() {
  Wire.begin(2);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  run = 1;

  theArm.boxDistance = 30;//cm to goalbox
  theArm.boxAngle = 20;//infront angle
  theArm.objectDistance = 40;//distance from obj
  theArm.objectAngle = 90;//from behind
  theArm.collectAll = 1;//collect all

  //init state for data to send
  txPickupStatus = PICKUP_IDLE;
  txDropoffStatus = DROPOFF_IDLE;
  //init state for data to recive
  rxPickupStatus = PICKUP_IDLE;
  rxDropoffStatus = DROPOFF_IDLE;

  //todo for test remove
  movedArmDown = 0;
  movedArmUp = 0;
  //todo remove end

  //cmds that got in -> state for arm
Serial.println("SETUP DONE");
}


void loop() {
  //check if we are done
  //since we are already in a loop

  if (run) {
    //handle pickup request
    switch (rxCurrent) {

      case TWI_CMD_ARM_INIT:
        //do nottin
        break;
      case TWI_CMD_PICKUP_START:
        Serial.println("TWI_CMD_PICKUP_START");
        txPickupStatus = PICKUP_RUNNING;
        movedArmDown++;
        Serial.println("txPickupStatus= PICKUP_RUNNING");
        break;

      case TWI_CMD_PICKUP_STATUS:
        Serial.println("TWI_CMD_PICKUP_STATUS");
        movedArmDown++;
        if (movedArmDown >= 100) {
          //arm is donw move frwd
          txPickupStatus = PICKUP_FORWARD;
          Serial.println("txPickupStatus= PICKUP_FORWARD");
        }

        //wait for done moving
        if (rxPickupStatus == PICKUP_DONE_DRIVE) {
          Serial.println("rxPickupStatus= PICKUP_DONE_DRIVE");
          movedArmUp++;
          txPickupStatus = PICKUP_RUNNING;
          Serial.println("txPickupStatus= PICKUP_RUNNING");
        }

        if (movedArmUp >= 100) {
          //we are done
          txPickupStatus = PICKUP_DONE;
          Serial.println("txPickupStatus= PICKUP_DONE");
          //twi resets status to idle in
          //requestEvent()
        }
        break;

      case TWI_CMD_DROPOFF_START:

        break;

      case TWI_CMD_DROPOFF_STATUS:
        break;

      case TWI_CMD_ERROR:
        break;
      case IDLE:
        //Serial.println("IDLE");
        //delay(300);
        break;
    }

    delay(100);
    rxCurrent = rxNext;
  }


}

