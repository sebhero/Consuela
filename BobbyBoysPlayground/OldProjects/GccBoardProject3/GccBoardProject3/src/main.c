
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


static void tc_capture_initialize(void) {

    ioport_set_pin_dir(PIO_PA2_IDX, IOPORT_DIR_INPUT);
    ioport_set_pin_mode(PIO_PA2_IDX,  IOPORT_MODE_MUX_A |  IOPORT_MODE_PULLUP);
    ioport_disable_pin(PIO_PA2_IDX);

    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC1);

    tc_init(TC0, 1,	TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_LDRA_RISING | TC_CMR_LDRB_FALLING
    | TC_CMR_ABETRG | TC_CMR_ETRGEDG_FALLING);

    tc_start(TC0, 1);
    tc_enable_interrupt(TC0, 1, TC_IER_LDRBS );
    NVIC_DisableIRQ(TC1_IRQn);
}

void TC1_Handler(void) {
    tc_get_status(TC0,1);
    NVIC_DisableIRQ(TC1_IRQn);
    uint32_t rb = tc_read_rb(TC0, 1);
    uint32_t ra = tc_read_ra(TC0, 1);
    uint32_t diff = rb - ra;
    uint32_t duration = (diff ) / (((CHIP_FREQ_CPU_MAX/1000)/1000) / 2);
    printf("\n %lu %lu %lu", ra, rb, duration);
}

int main (void)
{
	sysclk_init();

	board_init();

    configure_console();

    //pulse_init();
    //pulse_start(0);
    //pulse_start(1);
//
    //pulse_set_period(0, 1234);

    tc_capture_initialize();

    printf("Hello, World!\n");
	while(1) {

        delay_ms(1000);
        NVIC_EnableIRQ(TC1_IRQn);


    }
}
