


#ifndef PWM_H_
#define PWM_H_
#include <asf.h>
#include "../callbacks/callbacks.h"

#define LED IOPORT_CREATE_PIN(PIOC, 26)

typedef struct {
	Tc* TC;
	uint32_t ch;
	IRQn_Type IRQn;
	uint32_t ID_TC;
	void (*cpra_callback)(void);
	void (*cprc_callback)(void);
	} pwm_ch ;

typedef struct {
	uint32_t pwm_ch_id;
	uint32_t freq;
	uint32_t pulse_us;
	void (*cpra_callback)(void);
	void (*cprc_callback)(void);
	} pwm_t;


extern pwm_ch channels[];
void init_pwm();
void change_pulse_us(pwm_t pwm);
void configure_pwm(pwm_t pwm);

#endif /* PWM_H_ */