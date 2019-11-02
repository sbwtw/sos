
#ifndef __SCREEN_MANAGER_H_
#define __SCREEN_MANAGER_H_

#include "std/types.h"
#include "blinkingcursor.h"

class ScreenManager
{
public:
    static ScreenManager *instance();

    static uint8_t maximumScreenWidth();
    static uint8_t maximumScreenHeight();

    void clean();

    void write(char c);
    uint8_t x() const { return currentX; }
    uint8_t y() const { return currentY; }
    void setCurrentX(int x) { currentX = x; updateBlinkingCursor(); }
    void setCurrentY(int y) { currentY = y; updateBlinkingCursor(); }
    void moveBreakLine() { currentX = 0; moveCurrentLocation(0, 1); }
    void moveCurrentLocation(int offset_x, int offset_y);

    void enableCaret();
    void disableCaret();
    void setCaretLocation(uint8_t x, uint8_t y);
    void moveCareLocation(int offset_x, int offset_y);

protected:
    ScreenManager();

private:
    void updateBlinkingCursor();
    void reverseColor(uint8_t x, uint8_t y);

private:
    bool caretEnabled;
    uint8_t currentX;
    uint8_t currentY;
    uint8_t caretX;
    uint8_t caretY;

    BlinkingCursor blinkingCursor;
};

#endif // __SCREEN_MANAGER_H_
