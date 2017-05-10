
#include <asf.h>
#include "pulse/pulse.h"
#include "communication/communication.h"
//#include "navigation/navigation.h"

static xTaskHandle xControllerHandle;



void vApplicationMallocFailedHook() {
    // Indicate error
}

typedef enum {
    stopped, paused, running
} state_t;



#define val 256
const uint8_t rx_buffer_size = 4;
uint8_t rx_buffer[val];

pdc_packet_t rx_packet;


static xQueueHandle sem;


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


        printf("Tick %lu\n", ++cntr);
        xSemaphoreTake(sem, portMAX_DELAY);
        printf("\nrx: %c%c%c%c", rx_buffer[0],rx_buffer[1],rx_buffer[2],rx_buffer[3]);
        vTaskDelay(1000);
    }
}

int main (void) {
	sysclk_init();

	board_init();

    configure_console();
    printf("Hello, world!\n");

    configure_usart();

    vSemaphoreCreateBinary(sem);

    xTaskCreate(vRead, "Reader", configMINIMAL_STACK_SIZE + 100, NULL, 1, &xControllerHandle);
    vTaskStartScheduler();

    for(;;) {

    }
}
