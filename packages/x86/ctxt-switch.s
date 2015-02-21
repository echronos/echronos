/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that no right, title
 * or interest in or to any trade mark, service mark, logo or trade name
 * of NICTA or its licensors is granted.
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

/* Implementation of the function void rtos_internal_context_switch_x86(context_t *from, context_t to);
 *
 * from: points to the where the RTOS implementation stores the task state/context of the active, calling task.
 *  This function must store all active state of the active task at `from`.
 *  This allows the RTOS to resume this task at a later point in time.
 * to: the task context to resume and switch to.
 *  This function must restore all state of the `to` context and then resume it, i.e., return to it.
 *
 * In this current implementation, the only task state/context of a task is its stack pointer.
 * Therefore, the type "context_t" means "stack pointer".
 * This function effectively implements the following pseudo code:
 *      *from = stack_pointer;
 *      stack_pointer = to;
 *
 * If necessary in the future, this function might need to manage additional task state, such as that of the floating
 * point unit.
 */
.globl _rtos_internal_context_switch_x86    /* name mangling to match the gcc calling convention on Windows */
.globl rtos_internal_context_switch_x86     /* no name mangling with gcc on Linux */
_rtos_internal_context_switch_x86:
rtos_internal_context_switch_x86:
    /* This is the standard x86 function preamble that sets up the stack frame.
     * It is not strictly necessary to implement the context-switch functionality, but keeps gdb happy. */
    push %ebp
    movl %esp, %ebp

    /* Pseudo code: *from = stack_pointer;
     *  Move `from` pointer value into eax register
     *      0x8(%esp): first function argument `context_t *from`
     *      eax: used in next instruction */
    movl 0x8(%esp), %eax
    /*  Move current stack pointer into memory address in eax
     *      esp: current stack pointer (i.e., the context of the active task)
     *      eax: address value of the `from` pointer */
    movl %esp, (%eax)

    /* Pseudo code: stack_pointer = to;
     *  Set active stack pointer to `to`
     *      0xc(%esp): second function argument `context_t to'
     *      esp: current stack pointer
     * This switches the active stack pointer to the one in the `to` argument.
     * In the current implementation, this is all that is necessary to restore the task state in `to`. */
    movl 0xc(%esp), %esp

    /* This is the standard x86 function appendix that tears down the stack frame and returns to the return address on
     * the stack, which would be the code location from where the `to` task called this function. */
    pop %ebp
    ret
