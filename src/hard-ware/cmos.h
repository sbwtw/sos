
#ifndef __CMOS_H_
#define __CMOS_H_

#include "base/port.h"

struct Time
{
    int year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

class CMOSManager
{
public:
    CMOSManager();
    virtual ~CMOSManager();

    Time time();

protected:
    bool updateInProgress();
    uint8_t readRTCRegister(int reg);

private:
    Port8Bit commandPort;
    Port8Bit dataPort;

    Time tm;
};

#endif // __CMOS_H_