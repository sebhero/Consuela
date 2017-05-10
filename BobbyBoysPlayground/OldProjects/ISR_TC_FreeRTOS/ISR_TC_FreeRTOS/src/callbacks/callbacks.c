#include <asf.h>
#include "callbacks.h"

void init_callbacks() {
		ioport_set_pin_dir(LED0, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(LED0, IOPORT_PIN_LEVEL_HIGH);
		
		ioport_set_pin_dir(LED1, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(LED1, IOPORT_PIN_LEVEL_HIGH);
		
		ioport_set_pin_dir(LED2, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(LED2, IOPORT_PIN_LEVEL_HIGH);
}

void cpra_func_0() {
	ioport_set_pin_level(LED0, IOPORT_PIN_LEVEL_HIGH);
}

void cprc_func_0() {
	ioport_set_pin_level(LED0, IOPORT_PIN_LEVEL_LOW);
}

void cpra_func_1() {
	ioport_set_pin_level(LED1, IOPORT_PIN_LEVEL_HIGH);
}

void cprc_func_1() {
	ioport_set_pin_level(LED1, IOPORT_PIN_LEVEL_LOW);
}

void cpra_func_2() {
	ioport_set_pin_level(LED2, IOPORT_PIN_LEVEL_HIGH);
}

void cprc_func_2() {
	ioport_set_pin_level(LED2, IOPORT_PIN_LEVEL_LOW);
}