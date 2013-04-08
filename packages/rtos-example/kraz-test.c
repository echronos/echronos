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

    for (count = 0; count < 10; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("unblocking b");
            rtos_signal_send_set(1, 1);
        }
        rtos_yield();
    }

    debug_println("(1) task a finished sending signals");

    rtos_signal_send_set(1, 1);

    for (;;)
    {
        rtos_yield();
    }
}

void
fn_b(void)
{
    uint8_t count;
    SignalIdOption s;

    for (count = 0; count < 8; count++)
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

    debug_println("(1) task b finished receiving signals.");

    for (count = 0; ; count++)
    {
        if (rtos_signal_peek_set(1))
        {
            debug_println("signal!");
            s = rtos_signal_poll_set(1);
            if (s != 0)
            {
                debug_println("ERROR");
            }
            else
            {
                debug_println("Success.");
            }
            break;
        }
        else
        {
            debug_println("no signal!");
            s = rtos_signal_poll_set(1);
            if (s != SIGNAL_ID_NONE)
            {
                debug_println("ERROR");
            }
            else
            {
                debug_println("Success.");
            }
        }

        rtos_yield();
    }

    for (;;)
    {
        rtos_yield();
    }
}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
