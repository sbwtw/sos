
#include "ata.h"
#include "std/stdio.h"

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(uint16_t io_base, uint16_t ctrl_base, bool master)
    : master(master)
    , dataPort(io_base)
    , errorPort(io_base + 1)
    , sectorCountPort(io_base + 2)
    , lbaLowPort(io_base + 3)
    , lbaMidPort(io_base + 4)
    , lbaHiPort(io_base + 5)
    , devicePort(io_base + 6)
    , commandPort(io_base + 7)
    , controlPort(io_base + 0x206)
{
}

void AdvancedTechnologyAttachment::identify()
{
    // LBA 48Bit
    devicePort.write(master ? 0xa0 : 0xb0);
    controlPort.write(0);

    devicePort.write(0xa0);
    uint8_t status = commandPort.read();
    printf("Status: %x\n", status);

//    devicePort.write(0x40 | (master << 4) | 0xd0);
//    errorPort.write(0);
//    sectorCountPort.write(0);
//    lbaLowPort.write(0);
//    lbaMidPort.write(0);
//    lbaHiPort.write(0);
//    sectorCountPort.write(1);
//    lbaLowPort.write(0);
//    lbaMidPort.write(0);
//    lbaHiPort.write(0);
//    commandPort.write(0x24);

    //printf("ATA: %x\n", dataPort.read());

    // devicePort.write(master ? 0xa0 : 0xb0);
    // controlPort.write(0);
    // devicePort.write(0xa0);

    // printf("aaa1");
    // uint8_t status = commandPort.read();
    // if (status == 0xff)
    //     return;

    // printf("aaa");

    // devicePort.write(master ? 0xa0 : 0xb0);
    // sectorCountPort.write(0);
    // lbaLowPort.write(0);
    // lbaMidPort.write(0);
    // lbaHiPort.write(0);
    // commandPort.write(0xec);

//    int status = commandPort.read();
    // if (status == 0)
    //     return; // no device found

//    while ((status & 0x80) != 0x80)
//        status = commandPort.read();
//
//    if ((status & 0x21) == 0x21)
//        printf("error %x\n", errorPort.read());

    // if (status & 0x1)
    // {
    //     printf("ERROR");
    //     return;
    // }

    // for (uint16_t i(0); i != 256; ++i)
    // {
    //     uint16_t data = dataPort.read();
    //     printf("%c %c\n", (data >> 8) & 0xff, data & 0xff);
    // }
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

