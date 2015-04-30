/*| headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
#include <stdbool.h>
{{/interrupt_events.length}}

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
extern void rtos_internal_interrupts_disable(void);
extern void rtos_internal_interrupts_enable(void);
extern void rtos_internal_interrupts_wait(void);
extern bool rtos_internal_check_interrupts_enabled(void);

/*| function_declarations |*/
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
#define irqs_enabled() rtos_internal_check_interrupts_enabled()
#define precondition_interrupts_disabled() internal_assert(!irqs_enabled(), ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define precondition_interrupts_enabled() internal_assert(irqs_enabled(), ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_interrupts_disabled() internal_assert(!irqs_enabled(), ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_interrupts_enabled() internal_assert(irqs_enabled(), ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)

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
        const {{prefix_type}}InterruptEventId i = __builtin_ffs(tmp) - 1;
        interrupt_event &= ~(1U << i);
        interrupt_event_handle(i);
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
{{prefix_func}}interrupt_event_raise(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    interrupt_event |= (1U << interrupt_event_id);
}
{{/interrupt_events.length}}
