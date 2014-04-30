/*| schema |*/
<entry name="interrupteventid_size" type="int" default="8"/>
<entry name="interrupt_events" type="list" default="[]" auto_index_field="idx">
    <entry name="interrupt_event" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>

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
[[#task_set]]
{{#interrupt_events.length}}
void {{prefix_func}}interrupt_event_task_set({{prefix_type}}InterruptEventId interrupt_event_id, {{prefix_type}}TaskId task_id);
{{/interrupt_events.length}}
[[/task_set]]

/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static {{prefix_type}}TaskId interrupt_event_get_next(void);

/*| state |*/
static bool system_is_idle;

/*| function_like_macros |*/

/*| functions |*/
static {{prefix_type}}TaskId
interrupt_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        interrupt_event_process();
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
            system_is_idle = true;
            interrupt_event_wait();
        }
        else
        {
            system_is_idle = false;
            break;
        }
    }

    internal_assert_task_valid(next);

    return next;
}

/*| public_functions |*/
[[#task_set]]
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_task_set(const {{prefix_type}}InterruptEventId interrupt_event_id, const {{prefix_type}}TaskId task_id)
{
    api_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INVALID_ID);
    api_assert(task_id < {{tasks.length}}, ERROR_ID_INVALID_ID);
    interrupt_events[interrupt_event_id].task = task_id;
}
{{/interrupt_events.length}}
[[/task_set]]
