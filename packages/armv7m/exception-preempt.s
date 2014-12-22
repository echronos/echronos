/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="trampolines" type="list" default="[]">
     <entry name="trampoline" type="dict">
      <entry name="name" type="c_ident" />
      <entry name="handler" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/

/* This module generates a series of 'trampolines' that can be installed as exception vectors (using the
 * armv7m.vectable module).
 *
 * A trampoline calls an underlying handler function, and if the handler returns true it will preempt the existing
 * task by raising the 'PendSV' exception.
 *
 * Preemption can be disabled by raising the base priority (msr basepri) to that of the PendSV exception.
 * See ctxt-switch-preempt.s for more details. */

.syntax unified
.section .text

/* This macro is used to set the 'preemption pending' status. */
.macro asm_preempt_pend scratch0 scratch1
        /* Set the PendSV bit in the ICSR (Interrupt Control and State Register) */
        ldr \scratch0, =0xE000ED04
        ldr \scratch1, =0x10000000
        str \scratch1, [\scratch0]
.endm

/**
 * Set the 'preemption pending' status so that a preemption will occur at the soonest possible opportunity.
 */
.global rtos_internal_preempt_pend
.type rtos_internal_preempt_pend,#function
/* void rtos_internal_preempt_pend(void); */
rtos_internal_preempt_pend:
        asm_preempt_pend r0 r1
        bx lr
.size rtos_internal_preempt_pend, .-rtos_internal_preempt_pend

trampoline_completion:
        /* Note: value of ip is scratch, we only care about restoring LR, however stack must remain 8-byte aligned. */
        pop {ip, lr}

        /* If the handler returned 0, jump to the final bx as pre-emption is not required. */
        cbz r0, exception_return

        asm_preempt_pend r0 r1

exception_return:
        bx lr

{{#trampolines}}
.global rtos_internal_exception_preempt_trampoline_{{name}}
.type rtos_internal_exception_preempt_trampoline_{{name}},#function
rtos_internal_exception_preempt_trampoline_{{name}}:
        /* Note: We don't care about saving the value of ip (it is scratch), but it is important to keep the stack
         * 8-byte aligned, so push it as a dummy */
        push {ip, lr}
        bl {{handler}}
        b trampoline_completion
.size rtos_internal_exception_preempt_trampoline_{{name}}, .-rtos_internal_exception_preempt_trampoline_{{name}}
{{/trampolines}}
