/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2017, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */
.syntax unified
.section .text

.global debug_puts
.type debug_puts,#function

debug_puts:
        mov r1, r0
        push {r0, lr}
        mov r0, #4
        bkpt 0xab
        pop {r0, pc}

.size debug_puts, .-debug_puts
