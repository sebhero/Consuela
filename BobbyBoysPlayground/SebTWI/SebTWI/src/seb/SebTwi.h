//
// Created by trevl on 2017-05-03.
//

#ifndef DUEIO_SEBTWI_H
#define DUEIO_SEBTWI_H

#include <stdint.h>
#include "asf.h"

typedef volatile const uint32_t RoReg; /**< Read only 32-bit register (volatile const unsigned int) */
typedef volatile       uint32_t WoReg; /**< Write only 32-bit register (volatile unsigned int) */
typedef volatile       uint32_t RwReg; /**< Read-Write 32-bit register (volatile unsigned int) */




/* Returns 1 if the TXRDY bit (ready to transmit data) is set in the given status register value.*/
#define TWI_STATUS_TXRDY(status) (((status) & TWI_SR_TXRDY) == TWI_SR_TXRDY)

/* Returns 1 if the RXRDY bit (ready to receive data) is set in the given status register value.*/
#define TWI_STATUS_RXRDY(status) (((status) & TWI_SR_RXRDY) == TWI_SR_RXRDY)

/* Returns 1 if the TXCOMP bit (transfer complete) is set in the given status register value.*/
#define TWI_STATUS_TXCOMP(status) (((status) & TWI_SR_TXCOMP) == TWI_SR_TXCOMP)

extern void TWI_ConfigureMaster(Twi *pTwi, uint32_t twck, uint32_t mck);

extern void TWI_SetClock( Twi *pTwi, uint32_t dwTwCk, uint32_t dwMCk );

extern void TWI_ConfigureSlave(Twi *pTwi, uint8_t slaveAddress);

extern void TWI_Disable(Twi *pTwi);

extern void TWI_Stop(Twi *pTwi);

extern void TWI_StartRead(
        Twi *pTwi,
        uint8_t address,
        uint32_t iaddress,
        uint8_t isize);

extern uint8_t TWI_ReadByte(Twi *pTwi);

extern void TWI_WriteByte(Twi *pTwi, uint8_t byte);

extern void TWI_StartWrite(
        Twi *pTwi,
        uint8_t address,
        uint32_t iaddress,
        uint8_t isize,
        uint8_t byte);

extern uint8_t TWI_ByteReceived(Twi *pTwi);

extern uint8_t TWI_ByteSent(Twi *pTwi);

extern uint8_t TWI_TransferComplete(Twi *pTwi);

extern void TWI_EnableIt(Twi *pTwi, uint32_t sources);

extern void TWI_DisableIt(Twi *pTwi, uint32_t sources);

extern uint32_t TWI_GetStatus(Twi *pTwi);

extern uint32_t TWI_GetMaskedStatus(Twi *pTwi);

extern void TWI_SendSTOPCondition(Twi *pTwi);

#endif //DUEIO_SEBTWI_H
