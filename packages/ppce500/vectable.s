/*<module>
  <code_gen>template</code_gen>
  <schema>
    <entry name="preemption" type="bool" optional="true" default="false" />
    <entry name="machine_check" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
    </entry>
    <entry name="critical_input" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
    </entry>
    <entry name="watchdog_timer" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
    </entry>
    <entry name="debug" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
    </entry>
    <entry name="system_call" type="dict" optional="true">
        <entry name="handler" type="c_ident" />
    </entry>
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
 * For interrupt handlers an empty string (the default) will simply generate a vector that loops forever on itself.
 * Alternatively, setting the handler to "undefined" will generate a handler that first creates a stack frame for the
 * interrupted context and stores its registers there before looping forever at the label "undefined".
 * When an explicit handler is given, it MUST be responsible for clearing the condition that caused its interrupt.
 */

{{#preemption}}
/*
 * If preemption support is enabled (via the "preemption" tag), of the three classes of ppce500 interrupt (machine
 * check, critical, and non-critical), we allow non-critical interrupts to be configured as potentially "preempting".
 * If marked as "preempting", the given handler MUST return a true boolean value if the handler on that run made an
 * action that has the potential to cause a preemption, such as raising an irq event.
 */
{{/preemption}}

.section .vectors, "a"
/* This is here to catch unconfigured interrupts, or any other (deliberate or erroneous) jumps to address NULL. */
undefined:
        b undefined

/*
 * On a PowerPC interrupt, the CPU stores machine state into the interrupt save/restore registers:
 *   MCSRR/CSRR/SRR0 holds the address to resume at on return from interrupt (rfmci/rfci/rfi)
 *   MCSRR/CSRR/SRR1 holds the machine state register (MSR) of the interrupted context
 *
 * The Book E specification (e* core families) classes interrupts into noncritical and critical.
 * Noncritical interrupts use SRR0/1, and critical interrupts have a separate pair of save/restore registers CSRR0/1.
 * Additionally, the e500 core defines the machine check interrupt as its own class, with its own registers MCSRR0/1.
 *
 * These classes are ordered in priority such that if a noncritical interrupt is taken, the CPU will automatically
 * modify the MSR to mask out all noncritical interrupts, but critical and machine check interrupts may still occur.
 * Likewise, a critical interrupt will mask out all noncritical and critical interrupts (but not machine check
 * interrupts, on the e500).
 * Finally, on the e500 a machine check interrupt will mask out the enable bits of all types of interrupts.
 *
 * Despite this masking, an interrupt handler may still re-enable certain types of interrupts by re-setting the
 * relevant interrupt enable bits in the MSR.
 * In this case the M/C/SRRs should be saved to the stack since their contents will be lost if an interrupt is taken.
 *
 * Apart from the M/C/SRRs, all of the user-level registers remain as they were at the time the interrupt was taken,
 * and the handler must take responsibility for saving them if the potential exists for them to be corrupted.
 */

{{^preemption}}
/*
 * In order to support arbitrary, project-defined handlers potentially implemented in C, we define the following stack
 * frame structure in which to preserve the interrupted context:
 *
 *  76 +------------------------------------------------------------+ Highest address
 *     | r12                                                        |
 *     ... (r11-r4)   volatile general-purpose registers          ...
 *     | r3                                                         |
 *  36 +------------------------------------------------------------+
 *     | r0                                                         |
 *  32 +------------------------------------------------------------+
 *     | CR (condition register)                                    |
 *  28 +------------------------------------------------------------+
 *     | CTR (count register)                                       |
 *  24 +------------------------------------------------------------+
 *     | XER (integer exception register)                           |
 *  20 +------------------------------------------------------------+
 *     | MCSRR/CSRR/SRR1                                            |
 *  16 +------------------------------------------------------------+
 *     | MCSRR/CSRR/SRR0                                            |
 *  12 +------------------------------------------------------------+
 *     | LR (link register) from interrupted context                |
 *   8 +------------------------------------------------------------+
 *     | LR save word (placeholder) for next function               |
 *   4 +------------------------------------------------------------+
 *     | Back chain word                                            |
 *   0 +------------------------------------------------------------+ Lowest address
 *
 * The lowest two words (LR save and back chain word) follow the stack frame header convention of the EABI
 * specification (see ppce500-context-switch.c for more detail).
 *
 * Only the volatile registers (r3-r12) are preserved, because by the EABI convention, any handler would be
 * responsible for preserving the contents of the nonvolatile registers (r14-r31).
 */

.macro irq_frame_create
        stwu %sp,-76(%sp)
.endm

.macro irq_frame_dismantle
        addi %sp,%sp,76
.endm
{{/preemption}}
{{#preemption}}
/*
 * To support preemption, we unify the stack frame used to preserve interrupted contexts and to implement context
 * switching.
 * This allows us to prevent unnecessary stack frame operations once we determine that an interrupted context is to be
 * preempted.
 *
 * 152 +------------------------------------------------------------+ Highest address
 *     | Preempted context restore status flags                     |
 * 148 +------------------------------------------------------------+
 *     | r31                                                        |
 *     ... (r30-r15)  nonvolatile general-purpose registers       ...
 *     | r14                                                        |
 *  76 +------------------------------------------------------------+
 *     | r12                                                        |
 *     ... (r11-r4)   volatile general-purpose registers          ...
 *     | r3                                                         |
 *  36 +------------------------------------------------------------+
 *     | r0                                                         |
 *  32 +------------------------------------------------------------+
 *     | CR (condition register)                                    |
 *  28 +------------------------------------------------------------+
 *     | CTR (count register)                                       |
 *  24 +------------------------------------------------------------+
 *     | XER (integer exception register)                           |
 *  20 +------------------------------------------------------------+
 *     | MCSRR/CSRR/SRR1                                            |
 *  16 +------------------------------------------------------------+
 *     | MCSRR/CSRR/SRR0                                            |
 *  12 +------------------------------------------------------------+
 *     | LR (link register) from interrupted context                |
 *   8 +------------------------------------------------------------+
 *     | LR save word (placeholder) for next function               |
 *   4 +------------------------------------------------------------+
 *     | Back chain word                                            |
 *   0 +------------------------------------------------------------+ Lowest address
 *
 * In addition to space for the volatile registers (r3-r12), we also set aside space to preserve the nonvolatile
 * registers (r14-r31) in case the interrupted context needs to be preempted, as well as a field for
 * implementation-specific preemption status flags.
 *
 * The lowest two words (LR save and back chain word) follow the stack frame header convention of the EABI
 * specification (see ppce500-context-switch.c for more detail).
 */

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
{{/preemption}}

.macro irq_frame_store_r3
        stw %r3,36(%sp)
.endm

.macro irq_frame_store_remaining_volatile_gprs
        stw %r0,32(%sp)
        stw %r4,40(%sp)
        stw %r5,44(%sp)
        stw %r6,48(%sp)
        stw %r7,52(%sp)
        stw %r8,56(%sp)
        stw %r9,60(%sp)
        stw %r10,64(%sp)
        stw %r11,68(%sp)
        stw %r12,72(%sp)
.endm

.macro irq_frame_restore_volatile_gprs
        lwz %r0,32(%sp)
        lwz %r3,36(%sp)
        lwz %r4,40(%sp)
        lwz %r5,44(%sp)
        lwz %r6,48(%sp)
        lwz %r7,52(%sp)
        lwz %r8,56(%sp)
        lwz %r9,60(%sp)
        lwz %r10,64(%sp)
        lwz %r11,68(%sp)
        lwz %r12,72(%sp)
.endm

.macro irq_frame_store_sprs scratch
        mflr \scratch
        stw \scratch,8(%sp)
        mfxer \scratch
        stw \scratch,20(%sp)
        mfctr \scratch
        stw \scratch,24(%sp)
        mfcr \scratch
        stw \scratch,28(%sp)
.endm

.macro irq_frame_restore_sprs scratch
        lwz \scratch,8(%sp)
        mtlr \scratch
        lwz \scratch,20(%sp)
        mtxer \scratch
        lwz \scratch,24(%sp)
        mtctr \scratch
        lwz \scratch,28(%sp)
        mtcr \scratch
.endm

/* Unlike the ones above, these macros only store and retrieve the SRR values to/from their slot in the stack frame,
 * and are not responsible for fetching from or loading to the actual SRRs. */

.macro irq_frame_store_srr0 src_gpr
        stw \src_gpr,12(%sp)
.endm

.macro irq_frame_store_srr1 src_gpr
        stw \src_gpr,16(%sp)
.endm

.macro irq_frame_restore_srr0 dst_gpr
        lwz \dst_gpr,12(%sp)
.endm

.macro irq_frame_restore_srr1 dst_gpr
        lwz \dst_gpr,16(%sp)
.endm

/* The macros below this point are more generic helpers and should not feature any irq frame offset constants. */

.macro gpr_set_handler gpr handler
        li \gpr,\handler@l
        oris \gpr,\gpr,\handler@h
.endm

.macro irq_frame_store_r3_set handler
        irq_frame_store_r3
        gpr_set_handler %r3 \handler
.endm

.macro apply_32bit_mask target scratch upper_mask lower_mask
        lis \scratch,\upper_mask
        ori \scratch,\scratch,\lower_mask
        and \target,\target,\scratch
.endm

/* This macro sets to zero the MSR wait-enable (WE) bit of the "target" register, trashing the "scratch" register.
 * It is intended to be used on M/C/SRR1 prior to rf(m/c)i to wake up the interrupted context, if it was sleeping. */
.macro unset_msr_wait_enable target scratch
        apply_32bit_mask \target \scratch 0xfffb 0xffff /* upper 16 bits exclude 0x4 for MSR[WE] */
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
        create_irq_frame_set_r3 {{machine_check.handler}}
        b machine_check_irq_common
{{/machine_check}}
{{^machine_check}}
mchk_vector:    b mchk_vector
{{/machine_check}}

.align 4
{{#critical_input}}
crit_vector:
        create_irq_frame_set_r3 {{critical_input.handler}}
        b critical_irq_common
{{/critical_input}}
{{^critical_input}}
crit_vector:    b crit_vector
{{/critical_input}}

.align 4
{{#watchdog_timer}}
wdog_vector:
        create_irq_frame_set_r3 {{watchdog_timer.handler}}
        b critical_irq_common
{{/watchdog_timer}}
{{^watchdog_timer}}
wdog_vector:    b wdog_vector
{{/watchdog_timer}}

.align 4
{{#debug}}
debug_vector:
        create_irq_frame_set_r3 {{debug.handler}}
        b critical_irq_common
{{/debug}}
{{^debug}}
debug_vector:   b debug_vector
{{/debug}}

.align 4
{{#data_storage}}
dstor_vector:
        create_irq_frame_set_r3 {{data_storage.handler}}
{{#data_storage.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/data_storage.preempting}}
{{^data_storage.preempting}}
        b noncrit_irq_common
{{/data_storage.preempting}}
{{/data_storage}}
{{^data_storage}}
dstor_vector:   b dstor_vector
{{/data_storage}}

.align 4
{{#instruction_storage}}
istor_vector:
        create_irq_frame_set_r3 {{instruction_storage.handler}}
{{#instruction_storage.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/instruction_storage.preempting}}
{{^instruction_storage.preempting}}
        b noncrit_irq_common
{{/instruction_storage.preempting}}
{{/instruction_storage}}
{{^instruction_storage}}
istor_vector:   b istor_vector
{{/instruction_storage}}

.align 4
{{#external_input}}
exti_vector:
        create_irq_frame_set_r3 {{external_input.handler}}
{{#external_input.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/external_input.preempting}}
{{^external_input.preempting}}
        b noncrit_irq_common
{{/external_input.preempting}}
{{/external_input}}
{{^external_input}}
exti_vector:    b exti_vector
{{/external_input}}

.align 4
{{#alignment}}
align_vector:
        create_irq_frame_set_r3 {{alignment.handler}}
{{#alignment.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/alignment.preempting}}
{{^alignment.preempting}}
        b noncrit_irq_common
{{/alignment.preempting}}
{{/alignment}}
{{^alignment}}
align_vector:   b align_vector
{{/alignment}}

.align 4
{{#program}}
prog_vector:
        create_irq_frame_set_r3 {{program.handler}}
{{#program.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/program.preempting}}
{{^program.preempting}}
        b noncrit_irq_common
{{/program.preempting}}
{{/program}}
{{^program}}
prog_vector:    b prog_vector
{{/program}}

.align 4
{{#floating_point}}
fp_vector:
        create_irq_frame_set_r3 {{floating_point.handler}}
{{#floating_point.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/floating_point.preempting}}
{{^floating_point.preempting}}
        b noncrit_irq_common
{{/floating_point.preempting}}
{{/floating_point}}
{{^floating_point}}
fp_vector:      b fp_vector
{{/floating_point}}

.align 4
{{#aux_processor}}
aux_vector:
        create_irq_frame_set_r3 {{aux_processor.handler}}
{{#aux_processor.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/aux_processor.preempting}}
{{^aux_processor.preempting}}
        b noncrit_irq_common
{{/aux_processor.preempting}}
{{/aux_processor}}
{{^aux_processor}}
aux_vector:     b aux_vector
{{/aux_processor}}

.align 4
{{#decrementer}}
dec_vector:
        create_irq_frame_set_r3 {{decrementer.handler}}
{{#decrementer.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/decrementer.preempting}}
{{^decrementer.preempting}}
        b noncrit_irq_common
{{/decrementer.preempting}}
{{/decrementer}}
{{^decrementer}}
dec_vector:     b dec_vector
{{/decrementer}}

.align 4
{{#fixed_interval_timer}}
fit_vector:
        create_irq_frame_set_r3 {{fixed_interval_timer.handler}}
{{#fixed_interval_timer.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/fixed_interval_timer.preempting}}
{{^fixed_interval_timer.preempting}}
        b noncrit_irq_common
{{/fixed_interval_timer.preempting}}
{{/fixed_interval_timer}}
{{^fixed_interval_timer}}
fit_vector:     b fit_vector
{{/fixed_interval_timer}}

.align 4
{{#data_tlb_error}}
dtlb_vector:
        create_irq_frame_set_r3 {{data_tlb_error.handler}}
{{#data_tlb_error.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
       b preempt_irq_common
{{/data_tlb_error.preempting}}
{{^data_tlb_error.preempting}}
        b noncrit_irq_common
{{/data_tlb_error.preempting}}
{{/data_tlb_error}}
{{^data_tlb_error}}
dtlb_vector:    b dtlb_vector
{{/data_tlb_error}}

.align 4
{{#instruction_tlb_error}}
itlb_vector:
        create_irq_frame_set_r3 {{instruction_tlb_error.handler}}
{{#instruction_tlb_error.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/instruction_tlb_error.preempting}}
{{^instruction_tlb_error.preempting}}
        b noncrit_irq_common
{{/instruction_tlb_error.preempting}}
{{/instruction_tlb_error}}
{{^instruction_tlb_error}}
itlb_vector:    b itlb_vector
{{/instruction_tlb_error}}

/* Interrupts defined by Freescale Book E Implementation Standards (EIS) */

.align 4
{{#eis_spe_apu}}
eis_spe_apu_vector:
        create_irq_frame_set_r3 {{eis_spe_apu.handler}}
{{#eis_spe_apu.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/eis_spe_apu.preempting}}
{{^eis_spe_apu.preempting}}
        b noncrit_irq_common
{{/eis_spe_apu.preempting}}
{{/eis_spe_apu}}
{{^eis_spe_apu}}
eis_spe_apu_vector: b eis_spe_apu_vector
{{/eis_spe_apu}}

.align 4
{{#eis_fp_data}}
eis_fpdata_vector:
        create_irq_frame_set_r3 {{eis_fp_data.handler}}
{{#eis_fp_data.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/eis_fp_data.preempting}}
{{^eis_fp_data.preempting}}
        b noncrit_irq_common
{{/eis_fp_data.preempting}}
{{/eis_fp_data}}
{{^eis_fp_data}}
eis_fpdata_vector:  b eis_fpdata_vector
{{/eis_fp_data}}

.align 4
{{#eis_fp_round}}
eis_fpround_vector:
        create_irq_frame_set_r3 {{eis_fp_round.handler}}
{{#eis_fp_round.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/eis_fp_round.preempting}}
{{^eis_fp_round.preempting}}
        b noncrit_irq_common
{{/eis_fp_round.preempting}}
{{/eis_fp_round}}
{{^eis_fp_round}}
eis_fpround_vector: b eis_fpround_vector
{{/eis_fp_round}}

.align 4
{{#eis_perf_monitor}}
eis_perfmon_vector:
        create_irq_frame_set_r3 {{eis_perf_monitor.handler}}
{{#eis_perf_monitor.preempting}}{{^preemption}}.error "Preemption support is disabled"{{/preemption}}
        b preempt_irq_common
{{/eis_perf_monitor.preempting}}
{{^eis_perf_monitor.preempting}}
        b noncrit_irq_common
{{/eis_perf_monitor.preempting}}
{{/eis_perf_monitor}}
{{^eis_perf_monitor}}
eis_perfmon_vector: b eis_perfmon_vector
{{/eis_perf_monitor}}

.align 4
{{^preemption}}
{{#system_call}}
syscall_vector:
        create_irq_frame_set_r3 {{system_call.handler}}
        b noncrit_irq_common
{{/system_call}}
{{^system_call}}
syscall_vector: b syscall_vector
{{/system_call}}
{{/preemption}}

{{#preemption}}
{{#system_call}}
.error "The system call vector is not available on preemption-supporting systems"
{{/system_call}}
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

preempt_irq_common:
        irq_frame_store_remaining_volatile_gprs
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
{{/preemption}}

machine_check_irq_common:
        irq_frame_store_remaining_volatile_gprs
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

        irq_frame_dismantle
        rfmci

critical_irq_common:
        irq_frame_store_remaining_volatile_gprs
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

        irq_frame_dismantle
        rfci

noncrit_irq_common:
        irq_frame_store_remaining_volatile_gprs
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

        irq_frame_dismantle
        rfi

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

{{^preemption}}
        /* The RTOS only enables and disables noncritical interrupts.
         * Projects that define handlers for critical and machine-check interrupts are expected to enable or disable
         * them appropriately. */
        wrteei 1
{{/preemption}}

        b main
.size _entry, .-_entry

{{#preemption}}
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
        beq 1f
        irq_frame_restore_volatile_gprs
1:
        irq_frame_dismantle
        rfi
.size rtos_internal_restore_preempted_context, .-rtos_internal_restore_preempted_context
{{/preemption}}
