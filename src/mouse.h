
#ifndef __MOUSE_H_
#define __MOUSE_H_

#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"

class MouseDriver : public InterruptHandler, public Driver
{
public:
    MouseDriver(InterruptManager *interrupt_manager);

    uint32_t handleInterrupt(uint32_t esp) override;

    void activate() override;

private:
    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;

    Port8Bit dataPort;
    Port8Bit commandPort;
};

#endif // __MOUSE_H_