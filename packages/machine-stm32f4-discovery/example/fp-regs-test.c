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

#include "rtos-kochab.h"
#include "machine-timer.h"
#include "debug.h"

#define DEMO_ERROR_ID_TEST_FAIL 0xff

#define DEMO_FAIL_IF(cond, fail_str) \
    if (cond) { \
        debug_println(fail_str); \
        fatal(DEMO_ERROR_ID_TEST_FAIL); \
    }
#define DEMO_FAIL_UNLESS(cond, fail_str) DEMO_FAIL_IF(!(cond), fail_str)

/* This is the number of floating-point registers on the platform */
#define DEMO_NUM_FP_VALS 32

/* This arbitrary offset ensures the register values for tasks A and B are distinct */
#define DEMO_B_OFFSET 137

/* Respectively set/get the current values of the floating-point registers, from/to an array */
#define demo_fp_regs_set(vals) __asm volatile("vldm %0, {s0-s31}"::"r"(vals))
#define demo_fp_regs_get(regs) __asm volatile("vstm %0, {s0-s31}"::"r"(regs))

void
enable_fpu(void)
{
    /* This is the example code given in the Cortex-M4 Devices Generic User Guide */
    __asm volatile(
        "ldr.w r0, =0xe000ed88\n"
        "ldr r1, [r0]\n"
        "orr r1, r1, #(0xf << 20)\n"
        "str r1, [r0]\n"
        "dsb\n"
        "isb\n");
}

bool
tick_irq(void)
{
    machine_timer_clear();

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
    float vals_a[DEMO_NUM_FP_VALS], regs_a[DEMO_NUM_FP_VALS];
    int i, j;

    for (j = 0; true; j++) {
        debug_println("a: setting values");
        for (i = 0; i < DEMO_NUM_FP_VALS; i++) {
            vals_a[i] = (float)(i + j);
        }
        demo_fp_regs_set(vals_a);

        debug_println("a: switching");
        rtos_sleep(2);

        demo_fp_regs_get(regs_a);
        for (i = 0; i < DEMO_NUM_FP_VALS; i++) {
            DEMO_FAIL_UNLESS(vals_a[i] == regs_a[i], "a: incorrect value!");
        }
        debug_println("a: values correct");
    }

    debug_println("a: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_b(void)
{
    float vals_b[DEMO_NUM_FP_VALS], regs_b[DEMO_NUM_FP_VALS];
    int i, j;

    debug_println("b: delaying");

    rtos_sleep(1);

    for (j = DEMO_B_OFFSET; true; j++) {
        debug_println("b: setting values");
        for (i = 0; i < DEMO_NUM_FP_VALS; i++) {
            vals_b[i] = (float)(i + j);
        }
        demo_fp_regs_set(vals_b);

        debug_println("b: switching");
        rtos_sleep(2);

        demo_fp_regs_get(regs_b);
        for (i = 0; i < DEMO_NUM_FP_VALS; i++) {
            DEMO_FAIL_UNLESS(vals_b[i] == regs_b[i], "b: incorrect value!");
        }
        debug_println("b: values correct");
    }

    debug_println("b: shouldn't be here!");
    for (;;)
    {
    }
}

/* This task, which *doesn't* use floating-point, is designed to interleave with the others to ensure floating-point
 * context switch works regardless of whether switching to or from another task with/without floating-point state. */
void
fn_c(void)
{
    while (true) {
        debug_println("c: sleeping");
        rtos_sleep(3);
        debug_println("c: awake");
    }
}

int
main(void)
{
    machine_timer_init();

    enable_fpu();

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is easier to debug than returning somewhere random. */
    for (;;) ;
}
