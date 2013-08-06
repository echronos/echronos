/*| headers |*/
#include <stdbool.h>
#include <stddef.h>
/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

struct mutex {
    bool locked;
};

/*| extern_definitions |*/

/*| state |*/
static struct mutex mutexes[{{num_mutexes}}];

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
