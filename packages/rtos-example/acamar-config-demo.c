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

#include <stdint.h>
#include <debug.h>

#include "rtos-acamar.h"

{{#rtos.tasks}}
void
{{function}}(void)
{
    for (;;)
    {
        rtos_yield_to(({{rtos.prefix_const}}TASK_ID_{{name|u}} + 1) % {{rtos.tasks.length}});
        debug_println("task {{name}}");
    }
}
{{/rtos.tasks}}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
