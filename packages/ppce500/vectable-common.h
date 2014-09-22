/*
 * This file defines the portion of the stack frame structure that is common to the preservation of interrupted
 * contexts both with and without support for interrupt preemption, as defined in ppce500-manual under the sections
 * 'ppce500/vectable' and 'ppce500/vectable-preempt'.
 * All magic numbers used in this file should match the stack frame structures documented in ppce500-manual.
 */

/* These macros form the building blocks of all the others and are defined separately to assist maintenance of
 * offset constants in case of any future changes to the context/irq stack frame structure. */

.macro irq_frame_store_r3
        stw %r3,36(%sp)
.endm

.macro irq_frame_store_remaining_volatiles
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

/* These macros are more generic helpers and should not feature any irq frame offset constants. */

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
