/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/
#define TASK_ID_ZERO ((TaskId) UINT8_C(0))
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)

/*| type_definitions |*/
typedef uint8_t TaskId;
typedef TaskId TaskIdOption;


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
pub_sched_set_runnable(const TaskId task_id)
{
    sched_set_runnable(task_id);
}

void
pub_sched_set_blocked(const TaskId task_id)
{
    sched_set_blocked(task_id);
}

TaskIdOption
pub_sched_get_next(void)
{
    return sched_get_next();
}

struct sched * pub_sched_tasks = &sched_tasks;
