#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-rigel.h"

extern void debug_println(const char *msg);

#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018

#define SYST_CSR_READ() (*((volatile uint32_t*)SYST_CSR_REG))
#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)

#define SYST_RVR_READ() (*((volatile uint32_t*)SYST_RVR_REG))
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)

#define SYST_CVR_READ() (*((volatile uint32_t*)SYST_CVR_REG))
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

void
tick_irq(void)
{
    rtos_irq_event_raise(0);
}

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
        debug_println("task a yield");
        rtos_yield();
    }

    debug_println("A now waiting for ticks");
    for (;;)
    {
        (void) rtos_signal_wait_set(1);
        debug_println("tick");
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
    /* Set the systick reload value */
    SYST_RVR_WRITE(0x000fffff);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);

    debug_println("Starting RTOS");
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
