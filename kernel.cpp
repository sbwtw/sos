
#include "types.h"
#include "gdt.h"
#include "interrupts.h"

extern "C" int __attribute__((cdecl)) __sos_builtin_printf(char *format);

extern "C" void __attribute__((stdcall)) putc(char c)
{
    static uint16_t *video_memory = (uint16_t *)0xb8000;
    static uint8_t x(0), y(0);

    const int cols = 80;
    const int rows = 25;

    switch (c)
    {
    default:
        video_memory[cols * y + x] = (video_memory[cols * y + x] & 0xff00) | c;
        ++x;
        break;
    }
}

void printf(char *str)
{
    static uint16_t *video_memory = (uint16_t *)0xb8000;
    static uint8_t x(0), y(0);

    const int cols = 80;
    const int rows = 25;

    for (int i(0); str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
        case '\n':
            x = 0;
            ++y;
            break;
        default:
            video_memory[cols * y + x] = (video_memory[cols * y + x] & 0xff00) | str[i];
            ++x;
            break;
        }

        if (x == 80)
        {
            ++y;
            x = 0;
        }

        if (y == rows)
        {
            // clear screen
            for (int idx(0); idx != cols * rows; ++idx)
                video_memory[idx] = (video_memory[idx] & 0xff00) | ' ';

            x = 0;
            y = 0;
        }
    }

    char *s = "kpwsdfasd";
    int r = __sos_builtin_printf(s);
    *((uint16_t *)0xb8000) = (*((uint16_t *)0xb8000) & 0xff00) | char(r + '0');
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
    printf("sbw's Operating System\n");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    interrupts.activate();

    while (1);
}