/*
 * motorFunc.h
 *
 * Created: 07/04/2017 12:54:59
 *  Author: Robin_2
 */ 


#ifndef MOTORFUNC_H_
#define MOTORFUNC_H_
#define pin24 PIO_PA15_IDX

void pulse(uint16_t motorSpeed);
void forward(void);
void rotate(void);





#endif /* MOTORFUNC_H_ */