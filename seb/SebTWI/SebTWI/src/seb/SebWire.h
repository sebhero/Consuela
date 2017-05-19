//
// Created by Sebastian Boreback on 2017-05-03.
// based of arduino studio Wire libary
//

#ifndef SEBWIRE_H
#define SEBWIRE_H

//#include <Stream.h>
#include "SebTwi.h"
#include <string.h>

#define BUFFER_LENGTH 32
/** Master clock frequency */
#define VARIANT_MCK			84000000

//wire has end means wire has end()
#define WIRE_HAS_END 1

//extern SebWire Wire;

class SebWire{
public:

    SebWire(Twi* pTwi, void(*pBegin_cb)(void), void(*pEnd_cb)(void));

    //portNr 0 or 1 if its SDA0 or SDA1 arduino SDA0 or SDA1
    void initIOPort(uint8_t portNr);
    void disableIOPort(uint8_t portNr);

    void begin();

    void begin(uint8_t);

    //void begin(int);

    void end();

    void setClock(uint32_t);
    void beginTransmission(uint8_t);
    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
    //uint8_t requestFrom(int, int);
    //uint8_t requestFrom(int, int, int);
     size_t write(uint8_t);
    size_t write(const uint8_t *, size_t);
    int available(void);
    int read(void);
    int peek(void);
    void flush(void);
    void onReceive(void(*)(int));
    void onRequest(void(*)(void));

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    size_t write(const char *str) {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
    }
    //using Print::write;

    void onService(void);

    static SebWire* getInstance();

private:

    static SebWire* pInstance;

    uint8_t portNR;

    //RX Buffer
    uint8_t rxBuffer[BUFFER_LENGTH];
    uint8_t rxBufferIndex;
    uint8_t rxBufferLength;

    //TX Buffer
    uint8_t txAddress;
    uint8_t txBuffer[BUFFER_LENGTH];
    uint8_t txBufferLength;

    //Service Buffer
    uint8_t srvBuffer[BUFFER_LENGTH];
    uint8_t srvBufferIndex;
    uint8_t srvBufferLength;

    //callback user functions
    void (*onRequestCallback)(void);
    void(*onReciveCallback)(int);

    //called before init
    void (*onBeginCallback)(void);

    //called after define
    void (*onEndCallback)(void);

    //the Twi instance
    Twi* pTwi;

    //TWI state
    enum TWI_state{
        UNINITIALIZED,
        MASTER_IDLE,
        MASTER_SEND,
        MASTER_RECV,
        SLAVE_IDLE,
        SLAVE_RECV,
        SLAVE_SEND
    };
    TWI_state status;

    //TWI clock freq
    static const uint32_t TWI_CLOCK=100000;
    uint32_t twiClock;

    //timeouts
    static const uint32_t RECV_TIMEOUT=100000;
    static const uint32_t XMIT_TIMEOUT=100000;

    //init begin for master and slave
    void initBegin();
};


#endif //SEBWIRE_H
