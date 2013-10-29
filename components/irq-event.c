/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{irqeventid_size}}_t {{prefix_type}}IrqEventId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#irq_events}}
#define IRQ_EVENT_ID_{{name|u}} (({{prefix_type}}IrqEventId) UINT{{irqeventid_size}}_C({{idx}}))
{{/irq_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static {{prefix_type}}TaskId irq_event_get_next(void);

/*| state |*/

/*| function_like_macros |*/

/*| functions |*/
static {{prefix_type}}TaskId
irq_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        irq_event_process();
[[#timer_process]]
        /* IMPROVE: This indicates we may want to factor things differently in the future */
        if (timer_check())
        {
            timer_process();
        }
[[/timer_process]]
        next = sched_get_next();
        if (next == TASK_ID_NONE)
        {
            irq_event_wait();
        }
        else
        {
            break;
        }
    }

    return next;
}

/*| public_functions |*/
