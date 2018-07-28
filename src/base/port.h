
#ifndef __PORT_H_
#define __PORT_H_

#include "types.h"

template <typename Type>
class Port
{
public:
    Port(uint16_t port_number);
    virtual ~Port();

    void write(Type data);
    Type read();

protected:
    uint16_t portNumber;
};

template<typename Type>
Port<Type>::Port(uint16_t port_number)
    : portNumber(port_number)
{
}

template<typename Type>
Port<Type>::~Port()
{
}

typedef Port<uint8_t> Port8Bit;
typedef Port<uint16_t> Port16Bit;
typedef Port<uint32_t> Port32Bit;

typedef Port8Bit Port8BitSlow;

#endif // __PORT_H_