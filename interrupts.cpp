
#include "interrupts.h"

void printf(char *str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    printf("INTERRUPT\n");

    return esp;
}

void InterruptManager::setInterruptDescriptorTableEntry(
    uint8_t interrupt_number,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t descriptorPrivilegeLevel,
    uint8_t descriptorType
) {

    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interrupt_number].handlerAddressLowBits = ((uint32_t)handler) & 0xffff;
    interruptDescriptorTable[interrupt_number].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xffff;
    interruptDescriptorTable[interrupt_number].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interrupt_number].access = IDT_DESC_PRESENT | descriptorType | ((descriptorPrivilegeLevel & 3) << 5);
    interruptDescriptorTable[interrupt_number]._reserved = 0;
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
    : picMasterCommand(0x20)
    , picMasterData(0x21)
    , picSlaveCommand(0xa0)
    , picSlaveData(0xa1)
{
    uint16_t code_segment = gdt->codeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xe;

    for (uint16_t i(0); i != 256; ++i)
        setInterruptDescriptorTableEntry(i, code_segment, &ignoreInterruptRequest, 0, IDT_INTERRUPT_GATE);

    setInterruptDescriptorTableEntry(0x20, code_segment, &handleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x21, code_segment, &handleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    picMasterCommand.write(0x11);
    picSlaveCommand.write(0x11);

    picMasterData.write(0x20);
    picSlaveData.write(0x28);
    picMasterData.write(0x04);
    picSlaveData.write(0x02);
    picMasterData.write(0x01);
    picSlaveData.write(0x01);
    picMasterData.write(0x00);
    picSlaveData.write(0x00);

    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(InterruptManager::GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;

    asm volatile("lidt %0" : : "m"(idt));
}

InterruptManager::~InterruptManager()
{
}

void InterruptManager::activate()
{
    asm("sti");
}