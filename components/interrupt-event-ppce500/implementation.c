/*| headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
#include <stdbool.h>
{{/interrupt_events.length}}

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void interrupt_event_process(void);
static inline bool interrupt_application_event_check(void);
static inline void interrupt_event_wait(void);

/*| state |*/
{{#interrupt_events.length}}
static volatile uint32_t interrupt_event;
{{/interrupt_events.length}}

/*| function_like_macros |*/

/*| functions |*/
/* Clear the pending status for any outstanding interrupts and take the RTOS-defined action for each. */
static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    /* Take and operate on a copy so we're guaranteed to return after handling each pending event no more than once,
     * since an already-handled interrupt event may well arrive again during execution of the loop. */
    uint32_t tmp = interrupt_event;
    while (tmp != 0)
    {
        /* __builtin_ffs(x) returns 1 + the index of the least significant 1-bit in x, or returns zero if x is 0 */
        {{prefix_type}}InterruptEventId i = __builtin_ffs(tmp) - 1;
        interrupt_event &= ~(1U << i);
        handle_interrupt_event(i);
        tmp &= ~(1U << i);
    }
{{/interrupt_events.length}}
}

/* Return true if there are any pending interrupts, false otherwise. */
static inline bool
interrupt_application_event_check(void)
{
{{#interrupt_events.length}}
    return interrupt_event != 0;
{{/interrupt_events.length}}
{{^interrupt_events.length}}
    return false;
{{/interrupt_events.length}}
}

/* Check if there are any pending interrupt events, and if not, wait until an interrupt event has occurred. */
static inline void
interrupt_event_wait(void)
{
    disable_interrupts();
    if (!interrupt_event_check())
    {
        wait_for_interrupt();
    }
    enable_interrupts();
}

/*| public_functions |*/
{{#interrupt_events.length}}
/* Set the pending status for the given interrupt event id. */
void
{{prefix_func}}interrupt_event_raise({{prefix_type}}InterruptEventId interrupt_event_id)
{
    interrupt_event |= (1U << interrupt_event_id);
}
{{/interrupt_events.length}}
