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

/*| function_definitions |*/
static void _yield_to(TaskId to);
static void _block(void);
static void _unblock(TaskId task);
static void handle_irq_event(IrqEventId irq_event_id);


/*| state |*/
static TaskId current_task;
static struct task tasks[{{tasks.length}}];

struct irq_event_handler irq_events[{{irq_events.length}}] = {
{{#irq_events}}
 { {{task}}, SIGNAL_SET_IRQ_{{name|u}} },
{{/irq_events}}
};

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
#define irq_event_id_to_taskid(irq_event_id) ((TaskId)(irq_event_id))

/*| functions |*/
static void
_yield_to(TaskId to)
{
    TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
_block(void)
{
    sched_set_blocked(get_current_task());
    {{prefix}}yield();
}

static void
_unblock(TaskId task)
{
    sched_set_runnable(task);
}

static void
handle_irq_event(IrqEventId irq_event_id)
{
    TaskId task = irq_events[irq_event_id].task;
    SignalSet sig_set = irq_events[irq_event_id].sig_set;

    {{prefix}}signal_send_set(task, sig_set);
}

/*| public_functions |*/
void
{{prefix}}yield(void)
{
    TaskId to = irq_event_get_next();
    _yield_to(to);
}

void
{{prefix}}start(void)
{
    sem_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), {{entry}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}
