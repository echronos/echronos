/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
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
        dummy = ASSERT({{flash_size}} == (1 << LOG2CEIL({{flash_size}})),
            "Flash region has size that is not power-of-2.");

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

        {{#rtos.memory_protection}}
        /* Sections that encapsulate protection domains.
         *
         * SYMBOL DOMAINS:
         * This includes symbols that would normally occupy the .bss or .data
         * segment. They are combined into one section as separating the two
         * would give us half as many simultaneous protection domains. We
         * accept the flash usage tradeoff here as memory is cheaper than MPU
         * regions on this architecture. (normally .bss symbols do not use up
         * any flash as they are zeroed on initialization)
         *
         * We must align to the power-of-2 domain size at the beginning
         * and at the end of each domain to ensure that the MPU region we set up
         * encompasses the entire protection region, and nothing else.
         *
         * One can include symbols by specifying either a c identifier or
         * an object file. In practice, it should be very difficult to get
         * these confused as an object file is an invalid c identifier, and
         * a c identifier is much more narrow-grained then an object file.
         *
         * ADDRESS DOMAINS:
         * These simply use a base address and size to declare a domain. */
        {{#rtos.protection_domains}}

        dummy = ASSERT({{domain_size}} == (1 << LOG2CEIL({{domain_size}})),
            "Protection domain {{name}} has size that is not power-of-2.");

        {{#base_address}}
        /* Address domain */
        dummy = ASSERT({{base_address}} % {{domain_size}} == 0,
            "Address protection domain {{name}} has base address not aligned to its size");

        linker_domain_{{name}}_start = {{base_address}};
        linker_domain_{{name}}_size = {{domain_size}};
        {{/base_address}}

        {{^base_address}}
        /* Symbol domain */
        .domain_{{name}} :
        {
            . = ALIGN({{domain_size}});
            linker_domain_{{name}}_start = .;
            {{#symbols}}
            *(.data.{{.}} .bss.{{.}}) /* C identifier? put here */
            *{{.}} (.data* .bss*)     /* Object file? put here */
            {{/symbols}}
            BYTE(0xEE)                /* Only way to reliably force section contents
                                         to appear in the output ELF. TODO: Fix */
            . = ALIGN({{domain_size}});
        } > sram_region AT > flash_load_region

        linker_domain_{{name}}_size = . - linker_domain_{{name}}_start;

        dummy = ASSERT(linker_domain_{{name}}_size > 0,
            "No symbols (or only unused symbols) in protection domain: {{name}}");
        dummy = ASSERT(linker_domain_{{name}}_size == {{domain_size}},
            "Failed to fit symbols in protection domain: {{name}}");
        {{/base_address}}

        {{/rtos.protection_domains}}
        {{/rtos.memory_protection}}

        .data :
        {
            *(.data*)
            . = ALIGN(4);
        } > sram_region AT > flash_load_region

        data_load_addr = LOADADDR(.data.bitband);
        data_virt_addr = ADDR(.data.bitband);
        data_size = SIZEOF(.data.bitband) + SIZEOF(.data)
            {{#rtos.memory_protection}}
            /* We must increase the .data size by our symbol protection domain sizes
             * so that they are appropriately initialized when the RTOS starts */
            {{#rtos.protection_domains}}
            {{^base_address}}
            + SIZEOF(.domain_{{name}})
            {{/base_address}}
            {{/rtos.protection_domains}}
            {{/rtos.memory_protection}}
            ;

        /* Check addresses are correct */
        dummy = ASSERT(data_load_addr % 4 == 0, "data_load_addr not word aligned");
        dummy = ASSERT(data_virt_addr % 4 == 0, "data_virt_addr not word aligned");
        dummy = ASSERT(data_size % 4 == 0, "data_size not word aligned");

        .bss :
        {
            {{#rtos.memory_protection}}
            {{#rtos.tasks}}
            dummy = ASSERT({{stack_size}} == (1 << LOG2CEIL({{stack_size}})),
                           "Stack size for task: {{name}} is not a power-of-2.");
            *(.bss.stack.task_{{idx}})
            {{/rtos.tasks}}
            {{/rtos.memory_protection}}
            *(.bss*)
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

        {{#rtos.memory_protection}}
        /* Constants used by the MPU for run-time assertions. Necessary here
         * because these are properties of the vectable module, not the RTOS */
        linker_flash_start = {{flash_load_addr}};
        linker_flash_size = {{flash_size}};
        linker_sram_size = {{sram_size}};
        {{/rtos.memory_protection}}

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
