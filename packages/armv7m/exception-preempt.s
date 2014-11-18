/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="handler" type="c_ident"/>
   <entry name="trampolines" type="list">
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
  true it will preempt the existing task, pushing a stack frame to the current
  task's stack, so that the task will execute the specified 'preemption handler'.

  Pre-emption can be disabled by setting the 'exception_preempt_disabled' variable to 1.
  Pre-emption is automatically disabled before calling the 'preemption handler'.
  The 'preemption handler' should reenable preemptions if this is required.

  A task can check if a pre-emption is pending by checking the 'exception_preempt_pending'
  variable.

 Instruction primer:

   For those unfamiliar with ARM assembly:

     cpsid:  Interrupt disable
     cpsie: Interrupt enabled
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

.extern {{handler}}
.type {{handler}},#function

.type trampoline_restore,#function
trampoline_restore:
        /*
         * This function mimics the standard ARMv7M exception return
         * machinery.
         */

        /* The exception enter machinery does not set the thumb bit on the PC
           value, so we need to force it now. */
        ldr r0, [sp, #24]
        orr r0, r0, #1
        str r0, [sp, #24]

        /* Set the PSR value from the stack frame */
        ldr r0, [sp, #28]
        msr apsr, r0

        /* Now restore r0-r3, r12 and lr */
        pop {r0, r1, r2, r3, r12, r14}

        /* Restore the stack pointer */
        add sp, #8

        /* Finally, restore the PC */
        ldr pc, [sp, #-8]
.size trampoline_restore, .-trampoline_restore

trampoline_completion:
        /* Note: value of r1 is scratch, we only care about restoring
           LR, however stack must remain 8-byte aligned.

           We don't want to pop in to r0 as it contains the return
           value of the interrupt handler.
        */
        pop {r1, lr}

        /*
         * Check the return value.
         *  If false, jump to the final bx as pre-emption is not required.
         */
        cbz r0, exception_return

        /*
         * exception_preempt_pending = 1
         *
         * Note: preempt pending is always set, even in the case where preemption
         * actually occurs.
         */
        mov r1, #1
        ldr r2, =rtos_internal_exception_preempt_pending
        strb r1, [r2]

        /*
         * atomically:
         *   r0 = exception_preempt_disabled
         *   exception_preempt_disabled = 1
         *
         * Disable preemption before calling the handler.
         *
         * Note: If preemption is already disabled this is a no-op.
         *
         * This has to be performed atomically, otherwise a nested exception
         * could incorrectly cause multiple preemptions.
         */
        ldr r2, =rtos_internal_exception_preempt_disabled
        cpsid i
        ldrb r0, [r2]
        strb r1, [r2]
        cpsie i

        /*
         * if exception trampoline was already disabled, we don't want to preempt
         * so exit immediately
         */
        cbnz r0, exception_return

        /* From here on we are commited to preempting. */

        /* Push the xPSR value on the new stack frame -- copy the old one */
        ldr r0, [sp, #28]
        push {r0}

        /* Push the handler address, the exception return code will jump here
         *  (rather than the interrupt address).
         */
        ldr r0, ={{handler}}
        and r0, r0, #0xfffffffe /* Exception machinery requires to low bit to be cleared. */
        push {r0}

        /* Push the value for the link register. After the handler executes it will
           resume at trampoline_restore */
        ldr r0, =trampoline_restore
        push {r0}

        /* Finally push the rest of the register state for the new frame */
        push {r0, r1, r2, r3, r12}
exception_return:
        bx lr

{{#trampolines}}
.global rtos_internal_exception_preempt_trampoline_{{name}}
.type rtos_internal_exception_preempt_trampoline_{{name}},#function
rtos_internal_exception_preempt_trampoline_{{name}}:
        /* Note: We don't care about saving the value of r0 (it is scratch), but
           it is important to keep the stack 8-byte aligned, so push it as a dummy */
        push {r0, lr}
        bl {{handler}}
        b trampoline_completion
.size rtos_internal_exception_preempt_trampoline_{{name}}, .-rtos_internal_exception_preempt_trampoline_{{name}}
{{/trampolines}}

.data
.global rtos_internal_exception_preempt_disabled
rtos_internal_exception_preempt_disabled:
	.byte 0
.global rtos_internal_exception_preempt_pending
rtos_internal_exception_preempt_pending:
	.byte 0
