
#include "interrupts.h"
#include "std/stdio.h"
#include "multitasking.h"

#define IRQ0_SYS_TIMER  0x20
#define IRQ1_KEYBOARD   0x21

#define IDT32_INTERRUPT_GATE    0xE
#define IDT32_TRAP_GATE         0xF
#define IDT32_DPL_RING0         0x0
#define IDT32_DPL_RING3         0x3

/**
 *  TypeAttr:
 *
 *  +---+---+---+---+---+---+---+---+---+
 *  | P |  DPL  | S |     GateType      |
 *  +---+---+---+---+---+---+---+---+---+
 *
 *  P: Set to 0 for unused interrupts.
 *  DPL: Gate call protection. Specifies which privilege Level the calling Descriptor minimum should have.
 *       So hardware and CPU interrupts can be protected from being called out of userspace.
 *  S: Set to 0 for interrupt and trap gates (see below).
 *  GateType: Possible IDT gate types :
 *          0b0101 	0x5 	5 	80386 32-bit task gate
 *          0b0110 	0x6 	6 	80286 16-bit interrupt gate
 *          0b0111 	0x7 	7 	80286 16-bit trap gate
 *          0b1110 	0xE 	14 	80386 32-bit interrupt gate
 *          0b1111 	0xF 	15 	80386 32-bit trap gate
 *
 *  Here are some pre-cooked type_attr values people are likely to use (assuming DPL=0):
 *  32-bit Interrupt gate: 0x8E ( P=1, DPL=00b, S=0, type=1110b => type_attr=1000_1110b=0x8E)
 *  32-bit Trap gate: 0x8F ( P=1, DPL=00b, S=0, type=1111b => type_attr=1000_1111b=0x8F)
 *
 *  Thus, Trap and Interrupt gate descriptors hold the following data (other than type_attr):
 *  32-bit offset into that segment - address of the handler, where execution will be transferred
 *  16-bit selector of a code segment in GDT or LDT
 * */

/**
 *  BitFields mean: https://wiki.osdev.org/Interrupt_Descriptor_Table
 * */

// IA-32
struct GateDescriptor
{
    uint16_t handlerAddressLowBits;     // offset bits [0, 15]
    uint16_t gdt_codeSegmentSelector;   // a code segment selector in GDT or LDT
                                        // Selector of the interrupt function (to make sense - the kernel's selector).
                                        // The selector's descriptor's DPL field has to be 0 so the iret instruction
                                        // won't throw a #GP exeption when executed.
    uint8_t _reserved;                  // unused, set to 0
    uint8_t type_attr;                  // type and attributes
    uint16_t handlerAddressHighBits;    // offset bits [16, 31]
} __attribute__((packed));


// idt 描述
struct InterruptDescriptorTablePointer
{
    uint16_t size;      // length
    uint32_t base;      // base address
} __attribute__((packed));

static InterruptManager *ActiveInterruptManager = nullptr;
static GateDescriptor InterruptDescriptorTable[256];

// 对所有中断的声明，在 interrupt_stub.s 中实现，调回到 handleInterrupt 函数中由 C++ 处理
// C++ 中会调用 interruptHandlers 中的处理函数
extern "C" void handleInterruptRequest0x00();
extern "C" void handleInterruptRequest0x01();
extern "C" void handleInterruptRequest0x0c();

// 用于在 asm/interrupt_stubs.s 接收中断，将中断转回 C++ 中处理
extern "C" void * __attribute__((cdecl)) handleInterrupt(uint8_t interrupt_number, CpuRegisters *state)
{
    if (ActiveInterruptManager != nullptr)
        return ActiveInterruptManager->doHandleInterrupt(interrupt_number, state);

    return state;
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
    InterruptDescriptorTable[interrupt_number].type_attr = IDT_DESC_PRESENT | descriptorType | ((descriptorPrivilegeLevel & 3) << 5);
    InterruptDescriptorTable[interrupt_number]._reserved = 0;
}

InterruptManager::InterruptManager(TaskManager *task_manager)
    // 主片端口 0x20 0x21
    : picMasterCommand(0x20)
    , picMasterData(0x21)
    // 从片端口 0xa0 0xa1
    , picSlaveCommand(0xa0)
    , picSlaveData(0xa1)
    , taskManager(task_manager)
{
    uint16_t code_segment = get_code_segment();

    for (uint16_t i(0); i != 256; ++i)
    {
        interruptHandlers[i] = nullptr;
        setInterruptDescriptorTableEntry(i, code_segment, &ignoreInterruptRequest, IDT32_DPL_RING0, IDT32_INTERRUPT_GATE);
    }

    setInterruptDescriptorTableEntry(0x20, code_segment, &handleInterruptRequest0x00, IDT32_DPL_RING0, IDT32_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x21, code_segment, &handleInterruptRequest0x01, IDT32_DPL_RING0, IDT32_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2c, code_segment, &handleInterruptRequest0x0c, IDT32_DPL_RING0, IDT32_INTERRUPT_GATE);

    // 初始化 8259A PIC 主从片
    picMasterCommand.write(0x11);
    picSlaveCommand.write(0x11);

    // 设置主片 IRQ 从 0x20(32) 开始
    picMasterData.write(0x20);
    // 设置从片 IRQ 从 0x28(40) 开始
    picSlaveData.write(0x28);

    // 设置主片 IR2 引脚连接从片
    picMasterData.write(0x04);
    // 设置从片输出引脚连接 IR2
    picSlaveData.write(0x02);

    // 主从设置 8086 工作方式
    picMasterData.write(0x01);
    picSlaveData.write(0x01);

    // 主从开中断
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
CpuRegisters * InterruptManager::doHandleInterrupt(uint8_t interrupt_number, CpuRegisters *state)
{
    //if (interrupt_number != 0x20)
        //printf("Handle Interrupt %x\n", interrupt_number);

    // 如果设置了处理程序，则转向对应处理程序
    if (interruptHandlers[interrupt_number] != nullptr)
    {
        state = interruptHandlers[interrupt_number]->handleInterrupt(state);
    }
    else if (interrupt_number != IRQ0_SYS_TIMER)
    {
        // 未处理的中断
        printf("UNHANDLED INTERRUPT: %d", interrupt_number);
    }

    // 定时器中断
    if (interrupt_number == IRQ0_SYS_TIMER)
    {
        state = taskManager->schedule(state);
    }

    if (0x20 <= interrupt_number && interrupt_number < 0x30)
    {
        picMasterCommand.write(0x20);

        if (0x28 <= interrupt_number)
            picSlaveCommand.write(0x20);
    }

    return state;
}

InterruptHandler::InterruptHandler(uint8_t interrupt_number, InterruptManager *interrupt_manager)
    : interruptNumber(interrupt_number)
    , interruptManager(interrupt_manager)
{
    // 添加自己为中断服务程序
    interruptManager->interruptHandlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler()
{
    // 移除当前中断
    if (interruptManager->interruptHandlers[interruptNumber] == this)
        interruptManager->interruptHandlers[interruptNumber] = nullptr;
}

CpuRegisters * InterruptHandler::handleInterrupt(CpuRegisters *state)
{
    return state;
}
