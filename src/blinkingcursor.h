
#ifndef __BLINKING_CURSOR_H_
#define __BLINKING_CURSOR_H_

/**
 *  Text Mode Blinking Cursor Management
 */

#include "port.h"

class BlinkingCursor
{
public:
    BlinkingCursor();

    void enableCursor();
    void disableCursor();
    void move(uint8_t x, uint8_t y);

private:
    Port8Bit commandPort;
    Port8Bit dataPort;
};

#endif // __BLINKING_CURSOR_H_