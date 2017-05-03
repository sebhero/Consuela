
#include <asf.h>

void configure_console() {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}

int main (void)
{
    sysclk_init();

	board_init();

    configure_console();

	while(1) {
        // spin
    }
}
