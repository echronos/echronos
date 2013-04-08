/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-rigel.h" code_gen="template" />
  </headers>
</module>*/
/* Headers */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "rtos-rigel.h"
[[ctxt_switch.headers]]
[[sched.headers]]
[[signal.headers]]
[[irq_event_arch.headers]]
[[irq_event.headers]]

/* Object-like macros */
#define TASK_ID_ZERO ((TaskId) 0u)
#define TASK_ID_NONE ((TaskIdOption) 0xffU)
[[ctxt_switch.object_like_macros]]
[[sched.object_like_macros]]
[[signal.object_like_macros]]
[[irq_event_arch.object_like_macros]]
[[irq_event.object_like_macros]]

/* Type definitions */
typedef TaskId TaskIdOption;
typedef TaskId IrqEventId;
[[ctxt_switch.type_definitions]]
[[sched.type_definitions]]
[[signal.type_definitions]]
[[irq_event_arch.type_definitions]]
[[irq_event.type_definitions]]

/* Structure definitions */
struct task
{
    context_t ctx;
};
[[sched.structure_definitions]]
[[signal.structure_definitions]]
[[irq_event_arch.structure_definitions]]
[[irq_event.structure_definitions]]

/* External definitions */
{{#tasks}}
extern void {{entry}}(void);
{{/tasks}}
[[ctxt_switch.extern_definitions]]
[[signal.extern_definitions]]
[[irq_event_arch.extern_definitions]]
[[irq_event.extern_definitions]]

/* State */
{{#tasks}}
static [[stack_type]] stack_{{idx}}[{{stack_size}}];
{{/tasks}}
static TaskId current_task;
static struct task tasks[{{num_tasks}}];
[[sched.state]]
[[signal.state]]
[[irq_event_arch.state]]
[[irq_event.state]]

/* Function-like macros */
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
#define irq_event_id_to_taskid(irq_event_id) ((TaskId)(irq_event_id))

[[ctxt_switch.function_like_macros]]
[[sched.function_like_macros]]
[[signal.function_like_macros]]
[[irq_event_arch.function_like_macros]]
[[irq_event.function_like_macros]]

/* Private functions */
void {{prefix}}yield(void);
void {{prefix}}signal_send_set(TaskId task_id, SignalSet sig_set);

[[ctxt_switch.functions]]

static void
_yield_to(TaskId to)
{
    TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

[[sched.functions]]

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

[[signal.functions]]

static void
handle_irq_event(IrqEventId irq_event_id)
{
    /* FIXME */
    {{prefix}}signal_send_set(0, 0x1);
}

[[irq_event_arch.functions]]
[[irq_event.functions]]

/* Public functions */
void
{{prefix}}yield(void)
{
    TaskId to = irq_event_get_next();
    _yield_to(to);
}

void
{{prefix}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{entry}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}

[[signal.public_functions]]
[[irq_event_arch.public_functions]]
[[irq_event.public_functions]]
