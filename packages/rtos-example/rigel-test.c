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

#include "rtos-rigel.h"
#include "debug.h"

extern void debug_println(const char *msg);

#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018

#define SYST_CSR_READ() (*((volatile uint32_t*)SYST_CSR_REG))
#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)

#define SYST_RVR_READ() (*((volatile uint32_t*)SYST_RVR_REG))
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)

#define SYST_CVR_READ() (*((volatile uint32_t*)SYST_CVR_REG))
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

void
tick_irq(void)
{
    debug_println("irq tick");
    rtos_timer_tick();
}

void
fatal(RtosErrorId error_id)
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
    /* Set the systick reload value */
    SYST_RVR_WRITE(0x0001ffff);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
