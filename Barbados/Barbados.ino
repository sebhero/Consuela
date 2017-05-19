//MOTOR
#define DIR_A 12
#define DIR_B 13
#define BRAKE 9
#define PWM 3
#define UP LOW
#define DOWN HIGH
#define ON HIGH
#define OFF LOW
#define MOVING 0
#define DONE 1
int motortick;
//SERVO
#define SERVO 6
#define PICK_UP 2520
#define DROPPING 1200
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
//MODES
#define MODE_SERVO_READY 0
#define MODE_NOT_SERVO_READY 1
#define SUCKING 2
#define NOT_SUCKING 3

void setup() {
  Serial.begin(9600);
  pinMode(BRAKE, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(SERVO, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  state = WAIT;
  nextstate = WAIT;
  mode = MODE_SERVO_READY;
  tick = 0;
  motortick = 0;
  suctiontick = 0;
  servotick = 0;
  buttonpressed = 0;

  suckstatus = NOT_SUCKING;
  sticky = STICKY_1;
  digitalWrite(2, 0);
  middle = digitalRead(4);
}

void resetArm()
{
  
  Serial.println(middle);
  if (!middle)
  {
    buttonpressed = 1;
  }

  if (!buttonpressed)
  {
    digitalWrite(BRAKE, OFF);
    digitalWrite(DIR_A, UP);
    analogWrite(PWM, 255);
  }

  else
  {
    buttonpressed = 0;
    digitalWrite(BRAKE, ON);
    analogWrite(PWM, 0);
    armstatus = DONE;
  }
}
int moveArm(int armtime)
{

  if (motortick < armtime)
  {
    armstatus = MOVING;
    digitalWrite(BRAKE, OFF);
    digitalWrite(DIR_A, DOWN);
    analogWrite(PWM, 255);
    motortick++;

  }
  else
  {
    digitalWrite(BRAKE, ON);
    analogWrite(PWM, 0);
    armstatus = DONE;
    motortick = 0;
  }
}
void shakeIt()
{
  if (motortick <= 10)
  {
    digitalWrite(BRAKE, OFF);
    digitalWrite(DIR_A, DOWN);
    analogWrite(PWM, 255);
    motortick++;
  }

  else if (motortick > 10 && motortick <= 20 && middle)
  {
    digitalWrite(BRAKE, OFF);
    digitalWrite(DIR_A, UP);
    analogWrite(PWM, 255);
    motortick++;
  }
  else
  {
    digitalWrite(BRAKE, ON);
    analogWrite(PWM, 0);
    motortick = 0;

  }
}
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
int servoMove(int dir)
{
  digitalWrite(SERVO, HIGH);
  delayMicroseconds(dir);
  digitalWrite(SERVO, LOW);
  servotick++;
}
void changeSticky()
{
  Serial.println(sticky);
  if (sticky == STICKY_1)
  {
    sticky = STICKY_2;
  }
  else if (sticky == STICKY_2)
  {
    sticky = STICKY_3;
  }
  else
  {
    sticky = STICKY_1;
  }
}
void loop() {
  digitalWrite(4, HIGH);


  middle = digitalRead(4);


  switch (state) {
    case WAIT:
      nextstate = RESET;
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
        nextstate = GET_STICKY;
        armstatus = MOVING;
        servotick = 0;
        break;
      }
      break;

    case GET_STICKY:
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
          moveArm(250);
          break;
        }
        else if (armstatus == DONE && suctiontick < 150) {
          suckIt(SUCKING);
          suctiontick++;
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
      if (armstatus == MOVING)
      {
        resetArm();
      }
      if (armstatus == DONE)
      {
        servoMove(PICK_UP);
      }
      if (servotick > 150)
      {
        moveArm(280);
        servoMove(PICK_UP);
        if (armstatus == DONE)
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
        resetArm();
      }
      else if (armstatus == DONE)
      {
        servoMove(DROPPING);
      }
      if (servotick > 100)
      {
        suckIt(NOT_SUCKING);
        shakeIt();
        if (tick > 560) {
          digitalWrite(BRAKE, ON);
          analogWrite(PWM, 0);
          changeSticky();
          nextstate = RESET;
          armstatus = MOVING;
          motortick = 0;
          suctiontick = 0;
          servotick = 0;
          tick = 0;
          break;
        }
        tick++;
      } break;

  } state = nextstate;
  delay(10);
}




