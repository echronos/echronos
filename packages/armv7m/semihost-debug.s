.syntax unified
.section .text

.global rtos_internal_debug_putc
.type rtos_internal_debug_putc,#function

rtos_internal_debug_putc:
        push {r0, lr}
        mov r0, #3
        mov r1, sp
        bkpt 0xab
        pop {r0, pc}

.size rtos_internal_debug_putc, .-rtos_internal_debug_putc
