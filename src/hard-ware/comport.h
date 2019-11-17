
#ifndef SOS_COMPORT_H
#define SOS_COMPORT_H

#include "base/port.h"

#define COM1_ADDR 0x3f8
#define COM2_ADDR 0x2f8
#define COM3_ADDR 0x3e8
#define COM4_ADDR 0x2e8

class ComPort {
public:
    explicit ComPort(uint16_t com);

    void init();
    void write(char c);
    void write(char *s);

    int isTransmitEmpty();

private:
    uint16_t _comPort;
};


#endif //SOS_COMPORT_H
