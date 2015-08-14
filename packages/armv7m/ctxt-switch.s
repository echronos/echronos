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
