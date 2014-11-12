/*| headers |*/
#include "rtos-kochab.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/
struct interrupt_event_handler {
    {{prefix_type}}TaskId task;
    {{prefix_type}}SignalSet sig_set;
};

/*| extern_definitions |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_definitions |*/
static void block(void);
static void unblock({{prefix_type}}TaskId task);
{{#interrupt_events.length}}
static void interrupt_event_handle({{prefix_type}}InterruptEventId interrupt_event_id);
{{/interrupt_events.length}}

/*| state |*/
{{#interrupt_events.length}}
static struct interrupt_event_handler interrupt_events[{{interrupt_events.length}}] = {
{{#interrupt_events}}
    { {{prefix_const}}TASK_ID_{{task.name|u}}, {{prefix_const}}SIGNAL_SET_{{sig_set|u}} },
{{/interrupt_events}}
};
{{/interrupt_events.length}}

/*| function_like_macros |*/
#define mutex_block_on(task) block_on(task)
#define mutex_unblock(task) unblock(task)

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
block_on(const {{prefix_type}}TaskId t)
{
    precondition_preemption_disabled();

    sched_set_blocked_on(get_current_task(), t);
    yield();

    postcondition_preemption_disabled();
}
{{/mutexes.length}}

static void
unblock(const {{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_runnable(task);

    /*
     * Note: When preemption is enabled a yield should be forced
     * as a higher priority task may have been scheduled.
     */
    preempt_pend();

    postcondition_preemption_disabled();
}

{{#interrupt_events.length}}
static void
interrupt_event_handle(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    precondition_preemption_disabled();

    internal_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INTERNAL_INVALID_ID);

    signal_send_set(interrupt_events[interrupt_event_id].task, interrupt_events[interrupt_event_id].sig_set);

    postcondition_preemption_disabled();
}
{{/interrupt_events.length}}

/*| public_functions |*/

void
{{prefix_func}}start(void)
{
    sem_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first({{prefix_const}}TASK_ID_ZERO);
}
