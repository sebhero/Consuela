
#include <asf.h>
#include "pulse/pulse.h"
#include "navigation/navigation.h"

static xTaskHandle xControllerHandle;

static void configure_console(void) {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}

void vApplicationMallocFailedHook() {
    // Indicate error
}

typedef enum {
    stopped, paused, running
} state_t;

void vController(void *pvParameters) {

    for(;;) {
        uint8_t objectID = 0;
        int degreesToPos;
        double tempVariabel = 0;
        while(objectID<4){

            valuesCalc(objectID);
            degreesToPos = angleToPos();
            if (degreesToPos<0){
                degreesToPos = abs(degreesToPos);
                rotateRightByDegrees(degreesToPos);
                updateAngle();
            } else {
                rotateLeftByDegrees(degreesToPos);
                updateAngle();
            }
            while (distanceToPosition(objectID)>30.0){
                delay_ms(500);
                int ek = counterA-counterB;
                tempVariabel = counterA*1.355;
                reglerahjul3(ek);
                updatePos(tempVariabel);
                tempVariabel = 0;
            }
            objectID++;
            stop();
        }

    }

}

int main (void) {
	sysclk_init();

	board_init();

    configure_console();
    printf("Hello, world!\n");

    xTaskCreate(vController, "Controller", configMINIMAL_STACK_SIZE + 100, NULL, 1, &xControllerHandle);



    vTaskStartScheduler();

    for(;;) {

    }
}
