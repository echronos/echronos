/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t TaskId;
typedef uint8_t ErrorId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#tasks}}
#define TASK_ID_{{name|u}} {{idx}}
{{/tasks}}

{{#irq_events}}
#define SIGNAL_SET_IRQ_{{name|u}} {{sig_set}}
{{/irq_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}start(void);
void {{prefix}}yield(void);
void {{prefix}}sleep(TicksRelative ticks);
void {{prefix}}task_start(TaskId task);
TaskId {{prefix}}task_current(void);

/*| headers |*/
#include <stdint.h>
#include "rtos-rigel.h"

/*| object_like_macros |*/
#define TASK_ID_ZERO ((TaskId) 0u)
#define TASK_ID_NONE ((TaskIdOption) 0xffU)

#define ERROR_ID_NONE ((ErrorId) 0u)
#define ERROR_ID_TICK_OVERFLOW ((ErrorId) 1u)

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
extern void {{fatal_error}}(ErrorId error_id);

/*| function_definitions |*/
static void _yield_to(TaskId to);
static void _block(void);
static void _unblock(TaskId task);
static void handle_irq_event(IrqEventId irq_event_id);


/*| state |*/
static TaskId current_task;
static struct task tasks[{{tasks.length}}];
static TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    TIMER_ID_{{timer.name|u}},
{{/tasks}}
};

struct irq_event_handler irq_events[{{irq_events.length}}] = {
{{#irq_events}}
 { TASK_ID_{{task.name|u}}, SIGNAL_SET_IRQ_{{name|u}} },
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

/* entry point trampolines */
{{#tasks}}
void _task_entry_{{name}}(void)
{
    {{^start}}{{prefix}}signal_wait(SIGNAL_ID__RTOS_UTIL);{{/start}}
    {{entry}}();
}

{{/tasks}}

/*| public_functions |*/
TaskId
{{prefix}}task_current(void)
{
    return current_task;
}

void
{{prefix}}task_start(TaskId task)
{
    {{prefix}}signal_send(task, SIGNAL_ID__RTOS_UTIL);
}

void
{{prefix}}yield(void)
{
    TaskId to = irq_event_get_next();
    _yield_to(to);
}

void
{{prefix}}sleep(TicksRelative ticks)
{
    {{prefix}}timer_oneshot(task_timers[get_current_task()], ticks);
    {{prefix}}signal_wait(SIGNAL_ID__TASK_TIMER);
}

void
{{prefix}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), _task_entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}
