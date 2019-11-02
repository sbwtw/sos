
#include "screenmanager.h"

uint16_t * const VideoMemLoc = (uint16_t *)0xb8000;
const int MaxScreenWidth = 80;
const int MaxScreenHeight = 25;

ScreenManager *ScreenManager::instance()
{
    static ScreenManager *instance = nullptr;

    if (instance == nullptr)
        instance = new ScreenManager;

    return instance;
}

uint8_t ScreenManager::maximumScreenHeight()
{
    return MaxScreenHeight;
}

uint8_t ScreenManager::maximumScreenWidth()
{
    return MaxScreenWidth;
}

ScreenManager::ScreenManager()
    : caretEnabled(false)
    , currentX(0)
    , currentY(0)
    , caretX(MaxScreenWidth / 2)
    , caretY(MaxScreenHeight / 2)
{
    blinkingCursor.enableCursor();
}

void ScreenManager::write(char c)
{
    const int loc = MaxScreenWidth * currentY + currentX;

    VideoMemLoc[loc] = (VideoMemLoc[loc] & 0xff00) | (c & 0xff);

    // move cursor to next
    moveCurrentLocation(1, 0);
}

void ScreenManager::clean()
{
//    const int max = MaxScreenHeight * MaxScreenWidth;
//    const int loc = MaxScreenWidth * currentY + currentX;
//
//    memset(('c' & 0xff) | 0xff00)
//
//    VideoMemLoc[loc] = (VideoMemLoc[loc] & 0xff00) | (c & 0xff);
}

void ScreenManager::moveCurrentLocation(int offset_x, int offset_y)
{
    int new_x = currentX + offset_x;
    int new_y = currentY + offset_y;

    new_y += new_x / MaxScreenWidth;
    new_x %= MaxScreenWidth;
    new_y %= MaxScreenHeight;

    currentX = new_x;
    currentY = new_y;

    updateBlinkingCursor();
}

void ScreenManager::updateBlinkingCursor()
{
    blinkingCursor.move(currentX, currentY);
}

void ScreenManager::enableCaret()
{
    if (caretEnabled)
        return;

    caretEnabled = true;
    reverseColor(caretX, caretY);
}

void ScreenManager::disableCaret()
{
    if (!caretEnabled)
        return;

    caretEnabled = false;
    reverseColor(caretX, caretY);
}

void ScreenManager::setCaretLocation(uint8_t x, uint8_t y)
{
    if (x >= MaxScreenWidth || y >= MaxScreenHeight)
        return;

    if (caretEnabled)
    {
        reverseColor(caretX, caretY);
        reverseColor(x, y);
    }

    caretX = x;
    caretY = y;
}

void ScreenManager::moveCareLocation(int offset_x, int offset_y)
{
    int new_x = offset_x + caretX;
    int new_y = offset_y + caretY;

    if (new_x < 0)
        new_x = 0;
    if (new_y < 0)
        new_y = 0;
    if (new_x >= MaxScreenWidth)
        new_x = MaxScreenWidth - 1;
    if (new_y >= MaxScreenHeight)
        new_y = MaxScreenHeight - 1;

    setCaretLocation(new_x, new_y);
}

void ScreenManager::reverseColor(uint8_t x, uint8_t y)
{
    const uint16_t current = VideoMemLoc[MaxScreenWidth * y + x];

    VideoMemLoc[MaxScreenWidth * y + x] = (current & 0xf000) >> 4 |
                                          (current & 0x0f00) << 4 |
                                          (current & 0x00ff);
}
