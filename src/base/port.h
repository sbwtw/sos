
#ifndef __PORT_H_
#define __PORT_H_

#include "std/types.h"

/**
 * 端口输入/输出实现
 *
 * Port<Type> 实现了端口的直接读取和写入
 * PortSlow<Type> 实现了端口直接读取和带延迟的写入
 * */

template <typename Type>
class Port
{
public:
    explicit Port(uint16_t port_number);
    virtual ~Port();

    uint16_t port() const { return _portNumber; }
    virtual void write(Type data);
    virtual Type read();

protected:
    uint16_t _portNumber;
};

template<typename Type>
Port<Type>::Port(uint16_t port_number)
    : _portNumber(port_number)
{
}

template<typename Type>
Port<Type>::~Port() = default;

template <typename Type>
class PortSlow : public Port<Type>
{
public:
    explicit PortSlow(uint16_t port_number);
    virtual ~PortSlow();

    virtual void write(Type data);
};

template <typename Type>
PortSlow<Type>::PortSlow(uint16_t port_number)
    : Port<Type>(port_number) {
}

template <typename Type>
PortSlow<Type>::~PortSlow() = default;

typedef Port<uint8_t> Port8Bit;
typedef Port<uint16_t> Port16Bit;
typedef Port<uint32_t> Port32Bit;

typedef PortSlow<uint8_t> Port8BitSlow;
typedef PortSlow<uint16_t> Port16BitSlow;

#endif // __PORT_H_
