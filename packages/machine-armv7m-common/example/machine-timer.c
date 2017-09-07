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

#include <stdint.h>
#include "machine-timer.h"

#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018

#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

void
machine_timer_start(__attribute__((unused)) void (*application_timer_isr)(void))
{
    SYST_RVR_WRITE(0x0001ffff);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);
}

void
machine_timer_stop(void)
{
    /* not implemented because unused */
}

void
machine_timer_tick_isr(void)
{
    /* not implemented because no per-tick hardware handling required */
}
