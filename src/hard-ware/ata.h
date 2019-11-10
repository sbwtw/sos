
#ifndef __ATA_H_
#define __ATA_H_

#include "base/port.h"

/**
 * ATA Hard disk driver
 *
 * 以 CHS(Cylinder, Head, Sector) 模式实现的 ATA 磁盘驱动
 *
 * https://wiki.osdev.org/ATA_PIO_Mode#CHS_mode
 *
 * Cylinder: [0, 1023]
 * Head: [0, 15]
 * Sector: [1, 63]
 *
 * 最大支持 512MiB
 * */

class AtaChs
{

public:
    AtaChs(uint16_t io_base, uint16_t ctrl_base, bool master);

    void identify();
    void read(uint32_t sector, uint8_t *data);
    void write(uint32_t sector, uint8_t *data, int count);
    void flush();

protected:
    bool master;
    Port16Bit dataPort;
    Port8Bit errorPort;
    Port8Bit sectorCountPort;
    Port8Bit lbaLowPort;
    Port8Bit lbaMidPort;
    Port8Bit lbaHiPort;
    Port8Bit devicePort;
    Port8Bit commandPort;
    Port8Bit controlPort;
};

#endif // __ATA_H_

