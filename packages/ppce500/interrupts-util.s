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
.global rtos_internal_interrupts_wait
.type rtos_internal_interrupts_wait,STT_FUNC
/* rtos_internal_interrupts_wait(void) */
rtos_internal_interrupts_wait:
        /* PowerPC e500 doesn't appear to implement the "wait" instruction, so we do things the e500 way.
         * The msync-mtmsr(WE)-isync sequence is explicitly recommended by the e500 Core Family Reference Manual. */
        mfmsr %r3
        oris %r3,%r3,0x4 /* Set 0x40000 = MSR[WE] to gate DOZE/NAP/SLEEP depending on how HID0 is set */
        ori %r3,%r3,0x8000 /* Set 0x8000 = MSR[EE] to enable noncritical external input interrupts */
        msync
        mtmsr %r3
        isync
        blr
.size rtos_internal_interrupts_wait, .-rtos_internal_interrupts_wait

.global rtos_internal_interrupts_disable
.type rtos_internal_interrupts_disable,STT_FUNC
/* rtos_internal_interrupts_disable(void) */
rtos_internal_interrupts_disable:
        wrteei 0 /* Clear MSR[EE] to disable noncritical external input interrupts */
        blr
.size rtos_internal_interrupts_disable, .-rtos_internal_interrupts_disable

.global rtos_internal_interrupts_enable
.type rtos_internal_interrupts_enable,STT_FUNC
/* rtos_internal_interrupts_enable(void) */
rtos_internal_interrupts_enable:
        wrteei 1 /* Set MSR[EE] to enable noncritical external input interrupts */
        blr
.size rtos_internal_interrupts_enable, .-rtos_internal_interrupts_enable

.global rtos_internal_check_interrupts_enabled
.type rtos_internal_check_interrupts_enabled,STT_FUNC
/* bool rtos_internal_check_interrupts_enabled(void) */
rtos_internal_check_interrupts_enabled:
        mfmsr %r3
        andi. %r3,%r3,0x8000;
        cmpi 0,%r3,0
        beq 1f
        li %r3,1
1:
        blr
.size rtos_internal_check_interrupts_enabled, .-rtos_internal_check_interrupts_enabled
