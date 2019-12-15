
#include "std/stdio.h"
#include "mouse.h"
#include "utils/screenmanager.h"

/**
 *
 * https://wiki.osdev.org/%228042%22_PS/2_Controller
 * */

MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::onMousePressed(uint8_t button)
{

}

void MouseEventHandler::onMouseReleased(uint8_t button)
{

}

void MouseEventHandler::onMouseMove(int offset_x, int offset_y)
{

}

MouseDriver::MouseDriver(InterruptManager *interrupt_manager, MouseEventHandler *mouse_handler)
    : InterruptHandler(0x2c, interrupt_manager)
    , mouseEventHandler(mouse_handler)
    , dataPort(0x60)
    , commandPort(0x64)
{
}

CpuRegisters * MouseDriver::handleInterrupt(CpuRegisters *state)
{
    uint8_t status = commandPort.read();
    if (!(status & 0x20) || !mouseEventHandler)
        return state;

    buffer[offset] = dataPort.read();
    offset = (offset + 1) % 3;

    if (offset == 0 && (buffer[1] || buffer[2]))
    {
        ScreenManager *sm = ScreenManager::instance();

        sm->moveCareLocation(buffer[1], -buffer[2]);
    }

    return state;
}

void MouseDriver::activate()
{

    // activate interrupts
    commandPort.write(0xa8);
    waitAck();

    // get Compaq status
    commandPort.write(0x20);

    uint8_t status = dataPort.read();
    status |= 0x2;
    status &= ~0x20;

    // set Compaq status
    commandPort.write(0x60);
    dataPort.write(status);
    // ???
//    waitAck();

    commandPort.write(0xd4);
    dataPort.write(0xf4);
    waitAck();
}

void MouseDriver::waitAck()
{
    while (dataPort.read() != 0xfa)
        asm ("nop");
}
