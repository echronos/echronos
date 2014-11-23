/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}SemId;
typedef uint{{semaphore_value_size}}_t {{prefix_type}}SemValue;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#semaphores}}
#define {{prefix_const}}SEM_ID_{{name}} (({{prefix_type}}SemId) UINT8_C({{idx}}))
{{/semaphores}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}sem_post({{prefix_type}}SemId);
bool {{prefix_func}}sem_try_wait({{prefix_type}}SemId);
void {{prefix_func}}sem_wait({{prefix_type}}SemId) {{prefix_const}}REENTRANT;
{{#semaphore_enable_max}}
void {{prefix_func}}sem_max_init({{prefix_type}}SemId s, {{prefix_type}}SemValue max);
{{/semaphore_enable_max}}
