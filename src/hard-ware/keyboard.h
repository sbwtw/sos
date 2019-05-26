
#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include "types.h"
#include "base/interrupts.h"
#include "base/port.h"
#include "hw-abstract/driver.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();

    virtual void onKeyPressed(char key);
    virtual void onKeyReleased(char key);
};

class KeyboardDriver : public InterruptHandler, public Driver
{
public:
    KeyboardDriver(InterruptManager *interrupt_manager, KeyboardEventHandler *handler);

    uint32_t handleInterrupt(uint32_t esp) override;

    void activate() override;

private:
    Port8Bit dataPort;
    Port8Bit commandPort;

    KeyboardEventHandler *keyEventHandler;
};

#endif // __KEYBOARD_H_
