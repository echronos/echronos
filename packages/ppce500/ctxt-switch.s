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

.section .text

/*
 * Based on function prologue/epilogue example given in PowerPC EABI documentation.
 * WARNING: The constants used below MUST match the context frame layout defined in context-switch-ppce500 component!
 */

.global rtos_internal_context_switch
.type rtos_internal_context_switch,STT_FUNC
/* void rtos_internal_context_switch(context_t *to, context_t *from); */
rtos_internal_context_switch:
        mflr %r0            /* Get lr */
        stwu %r1,-80(%r1)   /* Move sp to create new frame (r14-r31 + lr + sp) & save old sp in its back chain word */
        stw  %r0,+84(%r1)   /* Save lr in LR save word of previous stack frame */
        stmw %r14,+8(%r1)   /* Save 18 non-volatiles (r14-r31) to stack */
        stw  %r1,0(%r4)     /* Write sp into "from" argument (r4) */
        /* fallthrough */

.global rtos_internal_context_switch_first
.type rtos_internal_context_switch_first,STT_FUNC
/* void rtos_internal_context_switch_first(context_t *to); */
rtos_internal_context_switch_first:
        lwz  %r1,0(%r3)     /* Restore sp from "to" argument (r3) */
        lwz  %r0,+84(%r1)   /* Get saved lr from LR save word of previous stack frame */
        mtlr %r0            /* Restore lr */
        lmw  %r14,+8(%r1)   /* Restore 18 non-volatiles (r14-r31) from stack */
        addi %r1,%r1,80     /* Move sp to remove stack frame */
        blr                 /* Branch to lr */

.size rtos_internal_context_switch_first, .-rtos_internal_context_switch_first
.size rtos_internal_context_switch, .-rtos_internal_context_switch
