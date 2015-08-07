/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @TAG(NICTA_AGPL)
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "rtos-kraz.h"

extern void debug_println(const char *msg);

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

    for (;;)
    {
        rtos_yield();
    }
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

    for (;;)
    {
        rtos_yield();
    }
}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
