/* Applications do not necessarily access all RTOS APIs.
 * Therefore, they are marked as potentially unused for static analysis. */
/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_types |*/
typedef uint8_t {{prefix_type}}MutexId;

/*| public_structures |*/

/*| public_object_like_macros |*/
{{#mutexes.length}}
#define {{prefix_const}}MUTEX_ID_ZERO (({{prefix_type}}MutexId) UINT8_C(0))
#define {{prefix_const}}MUTEX_ID_MAX (({{prefix_type}}MutexId) UINT8_C({{mutexes.length}} - 1))
{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}
{{/mutexes.length}}

/*| public_function_like_macros |*/

/*| public_state |*/
{{#mutex.stats}}
/*@unused@*/
extern bool {{prefix_func}}mutex_stats_enabled;
{{/mutex.stats}}

/*| public_function_declarations |*/
{{#mutexes.length}}
/*@unused@*/
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId) {{prefix_const}}REENTRANT;
[[#lock_timeout]]
/*@unused@*/
bool {{prefix_func}}mutex_lock_timeout({{prefix_type}}MutexId, {{prefix_type}}TicksRelative) {{prefix_const}}REENTRANT;
[[/lock_timeout]]
/*@unused@*/
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
/*@unused@*/
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
/*@unused@*/
bool {{prefix_func}}mutex_holder_is_current({{prefix_type}}MutexId);
{{#mutex.stats}}
/*@unused@*/
void {{prefix_func}}mutex_stats_clear(void);
{{/mutex.stats}}
{{/mutexes.length}}

/*| public_privileged_function_declarations |*/
