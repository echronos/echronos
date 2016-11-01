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
#include "rtos-acamar.h"

#pragma weak setup
void
setup(void)
{
    rtos_start();
}

#pragma weak loop
void
loop(void)
{
}

#pragma weak loop_a
void
loop_a(void)
{
    rtos_yield_to(RTOS_TASK_ID_B);
}

#pragma weak task_a
void
task_a(void)
{
    for (;;)
    {
        loop_a();
    }
}

#pragma weak loop_b
void
loop_b(void)
{
    rtos_yield_to(RTOS_TASK_ID_C);
}

#pragma weak task_b
void
task_b(void)
{
    for (;;)
    {
        loop_b();
    }
}

#pragma weak loop_c
void
loop_c(void)
{
    rtos_yield_to(RTOS_TASK_ID_A);
}

#pragma weak task_c
void
task_c(void)
{
    for (;;)
    {
        loop_c();
    }
}
