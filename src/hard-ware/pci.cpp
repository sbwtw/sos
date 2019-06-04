
#include "hard-ware/pci.h"
#include "sos_base.h"

PeripheralComponentInterconnectDescriptor::PeripheralComponentInterconnectDescriptor()
{
}

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
    : commandPort(0xcf8)
    , dataPort(0xcfc)
{

}

uint32_t PeripheralComponentInterconnectController::read(uint16_t bus, uint16_t device, uint16_t func, uint32_t reg_offset)
{
    uint32_t id = 0x1                 << 31 |
                  (bus & 0xff)        << 16 |
                  (device & 0x1f)     << 11 |
                  (func & 0x07)       << 8 |
                  (reg_offset & 0xfc);

    commandPort.write(id);
    uint32_t r = dataPort.read();

    return r >> (8 * (reg_offset % 4));
}

void PeripheralComponentInterconnectController::write(uint16_t bus, uint16_t device, uint16_t func, uint32_t reg_offset, uint32_t value)
{
    uint32_t id = 0x1                 << 31 |
                  (bus & 0xff)        << 16 |
                  (device & 0x1f)     << 11 |
                  (func & 0x07)       << 8 |
                  (reg_offset & 0xfc);

    commandPort.write(id);
    dataPort.write(value);
}

bool PeripheralComponentInterconnectController::deviceHasFuncs(uint16_t bus, uint16_t device)
{
    return read(bus, device, 0, 0x0e) & (1 << 7);
}

void PeripheralComponentInterconnectController::selectDrivers(DriverManager *driverMgr)
{
    for (int bus(0); bus != 8; ++bus)
    {
        for (int device(0); device != 32; ++device)
        {
            const int func_num = deviceHasFuncs(bus, device) ? 8 : 1;
            for (int func(0); func != func_num; ++func)
            {
                auto pci = getDeivceDescriptor(bus, device, func);

                if (pci.vendorId == 0 || pci.vendorId == 0xffff)
                    break;

                printf("PCI: %x %x %x, VENDOR: %x\n", bus, device, func, pci.vendorId);
            }
        }
    }
}

PeripheralComponentInterconnectDescriptor
PeripheralComponentInterconnectController::getDeivceDescriptor(uint16_t bus, uint16_t device, uint16_t func)
{
    PeripheralComponentInterconnectDescriptor r;
    r.bus = bus;
    r.device = device;
    r.function = func;

    r.vendorId = read(bus, device, func, 0x00);
    r.deviceId = read(bus, device, func, 0x02);

    r.classId = read(bus, device, func, 0x0b);
    r.subclassId = read(bus, device, func, 0x0a);
    r.interfaceId = read(bus, device, func, 0x09);

    r.revision = read(bus, device, func, 0x08);
    r.interrupt = read(bus, device, func, 0x3c);

    return r;
}
