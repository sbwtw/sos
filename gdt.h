
#ifndef __GDT_H_
#define __GDT_H_

#include "types.h"

class GlobalDescriptorTable
{
public:
    class SegmentDescriptor
    {
    private:
        uint16_t limit_lo;
        uint16_t base_lo;
        uint8_t base_hi;
        uint8_t type;
        uint8_t flags_limit_hi;
        uint8_t base_vhi;

    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);

        uint32_t base();
        uint32_t limit();

    } __attribute__((packed));

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t codeSegmentSelector();
    uint16_t dataSegmentSelector();

private:
    SegmentDescriptor nullSegSelector;
    SegmentDescriptor unusedSegSelector;
    SegmentDescriptor codeSegSelector;
    SegmentDescriptor dataSegSelector;
};

#endif // __GDT_H_