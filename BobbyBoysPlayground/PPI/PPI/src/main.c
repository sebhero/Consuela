#include <asf.h>

#include "PPI.h"

xQueueHandle sem;


void configureConsole(void) {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}


void vMaster(void *pvParam) {

    configureConsole();
    xQueueHandle * pSem = (xQueueHandle *) pvParam;

    uint8_t cntr = 0;
    uint32_t flag_done = 1;
    uint8_t d[2];
    ioport_set_pin_dir(PIO_PA15_IDX, IOPORT_DIR_OUTPUT);
    for(;;) {

        PPI_init(role_master, pSem);
        flag_done = 0;
        d[0] = (cntr & 0xF);
        d[1] = ((cntr>>4) & 0xF);
        ioport_set_pin_level(PIO_PA15_IDX, IOPORT_PIN_LEVEL_HIGH);
        master_send(0, d, d + 2, &flag_done);
        ioport_set_pin_level(PIO_PA15_IDX, IOPORT_PIN_LEVEL_LOW);
        printf("Sending: %lu\n", (((d[1] & 0xF)<<4) | (d[0] & 0xF)));
        xSemaphoreTake(*pSem, pdMSTOTICKS(1000));

        vTaskDelay(pdMSTOTICKS(100));
        if(flag_done == 0) {
            printf("Failed to send!\n");
        }
        cntr++;

    }

    vTaskDelete(NULL);
}

void vSlave(void *pvParam) {

    configureConsole();
    xQueueHandle *pSem = (xQueueHandle *) pvParam;

    uint8_t rx_buffer[10];
    uint32_t flag_done = 1;
    ioport_set_pin_dir(PIO_PD0_IDX, IOPORT_DIR_OUTPUT);
    for(;;) {
    	PPI_init(role_slave, pSem);
        slave_recieve(rx_buffer, rx_buffer+2, &flag_done);
        ioport_set_pin_level(PIO_PD0_IDX, IOPORT_PIN_LEVEL_HIGH);
        xSemaphoreTake(*pSem, pdMSTOTICKS(1000));
        ioport_set_pin_level(PIO_PD0_IDX, IOPORT_PIN_LEVEL_LOW);
        if(flag_done == 0) {
            printf("Failed to reviece!\n");
        } else {
            printf("Recieved: %u\n", (((rx_buffer[1] & 0xF)<<4) | (rx_buffer[0] & 0xF)));
        }
    }

    vTaskDelete(NULL);

}

int main (void) {

	sysclk_init();
	board_init();

    vSemaphoreCreateBinary(sem);
    xSemaphoreTake(sem, pdMSTOTICKS(10));

    xTaskCreate(vMaster, "Master", configMINIMAL_STACK_SIZE*10+100, &sem, 2, NULL);
    //xTaskCreate(vSlave, "Slave", configMINIMAL_STACK_SIZE*10+100, &sem, 2, NULL);
    vTaskStartScheduler();

    for(;;) {
        //
    }
}
