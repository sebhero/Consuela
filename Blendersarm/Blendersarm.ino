//
// Created by Sebastian Boreback on 2017-05-12.
// Author Viktor Malmgren, Robin Jonsson, Viktor Holmelin, Sebastian Börebäck.

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


//todo dummy
int movedArmDown;
int movedArmUp;

//int breakPin = 9;
int directionPin = 12;
int pwmPin = 3;
int breakTop = 4;
int breakBot = 7;
int onoffPin = 2;

void handleReadCmd() {

  //data to send
  //uint8_t data[3] = {};
  //Serial.println("handleReadCmd");

  uint8_t cmd = recivedData[0];
  Serial.print("cmd = ");
  Serial.println(cmd);


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
      Serial.println("TWI_CMD_DROPOFF_START");
      //      //set nextstate to dropoff in FSM
      //      rxCurrent = (TwiCmd) cmd;
      txDropoffStatus = DROPOFF_RUNNING;
      Serial.println("DROPOFF RUNNING");
      txBuff[0] = cmd;
      txBuff[1] = txDropoffStatus;
      rxNext = (TwiCmd) cmd;
      break;
    case TWI_CMD_PICKUP_START:
      //Serial.println("TWI_CMD_PICKUP_START");
      //start pickup in FSM, mainloop
      rxNext = (TwiCmd) cmd;
      //txPickupStatus = PICKUP_RUNNING;
      rxPickupStatus = PICKUP_RUNNING;
      txBuff[0] = TWI_CMD_PICKUP_STATUS;
      txBuff[1] = txPickupStatus;

      break;

    case TWI_CMD_PICKUP_STATUS:
      //Serial.println("TWI_CMD_PICKUP_STATUS");
      rxNext = (TwiCmd) cmd;
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
      Serial.println("TWI_CMD_DROPOFF_STATUS");
      rxNext = (TwiCmd) cmd;
      txBuff[0] = cmd;
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
  if (txBuff[1] == PICKUP_DONE) {
    Serial.println("Sent to master = PICKUP_DONE");
    txPickupStatus = PICKUP_IDLE;
    rxDropoffStatus = DROPOFF_IDLE;
    rxNext = IDLE;

    //todo remove
    movedArmDown = 0;
    movedArmUp = 0;
  }
  //reset states
  if (txBuff[1] == DROPOFF_DONE) {
    Serial.println("Sent to master = DROPOFF_DONE");
    txDropoffStatus = DROPOFF_IDLE;
    txPickupStatus = PICKUP_IDLE;
    rxNext = IDLE;
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


void setup() {
  Wire.begin(2);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  run = 1;


  //Olika pinnar vi använder, initieras här
  pinMode(directionPin, OUTPUT);
  pinMode(breakBot, INPUT_PULLUP);
  pinMode(breakTop, INPUT_PULLUP);
  pinMode(onoffPin, OUTPUT);

  theArm.boxDistance = 30;//cm to goalbox
  theArm.boxAngle = 20;//infront angle
  theArm.objectDistance = 40;//distance from obj
  theArm.objectAngle = 90;//from behind
  theArm.collectAll = 0;//collect all

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


void startMotor() { //Startar dammsugarens insug
  digitalWrite(onoffPin, HIGH);
  Serial.println("START SUG");
}
/*
   Stops the vacuum motor
*/
void stopMotor() { //Stannar dammsugarens insug
  digitalWrite(onoffPin, LOW);
  Serial.println("STOPP SUG");
}

void down() {
  while (digitalRead(breakBot)) { //När den nedre mikrobrytaren inte är nedtryckt körs loopen
    digitalWrite(directionPin, HIGH);
    analogWrite(pwmPin, 75); 
    Serial.println("NED");
  }
  stopMovement();
}

void up() { //Lyfter armen
  while (digitalRead(breakTop)) { //När den övre mikrobrytaren inte är nedtryckt körs loopen
    Serial.println("UPP");
    digitalWrite(directionPin, LOW); //motorn snurrar åt vänster, alltså upp
    analogWrite(pwmPin, 130); //hastigheten till motorn
  }
  stopMovement();
}
/*
   Stops the movement of the arm
*/
void stopMovement() { //Stannar motorn
  analogWrite(pwmPin, 0); //0 i hastighet till motorn
  Serial.println("STOP MOVEMENT");
}

void downDrop() { //Metod för att snabbt sänka armen så att objektet inte sitter kvar
  digitalWrite(directionPin, HIGH);
  analogWrite(pwmPin, 255);
  stopMotor();
  Serial.println("DOWNDROP");
  delay(75);
  stopMovement();
}

void upGlass() {
  digitalWrite(directionPin, LOW);
  analogWrite(pwmPin, 130);
  delay(150);
  stopMovement();
}

void loop() {
  //check if we are done
  //since we are already in a loop

  if (run) {
    //handle pickup request
    switch (rxCurrent) {

      case TWI_CMD_ARM_INIT: //Initiera armen, skicka specifikation för armen
        //do nottin
        break;
      case TWI_CMD_PICKUP_START: //Tillstånd för att börja plocka upp

        down(); //Sänker armen till den slår i en mikrobrytare
        startMotor(); //Startar insuget
        delay(200); //delay så att den hinner suga in objektet

       /*
        if(txbuff[2] == GLASS){
          upGlass();
        }
        
       */

        txPickupStatus = PICKUP_FORWARD; //Tillstånd som inte gör något men säger till roboten att köra fram en bit
        rxNext = IDLE; //Nästa tillstånd är idle
        txBuff[1] = txPickupStatus; //Uppdaterar tillstånd
        break;

      case TWI_CMD_PICKUP_STATUS:
        //Serial.println("TWI_CMD_PICKUP_STATUS");
        if (PICKUP_DONE_DRIVE ==  rxPickupStatus) {
          up(); //Efter att vi har kört klart den sträcka för att göra det lättare att plocka upp objektet så höjer vi armen
          txPickupStatus = PICKUP_DONE; //Säger att vi har plockat upp objektet
          txBuff[1] = txPickupStatus; //Uppdaterar tillstånd
        }

      
        /*
          txPickupStatus = PICKUP_RUNNING;
          Serial.println("PICKUP_RUNNING");
          txBuff[1] = txPickupStatus;


          down();
          startMotor();
          delay(200);
          up();
          delay(10);
        */

        break;

      /*
        Serial.println("PICKUP_FORWARD");
        txPickupStatus = PICKUP_FORWARD;
        down();
        startMotor();
        txBuff[1] = txPickupStatus;
        break;
      */


      case TWI_CMD_DROPOFF_START: //Tillstånd för att släppa objektet
        Serial.println("SLÄPP OBJEKT");

        downDrop(); //Sänker armen snabbt
        stopMotor(); //Stannar insuget
        stopMovement(); //Stannar motorn
        up(); //Höjer armen igen
        
        txDropoffStatus = DROPOFF_DONE; //Säger att vi är klara med dro
        Serial.println("DROPOFF DONE");
        rxNext = IDLE; //Går till idle-tillståndet
        txBuff[1] = txDropoffStatus; //Uppdaterar vårt tillstånd
        break;

      case TWI_CMD_DROPOFF_STATUS:
        //txDropoffStatus = DROPOFF_DONE;
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
  }


}

