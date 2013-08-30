/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/
#define SCHED_INDEX_ZERO ((SchedIndex) TASK_ID_ZERO)

/*| type_definitions |*/
typedef TaskId SchedIndex;

/*| structure_definitions |*/
struct sched_task {
    bool runnable;
};

/*
 * NOTE: An RTOS variant using the scheduler must ensure that tasks
 * array is sorted by priority.
 */
struct sched {
    struct sched_task tasks[{{tasks.length}}];
};

/*| extern_definitions |*/

/*| function_definitions |*/
static void sched_set_runnable(const TaskId task_id);
static void sched_set_blocked(const TaskId task_id);
static [[#assume_runnable]]TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]] sched_get_next(void);

/*| state |*/
static struct sched sched_tasks;

/*| function_like_macros |*/
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_max_index() (SchedIndex)({{tasks.length}} - 1U)
#define sched_index_to_taskid(sched_index) (TaskId)(sched_index)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]

/*| functions |*/
static void
sched_set_runnable(const TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = true;
}

static void
sched_set_blocked(const TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = false;
}

static [[#assume_runnable]]TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]]
sched_get_next(void)
{
    /* NOTE: In the case where assume_runnable is true and no runnable
       tasks are found, then an undefined task will be returned from this
       function.
    */
    TaskId[[^assume_runnable]]Option[[/assume_runnable]] task[[^assume_runnable]] = TASK_ID_NONE[[/assume_runnable]];
    SchedIndex idx;

    for (idx = SCHED_INDEX_ZERO; idx <= sched_max_index(); idx++)
    {
        if (sched_runnable(sched_index_to_taskid(idx)))
        {
            task = sched_index_to_taskid(idx);
            break;
        }
    }

    return task;
}

/*| public_functions |*/
