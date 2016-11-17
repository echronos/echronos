/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static uint8_t timer_pending_ticks_get_and_clear_atomically(void);

/*| state |*/
volatile uint8_t timer_ticks_pending;

/*| function_like_macros |*/
#define timer_pending_ticks_check() (timer_ticks_pending != 0)

/*| functions |*/
static uint8_t
timer_pending_ticks_get_and_clear_atomically(void)
{
    uint8_t result;

    asm volatile("cli");
    result = timer_ticks_pending;
    timer_ticks_pending = 0;
    asm volatile("sei");

    if (result > 1)
    {
        fatal(ERROR_ID_TICK_OVERFLOW);
    }

    return result;
}

/*| public_functions |*/
void
{{prefix_func}}timer_tick(void)
{
    if (timer_ticks_pending < 2)
    {
        timer_ticks_pending += 1;
    }
}

/*| public_privileged_functions |*/
