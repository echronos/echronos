/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_types |*/
typedef uint8_t {{prefix_type}}SemId;
typedef uint{{semaphore_value_size}}_t {{prefix_type}}SemValue;

/*| public_structures |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}SEM_ID_ZERO (({{prefix_type}}SemId) UINT8_C(0))
#define {{prefix_const}}SEM_ID_MAX (({{prefix_type}}SemId) UINT8_C({{semaphores.length}} - 1))
{{#semaphores}}
#define {{prefix_const}}SEM_ID_{{name|u}} (({{prefix_type}}SemId) UINT8_C({{idx}}))
{{/semaphores}}

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
void {{prefix_func}}sem_post({{prefix_type}}SemId);
bool {{prefix_func}}sem_try_wait({{prefix_type}}SemId);
void {{prefix_func}}sem_wait({{prefix_type}}SemId) {{prefix_const}}REENTRANT;
[[#timeouts]]
bool {{prefix_func}}sem_wait_timeout({{prefix_type}}SemId, {{prefix_type}}TicksRelative timeout)
        {{prefix_const}}REENTRANT;
[[/timeouts]]
{{#semaphore_enable_max}}
void {{prefix_func}}sem_max_init({{prefix_type}}SemId s, {{prefix_type}}SemValue max);
{{/semaphore_enable_max}}
