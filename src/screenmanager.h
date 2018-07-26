
#ifndef __SCREEN_MANAGER_H_
#define __SCREEN_MANAGER_H_

#include "types.h"

class ScreenManager
{
public:
    static ScreenManager *instance();

    static uint8_t maximumScreenWidth();
    static uint8_t maximumScreenHeight();

    void write(char c);
    uint8_t x() const { return currentX; }
    uint8_t y() const { return currentY; }
    void setCurrentX(int x) { currentX = x; }
    void setCurrentY(int y) { currentY = y; }
    void moveBreakLine() { currentX = 0; moveCurrentLocation(0, 1); }
    void moveCurrentLocation(int offset_x, int offset_y);

// protected:
    // FIXME: we don't have memory management yet.
    static void setInstance(ScreenManager *instance);
    ScreenManager();

private:
    uint8_t currentX;
    uint8_t currentY;
};

#endif // __SCREEN_MANAGER_H_