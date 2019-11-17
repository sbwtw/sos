
#include "interrupts.h"
#include "std/stdio.h"
#include "multitasking.h"

struct GateDescriptor
{
    uint16_t handlerAddressLowBits;
    uint16_t gdt_codeSegmentSelector;
    uint8_t _reserved;
    uint8_t access;
    uint16_t handlerAddressHighBits;
} __attribute__((packed));

// idt 描述
struct InterruptDescriptorTablePointer
{
    uint16_t size;
    uint32_t base;
} __attribute__((packed));

static InterruptManager *ActiveInterruptManager = nullptr;
static GateDescriptor InterruptDescriptorTable[256];

// 对所有中断的声明，在 interrupt_stub.s 中实现，调回到 handleInterrupt 函数中由 C++ 处理
extern "C" void handleInterruptRequest0x00();
extern "C" void handleInterruptRequest0x01();
extern "C" void handleInterruptRequest0x0c();

// 用于在 asm/interrupt_stubs.s 接收中断，将中断转回 C++ 中处理
extern "C" uint32_t handleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    if (ActiveInterruptManager != nullptr)
        return ActiveInterruptManager->doHandleInterrupt(interrupt_number, esp);

    return esp;
}

// 在 asm/interrupt_stubs.s 中实现，所有中断的默认处理例程（什么都不做，忽略中断）
extern "C" void ignoreInterruptRequest();

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

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt, TaskManager *task_manager)
    : picMasterCommand(0x20)
    , picMasterData(0x21)
    , picSlaveCommand(0xa0)
    , picSlaveData(0xa1)
    , taskManager(task_manager)
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

    // 初始化 PIC
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
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)InterruptDescriptorTable;

    // 加载 idt
    asm volatile("lidt %0" : : "m"(idt));
}

InterruptManager::~InterruptManager()
{
}

// 激活中断
void InterruptManager::activate()
{
    if (ActiveInterruptManager != nullptr)
        ActiveInterruptManager->deactivate();

    ActiveInterruptManager = this;
    asm("sti");
}

// 禁止中断
void InterruptManager::deactivate()
{
    if (ActiveInterruptManager == this)
    {
        ActiveInterruptManager = nullptr;
        asm("cli");
    }
}

// 调用中断处理子程序
uint32_t InterruptManager::doHandleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    if (interrupt_number != 0x20)
        printf("Handle Interrupt %x\n", interrupt_number);

    // 如果设置了处理程序，则转向对应处理程序
    if (interruptHandlers[interrupt_number] != nullptr)
    {
        esp = interruptHandlers[interrupt_number]->handleInterrupt(esp);
    }
    else if (interrupt_number != 0x20)
    {
        // 未处理的中断
        printf("UNHANDLED INTERRUPT: %d", interrupt_number);
    }

    // 定时器中断
    if (interrupt_number == 0x20)
    {
        esp = (uint32_t)taskManager->schedule((CPUState *)esp);
    }

    if (0x20 <= interrupt_number && interrupt_number < 0x30)
    {
        picMasterCommand.write(0x20);

        if (0x28 <= interrupt_number)
            picSlaveCommand.write(0x20);
    }

    return esp;
}

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
