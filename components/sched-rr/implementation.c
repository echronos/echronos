/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/

/*| types |*/
typedef {{prefix_type}}TaskId SchedIndex;

/*| structures |*/
struct sched_task {
    bool runnable;
};

struct sched {
    SchedIndex cur; /* The index of the currently scheduled task */
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
#define sched_next_index(cur) (((cur) == sched_max_index()) ? 0 : ((cur) + 1))
#define sched_get_cur_index() (sched_tasks.cur)
#define sched_set_cur_index(idx) sched_tasks.cur = (idx)
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
    [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]] task;
    SchedIndex next = sched_get_cur_index();
    bool found = false;

    do
    {
        next = sched_next_index(next);
        found = sched_runnable(sched_index_to_taskid(next));
    } while (
        (!found)
[[^assume_runnable]]
        && (next != (sched_get_cur_index()))
[[/assume_runnable]]
        );

[[^assume_runnable]]
    if (found)
[[/assume_runnable]]
    {
        task = sched_index_to_taskid(next);
    }
[[^assume_runnable]]
    else
    {
        next = sched_max_index();
        task = TASK_ID_NONE;
    }
[[/assume_runnable]]

    sched_set_cur_index(next);

    return task;
}

/*| public_functions |*/
