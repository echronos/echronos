/*<module>
  <code_gen>template</code_gen>
  <schema>
    <entry name="critical_input" type="c_ident" default="" />
    <entry name="machine_check" type="c_ident" default="" />
    <entry name="data_storage" type="c_ident" default="" />
    <entry name="instruction_storage" type="c_ident" default="" />
    <entry name="external_input" type="c_ident" default="" />
    <entry name="alignment" type="c_ident" default="" />
    <entry name="program" type="c_ident" default="" />
    <entry name="floating_point" type="c_ident" default="" />
    <entry name="system_call" type="c_ident" default="" />
    <entry name="aux_processor" type="c_ident" default="" />
    <entry name="decrementer" type="c_ident" default="" />
    <entry name="fixed_interval_timer" type="c_ident" default="" />
    <entry name="watchdog_timer" type="c_ident" default="" />
    <entry name="data_tlb_error" type="c_ident" default="" />
    <entry name="instruction_tlb_error" type="c_ident" default="" />
    <entry name="debug" type="c_ident" default="" />
    <entry name="eis_spe_apu" type="c_ident" default="" />
    <entry name="eis_fp_data" type="c_ident" default="" />
    <entry name="eis_fp_round" type="c_ident" default="" />
    <entry name="eis_perf_monitor" type="c_ident" default="" />
  </schema>
</module>*/

/*
 * For interrupt handlers an empty string (the default) will simply generate a vector that loops forever on itself.
 * Alternatively, setting the handler to "undefined" will generate a handler that first creates a stack frame for the
 * interrupted context and stores its registers there before looping forever at location "undefined".
 * The given handler MUST be responsible for clearing the condition that caused its interrupt.
 */

.section .vectors, "a"
/* This is here to catch unconfigured interrupts, or any other (deliberate or erroneous) jumps to address NULL. */
undefined:
        b undefined

/*
 * The stack frame structure used here to preserve interrupted contexts is defined in ppce500-manual under
 * 'ppce500/vectable'.
 * All magic numbers used in this file should match the stack frame structure documented in ppce500-manual.
 */

.include "vectable-common.h"

.macro irq_frame_create
        stwu %sp,-76(%sp)
.endm

