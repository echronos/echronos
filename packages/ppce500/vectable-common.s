.include "vectable-common.h"
.section .text

.global rtos_internal_machine_check_irq_common
.type rtos_internal_machine_check_irq_common,STT_FUNC
rtos_internal_machine_check_irq_common:
        irq_frame_store_remaining_volatiles
        irq_frame_store_sprs %r4
        mtctr %r3

        /* Store machine check interrupt save/restore registers:
         *   MCSRR0 holds address to resume at
         *   MCSRR1 holds machine state
         * We do this in case a handler manually re-sets MSR[ME], allowing further machine check interrupts. */
        mfmcsrr0 %r3
        irq_frame_store_srr0 %r3
        mfmcsrr1 %r3
        irq_frame_store_srr1 %r3

        /* Jump to handler function loaded into CTR */
        bctrl

        /* Restore MCSRR0, MCSRR1, masking out MSR[WE] bit to wake up interrupted context in case it was sleeping */
        irq_frame_restore_srr0 %r3
        mtmcsrr0 %r3
        irq_frame_restore_srr1 %r3
        unset_msr_wait_enable %r3 %r4
        mtmcsrr1 %r3

        irq_frame_restore_sprs %r3
        irq_frame_restore_volatile_gprs

        b rtos_internal_machine_check_rfi
.size rtos_internal_machine_check_irq_common, .-rtos_internal_machine_check_irq_common

.global rtos_internal_critical_irq_common
.type rtos_internal_critical_irq_common,STT_FUNC
rtos_internal_critical_irq_common:
        irq_frame_store_remaining_volatiles
        irq_frame_store_sprs %r4
        mtctr %r3

        /* Store critical interrupt save/restore registers:
         *   CSRR0 holds address to resume at
         *   CSRR1 holds machine state
         * A machine check irq may come in at any time.
         * We do this in case a handler manually re-sets MSR[CE] or MSR[DE], allowing further critical interrupts. */
        mfcsrr0 %r3
        irq_frame_store_srr0 %r3
        mfcsrr1 %r3
        irq_frame_store_srr1 %r3

        /* Jump to handler function loaded into CTR */
        bctrl

        /* Restore CSRR0, CSRR1, masking out MSR[WE] bit to wake up the interrupted context in case it was sleeping */
        irq_frame_restore_srr0 %r3
        mtcsrr0 %r3
        irq_frame_restore_srr1 %r3
        unset_msr_wait_enable %r3 %r4
        mtcsrr1 %r3

        irq_frame_restore_sprs %r3
        irq_frame_restore_volatile_gprs

        b rtos_internal_critical_rfi
.size rtos_internal_critical_irq_common, .-rtos_internal_critical_irq_common

.global rtos_internal_noncrit_irq_common
.type rtos_internal_noncrit_irq_common,STT_FUNC
rtos_internal_noncrit_irq_common:
        irq_frame_store_remaining_volatiles
        irq_frame_store_sprs %r4
        mtctr %r3

        /* Store interrupt save/restore registers:
         *   SRR0 holds address to resume at
         *   SRR1 holds machine state
         * A critical or machine check interrupt may come in at any time.
         * More noncritical interrupts may come in if a handler manually re-sets relevant bits in the MSR. */
        mfsrr0 %r3
        irq_frame_store_srr0 %r3
        mfsrr1 %r3
        irq_frame_store_srr1 %r3

        /* Jump to handler function loaded into CTR */
        bctrl

        /* Restore SRR0, SRR1, masking out MSR[WE] bit to wake up the interrupted context in case it was sleeping */
        irq_frame_restore_srr0 %r3
        mtsrr0 %r3
        irq_frame_restore_srr1 %r3
        unset_msr_wait_enable %r3 %r4
        mtsrr1 %r3

        irq_frame_restore_sprs %r3
        irq_frame_restore_volatile_gprs

        b rtos_internal_noncrit_rfi
.size rtos_internal_noncrit_irq_common, .-rtos_internal_noncrit_irq_common
