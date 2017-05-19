//
// Created by trevl on 2017-05-03.
//




//#include <Arduino.h>
#include "SebWire.h"
//#include <string.h>





static inline bool TWI_FailedAcknowledge(Twi *pTwi) {
    return pTwi->TWI_SR & TWI_SR_NACK;
}

static inline bool TWI_WaitTransferComplete(Twi *pTwi, uint32_t timeout) {
    uint32_t status_reg = 0;
    while ((status_reg & TWI_SR_TXCOMP) != TWI_SR_TXCOMP) {
        status_reg = TWI_GetStatus(pTwi);

        //got NACK
        if (status_reg & TWI_SR_NACK) {
            return false;
        }

        //timed out
        if (--timeout == 0) {
            return false;
        }
    }
    return true;
}

static inline bool TWI_WaitByteSent(Twi *pTwi, uint32_t timeout) {
    uint32_t status_reg = 0;
    while ((status_reg & TWI_SR_TXRDY) != TWI_SR_TXRDY) {
        status_reg = TWI_GetStatus(pTwi);

        if (status_reg & TWI_SR_NACK) {
            return false;
        }

        if (--timeout == 0) {
            return false;
        }
    }
    return true;
}


static inline bool TWI_WaitByteRecived(Twi *pTwi, uint32_t timeout) {
    uint32_t status_reg = 0;

    for (int i = timeout; i > 0; --i) {
        status_reg = TWI_GetStatus(pTwi);
        //todo
        //kanske if (status_reg & TWI_SR_RXRDY) {
        if ((status_reg & TWI_SR_RXRDY) == TWI_SR_RXRDY) {
            return true;
        }

        if (status_reg & TWI_SR_NACK) {
            return false;
        }
    }
    return false;

//        while ((status_reg & TWI_SR_RXRDY) != TWI_SR_RXRDY) {
//            status_reg = TWI_GetStatus(pTwi);
//
//            if (status_reg & TWI_SR_NACK)
//                return false;
//
//            if (--timeout == 0)
//                return false;
//        }
//
//        return true;
}

static inline bool TWI_STATUS_SVREAD(uint32_t status) {
    return (status & TWI_SR_SVREAD) == TWI_SR_SVREAD;
}

static inline bool TWI_STATUS_SVACC(uint32_t status) {
    return (status & TWI_SR_SVACC) == TWI_SR_SVACC;
}

static inline bool TWI_STATUS_GACC(uint32_t status) {
    return (status & TWI_SR_GACC) == TWI_SR_GACC;
}

static inline bool TWI_STATUS_EOSACC(uint32_t status) {
    return (status & TWI_SR_EOSACC) == TWI_SR_EOSACC;
}

static inline bool TWI_STATUS_NACK(uint32_t status) {
    return (status & TWI_SR_NACK) == TWI_SR_NACK;
}



SebWire::SebWire(Twi *pTwi, void (*pBegin_cb)(void), void (*pEnd_cb)(void)) :
        pTwi(pTwi), rxBufferIndex(0), rxBufferLength(0), txAddress(0),
        txBufferLength(0), srvBufferIndex(0), srvBufferLength(0), status(UNINITIALIZED),
        onBeginCallback(pBegin_cb), onEndCallback(pEnd_cb), twiClock(TWI_CLOCK) {

}

//turn off UART
void SebWire::initBegin(void) {
    if (onBeginCallback) {
        onBeginCallback();
    }

    //disable PDC channel
    this->pTwi->TWI_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
}

//start master
void SebWire::begin(void) {
    initBegin();

    TWI_ConfigureMaster(pTwi, twiClock, VARIANT_MCK);
    this->status = MASTER_IDLE;
}

//start slave
void SebWire::begin(uint8_t address) {
    initBegin();

    TWI_ConfigureSlave(pTwi, address);
    this->status = SLAVE_IDLE;
    TWI_EnableIt(pTwi, TWI_IER_SVACC);
}

//turn off TWI and enable UART
void SebWire::end(void) {
    TWI_Disable(pTwi);

    //enable PDC channel
    pTwi->TWI_PTCR &= ~(UART_PTCR_RXTDIS | UART_PTCR_TXTDIS);

    if (onEndCallback) {
        onEndCallback();
    }
}

void SebWire::setClock(uint32_t frequency) {
    twiClock = frequency;
    TWI_SetClock(pTwi, twiClock, VARIANT_MCK);

}

