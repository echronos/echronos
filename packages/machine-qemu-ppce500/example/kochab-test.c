/* @LICENSE(NICTA) */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-kochab.h"
#include "debug.h"

bool
tick_irq(void)
{
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[FIS] (fixed-interval timer interrupt status) */
        "lis %%r3,0x400\n"
        "mttsr %%r3\n"
        ::: "r3");

    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TICK);

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

    debug_println("task a: taking lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    if (rtos_mutex_try_lock(RTOS_MUTEX_ID_M0))
    {
        debug_println("unexpected mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(RTOS_MUTEX_ID_M0);

    for (count = 0; count < 10; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
        }
    }

    debug_println("A now waiting for ticks");
    for (;;)
    {
        (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TIMER);
        debug_println("tick");
        rtos_signal_send_set(RTOS_TASK_ID_B, RTOS_SIGNAL_SET_TEST);
    }
}

void
fn_b(void)
{
    debug_println("task b: attempting lock");
    rtos_mutex_lock(RTOS_MUTEX_ID_M0);
    debug_println("task b: got lock");

    while (1) {
        if (rtos_signal_poll_set(RTOS_SIGNAL_SET_TEST)) {
            debug_println("task b blocking");
            (void) rtos_signal_wait_set(RTOS_SIGNAL_SET_TEST);
            debug_println("task b unblocked");
        }
    }
}

int
main(void)
{
    /*
     * Configure a fixed interval timer
     * Enable:
     *  In TCR (timer control register)
     *    TCR[FIE] (fixed-interval interrupt enable)
     *  In MSR (machine state register)
     *    MSR[EE] (external enable) -> Note: this also enables other async interrupts
     *
     * Set period: TCR[FPEXT] || TCR[FP]
     */
    asm volatile(
        "mftcr %%r3\n"
        "oris %%r3,%%r3,0x380\n" /* 0x300 = TCR[FP], 0x80 = TCR[FIE] */
        "mttcr %%r3"
        ::: "r3");

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
