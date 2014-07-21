/*| schema |*/
<entry name="prefix" type="ident" optional="true" />
<entry name="tasks" type="list" auto_index_field="idx">
    <entry name="task" type="dict">
        <entry name="priority" type="int" />
    </entry>
</entry>

/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}start(void);
void {{prefix_func}}yield(void);

/*| headers |*/
#include "rtos-kochab.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_definitions |*/
static void _yield(void);
static void _block(void);
static void _unblock({{prefix_type}}TaskId task);
static void _preempt_enable(void);

/*| state |*/
/* Simulate kochab's desired "yield on unblock" behaviour until we implement preemption */
static volatile bool preempt_pending;

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable() _preempt_enable()
#define _preempt_pend() preempt_pending = true
#define mutex_block_on(task) _block_on(task)
#define mutex_unblock(task) _unblock(task)

/*| functions |*/
{{#tasks}}
static void
entry_{{name}}(void)
{
    preempt_enable();
    {{function}}();
}
{{/tasks}}

static void
_yield(void)
{
    /* pre-condition: preemption disabled */
    const {{prefix_type}}TaskId from = get_current_task();
    const {{prefix_type}}TaskId to = sched_get_next();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
    /* post-condition: preemption disabled */
}

static void
_block(void)
{
    /* pre-condition: preemption disabled */
    sched_set_blocked(get_current_task());
    _yield();
    /* post-condition: preemption disabled */
}

static void
_block_on({{prefix_type}}TaskId t)
{
    /* pre-condition: preemption disabled */
    sched_set_blocked_on(get_current_task(), t);
    _yield();
    /* post-condition: preemption disabled */
}

static void
_unblock({{prefix_type}}TaskId task)
{
    /* pre-condition: preemption disabled */
    sched_set_runnable(task);

    /*
     * Note: When preemption is enabled a yield should be forced
     * as a higher priority task may have been scheduled.
     */
    _preempt_pend();

    /* post-condition: preemption disabled */
}

static void
_preempt_enable(void)
{
    /* This simulates kochab's desired "yield on unblock" behaviour until we implement preemption */
    if (preempt_pending)
    {
        preempt_pending = false;
        _yield();
    }
}

/*| public_functions |*/

void
{{prefix_func}}start(void)
{
    sem_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
