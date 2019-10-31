
ENTRY(bootloader)

SECTIONS
{
    . = 0x0100000;

    .text :
    {
        *(.multiboot)
        *(.text*)
        *(.rodata)
        *(.rdata*) /* for windows PE format build */
    }

    .data :
    {
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )))
        end_ctors = .;

        *(.data)
    }

    .bss :
    {
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
    }
}