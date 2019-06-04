
#include "sos_base.h"
#include "types.h"
#include "base/gdt.h"
#include "base/interrupts.h"
#include "hard-ware/keyboard.h"
#include "hard-ware/mouse.h"
#include "hard-ware/ata.h"
#include "hw-abstract/driver.h"
#include "utils/screenmanager.h"
#include "hard-ware/pci.h"
#include "memory/firstfitallocator.h"
#include "hard-ware/cmos.h"
#include "std/stdlib.h"
#include "multitasking.h"

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

void put_simple_digit(int num)
{
    const int d = num / 10;
    const int r = num % 10;

    if (d)
        put_simple_digit(d);
    putc(r + '0');
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

    put_simple_digit(num);
}

void put_simple_hex(int num)
{
    const char *table = "0123456789ABCDEF";

    const int d = num / 16;
    const int r = num % 16;

    if (d)
        put_simple_hex(d);
    putc(table[r]);
}

extern "C" void __attribute__((stdcall)) putx(int num)
{
    putc('0');
    putc('x');

    if (!num)
        return putc('0');

    put_simple_hex(num);
}

void mainTask()
{
    while (true)
    {
        int *p = new int;

        delete p;
    }
}

void inputTask()
{

}

void interruptTask()
{
    asm("int $0x20");
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
    FirstFitAllocator memMgr(heap, (*mem_upper) * 1024 - heap - 10 * 1024);

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
    TaskManager taskMgr;
    InterruptManager interrupts(&gdt, &taskMgr);

    KeyboardEventHandler keyboardEventHandler;
    MouseEventHandler mouseEventHandler;

    KeyboardDriver keyboardDriver(&interrupts, &keyboardEventHandler);
    MouseDriver mouseDriver(&interrupts, &mouseEventHandler);

    DriverManager drvMgr;
    drvMgr.appendDriver(&mouseDriver);
    drvMgr.appendDriver(&keyboardDriver);
    drvMgr.activateAll();

    // interrupt 14
    AdvancedTechnologyAttachment ata0m(0x1f0, 0x3f6, true);
    AdvancedTechnologyAttachment ata0s(0x1f0, 0x3f6, false);

    // interrupt 15
    // AdvancedTechnologyAttachment ata1m(0x170, true);
    // ata1m.identify();
    // AdvancedTechnologyAttachment ata1s(0x170, false);
    // ata1s.identify();

    // PeripheralComponentInterconnectController pciController;
    // pciController.selectDrivers(&drvMgr);

    interrupts.activate();

    //Task task_main(&gdt, mainTask);
    //taskMgr.appendTask(&task_main);

    ScreenManager *sm = ScreenManager::instance();
    sm->enableCaret(); // enable mouse caret

    CMOSManager cmosMgr;
    Time tm = cmosMgr.time();
    printf("Time: %d/%d/%d %d:%d:%d\n", tm.year, tm.month, tm.day, tm.hour + 8, tm.minute, tm.second);
    printf("RANDOM: %d, %d, %d, %d\n", rand(), rand(), rand(), rand());

    char *a = "aac";
    char *b = "bbd";
    printf("%s\n", a);

    void *p = memcpy(a, b, 2);
    printf("%s\n", p);

    //ata0m.identify();
    //ata0s.identify();

    while (1);
}
