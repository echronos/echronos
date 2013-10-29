/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
{{#profiling}}

void {{prefix}}profiling_record_sample(void);

{{/profiling}}

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

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

void
{{prefix}}profiling_record_sample(void)
{
    {{#profiling.task_uptime}}
    TaskId idx = get_current_task();

    if (idx == TASK_ID_NONE)
    {
        idx = {{tasks.length}};
    }

    profiling_task_uptimes[idx] += 1;
    {{/profiling.task_uptime}}
}

{{/profiling}}
