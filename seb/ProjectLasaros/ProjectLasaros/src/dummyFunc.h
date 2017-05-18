/*
 * dummyFunc.h
 *
 * Created: 2017-05-18 07:56:32
 *  Author: Spellabbet
 */ 


#ifndef DUMMYFUNC_H_
#define DUMMYFUNC_H_
#include "../com/TwiComHandler.h"

//drive handle goto next object
uint8_t goToNext(void);

void setObject(Object object,int x, int y);

void setGetAll(uint8_t getall);

void setDonePickup(void);

void setDoneDropoff(void);


#endif /* DUMMYFUNC_H_ */