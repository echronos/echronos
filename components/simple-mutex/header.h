/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_types |*/
{{#mutexes.length}}
typedef uint8_t {{prefix_type}}MutexId;
{{/mutexes.length}}

/*| public_structures |*/

/*| public_object_like_macros |*/
{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
{{#mutexes.length}}
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId) {{prefix_const}}REENTRANT;
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
{{/mutexes.length}}
