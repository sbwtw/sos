
#include "ata.h"
#include "sos_io.h"

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(uint16_t port_base, bool master)
    : master(master)
    , dataPort(port_base)
    , errorPort(port_base + 1)
    , sectorCountPort(port_base + 2)
    , lbaLowPort(port_base + 3)
    , lbaMidPort(port_base + 4)
    , lbaHiPort(port_base + 5)
    , devicePort(port_base + 6)
    , commandPort(port_base + 7)
    , controlPort(port_base + 0x206)
{
}

void AdvancedTechnologyAttachment::identify()
{
    devicePort.write(master ? 0xa0 : 0xb0);
    controlPort.write(0);
    devicePort.write(0xa0);

    uint8_t status = commandPort.read();
    if (status == 0xff)
        return;

    devicePort.write(master ? 0xa0 : 0xb0);
    sectorCountPort.write(0);
    lbaLowPort.write(0);
    lbaMidPort.write(0);
    lbaHiPort.write(0);
    commandPort.write(0xec);

    status = commandPort.read();
    if (status == 0)
        return; // no device found

    while ((status & 0x80 == 0x80) && (status & 0x1 != 0x1))
        status = commandPort.read();

    if (status & 0x1)
    {
        printf("ERROR");
        return;
    }

    for (uint16_t i(0); i != 256; ++i)
    {
        uint16_t data = dataPort.read();
        printf("%c %c\n", (data >> 8) & 0xff, data & 0xff);
    }
}

void AdvancedTechnologyAttachment::read28(uint32_t sector)
{
}

void AdvancedTechnologyAttachment::write28(uint32_t sector, uint8_t *data, int count)
{
}

void AdvancedTechnologyAttachment::flush()
{
}
