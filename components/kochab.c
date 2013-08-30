/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define TASK_ID_C(x) ((TaskId) UINT{{taskid_size}}_C(x))
{{#tasks}}
#define TASK_ID_{{name|u}} TASK_ID_C({{idx}})
{{/tasks}}
#define TASK_ID_ZERO TASK_ID_C(0)
#define TASK_ID_MAX TASK_ID_C({{tasks.length}} - 1)

{{#irq_events}}
#define SIGNAL_SET_IRQ_{{name|u}} {{sig_set}}
{{/irq_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}start(void);
void {{prefix}}yield(void);

/*| headers |*/
#include "rtos-kochab.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) TASK_ID_C(UINT{{taskid_size}}_MAX))

/*| type_definitions |*/
typedef TaskId TaskIdOption;

/*| structure_definitions |*/
struct task
{
    context_t ctx;
};

struct irq_event_handler {
    TaskId task;
    SignalSet sig_set;
};

/*| extern_definitions |*/
{{#tasks}}
extern void {{entry}}(void);
{{/tasks}}
extern volatile uint8_t exception_preempt_disabled;
extern volatile uint8_t exception_preempt_pending;

/*| function_definitions |*/
static void _yield(void);
static void _block(void);
static void _unblock(TaskId task);
static void handle_irq_event(IrqEventId irq_event_id);
static void _preempt_enable(void);

/*| state |*/
static TaskId current_task;
static struct task tasks[{{tasks.length}}];

struct irq_event_handler irq_events[{{irq_events.length}}] = {
{{#irq_events}}
 { {{task}}, SIGNAL_SET_IRQ_{{name|u}} },
{{/irq_events}}
};

/*| function_like_macros |*/
#define preempt_disable() exception_preempt_disabled = 1
#define preempt_enable() _preempt_enable()
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
#define irq_event_id_to_taskid(irq_event_id) ((TaskId)(irq_event_id))

/*| functions |*/
static void
_yield(void)
{
    /* pre-condition: preemption disabled */
    TaskId to = irq_event_get_next();
    TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
    /* post-condition: preemption disabled */
}

static void
_block(void)
{
    /* pre-condition: preemption disabled */
    sched_set_blocked(get_current_task());
    _yield();
    /* post-condition: preemption disabled */
}

static void
_block_on(TaskId t)
{
    /* pre-condition: preemption disabled */
    sched_set_blocked_on(get_current_task(), t);
    _yield();
    /* post-condition: preemption disabled */
}

static void
_unblock(TaskId task)
{
    /* pre-condition: preemption disabled */
    sched_set_runnable(task);
    /* Note: Must yield here as the task being unblocked may have an effective
       priority higher than the current task */
    _yield();
    /* post-condition: preemption disabled */
}

static void
handle_irq_event(IrqEventId irq_event_id)
{
    /* pre-condition: preemption disabled */
    TaskId task = irq_events[irq_event_id].task;
    SignalSet sig_set = irq_events[irq_event_id].sig_set;

    {{prefix}}signal_send_set(task, sig_set);
    /* post-condition: preemption disabled */
}

static void
_preempt_enable(void)
{
    /* FIXME: This should be done atomically. I.e.: with interrupts disabled */
    bool pending = exception_preempt_pending;
    if (!pending)
    {
        exception_preempt_disabled = 0;
    }

    if (pending)
    {
        _yield();
    }
}

/*| public_functions |*/
void
{{prefix}}preempt_handler(void)
{
    _yield();
    _preempt_enable();
}

void
{{prefix}}yield(void)
{
    /* pre-condition: preemption enabled */
    preempt_disable();
    _yield();
    preempt_enable();
    /* pre-condition: preemption enabled */
}

void
{{prefix}}start(void)
{
    sem_init();
    mutex_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), {{entry}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}
