
#include "sos_io.h"
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"

extern "C" void __attribute__((stdcall)) putc(char c)
{
    static uint16_t *video_memory = (uint16_t *)0xb8000;
    static uint8_t x(0), y(0);

    const int cols = 80;
    const int rows = 25;

    switch (c)
    {
    case '\n':
        ++y;
        x = 0;
        break;
    case '\t':
    {
        const int loop = 4 - (x % 4);
        for (int i(0); i != loop; ++i)
            putc(' ');
        break;
    }
    default:
        video_memory[cols * y + x] = (video_memory[cols * y + x] & 0xff00) | (c & 0xff);
        ++x;
        break;
    }

    if (y == rows)
    {
        y = 0;
        x = 0;
    } else if (x == cols) {
        ++y;
        x = 0;
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
    printf("sbw's Operating System\n");
    printf("number: %d - %d = %d\n", 6, 2, 6 - 2);
    printf("char: %c\n", 'a');
    printf("string: %s\n", "I'm string with zero terminated");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    KeyboardDriver keyboardDriver(&interrupts);

    interrupts.activate();

    while (1);
}