
#include "interrupts.h"
#include "sos_io.h"

InterruptManager *InterruptManager::ActiveInterruptManager = nullptr;
InterruptManager::GateDescriptor InterruptManager::InterruptDescriptorTable[256];

InterruptHandler::InterruptHandler(uint8_t interrupt_number, InterruptManager *interrupt_manager)
    : interruptNumber(interrupt_number)
    , interruptManager(interrupt_manager)
{
    interruptManager->interruptHandlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    if (interruptManager->interruptHandlers[interruptNumber] == this)
        interruptManager->interruptHandlers[interruptNumber] = nullptr;
}

uint32_t InterruptHandler::handleInterrupt(uint32_t esp)
{
    return esp;
}

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    if (ActiveInterruptManager != nullptr)
        return ActiveInterruptManager->doHandleInterrupt(interrupt_number, esp);

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

    InterruptDescriptorTable[interrupt_number].handlerAddressLowBits = ((uint32_t)handler) & 0xffff;
    InterruptDescriptorTable[interrupt_number].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xffff;
    InterruptDescriptorTable[interrupt_number].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    InterruptDescriptorTable[interrupt_number].access = IDT_DESC_PRESENT | descriptorType | ((descriptorPrivilegeLevel & 3) << 5);
    InterruptDescriptorTable[interrupt_number]._reserved = 0;
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
    {
        interruptHandlers[i] = nullptr;
        setInterruptDescriptorTableEntry(i, code_segment, &ignoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    setInterruptDescriptorTableEntry(0x20, code_segment, &handleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x21, code_segment, &handleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2c, code_segment, &handleInterruptRequest0x0c, 0, IDT_INTERRUPT_GATE);

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
    idt.base = (uint32_t)InterruptDescriptorTable;

    asm volatile("lidt %0" : : "m"(idt));
}

InterruptManager::~InterruptManager()
{
}

void InterruptManager::activate()
{
    if (ActiveInterruptManager != nullptr)
        ActiveInterruptManager->deactivate();

    ActiveInterruptManager = this;
    asm("sti");
}

void InterruptManager::deactivate()
{
    if (ActiveInterruptManager == this)
    {
        ActiveInterruptManager = nullptr;
        asm("cli");
    }
}

uint32_t InterruptManager::doHandleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    if (interruptHandlers[interrupt_number] != nullptr)
    {
        esp = interruptHandlers[interrupt_number]->handleInterrupt(esp);
    }
    else if (interrupt_number != 0x20)
    {
        printf("UNHANDLED INTERRUPT: %d", interrupt_number);
    }

    if (0x20 <= interrupt_number && interrupt_number < 0x30)
    {
        picMasterCommand.write(0x20);

        if (0x28 <= interrupt_number)
            picSlaveCommand.write(0x20);
    }

    return esp;
}