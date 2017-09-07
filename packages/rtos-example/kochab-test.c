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

#include "rtos-kochab.h"
#include "machine-timer.h"
#include "debug.h"

bool tick_irq(void);
void fatal(RtosErrorId error_id);
void fn_a(void);
void fn_b(void);

bool
tick_irq(void)
{
    machine_timer_tick_isr();

    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TICK);

    return true;
}

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;)
    {
    }
}

void
fn_a(void)
{
    uint8_t count;

    debug_println("task a: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_M0))
    {
        debug_println("unexpected mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    for (count = 0; count < 10; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
        }
    }

    debug_println("A now waiting for ticks");
    for (;;)
    {
        (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TIMER);
        debug_println("tick");
        rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
    }
}

void
fn_b(void)
{
    debug_println("task b: attempting lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("task b: got lock");

    while (1) {
        if (rtos_signal_poll_set(RTOS_SIGNAL_SET_TEST)) {
            debug_println("task b blocking");
            (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TEST);
            debug_println("task b unblocked");
        }
    }
}

int
main(void)
{
    machine_timer_start((void (*)(void))tick_irq);

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
