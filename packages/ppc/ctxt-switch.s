.section .text

/* Based on function prologue/epilogue example given in PowerPC EABI specification. See ppc-context-switch.c */

.global ppc_context_switch
.type ppc_context_switch,STT_FUNC
/* void ppc_context_switch(context_t *to, context_t *from); */
ppc_context_switch:
        mflr %r0            /* Get lr */
        stwu %r1,-88(%r1)   /* Move sp to create frame for r14-r31 + lr + sp */
        stw  %r0,+92(%r1)   /* Save lr to stack */
        stmw %r14,+16(%r1)  /* Save 18 non-volatiles (r14-r31) to stack */
        stw  %r1,0(%r4)     /* Write sp into "from" argument (r4) */
        /* fallthrough */

.global ppc_context_switch_first
.type ppc_context_switch_first,STT_FUNC
/* void ppc_context_switch_first(context_t *to); */
ppc_context_switch_first:
        lwz  %r1,0(%r3)     /* Restore sp from "to" argument (r3) */
        lwz  %r0,+92(%r1)   /* Get saved lr from stack */
        mtlr %r0            /* Restore lr */
        lmw  %r14,+16(%r1)  /* Restore 18 non-volatiles (r14-r31) from stack */
        addi %r1,%r1,88     /* Move sp to remove stack frame */
        blr                 /* Branch to lr */

.size ppc_context_switch_first, .-ppc_context_switch_first
.size ppc_context_switch, .-ppc_context_switch
