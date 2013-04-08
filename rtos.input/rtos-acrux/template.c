/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-acrux.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="idx" type="int" />
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
  </schema>
</module>*/
/* Headers */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "rtos-acrux.h"
[[ctxt_switch.headers]]
[[sched.headers]]
[[irq_event_arch.headers]]
[[irq_event.headers]]

/* Object-like macros */
#define TASK_ID_ZERO ((TaskId) 0u)
#define TASK_ID_NONE ((TaskIdOption) 0xffU)
[[ctxt_switch.object_like_macros]]
[[sched.object_like_macros]]
[[irq_event_arch.object_like_macros]]
[[irq_event.object_like_macros]]

/* Type definitions */
typedef TaskId TaskIdOption;
[[ctxt_switch.type_definitions]]
[[sched.type_definitions]]
[[irq_event_arch.type_definitions]]
[[irq_event.type_definitions]]

/* Structure definitions */
struct task
{
    context_t ctx;
};
[[sched.structure_definitions]]
[[irq_event_arch.structure_definitions]]
[[irq_event.structure_definitions]]

/* External definitions */
{{#tasks}}
extern void {{entry}}(void);
{{/tasks}}
[[ctxt_switch.extern_definitions]]
[[irq_event_arch.extern_definitions]]
[[irq_event.extern_definitions]]

/* State */
{{#tasks}}
static [[stack_type]] stack_{{idx}}[{{stack_size}}];
{{/tasks}}
static TaskId current_task;
static struct task tasks[{{num_tasks}}];
[[sched.state]]
[[irq_event_arch.state]]
[[irq_event.state]]

/* Function-like macros */
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
#define irq_event_id_to_taskid(irq_event_id) ((TaskId)(irq_event_id))

[[ctxt_switch.function_like_macros]]
[[sched.function_like_macros]]
[[irq_event_arch.function_like_macros]]
[[irq_event.function_like_macros]]

/* Private functions */
[[ctxt_switch.functions]]
[[sched.functions]]

static void
handle_irq_event(IrqEventId irq_event_id)
{
    sched_set_runnable(irq_event_id_to_taskid(irq_event_id));
}

[[irq_event_arch.functions]]
[[irq_event.functions]]

/* Public functions */
void
{{prefix}}yield_to(TaskId to)
{
    TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix}}yield(void)
{
    TaskId to = irq_event_get_next();
    {{prefix}}yield_to(to);
}

void
{{prefix}}block(void)
{
    sched_set_blocked(get_current_task());
    {{prefix}}yield();
}

void
{{prefix}}unblock(TaskId task)
{
    sched_set_runnable(task);
}

void
{{prefix}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{entry}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}

[[irq_event_arch.public_functions]]
[[irq_event.public_functions]]
