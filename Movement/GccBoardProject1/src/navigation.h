/*
 * navigation.h
 *
 * Created: 03/05/2017 09:00:19
 *  Author: Robin_2
 */ 


#ifndef NAVIGATION_H_
#define NAVIGATION_H_
#include <inttypes.h>
#include <math.h>
#include <stdio_serial.h>
#include "motorFunc.h"

#define sock_x 133
#define sock_y 186
#define glass_x 23
#define glass_y 382
#define cube_x 472
#define cube_y 23
#define dropOff_x 478
#define dropOff_y 371
#define PI 3.141592

extern uint16_t x1_pos;
extern uint16_t y1_pos;
extern int currentAngle;

void callForData(uint16_t,uint16_t);
void calcMidPos(void);
double distanceToPosition(uint8_t obj);
void updatePos(double hyp);
int angleToPos(void);
void valuesCalc(uint8_t obj);
void updateAngle(void);
void angleCheck(void);
void rotationChooser(int degreesToPos);


#endif /* NAVIGATION_H_ */