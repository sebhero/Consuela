



void print_menu() {
  Serial.println("1. arm_setup");
  Serial.println("2. arm_raise");
  Serial.println("3. arm_lower");
  Serial.println("4. arm_sweep ON");
  Serial.println("5. arm_sweep OFF");
  Serial.println("6. arm_release");
}

void setup() {
  Serial.begin(9600);  
  Serial.println("Setup ok");
  //print_menu();
  yoga();
}

void yoga() {
  arm_setup();
  delay(2000);
  arm_lower();
  delay(2000);
  arm_sweep(HIGH);
  delay(2000);
  arm_sweep(LOW);
  delay(2000);
  arm_raise();
  delay(2000);
  arm_release();
  delay(2000);
  
  
}


void loop() {

  delay(5000);
  /*
  if(Serial.available() > 0) {
    int rd = Serial.read();
    if(rd != -1) {      
      char ch = (char) rd;
      Serial.print("Got :");
      Serial.println(ch);
      switch(ch) {
        case '1':
          Serial.println("Setting up arm");
          arm_setup();
          break;          
        case '2':
          arm_raise();
          break;
        case '3':
          arm_lower();
          break;
        case '4':
          arm_sweep(1);
          break;     
        case '5':
          arm_sweep(0);
          break;
        case '6':
          arm_release();
          break;                                 
        default:
        break;
      }
    }
  }
  */
}
