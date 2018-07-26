
#include "sos_io.h"
#include "mouse.h"

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

    static int8_t x(0), y(0);

    buffer[offset] = dataPort.read();
    offset = (offset + 1) % 3;

    if (offset == 0 && (buffer[1] || buffer[2]))
    {
        static uint16_t *VideoMemory = (uint16_t *)0xb8000;

        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xf000) >> 4 |
                                  (VideoMemory[80 * y + x] & 0x0f00) << 4 |
                                  (VideoMemory[80 * y + x] & 0x00ff);

        x += buffer[1];
        y -= buffer[2];

        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xf000) >> 4 |
                                  (VideoMemory[80 * y + x] & 0x0f00) << 4 |
                                  (VideoMemory[80 * y + x] & 0x00ff);

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
    offset = 0;
    buttons = 0;

    static uint16_t *VideoMemory = (uint16_t *)0xb8000;
    VideoMemory[80 * 0 + 0] = (VideoMemory[80 * 0 + 0] & 0xf000) >> 4 |
                              (VideoMemory[80 * 0 + 0] & 0x0f00) << 4 |
                              (VideoMemory[80 * 0 + 0] & 0x00ff);

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