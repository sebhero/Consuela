#include <asf.h>

#define BUFFER_SIZE  5

uint8_t rx_buffer[BUFFER_SIZE];
uint8_t tx_buffer[BUFFER_SIZE];

pdc_packet_t rx_packet;
pdc_packet_t tx_packet;

Pdc *pdc;
xQueueHandle sem;

void vApplicationMallocFailedHook() {
    // Indicate error
}

void vUART(void *pvParam) {

    uint8_t cntr = 0;
    for(;;) {
        xSemaphoreTake(sem, portMAX_DELAY);
        ++cntr;
        tx_buffer[0] = cntr/100;
        tx_buffer[1] = (cntr-tx_buffer[0]*100)/10;
        tx_buffer[2] = (cntr-tx_buffer[0]*100-tx_buffer[1]*10);
        tx_buffer[0] += '0';
        tx_buffer[1] += '0';
        tx_buffer[2] += '0';
        tx_buffer[3] =  '\n';
        tx_packet.ul_addr = (uint32_t) tx_buffer;
        tx_packet.ul_size = 4;
        pdc_tx_init(pdc, &tx_packet, NULL);

    }
    vTaskDelete(NULL);
}

void UART_Handler(void) {
    if ((uart_get_status(UART) & UART_SR_RXBUFF) == UART_SR_RXBUFF) {
        pdc_rx_init(pdc, &rx_packet, NULL);
        xSemaphoreGiveFromISR(sem, NULL);
    }
}


static void configure_console(void)
{
    const usart_serial_options_t uart_serial_options = {
        .baudrate = 115200,
        .paritytype = UART_MR_PAR_NO
    };
    sysclk_enable_peripheral_clock(ID_UART);
    stdio_serial_init(UART, &uart_serial_options);
}


void vTick(void *pvParam) {
    for(;;) {
        uint8_t str[] = {'T','i','c','k', '\n'};
        tx_packet.ul_size = 5;
        tx_packet.ul_addr = str;
        pdc_tx_init(pdc, &tx_packet, NULL);
        vTaskDelay(pdMSTOTICKS(5000));
    }
    vTaskDelete(NULL);
}

int main(void) {

    sysclk_init();
    board_init();

    configure_console();

    printf("Hello, world!\n");

    pdc = uart_get_pdc_base(UART);
    rx_packet.ul_addr = (uint32_t) rx_buffer;
    rx_packet.ul_size = BUFFER_SIZE;

    pdc_rx_init(pdc, &rx_packet, NULL);
    pdc_enable_transfer(pdc, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
    uart_enable_interrupt(UART, UART_IER_RXBUFF);
    NVIC_SetPriority(UART_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
    NVIC_EnableIRQ(UART_IRQn);

    vSemaphoreCreateBinary(sem);

    xTaskCreate(vUART, "", configMINIMAL_STACK_SIZE + 100, NULL, 3, NULL);
    xTaskCreate(vTick, "", configMINIMAL_STACK_SIZE + 100, NULL, 4, NULL);
    vTaskStartScheduler();

    while (1) {
        // Spin
    }

}
