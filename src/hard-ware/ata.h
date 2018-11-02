
#ifndef __ATA_H_
#define __ATA_H_

#include "base/port.h"

class AdvancedTechnologyAttachment
{

public:
    AdvancedTechnologyAttachment(uint16_t io_base, uint16_t ctrl_base, bool master);

    void identify();
    void read28(uint32_t sector);
    void write28(uint32_t sector, uint8_t *data, int count);
    void flush();

protected:
    bool master;
    Port16Bit dataPort;
    Port16Bit errorPort;
    Port16Bit sectorCountPort;
    Port16Bit lbaLowPort;
    Port16Bit lbaMidPort;
    Port16Bit lbaHiPort;
    Port8Bit devicePort;
    Port8Bit commandPort;

    Port8Bit controlPort;
    Port8Bit infoPort;
};

#endif // __ATA_H_