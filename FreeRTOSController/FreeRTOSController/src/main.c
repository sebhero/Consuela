
#include <asf.h>


#define LED  IOPORT_CREATE_PIN(PIOB, 26) // digital pin 22

int main (void)
{
	sysclk_init();
    ioport_init();
	board_init();
	
<<<<<<< HEAD
	ioport_set_pin_dir(LED, IOPORT_DIR_OUTPUT);   // LED pin set as output
	
	
	/* Insert application code here, after the board has been initialized. */
	
	
	while(1){
		ioport_set_pin_level(LED, true);
		delay_ms(1000);
		ioport_set_pin_level(LED, false);
		delay_ms(1000);
	}
=======
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.paritytype = CONF_UART_PARITY
	};
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
	

	printf("Hello, World!\n");
>>>>>>> f65643023557366cd132fcfbf92f1ae60d8a7347
}
