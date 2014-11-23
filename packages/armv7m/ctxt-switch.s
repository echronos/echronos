.syntax unified
.section .text

/*
 * A subroutine must preserve the contents of the registers r4-r8,
 * r10, r11 and SP (and r9 in PCS variants that
 * designate r9 as v6).
 */

.global rtos_internal_context_switch
.type rtos_internal_context_switch,#function
/* void rtos_internal_context_switch(context_t *to, context_t *from); */
rtos_internal_context_switch:
        push {r4-r12,r14}
        str sp, [r1]
        /* fallthrough */


.global rtos_internal_context_switch_first
.type rtos_internal_context_switch_first,#function
/* void rtos_internal_context_switch_first(context_t *to); */
rtos_internal_context_switch_first:
        ldr sp, [r0]
        pop {r4-r12,pc}
.size rtos_internal_context_switch_first, .-rtos_internal_context_switch_first
.size rtos_internal_context_switch, .-rtos_internal_context_switch

.global rtos_internal_trampoline
.type rtos_internal_trampoline,#function
/*
 * This function does not really obey a standard C abi.
 * It is designed to be used in conjunction with the context
 * switch code for the initial switch to a particular task.
 * The tasks entry point is stored in 'r4'.
 */
rtos_internal_trampoline:
        blx r4
.size rtos_internal_trampoline, .-rtos_internal_trampoline
