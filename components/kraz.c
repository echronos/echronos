/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT{{taskid_size}}_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT{{taskid_size}}_C({{tasks.length}} - 1))
{{#tasks}}
#define {{prefix_const}}TASK_ID_{{name|u}} (({{prefix_type}}TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}yield(void);
void {{prefix_func}}start(void);

/*| headers |*/
#include <stdint.h>
#include "rtos-kraz.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)

/*| type_definitions |*/
typedef {{prefix_type}}TaskId TaskIdOption;

/*| structure_definitions |*/
struct task
{
    context_t ctx;
};

/*| extern_definitions |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_definitions |*/
static void _yield_to(const {{prefix_type}}TaskId to);
static void _block(void);
static void _unblock(const {{prefix_type}}TaskId task);

/*| state |*/
static {{prefix_type}}TaskId current_task;
static struct task tasks[{{tasks.length}}];

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx

/*| functions |*/
static void
_yield_to(const {{prefix_type}}TaskId to)
{
    const {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
_block(void)
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

static void
_unblock(const {{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

/*| public_functions |*/
void
{{prefix_func}}yield(void)
{
    {{prefix_type}}TaskId to = sched_get_next();
    _yield_to(to);
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
