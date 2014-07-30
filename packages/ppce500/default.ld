OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpcle")
OUTPUT_ARCH(powerpc)
ENTRY(_entry)
SECTIONS
{
        .ro_start = .;
        .vectors :
        { *(.vectors) }

        .text :
        { *(.text.startup) *(.text) }

        .rodata :
        { *(.rodata*) }

        .ro_end = ALIGN(4);

        .data :
        {
              *(.data)
              . = ALIGN(4);
        }

        .bss :
        {
             *(.bss)
             *(COMMON)
             . = ALIGN(4);
        }

        .stack : {
               . = . + {{stack_size}};
               _stack = .;
        }

        /* Debug sections */

        .debug_aranges  0 : { *(.debug_aranges) }
        .debug_pubnames 0 : { *(.debug_pubnames) }

        .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
        .debug_abbrev   0 : { *(.debug_abbrev) }
        .debug_line     0 : { *(.debug_line) }
        .debug_frame    0 : { *(.debug_frame) }
        .debug_str      0 : { *(.debug_str) }
        .debug_loc      0 : { *(.debug_loc) }
        .debug_macinfo  0 : { *(.debug_macinfo) }

        .debug_pubtypes 0 : { *(.debug_pubtypes) }
        .debug_ranges   0 : { *(.debug_ranges) }

        .debug_macro    0 : { *(.debug_macro) }

        /DISCARD/ : { *(*) }
}