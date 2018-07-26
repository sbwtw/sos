
#include "blinkingcursor.h"

BlinkingCursor::BlinkingCursor()
    : commandPort(0x3d4)
    , dataPort(0x3d5)
{
}

void BlinkingCursor::enableCursor()
{
}

void BlinkingCursor::disableCursor()
{
    commandPort.write(0x0a);
    dataPort.write(0x20);
}