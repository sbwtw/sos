
#ifndef __MULTI_TASKING_H_
#define __MULTI_TASKING_H_

#include "std/types.h"

class GlobalDescriptorTable;

struct CpuRegisters
{
    uint32_t ds;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;

    // error code, pushed by CPU
    uint32_t errno;

    // pushed by CPU
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

class Task
{
    friend class TaskManager;

public:
    Task(void entryPoint());
    ~Task();

private:
    uint8_t stack[4096]; // 4 KiB
    CpuRegisters *cpuState;
};

class TaskManager
{
public:
    TaskManager();
    ~TaskManager();

    bool appendTask(Task *task);
    CpuRegisters *schedule(CpuRegisters *cpustate);

private:
    Task *tasks[256];
    int taskCount;
    int currentTaskID;
};

#endif // __MULTI_TASKING_H_
