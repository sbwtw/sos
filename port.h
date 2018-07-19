
#ifndef __PORT_H_
#define __PORT_H_

#include "types.h"

class Port
{
protected:
    Port(uint16_t port_number);
    ~Port();

protected:
    uint16_t portNumber;
};

class Port8Bit : public Port
{
public:
    Port8Bit(uint16_t port_number);
    ~Port8Bit();

    virtual void write(uint8_t data);
    virtual uint8_t read();
};

class Port8BitSlow : public Port8Bit
{
public:
    Port8BitSlow(uint16_t port_number);
    ~Port8BitSlow();

    virtual void write(uint8_t data);
};

class Port16Bit : public Port
{
public:
    Port16Bit(uint32_t port_number);
    ~Port16Bit();

    virtual void write(uint16_t data);
    virtual uint16_t read();
};

class Port32Bit : public Port
{
public:
    Port32Bit(uint64_t port_number);
    ~Port32Bit();

    virtual void write(uint32_t data);
    virtual uint32_t read();
};

#endif // __PORT_H_