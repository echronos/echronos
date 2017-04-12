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
        {{#rtos.memory_protection}}
        bl mpu_configure_for_current_task
        {{/rtos.memory_protection}}
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
 *{{#rtos.memory_protection}}
 * When memory protection is enabled, we must ensure that
 * we drop into user-mode before branching into our task.
 * Normally, this is the responsibility of API call wrappers,
 * however the first context switch into a function requires
 * us to explicitly drop privileges as we do here.
 *{{/rtos.memory_protection}} */
rtos_internal_trampoline:
        {{#rtos.memory_protection}}
        bl rtos_internal_drop_privileges
        {{/rtos.memory_protection}}
        blx r4
.size rtos_internal_trampoline, .-rtos_internal_trampoline

{{#rtos.memory_protection}}
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
{{/rtos.memory_protection}}
