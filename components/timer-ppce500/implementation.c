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

    interrupts_disable();

    pending_ticks = timer_pending_ticks;
    timer_pending_ticks = 0;

    interrupts_enable();

    return pending_ticks;
}

/*| public_functions |*/
void
{{prefix_func}}timer_tick(void)
{
    /* If time_pending_ticks > 1, a timer overflow has occurred, which is considered fatal.
     * We discard any ticks after that to prevent the the variable from wrapping back to zero. */
    if (timer_pending_ticks < 2) {
        timer_pending_ticks += 1;
    }
}
