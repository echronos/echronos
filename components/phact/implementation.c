/*| headers |*/
#include "rtos-phact.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_definitions |*/
static void block();
static void unblock({{prefix_type}}TaskId task);
{{#mutexes.length}}
static void mutex_core_locked_by({{prefix_type}}MutexId mutex, {{prefix_type}}TaskId task);
static void mutex_core_unlocked({{prefix_type}}MutexId mutex);
{{/mutexes.length}}

/*| state |*/

/*| function_like_macros |*/
#define mutex_core_block_on(blocker) block()
#define mutex_core_unblock(task) unblock(task)
#define sem_core_block() block()
#define sem_core_unblock(task) unblock(task)

/*| functions |*/
{{#tasks}}
static void
entry_{{name}}(void)
{
    precondition_preemption_disabled();

    preempt_enable();
    {{function}}();
}
{{/tasks}}

static void
block(void)
{
    precondition_preemption_disabled();

    sched_set_blocked(get_current_task());
    yield();

    postcondition_preemption_disabled();
}

static void
unblock(const {{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_runnable(task);

    /* Note: When preemption is enabled a yield should be forced as a higher priority task may have been scheduled. */
    preempt_pend();

    postcondition_preemption_disabled();
}

{{#mutexes.length}}
static void
mutex_core_locked_by({{prefix_type}}MutexId mutex, {{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_mutex_locked_by(mutex, task);

    postcondition_preemption_disabled();
}

static void
mutex_core_unlocked({{prefix_type}}MutexId mutex)
{
    precondition_preemption_disabled();

    sched_set_mutex_unlocked(mutex);

    /* Note: When preemption is enabled a yield should be forced as this task's priority may have been lowered. */
    preempt_pend();

    postcondition_preemption_disabled();
}
{{/mutexes.length}}

/*| public_functions |*/
void
{{prefix_func}}start(void)
{
    sem_init();
    preempt_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first({{prefix_const}}TASK_ID_ZERO);
}
