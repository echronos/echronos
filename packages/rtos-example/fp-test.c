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

#include <float.h>

#include "rtos-{{variant}}.h"
#include "machine-timer.h"
#include "machine-fp.h"
#include "debug.h"

#define LARGEST_FLOAT_INT 16777216
#define ARBITRARY_DOUBLE_MAX 30000000
{{#super_verbose}}
#define ARBITRARY_PROGRESS_TICK 0x8001
{{/super_verbose}}

#define FP_TEST_ERROR_ID_FAIL_FLOAT 0xff
{{#doubles_test}}
#define FP_TEST_ERROR_ID_FAIL_DOUBLE 0xfd
#define TEST_DOUBLES
{{/doubles_test}}

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    machine_timer_stop();
    for (;;)
    {
    }
}

bool
tick_irq(void)
{
    static bool toggle;

    machine_timer_tick_isr();

    if (toggle) {
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_SUBFP);
    } else {
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_ZERO);
    }
    toggle = !toggle;

    return true;
}

/* This task, which *doesn't* use floating-point, is designed to interleave with the others to ensure floating-point
 * context switch works regardless of whether switching to or from another task with/without floating-point state.
 * While we're at it we also zero all the non-FP registers. */
void
fn_z(void)
{
    debug_println("task z");

    while (true) {
        rtos_signal_wait(RTOS_SIGNAL_SET_TIMER);
{{#verbose}}
        debug_println("z: zeroing non-floats");
{{/verbose}}
        machine_fp_test_zero_nonfp_regs();
    }
}

void
fn_a(void)
{
    int k = -LARGEST_FLOAT_INT + 1;
    float u = (float)k;
#ifdef TEST_DOUBLES
    int l = -ARBITRARY_DOUBLE_MAX + 1;
    double v = (double)l;
#endif

    debug_println("task a");

    while (true) {
        rtos_signal_wait(RTOS_SIGNAL_SET_TIMER);

        debug_println("a: subtracting floats");
{{#verbose}}
        debug_print("a: k is ");
        debug_printhex32(k);
        debug_println("");
{{/verbose}}
        k--;
        u--;
        if (k == -LARGEST_FLOAT_INT) {
            debug_println("a: u, k wrapping around");
            u = 0;
            k = 0;
        }
        if (u != (float)k) {
            debug_print("a: u != (float)k found, where k = ");
            debug_printhex32(k);
            debug_println("");
            fatal(FP_TEST_ERROR_ID_FAIL_FLOAT);
        }

#ifdef TEST_DOUBLES
        l--;
        v--;
        if (l == -ARBITRARY_DOUBLE_MAX) {
            debug_println("a: v, l wrapping around");
            v = 0;
            l = 0;
        }
        if (v != (double)l) {
            debug_print("a: v != (double)l found, where l = ");
            debug_printhex32(l);
            debug_println("");
            fatal(FP_TEST_ERROR_ID_FAIL_DOUBLE);
        }
#endif
    }
}

void
fn_b(void)
{
    /* Comparing against ints assumes that int values are preserved correctly across context switch */
    int i = 0;
    float x = 0;
#ifdef TEST_DOUBLES
    int j = 0;
    double y = 0;
#endif

    debug_println("task b");

    while (true) {
        i++;
        x++;
{{#super_verbose}}
        if (!(i % ARBITRARY_PROGRESS_TICK)) {
            debug_print("b: i is ");
            debug_printhex32(i);
            debug_println("");
        }
{{/super_verbose}}
        if (i == LARGEST_FLOAT_INT) {
            debug_println("b: x, i wrapping around");
            x = 0;
            i = 0;
        }
        if (x != (float)i) {
            debug_print("b: x != (float)i found, where i = ");
            debug_printhex32(i);
            debug_println("");
            fatal(FP_TEST_ERROR_ID_FAIL_FLOAT);
        }

#ifdef TEST_DOUBLES
        j++;
        y++;
        if (j == ARBITRARY_DOUBLE_MAX) {
            debug_println("b: y, j wrapping around");
            y = 0;
            j = 0;
        }
        if (y != (double)j) {
            debug_print("b: y != (double)j found, where j = ");
            debug_printhex32(j);
            debug_println("");
            fatal(FP_TEST_ERROR_ID_FAIL_DOUBLE);
        }
#endif
    }
    while (true);

}

/* Dummy because some builds of GCC look for __eabi() even when using -no-sdata option. */
void
__eabi(void)
{
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
