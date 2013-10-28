/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t MutexId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#mutexes}}
#define MUTEX_ID_{{name|u}} ((MutexId) UINT8_C({{idx}}))
{{/mutexes}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}mutex_lock(MutexId);
bool {{prefix}}mutex_try_lock(MutexId);
void {{prefix}}mutex_unlock(MutexId);

/*| headers |*/
#include <stdbool.h>

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
static bool
internal_mutex_try_lock(const MutexId m)
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

/*| public_functions |*/
void
{{prefix}}mutex_lock(const MutexId m)
{
    preempt_disable();

    while (!internal_mutex_try_lock(m))
    {
        {{prefix}}yield();
    }

    preempt_enable();
}

void
{{prefix}}mutex_unlock(const MutexId m)
{
    /* Note: assumes writing a single word is atomic */
    mutexes[m].locked = false;
}

bool
{{prefix}}mutex_try_lock(const MutexId m)
{
    bool r;

    preempt_disable();
    r = internal_mutex_try_lock(m);
    preempt_enable();

    return r;
}
