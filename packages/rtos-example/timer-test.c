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

#include "rtos-{{variant}}.h"
#include "machine-timer.h"
#include "debug.h"

#define DEMO_ERROR_ID_WATCHDOG_A 0xfa
#define DEMO_ERROR_ID_WATCHDOG_B 0xfb
#define DEMO_ERROR_ID_TEST_FAIL 0xff

#define DEMO_A_WAKE_PERIOD 3
#define DEMO_A_SLEEP_TICKS (2 * DEMO_A_WAKE_PERIOD)
#define DEMO_A_WAIT_COUNT 10
#define DEMO_A_EXPECTED_TICKS ((DEMO_A_WAIT_COUNT / 2) * (DEMO_A_WAKE_PERIOD + DEMO_A_SLEEP_TICKS))
#define DEMO_A_WATCHDOG_TICKS (DEMO_A_EXPECTED_TICKS + 1)

#define DEMO_B_SLEEP_TICKS 2
#define DEMO_B_WATCHDOG_TICKS (DEMO_B_SLEEP_TICKS + 1)

#define DEMO_FAIL_IF(cond, fail_str) \
    if (cond) { \
        debug_println(fail_str); \
        fatal(DEMO_ERROR_ID_TEST_FAIL); \
    }
#define DEMO_FAIL_UNLESS(cond, fail_str) DEMO_FAIL_IF(!(cond), fail_str)

bool
tick_irq(void)
{
    static uint8_t count;

    machine_timer_clear();

    debug_print("tick_irq: ");
    debug_printhex32(count);
    debug_println("");
    count++;

    rtos_timer_tick();

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
    RtosTicksAbsolute demo_start;

    debug_println("a: enabling watchdog timer");
    rtos_timer_error_set(RTOS_TIMER_ID_WATCHDOG_A, DEMO_ERROR_ID_WATCHDOG_A);
    demo_start = rtos_timer_current_ticks;
    rtos_timer_oneshot(RTOS_TIMER_ID_WATCHDOG_A, DEMO_A_WATCHDOG_TICKS);

    debug_println("a: enabling periodic wake timer");
    rtos_timer_signal_set(RTOS_TIMER_ID_WAKE_A, RTOS_TASK_ID_A, RTOS_SIGNAL_ID_WAKE);
    rtos_timer_reload_set(RTOS_TIMER_ID_WAKE_A, DEMO_A_WAKE_PERIOD);
    rtos_timer_enable(RTOS_TIMER_ID_WAKE_A);

    for (count = 0; count < DEMO_A_WAIT_COUNT; count++) {
        /* Although timer_remaining and timer_check_overflow are subject to unpredictable delays on RTOS variants that
         * support preemption, this is not a problem on the highest priority Task A, which cannot be preempted. */
        DEMO_FAIL_UNLESS(DEMO_A_WATCHDOG_TICKS - (rtos_timer_current_ticks - demo_start) ==
                rtos_timer_remaining(RTOS_TIMER_ID_WATCHDOG_A), "a: unexpected time remaining!");

        if (count % 2) {
            /* No time elapses */
            DEMO_FAIL_UNLESS(rtos_signal_poll(RTOS_SIGNAL_ID_WAKE), "a: signal poll unexpectedly failed!");
            debug_println("a: polled pending wake signal");
        } else {
            /* DEMO_A_WAKE_PERIOD elapses */
            DEMO_FAIL_IF(rtos_signal_peek(RTOS_SIGNAL_ID_WAKE), "a: signal peek unexpectedly succeeded!");
            debug_println("a: waiting for wake signal");
            rtos_signal_wait(RTOS_SIGNAL_ID_WAKE);

            /* DEMO_A_SLEEP_TICKS elapses */
            debug_println("a: sleeping to overflow wake timer");
            rtos_sleep(DEMO_A_SLEEP_TICKS);

            /* This check should clear the overflow status */
            DEMO_FAIL_UNLESS(rtos_timer_check_overflow(RTOS_TIMER_ID_WAKE_A), "a: timer should have overflowed!");
        }
        DEMO_FAIL_IF(rtos_timer_check_overflow(RTOS_TIMER_ID_WAKE_A), "a: timer unexpectedly overflowed!");
    }

    debug_println("a: disabling periodic wake timer");
    rtos_timer_disable(RTOS_TIMER_ID_WAKE_A);

    DEMO_FAIL_UNLESS(rtos_timer_current_ticks == demo_start + DEMO_A_EXPECTED_TICKS, "a: unexpected elapsed time!");
    DEMO_FAIL_UNLESS(rtos_timer_remaining(RTOS_TIMER_ID_WATCHDOG_A) == 1, "a: unexpected time remaining!");

    debug_println("a: the watchdog should fatal error before this sleep completes");
    rtos_sleep(1);

    debug_println("a: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_b(void)
{
    debug_println("b: starting secondary watchdog");
    rtos_timer_error_set(RTOS_TIMER_ID_WATCHDOG_B, DEMO_ERROR_ID_WATCHDOG_B);

    for (;;) {
        rtos_timer_oneshot(RTOS_TIMER_ID_WATCHDOG_B, DEMO_B_WATCHDOG_TICKS);
        rtos_sleep(DEMO_B_SLEEP_TICKS);
        debug_println("b: restarting secondary watchdog");
        rtos_timer_disable(RTOS_TIMER_ID_WATCHDOG_B);
    }
}

int
main(void)
{
    machine_timer_init();

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
