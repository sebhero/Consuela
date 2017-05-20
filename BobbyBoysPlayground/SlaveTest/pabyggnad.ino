/*
Function    Ch.A  Ch.B
Direction   D12   D13
PWM         D3    D11
Brake       D9    D8
Curr.sns    A0    A1

Puls w.     D10
Int. high   D7
Int. low    D6
*/


#define break_A         9
#define break_B         8
#define PWM_A           3
#define PWM_B           11
#define dir_A           12
#define dir_B           13
#define sens_A          A0
#define sens_B          A1

#define dir_down        1
#define dir_up          0
#define break_off       0
#define break_on        1

#define high_limit_sw   4
#define low_limit_sw    5

#define sweepers        A2
#define solenoid        A3

//#define start_sw        2
//#define run_sw          6


uint8_t state_sweepers = 0;
uint8_t state_arm_position = LOW;
uint8_t state_release = 0;

//void start_sw_handler() {
//  state = start;
//}


int get_state_arm_position() {
  return state_arm_position;
}
int get_state_release() {
  return state_release;
}

// Raise arm into up position
void arm_raise() {
  /*
  if(state_arm_position != HIGH) {
    Serial.println("Arm: Raising");
    state_arm_position = HIGH;
    Serial.println("Arm: Up"); 
  } 
  */ 
  if(state_arm_position != HIGH) {
  //if(digitalRead(high_limit_sw) == LOW) {
    digitalWrite(dir_A, dir_up);
    digitalWrite(break_A, break_off);
    analogWrite(PWM_A, 255);  
    while(digitalRead(high_limit_sw) == HIGH) {
      // Arm not up yet, spin...
    }
    // Arm is up
    digitalWrite(break_A, break_on);
    analogWrite(PWM_A, 0);  
    state_arm_position = HIGH;
    Serial.println("Arm: Up");   
 // }
  }
}

void arm_lower() {
  /*
  if(state_arm_position != LOW) {
    Serial.println("Arm: Lowering");
    state_arm_position = LOW;
    Serial.println("Arm: Down"); 
  }
  */
  if(state_arm_position != LOW) {
 // if(digitalRead(low_limit_sw) == LOW) {
    digitalWrite(dir_A, dir_down);
    digitalWrite(break_A, break_off);
    analogWrite(PWM_A, 255);   
    while(digitalRead(low_limit_sw) == HIGH) {
      // Arm not down yet, spin...
    }        
    digitalWrite(break_A, break_on);
    analogWrite(PWM_A, 0);
    state_arm_position = LOW;
    Serial.println("Arm: Down");
  //} 
  }
}

void arm_sweep(uint8_t state) {
  /*
  if(state != state_sweepers) {
    if(state = 1) {
      state_sweepers = 1;
      Serial.println("Arm: Sweepers are on");      
    } else {
      state_sweepers = 0;
      Serial.println("Arm: Sweepers are off");    
    }    
  }
  */
  
   if(state != state_sweepers) { 
 // if(state != sweep_motor_state) {   
    if(state == 1) {
      //digitalWrite(dir_B, dir_down);
      //digitalWrite(break_B, break_off);
      //analogWrite(PWM_B, 155);  
      digitalWrite(sweepers, HIGH);
      state_sweepers = HIGH;
      Serial.println("Arm: Sweepers are on");
    } else {
      //digitalWrite(break_B, break_on);
      //analogWrite(PWM_B, 0);
      digitalWrite(sweepers, LOW);
      state_sweepers = LOW;
      Serial.println("Arm: Sweepers are off");   
    }    
  //}
 }
}

void arm_release() {
  /*
  if(state_release == 0) {  
    state_release = 1;
    Serial.println("Arm: Released objects");  
  }
  */
  if(state_release == 0) { 
    digitalWrite(solenoid, HIGH);
    delay(100);
    digitalWrite(solenoid, LOW);
    delay(100);
    digitalWrite(solenoid, HIGH);
    delay(100);
    digitalWrite(solenoid, LOW);
    delay(100);
    digitalWrite(solenoid, HIGH);
    delay(100);
    digitalWrite(solenoid, LOW);    
    state_release = 1;
    Serial.println("Arm: Released objects");  
  }
}



void arm_setup() {
  pinMode(break_A, OUTPUT);
  pinMode(break_B, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(dir_A, OUTPUT);
  pinMode(dir_B, OUTPUT);
  
  digitalWrite(break_A, break_on);
  digitalWrite(break_B, break_on);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);

  pinMode(high_limit_sw, INPUT_PULLUP);
  
  pinMode(low_limit_sw, INPUT_PULLUP);

  pinMode(solenoid, OUTPUT);
  digitalWrite(solenoid, LOW);

  pinMode(sweepers, OUTPUT);
  digitalWrite(sweepers, LOW);
  
  state_sweepers = 0;
  state_release = 0;
  
  //pinMode(start_sw, INPUT_PULLUP);  
  //attachInterrupt(digitalPinToInterrupt(start_sw), start_sw_handler, FALLING); 
  
  //pinMode(run_sw, INPUT_PULLUP);  

  arm_raise();
  
  Serial.println("Arm setup ok!");
}


