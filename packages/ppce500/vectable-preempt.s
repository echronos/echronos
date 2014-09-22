/*<module>
  <code_gen>template</code_gen>
  <schema>
    <entry name="machine_check" type="c_ident" default="" />
    <entry name="critical_input" type="c_ident" default="" />
    <entry name="watchdog_timer" type="c_ident" default="" />
    <entry name="debug" type="c_ident" default="" />
    <entry name="data_storage" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="instruction_storage" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="external_input" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="alignment" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="program" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="floating_point" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="system_call" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="aux_processor" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="decrementer" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="fixed_interval_timer" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="data_tlb_error" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="instruction_tlb_error" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="eis_spe_apu" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="eis_fp_data" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="eis_fp_round" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
    <entry name="eis_perf_monitor" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
        <entry name="preempting" type="bool" optional="true" default="false" />
    </entry>
  </schema>
</module>*/

/*
 * Of the three classes of ppce500 interrupt (machine check, critical, and non-critical), we allow non-critical
 * interrupts to be configured as potentially "preempting".
 * If marked as "preempting", the given handler MUST return a true boolean value if the handler on that run made an
 * action that has the potential to cause a preemption, such as raising an irq event.
 *
 * For interrupt handlers an empty string (the default) will simply generate a vector that loops forever on itself.
 * Alternatively, setting the handler to "undefined" will generate a handler that first creates a stack frame for
 * the interrupted context and stores its registers there before looping forever at location "undefined".
 * The given handler MUST be responsible for clearing the condition that caused its interrupt.
 */

.section .vectors, "a"
/* This is here to catch unconfigured interrupts, or any other (deliberate or erroneous) jumps to address NULL. */
undefined:
        b undefined

/*
 * The unified stack frame structure used here both to preserve interrupted contexts and to implement context switch
 * on RTOS variants that support task preemption by interrupts is defined in ppce500-manual under
 * 'ppce500/vectable-preempt'.
 * All magic numbers used in this file should match the stack frame structure documented in ppce500-manual.
 */

.include "vectable-common.h"

.macro irq_frame_create
        stwu %sp,-152(%sp)
.endm

.macro irq_frame_dismantle
        addi %sp,%sp,152
.endm

.macro irq_frame_store_nonvolatile_gprs
        /* r14-r31 */
        stmw %r14,76(%sp)
.endm

.macro irq_frame_restore_nonvolatile_gprs
        /* r14-r31 */
        lmw %r14,76(%sp)
.endm

/* This macro defines the first half of storing the interrupted context to stack.
 * It creates the stack frame, stores the contents of r3, and then loads the given handler address into it.
 * This macro is intended to be called from individual vector entry points before jumping to common code. */
.macro create_irq_frame_set_r3 handler
        irq_frame_create
        irq_frame_store_r3_set \handler
.endm

/*
 * The PowerPC interrupt vectors are configured at runtime by setting the IVPR (interrupt vector prefix) and IVOR
 * (interrupt vector offset) registers, which are indeterminate upon reset.
 * Since the bottom 4 bits of IVOR are ignored, the following vectors have to be spaced and aligned to 0x10.
 * For PowerPC, the .align assembler directive takes an exponent of 2.
 * We'll put the first one at 0x100, and space them by 0x10 after that.
 * The order of placement of these vectors in this file is arbitrary.
 */
