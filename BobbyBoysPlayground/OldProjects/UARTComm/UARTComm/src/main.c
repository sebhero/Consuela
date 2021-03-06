#include <asf.h>

static void configure_console(void) {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };

    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}



typedef struct {
    Usart *usart;
    sam_usart_opt_t usart_opt;
    uint32_t usart_ID;
    IRQn_Type usart_IRQn;
    uint32_t usart_IRQ_sources;
    ioport_pin_t usart_rx_pin;
    ioport_pin_t usart_tx_pin;
    void (*callback)(void);
} usart_serial_t;

usart_serial_t usarts[] = {
    {
        .usart = USART0,
        .usart_opt = {
            .baudrate = 115200,
            .char_length = US_MR_CHRL_8_BIT,
            .parity_type = US_MR_PAR_NO,
            .stop_bits = US_MR_NBSTOP_1_BIT,
            .channel_mode = US_MR_CHMODE_NORMAL,
            .irda_filter = 0
        },
        .usart_ID = ID_USART0,
        .usart_IRQn = USART0_IRQn,
        .usart_IRQ_sources = (US_IER_RXRDY),
        .usart_rx_pin = PIO_PA10_IDX,
        .usart_tx_pin = PIO_PA11_IDX,
    }
};

void USART0_Handler(void) {
    const uint32_t ch_n = 0;
    uint32_t status = usart_get_status(usarts[ch_n].usart);
    uint32_t buffer;
    if (status & US_CSR_RXRDY) {
        usart_getchar(usarts[ch_n].usart, &buffer);
        usart_putchar(usarts[ch_n].usart, buffer);
        printf("\nrx ch: %c", (char) buffer);
    }
}


void configure_usart(uint32_t ch_n) {

    ioport_set_pin_dir(usarts[ch_n].usart_rx_pin, IOPORT_DIR_INPUT);
    ioport_disable_pin(usarts[ch_n].usart_rx_pin);
    ioport_set_pin_dir(usarts[ch_n].usart_tx_pin, IOPORT_DIR_OUTPUT);
    ioport_set_pin_mode(usarts[ch_n].usart_tx_pin, IOPORT_MODE_PULLUP);
    ioport_disable_pin(usarts[ch_n].usart_tx_pin);

    sysclk_enable_peripheral_clock(usarts[ch_n].usart_ID);

    usart_init_rs232(usarts[ch_n].usart, &usarts[ch_n].usart_opt, sysclk_get_cpu_hz());

    usart_disable_interrupt(usarts[ch_n].usart, 0xFFFFFFFF);

    NVIC_EnableIRQ(usarts[ch_n].usart_IRQn);

    usart_enable_tx(usarts[ch_n].usart);
    usart_enable_rx(usarts[ch_n].usart);

    usart_enable_interrupt(usarts[ch_n].usart, usarts[ch_n].usart_IRQ_sources);
}

void configure_usarts() {
    configure_usart(0);
}

int main(void) {

	sysclk_init();

	board_init();

	configure_console();

    configure_usarts();
    printf("\nStarting...\n\n");

    char ch;
    while(1) {
        char buff[128];
        char *buff_ptr;
        if(scanf("%s", buff)) {
            printf("\nGot buffer");
            buff_ptr = buff;
            while(*buff_ptr != '\0') {
                printf("\Transmission...");
                while((usart_get_status(usarts[0].usart) & UART_SR_TXRDY) != UART_SR_TXRDY) {
                    // Wait until TXRDY, i.e.
                }
                if(usart_putchar(usarts[0].usart, *buff_ptr)) {
                    printf("\nFailed to send: %c", *buff_ptr);
                } else {
                    printf("\nSent: %c", *buff_ptr);
                }
                ++buff_ptr;
            }
        }
   	}
}
