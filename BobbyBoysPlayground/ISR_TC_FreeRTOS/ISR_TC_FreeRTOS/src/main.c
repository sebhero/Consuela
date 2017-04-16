
#include <asf.h>
#include "pwm/pwm.h"
#include "callbacks/callbacks.h"

xTimerHandle timer_switchDuty;
pwm_t pwm_a = {
	.pwm_ch_id = 0,
	.freq = 10,
	.pulse_us = 30,
	.cpra_callback = cpra_func_0,
	.cprc_callback = cprc_func_0
};

pwm_t pwm_b = {
	.pwm_ch_id = 1,
	.freq = 10,
	.pulse_us = 30,
	.cpra_callback = cpra_func_1,
	.cprc_callback = cprc_func_1
};

void vSwitchDuty(xTimerHandle pxTimer) {
	pwm_a.pulse_us = 110L;
	change_pulse_us(pwm_a);
	pwm_b.pulse_us = 120L;
	change_pulse_us(pwm_b);
	//static uint32_t state = 0;
	//if(state) {
		//pwm_a.pulse_us = 10000L;
		//change_pulse_us(pwm_a);
		//pwm_b.pulse_us = 20000L;
		//change_pulse_us(pwm_b);
		//state = 0;
	//} else {
		//pwm_a.pulse_us = 50000L;
		//change_pulse_us(pwm_a);	
		//pwm_b.pulse_us = 90000L;
		//change_pulse_us(pwm_b);
		//state = 1;
	//}
	
}

int main (void) {
	sysclk_init();

	board_init();
	
	init_callbacks();
	
	init_pwm();
		
	configure_pwm(pwm_a);
	configure_pwm(pwm_b);
	
	timer_switchDuty = xTimerCreate("Switch duty", 1000, pdTRUE, 0, vSwitchDuty);
	xTimerStart(timer_switchDuty, 0);
    vTaskStartScheduler();
	while(1) {
		// spin
	}
}
