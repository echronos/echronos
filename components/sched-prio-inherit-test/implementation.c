/*| headers |*/
#include <stdint.h>
#include "rtos-sched-prio-inherit-test.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)

/*| types |*/
typedef uint8_t {{prefix_type}}TaskId;
typedef {{prefix_type}}TaskId TaskIdOption;

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()

/*| functions |*/

/*| public_functions |*/
void
pub_sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    sched_set_runnable(task_id);
}

void
pub_sched_set_blocked(const {{prefix_type}}TaskId task_id)
{
    sched_set_blocked(task_id);
}

void
pub_sched_set_blocked_on(const {{prefix_type}}TaskId task_id, const {{prefix_type}}TaskId blocked_on)
{
    sched_set_blocked_on(task_id, blocked_on);
}

TaskIdOption
pub_sched_get_next(void)
{
    return sched_get_next();
}

struct sched * pub_sched_tasks = &sched_tasks;
