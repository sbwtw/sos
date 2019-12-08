
#include "multitasking.h"
#include "base/gdt.h"

Task::Task(void entrypoint())
{
    cpuState = (CPUState *)(stack + 4096 - sizeof(CPUState));

    cpuState->eax = 0;
    cpuState->ebx = 0;
    cpuState->ecx = 0;
    cpuState->edx = 0;

    cpuState->esi = 0;
    cpuState->edi = 0;
    cpuState->ebp = 0;

    // cpuState->gs = 0;
    // cpuState->fs = 0;
    // cpuState->es = 0;
    // cpuState->ds = 0;

    // cpuState->errno = 0;

    // cpuState->esp = 0;
    cpuState->eip = (uint32_t)entrypoint;
    //cpuState->cs = gdt->codeSegmentSelector();
    cpuState->cs = get_code_segment();
    // cpuState->ss = 0;
    cpuState->eflags = 0x202;
}

Task::~Task()
{}

TaskManager::TaskManager()
    : taskCount(0)
    , currentTaskID(-1)
{}

TaskManager::~TaskManager()
{}

bool TaskManager::appendTask(Task *task)
{
    if (taskCount >= 256)
        return false;

    tasks[taskCount++] = task;

    return true;
}

CPUState *TaskManager::schedule(CPUState *cpustate)
{
    if (taskCount <= 0)
        return cpustate;

    if (currentTaskID >= 0)
        tasks[currentTaskID]->cpuState = cpustate;

    if (++currentTaskID >= taskCount)
        currentTaskID %= taskCount;

    return tasks[currentTaskID]->cpuState;
}

