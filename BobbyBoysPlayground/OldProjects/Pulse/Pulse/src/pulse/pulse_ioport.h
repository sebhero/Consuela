
#ifndef PULSE_IOPORT_H_
#define PULSE_IOPORT_H_

typedef struct {
	ioport_pin_t pin;
	uint32_t mode;
	void (*handler)(const uint32_t, const uint32_t);
	uint32_t trigg_attr;
	uint32_t cnt;
} pulse_ioport_t;


uint32_t pulse_ioport_get_cnt(uint32_t ch_n);



void ch0_handler(const uint32_t id, const uint32_t index);



#endif /* PULSE_IOPORT_H_ */