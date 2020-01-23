
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
#include "hard-ware/systimer.h"
#include "hard-ware/comport.h"
#include "utils/multiboot.h"

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];

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

void put_simple_hex(uint32_t num)
{
    const char *table = "0123456789ABCDEF";

    const uint32_t d = num >> 4;
    const uint32_t r = num & 0xf;

    if (d)
        put_simple_hex(d);
    putc(table[r]);
}

extern "C" void __attribute__((stdcall)) putx(uint32_t num)
{
    putc('0');
    putc('x');

    if (num == 0)
        return putc('0');

    put_simple_hex(num);
}

void mainTask()
{
    while (true)
    {
        unsigned int a = abs(rand() % 10);
        int *p = new int[a];

//        printf("mainTask alloc %d, Pointer Address: %x\n", a, p);

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

//        printf("inputTask alloc %d, Pointer Address: %x\n", a, p);

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

void show_memory_map(multiboot_uint32_t mmap_addr, multiboot_uint32_t mmap_length)
{
    printf("Memory map:\n");

    for (auto mmap = (multiboot_mmap_entry *)mmap_addr; (multiboot_uint32_t)mmap < mmap_addr + mmap_length; mmap++)
    {
        printf("base = %x, length = %x, type = %x\n",
                (uint32_t)mmap->addr,
                (uint32_t)mmap->len,
                (uint32_t)mmap->type);
    }
}

void get_available_memory_range(multiboot_uint32_t mmap_addr, multiboot_uint32_t mmap_length, uint32_t &low_addr, uint32_t &high_addr)
{
    for (auto mmap = (multiboot_mmap_entry *)mmap_addr; (multiboot_uint32_t)mmap < mmap_addr + mmap_length; mmap++)
    {
        if (mmap->type != 0x1 || mmap->addr != 0x100000)
            continue;

        low_addr = mmap->addr;
        high_addr = mmap->addr + mmap->len;

        return;
    }
}

extern "C" void kernelMain(multiboot_info *multiboot_structure, uint32_t _magic_number)
{
//    uint32_t *mem_upper = (uint32_t *)((size_t)multiboot_structure + 8);
//    size_t heap = 20 * 1024 * 1024;
//    FirstFitAllocator memMgr(heap, (*mem_upper) * 1024 - heap - 10 * 1024);

    uint32_t mem_low, mem_high;
    get_available_memory_range(multiboot_structure->mmap_addr, multiboot_structure->mmap_length, mem_low, mem_high);

    FirstFitAllocator _memMgr(mem_low + (kernel_end - kernel_start), mem_high);

    ScreenManager::instance()->clean();

    printf("sbw's Operating System\n");
    show_memory_map(multiboot_structure->mmap_addr, multiboot_structure->mmap_length);

    printf("AVAILABLE MEMORY RANGE: %x~%x\n", mem_low + (kernel_end - kernel_start), mem_high);
    printf("kernel memory: %x ~ %x, total %dKB\n", kernel_start, kernel_end, (kernel_end - kernel_start + 1023) / 1024);

    gdt_init();
    //GlobalDescriptorTable gdt;
    TaskManager taskMgr;
    InterruptManager interrupts(&taskMgr);

    // init timer
    init_timer(200);

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


    printf("Start #");

    while (true)
        asm ("hlt");
}
