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

#include "rtos-rigel.h"
#include "debug.h"
#include "machine-timer.h"

void tick_irq(void);
void fatal(RtosErrorId error_id);
void fn_a(void);
void fn_b(void);

void
tick_irq(void)
{
    machine_timer_tick_isr();
    debug_println("irq tick");
    rtos_timer_tick();
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
    volatile int i;
    uint8_t count;

    rtos_task_start(RTOS_TASK_ID_B);

    if (rtos_task_current() != RTOS_TASK_ID_A)
    {
        debug_println("task a: wrong task??");
        for (;;)
        {
        }
    }


    debug_println("task a: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_TEST);
    rtos_yield();
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_TEST))
    {
        debug_println("task a: ERROR: unexpected mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(0);
    rtos_yield();

    for (count = 0; count < 10; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("task a: unblocking b");
            rtos_signal_send(RTOS_TASK_ID_B, RTOS_SIGNAL_ID_TEST);
        }
        debug_println("task a: yield");
        rtos_yield();
    }

    /* Do some sleeps */
    debug_println("task a: sleep 10");
    rtos_sleep(10);
    debug_println("task a: sleep done - sleep 5");
    rtos_sleep(5);
    debug_println("task a: sleep done");


    do {
        debug_print("task a: remaining test - ");
        debug_printhex32(rtos_timer_remaining(RTOS_TIMER_ID_TEST));
        debug_print(" - remaining supervisor - ");
        debug_printhex32(rtos_timer_remaining(RTOS_TIMER_ID_SUPERVISOR));
        debug_print(" - ticks - ");
        debug_printhex32(rtos_timer_current_ticks);
        debug_println("");
        rtos_yield();
    } while (!rtos_timer_check_overflow(RTOS_TIMER_ID_TEST));



    if (!rtos_signal_poll(RTOS_SIGNAL_ID_TIMER))
    {
        debug_println("ERROR: couldn't poll expected timer.");
    }

    debug_println("task a: sleep for 100");
    rtos_sleep(100);

    /* Spin for a bit - force a missed ticked */
    debug_println("task a: start delay");
    for (i = 0 ; i < 50000000; i++)
    {

    }
    debug_println("task a: complete delay");
    rtos_yield();

    debug_println("task a: now waiting for ticks");
    for (;;)
    {
        rtos_signal_wait(RTOS_SIGNAL_ID_TIMER);
        debug_println("task a: timer tick");
    }
}

void
fn_b(void)
{
    for (;;)
    {
        debug_println("task b: sleeping for 7");
        rtos_sleep(7);
    }
}

int
main(void)
{
    machine_timer_start(tick_irq);

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
