
#include <asf.h>

#define ERROR_LED PIO_PC25_IDX

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( portTickType ) ( ( ( portTickType) ( xTimeInMs ) * ( portTickType) configTICK_RATE_HZ ) / ( portTickType) 1000 ) )
#endif

#define RGB_RED_LED PIO_PC17_IDX
#define RGB_GREEN_LED PIO_PC15_IDX
#define RGB_BLUE_LED PIO_PC13_IDX

typedef struct {
    uint32_t id;
    uint32_t delay_ms;
    uint32_t led;
} taskData_t;

taskData_t red = {
    .id = 0,
    .delay_ms = 300,
    .led = RGB_RED_LED
};

taskData_t green = {
    .id = 1,
    .delay_ms = 400,
    .led = RGB_GREEN_LED
};

taskData_t blue = {
    .id = 2,
    .delay_ms = 500,
    .led = RGB_BLUE_LED
};



static xTaskHandle vTemplateTask_Handle1;
static xTaskHandle vTemplateTask_Handle2;
static xTaskHandle vTemplateTask_Handle3;

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

void configure_console() {
    const usart_serial_options_t uart_serial_options = {
        .baudrate = CONF_UART_BAUDRATE,
        .paritytype = CONF_UART_PARITY
    };
    sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
    stdio_serial_init(CONF_UART, &uart_serial_options);
}




void vTemplateTask(void *pvParameters) {

    // This is the loop of the task. It should never exit

    taskData_t data  =  *((taskData_t *) pvParameters);

    ioport_set_pin_dir(data.led, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(data.led, IOPORT_PIN_LEVEL_LOW);

    while(1) {
        ioport_toggle_pin_level(data.led);
        vTaskDelay(pdMS_TO_TICKS(data.delay_ms));
    }

    // If we drop out of the infinite loop for some reason, then delete ourself.
    // If neede also log that the task has ended
    vTaskDelete(NULL);
}


void gotoError() {

    // Indicate error
    // If the vTaskStartScheduler returns indicate that we are in an error state by blinking a led
    ioport_set_pin_dir(ERROR_LED, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(ERROR_LED, IOPORT_PIN_LEVEL_LOW);
    while(1) {
        for(uint32_t i = 0; i < (84000000>>3); i++) {
            // spin
        }
        ioport_toggle_pin_level(ERROR_LED);
    }

}

int main (void)
{
	sysclk_init();

	board_init();

    //configure_console();
    //printf("\nHello, World!");

    uint32_t task_prio = 2;
    // vTemplaeTask is the function name, i.e. the code of the task
    // "Templa task"  is the name of the task
    // configMINIMAL_STACK_SIZE+100 is the size of the tasks stack in _words_
    // &delay_RGB_flash is the address of variable being sent into the task
    // task_prio is the priority of the task, low numeric values represent low priority task
    // &vTemplateTask_Handle is the address of the handle to the tas, this varialbe will be set in xTaskCreate
    // the if(pdFail == ...) gotoError(); construction is to test that the task could be created
    if(pdFAIL == xTaskCreate(vTemplateTask, "Template task1", configMINIMAL_STACK_SIZE + 100, &red, 1, &vTemplateTask_Handle1)) gotoError();
    if(pdFAIL == xTaskCreate(vTemplateTask, "Template task2", configMINIMAL_STACK_SIZE + 100, &green, 2, &vTemplateTask_Handle2)) gotoError();
    if(pdFAIL == xTaskCreate(vTemplateTask, "Template task3", configMINIMAL_STACK_SIZE + 100, &blue, 3, &vTemplateTask_Handle3)) gotoError();

    // Start the scheduler.
    // Again this function should never return
    vTaskStartScheduler();

    gotoError();

}
