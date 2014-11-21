#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-kochab.h"
#include "debug.h"

#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018

#define SYST_CSR_READ() (*((volatile uint32_t*)SYST_CSR_REG))
#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)

#define SYST_RVR_READ() (*((volatile uint32_t*)SYST_RVR_REG))
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)

#define SYST_CVR_READ() (*((volatile uint32_t*)SYST_CVR_REG))
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

bool
tick_irq(void)
{
    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TICK);
    return true;
}

void
fatal(RtosErrorId error_id)
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
    rtos_signal_send_set(RTOS_TASK_ID_A, 0);
    rtos_signal_send_set(RTOS_TASK_ID_B, 0);

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
