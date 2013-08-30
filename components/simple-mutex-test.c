/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include "rtos-simple-mutex-test.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/

/*| function_like_macros |*/
#define preempt_enable()
#define preempt_disable()

/*| functions |*/

/*| public_functions |*/

void (*yield_ptr)(void);
struct mutex * pub_mutexes = mutexes;

void pub_set_yield_ptr(void (*y)(void))
{
    yield_ptr = y;
}

void {{prefix}}yield(void)
{
    if (yield_ptr != NULL)
    {
        yield_ptr();
    }
}
