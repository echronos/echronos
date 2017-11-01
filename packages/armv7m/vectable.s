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

.syntax unified

/* See ARMv7M Architecture Reference Manual */
.set reset_register, 0xe000ed0c
.set reset_value, 0x05fa0004

.section .vectors, "a"
.global vector_table
vector_table:
        .word stack
        .word entry
        .word {{nmi}}
        .word {{hardfault}}
{{#mpu_enabled}}
        {{#memmanage}}.error "The memmanage vector is not available when memory protection is enabled"{{/memmanage}}
        .word rtos_internal_memmanage_handler
{{/mpu_enabled}}
{{^mpu_enabled}}
        {{#memmanage}}.word {{memmanage}}{{/memmanage}}
        {{^memmanage}}.word reset{{/memmanage}}
{{/mpu_enabled}}
        .word {{busfault}}
        .word {{usagefault}}
        .word reset
        .word reset
        .word reset
        .word reset
{{#preemption}}{{#svcall}}.error "The SVCall vector is not available on preemption-supporting systems"{{/svcall}}
        /* On preemption-supporting systems, we use the SVCall vector to implement a manual context switch triggered
         * from inside the RTOS. */
        .word rtos_internal_svc_handler
{{/preemption}}{{^preemption}}
        /* On non-preemption-supporting systems, the SVCall vector defaults to 'reset' if not specified by the system
         * configuration. */
{{#svcall}}
        .word {{svcall}}
{{/svcall}}
{{#mpu_enabled}}
        .word rtos_internal_svc_handler
{{/mpu_enabled}}
{{^svcall}}{{^mpu_enabled}}
        .word reset
{{/mpu_enabled}}{{/svcall}}
{{/preemption}}

        .word {{debug_monitor}}
        .word reset
{{#preemption}}{{#pendsv}}.error "The PendSV vector is not available on preemption-supporting systems"{{/pendsv}}
        /* On preemption-supporting systems, we use the PendSV vector to implement task preemption triggered by an
         * exception handler. */
        .word rtos_internal_pendsv_handler
{{/preemption}}{{^preemption}}
        /* On non-preemption-supporting systems, the PendSV vector defaults to 'reset' if not specified by the system
         * configuration. */
{{#pendsv}}
        .word {{pendsv}}
{{/pendsv}}
{{^pendsv}}
        .word reset
{{/pendsv}}
{{/preemption}}

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
        ldr r0, =data_load_addr
        ldr r1, =data_virt_addr
        ldr r2, =data_size
1:      cbz r2, 2f
        ldm r0!, {r3}
        stm r1!, {r3}
        sub r2, #4
        b 1b
2:

        /* Zero .bss section */
        ldr r1, =bss_virt_addr
        ldr r2, =bss_size
        mov r3, #0
1:      cbz r2, 2f
        stm r1!, {r3}
        subs r2, #4
        b 1b
2:

        b main
.size entry, .-entry
