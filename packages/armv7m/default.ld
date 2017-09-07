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
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(vector_table)

MEMORY
{
        /* Bounds of physical flash on the device */
        flash_region(rx) : ORIGIN = {{flash_addr}}, LENGTH = {{flash_size}}

        /* Area of flash we are actually able to put our code in (i.e. excluding bootloaders
         * or similar that might exist at the start of our physical flash region) */
        flash_load_region(rx) :
            ORIGIN = {{flash_load_addr}},
            LENGTH = {{flash_size}} - ({{flash_load_addr}} - {{flash_addr}})

        /* Bounds of physical SRAM on the device */
        sram_region(rw) : ORIGIN = {{sram_addr}}, LENGTH = {{sram_size}}
}

SECTIONS
{
        . = {{code_addr}};

        .vectors :
        {
            *(.vectors)
        } AT > flash_load_region

        .text :
        {
            *(.text.startup)
            *(.text)
            . = ALIGN(4);
        } AT > flash_load_region

        .rodata :
        {
            *(.rodata*)
            . = ALIGN(4);
        } AT > flash_load_region

        .data.bitband :
        {
            *(.data.bitband)
            . = ALIGN(4);
        } > sram_region AT > flash_load_region

        /* Check that bitband variables don't overflow */
        dummy = ASSERT(SIZEOF(.data.bitband) <= {{bitband_size}}, "bitband variables too large");

        .data :
        {
            *(.data)
            . = ALIGN(4);
        } > sram_region AT > flash_load_region

        data_load_addr = LOADADDR(.data.bitband);
        data_virt_addr = ADDR(.data.bitband);
        data_size = SIZEOF(.data.bitband) + SIZEOF(.data);

        /* Check addresses are correct */
        dummy = ASSERT(data_load_addr % 4 == 0, "data_load_addr not word aligned");
        dummy = ASSERT(data_virt_addr % 4 == 0, "data_virt_addr not word aligned");
        dummy = ASSERT(data_size % 4 == 0, "data_size not word aligned");

        .bss :
        {
            *(.bss)
            *(COMMON)
            . = ALIGN(4);
        } > sram_region

        bss_virt_addr = ADDR(.bss);
        bss_size = SIZEOF(.bss);

        .stack : {
            . = . + {{stack_size}};
            stack = .;
        } > sram_region

        /* Aliases for all the bitband variables */
        {{#bit_aliases}}
        {{.}}_bitband = {{bitband_alias}} + (({{.}} - {{bitband_base}}) * 32);
        {{/bit_aliases}}

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