uint8_t SebWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) {
    if (quantity > BUFFER_LENGTH) {
        quantity = BUFFER_LENGTH;
    }

    //perform blocking read into buffer
    uint8_t readed = 0;
    TWI_StartRead(pTwi, address, iaddress, isize);

    //read bytes while less than bufflength or if its more
    do {

        //stop condition must be set during the reception of last byte
        if (readed + 1 == quantity) {
            TWI_SendSTOPCondition(pTwi);
        }

        if (TWI_WaitByteRecived(pTwi, RECV_TIMEOUT)) {
            rxBuffer[readed++] = TWI_ReadByte(pTwi);
        } else {
            //when timeout or got nack
            break;
        }
    } while (readed < quantity);

    TWI_WaitTransferComplete(pTwi, RECV_TIMEOUT);

    //set rx buffer iterator vars
    //todo why reset rxbufferindex
    this->rxBufferIndex = 0; //due to SebWire::read increases indx
    this->rxBufferLength = readed;

    return readed;
}

uint8_t SebWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint32_t) 0, (uint8_t) 0, (uint8_t) sendStop);
}

uint8_t SebWire::requestFrom(uint8_t address, uint8_t quantity) {
    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}

//uint8_t SebWire::requestFrom(int address, int quantity) {
//    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
//}

//uint8_t SebWire::requestFrom(int address, int quantity, int sendStop) {
//    return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) sendStop);
//}

