
#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include "types.h"
#include "interrupts.h"
#include "port.h"

class KeyboardDriver : public InterruptHandler
{
public:
    KeyboardDriver(InterruptManager *interrupt_manager);

    uint32_t handleInterrupt(uint32_t esp) override;

private:
    Port8Bit dataPort;
    Port8Bit commandPort;
};

#endif // __KEYBOARD_H_