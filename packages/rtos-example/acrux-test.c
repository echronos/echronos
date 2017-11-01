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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-acrux.h"
#include "machine-timer.h"
#include "debug.h"

void tick_irq(void);
void fn_a(void);
void fn_b(void);

void
tick_irq(void)
{
    machine_timer_tick_isr();

    rtos_interrupt_event_raise(0);
}

void
fn_a(void)
{
    uint8_t count;
    rtos_unblock(0);
    rtos_unblock(1);

    debug_println("task a: taking lock");
    rtos_mutex_lock(0);
    rtos_yield();
    if (rtos_mutex_try_lock(0))
    {
        debug_println("unexpected mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(0);
    rtos_yield();

    for (count = 0; ; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_unblock(1);
        }
        debug_println("task a blocking");
        rtos_block();
    }
}

void
fn_b(void)
{
    uint8_t count;

    debug_println("task b: attempting lock");
    rtos_mutex_lock(0);
    debug_println("task b: got lock");

    for (count = 0; ; count++)
    {
        debug_println("task b");
        if (count % 4 == 0)
        {
            debug_println("b blocking");
            rtos_block();
        }
        else
        {
            rtos_yield();
        }
    }
}

int
main(void)
{
    machine_timer_start(tick_irq);

    rtos_start();
    for (;;) ;
}
