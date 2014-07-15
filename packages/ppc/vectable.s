.section .vectors, "a"
undefined:
        b undefined

/*
 * Stack frame to stow interrupted context registers:
 * 152 (stack frame size, 8-byte aligned)
 * 36 r3-r31
 * 32 r0
 * 28 CR
 * 24 CTR
 * 20 XER
 * 16 M/C/SRR1
 * 12 M/C/SRR0
 * 8 LR from interrupted context
 * 4 LR save (placeholder) for next function
 * 0 back chain word
 */

.macro create_irq_frame_set_ctr handler
        /* Create stack frame and stow r3 for working */
        stwu %sp,-152(%sp)
        stw %r3,36(%sp)

        /* Stow CTR and replace with handler address */
        mfctr %r3
        stw %r3,24(%sp)
        li %r3,\handler@l
        oris %r3,%r3,\handler@h
        mtctr %r3
.endm

.macro irq_frame_stow_remaining_regs
        /* Stow link register */
        mflr %r3
        stw %r3,8(%sp)

        /* Stow XER and CR */
        mfxer %r3
        stw %r3,20(%sp)
        mfcr %r3
        stw %r3,28(%sp)

        stw %r0,32(%sp)
        stmw %r4,40(%sp) /* r4-r31 */
.endm

.macro restore_regs_dismantle_irq_frame
        lwz %r0,32(%sp)
        lmw %r4,40(%sp) /* r4-r31 */

        /* Restore XER, CTR and CR */
        lwz %r3,20(%sp)
        mtxer %r3
        lwz %r3,24(%sp)
        mtctr %r3
        lwz %r3,28(%sp)
        mtcr %r3

        /* Restore link register */
        lwz %r3,8(%sp)
        mtlr %r3

        /* Restore r3 and dismantle stack frame */
        lwz %r3,36(%sp)
        addi %sp,%sp,152
.endm

.macro unset_msr_wait_enable target scratch
        lis \scratch,0xfffb /* upper 16 bits excluding 0x4 for MSR[WE] */
        ori \scratch,\scratch,0xffff /* lower 16 bits */
        and \target,\target,\scratch
.endm

/* These have to be spaced and aligned to 0x10 because the bottom 4 bits of IVOR are ignored.
 * For PowerPC, .align takes an exponent of 2.
 * We'll put the first one at 0x100, and space them by 0x10 after that. */