.align 8
{{#machine_check}}
mchk_vector:
        create_irq_frame_set_r3 {{machine_check}}
        b rtos_internal_machine_check_irq_common
mchk_should_not_be_here:
        b mchk_should_not_be_here
{{/machine_check}}
{{^machine_check}}
mchk_vector:    b mchk_vector
{{/machine_check}}

.align 4
{{#critical_input}}
crit_vector:
        create_irq_frame_set_r3 {{critical_input}}
        b rtos_internal_critical_irq_common
crit_should_not_be_here:
        b crit_should_not_be_here
{{/critical_input}}
{{^critical_input}}
crit_vector:    b crit_vector
{{/critical_input}}

.align 4
{{#watchdog_timer}}
wdog_vector:
        create_irq_frame_set_r3 {{watchdog_timer}}
        b rtos_internal_critical_irq_common
wdog_should_not_be_here:
        b wdog_should_not_be_here
{{/watchdog_timer}}
{{^watchdog_timer}}
wdog_vector:    b wdog_vector
{{/watchdog_timer}}

.align 4
{{#debug}}
debug_vector:
        create_irq_frame_set_r3 {{debug}}
        b rtos_internal_critical_irq_common
debug_should_not_be_here:
        b debug_should_not_be_here
{{/debug}}
{{^debug}}
debug_vector:   b debug_vector
{{/debug}}

.align 4
{{#data_storage}}
dstor_vector:
        create_irq_frame_set_r3 {{data_storage.handler}}
{{#data_storage.preempting}}
        b preempt_irq_common
{{/data_storage.preempting}}
{{^data_storage.preempting}}
        b rtos_internal_noncrit_irq_common
{{/data_storage.preempting}}
dstor_should_not_be_here:
        b dstor_should_not_be_here
{{/data_storage}}
{{^data_storage}}
dstor_vector:   b dstor_vector
{{/data_storage}}

.align 4
{{#instruction_storage}}
istor_vector:
        create_irq_frame_set_r3 {{instruction_storage.handler}}
{{#instruction_storage.preempting}}
        b preempt_irq_common
{{/instruction_storage.preempting}}
{{^instruction_storage.preempting}}
        b rtos_internal_noncrit_irq_common
{{/instruction_storage.preempting}}
istor_should_not_be_here:
        b istor_should_not_be_here
{{/instruction_storage}}
{{^instruction_storage}}
istor_vector:   b istor_vector
{{/instruction_storage}}

.align 4
{{#external_input}}
exti_vector:
        create_irq_frame_set_r3 {{external_input.handler}}
{{#external_input.preempting}}
        b preempt_irq_common
{{/external_input.preempting}}
{{^external_input.preempting}}
        b rtos_internal_noncrit_irq_common
{{/external_input.preempting}}
exti_should_not_be_here:
        b exti_should_not_be_here
{{/external_input}}
{{^external_input}}
exti_vector:    b exti_vector
{{/external_input}}

.align 4
{{#alignment}}
align_vector:
        create_irq_frame_set_r3 {{alignment.handler}}
{{#alignment.preempting}}
        b preempt_irq_common
{{/alignment.preempting}}
{{^alignment.preempting}}
        b rtos_internal_noncrit_irq_common
{{/alignment.preempting}}
align_should_not_be_here:
        b align_should_not_be_here
{{/alignment}}
{{^alignment}}
align_vector:   b align_vector
{{/alignment}}

.align 4
{{#program}}
prog_vector:
        create_irq_frame_set_r3 {{program.handler}}
{{#program.preempting}}
        b preempt_irq_common
{{/program.preempting}}
{{^program.preempting}}
        b rtos_internal_noncrit_irq_common
{{/program.preempting}}
prog_should_not_be_here:
        b prog_should_not_be_here
{{/program}}
{{^program}}
prog_vector:    b prog_vector
{{/program}}

.align 4
{{#floating_point}}
fp_vector:
        create_irq_frame_set_r3 {{floating_point.handler}}
{{#floating_point.preempting}}
        b preempt_irq_common
{{/floating_point.preempting}}
{{^floating_point.preempting}}
        b rtos_internal_noncrit_irq_common
{{/floating_point.preempting}}
fp_should_not_be_here:
        b fp_should_not_be_here
{{/floating_point}}
{{^floating_point}}
fp_vector:      b fp_vector
{{/floating_point}}

.align 4
{{#aux_processor}}
aux_vector:
        create_irq_frame_set_r3 {{aux_processor.handler}}
{{#aux_processor.preempting}}
        b preempt_irq_common
{{/aux_processor.preempting}}
{{^aux_processor.preempting}}
        b rtos_internal_noncrit_irq_common
{{/aux_processor.preempting}}
aux_should_not_be_here:
        b aux_should_not_be_here
{{/aux_processor}}
{{^aux_processor}}
aux_vector:     b aux_vector
{{/aux_processor}}

.align 4
{{#decrementer}}
dec_vector:
        create_irq_frame_set_r3 {{decrementer.handler}}
{{#decrementer.preempting}}
        b preempt_irq_common
{{/decrementer.preempting}}
{{^decrementer.preempting}}
        b rtos_internal_noncrit_irq_common
{{/decrementer.preempting}}

dec_should_not_be_here:
        b dec_should_not_be_here
{{/decrementer}}
{{^decrementer}}
dec_vector:     b dec_vector
{{/decrementer}}

.align 4
{{#fixed_interval_timer}}
fit_vector:
        create_irq_frame_set_r3 {{fixed_interval_timer.handler}}
{{#fixed_interval_timer.preempting}}
        b preempt_irq_common
{{/fixed_interval_timer.preempting}}
{{^fixed_interval_timer.preempting}}
        b rtos_internal_noncrit_irq_common
{{/fixed_interval_timer.preempting}}

fit_should_not_be_here:
        b fit_should_not_be_here
{{/fixed_interval_timer}}
{{^fixed_interval_timer}}
fit_vector:     b fit_vector
{{/fixed_interval_timer}}

.align 4
{{#data_tlb_error}}
dtlb_vector:
        create_irq_frame_set_r3 {{data_tlb_error.handler}}
{{#data_tlb_error.preempting}}
        b preempt_irq_common
{{/data_tlb_error.preempting}}
{{^data_tlb_error.preempting}}
        b rtos_internal_noncrit_irq_common
{{/data_tlb_error.preempting}}
dtlb_should_not_be_here:
        b dtlb_should_not_be_here
{{/data_tlb_error}}
{{^data_tlb_error}}
dtlb_vector:    b dtlb_vector
{{/data_tlb_error}}

.align 4
{{#instruction_tlb_error}}
itlb_vector:
        create_irq_frame_set_r3 {{instruction_tlb_error.handler}}
{{#instruction_tlb_error.preempting}}
        b preempt_irq_common
{{/instruction_tlb_error.preempting}}
{{^instruction_tlb_error.preempting}}
        b rtos_internal_noncrit_irq_common
{{/instruction_tlb_error.preempting}}
itlb_should_not_be_here:
        b itlb_should_not_be_here
{{/instruction_tlb_error}}
{{^instruction_tlb_error}}
itlb_vector:    b itlb_vector
{{/instruction_tlb_error}}

/* Interrupts defined by Freescale Book E Implementation Standards (EIS) */

.align 4
{{#eis_spe_apu}}
eis_spe_apu_vector:
        create_irq_frame_set_r3 {{eis_spe_apu.handler}}
{{#eis_spe_apu.preempting}}
        b preempt_irq_common
{{/eis_spe_apu.preempting}}
{{^eis_spe_apu.preempting}}
        b rtos_internal_noncrit_irq_common
{{/eis_spe_apu.preempting}}
eis_spe_apu_should_not_be_here:
        b eis_spe_apu_should_not_be_here
{{/eis_spe_apu}}
{{^eis_spe_apu}}
eis_spe_apu_vector: b eis_spe_apu_vector
{{/eis_spe_apu}}

.align 4
{{#eis_fp_data}}
eis_fpdata_vector:
        create_irq_frame_set_r3 {{eis_fp_data.handler}}
{{#eis_fp_data.preempting}}
        b preempt_irq_common
{{/eis_fp_data.preempting}}
{{^eis_fp_data.preempting}}
        b rtos_internal_noncrit_irq_common
{{/eis_fp_data.preempting}}
eis_fpdata_should_not_be_here:
        b eis_fpdata_should_not_be_here
{{/eis_fp_data}}
{{^eis_fp_data}}
eis_fpdata_vector:  b eis_fpdata_vector
{{/eis_fp_data}}

.align 4
{{#eis_fp_round}}
eis_fpround_vector:
        create_irq_frame_set_r3 {{eis_fp_round.handler}}
{{#eis_fp_round.preempting}}
        b preempt_irq_common
{{/eis_fp_round.preempting}}
{{^eis_fp_round.preempting}}
        b rtos_internal_noncrit_irq_common
{{/eis_fp_round.preempting}}
eis_fpround_should_not_be_here:
        b eis_fpround_should_not_be_here
{{/eis_fp_round}}
{{^eis_fp_round}}
eis_fpround_vector: b eis_fpround_vector
{{/eis_fp_round}}

.align 4
{{#eis_perf_monitor}}
eis_perfmon_vector:
        create_irq_frame_set_r3 {{eis_perf_monitor.handler}}
{{#eis_perf_monitor.preempting}}
        b preempt_irq_common
{{/eis_perf_monitor.preempting}}
{{^eis_perf_monitor.preempting}}
        b rtos_internal_noncrit_irq_common
{{/eis_perf_monitor.preempting}}
eis_perfmon_should_not_be_here:
        b eis_perfmon_should_not_be_here
{{/eis_perf_monitor}}
{{^eis_perf_monitor}}
eis_perfmon_vector: b eis_perfmon_vector
{{/eis_perf_monitor}}

.align 4
/* _yield_syscall(TaskId to, bool return_with_preempt_disabled) */
syscall_vector:
        /* Note: Interrupts are disabled but we want to leave here with interrupts enabled */
        irq_frame_create

        mfsrr0 %r5
        irq_frame_store_srr0 %r5
        mfsrr1 %r5
        /* We set MSR[EE] bit so the yielding task is restored with interrupts enabled. */
        ori %r5,%r5,0x8000
        irq_frame_store_srr1 %r5

        irq_frame_store_sprs %r5
        irq_frame_store_nonvolatile_gprs

        /* r6: Yield was manual, so volatiles should NOT be restored */
        li %r6,0
        /* r5: return_with_preempt_disabled flag that was passed into syscall */
        mr %r5,%r4
        /* r4: Address of newly constructed irq stack frame */
        mr %r4,%sp
        /* r3: Untouched "TaskId to" passed into syscall */

        /* This never returns */
        b ppce500_context_preempt

syscall_should_not_be_here:
        b syscall_should_not_be_here

preempt_irq_common:
        irq_frame_store_remaining_volatiles
        irq_frame_store_sprs %r4

        mfsrr0 %r4
        irq_frame_store_srr0 %r4
        mfsrr1 %r4
        /* We mask out MSR[WE] bit so the interrupted context is woken up on restore in case it was sleeping. */
        unset_msr_wait_enable %r4 %r5
        irq_frame_store_srr1 %r4

        /* r3 should still contain the function pointer of the handler */
        bl preempt_irq_handler_wrapper
        /* r3 should now contain TaskId to */

        /* If to == TASK_ID_NONE we restore the interrupted context */
        cmpi 0,%r3,255 /* FIXME: TASK_ID_NONE depends on taskid_size */
        bne 1f

        /* r3: Task was interrupted, so we want to restore the volatiles */
        li %r3,1
        b preempt_irq_restore_context

1:      /* Now we know we're context switching to a different task.
         * Store the nonvolatiles, which should have been preserved by all of the function calls up to this point. */
        irq_frame_store_nonvolatile_gprs

        /* r6: Task was interrupted, so we want to restore the volatiles */
        li %r6,1
        /* r5: We only got here because preemption was enabled, so restore preemption_disabled = 0 with the task */
        li %r5,0
        /* r4: Address of newly constructed irq stack frame */
        mr %r4,%sp
        /* r3: Untouched "TaskId to" returned from preempt_irq_handler_wrapper */

        /* This never returns */
        b ppce500_context_preempt

preempt_irq_should_not_be_here:
        b preempt_irq_should_not_be_here

.section .text
/* The _entry function initialises the C run-time and then jumps to main (which should never return!)
 * If there is a Reset_Handler function defined, then this will be invoked.
 * It should never return. */
.weak Reset_Handler
.global _entry
.type _entry,STT_FUNC
_entry:
        /* If there is a Reset_Handler call it - it shouldn't return. */
        lis %r3,Reset_Handler@h
        ori %r3,%r3,Reset_Handler@l
        cmpi 0,%r3,0
        beq 1f
        b Reset_Handler
1:
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

        b main
.size _entry, .-_entry

.global rtos_internal_machine_check_rfi
.type rtos_internal_machine_check_rfi,STT_FUNC
rtos_internal_machine_check_rfi:
        irq_frame_dismantle
        rfmci
.size rtos_internal_machine_check_rfi, .-rtos_internal_machine_check_rfi

.global rtos_internal_critical_rfi
.type rtos_internal_critical_rfi,STT_FUNC
rtos_internal_critical_rfi:
        irq_frame_dismantle
        rfci
.size rtos_internal_critical_rfi, .-rtos_internal_critical_rfi

.global rtos_internal_noncrit_rfi
.type rtos_internal_noncrit_rfi,STT_FUNC
rtos_internal_noncrit_rfi:
        irq_frame_dismantle
        rfi
.size rtos_internal_noncrit_rfi, .-rtos_internal_noncrit_rfi

.global rtos_internal_yield_syscall
.type rtos_internal_yield_syscall,STT_FUNC
/* rtos_internal_yield_syscall(TaskId to, bool return_with_preempt_disabled) */
rtos_internal_yield_syscall:
        sc
        blr
.size rtos_internal_yield_syscall, .-rtos_internal_yield_syscall

.global rtos_internal_restore_preempted_context
.type rtos_internal_restore_preempted_context,STT_FUNC
/* void rtos_internal_restore_preempted_context(bool restore_volatiles, context_t ctxt_to); */
rtos_internal_restore_preempted_context:
        /* Context switch */
        mr %sp,%r4
        irq_frame_restore_nonvolatile_gprs
        /* FALL THROUGH */
/* void preempt_irq_restore_context(bool restore_volatiles); */
preempt_irq_restore_context:
        irq_frame_restore_srr0 %r4
        mtsrr0 %r4
        irq_frame_restore_srr1 %r4
        mtsrr1 %r4
        irq_frame_restore_sprs %r4
        /* Skip restoring volatiles if argument says so */
        cmpi 0,%r3,0
        beq rtos_internal_noncrit_rfi
        irq_frame_restore_volatile_gprs
        b rtos_internal_noncrit_rfi
.size rtos_internal_restore_preempted_context, .-rtos_internal_restore_preempted_context