void SebWire::beginTransmission(uint8_t address) {
    this->status = MASTER_SEND;

    //save address of target and empty buffer
    this->txAddress = address;
    this->txBufferLength = 0;
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t SebWire::endTransmission(uint8_t sendStop) {
    //todo OMG they are using int for error msg!
    uint8_t error = 0;
    //transmit buffer (blcoking)
    TWI_StartWrite(pTwi, txAddress, 0, 0, txBuffer[0]);

    if (!TWI_WaitByteSent(pTwi, XMIT_TIMEOUT)) {
        error = 2;//error got nack on address transmit
    }

    if (error == 0) {
        uint16_t sent = 1;
        while (sent < txBufferLength) {
            TWI_WriteByte(pTwi, txBuffer[sent++]);

            if (!TWI_WaitByteSent(pTwi, XMIT_TIMEOUT)) {
                error = 3;// error got nack during data transmit
            }
        }
    }

    if (error == 0) {
        TWI_Stop(pTwi);
        if (!TWI_WaitTransferComplete(pTwi, XMIT_TIMEOUT)) {
            error = 4; //error finishing up
        }
    }

    this->txBufferLength = 0; //empty buffer;
    this->status = MASTER_IDLE;
    return error;
}


//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
uint8_t SebWire::endTransmission() {
    return endTransmission(1);//was true
}

size_t SebWire::write(uint8_t data) {

    if (this->status == MASTER_SEND) {
        if (txBufferLength >= BUFFER_LENGTH) {
            return 0;//ooh i see zero
        }
        this->txBuffer[txBufferLength] = data;
        return 1;
    } else {
        if (srvBufferLength >= BUFFER_LENGTH) {
            return 0;
        }
        srvBuffer[srvBufferLength++] = data;
        return 1;
    }

}

size_t SebWire::write(const uint8_t *data, size_t quantity) {
    if (status == MASTER_SEND) {
        for (size_t i = 0; i < quantity; ++i) {
            if (txBufferLength >= BUFFER_LENGTH) {
                //return data sent
                return i;
            }
            txBuffer[txBufferLength++] = data[i];
        }
    } else {
        for (size_t i = 0; i < quantity; ++i) {
            if (srvBufferLength >= BUFFER_LENGTH) {
                //return data sent
                return i;
            }
            srvBuffer[srvBufferLength++] = data[i];
        }
    }
    //return data sent
    return quantity;
}

int SebWire::available() {
    return rxBufferLength - rxBufferIndex;
}

//read buff and increase indx
int SebWire::read() {
    if (rxBufferIndex < rxBufferLength) {
        return rxBuffer[rxBufferIndex++];
    }
    return -1;
}

int SebWire::peek() {
    if (rxBufferIndex < rxBufferLength) {
        return rxBuffer[rxBufferIndex];
    }
    return -1;
}

void SebWire::flush() {
    // Do nothing, use endTransmission(..) to force
    // data transfer.
}

//bind pointer to function receive function
void SebWire::onReceive(void (*function)(int)) {
    this->onReciveCallback = function;
}

//bind pointer to function request function
void SebWire::onRequest(void (*function)(void)) {
    this->onRequestCallback = function;
}

void SebWire::onService() {
    //retrive intrerrupt status
    uint32_t sr = TWI_GetStatus(pTwi);

    if (this->status == SLAVE_IDLE && TWI_STATUS_SVACC(sr)) {
        //disable interrupt
        TWI_DisableIt(pTwi, TWI_IDR_SVACC);
        TWI_EnableIt(pTwi, TWI_IER_RXRDY | TWI_IER_GACC | TWI_IER_NACK
                           | TWI_IER_EOSACC | TWI_IER_SCL_WS | TWI_IER_TXCOMP);

        this->srvBufferLength=0;
        this->srvBufferIndex=0;

        //dectect if we should go into ReCiVe or SEND status
        //SVREAD == 1 means *master reading -> slave_send
        if (!TWI_STATUS_SVREAD(sr)) {
            status = SLAVE_RECV;
        } else {
            status = SLAVE_SEND;

            //alert calling program to generate a response ASAP
            if (onRequestCallback) {
                onRequestCallback();
            } else {
                //create a default 1-byte response
                write((uint8_t)0);
            }
        }
    }

    if (status != SLAVE_IDLE && TWI_STATUS_EOSACC(sr)) {
        if (status == SLAVE_RECV && onReciveCallback) {
            //copy data into rxBuffer
            //(allows to recive another packet while the
            //user programs reads actual data)
            for (uint8_t i = 0; i < srvBufferLength; ++i) {
                rxBuffer[i] = srvBuffer[i];
            }

            //reset rxbuffer idx
            this->rxBufferIndex=0;
            this->rxBufferLength=srvBufferLength;

            //alert calling program with size of new data
            onReciveCallback(rxBufferLength);
        }

        //transfer completed reset slave to idle status
        TWI_EnableIt(pTwi, TWI_SR_SVACC);
        TWI_DisableIt(pTwi, TWI_IDR_RXRDY | TWI_IDR_GACC | TWI_IDR_NACK
                            | TWI_IDR_EOSACC | TWI_IDR_SCL_WS | TWI_IER_TXCOMP);
        this->status=SLAVE_IDLE;
    }

    if (status == SLAVE_RECV) {
        if (TWI_STATUS_RXRDY(sr)) {
            if (srvBufferLength < BUFFER_LENGTH) {
                //Todo increases srvbufferLength
                srvBuffer[srvBufferLength++] = TWI_ReadByte(pTwi);
            }
        }
    }

    if (status == SLAVE_SEND) {
        if (TWI_STATUS_TXRDY(sr) && !TWI_STATUS_NACK(sr)) {
            uint8_t c= 'x';//120
            if (srvBufferIndex < srvBufferLength) {
                c = srvBuffer[srvBufferIndex++];//todo increase srvBuffIdx
            }
            //send what is in srvBuffer at Idx or the int value of x *120
            TWI_WriteByte(pTwi, c);
        }
    }
}



static void Wire_Init(void) {
    pmc_enable_periph_clk(WIRE1_INTERFACE_ID);
//     PIO_Configure(
//             g_APinDescription[PIN_WIRE1_SDA].pPort,
//             g_APinDescription[PIN_WIRE1_SDA].ulPinType,
//             g_APinDescription[PIN_WIRE1_SDA].ulPin,
//             g_APinDescription[PIN_WIRE1_SDA].ulPinConfiguration);
//     PIO_Configure(
//             g_APinDescription[PIN_WIRE1_SCL].pPort,
//             g_APinDescription[PIN_WIRE1_SCL].ulPinType,
//             g_APinDescription[PIN_WIRE1_SCL].ulPin,
//             g_APinDescription[PIN_WIRE1_SCL].ulPinConfiguration);

    NVIC_DisableIRQ(WIRE1_ISR_ID);
    NVIC_ClearPendingIRQ(WIRE1_ISR_ID);
    NVIC_SetPriority(WIRE1_ISR_ID, 0);
    NVIC_EnableIRQ(WIRE1_ISR_ID);
}

static void Wire_Deinit(void) {
    NVIC_DisableIRQ(WIRE1_ISR_ID);
    NVIC_ClearPendingIRQ(WIRE1_ISR_ID);

    pmc_disable_periph_clk(WIRE1_INTERFACE_ID);

    // no need to undo PIO_Configure,
    // as Peripheral A was enable by default before,
    // and pullups were not enabled
}
//todo wtf
SebWire Wire = SebWire(WIRE1_INTERFACE, Wire_Init, Wire_Deinit);
//todo more wtf..ok its the interrupt but WIRE?!?
void WIRE_ISR_HANDLER(void) {
    Wire.onService();
}



























