.section .text
.global rtos_internal_wait_for_interrupt
.type rtos_internal_wait_for_interrupt,STT_FUNC
/* rtos_internal_wait_for_interrupt(void) */
rtos_internal_wait_for_interrupt:
        /* PowerPC e500 doesn't appear to implement the "wait" instruction, so we do things the e500 way.
         * The msync-mtmsr(WE)-isync sequence is explicitly recommended by the e500 Core Family Reference Manual. */
        mfmsr %r3
        oris %r3,%r3,0x4 /* Set 0x40000 = MSR[WE] to gate DOZE/NAP/SLEEP depending on how HID0 is set */
        ori %r3,%r3,0x8000 /* Set 0x8000 = MSR[EE] to enable noncritical external input interrupts */
        msync
        mtmsr %r3
        isync
        blr
.size rtos_internal_wait_for_interrupt, .-rtos_internal_wait_for_interrupt

.global rtos_internal_disable_interrupts
.type rtos_internal_disable_interrupts,STT_FUNC
/* rtos_internal_disable_interrupts(void) */
rtos_internal_disable_interrupts:
        wrteei 0 /* Clear MSR[EE] to disable noncritical external input interrupts */
        blr
.size rtos_internal_disable_interrupts, .-rtos_internal_disable_interrupts

.global rtos_internal_enable_interrupts
.type rtos_internal_enable_interrupts,STT_FUNC
/* rtos_internal_enable_interrupts(void) */
rtos_internal_enable_interrupts:
        wrteei 1 /* Set MSR[EE] to enable noncritical external input interrupts */
        blr
.size rtos_internal_enable_interrupts, .-rtos_internal_enable_interrupts

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
