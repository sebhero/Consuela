
#include <asf.h>
#include "pulse/pulse.h"
#include "communication/communication.h"
//#include "navigation/navigation.h"

static xTaskHandle xControllerHandle;

void vApplicationMallocFailedHook() {
    // Indicate error
}

#define val 256
const uint8_t rx_buffer_size = 4;
uint8_t rx_buffer[val];

pdc_packet_t rx_packet;
pdc_packet_t tx_packet;


xQueueHandle sem;
Pdc *pdc;

void vRead(void *pvParameters) {

    uint32_t cntr = 0;
    // Det är inte detta somtriggar
       // xSemaphoreTake(sem, 10);
         xSemaphoreTake(sem, 10);
    for(;;) {


        rx_packet.ul_size = rx_buffer_size;
        rx_packet.ul_addr = rx_buffer;
        pdc_rx_init(pdc, &rx_packet, NULL);
        usart_enable_interrupt(USART0, US_IER_ENDRX);


        uint8_t str[] = {'H','E','L','L', 'O'};
        tx_packet.ul_size = 5;
        tx_packet.ul_addr = str;
        pdc_tx_init(pdc, &tx_packet, NULL);


        printf("Tick %lu\n", ++cntr);
        xSemaphoreTake(sem, portMAX_DELAY);
        printf("\nrx: %c%c%c%c", rx_buffer[0],rx_buffer[1],rx_buffer[2],rx_buffer[3]);
        vTaskDelay(1000);
    }
    vTaskDelete(NULL);
}

void vBlinker(void *pvParam) {
    ioport_set_pin_dir(PIO_PA15_IDX, IOPORT_DIR_OUTPUT);

    for(;;) {
        ioport_toggle_pin_level(PIO_PA15_IDX);
        vTaskDelay(200);
    }

    vTaskDelete(NULL);
}

int main (void) {
	sysclk_init();

	board_init();

    configure_console();
    printf("Hello, world!\n");


    vSemaphoreCreateBinary(sem);

    configure_usart(&pdc, sem);

    xTaskCreate(vBlinker, "", configMINIMAL_STACK_SIZE + 100, NULL, 4,NULL);

    xTaskCreate(vRead, "Reader", configMINIMAL_STACK_SIZE + 100, NULL, 1, &xControllerHandle);
    vTaskStartScheduler();

    for(;;) {

    }
}
