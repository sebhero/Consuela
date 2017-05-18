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

#define sweep_motor     A2
#define solenoid        A3

//#define start_sw        2
//#define run_sw          6


uint8_t sweep_motor_state = LOW;

//void start_sw_handler() {
//  state = start;
//}

// Raise arm into up position
void arm_raise() {
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
  Serial.println("Arm: Up");
  // }
}

uint8_t arm_lower() {
  // if(digitalRead(low_limit_sw) == LOW) {
  digitalWrite(dir_A, dir_down);
  digitalWrite(break_A, break_off);
  analogWrite(PWM_A, 255);
  while(digitalRead(low_limit_sw) == HIGH) {
    // Arm not down yet, spin...
  }
  digitalWrite(break_A, break_on);
  analogWrite(PWM_A, 0);
  Serial.println("Arm: Down");
  return 1;
  //}
}

uint8_t arm_sweep(uint8_t state) {
  // if(state != sweep_motor_state) {
  if(state == 1) {
    digitalWrite(sweep_motor, HIGH);
    sweep_motor_state = HIGH;
    Serial.println("Arm: Sweepers are on");
    return 1;
  } else {
    digitalWrite(sweep_motor, LOW);
    sweep_motor_state = LOW;
    Serial.println("Arm: Sweepers are off");
    return 0;
  }
  //}
}

void arm_release() {
  digitalWrite(solenoid, HIGH);
  delay(100);
  digitalWrite(solenoid, LOW);
  Serial.println("Arm: Released objects");
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

  pinMode(sweep_motor, OUTPUT);
  digitalWrite(sweep_motor, LOW);
  sweep_motor_state = LOW;

  pinMode(solenoid, OUTPUT);
  digitalWrite(solenoid, LOW);

//  pinMode(start_sw, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(start_sw), start_sw_handler, FALLING);

  //pinMode(run_sw, INPUT_PULLUP);



  arm_raise();

  Serial.println("Arm setup ok!");
}

//
// Created by trevl on 2017-05-18.
//


