/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="handler" type="c_ident"/>
   <entry name="exception_trampolines" type="list">
     <entry name="exception_trampoline" type="dict">
      <entry name="name" type="c_ident" />
      <entry name="real_handler" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/

.syntax unified
.section .text

.extern {{handler}}
.type {{handler}},#function

.type trampoline_restore,#function
trampoline_restore:
        ldr r0, [sp, #24]
        orr r0, r0, #1      /* Force thumb mode */
        str r0, [sp, #24]
        ldr r0, [sp, #28]
        msr apsr, r0
        pop {r0, r1, r2, r3, r12, r14}
        add sp, #8
        ldr pc, [sp, #-8]
.size trampoline_restore, .-trampoline_restore

trampoline_completion:
        pop {r2, lr}
        ldr r1, [sp, #28]

        push {r1}
        ldr r2, ={{handler}}
        and r2, r2, #0xfffffffe /* This shouldn't be necessary, but gas is a bit stupid */
        push {r2}
        ldr r2, =trampoline_restore
        push {r2} /* LR */
        mov r0, #42
        push {r0} /* r12 */
        mov r0, #32
        push {r0} /* r3 */
        mov r0, #22
        push {r0}
        mov r0, #12
        push {r0}
        mov r0, #2
        push {r0}
        bx lr

{{#exception_trampolines}}
.global exception_trampoline_{{name}}
.type exception_trampoline_{{name}},#function
exception_trampoline_{{name}}:
        push {r0, lr}
        bl {{real_handler}}
        b trampoline_completion
.size exception_trampoline_{{name}}, .-exception_trampoline_{{name}}
{{/exception_trampolines}}
