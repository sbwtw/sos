
/**
 * 用于 Intel 8259A 可编程中断控制器的中断控制
 * 
 * https://wiki.osdev.org/8259_PIC
 * https://pdos.csail.mit.edu/6.828/2016/readings/hardware/8259A.pdf
 * */


#ifndef __INTERRUPTS_H_
#define __INTERRUPTS_H_

#include "std/types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;
class TaskManager;

class InterruptHandler
{
public:
    virtual uint32_t handleInterrupt(uint32_t esp);

protected:
    InterruptHandler(uint8_t interrupt_number, InterruptManager *interrupt_manager);
    virtual ~InterruptHandler();

protected:
    uint8_t interruptNumber;
    InterruptManager *interruptManager;
};

// 中断管理
class InterruptManager
{
    friend class InterruptHandler;

public:
    InterruptManager(GlobalDescriptorTable *gdt, TaskManager *task_manager);
    virtual ~InterruptManager();

    void activate();
    void deactivate();

    uint32_t doHandleInterrupt(uint8_t interrupt_number, uint32_t esp);

protected:
    static void setInterruptDescriptorTableEntry(
        uint8_t interrupt_number,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t descriptorPrivilegeLevel,
        uint8_t descriptorType
    );

    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;

    InterruptHandler *interruptHandlers[256];
    TaskManager *taskManager;
};

#endif // __INTERRUPTS_H_
