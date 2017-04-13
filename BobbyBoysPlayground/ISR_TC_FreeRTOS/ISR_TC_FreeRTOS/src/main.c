
#include <asf.h>
#include "pwm/pwm.h"
#include "callbacks/callbacks.h"

xTimerHandle timer_switchDuty;
pwm_t 	pwm = {
	.pwm_ch_id = 0,
	.freq = 10,
	.pct_dty = 30,
	.cpra_callback = cpra_func,
	.cprc_callback = cprc_func
};


void vSwitchDuty(xTimerHandle pxTimer) {
	static uint32_t state = 0;
	if(state) {
		pwm.pct_dty = 10;
		change_pct_dty(pwm);
		state = 0;
	} else {
		pwm.pct_dty = 90;
		change_pct_dty(pwm);	
		state = 1;
	}
	
}

int main (void) {
	sysclk_init();

	board_init();

	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
	
	init_pwm();
	

	
	configure_pwm(pwm);
	
	timer_switchDuty = xTimerCreate("Switch duty", 1000, pdTRUE, 0, vSwitchDuty);
	xTimerStart(timer_switchDuty, 0);
    vTaskStartScheduler();
	while(1) {
		// spin
	}
}
