#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtos-acamar.h"

#define SYST_CSR_REG 0xE000E010
#define SYST_RVR_REG 0xE000E014
#define SYST_CVR_REG 0xE000E018
#define AIRCR_REG 0xE000ED0C

#define AIRCR_WRITE(x) (*((volatile uint32_t*)AIRCR_REG) = x)

#define SYST_CSR_READ() (*((volatile uint32_t*)SYST_CSR_REG))
#define SYST_CSR_WRITE(x) (*((volatile uint32_t*)SYST_CSR_REG) = x)

#define SYST_RVR_READ() (*((volatile uint32_t*)SYST_RVR_REG))
#define SYST_RVR_WRITE(x) (*((volatile uint32_t*)SYST_RVR_REG) = x)

#define SYST_CVR_READ() (*((volatile uint32_t*)SYST_CVR_REG))
#define SYST_CVR_WRITE(x) (*((volatile uint32_t*)SYST_CVR_REG) = x)

#define COUNT_DELAY 50000000
#define SMALL_DELAY 50000

#define exception_preempt_disabled rtos_internal_exception_preempt_disabled
#define exception_preempt_pending rtos_internal_exception_preempt_pending

extern void debug_println(const char *msg);

static RtosTaskId yield_next;
static uint8_t ticks;

extern volatile uint8_t rtos_internal_exception_preempt_disabled;
extern volatile uint8_t rtos_internal_exception_preempt_pending;

void
special_yield(void)
{
    RtosTaskId x = yield_next;

    exception_preempt_disabled = 1;

    do
    {
        exception_preempt_pending = 0;

        debug_println("special!\n");
        if (yield_next == 0)
        {
            yield_next = 1;
        }
        else
        {
            yield_next = 0;
        }
        rtos_yield_to(x);

        if (exception_preempt_pending)
        {
            debug_println("Trampoline is pending!\n");
        }
    }
    while (exception_preempt_pending);

    exception_preempt_disabled = 0;
}

bool
tick_irq(void)
{
    debug_println("tick");
    return (ticks++ % 2) == 0;
}

void
fn_a(void)
{
    uint32_t count;
    for (count = 0; ; count++)
    {
        if ((count % SMALL_DELAY) == 0)
        {
            special_yield();
        }
        if ((count % COUNT_DELAY) == 0)
        {
            debug_println("task a");
        }
    }
}

void
fn_b(void)
{
    uint32_t count;
    for (count = 0; ; count++)
    {
        if ((count % SMALL_DELAY) == 0)
        {
            special_yield();
        }
        if ((count % COUNT_DELAY) == 0)
        {
            debug_println("task b");
        }
    }
}

int
main(void)
{
    /* Set the systick reload value */
    SYST_RVR_WRITE(20000);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);
    rtos_start();
    for (;;);
}
