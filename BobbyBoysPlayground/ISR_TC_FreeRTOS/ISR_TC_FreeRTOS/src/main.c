
#include <asf.h>
#include "pwm/pwm.h"
#include "callbacks/callbacks.h"

int main (void) {
	sysclk_init();

	board_init();

	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED, IOPORT_PIN_LEVEL_HIGH);
	
	init_pwm();
	
	pwm_t pwm = {
		.pwm_ch_id = 0,
		.freq = 10,
		.pct_dty = 30,
		.cpra_callback = cpra_func,
		.cprc_callback = cprc_func
	};
	
	configure_pwm(pwm);

	while(1) {
		delay_ms(1000);
		pwm.pct_dty = 10;
		change_pct_dty(pwm);
		delay_ms(1000);
		pwm.pct_dty = 90;
		change_pct_dty(pwm);
	}
}
