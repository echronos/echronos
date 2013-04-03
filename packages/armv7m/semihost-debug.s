.syntax unified
.section .text

.global armv7m_semihost_debug_putc
.type armv7m_semihost_debug_putc,#function

armv7m_semihost_debug_putc:
        push {r0, lr}
        mov r0, #3
        mov r1, sp
        bkpt 0xab
        pop {r0, pc}

.size armv7m_semihost_debug_putc, .-armv7m_semihost_debug_putc
