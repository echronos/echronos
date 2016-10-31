/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2018, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */
#include <avr/io.h>

; extern void context_switch_avr(context_t to, context_t *from);
; extern void context_switch_avr(void *to, void **from);
.global context_switch_avr

; extern void context_switch_first_avr(context_t to);
; extern void context_switch_first_avr(void *to);
.global context_switch_first_avr

context_switch_avr:
    ; TODO: this must only be called with interrupts enabled because interrupt enabled/disabled state is not preserved across context switch; might want to check for that

    ; Store the STATUS register on the stack via r0.
    ; The compiler uses r0 is a scratch register that is clobbered by function calls.
    ; Therefore, there is no need to preserve and restore its value.
    in      r0,SREG-__SFR_OFFSET
    ; Disable interrupts as early as possible.
    ; However, only disable interrupts after having stored the STATUS register value elsewhere because the 'interrupt enabled' state is stored in the STATUS register.
    cli
    push    r0

    ; Store and preserve all call-saved registers on the stack
    push    r1
    push    r2
    push    r3
    push    r4
    push    r5
    push    r6
    push    r7
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15
    push    r16
    push    r17
    push    r28
    push    r29

    ; load 'from' argument into R26/X
    movw    r26,r22
    ; store low byte of stack pointer at 'from' argument
    in      r0,SPL-__SFR_OFFSET
    st      X+,r0
    ; store high byte of stack pointer at 'from' argument
    in      r0,SPH-__SFR_OFFSET
    st      X,r0

context_switch_first_avr:
    ; load low byte of stack pointer from 'to' argument
    out     SPL-__SFR_OFFSET,r24
    ; load high byte of stack pointer from 'to' argument
    out     SPH-__SFR_OFFSET,r25

    ; Load and restore all call-saved registers from the stack
    pop     r29
    pop     r28
    pop     r17
    pop     r16
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     r7
    pop     r6
    pop     r5
    pop     r4
    pop     r3
    pop     r2
    pop     r1

    ; restore STATUS register, implicitly enables interrupts
    pop     r0
    out     SREG-__SFR_OFFSET,r0
    ; The compiler uses r0 is a scratch register that is clobbered by function calls.
    ; Therefore, there is no need to preserve and restore its value.

    sei     ; explicitly enable interrupts; should only be necessary for the first context switch, but makes acrux sys test almost work?!?

    ret
