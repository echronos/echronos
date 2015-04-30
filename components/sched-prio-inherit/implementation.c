/*| headers |*/

/*| object_like_macros |*/
#define SCHED_INDEX_ZERO ((SchedIndex) {{prefix_const}}TASK_ID_ZERO)

/*| types |*/
typedef {{prefix_type}}TaskId SchedIndex;

/*| structures |*/
struct sched_task {
    TaskIdOption blocked_on;
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
static void sched_set_blocked_on(const {{prefix_type}}TaskId task_id, const {{prefix_type}}TaskId blocker);
static [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]] sched_get_next(void);

/*| state |*/
static struct sched sched_tasks;

/*| function_like_macros |*/
#define sched_set_blocked(task_id) sched_set_blocked_on(task_id, TASK_ID_NONE)
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_max_index() (SchedIndex)({{tasks.length}} - 1U)
#define sched_index_to_taskid(sched_index) ({{prefix_type}}TaskId)(sched_index)
#define sched_taskid_to_index(task_id) (SchedIndex)(task_id)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]

/*| functions |*/
static void
sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).blocked_on = task_id;
}

static void
sched_set_blocked_on(const {{prefix_type}}TaskId task_id, const {{prefix_type}}TaskId blocker)
{
    SCHED_OBJ(task_id).blocked_on = blocker;
}

static [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]]
sched_get_next(void)
{
    /* NOTE: In the case where assume_runnable is true and no runnable
       tasks are found, then an undefined task will be returned from this
       function.
    */
    TaskIdOption task, next_task;
    SchedIndex idx;

    for (idx = SCHED_INDEX_ZERO; idx <= sched_max_index(); idx++)
    {
        task = sched_index_to_taskid(idx);
        do
        {
            next_task = SCHED_OBJ(task).blocked_on;
            if (next_task == task)
            {
                goto found;
            }
            task = next_task;
        }
        while (task != TASK_ID_NONE);
    }
found:
    return [[#assume_runnable]]({{prefix_type}}TaskId)[[/assume_runnable]] task;
}

/*| public_functions |*/
