
#include "types.h"
#include "stdlib.h"

/**
 * https://wiki.osdev.org/Random_Number_Generator
 * */

static unsigned long int next = 1;

struct RandResult
{
    bool success;
    int rand;
};

RandResult rand_using_x86_rdrand()
{
    const int retry_times = 10;

    int32_t success = 1;
    int32_t random = 0;

    asm volatile ("mov $1, %%eax \n"
                  "cpuid \n"
                  "bt $30, %%ecx \n"
                  "jnc _fail \n"
                  ""
                  "mov %2, %%ecx \n"
                  "retry: \n"
                  "rdrand %%eax \n"
                  "jc _end \n"
                  "loop retry \n"
                  ""
                  "_fail: \n"
                  "mov $0, %0 \n"
                  "_end: \n"
                  "mov %%eax, %1 \n"
                  : "=al"(success), "=al"(random)
                  : "m"(retry_times)
                  : "%eax", "%ecx");

    return { success, random };
}

int rand()
{
    const auto r = rand_using_x86_rdrand();
    if (r.success)
        return r.rand;

    next = next * 1103515245 + 12345;

    return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed)
{
    next = seed;
}
