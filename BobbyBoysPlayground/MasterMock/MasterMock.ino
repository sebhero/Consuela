#include <Wire.h>


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
  PICKUP_DONE = 2,
  PICKUP_FORWARD = 3,
  PICKUP_BACKWARD = 4,
  PICKUP_RUNNING = 5,
  PICKUP_FAILED = 6,
  PICKUP_DONE_DRIVE = 7,
  PICKUP_IDLE = 8
} PICKUP_STATUS;

typedef enum {
  //retuns distance to box goal, and angle to set robot for dropoff
  TWI_CMD_ARM_REQ_BOX_INFO = 2,
  //returns distance to object and angle for collect
  TWI_CMD_ARM_REQ_OBJ_INFO = 3,
  TWI_CMD_ARM_REQ_COLLECT_INFO = 4
} TwiCmdInitReq;


#define SLAVE_ADDRESS 2

void print_menu() {
  Serial.println("1. {TWI_CMD_PICKUP_START,0,0}"); // ok
  Serial.println("2. {TWI_CMD_PICKUP_STATUS,0,0}");
  Serial.println("3. {TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE,0}");
  Serial.println("4. {TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE,0}");
  Serial.println("5. {TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_BOX_INFO,0}"); // ok
  Serial.println("6. {TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_OBJ_INFO,0}"); // ok
  Serial.println("7. {TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_COLLECT_INFO,0}"); // ok
  Serial.println("8. {TWI_CMD_DROPOFF_START,0,0}"); // ok
  Serial.println("9. {TWI_CMD_DROPOFF_STATUS,0,0}"); // ok
}

void request_and_print_data() {
  Wire.requestFrom(SLAVE_ADDRESS, 3);
  delay(100);
  while (Wire.available()) {
    byte c = Wire.read();
    Serial.print(c);
    Serial.print(' ');
  }   
  Serial.println("");      
}

void send_data(byte b1, byte b2, byte b3) {
  Wire.beginTransmission(SLAVE_ADDRESS);         
  Wire.write(b1);
  Wire.write(b2);
  Wire.write(b3);
  Wire.endTransmission();  
}

void setup() {
  Serial.begin(115200);  
  Wire.begin();
  Serial.println("Setup ok");  
  print_menu();
}

void loop() {
  delay(100);
   if(Serial.available() > 0) {
    int rd = Serial.read();
    if(rd != -1) {      
      char ch = (char) rd;
      Serial.print("Got ");
      Serial.print(ch);
      Serial.print(": ");
      switch(ch) {
        case '1':
          Serial.println(" {TWI_CMD_PICKUP_START,0,0}"); 
          send_data(TWI_CMD_PICKUP_START, 0, 0);
          delay(100); 
          request_and_print_data();        
          break;
        case '2':
          Serial.println(" {TWI_CMD_PICKUP_STATUS,0,0}"); 
          send_data( TWI_CMD_PICKUP_STATUS, 0, 0);
          delay(100); 
          request_and_print_data();
          break; 
        case '3':
          Serial.println(" {TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE,0}"); 
          send_data( TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE, 0);
          delay(100); 
          request_and_print_data();
          break;                     
        case '4':
          Serial.println(" {TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE,0}"); 
          send_data(TWI_CMD_PICKUP_STATUS, PICKUP_DONE_DRIVE, 0);
          delay(100); 
          request_and_print_data();
          break;        
        case '5':
          Serial.println(" {TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_BOX_INFO,0}"); 
          send_data(TWI_CMD_ARM_INIT, TWI_CMD_ARM_REQ_BOX_INFO, 0);
          delay(100); 
          request_and_print_data();  
          break;     
        case '6':
          Serial.println(" {TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_OBJ_INFO,0}"); 
          send_data( TWI_CMD_ARM_INIT, TWI_CMD_ARM_REQ_OBJ_INFO, 0);
          delay(100); 
          request_and_print_data();       
          break; 
        case '7': 
          Serial.println(" {TWI_CMD_ARM_INIT,TWI_CMD_ARM_REQ_COLLECT_INFO,0}"); 
          send_data( TWI_CMD_ARM_INIT, TWI_CMD_ARM_REQ_COLLECT_INFO, 0); 
          delay(100); 
          request_and_print_data();      
          break;  
        case '8':
          Serial.println(" {TWI_CMD_DROPOFF_START,0,0}"); 
          send_data(TWI_CMD_DROPOFF_START, 0,0);
          delay(100); 
          request_and_print_data();           
          break;  
        case '9':
          Serial.println(" {TWI_CMD_DROPOFF_STATUS,0,0}"); 
          send_data(TWI_CMD_DROPOFF_STATUS, 0, 0);
          delay(100); 
          request_and_print_data();              
          break;                                                      
        default:
        break;
      }
    }
  }

}
