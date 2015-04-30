/*| headers |*/
#include "rtos-kochab.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_declarations |*/
static void block_on({{prefix_type}}TaskId blocker);
{{#mutexes.length}}
static void mutex_core_block_on_timeout({{prefix_type}}TaskId t, {{prefix_type}}TicksRelative ticks);
{{/mutexes.length}}
static void sem_core_block_timeout({{prefix_type}}TicksRelative ticks);
static void unblock({{prefix_type}}TaskId task);

/*| state |*/
{{#timers.length}}
static {{prefix_type}}TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    {{prefix_const}}TIMER_ID_{{timer.name|u}},
{{/tasks}}
};
{{/timers.length}}

/*| function_like_macros |*/
#define block() block_on(TASK_ID_NONE)
#define mutex_core_block_on(blocker) signal_wait_blocked_on({{prefix_const}}SIGNAL_ID__TASK_TIMER, blocker)
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
block_on(const {{prefix_type}}TaskId blocker)
{
    precondition_preemption_disabled();

    sched_set_blocked_on(get_current_task(), blocker);
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

    /* Note: When preemption is enabled a yield should be forced as a higher priority task may have been scheduled. */
    preempt_pend();

    postcondition_preemption_disabled();
}

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
