/*| headers |*/
#include "rtos-phact.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_declarations |*/
static void block();
static void unblock({{prefix_type}}TaskId task);
{{#mutexes.length}}
static void mutex_core_locked_by({{prefix_type}}MutexId mutex, {{prefix_type}}TaskId task);
static void mutex_core_unlocked({{prefix_type}}MutexId mutex);
{{/mutexes.length}}

/*| state |*/
{{#timers.length}}
static {{prefix_type}}TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    {{prefix_const}}TIMER_ID_{{timer.name|u}},
{{/tasks}}
};
{{/timers.length}}

/*| function_like_macros |*/
#define mutex_core_block_on(blocker) signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define mutex_core_unblock(task) signal_send_set(task, {{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define sem_core_block() signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define sem_core_unblock(task) signal_send_set(task, {{prefix_const}}SIGNAL_ID__TASK_TIMER)

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

{{#mutexes.length}}
static void
mutex_core_block_on_timeout(const {{prefix_type}}TaskId t, const {{prefix_type}}TicksRelative ticks)
{
    precondition_preemption_disabled();

    timer_oneshot(task_timers[get_current_task()], ticks);
    mutex_core_block_on(t);
    timer_disable(task_timers[get_current_task()]);

    postcondition_preemption_disabled();
}
{{/mutexes.length}}

static void
sem_core_block_timeout(const {{prefix_type}}TicksRelative ticks)
{
    precondition_preemption_disabled();

    timer_oneshot(task_timers[get_current_task()], ticks);
    sem_core_block();
    timer_disable(task_timers[get_current_task()]);

    postcondition_preemption_disabled();
}

static void
unblock(const {{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_runnable(task);

    /* Yield when we later re-enable preemption, because we may have set a higher priority task runnable. */
    preempt_pend();

    postcondition_preemption_disabled();
}

{{#mutexes.length}}
static void
mutex_core_locked_by(const {{prefix_type}}MutexId mutex, const {{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_mutex_locked_by(mutex, task);

    postcondition_preemption_disabled();
}

static void
mutex_core_unlocked(const {{prefix_type}}MutexId mutex)
{
    precondition_preemption_disabled();

    sched_set_mutex_unlocked(mutex);

    /* Yield when we later re-enable preemption, because this task will have reverted from the mutex's priority
     * ceiling back to its original (and necessarily lower) explicitly assigned priority. */
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
