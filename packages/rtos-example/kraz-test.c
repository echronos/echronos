#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "rtos-kraz.h"

extern void debug_println(const char *msg);

void
fn_a(void)
{
    uint8_t count;
    rtos_signal_send_set(0, 0);
    rtos_signal_send_set(1, 0);

    for (count = 0; ; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_signal_send_set(1, 1);
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
            (void) rtos_signal_wait_set(1);
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
