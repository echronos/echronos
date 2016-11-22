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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "rtos-kraz.h"

extern void debug_println(const char *msg);
extern void debug_printhex32(uint32_t val);

void complete(RtosTaskId task);

void
complete(const RtosTaskId task)
{
    debug_printhex32(task);
    debug_println(" completed");
    for (;;)
    {
        rtos_yield();
    }
}

void fn_a(void);
void fn_b(void);

void
fn_a(void)
{
    uint8_t count;

    /* FIXME: These are really just here to force both tasks to be runnable */
    rtos_signal_send_set(0, 0);
    rtos_signal_send_set(1, 0);

    debug_println("task a: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_TEST);
    debug_println("task a: got lock");
    rtos_yield();
    debug_println("task a: yielded");
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_TEST))
    {
        debug_println("task a: ERROR: mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_TEST);
    rtos_yield();

    for (count = 0; count < 10; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("task a: unblocking b");
            rtos_signal_send(RTOS_TASK_ID_B, RTOS_SIGNAL_ID_TEST);
        }
        rtos_yield();
    }

    debug_println("task a: finished sending signals");

    rtos_signal_send(RTOS_TASK_ID_B, RTOS_SIGNAL_ID_TEST);

    debug_println("task a: completed.");

    complete(0);
}

void
fn_b(void)
{
    uint8_t count;

    debug_println("task b: attempting lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_TEST);
    debug_println("task b: got lock");

    for (count = 0; count < 8; count++)
    {
        debug_println("task b");
        if (count % 4 == 0)
        {
            debug_println("task b: blocking");
            rtos_signal_wait(RTOS_SIGNAL_ID_TEST);
            debug_println("task b: unblocked");
        }
        else
        {
            rtos_yield();
        }
    }

    debug_println("task b: finished receiving signals.");

    for (count = 0; ; count++)
    {
        if (rtos_signal_peek(RTOS_SIGNAL_ID_TEST))
        {
            debug_println("task b: signal available on peek");
            if (!rtos_signal_poll(RTOS_SIGNAL_ID_TEST))
            {
                debug_println("task b: ERROR: unable to poll after peek.");
            }
            else
            {
                debug_println("task b: Successful poll after peek.");
            }
            break;
        }
        else
        {
            debug_println("task b: no signal available on peek");
            if (rtos_signal_poll(RTOS_SIGNAL_ID_TEST))
            {
                debug_println("task b: ERROR: poll available no-peek.");
            }
            else
            {
                debug_println("task b: Success: no signal available to poll after no-peek.");
            }
        }

        rtos_yield();
    }

    debug_println("task b: completed.");

    complete(1);
}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
