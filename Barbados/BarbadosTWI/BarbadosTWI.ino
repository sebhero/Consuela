
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
uint8_t theOjb;

//todo dummy
int movedArmDown;
int movedArmUp;

void handleReadCmd() {

  //data to send
  //uint8_t data[3] = {};
  //Serial.println("handleReadCmd");

  uint8_t cmd = recivedData[0];
  Serial.print("cmd = ");
  Serial.println(cmd,HEX);


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
       rxNext = TWI_CMD_DROPOFF_START;
      //txPickupStatus = PICKUP_RUNNING;
      txDropoffStatus= DROPOFF_RUNNING;
      txBuff[0] = TWI_CMD_DROPOFF_STATUS;
      txBuff[1] = txDropoffStatus;
      break;
    case TWI_CMD_PICKUP_START:

      theOjb = recivedData[2];
      //Serial.println("TWI_CMD_PICKUP_START");
      //start pickup in FSM, mainloop
      rxNext = TWI_CMD_PICKUP_START;
      //txPickupStatus = PICKUP_RUNNING;
      rxPickupStatus = PICKUP_RUNNING;
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = txPickupStatus;

      break;

    case TWI_CMD_PICKUP_STATUS:
      //Serial.println("TWI_CMD_PICKUP_STATUS");
      rxNext = TWI_CMD_PICKUP_STATUS;
      if (recivedData[1] == PICKUP_DONE_DRIVE) {
        rxPickupStatus = (PickupStatus) recivedData[1];
      }
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = txPickupStatus;
      Serial.print("Send status= ");
      Serial.println(txPickupStatus);
      break;

    case TWI_CMD_DROPOFF_STATUS:
      //todo implement
      //      //Serial.println("TWI_CMD_DROPOFF_STATUS");
      //      txBuff[0] = cmd;
      //      txBuff[1] = rxDropoffStatus;

      rxNext = TWI_CMD_DROPOFF_STATUS;
      txBuff[0] = TWI_CMD_DROPOFF_STATUS;
      txBuff[1] = txDropoffStatus;
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

    //todo remove
    movedArmDown = 0;
    movedArmUp = 0;
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
  //   Serial.print("how many: ");
  //   Serial.println(howMany);



  if (3 <= howMany) {
    int temp = 0;
    int i = 0;
    temp = Wire.read();
    if (temp != 0) {
      recivedData[i] = (uint8_t) temp;
      i++;
    } else {
      Serial.println("got Zero");
    }
    while (Wire.available()) {

      recivedData[i] = (uint8_t) Wire.read();
      i++;

    }

    handleReadCmd();
  }

  //if its a command from master.
  if (howMany <= 1) {
    Serial.println(Wire.read());    // receive byte as an integer
    Serial.println("on or less");


  }
  //end of receive
}
//MOTOR
#define DIR_A 12
#define DIR_B 13
#define BRAKE_A 9
#define BRAKE_B 8
#define PWM_A 3
#define PWM_B 11
#define UP LOW
#define DOWN HIGH
#define ON HIGH
#define OFF LOW
#define MOVING 0
#define DONE 1
int motortick;
int choptick;

//SERVO
#define SERVO 6
#define PICK_UP 2380
#define DROPPING 1300
#define STICKY_1 0
#define STICKY_2 1
#define STICKY_3 2
int pulse_sticky1 = 600;
int pulse_sticky2 = 750;
int pulse_sticky3 = 930;
int tick;
int suctiontick;

//STATES
#define WAIT 0
#define RESET 1
#define GET_STICKY 2
#define PICKUP 3
#define DROP 4

//integers
int state;
int armstatus;
int nextstate;
int mode;
int adjusted;
int servotick;
int sticky;
int buttonpressed;
int suckstatus;
int middle;
int stickynr;
int done;
//MODES
#define SUCKING 2
#define NOT_SUCKING 3

