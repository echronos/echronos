/*| headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static uint8_t time_pending_ticks_get_and_clear_atomically(void);

/*| state |*/
static volatile uint8_t time_pending_ticks;

/*| function_like_macros |*/
#define time_pending_ticks_check() ((bool)time_pending_ticks)

/*| functions |*/
static uint8_t
time_pending_ticks_get_and_clear_atomically(void)
{
    uint8_t pending_ticks;
    asm volatile("cpsid i");
    pending_ticks = time_pending_ticks;
    time_pending_ticks = 0;
    asm volatile("cpsie i");
    return pending_ticks;
}

/*| public_functions |*/
void
{{prefix_func}}time_tick(void)
{
    /* If time_pending_ticks > 1, a timer overflow has occurred, which is considered fatal.
     * We discard any ticks after that to prevent the the variable from wrapping back to zero. */
    if (time_pending_ticks < 2)
    {
        time_pending_ticks += 1;
    }
}
