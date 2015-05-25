/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/
#define SCHED_INDEX_ZERO ((SchedIndex) {{prefix_const}}TASK_ID_ZERO)

/*| types |*/
typedef {{prefix_type}}TaskId SchedIndex;

/*| structures |*/
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

/*| extern_declarations |*/

/*| function_declarations |*/
static void sched_set_runnable(const {{prefix_type}}TaskId task_id);
static void sched_set_blocked(const {{prefix_type}}TaskId task_id);
static [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]] sched_get_next(void);

/*| state |*/
static struct sched sched_tasks;

/*| function_like_macros |*/
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_max_index() (SchedIndex)({{tasks.length}} - 1U)
#define sched_index_to_taskid(sched_index) ({{prefix_type}}TaskId)(sched_index)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]

/*| functions |*/
static void
sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = true;
}

static void
sched_set_blocked(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = false;
}

static [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]]
sched_get_next(void)
{
    /* NOTE: In the case where assume_runnable is true and no runnable
       tasks are found, then an undefined task will be returned from this
       function.
    */
    {{prefix_type}}TaskId[[^assume_runnable]]Option[[/assume_runnable]] task[[^assume_runnable]] = TASK_ID_NONE[[/assume_runnable]];
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
