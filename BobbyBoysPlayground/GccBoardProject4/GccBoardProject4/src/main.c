
#include <asf.h>


void configure_console() {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}


void configure_tc() {
    ioport_set_pin_dir(PIO_PB26_IDX, IOPORT_DIR_INPUT);
    ioport_set_pin_mode(PIO_PB26_IDX,  IOPORT_MODE_MUX_B |  IOPORT_MODE_PULLUP | IOPORT_MODE_DEBOUNCE);
    ioport_disable_pin(PIO_PB26_IDX);

    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC0);

    tc_init(TC0, 0, TC_CMR_TCCLKS_XC0 | TC_CMR_CLKI);
    tc_start(TC0, 0);

}


int main (void)
{
	sysclk_init();

	board_init();

    configure_console();

    printf("Hello, World!\n");

    configure_tc();
    uint32_t cntr = 0;
	while(1) {
        delay_ms(2000);
        printf("\n #%lu cv: %lu", cntr, tc_read_cv(TC0, 0));
        cntr++;
    }
}
