
#include <asf.h>

int main (void)
{
	sysclk_init();

	board_init();
	
	// PC2 <-> Digital 34
	ioport_set_pin_mode(PIO_PC2_IDX, IOPORT_MODE_MUX_B);
	ioport_disable_pin(PIO_PC2_IDX);
	
	pwm_clock_t clock_setting = {
		.ul_clka = 1000 * 100,
		.ul_clkb = 0,
		.ul_mck = CHIP_FREQ_CPU_MAX
	};
	
	pwm_channel_t pwm_channel_instance = {
		.ul_prescaler = PWM_CMR_CPRE_CLKA,
		.ul_period = 100,
		.ul_duty = 50,
		.channel = PWM_CHANNEL_0
	};
	
	pmc_enable_periph_clk(ID_PWM);
	pwm_channel_disable(PWM, PWM_CHANNEL_0);	
	pwm_init(PWM, &clock_setting);
	pwm_channel_init(PWM, &pwm_channel_instance);	
	pwm_channel_enable(PWM, PWM_CHANNEL_0);
	
	while(1) {
		// spin
	}

}
