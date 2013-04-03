#include <stddef.h>
#include <stdint.h>

#include "rtos-acamar.h"

extern void debug_println(const char *msg);

void
fn_a(void)
{
    for (;;)
    {
        rtos_yield_to(1);
        debug_println("task a");
    }
}

void
fn_b(void)
{
    for (;;)
    {
        rtos_yield_to(0);
        debug_println("task b");
    }
}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
