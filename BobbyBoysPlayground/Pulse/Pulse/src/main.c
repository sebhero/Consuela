
#include <asf.h>

#include "pulse/pulse.h"


void configure_console() {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

void stop0(void) {
    printf("\nCounter reached(%lu%)! Stopping 0!", pulse_counter_get_cnt(0));
    pulse_generator_stop(0);
}

void stop1(void) {
    printf("\nCounter reached(%lu%)! Stopping 1!", pulse_counter_get_cnt(1));
    pulse_generator_stop(1);
}

uint32_t flag_p0 = 0;
uint32_t target = 0;
uint32_t cntr = 0;


void p0(void) {
    ioport_toggle_pin_level(PIO_PB27_IDX);
    pulse_counter_start(0);
}


int main (void) {
	// Standard funktioner
    sysclk_init();
	board_init();

    // Initialize a iopin
    ioport_set_pin_dir(PIO_PB27_IDX, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(PIO_PB27_IDX, IOPORT_PIN_LEVEL_LOW);

    // Initializera pulse
    uint32_t pulse_counter_channel = 0;
    pulse_counter_init_channel(pulse_counter_channel);
    pulse_counter_set_callback_function(pulse_counter_channel, p0);
    pulse_counter_set_trig_cnt(pulse_counter_channel, 100);
    pulse_counter_start(pulse_counter_channel);
    while(1) {
        delay_ms(1000);
    }


    /*
    configure_console();
    printf("\nStarting...\n");



    ioport_set_pin_dir(PIO_PC6_IDX, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(PIO_PC6_IDX, IOPORT_PIN_LEVEL_LOW);
    pulse_counter_set_callback_function(0, p0);
    while(1) {
        for(target = 1; target < 10; target = target + 1) {
            printf("\n%lu", target);
            flag_p0 = 0;
            cntr = 0;
            pulse_counter_set_trig_cnt(0, target);
            pulse_counter_start(0);
            for(uint32_t i = 1; i < target+10 && !flag_p0; ++i) {
                ++cntr;
                ioport_set_pin_level(PIO_PC6_IDX, IOPORT_PIN_LEVEL_HIGH);
                delay_ms(1);
                ioport_set_pin_level(PIO_PC6_IDX, IOPORT_PIN_LEVEL_LOW);
                delay_ms(1);
            }
            printf("\n---");
            delay_ms(100);
        }


    }
    */

    /*
    // Test of integrated system
    pulse_counter_set_callback_function(0, stop0);
    pulse_counter_set_callback_function(1, stop1);

    const uint32_t p_min = 500;
    const uint32_t p_max = 2500;

    uint32_t p0 = p_min;
    uint32_t p1 = p_max;
    pulse_generator_set_period(0, p0);
    pulse_generator_set_period(1, p1);

    printf("\nStarting generator...");
    printf("\np0 %lu", p0);
    printf("\np1 %lu", p1);
    pulse_generator_start(0);
    pulse_generator_start(1);
    delay_ms(2000);

    uint32_t restart_flag = 0;
    while(1) {

        printf("\n------------------------");
        p0 += 100;
        if(p0 > p_max) {
            p0 = p_min;
            restart_flag = 1;
            printf("\nStopping channe 0...");
            pulse_generator_stop(0);
        }
        p1 -= 100;
        if(p1 < p_min) {
            p1 = p_max;
            restart_flag = 1;
            printf("\nStopping channe 1...");
            pulse_generator_stop(1);
        }
        printf("\nSetting period p0: %lu", p0);
        printf("\nSetting period p1: %lu", p1);
        pulse_generator_set_period(0, p0);
        pulse_generator_set_period(1, p1);
        delay_ms(100);
        printf("\nMeasured(set) p0: %lu (%lu)", pulse_timer_get(0), p0);
        printf("\nMeasured(set) p1: %lu (%lu)", pulse_timer_get(1), p1);
        printf("\nPulses p0: %lu", pulse_counter_get_cnt(0));
        printf("\nPulses p1: %lu", pulse_counter_get_cnt(1));
        delay_ms(2000);
        if(restart_flag) {
            restart_flag = 0;
            pulse_counter_set_trig_cnt(0, 200);
            pulse_counter_start(0);
            pulse_counter_set_trig_cnt(1, 400);
            pulse_counter_start(1);
            printf("\nStarting generator...");
            printf("\np0 %lu", p0);
            printf("\np1 %lu", p1);
            pulse_generator_start(0);
            pulse_generator_start(1);
            delay_ms(2000);
        }
		// spin
	}
    */

}


