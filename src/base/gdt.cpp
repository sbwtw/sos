
#include "gdt.h"
#include "std/stdio.h"

/**
 * https://wiki.osdev.org/Global_Descriptor_Table
 * https://wiki.osdev.org/Getting_to_Ring_3
 * */

/**
 *  Pr: Present bit. This must be 1 for all valid selectors.
 *  Privl: Privilege, 2 bits. Contains the ring level, 0 = highest (kernel), 3 = lowest (user applications).
 *  S: Descriptor type. This bit should be set for code or data segments and should be cleared for system segments (eg. a Task State Segment)
 *  Ex: Executable bit. If 1 code in this segment can be executed, ie. a code selector. If 0 it is a data selector.
 *  DC: Direction bit/Conforming bit.
 *      Direction bit for data selectors: Tells the direction. 0 the segment grows up. 1 the segment grows down, ie. the offset has to be greater than the limit.
 *      Conforming bit for code selectors:
 *          If 1 code in this segment can be executed from an equal or lower privilege level. For example, code in ring 3 can far-jump to conforming code in a ring 2 segment. The privl-bits represent the highest privilege level that is allowed to execute the segment. For example, code in ring 0 cannot far-jump to a conforming code segment with privl==0x2, while code in ring 2 and 3 can. Note that the privilege level remains the same, ie. a far-jump form ring 3 to a privl==2-segment remains in ring 3 after the jump.
 *          If 0 code in this segment can only be executed from the ring set in privl.
 *  RW: Readable bit/Writable bit.
 *      Readable bit for code selectors: Whether read access for this segment is allowed. Write access is never allowed for code segments.
 *      Writable bit for data selectors: Whether write access for this segment is allowed. Read access is always allowed for data segments.
 *  Ac: Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed.
 *  Gr: Granularity bit. If 0 the limit is in 1 B blocks (byte granularity), if 1 the limit is in 4 KiB blocks (page granularity).
 *  Sz: Size bit. If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode. You can have both 16 bit and 32 bit selectors at once.
 *
 *  x86-64 Changes
 *
 *  L bit (bit 53, next to Sz) is used to indicate x86-64 code descriptor. This bit is reserved for data segments
 *  Sz bit (bit 54) has to be 0 when the L bit is set, as the combination Sz = 1, L = 1 is reserved for future use (and will throw an exception if you try to use it)
 *
 *  limit_hi: Limit[16:19]
 * */
struct SegmentDescriptor {
    uint16_t limit_lo;          // bit 0~15, Limit[0:15] 低 16 字节
    uint16_t base_lo;           // bit 16~31, Base[0:15] 低 16 字节
    uint8_t base_mid;           // bit 32~39, Base[16:23] 中间 8 字节
    bool ac: 1;                 // bit 40, 必须设置为 0, cpu 访问此段时被设置为 1
    bool rw: 1;                 // bit 41, 代码段是否可读或数据段是否可写
    bool dc: 1;                 // bit 42, 对于数据段: 1 向下增长, 0 向上增长
                                //         对于代码段: 1 可由低权限执行, 0 只能由当前权限执行
    bool ex: 1;                 // bit 43, 1 代表此段为代码段, 0 代表为数据段
    bool s:  1;                 // bit 44, 系统段设置 0, 数据或代码段设置 1
    short privl: 2;             // bit 45~46, 权限, 0 = kernel, 3 = user
    bool pr: 1;                 // bit 47, 有效的段设置为 1
    uint8_t limit_hi: 4;        // bit 48~51, Limit[16:19]
    bool _reversed: 1;          // bit 52, 保留, 设置为 0
    bool l: 1;                  // bit 53, 设置为 1 表明这是 x86-64 代码段
    bool sz: 1;                 // bit 54, 0 代表 16 位保护模式, 1 代表 32 位保护模式
                                //         在 x86-64 下当 l 设置为 1 时这里应该为 0, Sz = 1, L = 1 保留
    bool gr: 1;                 // bit 55, 0 代表 limit 单位是 1B, 1 代表 limit 单位是 4KiB
    uint8_t base_hi;            // bit 56~63, Base[24:31]
} __attribute__((packed));

struct GDTDescriptor {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

SegmentDescriptor GDT_TABLE[4];

void set_segment_descriptor(SegmentDescriptor *s, uint32_t base, uint32_t limit)
{
    s->sz = 1;
    if (limit > 65536)
    {
        s->gr = 1;

        if ((limit & 0xfff) != 0xfff)
            limit = (limit >> 12) - 1;
        else
            limit = limit >> 12;
    }

    s->limit_lo = limit & 0xffff;
    s->limit_hi = (limit >> 16) & 0xf;

    s->base_lo = base & 0xffff;
    s->base_mid = (base >> 16) & 0xff;
    s->base_hi = (base >> 24) & 0xff;
}

void gdt_init()
{
    // init gdt
    memset(GDT_TABLE, 0, sizeof(GDT_TABLE));

    // null segment 0 0 0
    SegmentDescriptor &null_seg = GDT_TABLE[0];
    set_segment_descriptor(&null_seg, 0, 0);

    // unused segment 0 0 0
    SegmentDescriptor &unused_seg = GDT_TABLE[1];
    set_segment_descriptor(&unused_seg, 0, 0);

    // code segment 0 64*1024*1024 0x9a
    SegmentDescriptor &code_seg = GDT_TABLE[2];
    set_segment_descriptor(&code_seg, 0, 64 * 1024 * 1024);
    code_seg.rw = 1;
    code_seg.ex = 1;
    code_seg.s = 1;
    code_seg.pr = 1;

    // data segment 0 64*1024*1024 0x92
    SegmentDescriptor &data_seg = GDT_TABLE[3];
    set_segment_descriptor(&data_seg, 0, 64 * 1024 * 1024);
    data_seg.rw = 1;
    data_seg.s = 1;
    data_seg.pr = 1;

    // load gdt to cpu
    GDTDescriptor gdt { sizeof(GDT_TABLE), (uint32_t)GDT_TABLE };

    asm volatile ("lgdt (%0)" : : "p"((uint8_t *)&gdt));
}

uint16_t get_code_segment()
{
    return sizeof(SegmentDescriptor) * 2;
}

uint16_t get_data_segment()
{
    return sizeof(SegmentDescriptor) * 3;
}
