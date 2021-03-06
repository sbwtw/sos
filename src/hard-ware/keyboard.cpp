
#include "std/stdio.h"
#include "keyboard.h"
#include "utils/screenmanager.h"

KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::onKeyPressed(char key)
{
    //printf("PRESSED %x\t", key);
}

void KeyboardEventHandler::onKeyReleased(char key)
{
    // printf("RELEASED %x %c\t", key, key & 0xff);
    // printf("RELEASED %c\t", key);
}

KeyboardDriver::KeyboardDriver(InterruptManager *interrupt_manager, KeyboardEventHandler *key_handler)
    : InterruptHandler(0x21, interrupt_manager)
    , dataPort(0x60)
    , commandPort(0x64)
    , keyEventHandler(key_handler)
{
}

CpuRegisters * KeyboardDriver::handleInterrupt(CpuRegisters *state)
{
    uint8_t key = dataPort.read();

    if (keyEventHandler == nullptr)
        return state;

    if (key & 0x80)
        keyEventHandler->onKeyReleased(key);
    else
        keyEventHandler->onKeyPressed(key);

    return state;
}

void KeyboardDriver::activate()
{
    while (commandPort.read() & 0x1)
        dataPort.read();

    // activate interrupts
    commandPort.write(0xae);
    // get current state
    commandPort.write(0x20);

    const uint8_t data = dataPort.read();

    printf("Keyboard Driver Stat: %x\n", data);

    uint8_t status = (data | 0x1) & ~0x10;
    // set state
    commandPort.write(0x60);
    dataPort.write(status);
    dataPort.write(0xf4);
}
