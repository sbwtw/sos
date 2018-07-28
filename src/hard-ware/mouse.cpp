
#include "sos_io.h"
#include "mouse.h"
#include "utils/screenmanager.h"

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

uint32_t MouseDriver::handleInterrupt(uint32_t esp)
{
    uint8_t status = commandPort.read();
    if (!(status & 0x20) || !mouseEventHandler)
        return esp;

    buffer[offset] = dataPort.read();
    offset = (offset + 1) % 3;

    if (offset == 0 && (buffer[1] || buffer[2]))
    {
        ScreenManager *sm = ScreenManager::instance();

        sm->moveCareLocation(buffer[1], -buffer[2]);

        // for (uint8_t i(0); i != 3; ++i)
        // {
            // if (buffer[0] & (0x1 << i) != buttons & (0x1 << i))
            // {
                // button pressed
            // }
        // }
        // buttons = buffer[0];
    }

    return esp;
}

void MouseDriver::activate()
{
    // activate interrupts
    commandPort.write(0xa8);
    // get current state
    commandPort.write(0x20);

    uint8_t status = dataPort.read() | 0x2;
    // set state
    commandPort.write(0x60);
    dataPort.write(status);

    commandPort.write(0xd4);
    dataPort.write(0xf4);
    dataPort.read();
}