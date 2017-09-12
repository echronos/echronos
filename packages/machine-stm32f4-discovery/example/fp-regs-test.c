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

#include "rtos-kochab.h"
#include "machine-timer.h"
#include "machine-fp.h"
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

bool
tick_irq(void)
{
    machine_timer_tick_isr();

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
    machine_timer_start((void (*)(void))tick_irq);

    machine_fp_init();

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is easier to debug than returning somewhere random. */
    for (;;) ;
}
