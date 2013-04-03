.syntax unified
.section .vectors, "a"
.global _vector_table
_vector_table:
        .word _stack
        .word _entry
        .word {{nmi}}
        .word {{hardfault}}
        .word {{memmanage}}
        .word {{busfault}}
        .word {{usagefault}}
        .word reset
        .word reset
        .word reset
        .word reset
        .word {{svcall}}
        .word {{debug_monitor}}
        .word reset
        .word {{pendsv}}
        .word {{systick}}
{{#external_irqs}}
        .word {{handler}}
{{/external_irqs}}

.section .text
.type reset,#function
reset:
        ldr r0, =0xe000ed0c
        ldr r1, =0x5fa00004
        str r1, [r0]
        dsb
1:      b 1b
        .ltorg
.size reset, .-reset

/*
The _entry function initialises the C run-time and then jumps to main. (Which should never return!)

Specifically, this loads the .data section from flash in to SRAM, and then zeros the .bss section.
*/
.type _entry,#function
_entry:
        /* Load .data section */
        ldr r0, =_data_load_addr
        ldr r1, =_data_virt_addr
        ldr r2, =_data_size
1:      cbz r2, 2f
        ldm r0!, {r3}
        stm r1!, {r3}
        sub r2, #4
        b 1b
2:

        /* Zero .bss section */
        ldr r1, =_bss_virt_addr
        ldr r2, =_bss_size
        mov r3, #0
1:      cbz r2, 2f
        stm r1!, {r3}
        subs r2, #4
        b 1b
2:

        b main
.size _entry, .-_entry
