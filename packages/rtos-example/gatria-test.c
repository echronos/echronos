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
