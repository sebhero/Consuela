//
// Created by Sebastian Boreback on 2017-05-12.
//

#include <Wire.h>

typedef struct ArmInfo {
  //distances are in cm
  uint8_t boxDistance;
  //angles in degrees
  uint8_t boxAngle;
  uint8_t objectDistance;
  uint8_t objectAngle;
  //Boolean. true that arm can collect all obj. then dropoff
  //false if need to dropoff each obj. after pickup
  uint8_t collectAll;

} arminfo_t;

//TWI state
typedef enum TWI_CMD {

  TWI_CMD_ARM_INIT = 0x20,
  TWI_CMD_DROPOFF_START = 0x21,
  TWI_CMD_PICKUP_START = 0x22,
  TWI_CMD_DROPOFF_STATUS = 0x23,
  TWI_CMD_PICKUP_STATUS = 0x24,
  TWI_CMD_ERROR = 0x25,
} twi_cmd;

typedef enum TWI_CMD_INIT_REQ {
  TWI_CMD_ARM_REQ_BOX_INFO = 2,
  TWI_CMD_ARM_REQ_OBJ_INFO = 3,
  TWI_CMD_ARM_REQ_COLLECT_INFO = 4
} twi_cmd_init_req;

typedef enum PICKUPSTATUS {
  PICKUP_DONE = 2,
  PICKUP_FORWARD = 3,
  PICKUP_BACKWARD = 4,
  PICKUP_RUNNING = 5,
  PICKUP_FAILED = 6,
  PICKUP_DONE_DRIVE = 7,
  PICKUP_IDLE = 8
} PICKUP_STATUS;

typedef enum DROPOFFSTATUS {
  DROPOFF_DONE = 2,
  DROPOFF_RUNNING = 3,
  DROPOFF_FAILED = 4,
  DROPOFF_IDLE = 5
} DROPOFF_STATUS;

typedef enum object {
  SOCK = 2,
  SQUARE = 3,
  GLASS = 4
} OBJECT;

typedef enum FSMSTATES {
  ARM_IDLE = 2,
  ARM_PICKUP = 3,
  ARM_DROPOFF = 4
} STATE;

STATE currentState = ARM_IDLE;
STATE nextState;

/***********************/
//variables
uint8_t recivedData[3] = {0};
uint8_t txBuff[3] = {0};
arminfo_t theArm;
PICKUP_STATUS pickupStatus;
DROPOFF_STATUS dropoffStatus;
uint8_t run;
uint8_t doneBack;
uint8_t doneFrwd;


void handleReadCmd() {

  //data to send
  //uint8_t data[3] = {};
//Serial.println("handleReadCmd");

  uint8_t cmd = recivedData[0];

  switch (cmd) {
    case TWI_CMD_ARM_INIT:
      //Serial.println("TWI_CMD_ARM_INIT");
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
      //Serial.println("TWI_CMD_DROPOFF_START");
      //set nextstate to dropoff in FSM
      nextState = ARM_DROPOFF;
      dropoffStatus = DROPOFF_RUNNING;
      txBuff[0] = cmd;
      txBuff[1] = dropoffStatus;
      break;
    case TWI_CMD_PICKUP_START:
      //start pickup in FSM, mainloop
      nextState = ARM_PICKUP;
      pickupStatus = PICKUP_RUNNING;
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = pickupStatus;
      break;
    case TWI_CMD_DROPOFF_STATUS:
      //Serial.println("TWI_CMD_DROPOFF_STATUS");
      txBuff[0] = cmd;
      txBuff[1] = dropoffStatus;
      break;
    case TWI_CMD_PICKUP_STATUS:
      //Serial.println("TWI_CMD_PICKUP_STATUS");
      if (recivedData[1] == PICKUP_DONE_DRIVE) {

        //wanted to drive back
        if (pickupStatus == PICKUP_BACKWARD) {
          doneBack = 1;
        }
        //wanted to drive forward
        if (pickupStatus == PICKUP_FORWARD) {
          doneFrwd = 1;
        }
        pickupStatus= (PICKUP_STATUS)recivedData[1];
      }
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = pickupStatus;
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
  txBuff[0] = 0;
  txBuff[1] = 0;
  txBuff[2] = 0;


//end of request
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
//  Serial.println("Handling new cmd Req");
  // Serial.print("how many: ");
  // Serial.println(howMany);

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

  //todo for test remove
  uint8_t doneBack = 0;
  uint8_t doneFrwd = 0;
  //todo remove end
}

void loop() {
  //check if we are done
  //since we are already in a loop

  if (run) {
    // Serial.println("IS running");
    switch (currentState) {

      //WHEN PLATTFORM IS MOVING THIS CODE IS RUNNING
      case ARM_IDLE:
        Serial.println("is idle");
        delay(300);
        //nextState = ARM_IDLE;
        break;
        //PUT CODE HERE FOR PICKING UP OBJECTS
      case ARM_PICKUP:
        Serial.println("PICKUP");

        //for TWI communicatiion to plattform
        // set pickupStatus
        // this will tell plattform about status of arm.

        //when doing pickup
        pickupStatus = PICKUP_RUNNING;

        //if need to move backwards
        pickupStatus = PICKUP_BACKWARD;

        //if need to move forward
        pickupStatus = PICKUP_FORWARD;

        //when plattform is done driving
        //we recive PICKUP_DONE_DRIVE
        //then
        if(pickupStatus ==PICKUP_DONE_DRIVE)
        {
          //then these are set to 1
          //to indicate movement.
//          uint8_t doneBack = 1;
//          uint8_t doneFrwd = 1;
        }

        //if faild to do pickup
        pickupStatus = PICKUP_FAILED;

        //When done with pickup
        //these to need to be set
        pickupStatus = PICKUP_DONE;
        nextState = ARM_IDLE;


        break;

        //PUT CODE HERE FOR DROPOFF OBJECT
      case ARM_DROPOFF:
        Serial.println("DROPOFF");
        //for TWI communicatiion to plattform
        // set dropoffStatus
        // this will tell plattform about status of arm.

        //when start to drop off need to set:
        dropoffStatus=DROPOFF_RUNNING;

        //if problem with dropoff
        //reset arm
        //and set
        dropoffStatus = DROPOFF_FAILED;

        //when done with dropoff
        dropoffStatus = DROPOFF_DONE;
        //and
        nextState = ARM_IDLE;

        break;
    }

    delay(100);
    currentState = nextState;
  }


}

