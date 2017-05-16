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


#define break_A 9
#define break_B 8
#define PWM_A 3
#define PWM_B 11
#define dir_A 12
#define dir_B 13

void setup() {
  pinMode(break_A, OUTPUT);
  pinMode(break_B, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(dir_A, OUTPUT);
  pinMode(dir_B, OUTPUT);

  digitalWrite(break_A, LOW);
  digitalWrite(break_B, LOW);
}





  Serial.begin(115200);
  /*
  pinMode(pb1, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(pb1), pb1_handler, FALLING);
  pinMode(pb2, INPUT_PULLUP);  
  attachInterrupt(digitalPinToInterrupt(pb2), pb2_handler, FALLING);

  pinMode(width_pin, OUTPUT);
  servo_stop();
  */


#define width_pin 5
#define width_stop 110
#define width_left 80
#define width_right 180

#define pb1 2
#define pb2 3

typedef enum {
  stopped = 0,
  left = 1,
  right = 2,
  max_left = 3,
  max_right = 4
} state_t;

volatile state_t servo_state = stopped;

void servo_stop() {
  analogWrite(width_pin, width_stop);
  servo_state = stopped;
}

void servo_right() {
  analogWrite(width_pin, width_right);
  servo_state = right;
}

void servo_left() {
  analogWrite(width_pin, width_left);
  servo_state = left;
}

void pb1_handler() {
  servo_stop();
  servo_state = max_left;
}

void pb2_handler() {
  servo_stop();
  servo_state = max_right;  
}




void loop() {
  // 1
  digitalWrite(dir_A, LOW);
  digitalWrite(PWM_A, HIGH);
  delay(2);
  // 2
  digitalWrite(dir_B, HIGH);
  digitalWrite(PWM_B, HIGH);
  delay(2);
  // 3
  digitalWrite(dir_A, HIGH);
  digitalWrite(PWM_A, HIGH);
  delay(2);
  // 4
  digitalWrite(dir_B, LOW);
  digitalWrite(PWM_B, HIGH);
  delay(2);

  

  

  /*
  switch(servo_state) {
    case stopped:
      Serial.println("Servo stopped, starting...");
      servo_left();
      break;
    case max_left:
      Serial.println("Servo max left, going right...");      
      servo_right();
      break;
    case max_right:
      Serial.println("Servo max right, going left...");      
      servo_left();
      break;
    default:
      break;
      }
      */
  
  
  
}
