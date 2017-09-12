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
        <header path="../rtos-example/machine-timer.h" code_gen="template" />
    </headers>
</module>*/

#include <signal.h>
#include <unistd.h>
#include "machine-timer.h"

#define UALARM_MILLISECOND (1000U)
#define TICK_DURATION (100U * UALARM_MILLISECOND)

static void sigalrm_handler(int sig);

static void (*application_isr)(void);

static void
sigalrm_handler(__attribute__((unused)) const int sig)
{
    if (application_isr)
    {
        application_isr();
    }
}

void
machine_timer_start(void (*application_timer_isr)(void))
{
    application_isr = application_timer_isr;
    signal(SIGALRM, sigalrm_handler);
    ualarm(TICK_DURATION, TICK_DURATION);
}

void
machine_timer_stop(void)
{
    ualarm(0, 0);
}

void
machine_timer_tick_isr(void)
{
}
