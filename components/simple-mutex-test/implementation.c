/*| headers |*/
#include "rtos-simple-mutex-test.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/
#define preempt_enable()
#define preempt_disable()
#define api_assert(expression, error_id) do { } while(0)

/*| functions |*/

/*| public_functions |*/

void (*yield_ptr)(void);
struct mutex * pub_mutexes = mutexes;

void pub_set_yield_ptr(void (*y)(void))
{
    yield_ptr = y;
}

void {{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    if (yield_ptr != NULL)
    {
        yield_ptr();
    }
}
