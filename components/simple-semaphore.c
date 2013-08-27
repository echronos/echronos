/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t SemId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define SEM_ID_C(x) ((SemId) UINT8_C(x))
{{#semaphores}}
#define SEM_ID_{{name}} SEM_ID_C({{idx}})
{{/semaphores}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix}}sem_post(SemId);
bool {{prefix}}sem_try_wait(SemId);
void {{prefix}}sem_wait(SemId);

/*| headers |*/

/*| object_like_macros |*/
#define SEM_ID_NONE ((SemIdOption) SEM_ID_C(UINT8_MAX))
#define SEM_ID_ZERO SEM_ID_C(0)
#define SEM_ID_MAX SEM_ID_C({{num_semaphores}})
#define SEM_VALUE_ZERO ((SemValue) UINT8_C(0))


/*| type_definitions |*/
typedef uint8_t SemValue;
typedef SemId SemIdOption;

/*| structure_definitions |*/

struct semaphore {
    SemValue value;
};

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
static struct semaphore semaphores[{{num_semaphores}}];
static SemIdOption waiters[{{num_tasks}}];

/*| function_like_macros |*/

/*| functions |*/
static void
sem_init(void)
{
    TaskId t;

    for (t = TASK_ID_ZERO; t <= TASK_ID_MAX; t++)
    {
        waiters[t] = SEM_ID_NONE;
    }
}

/*| public_functions |*/
void
{{prefix}}sem_wait(const SemId s)
{
    while (!{{prefix}}sem_try_wait(s))
    {
        waiters[get_current_task()] = s;
        _block();
    }
}

void
{{prefix}}sem_post(const SemId s)
{
    TaskId t;

    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        for (t = TASK_ID_ZERO; t <= TASK_ID_MAX; t++)
        {
            if (waiters[t] == s)
            {
                waiters[t] = SEM_ID_NONE;
                _unblock(t);
            }
        }
    }

    semaphores[s].value++;
}

bool
{{prefix}}sem_try_wait(const SemId m)
{
    if (semaphores[m].value == SEM_VALUE_ZERO)
    {
        return false;
    }
    else
    {
        semaphores[m].value--;
        return true;
    }
}
