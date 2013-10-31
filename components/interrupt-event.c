/*| public_headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
{{/interrupt_events.length}}

/*| public_type_definitions |*/
{{#interrupt_events.length}}
typedef uint{{interrupteventid_size}}_t {{prefix_type}}InterruptEventId;
{{/interrupt_events.length}}

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#interrupt_events}}
#define {{prefix_const}}INTERRUPT_EVENT_ID_{{name|u}} (({{prefix_type}}InterruptEventId) UINT{{interrupteventid_size}}_C({{idx}}))
{{/interrupt_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static {{prefix_type}}TaskId interrupt_event_get_next(void);

/*| state |*/

/*| function_like_macros |*/

/*| functions |*/
static {{prefix_type}}TaskId
interrupt_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
{{#interrupt_events.length}}
        interrupt_event_process();
{{/interrupt_events.length}}
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
{{#interrupt_events.length}}
            /* IMPROVE: reference to external 'current_task'; may require refactoring.
             * For example, the system falling idle could be treated as an event that can be hooked into.
             * Alternatively, this whole loop could be externalized and hooked into by components. */
            current_task = TASK_ID_NONE;

            interrupt_event_wait();
{{/interrupt_events.length}}
{{^interrupt_events.length}}
            {{fatal_error}}(ERROR_ID_NO_TASKS_AVAILABLE);
{{/interrupt_events.length}}
        }
        else
        {
            break;
        }
    }

    return next;
}

/*| public_functions |*/
