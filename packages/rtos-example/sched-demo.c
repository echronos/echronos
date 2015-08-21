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

#include "rtos-{{variant}}.h"
#include "machine-timer.h"
#include "debug.h"

/*
 * Priority inheritance will deadlock if the deadlock test is turned on.
 * Turn this off if you want this test program to complete when using priority inheritance.
 */
{{#deadlock_test}}
#define DEADLOCK
{{/deadlock_test}}

void
block(void)
{
    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_BLOCK);
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
fn_barrier(void)
{
    while (1) {
        rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B_A);
        rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B_B);
        rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B_C);

        rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_SIG_B_A);
        rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_SIG_B_B);
        rtos_signal_send_set(RTOS_TASK_ID_C, RTOS_SIGNAL_SET_SIG_B_C);
    }
}

void
fn_a_barrier(void)
{
    rtos_signal_send_set(RTOS_TASK_ID_BARRIER, RTOS_SIGNAL_SET_SIG_B_A);
    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B_A);
}

void
fn_b_barrier(void)
{
    rtos_signal_send_set(RTOS_TASK_ID_BARRIER, RTOS_SIGNAL_SET_SIG_B_B);
    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B_B);
}

void
fn_c_barrier(void)
{
    rtos_signal_send_set(RTOS_TASK_ID_BARRIER, RTOS_SIGNAL_SET_SIG_B_C);
    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B_C);
}

#ifdef DEADLOCK
/*
 * Deadlock:
 *
 * this test will cause a deadlock with:
 * - strict priority
 * - strict priority with priority inheritance
 *
 * this will not cause a deadlock with:
 * - strict priority with priority ceiling (highest locker)
 */

void
fn_a_deadlock(void)
{
    debug_println("fn_a test: deadlock");

    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_A);
    debug_println("fn_a: go");
    debug_println("fn_a: locking M1");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("fn_a: got M1");
    debug_println("fn_a: locking M0");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("fn_a: got M0");
    debug_println("fn_a: releasing M0");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);
    debug_println("fn_a: releasing M1");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);

    debug_println("fn_a test: deadlock: completed");
}

void
fn_b_deadlock(void)
{
    debug_println("fn_b test: deadlock");

    debug_println("fn_b: locking M0");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("fn_b: got M0");
    debug_println("fn_b: sending sig A");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_SIG_A);
    debug_println("fn_b: locking M1");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("fn_b: got M1");
    debug_println("fn_b: releasing M1");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);
    debug_println("fn_b: releasing M0");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    debug_println("fn_b test: deadlock: completed");
}

void
fn_c_deadlock(void)
{
    debug_println("fn_c test: deadlock");

    debug_println("fn_c test: deadlock: completed");
}
#endif /* DEADLOCK */

/*
 * Priority Inversion
 *
 * When priority inversion occurs in this example then:
 * - task B will run through its loop and complete before A can run and complete
 *
 * this test will cause priority inversion with:
 * - strict priority
 *
 * this will not cause priority inversion with:
 * - strict priority with priority inheritance
 * - strict priority with priority ceiling (highest locker)
 */

void
fn_a_prioinv(void)
{
    debug_println("fn_a test: priority inversion");

    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_A);
    debug_println("fn_a: go");
    debug_println("fn_a: sending sig B");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_SIG_B);
    debug_println("fn_a: locking M0");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("fn_a: got M0");
    debug_println("fn_a: releasing m0");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    debug_println("fn_a test: priority inversion: completed");
}

void
fn_b_prioinv(void)
{
    int i;

    debug_println("fn_b test: priority inversion");

    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B);
    debug_println("fn_b: go");

    for (i = 0; i < 2000; i++) {
        if (i%100 == 0) {
            debug_println("fn_b: looping");
        }
    }

    debug_println("fn_b test: priority inversion: completed");
}

void
fn_c_prioinv(void)
{
    debug_println("fn_c test: priority inversion");

    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("fn_c: got m0");
    debug_println("fn_c: sending sig A");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_SIG_A);
    debug_println("fn_c: releasing m0");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    debug_println("fn_c test: priority inversion: completed");
}

/*
 * Chain Blocking
 *
 * When chain blocking occurs then the highest priority task (a) will be blocked by the lower priority tasks (b, c) in
 * a chain, before it can complete.
 *
 * When it does not occur then the highest priority task will execute without blocking.
 * Note that it will be scheduled after the lower priority tasks finish, however its blocking time is shorter and
 * doesn't involve context switches to other tasks.
 *
 * this test will cause chain blocking with:
 * - strict priority
 * - strict priority with priority inheritance
 *
 * this test will not cause chain blocking with:
 * - strict priority with priority ceiling (highest locker)
 */

void
fn_a_chain(void)
{
    debug_println("fn_a test: chain blocking");

    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_A);
    debug_println("fn_a: go");
    debug_println("fn_a: locking M0");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("fn_a: got m0");
    debug_println("fn_a: locking M1");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("fn_a: got m1");
    debug_println("fn_a: releasing m1");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);
    debug_println("fn_a: releasing m0");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    debug_println("fn_a test: chain blocking: completed");
}

void
fn_b_chain(void)
{
    debug_println("fn_b test: chain blocking");

    rtos_signal_wait_set(RTOS_SIGNAL_SET_SIG_B);
    debug_println("fn_b: go");
    debug_println("fn_b: locking M1");
    rtos_mutex_lock(RTOS_MUTEX_ID_M1);
    debug_println("fn_b: got m1");
    debug_println("fn_b: sending sig A");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_SET_SIG_A);
    debug_println("fn_b: locking M2");
    rtos_mutex_lock(RTOS_MUTEX_ID_M2);
    debug_println("fn_b: got m2");
    debug_println("fn_b: releasing m2");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M2);
    debug_println("fn_b: releasing m1");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M1);

    debug_println("fn_b test: chain blocking: completed");
}

void
fn_c_chain(void)
{
    debug_println("fn_c test: chain blocking");

    debug_println("fn_c: locking M2");
    rtos_mutex_lock(RTOS_MUTEX_ID_M2);
    debug_println("fn_c: got m2");
    debug_println("fn_c: sending sig B");
    rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_SIG_B);
    debug_println("fn_c: releasing m2");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M2);

    debug_println("fn_c test: chain blocking: completed");
}

void
fn_a(void)
{
    debug_println("fn_a starting");

    fn_a_prioinv();

    fn_a_barrier();

    fn_a_chain();

    fn_a_barrier();

#ifdef DEADLOCK
    fn_a_deadlock();

    fn_a_barrier();
#endif

    debug_println("fn_a done");

    block();
}

void
fn_b(void)
{
    debug_println("fn_b starting");

    fn_b_prioinv();

    fn_b_barrier();

    fn_b_chain();

    fn_b_barrier();

#ifdef DEADLOCK
    fn_b_deadlock();

    fn_b_barrier();
#endif

    debug_println("fn_b done");

    block();
}

void
fn_c(void)
{
    debug_println("fn_c starting");

    fn_c_prioinv();

    fn_c_barrier();

    fn_c_chain();

    fn_c_barrier();

#ifdef DEADLOCK
    fn_c_deadlock();

    fn_c_barrier();
#endif

    debug_println("fn_c done");

    block();
}


int
main(void)
{
    machine_timer_deinit();

    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
