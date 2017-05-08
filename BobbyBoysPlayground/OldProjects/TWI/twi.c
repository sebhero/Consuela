/*
* twi.c
*
*  Author: viktor, erik, jelena, axel
*/

#include <asf.h>
#include <indirections.h>
#include "twi.h"
#include "twi_defs.h"

void configure_twi() {
    pmc_enable_periph_clk(ID_TWI1);
    twi_slave_init(TWI1, SLAVE_ADDRESS);
    twi_read_byte(TWI1); // Clear rx-buffer/reset flags
    NVIC_DisableIRQ(TWI1_IRQn);
    NVIC_ClearPendingIRQ(TWI1_IRQn);
    NVIC_SetPriority(TWI1_IRQn, 0);
    NVIC_EnableIRQ(TWI1_IRQn);
    twi_enable_interrupt(TWI1, TWI_IER_SVACC);
}

void TWI1_Handler() {
    
    static uint32_t status = 0;
    status = twi_get_interrupt_status(TWI1);

    if(status & TWI_SR_SVACC) {
        // Slave access
        twi_disable_interrupt(TWI1, TWI_IDR_SVACC);
        twi_enable_interrupt(TWI1, TWI_IER_RXRDY | TWI_IER_NACK | TWI_IER_EOSACC | TWI_IER_SCL_WS);
        if(status & TWI_SR_GACC) {
            // General call, do not handle
            twi_enable_interrupt(TWI1, TWI_SR_SVACC);
            twi_disable_interrupt(TWI1,	TWI_IDR_RXRDY | TWI_IDR_GACC | TWI_IDR_NACK | TWI_IDR_EOSACC | TWI_IDR_SCL_WS);
        }
        else {
            if(status & TWI_SR_SVREAD) {
                // Slave read (i.e. send data from slave to master)
                if(status & TWI_SR_TXRDY) {
                    // Transmission buffer (TWI_THR) is empty and/or last written byte has been sent
                    tx_event = 1;
                    twi_write_byte(TWI1,tx_buffer[tx_buffer_idx]);
                    ++tx_buffer_idx;
                    tx_buffer_idx = tx_buffer_idx % tx_buffer_length;
                }
                else {
                    // Nothing
                }
            }
            else {
                // Not slave read (i.e. receive data from master to slave)
                if(status & TWI_SR_RXRDY) {
                    // Byte in buffer (TWI_RHR)
                    rx_event = 1;
                    rx_buffer[rx_buffer_idx] = twi_read_byte(TWI1);
                    ++rx_buffer_idx; 
                    rx_buffer_idx = rx_buffer_idx % rx_buffer_length;
                }
                else {
                    // Nothing
                }
            }
        }
    }
    else {
        if(status & TWI_SR_EOSACC) {
            // End of slave access
            if(status & TWI_SR_TXCOMP) {
                // Transmission complete
                if(rx_event ) {
					// End of slave read (i.e. receive data from master to slave)
					// Use the data sent from the master as the memory address (which sensor)
					// Read from that memory address and put the first byte into the TWI transmission buffer
					// the TWI hw will send this byte when the master initiates a read from the slave
                    rx_event = 0;
                    rx_buffer_idx = 0;
					tx_buffer[0] = rx_buffer[0];
					tx_buffer[1] = rx_buffer[1];
                    //printf("%X %X\n", rx_buffer[0], rx_buffer[1]);
					if((rx_buffer[0] == button_1_reg[0]) && (rx_buffer[1] == button_1_reg[1])) {
						tx_buffer[2] = button_1_mem[2]; //(uint8_t) (trng_read_output_data(TRNG) & 0XFF);		
						tx_buffer[3] = 0;	
						tx_buffer[4] = 0;	
						tx_buffer[5] = 0;				
						button_1_mem[2] = 0;
					}
					if((rx_buffer[0] == door_reg[0]) && (rx_buffer[1] == door_reg[1])) {
						tx_buffer[2] = door_mem[2]; //(uint8_t) (trng_read_output_data(TRNG) & 0XFF);
						tx_buffer[3] = 0;
						tx_buffer[4] = 0;
						tx_buffer[5] = 0;						
						door_mem[2] = 0;							
						ioport_set_pin_level(LED_GREEN, door_mem[2]?1:0);				
					}
					if((rx_buffer[0] == pool_reg[0]) && (rx_buffer[1] ==  pool_reg[1])) {
						tx_buffer[2] = pool_mem[2]; //(uint8_t) (trng_read_output_data(TRNG) & 0XFF);
						tx_buffer[3] = 0;
						tx_buffer[4] = 0;
						tx_buffer[5] = 0;						
						pool_mem[2] = 0;						
					}
                        
                    tx_buffer[6] = tx_buffer[0] ^ tx_buffer[1] ^ tx_buffer[2] ^ tx_buffer[3] ^ tx_buffer[4] ^ tx_buffer[5];
                    tx_buffer_idx = 0;
                    twi_write_byte(TWI1, tx_buffer[tx_buffer_idx]);
                    ++tx_buffer_idx;
					rx_buffer[0] = 0;
					rx_buffer[1] = 0;
                }
                else if(tx_event) {
					// End of slave write (i.e. send data to master from slave)
					// Just reset the buffer indices and wait for next event
                    tx_event = 0;
                    tx_buffer_idx = 0;
                    rx_buffer_idx = 0;
                }
                twi_enable_interrupt(TWI1, TWI_SR_SVACC);
                twi_disable_interrupt(TWI1,	TWI_IDR_RXRDY | TWI_IDR_GACC | TWI_IDR_NACK | TWI_IDR_EOSACC | TWI_IDR_SCL_WS);
            }
            else {
                // Nothing
            }
        }
        else {
            // Nothing
        }
    }
}