#include <asf.h>
#include "callbacks.h"

void cpra_func() {
	ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
}

void cprc_func() {
	ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_LOW);
}