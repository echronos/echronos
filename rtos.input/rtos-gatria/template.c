/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-gatria.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="num_mutexes" type="int"/>
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
#include "rtos-gatria.h"
[[context_switch.headers]]
[[sched.headers]]

/* Object-like macros */
#define TASK_ID_ZERO ((TaskId) 0)
[[context_switch.object_like_macros]]
[[sched.object_like_macros]]
[[mutex.object_like_macros]]

/* Type definitions */
[[context_switch.type_definitions]]
[[sched.type_definitions]]
[[mutex.type_definitions]]

/* Structure definitions */
struct task
{
    context_t ctx;
};
[[sched.structure_definitions]]
[[mutex.structure_definitions]]

/* External definitions */
[[context_switch.extern_definitions]]
{{#tasks}}
extern void {{entry}}(void);
{{/tasks}}

/* Internal interface definitions */
/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
  *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The concept of a context and of the context_t type is abstract and may have different implementations on
 *  different platforms.
 *  It can be, e.g., a stack pointer or a data structure for user-level task switching as on POSIX.
 *  This function is expected to set ctx to a value that the RTOS can pass to this platform's implementation of
 *  context_switch() and context_switch_first().
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 *  For hardware platforms, this typically requires the following set up steps:
 *  - The value of ctx points to either the beginning or the end of the stack area.
 *  - The stack area contains fn so that the context-switch functions can pop it off the stack as a return address to
 *    begin execution at.
 *  - Optionally reserve additional stack space if the context-switch functions depend on it.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void
context_init(context_t *ctx, void (*fn)(void), [[stack_type]] *stack_base, size_t stack_size);

/* State */
{{#tasks}}
static [[stack_type]] stack_{{idx}}[{{stack_size}}];
{{/tasks}}
static TaskId current_task;
static struct task tasks[{{num_tasks}}];
[[sched.state]]
[[mutex.state]]

/* Function-like macros */
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
[[context_switch.function_like_macros]]
[[sched.function_like_macros]]
[[mutex.function_like_macros]]


/* Private functions */
[[context_switch.functions]]
[[sched.functions]]
[[mutex.functions]]

/* Public functions */
void
{{prefix}}yield_to(const TaskId to)
{
    const TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix}}yield(void)
{
    TaskId to = sched_get_next();
    {{prefix}}yield_to(to);
}

void
{{prefix}}block(void)
{
    sched_set_blocked(get_current_task());
    {{prefix}}yield();
}

void
{{prefix}}unblock(const TaskId task)
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

[[mutex.public_functions]]
