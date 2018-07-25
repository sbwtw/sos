
#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"

class KeyboardDriver : public InterruptHandler, public Driver
{
public:
    KeyboardDriver(InterruptManager *interrupt_manager);

    uint32_t handleInterrupt(uint32_t esp) override;

    void activate() override;

private:
    Port8Bit dataPort;
    Port8Bit commandPort;
};

#endif // __KEYBOARD_H_