void resetArm()
{

  delayMicroseconds(20000);
  if (!middle)
  {
    buttonpressed = 1;
  }

  if (!buttonpressed)
  {
    digitalWrite(BRAKE_A, OFF);
    digitalWrite(DIR_A, UP);
    analogWrite(PWM_A, 255);
  }

  else
  {
    buttonpressed = 0;
    digitalWrite(BRAKE_A, ON);
    analogWrite(PWM_A, 0);
    armstatus = DONE;
  }
}
//This method controlls the height of the arm. It's usually used after resetArm() as
//it is used to lower the arm. You send the time you want the arm to move and at what direction. 
//servoMove is used in this to counter it from rorating while lowering.
int moveArm(int armtime, int stickynr)
{
  delayMicroseconds(20000);
  if (stickynr != 0) {
    if (motortick < armtime)
    {
      if (motortick < (armtime - 30)) {
        servoMove(stickynr);
      }
      armstatus = MOVING;
      digitalWrite(BRAKE_A, OFF);
      digitalWrite(DIR_A, DOWN);
      analogWrite(PWM_A, 255);
      motortick++;

    }
    else
    {
      digitalWrite(BRAKE_A, ON);
      analogWrite(PWM_A, 0);
      armstatus = DONE;
      motortick = 0;
      servotick = 0;
    }
  }
  else
  {
    if (motortick < armtime)
    {
      servoMove(stickynr);
      armstatus = MOVING;
      digitalWrite(BRAKE_A, OFF);
      digitalWrite(DIR_A, DOWN);
      analogWrite(PWM_A, 255);
      motortick++;

    }
    else
    {
      digitalWrite(BRAKE_A, ON);
      analogWrite(PWM_A, 0);
      armstatus = DONE;
      motortick = 0;
    }
  }
}
void shakeIt()
{
  delayMicroseconds(20000);
  if (motortick <= 10)
  {
    digitalWrite(BRAKE_A, OFF);
    digitalWrite(DIR_A, DOWN);
    analogWrite(PWM_A, 255);
    motortick++;
  }

  else if (motortick > 10 && motortick <= 20 && middle)
  {
    digitalWrite(BRAKE_A, OFF);
    digitalWrite(DIR_A, UP);
    analogWrite(PWM_A, 255);
    motortick++;
  }
  else
  {
    digitalWrite(BRAKE_A, ON);
    analogWrite(PWM_A, 0);
    motortick = 0;
  }
}
//This method is called when you want to start or stop the vacuumcleaner.
//You send either SUCKING or NOT_SUCKING to chose the desired outcome.
int suckIt(int suckstatus)
{
  if (suckstatus == SUCKING)
  {
    digitalWrite(2, 1);
  }
  else
  {
    digitalWrite(2, 0);
  }
}
//This method controlls the rotation of the arm.
//a pulselenght is being sent in (in microseconds) and it translates to an angle
int servoMove(int dir)
{

  delayMicroseconds(20000);
  digitalWrite(SERVO, HIGH);
  delayMicroseconds(dir);
  digitalWrite(SERVO, LOW);
  servotick++;
}
//This method is called at the end of each pickup. It changes which sticky is to be used.
//It goes from 1 - 3 and starts over at 1
void changeSticky()
{
  //Serial.println(sticky);
  if (sticky == STICKY_1)
  {
    sticky = STICKY_2;
    stickynr = pulse_sticky2;
  }
  else if (sticky == STICKY_2)
  {
    sticky = STICKY_3;
    stickynr = pulse_sticky3;
  }
  else
  {
    sticky = STICKY_1;
    stickynr = pulse_sticky1;
  }
}

