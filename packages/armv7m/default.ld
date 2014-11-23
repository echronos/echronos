OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(rtos_internal_vector_table)
SECTIONS
{
        . = {{code_addr}};
        rtos_internal_ro_start = .;
        .vectors : AT ({{flash_load_addr}})
        { *(.vectors) }

        .text : AT (LOADADDR(.vectors) + SIZEOF(.vectors))
        { *(.text.startup) *(.text) }

        .rodata : AT (LOADADDR (.text) + SIZEOF(.text))
        { *(.rodata*) }

        rtos_internal_ro_end = ALIGN(4);

        . = {{data_addr}};
        .data.bitband : AT ({{flash_load_addr}} + (rtos_internal_ro_end - rtos_internal_ro_start))
        {
              *(.data.bitband)
        }

        /* Check that bitband variables don't overflow */
        rtos_internal_dummy = ASSERT(. <= {{data_addr}} + {{bitband_size}}, "bitband variables too large");

        .data : AT (LOADADDR(.data.bitband) + SIZEOF(.data.bitband))
        {
              *(.data)
              . = ALIGN(4);
        }

        rtos_internal_data_load_addr = LOADADDR(.data.bitband);
        rtos_internal_data_virt_addr = ADDR(.data.bitband);
        rtos_internal_data_size = . - ADDR(.data.bitband);

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
