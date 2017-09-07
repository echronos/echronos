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