void setup() {
  Wire.begin(2);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  run = 1;

  theArm.boxDistance = 28;//cm to goalbox
  theArm.boxAngle = 90;//infront angle
  theArm.objectDistance = 48;//distance from obj
  theArm.objectAngle = 90;//from behind
  theArm.collectAll = 1;//collect all

  //init state for data to send
  txPickupStatus = PICKUP_IDLE;
  txDropoffStatus = DROPOFF_IDLE;
  //init state for data to recive
  rxPickupStatus = PICKUP_IDLE;
  rxDropoffStatus = DROPOFF_IDLE;
  theOjb = 0;

  pinMode(BRAKE_A, OUTPUT);
  pinMode(BRAKE_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(SERVO, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  state = WAIT;
  nextstate = WAIT;
  tick = 0;
  motortick = 0;
  choptick = 0;
  suctiontick = 0;
  servotick = 0;
  buttonpressed = 0;
  done = 0;
  
  stickynr = pulse_sticky1;
  suckstatus = NOT_SUCKING;
  sticky = STICKY_1;
  int setupready = 0;
  digitalWrite(2, 0);
  digitalWrite(4, 1);
  middle = digitalRead(4);
  armstatus = MOVING;
  while (!setupready)
  {
    Serial.println("reset");
    middle = digitalRead(4);
    resetArm();
    if (armstatus == DONE)
    {

      servoMove(600);
    }
    if (servotick > 300)
    {
      armstatus = MOVING;
      servotick = 0;
      setupready = 1;
      break;
    }
  }
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
        Serial.print("theOjb =");
        Serial.println(theOjb);
        txPickupStatus = PICKUP_RUNNING;
        nextstate = GET_STICKY;

        break;

      case TWI_CMD_PICKUP_STATUS:
      if(!done){
        txPickupStatus = PICKUP_RUNNING;
      }
        digitalWrite(4, HIGH);

        middle = digitalRead(4);
        switch (state) {
          
          case WAIT:
            break;

          case RESET:
            Serial.println("RESET");
            if (armstatus == MOVING)
            {
              resetArm();
            }

            if (armstatus == DONE)
            {
              servoMove(600);
            }
            if (servotick > 300)
            {
              nextstate = WAIT;
              txPickupStatus = PICKUP_DONE;
              done = 1;
              txBuff[1] = txPickupStatus;
              Serial.println("done");
              armstatus = MOVING;
              servotick = 0;
              break;
            }
            break;

          case GET_STICKY:
          done = 0;
            Serial.println("GET_STICKY");
            if (servotick < 100) {

              switch (sticky) {
                case STICKY_1:
                  servoMove(pulse_sticky1);
                  break;
                case STICKY_2:
                  servoMove(pulse_sticky2);
                  break;
                case STICKY_3:
                  servoMove(pulse_sticky3);
                  break;
              }
            }
            else {
              if (armstatus == MOVING)
              {
                moveArm(340, stickynr);
                break;
              }
              else if (armstatus == DONE && suctiontick < 150) {
                suckIt(SUCKING);
                suctiontick++;
                //Serial.println("Suction");
                //Serial.println(suctiontick);
              }
              else {
                nextstate = PICKUP;
                armstatus = MOVING;
                motortick = 0;
                suctiontick = 0;
                servotick = 0;
                break;
              }
            }
            break;
          case PICKUP:
            Serial.println("PICKUP");
            if (armstatus == MOVING && servotick <= 150)
            {
              Serial.println("reseting arm");
              resetArm();
            }
            if (armstatus == DONE && servotick <= 150)
            {
              Serial.println("done reseting, moving to position");
              servoMove(PICK_UP);

            }
            if (servotick > 150)
            {
              Serial.println("picking up, ALLEGEDLY");
              Serial.println(theOjb);
              //moveArm(430, PICK_UP);
              if (theOjb == GLASS) {
                moveArm(430, PICK_UP);
                Serial.println("GLASS");
              }
              else if (theOjb == SQUARE)
              {
                moveArm(470, PICK_UP);
                Serial.println("SQUARE");
              }
              else if (theOjb == SOCK)
              {
                Serial.println("SOCK");
                moveArm(490, PICK_UP);
              }
              else if (theOjb == 0)
              {
                moveArm(470, PICK_UP);
                Serial.println("NO OBJECT CHOSEN");
              }
//              if(armstatus == MOVING)
//              {
//                moveArm(430, PICK_UP);
//              }
              Serial.print("ARMSTATUS: ");
              Serial.println(armstatus);
              Serial.println(motortick);
             if (armstatus == DONE && motortick == 0)
              {
                nextstate = DROP;
                servotick = 0;
                buttonpressed = 0;
                armstatus = MOVING;
                break;
              }
            }
            break;

          case DROP:
            Serial.println("DROP");
            if (armstatus == MOVING)
            {
              Serial.println("reseting");
              resetArm();
            }
            else if (armstatus == DONE)
            {
              Serial.println("moving to dropzone");
              servoMove(DROPPING);
            }
            if (servotick > 100)
            {
              Serial.println("shaking and stuff");
              suckIt(NOT_SUCKING);
              shakeIt();
              if (tick > 560) {
                digitalWrite(BRAKE_A, ON);
                analogWrite(PWM_A , 0);
                changeSticky();
                armstatus = MOVING;
                nextstate = RESET;
                motortick = 0;
                suctiontick = 0;
                servotick = 0;
                tick = 0;
                break;
              }
              tick++;
            } break;

        }
        break;

      case TWI_CMD_DROPOFF_START:
        txDropoffStatus = DROPOFF_RUNNING;
        break;

      case TWI_CMD_DROPOFF_STATUS:

        if(txDropoffStatus != DROPOFF_DONE)
        {
          txDropoffStatus = DROPOFF_RUNNING;
        }
        else
        {
            txDropoffStatus = DROPOFF_DONE;
        }
       
        if (choptick < 1000 && txDropoffStatus == DROPOFF_RUNNING)
        {
          Serial.println(choptick);
          delayMicroseconds(20000);
          digitalWrite(BRAKE_B, OFF);
          digitalWrite(DIR_B, 1);
          analogWrite(PWM_B, 255);
          choptick++;
        }
        else
        {
          digitalWrite(BRAKE_B, ON);
          analogWrite(PWM_B, 0);
          choptick = 0;
          txDropoffStatus = DROPOFF_DONE;
        }
        break;


      case TWI_CMD_ERROR:
        break;
      case IDLE:
        //Serial.println("IDLE");
        //delay(300);
        break;
    }

    //delay(100);
    rxCurrent = rxNext;
    state = nextstate;
  }


}

