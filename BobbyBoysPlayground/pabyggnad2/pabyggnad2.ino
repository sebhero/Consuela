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

#define start_sw        2
#define run_sw          6
#define done_sw         7

typedef enum {
  stopped = 0,
  start = 1,
  raising = 2,
  lowering = 3,
  up = 4,
  down = 5,
  sweeping = 6,
  done = 7,
  dropping = 8,
  ending = 9
} state_t;

volatile uint8_t flag = 0;
volatile state_t state = stopped;

void start_sw_handler() {
  state = start;
}



void setup() {
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

  pinMode(solenoid, OUTPUT);
  digitalWrite(solenoid, LOW);
  
  pinMode(start_sw, INPUT_PULLUP);  
  //attachInterrupt(digitalPinToInterrupt(start_sw), start_sw_handler, FALLING); 
  
  pinMode(run_sw, INPUT_PULLUP);
  
  pinMode(done_sw, INPUT_PULLUP);
  
  Serial.begin(115200);
  Serial.println("ok!");
}

void loop() {
  switch(state) {
    case stopped:
      if(digitalRead(start_sw) == LOW) {
        Serial.println("Starting...");
        state = start;
      }    
      break;
    case start:
      Serial.println("Raising...");
      delay(300);
      digitalWrite(dir_A, dir_up);
      digitalWrite(break_A, break_off);
      analogWrite(PWM_A, 255);
      state = raising;
      break;
    case raising:
      if(digitalRead(high_limit_sw) == LOW) {
        digitalWrite(break_A, break_on);
        analogWrite(PWM_A, 0);
        Serial.println("Up! Waiting for run...");
        delay(300);
        state = up;
      }
      break;
    case up:
      if(digitalRead(run_sw) == LOW) {
        Serial.println("Lowering...");
        delay(300);
        digitalWrite(dir_A, dir_down);
        digitalWrite(break_A, break_off);
        analogWrite(PWM_A, 255);        
        state = lowering;        
      }
      break;
    case lowering:
      if(digitalRead(low_limit_sw) == LOW) {        
        digitalWrite(break_A, break_on);
        analogWrite(PWM_A, 0);        
        digitalWrite(sweep_motor, HIGH);
        Serial.println("Down! Sweeping...");
        state = sweeping;
      }
      break;
    case sweeping:
      if(digitalRead(done_sw) == LOW) {        
        digitalWrite(sweep_motor, LOW);
        Serial.println("Done sweeping");
        digitalWrite(dir_A, dir_up);
        digitalWrite(break_A, break_off);
        analogWrite(PWM_A, 255);        
        state = done;
      }
      break;
    case done:
      if(digitalRead(high_limit_sw) == LOW) {
        digitalWrite(break_A, break_on);
        analogWrite(PWM_A, 0);
        Serial.println("Up! Dropping off...");
        delay(300);
        state = dropping;
      }
      break;
    case dropping:
      digitalWrite(solenoid, HIGH);
      delay(100);
      digitalWrite(solenoid, LOW);
      Serial.println("Ending...");
      state = ending;
      break;
    case ending:
      break;
    default:
      break;
  }
  


}

