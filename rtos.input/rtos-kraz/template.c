/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-kraz.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="signalset_size" type="int" default="8"/>
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
#include "rtos-kraz.h"
[[ctxt_switch.headers]]
[[sched.headers]]

/* Object-like macros */
#define TASK_ID_ZERO ((TaskId) 0)
[[ctxt_switch.object_like_macros]]
[[sched.object_like_macros]]
[[signal.object_like_macros]]

/* Type definitions */
[[ctxt_switch.type_definitions]]
[[sched.type_definitions]]
[[signal.type_definitions]]

/* Structure definitions */
struct task
{
    context_t ctx;
};
[[sched.structure_definitions]]
[[signal.structure_definitions]]

/* External definitions */
{{#tasks}}
extern void {{entry}}(void);
{{/tasks}}
[[ctxt_switch.extern_definitions]]

/* State */
{{#tasks}}
static [[stack_type]] stack_{{idx}}[{{stack_size}}];
{{/tasks}}
static TaskId current_task;
static struct task tasks[{{num_tasks}}];
[[sched.state]]
[[signal.state]]

/* Function-like macros */
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
[[ctxt_switch.function_like_macros]]
[[sched.function_like_macros]]
[[signal.function_like_macros]]

/* Private functions */
void {{prefix}}yield(void);

[[ctxt_switch.functions]]

static void
_yield_to(const TaskId to)
{
    const TaskId from = get_current_task();
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
_unblock(const TaskId task)
{
    sched_set_runnable(task);
}

[[signal.functions]]

/* Public functions */
void
{{prefix}}yield(void)
{
    TaskId to = sched_get_next();
    _yield_to(to);
}

void
{{prefix}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{entry}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}

[[signal.public_functions]]
