/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/
#define SCHED_INDEX_ZERO ((SchedIndex) UINT{{schedindex_size}}_C(0))
#define SCHED_INDEX_NONE ((SchedIndexOption) UINT{{schedindex_size}}_MAX)

/*| types |*/
typedef uint{{schedindex_size}}_t SchedIndex;
typedef SchedIndex SchedIndexOption;

/*| structures |*/
/*
 * The 'locked_by' field can take the following values:
 * locked_by == SCHED_INDEX_NONE indicates this entry is either a mutex that is not locked, or a task that is blocked.
 * locked_by == (the 'sched_queue' index of this entry) indicates this entry is a task that is runnable.
 * Else this entry is a locked mutex, with locked_by indicating the 'sched_queue' index of the task holding it.
 */
struct sched_entry {
    SchedIndexOption locked_by;
};

/*
 * A RTOS variant using the priority-ceiling protocol scheduler must ensure that the 'sched_queue' array (whose
 * entries represent mutexes and tasks) is sorted by descending priority.
 */
struct sched {
    struct sched_entry entries[{{mutex_tasks_length}}];
};

/*| extern_declarations |*/

/*| function_declarations |*/
static void sched_set_runnable({{prefix_type}}TaskId task_id);
static void sched_set_blocked({{prefix_type}}TaskId task_id);
{{#mutexes.length}}
static void sched_set_mutex_locked_by({{prefix_type}}MutexId mutex_id, {{prefix_type}}TaskId task_id);
static void sched_set_mutex_unlocked({{prefix_type}}MutexId mutex_id);
{{/mutexes.length}}
static [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]] sched_get_next(void);

/*| state |*/
static struct sched sched_queue = {
    {
{{#mutex_tasks}}
        {SCHED_INDEX_NONE},
{{/mutex_tasks}}
    }
};

{{#mutexes.length}}
static const SchedIndex mutex_to_index[{{mutexes.length}}] = {
{{#mutexes}}
    {{sched_idx}},
{{/mutexes}}
};
{{/mutexes.length}}

static const SchedIndex task_to_index[{{tasks.length}}] = {
{{#tasks}}
    {{sched_idx}},
{{/tasks}}
};

static const SchedIndex index_to_mutex_task[{{mutex_tasks_length}}] = {
{{#mutex_tasks}}
    {{idx}},
{{/mutex_tasks}}
};

/*| function_like_macros |*/
#define sched_runnable(task_id) (SCHED_OBJ_TASK(task_id).runnable)
#define sched_max_index() (SchedIndex)({{mutex_tasks_length}} - 1U)
#define sched_index_to_taskid(sched_index) ({{prefix_type}}TaskId)(index_to_mutex_task[sched_index])
#define sched_taskid_to_index(task_id) (SchedIndex)(task_to_index[task_id])
#define SCHED_OBJ(idx) sched_queue.entries[idx]
#define SCHED_OBJ_TASK(task_id) sched_queue.entries[sched_taskid_to_index(task_id)]
{{#mutexes.length}}
#define sched_index_to_mutexid(sched_index) ({{prefix_type}}MutexId)(index_to_mutex_task[sched_index])
#define sched_mutexid_to_index(mutex_id) (SchedIndex)(mutex_to_index[mutex_id])
#define SCHED_OBJ_MUTEX(mutex_id) sched_queue.entries[sched_mutexid_to_index(mutex_id)]
{{/mutexes.length}}

/*| functions |*/
static void
sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ_TASK(task_id).locked_by = sched_taskid_to_index(task_id);
}

static void
sched_set_blocked(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ_TASK(task_id).locked_by = SCHED_INDEX_NONE;
}

{{#mutexes.length}}
static void
sched_set_mutex_locked_by(const {{prefix_type}}MutexId mutex_id, const {{prefix_type}}TaskId task_id)
{
    internal_assert(SCHED_OBJ_MUTEX(mutex_id).locked_by == SCHED_INDEX_NONE,
            ERROR_ID_SCHED_PRIO_CEILING_MUTEX_ALREADY_LOCKED);
    /* Lower array indices correspond to higher priorities */
    internal_assert(sched_mutexid_to_index(mutex_id) < sched_taskid_to_index(task_id),
            ERROR_ID_SCHED_PRIO_CEILING_TASK_LOCKING_LOWER_PRIORITY_MUTEX);
    SCHED_OBJ_MUTEX(mutex_id).locked_by = sched_taskid_to_index(task_id);
}

static void
sched_set_mutex_unlocked(const {{prefix_type}}MutexId mutex_id)
{
    SCHED_OBJ_MUTEX(mutex_id).locked_by = SCHED_INDEX_NONE;
}
{{/mutexes.length}}

static [[#assume_runnable]]{{prefix_type}}TaskId[[/assume_runnable]][[^assume_runnable]]TaskIdOption[[/assume_runnable]]
sched_get_next(void)
{
    /*
     * In the case where assume_runnable is true and no runnable tasks are found, then an undefined task will be
     * returned from this function.
     */
    SchedIndexOption sched_idx, next_idx;
    SchedIndex idx;

    for (idx = SCHED_INDEX_ZERO; idx <= sched_max_index(); idx++) {
        sched_idx = idx;

        next_idx = SCHED_OBJ(sched_idx).locked_by;
        if (next_idx == SCHED_INDEX_NONE) {
            /* An unlocked mutex, or a non-runnable task */
            continue;
        } else if (next_idx == sched_idx) {
            /* A runnable task */
            break;
        } else if (SCHED_OBJ(next_idx).locked_by == next_idx) {
            /* A mutex locked by a runnable task */
            sched_idx = next_idx;
            break;
        }
    }

    return [[^assume_runnable]](TaskIdOption)[[/assume_runnable]] sched_index_to_taskid(sched_idx);
}

/*| public_functions |*/
