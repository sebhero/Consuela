/*
 * ultra_servo.h
 *
 * Created: 2017-05-10 10:04:37
 *  Author: Johnatan
 */ 


#ifndef ULTRA_SERVO_H_
#define ULTRA_SERVO_H_
#define trig PIO_PC26_IDX
#define echo PIO_PC25_IDX
#define servo PIO_PC24_IDX
#define STATUS_FOUND 1
#define STATUS_SEARCHING 0
#define STATUS_MAPPING 2
#define ANGLE_RESET 3

//"scaling" ?r sammans?ttning av tiden f?r ljudets f?rd i "width-ticks" per mikro_s (3.64) g?nger 2 (p.g.a ljudets str?cka ?r dubbbelt
//s? l?ng ?n avst?ndet som ska m?tas) och sedan delas detta med ljudets hastighet i cm / us (0.034).
// 'scaling' ?r allts? (3.64 * 2) / 0.34 = 214.
#define scaling 214;


long pulseIn(void);

void servoControll(unsigned long dist);

void servoDirection(uint8_t angle);

void testingUltraSound(void);

void testServo(void);

#endif /* ULTRA_SERVO_H_ */