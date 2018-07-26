
#include "sos_io.h"
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "driver.h"
#include "screenmanager.h"
#include "blinkingcursor.h"

extern "C" void __attribute__((stdcall)) putc(char c)
{
    ScreenManager *sm = ScreenManager::instance();

    switch (c)
    {
    case '\n':
        sm->moveBreakLine();
        break;
    case '\t':
    {
        const int space_count = 4 - (sm->x() % 4);
        for (int i(0); i != space_count; ++i)
            sm->write(' ');
        break;
    }
    default:
        sm->write(c);
        break;
    }
}

// void printf(char *str)
// {
    // __sos_builtin_printf(str);

    // char *s = "test";
    // int r = __sos_builtin_printf(s);
    // *((uint16_t *)0xb8000) = (*((uint16_t *)0xb8000) & 0xff00) | char(r + '0');
// }

typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; ++i)
    {
        (*i)();
    }
}

extern "C" void kernelMain(void *multiboot_structure, uint32_t magic_number)
{
    ScreenManager screenManager;
    ScreenManager::setInstance(&screenManager);

    printf("sbw's Operating System\n");
    printf("number: %d - %d = %d\n", 6, 2, 6 - 2);
    printf("hex: %x\n", 0x0123abc);
    printf("char: %c\n", 'a');
    printf("string: %s\n", "I'm string with zero terminated");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    KeyboardEventHandler keyboardEventHandler;
    MouseEventHandler mouseEventHandler;

    KeyboardDriver keyboardDriver(&interrupts, &keyboardEventHandler);
    MouseDriver mouseDriver(&interrupts, &mouseEventHandler);

    DriverManager drvMgr;
    drvMgr.appendDriver(&mouseDriver);
    drvMgr.appendDriver(&keyboardDriver);
    drvMgr.activateAll();

    interrupts.activate();

    BlinkingCursor cursorMgr;
    cursorMgr.disableCursor();

    while (1);
}