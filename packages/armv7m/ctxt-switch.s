.syntax unified
.section .text

/*
 * A subroutine must preserve the contents of the registers r4-r8,
 * r10, r11 and SP (and r9 in PCS variants that
 * designate r9 as v6).
 */

.global armv7m_context_switch
.type armv7m_context_switch,#function
/* void armv7m_context_switch(context_t *to, context_t *from); */
armv7m_context_switch:
        push {r4-r12,r14}
        str sp, [r1]
        /* fallthrough */


.global armv7m_context_switch_first
.type armv7m_context_switch_first,#function
/* void armv7m_context_switch_first(context_t *to); */
armv7m_context_switch_first:
        ldr sp, [r0]
        pop {r4-r12,pc}
.size armv7m_context_switch_first, .-armv7m_context_switch_first
.size armv7m_context_switch, .-armv7m_context_switch

.global armv7m_trampoline
.type armv7m_trampoline,#function
/*
 * This function does not really obey a standard C abi.
 * It is designed to be used in conjunction with the context
 * switch code for the initial switch to a particular task.
 * The tasks entry point is stored in 'r4'.
 */
armv7m_trampoline:
        blx r4
.size armv7m_trampoline, .-armv7m_trampoline
