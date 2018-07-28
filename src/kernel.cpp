
#include "sos_io.h"
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "driver.h"
#include "screenmanager.h"
#include "pci.h"
#include "memorymanagement.h"
#include "cmos.h"

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

extern "C" void __attribute__((stdcall)) putd(int num)
{
    if (!num)
        return putc('0');

    if (num < 0)
    {
        putc('-');
        num = -num;
    }

    const int d = num / 10;
    const int r = num % 10;

    if (d)
        putd(d);
    putc(r + '0');
}

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
    uint32_t *mem_upper = (uint32_t *)((size_t)multiboot_structure + 8);
    size_t heap = 20 * 1024 * 1024;
    MemoryManager memMgr(heap, (*mem_upper) * 1024 - heap - 10 * 1024);

    printf("sbw's Operating System\n");
    printf("number: %d - %d = %d\n", 16, 2, 16 - 2);
    printf("hex: %x\n", 0x0123abc);
    printf("char: %c\n", 'a');
    printf("string: %s\n", "I'm string with zero terminated");

    printf("AVAILABLE MEMORY RANGE: %x\n", *mem_upper * 1024);
    printf("AVAILABLE MEMORY START: %x\n", heap);
    printf("HEAP: %x - %x\n", heap, (*mem_upper) * 1024 - heap - 10 * 1024);

    memMgr.dumpAllocatorInfo();
    memMgr.malloc(512);
    memMgr.dumpAllocatorInfo();

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

    // PeripheralComponentInterconnectController pciController;
    // pciController.selectDrivers(&drvMgr);

    interrupts.activate();

    ScreenManager *sm = ScreenManager::instance();
    sm->enableCaret();

    CMOSManager cmosMgr;
    Time tm = cmosMgr.time();
    printf("Time: %d/%d/%d %d:%d:%d\n", tm.year, tm.month, tm.day, tm.hour + 8, tm.minute, tm.second);

    while (1);
}