/*
 * PPI.c
 *
 * Created: 2017-04-21 14:47:39
 *  Author: Sebastian Hoggard
 */

#include "PPI.h"

// Databus pins data_pins[i] is bit i
ioport_pin_t data_pins[] = {PIO_PC16_IDX, PIO_PC14_IDX, PIO_PC12_IDX, PIO_PB14_IDX};
//ioport_pin_t data_pins[] = {PIO_PA15_IDX, PIO_PD1_IDX, PIO_PD3_IDX, PIO_PD9_IDX};
// Slave select pins
ioport_pin_t ss_pins[] = {PIO_PC9_IDX};
// Slave select pin
ioport_pin_t ss_pin = PIO_PC9_IDX;
// Acknowledge pin
ioport_pin_t ack_pin = PIO_PA20_IDX;
//ioport_pin_t ack_pin = PIO_PD10_IDX;
// Data direction pin
ioport_pin_t dd_pin = PIO_PC18_IDX;

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



void PPI_init(role_t instance_role, xQueueHandle *semaphore) {

    pSem = semaphore;
    role = instance_role;

    for(int i = 0; i < 4; ++i) {
        ioport_set_pin_dir(data_pins[i], IOPORT_DIR_INPUT);
        ioport_set_pin_mode(data_pins[i], IOPORT_MODE_PULLUP);
    }

    if(role == role_master) {
        for(int i = 0; i < 1; ++i) {
            Pio *pio_base = arch_ioport_pin_to_base(ss_pins[i]);
            uint32_t pin_mask = arch_ioport_pin_to_mask(ss_pins[i]);
            uint32_t ID = ioport_pin_to_ID_Port(ss_pins[i]);
            pio_handler_set(pio_base, ID, pin_mask, PIO_IT_EDGE, PPI_ISR);

            ioport_set_pin_dir(ss_pins[i], IOPORT_DIR_INPUT);
            ioport_set_pin_mode(ss_pins[i], IOPORT_MODE_PULLUP);

        }
    } else {
        Pio *pio_base = arch_ioport_pin_to_base(ss_pin);
        uint32_t pin_mask = arch_ioport_pin_to_mask(ss_pin);
        uint32_t ID = ioport_pin_to_ID_Port(ss_pin);
        pio_handler_set(pio_base, ID, pin_mask, PIO_IT_EDGE, PPI_ISR);

        ioport_set_pin_dir(ss_pin, IOPORT_DIR_INPUT);
        ioport_set_pin_mode(ss_pin, IOPORT_MODE_PULLUP);
    }

    {
        Pio *pio_base = arch_ioport_pin_to_base(ack_pin);
        uint32_t pin_mask = arch_ioport_pin_to_mask(ack_pin);
        uint32_t ID = ioport_pin_to_ID_Port(ack_pin);
        pio_handler_set(pio_base, ID, pin_mask, (PIO_IT_EDGE), PPI_ISR);

        ioport_set_pin_dir(ack_pin, IOPORT_DIR_INPUT);
        ioport_set_pin_mode(ack_pin, IOPORT_MODE_PULLUP);
    }

    {
        ioport_set_pin_dir(dd_pin, IOPORT_DIR_INPUT);
        ioport_set_pin_mode(dd_pin, IOPORT_MODE_PULLUP);
    }

}


