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
#include "rtos-gatria.h"

#pragma weak loop_a
void
loop_a(void)
{
    rtos_yield();
}

#pragma weak loop_b
void
loop_b(void)
{
    rtos_yield();
}

#pragma weak loop_c
void
loop_c(void)
{
    rtos_yield();
}
