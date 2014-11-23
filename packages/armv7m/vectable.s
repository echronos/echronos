.syntax unified

/* See ARMv7M Architecture Reference Manual */
.set reset_register, 0xe000ed0c
.set reset_value, 0x05fa0004

.section .vectors, "a"
.global rtos_internal_vector_table
rtos_internal_vector_table:
        .word rtos_internal_stack
        .word entry
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
        ldr r0, =reset_register
        ldr r1, =reset_value
        str r1, [r0]
        dsb
1:      b 1b
        .ltorg
.size reset, .-reset

/*
The entry function initialises the C run-time and then jumps to main. (Which should never return!)

Specifically, this loads the .data section from flash in to SRAM, and then zeros the .bss section.
*/
.type entry,#function
entry:
        /* Load .data section */
        ldr r0, =rtos_internal_data_load_addr
        ldr r1, =rtos_internal_data_virt_addr
        ldr r2, =rtos_internal_data_size
1:      cbz r2, 2f
        ldm r0!, {r3}
        stm r1!, {r3}
        sub r2, #4
        b 1b
2:

        /* Zero .bss section */
        ldr r1, =rtos_internal_bss_virt_addr
        ldr r2, =rtos_internal_bss_size
        mov r3, #0
1:      cbz r2, 2f
        stm r1!, {r3}
        subs r2, #4
        b 1b
2:

        b main
.size entry, .-entry
