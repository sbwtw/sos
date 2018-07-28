
#ifndef __MOUSE_H_
#define __MOUSE_H_

#include "types.h"
#include "base/interrupts.h"
#include "base/port.h"
#include "hw-abstract/driver.h"

class MouseEventHandler
{
public:
    MouseEventHandler();

    virtual void onMousePressed(uint8_t button);
    virtual void onMouseReleased(uint8_t button);
    virtual void onMouseMove(int offset_x, int offset_y);

private:
    int mouseX;
    int mouseY;
};

class MouseDriver : public InterruptHandler, public Driver
{
public:
    MouseDriver(InterruptManager *interrupt_manager, MouseEventHandler *mouse_handler);

    uint32_t handleInterrupt(uint32_t esp) override;

    void activate() override;

private:
    int8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;

    MouseEventHandler *mouseEventHandler;

    Port8Bit dataPort;
    Port8Bit commandPort;
};

#endif // __MOUSE_H_