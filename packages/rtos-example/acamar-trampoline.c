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

extern void debug_println(const char *msg);
extern void armv7m_semihost_reset(void);

static TaskId yield_next;

void
special_yield(void)
{
//    debug_println("special yield");
    TaskId x = yield_next;
    if (yield_next == 0)
    {
        yield_next = 1;
    }
    else
    {
        yield_next = 0;
    }
    rtos_yield_to(x);
}

void
tick_irq(void)
{
    debug_println("tick");
}

void
fn_a(void)
{
    uint32_t count;
    for (count = 0; ; count++)
    {
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
    SYST_RVR_WRITE(0x000fffff);
    SYST_CVR_WRITE(0);
    SYST_CSR_WRITE((1 << 1) | 1);

    //AIRCR_WRITE(1);
    armv7m_semihost_reset();
//    rtos_start();
    for (;;) ;
}
