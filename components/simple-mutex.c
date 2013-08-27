/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t MutexId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define MUTEX_ID_C(x) ((MutexId) UINT8_C(x))
{{#mutexes}}
#define MUTEX_ID_{{name|u}} MUTEX_ID_C({{idx}})
{{/mutexes}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}mutex_lock(MutexId);
bool {{prefix}}mutex_try_lock(MutexId);
void {{prefix}}mutex_unlock(MutexId);

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

struct mutex {
    bool locked;
};

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
static struct mutex mutexes[{{mutexes.length}}];

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
void
{{prefix}}mutex_lock(const MutexId m)
{
    while (!{{prefix}}mutex_try_lock(m)) {
        {{prefix}}yield();
    }
}

void
{{prefix}}mutex_unlock(const MutexId m)
{
    mutexes[m].locked = false;
}

bool
{{prefix}}mutex_try_lock(const MutexId m)
{
    if (mutexes[m].locked)
    {
        return false;
    }
    else
    {
        mutexes[m].locked = true;
        return true;
    }
}
