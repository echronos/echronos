/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}yield(void);
void {{prefix}}start(void);

/*| headers |*/
#include <stdint.h>
#include "rtos-kraz.h"

/*| object_like_macros |*/
#define TASK_ID_ZERO ((TaskId) 0)

/*| type_definitions |*/

/*| structure_definitions |*/
struct task
{
    context_t ctx;
};

/*| extern_definitions |*/
{{#tasks}}
extern void {{entry}}(void);
{{/tasks}}

/*| function_definitions |*/
static void _yield_to(const TaskId to);
static void _block(void);
static void _unblock(const TaskId task);

/*| state |*/
static TaskId current_task;
static struct task tasks[{{num_tasks}}];

/*| function_like_macros |*/
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx

/*| functions |*/
static void
_yield_to(const TaskId to)
{
    const TaskId from = get_current_task();
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
_unblock(const TaskId task)
{
    sched_set_runnable(task);
}

/*| public_functions |*/
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
