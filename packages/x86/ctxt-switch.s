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
 * from: points to where the RTOS core will store the execution context of the active task.
 *  This context switch implementation stores the execution context of the active task on its stack and returns only
 *  the stack pointer to the RTOS core.
 *  That makes `from` a pointer pointer and this function must store the stack pointer of the active task at `from`.
 * to: the task context to resume and switch to, i.e., the stack pointer to restore.
 *  This function must set the CPU's stack pointer to `to` and restore all execution state it previously stored on
 *  that stack.
 *
 * In its current implementation, this function does not store any particular task execution state on the stack.
 * The only data a call to this function pushes onto or removes from the stack is the return address and frame pointer
 * as per the x86 calling convention.
 * Therefore, this function effectively implements the following pseudo code:
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

    /* Per the calling convention, the callee is responsible for preserving EDI, ESI, and EBX */
    push %edi
    push %esi
    push %ebx

    /* Pseudo code: *from = stack_pointer;
     *  Move `from` pointer value into eax register
     *      0x14(%esp): first function argument `context_t *from`
     *      eax: used in next instruction */
    movl 0x14(%esp), %eax
    /*  Move CPU's stack pointer to memory address in eax, i.e., `*from`.
     *      esp: CPU's stack pointer
     *      eax: address value of the `from` pointer */
    movl %esp, (%eax)

    /* Pseudo code: stack_pointer = to;
     *  Set CPU's stack pointer to `to`
     *      0x18(%esp): second function argument `context_t to`
     *      esp: CPU's stack pointer
     * This switches the CPU's stack pointer to the one in the `to` argument.
     * In the current implementation, this is all that is necessary to restore the task state in `to`. */
    movl 0x18(%esp), %esp

    /* Restore EID, ESI, EBX that were stored at the beginning of the function */
    pop %ebx
    pop %esi
    pop %edi

    /* This is the standard x86 function appendix that tears down the stack frame and returns to the return address on
     * the stack, which would be the code location from where the `to` task called this function. */
    pop %ebp
    ret
