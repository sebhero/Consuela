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
#include "pulseCounterHandler.h"

#define sock_x 136
#define sock_y 200
#define glass_x 56
#define glass_y 362
#define cube_x 434
#define cube_y 35
#define dropOff_x 451
#define dropOff_y 350
#define PI 3.141592

//Temporary
typedef enum _object_t {
	SOCK = 2,
	SQUARE = 3,
	GLASS = 4
} Object;

extern uint16_t x1_pos;
extern uint16_t y1_pos;

void setObject(Object obj,uint16_t x, uint16_t y);
void setCollectAll(uint8_t getAll);
void setDonePickup(void);
void setDropoffDone(void);
uint8_t goToNext();
void setPath(void);

void initNav(void);
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