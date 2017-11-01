/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2017, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */

OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpcle")
OUTPUT_ARCH(powerpc)
ENTRY(entry)
SECTIONS
{
        ro_start = .;

        /* Some U-Boot builds discard sections at address 0, so separate this less useful section from .vectors */
        .undefined : AT ({{load_addr}})
        { *(.undefined) }

        .vectors : AT (LOADADDR(.undefined) + SIZEOF(.undefined))
        { *(.vectors) }

        .text : AT (LOADADDR(.vectors) + SIZEOF(.vectors))
        { *(.text.startup) *(.text) }

        .rodata : AT (LOADADDR (.text) + SIZEOF(.text))
        { *(.rodata*) }

        ro_end = ALIGN(4);

        . = ALIGN(4);
        .data : AT ({{load_addr}} + (ro_end - ro_start))
        {
              *(.data)
              . = ALIGN(4);
        }

        data_load_addr = LOADADDR(.data);
        data_virt_addr = ADDR(.data);
        data_size = . - ADDR(.data);

        /* Check addresses are correct */
        dummy = ASSERT(data_load_addr % 4 == 0, "data_load_addr not word aligned");
        dummy = ASSERT(data_virt_addr % 4 == 0, "data_virt_addr not word aligned");
        dummy = ASSERT(data_size % 4 == 0, "data_size not word aligned");

        .bss :
        {
             *(.bss)
             *(COMMON)
             . = ALIGN(4);
        }

        bss_virt_addr = ADDR(.bss);
        bss_size = SIZEOF(.bss);

        .stack : {
               . = . + {{stack_size}};
               stack = .;
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
