
#include "sos_io.h"
#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager *interrupt_manager)
    : InterruptHandler(0x21, interrupt_manager)
    , dataPort(0x60)
    , commandPort(0x64)
{
    while (commandPort.read() & 0x1)
        dataPort.read();

    // activate interrupts
    commandPort.write(0xae);
    // get current state
    commandPort.write(0x20);

    uint8_t status = (dataPort.read() | 0x1) & ~0x10;
    // set state
    commandPort.write(0x60);
    dataPort.write(status);
    dataPort.write(0xf4);
}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp)
{
    uint8_t key = dataPort.read();

    printf("KEYBOARD %x\t", key);

    return esp;
}