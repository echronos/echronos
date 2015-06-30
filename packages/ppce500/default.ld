/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that no right, title
 * or interest in or to any trade mark, service mark, logo or trade name
 * of NICTA or its licensors is granted.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @TAG(NICTA_AGPL)
 */

OUTPUT_FORMAT("elf32-powerpc", "elf32-powerpc", "elf32-powerpcle")
OUTPUT_ARCH(powerpc)
ENTRY(rtos_internal_entry)
SECTIONS
{
        rtos_internal_ro_start = .;

        /* Some U-Boot builds discard sections at address 0, so separate this less useful section from .vectors */
        .undefined : AT ({{load_addr}})
        { *(.undefined) }

        .vectors : AT (LOADADDR(.undefined) + SIZEOF(.undefined))
        { *(.vectors) }

        .text : AT (LOADADDR(.vectors) + SIZEOF(.vectors))
        { *(.text.startup) *(.text) }

        .rodata : AT (LOADADDR (.text) + SIZEOF(.text))
        { *(.rodata*) }

        rtos_internal_ro_end = ALIGN(4);

        . = ALIGN(4);
        .data : AT ({{load_addr}} + (rtos_internal_ro_end - rtos_internal_ro_start))
        {
              *(.data)
              . = ALIGN(4);
        }

        rtos_internal_data_load_addr = LOADADDR(.data);
        rtos_internal_data_virt_addr = ADDR(.data);
        rtos_internal_data_size = . - ADDR(.data);

        /* Check addresses are correct */
        rtos_internal_dummy = ASSERT(rtos_internal_data_load_addr % 4 == 0, "data_load_addr not word aligned");
        rtos_internal_dummy = ASSERT(rtos_internal_data_virt_addr % 4 == 0, "data_virt_addr not word aligned");
        rtos_internal_dummy = ASSERT(rtos_internal_data_size % 4 == 0, "data_size not word aligned");

        .bss :
        {
             *(.bss)
             *(COMMON)
             . = ALIGN(4);
        }

        rtos_internal_bss_virt_addr = ADDR(.bss);
        rtos_internal_bss_size = SIZEOF(.bss);

        .stack : {
               . = . + {{stack_size}};
               rtos_internal_stack = .;
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
