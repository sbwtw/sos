
#include "utils/blinkingcursor.h"
#include "utils/screenmanager.h"

BlinkingCursor::BlinkingCursor()
    : commandPort(0x3d4)
    , dataPort(0x3d5)
{
    disableCursor();
}

void BlinkingCursor::enableCursor()
{
    const int scan_line_start = 14;
    const int scan_line_end = 15;

   commandPort.write(0x0a);
   dataPort.write((dataPort.read() & 0xc0) | scan_line_start);

   commandPort.write(0x0b);
   dataPort.write((dataPort.read() & 0xe0) | scan_line_end);
}

void BlinkingCursor::disableCursor()
{
    commandPort.write(0x0a);
    dataPort.write(0x20);
}

void BlinkingCursor::move(uint8_t x, uint8_t y)
{
    uint16_t location = y * ScreenManager::maximumScreenWidth() + x;

    commandPort.write(0x0f);
    dataPort.write(location & 0xff);

    commandPort.write(0x0e);
    dataPort.write((location >> 8) & 0xff);
}
