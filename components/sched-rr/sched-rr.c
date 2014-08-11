/*| dependencies |*/
task
stack

/*| capabilities |*/
sched

/*| doc_concepts |*/
## Scheduling algorithm

The scheduling algorithm is an important component of the RTOS as it determines which of all the runnable tasks should become the next current task.
The RTOS uses a round-robin algorithm.

The tasks in the system can be imagined as forming points on a clockface, with the clock hand pointing to the current task.
When a task blocks (or yields) the algorithm determines the next task by moving the hand forward until a runnable task is found.
In the example on the left, A is the current task, and has just performed a yield.
As A has just performed a yield operation it is in the runnable state.
The algorithm firstly considers B, which is not runnable, and then consider C, which is runnable, so C is chosen.

<img src="img/round_robin_1.png" width="33%" />

The next diagram shows the state of the algorithm after choosing C.

<img src="img/round_robin_2.png" width="33%" />

While C is executing the task B may exit the blocked state and become runnable.
If C now performs a yield operation, the scheduling algorithm continues and chooses the next runnable task D.

<img src="img/round_robin_3.png" width="33%" />

As D was runnable before B became runnable this behaviour is what would be expected in most cases (and is the same as would happen in a FIFO algorithm).

The exact behaviour of the scheduler becomes slightly more interesting, and possibly somewhat unexpected if task E should now become runnable while D is executing.

If D now performs a yield after E becomes runnable then the scheduling algorithm picks E as the next current task.
In this case E is chosen before the task B is chosen, even though it became runnable after B.
Not only is E selected before B, it also selected before both F and G which were already runnable.
This is different to how a FIFO based scheduling algorithm would operate.

<img src="img/round_robin_4.png" width="33%" />

For every revolution of the clock hand each task has an opportunity to be selected.
This algorithm has the property that task execution always occurs in a predictable order.
For example, if A, B, and C are all runnable then the order of execution is always [A,B,C], and never, [B,A,C] or some other permutation.

Tasks are considered in the same order in which they are defined in the system configuration.

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

/*| type_definitions |*/
typedef {{prefix_type}}TaskId SchedIndex;

/*| structure_definitions |*/
struct sched_task {
    bool runnable;
};

struct sched {
    SchedIndex cur; /* The index of the currently scheduled task */
    struct sched_task tasks[{{tasks.length}}];
};

/*| extern_definitions |*/

/*| function_definitions |*/
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
