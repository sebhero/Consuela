#include <asf.h>

ioport_pin_t nibble[] = {PIO_PC16_IDX, PIO_PC14_IDX, PIO_PC12_IDX, PIO_PB14_IDX};
ioport_pin_t leds[] = {PIO_PA15_IDX,PIO_PD1_IDX,PIO_PD3_IDX, PIO_PD9_IDX};

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


static void evt_handler(const uint32_t id, const uint32_t index) {
    if ((id == ioport_pin_to_ID_Port(PIO_PC18_IDX)) && (index == ioport_pin_to_mask(PIO_PC18_IDX))) {
        for(uint32_t i = 0; i < 4; ++i) {
            ioport_set_pin_level(leds[i], ioport_get_pin_level(nibble[i]));
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



typedef struct {
    uint32_t delay_ms;
    ioport_pin_t pin;
} vBlinkParam_t;

vBlinkParam_t red = {.delay_ms = 100, .pin = PIO_PD3_IDX};
vBlinkParam_t yellow = {.delay_ms = 200,.pin = PIO_PD1_IDX};
vBlinkParam_t green = {.delay_ms = 300,.pin = PIO_PA15_IDX};

void vBlink(void *pvParam) {
    uint32_t delay_ms = ((vBlinkParam_t *)pvParam)->delay_ms;
    ioport_pin_t pin = ((vBlinkParam_t *)pvParam)->pin;
    ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_LOW);
    for(;;) {
        ioport_toggle_pin_level(pin);
        vTaskDelay(pdMSTOTICKS(delay_ms));
    }
    vTaskDelete(NULL);
}


void config_slave() {
    for(uint32_t i = 0; i < 4; ++i) {
        ioport_set_pin_dir(leds[i], IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(leds[i], IOPORT_PIN_LEVEL_LOW);
        ioport_set_pin_dir(nibble[i], IOPORT_DIR_INPUT);
        ioport_set_pin_mode(nibble[i], IOPORT_MODE_PULLUP);
    }

    configure_button(PIO_PC18_IDX, evt_handler, (PIO_DEBOUNCE|PIO_PULLUP),  PIO_IT_RISE_EDGE);
}

void master() {


    for(uint32_t i = 0; i < 4; ++i) {
        ioport_set_pin_dir(nibble[i], IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(nibble[i], IOPORT_PIN_LEVEL_LOW);
        //ioport_set_pin_mode(nibble[i], IOPORT_MODE_PULLUP);
    }


    ioport_set_pin_dir(PIO_PC18_IDX, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(PIO_PC18_IDX, IOPORT_PIN_LEVEL_LOW);

    uint32_t cntr = 0;
    for(;;) {

        ioport_set_pin_level(nibble[0], cntr & 0x1);
        ioport_set_pin_level(nibble[1], cntr & 0x2);
        ioport_set_pin_level(nibble[2], cntr & 0x4);
        ioport_set_pin_level(nibble[3], cntr & 0x8);
        ioport_set_pin_level(PIO_PC18_IDX, IOPORT_PIN_LEVEL_HIGH);
        delay_ms(1);
        ioport_set_pin_level(PIO_PC18_IDX, IOPORT_PIN_LEVEL_LOW);

        ++cntr;
        cntr = cntr %  16;

        delay_ms(1000);
    }
}


int main() {
	sysclk_init();
	board_init();

    config_slave();

    //master();

    for(;;) {
        // Spin
    }

}



int main_temp (void) {
	sysclk_init();
	board_init();
    xTaskCreate(vBlink, "Red", configMINIMAL_STACK_SIZE + 100, &red, 4,NULL);
    xTaskCreate(vBlink, "Yellow", configMINIMAL_STACK_SIZE + 100, &yellow, 3,NULL);
    xTaskCreate(vBlink, "Green", configMINIMAL_STACK_SIZE + 100, &green, 2,NULL);
    vTaskStartScheduler();
    for(;;) {
    }
}
