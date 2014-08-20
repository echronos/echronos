#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-acrux.h"

extern void debug_println(const char *msg);

void
tick_irq(void)
{
    rtos_interrupt_event_raise(0);
}

void
fn_a(void)
{
    uint8_t count;
    rtos_unblock(0);
    rtos_unblock(1);

    debug_println("task a: taking lock");
    rtos_mutex_lock(0);
    rtos_yield();
    if (rtos_mutex_try_lock(0))
    {
        debug_println("unexpected mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(0);
    rtos_yield();

    for (count = 0; ; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_unblock(1);
        }
        debug_println("task a blocking");
        rtos_block();
    }
}

void
fn_b(void)
{
    uint8_t count;

    debug_println("task b: attempting lock");
    rtos_mutex_lock(0);
    debug_println("task b: got lock");

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

    rtos_start();
    for (;;) ;
}
