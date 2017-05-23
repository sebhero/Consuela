#include <Wire.h>

uint8_t rx_buff[3];
byte send_data[5];
byte byteArray[8];
int8_t twi_state = 0,request_state = 0;
byte led1[5];
byte led2[5];
byte strumpa[5];
byte kub[5];
byte glas[5];

typedef struct {
   const char* name;
   byte *data;
} object_t;

object_t objects[] = {
  { 
    .name = "Led 1",
    .data = led1
  },
  {
    .name = "Led 2",
    .data = led2
  },
  {
    .name = "Strumpa",
    .data = strumpa
  },
  {
    .name = "Kub",
    .data = kub
  },
  {
    .name = "Glas",
    .data = glas
  }  
};

#define SLAVE_ADR 0x03

void setup() {
  Wire.begin(SLAVE_ADR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(recieveEvent);
  
  Serial.begin(115200);  

  led1[0] = 0x50; 
  led1[1] = 0;
  led1[2] = 200;
  led1[3] = 0;
  led1[4] = 200;

  led2[0] = 0x51;
  led2[1] = 0;
  led2[2] = 150;
  led2[3] = 0;
  led2[4] = 150;

  strumpa[0] = 0x52;
  strumpa[1] = 1;
  strumpa[2] = 44;
  strumpa[3] = 0;
  strumpa[4] = 251;

  kub[0] = 0x53;
  kub[1] = 0;
  kub[2] = 160;
  kub[3] = 0;
  kub[4] = 247;

  glas[0] = 0x54;
  glas[1] = 0;
  glas[2] = 85;
  glas[3] = 1;
  glas[4] = 52;

  Serial.println("Setup ok");
  print_menu();
}

void print_menu() {
  Serial.println("1 - set");
  Serial.println("2 - print");
}

void print_object(int id) {
  Serial.print("\nName: ");
  Serial.println((objects[id]).name);
  Serial.print("ID: 0x");
  Serial.print(objects[id].data[0], HEX);
  Serial.println("");  
  Serial.print("Coordinates: ");
  Serial.print(objects[id].data[1]);
  Serial.print(" ");
  Serial.print(objects[id].data[2]);
  Serial.print(" ");
  Serial.print(objects[id].data[3]);
  Serial.print(" ");
  Serial.print(objects[id].data[4]);
  Serial.print(" ");  
  Serial.println("");
}

int get_object_id() {
  int idx = 0;
  Serial.print("Enter objet number [0-4]: ");
  while(Serial.available() == 0) {
    // Spin
  }   
  idx = Serial.read();
  idx = idx - '0'; 
  return idx;
}

void set_object_coords(int idx) {
  Serial.println("Setting coords for:");
  Serial.print("\nName: ");
  Serial.println((objects[idx]).name);
  Serial.print("ID: 0x");
  Serial.print(objects[idx].data[0], HEX);
  Serial.println("");  
  for(int i = 1; i < 5; ++i) {
    Serial.print("coord[");
    Serial.print(i);
    Serial.print("]: ");
    while(Serial.available() == 0) {
      // Spin
    }   
    int val = (int) Serial.parseInt();
    Serial.print("\nSetting coordinate ");
    Serial.print(i);
    Serial.print(" to ");
    Serial.print(val);
    Serial.println("");
    objects[idx].data[i] = val;
  }
}

void loop() {
  delay(100);
  int idx = 0;
  if(Serial.available() > 0) {
    int rd = Serial.read();        
    switch(rd) {
      case '1':
        idx = get_object_id();        
        if(0 <= idx && idx <= 4) {          
          set_object_coords(idx);
        }
        break;
      case '2':
        idx = get_object_id();        
        if(0 <= idx && idx <= 4) {          
          print_object(idx);
        }
        break;
      default:
        Serial.println("Hoder!");            
    } 
    print_menu();
  }  
}

void requestEvent(){
  if(send_data[0] != 0){
    Wire.write(send_data, 5);
    for(int n = 0; n < 5; n++) {
      Serial.println(send_data[n]);
    }
  }
}

void recieveEvent(int TURNDOWNFORWHAT){
  while(Wire.available()){
    twi_state = Wire.read();
    rx_buff[0] = twi_state;
  }
  Serial.print("Kommando: ");
  Serial.println(rx_buff[0], HEX);
  switch(rx_buff[0]){
    case 0x00:
      send_data[0] = 0;
    break;
    case 0x50:  //efterfrågar lysdiod 1 positon
      for(int n = 0; n < 5; n++) {
        send_data[n] = led1[n];
      }
    break;
    case 0x51: //efterfrågar lysdiod 2 position
      for(int n = 0; n < 5; n++) {
        send_data[n] = led2[n];
      }
    break;
    case 0x52:  //efterfrågar strumpa position
      for(int n = 0; n < 5; n++) {
        send_data[n] = strumpa[n];
      }
    break;
    case 0x53:  //efterfrågar kubens position
      for(int n = 0; n < 5; n++) {
        send_data[n] = kub[n];
      }
    break;
    case 0x54:  //efterfrågar glasets position
      for(int n = 0; n < 5; n++) {
        send_data[n] = glas[n];
      }
    break;
    case 0x55:
      send_data[0] = 0x55;
      send_data[1] = 0;
      send_data[2] = 0;
      send_data[3] = 0;
      send_data[4] = 0;
    break;
  }
  
}

