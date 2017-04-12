/*| headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static uint8_t timer_pending_ticks_get_and_clear_atomically(void);

/*| state |*/
static volatile uint8_t timer_pending_ticks;

/*| function_like_macros |*/
#define timer_pending_ticks_check() ((bool)timer_pending_ticks)

/*| functions |*/
static uint8_t
timer_pending_ticks_get_and_clear_atomically(void)
{
    uint8_t pending_ticks;
    asm volatile("cpsid i");
    pending_ticks = timer_pending_ticks;
    timer_pending_ticks = 0;
    asm volatile("cpsie i");
    return pending_ticks;
}

/*| public_functions |*/

/*| public_privileged_functions |*/
void
{{prefix_func}}timer_tick(void)
{
    if (timer_pending_ticks < 2)
    {
        timer_pending_ticks += 1;
    }
}
