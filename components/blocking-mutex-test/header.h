/*| public_headers |*/
#include <stdint.h>

/*| public_types |*/
typedef uint8_t {{prefix_type}}TaskId;

/*| public_structures |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT8_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT8_C({{tasks.length}} - 1))

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
