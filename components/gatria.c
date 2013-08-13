/*| headers |*/

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t TaskId;

/*| public_macros |*/

/*| public_function_definitions |*/
void {{prefix}}yield_to(TaskId);
void {{prefix}}yield(void);
void {{prefix}}block(void);
void {{prefix}}unblock(TaskId);
void {{prefix}}start(void);

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

/*| state |*/
static TaskId current_task;
static struct task tasks[{{num_tasks}}];

/*| function_like_macros |*/
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
