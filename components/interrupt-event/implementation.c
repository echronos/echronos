/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
{{prefix_type}}TaskId rtos_internal_interrupt_event_get_next(void);

/*| state |*/
static bool system_is_idle;

/*| function_like_macros |*/
#define interrupt_event_check() (interrupt_application_event_check() || interrupt_system_event_check())
#define interrupt_system_event_check() [[#timer_process]]timer_pending_ticks_check()[[/timer_process]][[^timer_process]]false[[/timer_process]]
#define interrupt_event_get_next() rtos_internal_interrupt_event_get_next()

/*| functions |*/
{{prefix_type}}TaskId
rtos_internal_interrupt_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        interrupt_event_process();
[[#timer_process]]
        timer_tick_process();
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
