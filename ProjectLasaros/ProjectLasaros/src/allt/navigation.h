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

#define sock_x 300
#define sock_y 50
#define glass_x 300
#define glass_y 350
#define cube_x 200
#define cube_y 200
#define dropOff_x 100
#define dropOff_y 300
#define PI 3.141592

//static uint16_t mid_x;
//static uint16_t mid_y;
extern int currentAngle;

void callForData(void);
void calcMidPos(void);
double distanceToPosition(uint8_t obj);
void updatePos(double hyp);
int angleToPos(void);
void valuesCalc(uint8_t obj);
void updateAngle(void);



#endif /* NAVIGATION_H_ */