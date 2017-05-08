
#include <asf.h>
#include "queue.h"



#define ERROR_LED PIO_PC22_IDX

#define GREEN_LED PIO_PC25_IDX
#define RED_LED PIO_PC26_IDX
#define YELLOW_LED PIO_PC28_IDX

//#define pdMS_TO_TICKS( xTimeInMs ) ( ( portTickType ) ( ( ( unsigned long ) ( xTimeInMs ) * ( portTickType ) configTICK_RATE_HZ ) / ( portTickType) 1000 ) )

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( portTickType ) ( ( ( portTickType) ( xTimeInMs ) * ( portTickType) configTICK_RATE_HZ ) / ( portTickType) 1000 ) )
#endif


static xTimerHandle timer_BlinkGreen;
static xTaskHandle task_ButtonPress;
#define TSK_PRIO_BTN 2

static xQueueHandle sem;

/**
 * \file
 *
 * \brief Helper function to get the peripheral ID from a pin
 *
 */
static uint32_t ioport_pin_to_ID_Port(ioport_pin_t pin) {
     uint32_t ID = 0;
    switch(arch_ioport_pin_to_port_id(pin)) {
        case(IOPORT_PIOA):
        ID = ID_PIOA;
        break;
        case(IOPORT_PIOB):
        ID = ID_PIOB;
        break;
        case(IOPORT_PIOC):
        ID = ID_PIOC;
        break;
        case(IOPORT_PIOD):
        ID = ID_PIOD;
        break;
    }
    return ID;
}

/**
 * \file
 *
 * \brief Helper function to get the port IRQn froma pin
 *
 */
static IRQn_Type ioport_pin_to_Port_IRQn(ioport_pin_t pin) {
    IRQn_Type IRQn = 0;
    switch(arch_ioport_pin_to_port_id(pin)) {
        case(IOPORT_PIOA):
        IRQn = PIOA_IRQn;
        break;
        case(IOPORT_PIOB):
        IRQn = PIOB_IRQn;
        break;
        case(IOPORT_PIOC):
        IRQn = PIOC_IRQn;
        break;
        case(IOPORT_PIOD):
        IRQn = PIOD_IRQn;
        break;
    }
    return IRQn;
}

void configure_console() {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}

void configure_leds() {
    ioport_set_pin_dir(GREEN_LED, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(GREEN_LED, IOPORT_PIN_LEVEL_HIGH);

    ioport_set_pin_dir(RED_LED, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(RED_LED, IOPORT_PIN_LEVEL_HIGH);

    ioport_set_pin_dir(YELLOW_LED, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(YELLOW_LED, IOPORT_PIN_LEVEL_HIGH);

    ioport_set_pin_dir(ERROR_LED, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(ERROR_LED, IOPORT_PIN_LEVEL_LOW);
}

void vBlinkGreen(xTimerHandle pxTimer) {
    ioport_toggle_pin_level(GREEN_LED);
}

void vApplicationMallocFailedHook() {
        // Indicate error
        ioport_set_pin_dir(ERROR_LED, IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(ERROR_LED, IOPORT_PIN_LEVEL_LOW);
        uint32_t cntr = 100;
        while(cntr != 0) {
            for(uint32_t i = 0; i < (84000000>>4); i++) {
                // spin
            }
            ioport_toggle_pin_level(ERROR_LED);
            --cntr;
        }
}

void vButtonPress(void *pvParameters) {
    while(1) {
        ioport_set_pin_level(YELLOW_LED, IOPORT_PIN_LEVEL_HIGH);
        xSemaphoreTake(sem, portMAX_DELAY);
        ioport_toggle_pin_level(RED_LED);
        ioport_set_pin_level(YELLOW_LED, IOPORT_PIN_LEVEL_LOW);
    }
    vTaskDelete(task_ButtonPress);
}

static void btn1_handler(const uint32_t id, const uint32_t index) {
    if ((id == ioport_pin_to_ID_Port(PIO_PC24_IDX)) && (index == ioport_pin_to_mask(PIO_PC24_IDX))){
        if(!ioport_get_pin_level(PIO_PC24_IDX)) {
            ioport_toggle_pin_level(YELLOW_LED);
            xSemaphoreGiveFromISR(sem, pdTRUE);
        }
    }
}

static void configure_button(ioport_pin_t pin, void (*handler)(const uint32_t, const uint32_t), uint32_t pin_attr, uint32_t trig_attr) {
    Pio *pio_base = arch_ioport_pin_to_base(pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(pin);
    IRQn_Type IRQn = ioport_pin_to_Port_IRQn(pin);
    uint32_t ID = ioport_pin_to_ID_Port(pin);

    pio_set_input(pio_base, pin_mask, pin_attr);
    pio_handler_set(pio_base, ID, pin_mask, trig_attr, handler);
    pio_enable_interrupt(pio_base, pin_mask);
    NVIC_ClearPendingIRQ(IRQn);
    NVIC_EnableIRQ(IRQn);
}




int main (void)
{
    sysclk_init();

	board_init();

    configure_console();
    printf("\nConsole configured...");

    configure_leds();

    configure_button(PIO_PC24_IDX, btn1_handler, (PIO_PULLUP | PIO_DEBOUNCE),  PIO_IT_FALL_EDGE);

    timer_BlinkGreen = xTimerCreate("Blink Green", pdMS_TO_TICKS(200), pdTRUE, 0, vBlinkGreen);
    xTimerStart(timer_BlinkGreen, 0);


    vSemaphoreCreateBinary(sem); //xSemaphoreCreateCounting(1, 0);

    if(sem == NULL) {
        printf("\nCould not create semaphore!");
    }

    xTaskCreate(vButtonPress, "Button press", configMINIMAL_STACK_SIZE + 100, NULL, TSK_PRIO_BTN, &task_ButtonPress);

    vTaskStartScheduler();


    // Indicate error
    ioport_set_pin_dir(ERROR_LED, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(ERROR_LED, IOPORT_PIN_LEVEL_LOW);
    while(1) {
        for(uint32_t i = 0; i < (84000000>>3); i++) {
            // spin
        }
        ioport_toggle_pin_level(ERROR_LED);
    }


}