.align 8
{{#critical_input}}
crit_vector:
        create_irq_frame_set_ctr {{critical_input}}
        b critical_irq_common
{{/critical_input}}
{{^critical_input}}
crit_vector:    b crit_vector
{{/critical_input}}

.align 4
{{#machine_check}}
mchk_vector:
        create_irq_frame_set_ctr {{machine_check}}
        b machine_check_irq
{{/machine_check}}
{{^machine_check}}
mchk_vector:    b mchk_vector
{{/machine_check}}

.align 4
{{#data_storage}}
dstor_vector:
        create_irq_frame_set_ctr {{data_storage}}
        b noncrit_irq_common
{{/data_storage}}
{{^data_storage}}
dstor_vector:   b dstor_vector
{{/data_storage}}

.align 4
{{#instruction_storage}}
istor_vector:
        create_irq_frame_set_ctr {{instruction_storage}}
        b noncrit_irq_common
{{/instruction_storage}}
{{^instruction_storage}}
istor_vector:   b istor_vector
{{/instruction_storage}}

.align 4
{{#external_input}}
/* Note: the given handler must be responsible for clearing the condition that caused this interrupt */
exti_vector:
        create_irq_frame_set_ctr {{external_input}}
        b noncrit_irq_common
{{/external_input}}
{{^external_input}}
exti_vector:    b exti_vector
{{/external_input}}

.align 4
{{#alignment}}
align_vector:
        create_irq_frame_set_ctr {{alignment}}
        b noncrit_irq_common
{{/alignment}}
{{^alignment}}
align_vector:   b align_vector
{{/alignment}}

.align 4
{{#program}}
prog_vector:
        create_irq_frame_set_ctr {{program}}
        b noncrit_irq_common
{{/program}}
{{^program}}
prog_vector:    b prog_vector
{{/program}}

.align 4
{{#floating_point}}
fp_vector:
        create_irq_frame_set_ctr {{floating_point}}
        b noncrit_irq_common
{{/floating_point}}
{{^floating_point}}
fp_vector:      b fp_vector
{{/floating_point}}

.align 4
{{#system_call}}
syscall_vector:
        create_irq_frame_set_ctr {{system_call}}
        b noncrit_irq_common
{{/system_call}}
{{^system_call}}
syscall_vector: b syscall_vector
{{/system_call}}

.align 4
{{#aux_processor}}
aux_vector:
        create_irq_frame_set_ctr {{aux_processor}}
        b noncrit_irq_common
{{/aux_processor}}
{{^aux_processor}}
aux_vector:     b aux_vector
{{/aux_processor}}

.align 4
{{#decrementer}}
dec_vector:
        create_irq_frame_set_ctr {{decrementer}}

        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[DIS] (decrementer interrupt status) */
        lis %r3,0x800
        mttsr %r3

        b noncrit_irq_common
{{/decrementer}}
{{^decrementer}}
dec_vector:     b dec_vector
{{/decrementer}}

.align 4
{{#fixed_interval_timer}}
fit_vector:
        create_irq_frame_set_ctr {{fixed_interval_timer}}

        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[FIS] (fixed-interval timer interrupt status) */
        lis %r3,0x400
        mttsr %r3

        b noncrit_irq_common
{{/fixed_interval_timer}}
{{^fixed_interval_timer}}
fit_vector:     b fit_vector
{{/fixed_interval_timer}}

.align 4
{{#watchdog_timer}}
wdog_vector:
        create_irq_frame_set_ctr {{watchdog_timer}}

        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[WIS] (watchdog timer interrupt status) */
        lis %r3,0x4000
        mttsr %r3

        b critical_irq_common
{{/watchdog_timer}}
{{^watchdog_timer}}
wdog_vector:    b wdog_vector
{{/watchdog_timer}}

.align 4
{{#data_tlb_error}}
dtlb_vector:
        create_irq_frame_set_ctr {{data_tlb_error}}
        b noncrit_irq_common
{{/data_tlb_error}}
{{^data_tlb_error}}
dtlb_vector:    b dtlb_vector
{{/data_tlb_error}}

.align 4
{{#instruction_tlb_error}}
itlb_vector:
        create_irq_frame_set_ctr {{instruction_tlb_error}}
        b noncrit_irq_common
{{/instruction_tlb_error}}
{{^instruction_tlb_error}}
itlb_vector:    b itlb_vector
{{/instruction_tlb_error}}

.align 4
{{#debug}}
debug_vector:
        create_irq_frame_set_ctr {{debug}}
        b critical_irq_common
{{/debug}}
{{^debug}}
debug_vector:   b debug_vector
{{/debug}}

/* Interrupts defined by Freescale Book E Implementation Standards (EIS) */

.align 4
{{#eis_spe_apu}}
eis_spe_apu_vector:
        create_irq_frame_set_ctr {{eis_spe_apu}}
        b noncrit_irq_common
{{/eis_spe_apu}}
{{^eis_spe_apu}}
eis_spe_apu_vector: b eis_spe_apu_vector
{{/eis_spe_apu}}

.align 4
{{#eis_fp_data}}
eis_fpdata_vector:
        create_irq_frame_set_ctr {{eis_fp_data}}
        b noncrit_irq_common
{{/eis_fp_data}}
{{^eis_fp_data}}
eis_fpdata_vector:  b eis_fpdata_vector
{{/eis_fp_data}}

.align 4
{{#eis_fp_round}}
eis_fpround_vector:
        create_irq_frame_set_ctr {{eis_fp_round}}
        b noncrit_irq_common
{{/eis_fp_round}}
{{^eis_fp_round}}
eis_fpround_vector: b eis_fpround_vector
{{/eis_fp_round}}

.align 4
{{#eis_perf_monitor}}
eis_perfmon_vector:
        create_irq_frame_set_ctr {{eis_perf_monitor}}
        b noncrit_irq_common
{{/eis_perf_monitor}}
{{^eis_perf_monitor}}
eis_perfmon_vector: b eis_perfmon_vector
{{/eis_perf_monitor}}

{{#machine_check}}
machine_check_irq:
        /* Stow machine check interrupt save/restore registers:
         *   MCSRR0 holds address to resume at
         *   MCSRR1 holds machine state
         * We do this in case a handler manually re-sets MSR[ME], allowing further machine check interrupts. */
        mfmcsrr0 %r3
        stw %r3,12(%sp)
        mfmcsrr1 %r3
        stw %r3,16(%sp)

        irq_frame_stow_remaining_regs

        /* Jump to handler function loaded into CTR */
        bctrl

        /* Restore MCSRR0, MCSRR1, masking out MSR[WE] bit to wake up interrupted context in case it was sleeping */
        lwz %r3,12(%sp)
        mtmcsrr0 %r3
        lwz %r3,16(%sp)
        unset_msr_wait_enable %r3 %r4
        mtmcsrr1 %r3

        restore_regs_dismantle_irq_frame

        rfmci
{{/machine_check}}

critical_irq_common:
        /* Stow critical interrupt save/restore registers:
         *   CSRR0 holds address to resume at
         *   CSRR1 holds machine state
         * A machine check irq may come in at any time.
         * We do this in case a handler manually re-sets MSR[CE] or MSR[DE], allowing further critical interrupts. */
        mfcsrr0 %r3
        stw %r3,12(%sp)
        mfcsrr1 %r3
        stw %r3,16(%sp)

        irq_frame_stow_remaining_regs

        /* Jump to handler function loaded into CTR */
        bctrl

        /* Restore CSRR0, CSRR1, masking out MSR[WE] bit to wake up the interrupted context in case it was sleeping */
        lwz %r3,12(%sp)
        mtcsrr0 %r3
        lwz %r3,16(%sp)
        unset_msr_wait_enable %r3 %r4
        mtcsrr1 %r3

        restore_regs_dismantle_irq_frame

        rfci

noncrit_irq_common:
        /* Stow interrupt save/restore registers:
         *   SRR0 holds address to resume at
         *   SRR1 holds machine state
         * A critical or machine check interrupt may come in at any time.
         * More noncritical interrupts may come in if a handler manually re-sets relevant bits in the MSR. */
        mfsrr0 %r3
        stw %r3,12(%sp)
        mfsrr1 %r3
        stw %r3,16(%sp)

        irq_frame_stow_remaining_regs

        /* Jump to handler function loaded into CTR */
        bctrl

        /* Restore SRR0, SRR1, masking out MSR[WE] bit to wake up the interrupted context in case it was sleeping */
        lwz %r3,12(%sp)
        mtsrr0 %r3
        lwz %r3,16(%sp)
        unset_msr_wait_enable %r3 %r4
        mtsrr1 %r3

        restore_regs_dismantle_irq_frame

        rfi

.section .text
.global _entry
.type _entry,STT_FUNC
_entry:
        /* IVPR, IVOR contents are indeterminate upon reset and must be initialized by system software.
         * IVPR is the 16 bit address prefix of ALL interrupt vectors */
        li %r3,0
        mtivpr %r3

        /* IVORs only have the lower 16 bits (excluding bottom 4) of each vector */
        li %r3,crit_vector@l
        mtivor0 %r3
        li %r3,mchk_vector@l
        mtivor1 %r3
        li %r3,dstor_vector@l
        mtivor2 %r3
        li %r3,istor_vector@l
        mtivor3 %r3
        li %r3,exti_vector@l
        mtivor4 %r3
        li %r3,align_vector@l
        mtivor5 %r3
        li %r3,prog_vector@l
        mtivor6 %r3
        li %r3,fp_vector@l
        mtivor7 %r3
        li %r3,syscall_vector@l
        mtivor8 %r3
        li %r3,aux_vector@l
        mtivor9 %r3
        li %r3,dec_vector@l
        mtivor10 %r3
        li %r3,fit_vector@l
        mtivor11 %r3
        li %r3,wdog_vector@l
        mtivor12 %r3
        li %r3,dtlb_vector@l
        mtivor13 %r3
        li %r3,itlb_vector@l
        mtivor14 %r3
        li %r3,debug_vector@l
        mtivor15 %r3

        /* EIS-specific */
        li %r3,eis_spe_apu_vector@l
        mtivor32 %r3
        li %r3,eis_fpdata_vector@l
        mtivor33 %r3
        li %r3,eis_fpround_vector@l
        mtivor34 %r3
        li %r3,eis_perfmon_vector@l
        mtivor35 %r3

        /* Set HID0[DOZE] so that setting MSR[WE] in interrupt_event_wait will gate the DOZE output.
         * A context-synchronising instruction is required before and after mtspr HID0 by the e500 Reference Manual. */
        mfspr %r3,1008
        oris %r3,%r3,0x80 /* 0x800000 = HID0[DOZE] */
        isync
        mtspr 1008,%r3
        isync

        /* The RTOS only enables and disables noncritical interrupts.
         * Projects that define handlers for critical and machine-check interrupts are expected to enable or disable
         * them appropriately. */
        wrteei 1

        b main
.size _entry, .-_entry
