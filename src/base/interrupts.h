
#ifndef __INTERRUPTS_H_
#define __INTERRUPTS_H_

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;

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

class InterruptManager
{
    friend class InterruptHandler;

public:
    InterruptManager(GlobalDescriptorTable *gdt);
    virtual ~InterruptManager();

    void activate();
    void deactivate();

    uint32_t doHandleInterrupt(uint8_t interrupt_number, uint32_t esp);

    static uint32_t handleInterrupt(uint8_t interrupt_number, uint32_t esp);

    static void ignoreInterruptRequest();
    static void handleInterruptRequest0x00();
    static void handleInterruptRequest0x01();
    static void handleInterruptRequest0x0c();

protected:
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t _reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

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

    static InterruptManager *ActiveInterruptManager;
    static GateDescriptor InterruptDescriptorTable[256];
};

#endif // __INTERRUPTS_H_