.macro irq_frame_dismantle
        addi %sp,%sp,76
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
{{#critical_input}}
crit_vector:
        create_irq_frame_set_r3 {{critical_input}}
        b rtos_internal_critical_irq_common
{{/critical_input}}
{{^critical_input}}
crit_vector:    b crit_vector
{{/critical_input}}

.align 4
{{#machine_check}}
mchk_vector:
        create_irq_frame_set_r3 {{machine_check}}
        b rtos_internal_machine_check_irq_common
{{/machine_check}}
{{^machine_check}}
mchk_vector:    b mchk_vector
{{/machine_check}}

.align 4
{{#data_storage}}
dstor_vector:
        create_irq_frame_set_r3 {{data_storage}}
        b rtos_internal_noncrit_irq_common
{{/data_storage}}
{{^data_storage}}
dstor_vector:   b dstor_vector
{{/data_storage}}

.align 4
{{#instruction_storage}}
istor_vector:
        create_irq_frame_set_r3 {{instruction_storage}}
        b rtos_internal_noncrit_irq_common
{{/instruction_storage}}
{{^instruction_storage}}
istor_vector:   b istor_vector
{{/instruction_storage}}

.align 4
{{#external_input}}
exti_vector:
        create_irq_frame_set_r3 {{external_input}}
        b rtos_internal_noncrit_irq_common
{{/external_input}}
{{^external_input}}
exti_vector:    b exti_vector
{{/external_input}}

.align 4
{{#alignment}}
align_vector:
        create_irq_frame_set_r3 {{alignment}}
        b rtos_internal_noncrit_irq_common
{{/alignment}}
{{^alignment}}
align_vector:   b align_vector
{{/alignment}}

.align 4
{{#program}}
prog_vector:
        create_irq_frame_set_r3 {{program}}
        b rtos_internal_noncrit_irq_common
{{/program}}
{{^program}}
prog_vector:    b prog_vector
{{/program}}

.align 4
{{#floating_point}}
fp_vector:
        create_irq_frame_set_r3 {{floating_point}}
        b rtos_internal_noncrit_irq_common
{{/floating_point}}
{{^floating_point}}
fp_vector:      b fp_vector
{{/floating_point}}

.align 4
{{#system_call}}
syscall_vector:
        create_irq_frame_set_r3 {{system_call}}
        b rtos_internal_noncrit_irq_common
{{/system_call}}
{{^system_call}}
syscall_vector: b syscall_vector
{{/system_call}}

.align 4
{{#aux_processor}}
aux_vector:
        create_irq_frame_set_r3 {{aux_processor}}
        b rtos_internal_noncrit_irq_common
{{/aux_processor}}
{{^aux_processor}}
aux_vector:     b aux_vector
{{/aux_processor}}

.align 4
{{#decrementer}}
dec_vector:
        create_irq_frame_set_r3 {{decrementer}}
        b rtos_internal_noncrit_irq_common
{{/decrementer}}
{{^decrementer}}
dec_vector:     b dec_vector
{{/decrementer}}

.align 4
{{#fixed_interval_timer}}
fit_vector:
        create_irq_frame_set_r3 {{fixed_interval_timer}}
        b rtos_internal_noncrit_irq_common
{{/fixed_interval_timer}}
{{^fixed_interval_timer}}
fit_vector:     b fit_vector
{{/fixed_interval_timer}}

.align 4
{{#watchdog_timer}}
wdog_vector:
        create_irq_frame_set_r3 {{watchdog_timer}}
        b rtos_internal_critical_irq_common
{{/watchdog_timer}}
{{^watchdog_timer}}
wdog_vector:    b wdog_vector
{{/watchdog_timer}}

.align 4
{{#data_tlb_error}}
dtlb_vector:
        create_irq_frame_set_r3 {{data_tlb_error}}
        b rtos_internal_noncrit_irq_common
{{/data_tlb_error}}
{{^data_tlb_error}}
dtlb_vector:    b dtlb_vector
{{/data_tlb_error}}

.align 4
{{#instruction_tlb_error}}
itlb_vector:
        create_irq_frame_set_r3 {{instruction_tlb_error}}
        b rtos_internal_noncrit_irq_common
{{/instruction_tlb_error}}
{{^instruction_tlb_error}}
itlb_vector:    b itlb_vector
{{/instruction_tlb_error}}

.align 4
{{#debug}}
debug_vector:
        create_irq_frame_set_r3 {{debug}}
        b rtos_internal_critical_irq_common
{{/debug}}
{{^debug}}
debug_vector:   b debug_vector
{{/debug}}

/* Interrupts defined by Freescale Book E Implementation Standards (EIS) */

.align 4
{{#eis_spe_apu}}
eis_spe_apu_vector:
        create_irq_frame_set_r3 {{eis_spe_apu}}
        b rtos_internal_noncrit_irq_common
{{/eis_spe_apu}}
{{^eis_spe_apu}}
eis_spe_apu_vector: b eis_spe_apu_vector
{{/eis_spe_apu}}

.align 4
{{#eis_fp_data}}
eis_fpdata_vector:
        create_irq_frame_set_r3 {{eis_fp_data}}
        b rtos_internal_noncrit_irq_common
{{/eis_fp_data}}
{{^eis_fp_data}}
eis_fpdata_vector:  b eis_fpdata_vector
{{/eis_fp_data}}

.align 4
{{#eis_fp_round}}
eis_fpround_vector:
        create_irq_frame_set_r3 {{eis_fp_round}}
        b rtos_internal_noncrit_irq_common
{{/eis_fp_round}}
{{^eis_fp_round}}
eis_fpround_vector: b eis_fpround_vector
{{/eis_fp_round}}

.align 4
{{#eis_perf_monitor}}
eis_perfmon_vector:
        create_irq_frame_set_r3 {{eis_perf_monitor}}
        b rtos_internal_noncrit_irq_common
{{/eis_perf_monitor}}
{{^eis_perf_monitor}}
eis_perfmon_vector: b eis_perfmon_vector
{{/eis_perf_monitor}}

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

        /* The RTOS only enables and disables noncritical interrupts.
         * Projects that define handlers for critical and machine-check interrupts are expected to enable or disable
         * them appropriately. */
        wrteei 1

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
