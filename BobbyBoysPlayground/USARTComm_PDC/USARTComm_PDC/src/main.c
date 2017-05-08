
#include <asf.h>

static uint32_t tx_complete = 1;
#define tx_buff_size 1
static char tx_buff[2][tx_buff_size];

#define rx_buff_size 1
static char rx_buff[2][rx_buff_size];


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
        .usart_IRQ_sources = 0,
        .usart_rx_pin = PIO_PA10_IDX,
        .usart_tx_pin = PIO_PA11_IDX,
    }
};

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

static void clear_usart(uint32_t ch_n) {
    usart_reset_rx(usarts[ch_n].usart);
    usart_reset_tx(usarts[ch_n].usart);

    usart_enable_tx(usarts[ch_n].usart);
    usart_enable_rx(usarts[ch_n].usart);
}

void configure_usarts() {

    configure_usart(0);

}

static void configure_console(void)
{
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}


void USART0_Handler(void) {
	uint32_t status;

	status = usart_get_status(USART0);
	if (status & US_CSR_TXBUFE) {
        tx_complete = 1;
        usart_disable_interrupt(usarts[0].usart, US_IDR_TXBUFE);
        printf("\nSent: %s", tx_buff);
    }
    if(status & US_CSR_ENDRX) {
        printf("\nrx: %s", rx_buff[0]);

        pdc_packet_t rx_packet;
        rx_packet.ul_size = rx_buff_size;
        rx_packet.ul_addr = rx_buff[0];
        Pdc *pdc = usart_get_pdc_base(usarts[0].usart);
        pdc_rx_init(pdc, &rx_packet, NULL);


    }

}



    // Disable PDC
    /*
	pdc_disable_transfer(pdc,PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);
	usart_disable_interrupt(USART0, US_IDR_RXBUFF);
    usart_clear();
    */



int main() {

	sysclk_init();

	board_init();

    configure_console();

    pdc_packet_t tx_packet;
    pdc_packet_t rx_packet;
    rx_packet.ul_size = rx_buff_size;
    rx_packet.ul_addr = rx_buff[0];

    configure_usarts();

    clear_usart(0);

    Pdc *pdc = usart_get_pdc_base(usarts[0].usart);

    pdc_enable_transfer(pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);

    usart_disable_interrupt(usarts[0].usart, US_IDR_RXRDY);

    pdc_rx_init(pdc, &rx_packet, NULL);
    usart_enable_interrupt(usarts[0].usart, US_IER_RXBUFF);

    uint32_t cntr = 0;
    while(1) {
    /*
        if(tx_complete) {
            tx_complete = 0;
            tx_packet.ul_addr = (uint32_t) tx_buff;
            tx_packet.ul_size = sprintf(tx_buff, "\nBuffer: %lu", cntr);
            pdc_tx_init(pdc, &tx_packet, NULL);
            usart_enable_interrupt(usarts[0].usart, US_IER_TXBUFE);
            ++cntr;

        } else {
            printf("\nCould not start tx?");
        }
        */
        delay_ms(4000);
    }
}