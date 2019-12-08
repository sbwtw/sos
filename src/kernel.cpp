
#include "std/types.h"
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
#include "std/stdio.h"
#include "std/stdlib.h"
#include "std/unistd.h"
#include "multitasking.h"
#include "hard-ware/comport.h"

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

// print a unsigned digit
void put_unsigned_digit(uint32_t unum)
{
    const int d = unum / 10;
    const int r = unum % 10;

    if (d)
        put_unsigned_digit(d);
    putc(r + '0');
}

// print digit
extern "C" void __attribute__((stdcall)) putd(int num)
{
    if (!num)
        return putc('0');

    if (num < 0)
    {
        putc('-');

        // TODO: overflow?
        put_unsigned_digit(-num);
    } else {
        put_unsigned_digit(num);
    }
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
        unsigned int a = abs(rand() % 10);
        int *p = new int[a];

        //printf("mainTask alloc %d, Pointer Address: %x\n", a, p);

        delete[] p;

        sleep(1);
    }
}

void inputTask()
{
    while (true)
    {
        unsigned int a = abs(rand() % 10);
        int *p = new int[a];

        //printf("inputTask alloc %d, Pointer Address: %x\n", a, p);

        delete[] p;

        sleep(1);
    }
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

    ScreenManager::instance()->clean();

    printf("sbw's Operating System\n");
    printf("number: %d - %d = %d\n", 16, 2, 16 - 2);
    printf("hex: %x\n", 0x0123abc);
    printf("char: %c\n", 'a');
    printf("string: %s\n", "I'm string with zero terminated");

    printf("AVAILABLE MEMORY RANGE: %x\n", *mem_upper * 1024);
    printf("AVAILABLE MEMORY START: %x\n", heap);
    printf("HEAP: %x - %x\n", heap, (*mem_upper) * 1024 - heap - 10 * 1024);

    gdt_init();
    //GlobalDescriptorTable gdt;
    TaskManager taskMgr;
    InterruptManager interrupts(&taskMgr);

    KeyboardEventHandler keyboardEventHandler;
    MouseEventHandler mouseEventHandler;

    KeyboardDriver keyboardDriver(&interrupts, &keyboardEventHandler);
    MouseDriver mouseDriver(&interrupts, &mouseEventHandler);
//
    DriverManager drvMgr;
    drvMgr.appendDriver(&mouseDriver);
    drvMgr.appendDriver(&keyboardDriver);
    drvMgr.activateAll();

//    PeripheralComponentInterconnectController pciController;
//    pciController.selectDrivers(&drvMgr);

    interrupts.activate();

    Task task_main(mainTask);
    Task task_input(inputTask);
    taskMgr.appendTask(&task_main);
    taskMgr.appendTask(&task_input);

    ScreenManager *sm = ScreenManager::instance();
    sm->enableCaret(); // enable mouse caret

    CMOSManager cmosMgr;
    Time tm = cmosMgr.time();
    printf("Time: %d/%d/%d %d:%d:%d\n", tm.year, tm.month, tm.day, tm.hour + 8, tm.minute, tm.second);
    printf("RANDOM: %d, %d, %d, %d\n", rand(), rand(), rand(), rand());

//    AtaChs *ata0m = new AtaChs(0x1f0, 0x3f6, true);
//    AtaChs ata0s(0x1f0, 0x3f6, false);
//    AtaChs ata1m(0x170, 0x376, true);
//    AtaChs ata1s(0x170, 0x376, false);

//    ata0m->identify();
//    ata0s.identify();
//    ata1m.identify();
//    ata1s.identify();

//    uint8_t data[512] = { 0 };
//    for (int i(0); i != 512; ++i)
//        data[i] = i % 256;

//    printf("Write Start\n");
//    ata0m->write(1, data, 256);
//    printf("Write Finished\n");
//    ata0m->flush();
//    printf("flush Finished\n");
//    ata0m->read(1, data);
//    printf("read Finished\n");
//
//    ComPort com1(COM1_ADDR);
//    com1.init();
//    for (int i(0); i != 512; ++i)
//        com1.write(data[i]);


    asm ("movb $0x13, %%al \n" \
         "movb $0x00, %%ah \n" \
         "int $0x10" : : );

    printf("Start #");

    while (true)
        asm ("hlt");
}
