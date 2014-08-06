.section .text

/*
 * Based on function prologue/epilogue example given in PowerPC EABI documentation.
 * WARNING: The constants used below MUST match the context frame layout defined in ppce500-context-switch.c!
 */

.global ppce500_context_switch
.type ppce500_context_switch,STT_FUNC
/* void ppce500_context_switch(context_t *to, context_t *from); */
ppce500_context_switch:
        mflr %r0            /* Get lr */
        stwu %r1,-80(%r1)   /* Move sp to create new frame (r14-r31 + lr + sp) & save old sp in its back chain word */
        stw  %r0,+84(%r1)   /* Save lr in LR save word of previous stack frame */
        stmw %r14,+8(%r1)   /* Save 18 non-volatiles (r14-r31) to stack */
        stw  %r1,0(%r4)     /* Write sp into "from" argument (r4) */
        /* fallthrough */

.global ppce500_context_switch_first
.type ppce500_context_switch_first,STT_FUNC
/* void ppce500_context_switch_first(context_t *to); */
ppce500_context_switch_first:
        lwz  %r1,0(%r3)     /* Restore sp from "to" argument (r3) */
        lwz  %r0,+84(%r1)   /* Get saved lr from LR save word of previous stack frame */
        mtlr %r0            /* Restore lr */
        lmw  %r14,+8(%r1)   /* Restore 18 non-volatiles (r14-r31) from stack */
        addi %r1,%r1,80     /* Move sp to remove stack frame */
        blr                 /* Branch to lr */

.size ppce500_context_switch_first, .-ppce500_context_switch_first
.size ppce500_context_switch, .-ppce500_context_switch
