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

/*
 Notes for readers:

  This module generates a series of 'trampolines' that can be installed
  as exception vectors (using the armv7m.vectable module.)

  A trampoline calls an underlying handler function, and if the handler returns
  true it will preempt the existing task by raising the 'PendSV' exception.

  Pre-emption can be enabled/disabled by raising the setting the base priority (msr baseprio).

 Instruction primer:

   For those unfamiliar with ARM assembly:

     cbz: Compare and branch if zero.
     cbnz: Compare and branch if not-zero.
     bl: Branch and link (set the link-register. Used for calling functions.)
     bx: Branch and exchange - used for exception return.

 Exception handling primer:

   The ARMv7M architecture automatically pushes some register to the current
   stack during an exception before jumping to the exception vector.
   This means an exception handler has free register to work with immediately.

   On an exception the LR is set to contain a special exception return value.
   To return from an exception setting the PC to the special value stored in
   LR (e.g.: with bx lr), will cause the core to perform the special exception
   return routine.

   During exception return the previously stacked values are popped in to registers
   as appropriate.
*/

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
