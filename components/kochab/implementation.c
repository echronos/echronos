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
extern void rtos_internal_interrupts_disable(void);
extern void rtos_internal_interrupts_enable(void);
extern void rtos_internal_interrupts_wait(void);
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_definitions |*/
static void _block(void);
static void _unblock({{prefix_type}}TaskId task);
{{#interrupt_events.length}}
static void handle_interrupt_event({{prefix_type}}InterruptEventId interrupt_event_id);
{{/interrupt_events.length}}

/*| state |*/
struct interrupt_event_handler interrupt_events[{{interrupt_events.length}}] = {
{{#interrupt_events}}
 { {{prefix_const}}TASK_ID_{{task_name|u}}, {{prefix_const}}SIGNAL_SET_INTERRUPT_{{name|u}} },
{{/interrupt_events}}
};

/*| function_like_macros |*/
#define mutex_block_on(task) _block_on(task)
#define mutex_unblock(task) _unblock(task)
#define interrupts_disable() rtos_internal_interrupts_disable()
#define interrupts_enable() rtos_internal_interrupts_enable()
#define interrupts_wait() rtos_internal_interrupts_wait()

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
_block(void)
{
    precondition_preemption_disabled();

    sched_set_blocked(get_current_task());
    _yield();

    postcondition_preemption_disabled();
}

{{#mutexes.length}}
static void
_block_on({{prefix_type}}TaskId t)
{
    precondition_preemption_disabled();

    sched_set_blocked_on(get_current_task(), t);
    _yield();

    postcondition_preemption_disabled();
}
{{/mutexes.length}}

static void
_unblock({{prefix_type}}TaskId task)
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
handle_interrupt_event({{prefix_type}}InterruptEventId interrupt_event_id)
{
    precondition_preemption_disabled();

    {{prefix_type}}TaskId task = interrupt_events[interrupt_event_id].task;
    {{prefix_type}}SignalSet sig_set = interrupt_events[interrupt_event_id].sig_set;

    signal_send_set(task, sig_set);

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

    context_preempt_first({{prefix_const}}TASK_ID_ZERO);
}
