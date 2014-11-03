/*| headers |*/
#include "rtos-kochab.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_definitions |*/
static void yield(void);
static void block(void);
static void unblock({{prefix_type}}TaskId task);
static void preempt_enable(void);

/*| state |*/
/* Simulate kochab's desired "yield on unblock" behaviour until we implement preemption */
static volatile bool preempt_pending;

/*| function_like_macros |*/
#define preempt_disable()
#define mutex_block_on(task) block_on(task)
#define mutex_unblock(task) unblock(task)
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define postcondition_preemption_enabled()

/*| functions |*/
{{#tasks}}
static void
entry_{{name}}(void)
{
    preempt_enable();
    {{function}}();
}
{{/tasks}}

static void
yield(void)
{
    precondition_preemption_disabled();
    {
        const {{prefix_type}}TaskId from = get_current_task();
        const {{prefix_type}}TaskId to = sched_get_next();
        current_task = to;
        context_switch(get_task_context(from), get_task_context(to));
    }
    postcondition_preemption_disabled();
}

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
block_on({{prefix_type}}TaskId t)
{
    precondition_preemption_disabled();

    sched_set_blocked_on(get_current_task(), t);
    yield();

    postcondition_preemption_disabled();
}
{{/mutexes.length}}

static void
unblock({{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_runnable(task);

    /*
     * Note: When preemption is enabled a yield should be forced
     * as a higher priority task may have been scheduled.
     */
    preempt_pending = true;

    postcondition_preemption_disabled();
}

static void
preempt_enable(void)
{
    precondition_preemption_disabled();

    /* This simulates kochab's desired "yield on unblock" behaviour until we implement preemption */
    while (preempt_pending)
    {
        preempt_pending = false;
        yield();
    }

    postcondition_preemption_enabled();
}

/*| public_functions |*/

void
{{prefix_func}}start(void)
{
    sem_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
