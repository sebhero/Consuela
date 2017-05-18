/*
 * PPI.h
 *
 * Created: 2017-04-21 14:15:34
 *  Author: Sebastian Hoggard
 */


#ifndef PPI_H_
#define PPI_H_

#include <asf.h>

typedef enum {
    role_master,
    role_slave
} role_t;

typedef enum {
    MOSI,
    MISO
} data_dir_t;

uint32_t slave_id;
uint8_t *data_begin;
uint8_t *data_end;
uint32_t *flag;
role_t role;

xQueueHandle *pSem;

uint8_t *tmp;

void PPI_init(role_t instance_role, xQueueHandle *semaphore);
void master_send(uint32_t slave_id, uint8_t *send_data_begin, uint8_t *send_data_end, uint32_t *flag_done);
void PPI_ISR(const uint32_t id, const uint32_t index);

void slave_recieve(uint8_t *send_data_begin, uint8_t *send_data_end, uint32_t *flag_done);



#endif /* PPI_H_ */