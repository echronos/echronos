/*| headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
#include <stdbool.h>
{{/interrupt_events.length}}

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/
extern void rtos_internal_interrupts_disable(void);
extern void rtos_internal_interrupts_enable(void);
extern void rtos_internal_interrupts_wait(void);

/*| function_definitions |*/
static void interrupt_event_process(void);
static void interrupt_event_wait(void);

/*| state |*/
{{#interrupt_events.length}}
static volatile uint32_t interrupt_event;
{{/interrupt_events.length}}

/*| function_like_macros |*/
{{#interrupt_events.length}}
/* Return true if there are any pending interrupts, false otherwise. */
#define interrupt_application_event_check() (interrupt_event != 0)
{{/interrupt_events.length}}
{{^interrupt_events.length}}
#define interrupt_application_event_check() false
{{/interrupt_events.length}}
#define interrupts_disable() rtos_internal_interrupts_disable()
#define interrupts_enable() rtos_internal_interrupts_enable()
#define interrupts_wait() rtos_internal_interrupts_wait()

/*| functions |*/
/* Clear the pending status for any outstanding interrupts and take the RTOS-defined action for each. */
static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    uint32_t tmp;

    /* Take and operate on a copy so we're guaranteed to return after handling each pending event no more than once,
     * since an already-handled interrupt event may well arrive again during execution of the loop.
     *
     * If an interrupt comes in between the preempt_clear and the taking of the copy, it will be taken into account by
     * the scheduling run but the pending preempt will force the scheduler to be run again, unnecessarily.
     *
     * This is somewhat a lesser evil than placing the preempt_clear after the taking of the copy, in which case an
     * interrupt that comes in between may result in the scheduling invariant being violated, since the interrupt will
     * not affect the current scheduling run, nor will it trigger a re-run of the scheduler.
     *
     * To prevent either case, we turn the two actions into a single atomic step by disabling interrupts. */
#ifdef PREEMPTION_SUPPORT
    interrupts_disable();
    preempt_clear();
    tmp = interrupt_event;
    interrupts_enable();
#else
    tmp = interrupt_event;
#endif
    while (tmp != 0)
    {
        /* __builtin_ffs(x) returns 1 + the index of the least significant 1-bit in x, or returns zero if x is 0 */
        {{prefix_type}}InterruptEventId i = __builtin_ffs(tmp) - 1;
        interrupt_event &= ~(1U << i);
        handle_interrupt_event(i);
        tmp &= ~(1U << i);
    }
{{/interrupt_events.length}}
{{^interrupt_events.length}}
    /* Even if this system has no interrupt events configured, RTOS variants that use the preempt pending status to
     * determine whether or not to run the scheduler need to have it cleared at this juncture. */
    preempt_clear();
{{/interrupt_events.length}}
}

/* Check if there are any pending interrupt events, and if not, wait until an interrupt event has occurred. */
static void
interrupt_event_wait(void)
{
    interrupts_disable();
    if (!interrupt_event_check())
    {
        interrupts_wait();
    }
    interrupts_enable();
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
