
#include "screenmanager.h"

ScreenManager *ScreenManagerInstance = nullptr;

uint16_t * const VideoMemLoc = (uint16_t *)0xb8000;
const int MaxScreenWidth = 80;
const int MaxScreenHeight = 25;

ScreenManager *ScreenManager::instance()
{
    static ScreenManager *instance = nullptr;

    if (!instance)
        instance = new ScreenManager;

    return ScreenManagerInstance;
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
    : currentX(0)
    , currentY(0)
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
