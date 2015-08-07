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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-gatria.h"

extern void debug_println(const char *msg);

void
fn_a(void)
{
    uint8_t count;
    rtos_unblock(0);
    rtos_unblock(1);

    debug_println("task a -- lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_A);
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_A))
    {
        debug_println("unexpected mutex not locked.");
    }
    for (count = 0; count < 5; count++)
    {
        rtos_yield();
    }
    debug_println("task a -- unlock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_A);
    rtos_yield();

    for (count = 0; ; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_unblock(1);
        }
        rtos_yield();
    }
}

void
fn_b(void)
{
    uint8_t count;

    debug_println("task b -- try lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_A);
    debug_println("task b -- got lock");

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
    rtos_start();
    for (;;) ;
}
