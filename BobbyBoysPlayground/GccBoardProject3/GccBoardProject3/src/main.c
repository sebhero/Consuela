
#include <asf.h>
#include "pulse.h"

#define TIMER_LED IOPORT_CREATE_PIN(PIOC, 24)

pwm_clock_t clock_setting = {
    .ul_clka = 1000000L,
    .ul_clkb = 0,
    .ul_mck = CHIP_FREQ_CPU_MAX
};

pwm_channel_t pwm_channel_instance = {
    .channel = PWM_CHANNEL_0,
    .ul_prescaler = PWM_CMR_CPRE_CLKA,
    .ul_duty = 1375,
    .ul_period = 50000,
    .polarity = PWM_HIGH
};

void configure_console() {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}

void configure_pwm() {

    // PC3 <-> Digital 35
    ioport_set_pin_mode(PIO_PC3_IDX, IOPORT_MODE_MUX_B);
    ioport_disable_pin(PIO_PC3_IDX);

    //// PC4 <-> Digital 36
    //ioport_set_pin_mode(PIO_PC4_IDX, IOPORT_MODE_MUX_B);
    //ioport_disable_pin(PIO_PC4_IDX);

    ioport_set_pin_dir(TIMER_LED, IOPORT_DIR_OUTPUT);


    pmc_enable_periph_clk(ID_PWM);
    pwm_channel_disable(PWM, PWM_CHANNEL_0);
    pwm_init(PWM, &clock_setting);
    pwm_channel_init(PWM, &pwm_channel_instance);
    pwm_channel_enable(PWM, PWM_CHANNEL_0);

    //pwm_channel_instance.channel = PWM_CHANNEL_1;
    //pwm_channel_instance.ul_duty = 0;
    //pwm_channel_disable(PWM, PWM_CHANNEL_1);
    //pwm_init(PWM, &clock_setting);
    //pwm_channel_init(PWM, &pwm_channel_instance);
    ////pwm_channel_enable(PWM, PWM_CHANNEL_1);
}


int main (void)
{
	sysclk_init();

	board_init();

    configure_console();

    pulse_init();
    pulse_start(0);
    pulse_start(1);

    uint32_t pulse_length;
    uint32_t result;
    uint32_t channel = 0;
    uint32_t state = 0;
    char ch;
    printf("Enter period for channel %lu", channel);
	while(1) {
        // Read the period from stdin
        result = scanf("%lu", &pulse_length);
        if(result) {
            // Set the new period of the pulse generator
            pulse_set_period(channel, pulse_length);
            // Switch over to the next pulse channel
            channel  = (channel + 1)%2;
            printf("Enter period for channel %lu", channel);
        }





    }
}
