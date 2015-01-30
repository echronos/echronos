/* @LICENSE(NICTA) */

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
