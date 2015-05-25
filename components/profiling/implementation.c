/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
{{#profiling}}
{{#profiling.task_uptime}}

static uint32_t profiling_task_uptimes[{{tasks.length}} + 1];

{{/profiling.task_uptime}}
{{/profiling}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
{{#profiling}}

/* MAINTENANCE:
 * When modifying or extending the profiling component, ensure that its code is covered by at least one of the example
 * systems to ensure it builds correctly.
 */

void
{{prefix_func}}profiling_record_sample(void)
{
    {{#profiling.task_uptime}}
    {{prefix_type}}TaskId idx;

    if (!system_is_idle)
    {
        idx = get_current_task();
    }
    else
    {
        idx = {{tasks.length}};
    }

    profiling_task_uptimes[idx] += 1;
    {{/profiling.task_uptime}}
}

{{/profiling}}
