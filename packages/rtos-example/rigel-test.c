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

    /* FIXME: This is necessary to ensure all tasks are started */
    rtos_signal_send_set(0, 0);
    rtos_signal_send_set(1, 0);

    debug_println("task a: taking lock");
    rtos_mutex_lock(MUTEX_ID_TEST);
    rtos_yield();
    if (rtos_mutex_try_lock(MUTEX_ID_TEST))
    {
        debug_println("task a: ERROR: unexpected mutex not locked.");
    }
    debug_println("task a: releasing lock");
    rtos_mutex_unlock(0);
    rtos_yield();

    for (count = 0; count < 10; count++)
    {
        debug_println("task a");
        if (count % 5 == 0)
        {
            debug_println("task a: unblocking b");
            rtos_signal_send(TASK_ID_B, SIGNAL_ID_TEST);
        }
        debug_println("task a: yield");
        rtos_yield();
    }

    debug_println("task a: now waiting for ticks");
    for (;;)
    {
        rtos_signal_wait(SIGNAL_ID_TEST);
        debug_println("task a: tick");
    }
}

void
fn_b(void)
{
    uint8_t count;

    debug_println("task b: attempting lock");
    rtos_mutex_lock(MUTEX_ID_TEST);
    debug_println("task b: got lock");

    for (count = 0; ; count++)
    {
        debug_println("task b");
        if (count % 4 == 0)
        {
            debug_println("task b: blocking");
            rtos_signal_wait(SIGNAL_ID_TEST);
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