void master_send(uint32_t send_slave_id,
                    uint8_t *send_data_begin,
                    uint8_t *send_data_end,
                    uint32_t *flag_done) {

    slave_id = send_slave_id;
    data_begin = send_data_begin;
    data_end = send_data_end;
    flag = flag_done;
    *flag = 0;

    for(uint32_t i = 0; i < 4; ++i) {
        ioport_set_pin_dir(data_pins[i], IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(data_pins[i], ((*data_begin)>>i) & 0x1);
    }
    ++data_begin;

    ioport_set_pin_dir(dd_pin, IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(dd_pin, MOSI);

    Pio *pio_base = arch_ioport_pin_to_base(ack_pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(ack_pin);
    IRQn_Type IRQn = ioport_pin_to_Port_IRQn(ack_pin);
    NVIC_ClearPendingIRQ(IRQn);
    pio_enable_interrupt(pio_base, pin_mask);

    NVIC_SetPriority(IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(IRQn);

    ioport_set_pin_dir(ss_pins[slave_id], IOPORT_DIR_OUTPUT);
    ioport_set_pin_level(ss_pins[slave_id], IOPORT_PIN_LEVEL_LOW);

}

void slave_recieve(uint8_t *send_data_begin,
                    uint8_t *send_data_end,
                    uint32_t *flag_done) {
    data_begin = send_data_begin;
    data_end = send_data_end;
    tmp = send_data_begin;
    flag = flag_done;
    *flag = 0;

    Pio *pio_base = arch_ioport_pin_to_base(ss_pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(ss_pin);
    IRQn_Type IRQn = ioport_pin_to_Port_IRQn(ss_pin);

    NVIC_ClearPendingIRQ(IRQn);
    pio_enable_interrupt(pio_base, pin_mask);
    NVIC_SetPriority(IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(IRQn);

}

static void PPI_ISR_master_send() {
    Pio *pio_base = arch_ioport_pin_to_base(ack_pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(ack_pin);
    pio_disable_interrupt(pio_base, pin_mask);
    if(data_begin != data_end) {
        ioport_set_pin_dir(ss_pins[slave_id], IOPORT_DIR_INPUT);

        for(uint32_t i = 0; i < 4; ++i) {
            ioport_set_pin_level(data_pins[i], ((*data_begin)>>i) & 0x1);
        }
        ++data_begin;

        pio_enable_interrupt(pio_base, pin_mask);

        ioport_set_pin_dir(ss_pins[slave_id], IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(ss_pins[slave_id], IOPORT_PIN_LEVEL_LOW);
      } else {
        *flag = 1;
        ioport_set_pin_dir(ss_pins[slave_id], IOPORT_DIR_INPUT);
        ioport_set_pin_dir(dd_pin, IOPORT_DIR_INPUT);
        for(uint32_t i = 0; i < 4; ++i) {
            ioport_set_pin_dir(data_pins[i], IOPORT_DIR_INPUT);
        }
        xSemaphoreGiveFromISR(*pSem, NULL);
    }
}


static void PPI_ISR_slave_recieve() {
    Pio *pio_base = arch_ioport_pin_to_base(ss_pin);
    uint32_t pin_mask = arch_ioport_pin_to_mask(ss_pin);
    pio_disable_interrupt(pio_base, pin_mask);

    if(data_begin != data_end) {
        uint8_t data = 0;
        for(uint32_t i = 0; i < 4; ++i) {
            data |= (ioport_get_pin_level(data_pins[i])<<i);
        }
        *data_begin = data;
        ++data_begin;
        pio_enable_interrupt(pio_base, pin_mask);
        ioport_set_pin_dir(ack_pin, IOPORT_DIR_OUTPUT);
        ioport_set_pin_level(ack_pin, IOPORT_PIN_LEVEL_LOW);
        delay_us(10);
        ioport_set_pin_dir(ack_pin, IOPORT_DIR_INPUT);
    }
    if(data_begin == data_end) {
        *flag = 1;
        xSemaphoreGiveFromISR(*pSem, NULL);
        //printf("ISR: %u %u\n", ((uint8_t) *tmp), ((uint8_t) *(tmp+1)))  ;
    }
}

void PPI_ISR(const uint32_t id, const uint32_t index) {

    if ((id == ioport_pin_to_ID_Port(ack_pin))
            && (index == ioport_pin_to_mask(ack_pin))
            && (IOPORT_PIN_LEVEL_LOW == ioport_get_pin_level(ack_pin))
            && (role == role_master)) {
        PPI_ISR_master_send();
    } else if((id == ioport_pin_to_ID_Port(ss_pin))
                && (index == ioport_pin_to_mask(ss_pin))
                && (IOPORT_PIN_LEVEL_LOW == ioport_get_pin_level(ss_pin))
                && (role == role_slave)) {

        PPI_ISR_slave_recieve();
    }
}

