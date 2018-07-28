
#ifndef __PCI_H_
#define __PCI_H_

#include "base/port.h"
#include "hw-abstract/driver.h"

class PeripheralComponentInterconnectDescriptor
{
    friend class PeripheralComponentInterconnectController;

public:
    PeripheralComponentInterconnectDescriptor();

private:
    uint32_t portBase;
    uint32_t interrupt;

    uint16_t bus;
    uint16_t device;
    uint16_t function;

    uint16_t vendorId;
    uint16_t deviceId;

    uint8_t classId;
    uint8_t subclassId;
    uint8_t interfaceId;

    uint8_t revision;
};

class PeripheralComponentInterconnectController
{
public:
    PeripheralComponentInterconnectController();

    bool deviceHasFuncs(uint16_t bus, uint16_t device);
    uint32_t read(uint16_t bus, uint16_t device, uint16_t func, uint32_t reg_offset);
    PeripheralComponentInterconnectDescriptor getDeivceDescriptor(uint16_t bus, uint16_t device, uint16_t func);
    void write(uint16_t bus, uint16_t device, uint16_t func, uint32_t reg_offset, uint32_t value);
    void selectDrivers(DriverManager *driverMgr);

private:
    Port32Bit commandPort;
    Port32Bit dataPort;
};

#endif // __PCI_H_