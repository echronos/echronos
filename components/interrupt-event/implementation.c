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
#define interrupt_event_check() (interrupt_application_event_check() || interrupt_system_event_check())
#define interrupt_system_event_check() [[#time_process]]time_pending_ticks_check()[[/time_process]][[^time_process]]false[[/time_process]]

/*| functions |*/
static {{prefix_type}}TaskId
interrupt_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        interrupt_event_process();
[[#time_process]]
        time_tick_process();
[[/time_process]]
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
