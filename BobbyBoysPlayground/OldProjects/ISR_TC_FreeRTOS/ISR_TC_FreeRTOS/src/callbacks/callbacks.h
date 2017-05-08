


#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#define LED0 IOPORT_CREATE_PIN(PIOC, 26)
#define LED1 IOPORT_CREATE_PIN(PIOC, 24)
#define LED2 IOPORT_CREATE_PIN(PIOC, 22)

void init_callbacks();

void cpra_func_0();
void cprc_func_0();

void cpra_func_1();
void cprc_func_1();

void cpra_func_2();
void cprc_func_2();


#endif /* CALLBACKS_H_ */