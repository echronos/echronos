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

/*<module>
    <code_gen>template</code_gen>
    <headers>
        <header path="../../rtos-example/machine-timer.h" code_gen="template" />
    </headers>
</module>*/

#include "machine-timer.h"

void
machine_timer_start(__attribute__((unused)) void (*application_timer_isr)(void))
{
    /*
     * Configure a fixed interval timer
     * Enable:
     *  In TCR (timer control register)
     *    TCR[FIE] (fixed-interval interrupt enable)
     *  In MSR (machine state register)
     *    MSR[EE] (external enable) -> Note: this also enables other async interrupts
     *
     * Set period: TCR[FPEXT] || TCR[FP]
     */
    asm volatile(
        "mftcr %%r3\n"
        "oris %%r3,%%r3,0x380\n" /* 0x300 = TCR[FP], 0x80 = TCR[FIE] */
        "mttcr %%r3"
        ::: "r3");
}

void
machine_timer_stop(void)
{
    /* not implemented because unused */
}

void
machine_timer_tick_isr(void)
{
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[FIS] (fixed-interval timer interrupt status) */
        "lis %%r3,0x400\n"
        "mttsr %%r3\n"
        ::: "r3");
}
