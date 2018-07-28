
#include "cmos.h"

CMOSManager::CMOSManager()
    : commandPort(0x70)
    , dataPort(0x71)
{
}

CMOSManager::~CMOSManager()
{
}

uint8_t CMOSManager::readRTCRegister(int reg)
{
    commandPort.write(reg);

    return dataPort.read();
}

bool CMOSManager::updateInProgress()
{
    commandPort.write(0x0a);

    return dataPort.read() & 0x80;
}

Time CMOSManager::time()
{
    uint8_t last_second;
    uint8_t last_minute;
    uint8_t last_hour;
    uint8_t last_day;
    uint8_t last_month;
    uint8_t last_year;
    uint8_t register_B;

    // waitting for update
    while (updateInProgress());

    tm.second = readRTCRegister(0x00);
    tm.minute = readRTCRegister(0x02);
    tm.hour = readRTCRegister(0x04);
    tm.day = readRTCRegister(0x07);
    tm.month = readRTCRegister(0x08);
    tm.year = readRTCRegister(0x09);

    do {
        last_second = tm.second;
        last_minute = tm.minute;
        last_hour = tm.hour;
        last_day = tm.day;
        last_month = tm.month;
        last_year = tm.year;

        while (updateInProgress());

        tm.second = readRTCRegister(0x00);
        tm.minute = readRTCRegister(0x02);
        tm.hour = readRTCRegister(0x04);
        tm.day = readRTCRegister(0x07);
        tm.month = readRTCRegister(0x08);
        tm.year = readRTCRegister(0x09);

    } while (last_second != tm.second ||
             last_minute != tm.minute ||
             last_hour != tm.hour ||
             last_day != tm.day ||
             last_month != tm.month ||
             last_year != tm.year);

    register_B = readRTCRegister(0x0b);

    // convert BCD to binary
    if (!(register_B & 0x04))
    {
        tm.second = (tm.second & 0x0f) + ((tm.second / 16) * 10);
        tm.minute = (tm.minute & 0x0f) + ((tm.minute / 16) * 10);
        tm.hour = (tm.hour & 0x0f) + (((tm.hour & 0x70) / 16) * 10 | (tm.hour & 0x80));
        tm.day = (tm.day & 0x0f) + ((tm.day / 16) * 10);
        tm.month = (tm.month & 0x0f) + ((tm.month / 16) * 10);
        tm.year = (tm.year & 0x0f) + ((tm.year / 16) * 10);
    }

    while (tm.year < 2018)
        tm.year += 100;

    return tm;
}
