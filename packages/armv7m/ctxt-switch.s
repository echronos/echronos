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

/*<module>
  <code_gen>template</code_gen>
</module>*/


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
        {{#rtos.mpu_enabled}}
        bl rtos_internal_mpu_configure_for_current_task
        {{/rtos.mpu_enabled}}
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
 *{{#rtos.mpu_enabled}}
 * When memory protection is enabled, we must ensure that
 * we drop into user-mode before branching into our task.
 * Normally, this is the responsibility of API call wrappers,
 * however the first context switch into a function requires
 * us to explicitly drop privileges as we do here.
 *{{/rtos.mpu_enabled}} */
rtos_internal_trampoline:
        {{#rtos.mpu_enabled}}
        bl rtos_internal_drop_privileges
        {{/rtos.mpu_enabled}}
        blx r4
.size rtos_internal_trampoline, .-rtos_internal_trampoline

{{#rtos.mpu_enabled}}
.global rtos_internal_elevate_privileges
.type rtos_internal_elevate_privileges,#function
rtos_internal_elevate_privileges:
    /* 0 is used for pre-emption on other variants, so we
     * use 1 to indicate an svc for a privilege raise request */
    svc #1
    /* At this point we are running in privileged mode */
    mov pc, lr
.size rtos_internal_elevate_privileges, .-rtos_internal_elevate_privileges

.global rtos_internal_drop_privileges
.type rtos_internal_drop_privileges,#function
rtos_internal_drop_privileges:
    mrs r0, control
    orr r0, r0, #1
    msr control, r0
    mov pc, lr
.size rtos_internal_drop_privileges, .-rtos_internal_drop_privileges

.global rtos_internal_in_usermode
.type rtos_internal_in_usermode,#function
rtos_internal_in_usermode:
    mrs r0, control
    mov pc, lr
.size rtos_internal_in_usermode, .-rtos_internal_in_usermode

.global rtos_internal_svc_handler
.type rtos_internal_svc_handler,#function
/* Elevates the processor into privileged mode and continues execution
 * TODO: Use a linker symbol defining RTOS bounds to ensure the call
 * originates from RTOS code. */
rtos_internal_svc_handler:
    /* Switch to privileged mode */
    mrs r0, control
    bic r0, r0, #1
    msr control, r0
    /* RFE to straight after the offending svc call */
    bx lr
.size rtos_internal_svc_handler, .-rtos_internal_svc_handler
{{/rtos.mpu_enabled}}
