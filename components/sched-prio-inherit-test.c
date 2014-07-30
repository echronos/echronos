/*| schema |*/
<entry name="prefix" type="ident" optional="true" />
<entry name="semaphores" type="list" default="[]" auto_index_field="idx">
    <entry name="semaphore" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>
<entry name="tasks" type="list" auto_index_field="idx">
    <entry name="task" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>
<entry name="mutexes" type="list" default="[]" auto_index_field="idx">
    <entry name="mutex" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>

/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT8_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT8_C({{tasks.length}} - 1))

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include <stdint.h>
#include "rtos-sched-prio-inherit-test.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)

/*| type_definitions |*/
typedef uint8_t {{prefix_type}}TaskId;
typedef {{prefix_type}}TaskId TaskIdOption;


/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

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
