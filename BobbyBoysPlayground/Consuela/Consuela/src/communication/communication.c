#include "communication.h"
#include <asf.h>

void configure_console(void) {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}


Pdc *pdc = NULL;
xQueueHandle *sem;

void configure_usart(Pdc **pdc_to_set, xQueueHandle sem_to_use) {

    sem = sem_to_use;
    Usart *usart = USART0;
    sam_usart_opt_t usart_opt = {
        .baudrate = 115200,
        .char_length = US_MR_CHRL_8_BIT,
        .parity_type = US_MR_PAR_NO,
        .stop_bits = US_MR_NBSTOP_1_BIT,
        .channel_mode = US_MR_CHMODE_NORMAL,
        .irda_filter = 0
    };
    uint32_t usart_ID = ID_USART0;
    IRQn_Type usart_IRQn = USART0_IRQn;
    uint32_t usart_IRQ_sources = 0;
    ioport_pin_t usart_rx_pin = PIO_PA10_IDX;
    ioport_pin_t usart_tx_pin = PIO_PA11_IDX;

    ioport_set_pin_dir(usart_rx_pin, IOPORT_DIR_INPUT);
    ioport_disable_pin(usart_rx_pin);

    ioport_set_pin_dir(usart_tx_pin, IOPORT_DIR_OUTPUT);
    ioport_set_pin_mode(usart_tx_pin, IOPORT_MODE_PULLUP);
    ioport_disable_pin(usart_tx_pin);

    sysclk_enable_peripheral_clock(usart_ID);

    usart_init_rs232(usart, &usart_opt, sysclk_get_cpu_hz());

    usart_disable_interrupt(usart, 0xFFFFFFFF);

    NVIC_SetPriority(usart_IRQn, 12);
    NVIC_EnableIRQ(usart_IRQn);

    usart_enable_tx(usart);
    usart_enable_rx(usart);

    usart_enable_interrupt(usart,  usart_IRQ_sources);

    pdc = usart_get_pdc_base(usart);
    *pdc_to_set = pdc;
    pdc_enable_transfer(pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);

    usart_get_status(USART0);
    NVIC_ClearPendingIRQ(usart_IRQn);
}

void USART0_Handler(void) {
    uint32_t status;
    status = usart_get_status(USART0);
    if(status & US_CSR_ENDRX) {
        printf("ISR\n");
        usart_disable_interrupt(USART0, US_IER_ENDRX);
        xSemaphoreGiveFromISR(sem, pdFALSE);
    }
}