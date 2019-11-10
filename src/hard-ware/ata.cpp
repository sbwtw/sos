
#include "ata.h"
#include "std/stdio.h"

union SectorCount
{
    uint64_t sectors;

    struct
    {
        uint16_t s0;
        uint16_t s1;
        uint16_t s2;
        uint16_t s3;
    };
};

AtaChs::AtaChs(uint16_t io_base, uint16_t ctrl_base, bool master)
    : master(master)
    , dataPort(io_base)
    , errorPort(io_base + 1)
    , sectorCountPort(io_base + 2)
    , lbaLowPort(io_base + 3)
    , lbaMidPort(io_base + 4)
    , lbaHiPort(io_base + 5)
    , devicePort(io_base + 6)
    , commandPort(io_base + 7)
    , controlPort(ctrl_base)
{
}

void AtaChs::identify()
{
    devicePort.write(master ? 0xa0 : 0xb0);
    controlPort.write(0);

    // status check
    devicePort.write(0xa0);
    uint8_t status = commandPort.read();
    // channel unavailable
    if (status == 0xff)
        return;

    // IDENTIFY command
    devicePort.write(master ? 0xa0 : 0xb0);
    sectorCountPort.write(0);
    lbaLowPort.write(0);
    lbaMidPort.write(0);
    lbaHiPort.write(0);
    commandPort.write(0xec);

    status = commandPort.read();
    // no device
    if (status == 0x00)
        return;

    while ((status & 0x80) == 0x80 && (status & 0x01) != 0x01)
        status = commandPort.read();

    // error
    if (status & 0x01)
    {
        printf("Identify Failed, Error = %x\n", status);
        return;
    }

    uint16_t data[256];
    for (auto i(0); i != 256; ++i)
        data[i] = dataPort.read();

    printf("Disk Information:\n");

    // Read disk size
    SectorCount sector { 0 };
    sector.s0 = data[100];
    sector.s1 = data[101];
    sector.s2 = data[102];
    sector.s3 = data[103];
    printf("%d sectors\t", sector.sectors);
    printf("%d MiB\t", sector.sectors / 2048);
    printf("\n");
}

void AtaChs::read(uint32_t sector, uint8_t *data)
{
    // sector error
    if ((sector & 0xf0000000) != 0)
        return;

    devicePort.write((master ? 0xe0 : 0xf0) | ((sector & 0x0f000000) >> 24));
    errorPort.write(0);
    sectorCountPort.write(1);

    lbaLowPort.write(sector & 0x000000ff);
    lbaLowPort.write((sector & 0x0000ff00) >> 8);
    lbaLowPort.write((sector & 0x00ff0000) >> 16);
    commandPort.write(0x30);

    uint8_t status = commandPort.read();
    while ((status & 0x80) == 0x80 && (status && 0x01) != 0x01)
        status = commandPort.read();

    if (status & 0x01)
        return;


    for (uint16_t i(0); i != 256; i += 2)
    {
        uint16_t r = dataPort.read();
        data[i] = r & 0x00ff;
        data[i + 1] = (r >> 8) & 0x00ff;
    }
}

void AtaChs::write(uint32_t sector, uint8_t *data, int count)
{
}

void AtaChs::flush()
{
}

