/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#tasks}}
#define TASK_ID_{{name|u}} ((TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}yield_to(TaskId);
void {{prefix}}start(void);

/*| headers |*/
#include <stdint.h>
#include "rtos-acamar.h"

/*| object_like_macros |*/
#define TASK_ID_ZERO ((TaskId) UINT{{taskid_size}}_C(0))
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)

/*| type_definitions |*/
typedef TaskId TaskIdOption;

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

/*| state |*/
static TaskId current_task;
static struct task tasks[{{tasks.length}}];

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx

/*| functions |*/

/*| public_functions |*/
void
{{prefix}}yield_to(const TaskId to)
{
    const TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context(TASK_ID_ZERO));
}
