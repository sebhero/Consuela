#include <asf.h>


static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};

	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

void USART0_Handler(void) {
    uint32_t status = usart_get_status(USART0);
    uint32_t buffer;
    if (status & US_CSR_RXRDY) {
        usart_getchar(USART0, &buffer);
        usart_putchar(USART0, buffer);
        printf("\nrx ch: %c", (char) buffer);
    }
}


int main(void) {

	sysclk_init();

	board_init();

	configure_console();

	const sam_usart_opt_t usart_console_settings = {
    	.baudrate = 115200,
    	.char_length = US_MR_CHRL_8_BIT,
    	.parity_type = US_MR_PAR_NO,
    	.stop_bits = US_MR_NBSTOP_1_BIT,
    	.channel_mode = US_MR_CHMODE_NORMAL,
    	.irda_filter = 0
	};

    ioport_set_pin_dir(PIO_PA10_IDX, IOPORT_DIR_INPUT);
    ioport_disable_pin(PIO_PA10_IDX);
    ioport_set_pin_dir(PIO_PA10_IDX, IOPORT_DIR_OUTPUT);
    ioport_disable_pin(PIO_PA11_IDX);

	sysclk_enable_peripheral_clock(ID_USART0);

	usart_init_rs232(USART0, &usart_console_settings, sysclk_get_cpu_hz());

	usart_disable_interrupt(USART0, 0xFFFFFFFF);

	NVIC_EnableIRQ(USART0_IRQn);

	usart_enable_tx(USART0);
	usart_enable_rx(USART0);



	usart_enable_interrupt(USART0, US_IER_RXRDY);

	while(1) {
        // Spin
   	}
}
