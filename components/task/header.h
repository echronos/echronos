/*| public_headers |*/
#include <stdint.h>

/*| public_types |*/
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;

/*| public_structures |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT{{taskid_size}}_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT{{taskid_size}}_C({{tasks.length}} - 1))
{{#tasks}}
#define {{prefix_const}}TASK_ID_{{name|u}} (({{prefix_type}}TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
{{prefix_type}}TaskId {{prefix_func}}task_current(void);
