/*| headers |*/

/*| object_like_macros |*/
#define SEM_ID_NONE ((SemIdOption) UINT8_MAX)
#define SEM_ID_ZERO (({{prefix_type}}SemId) UINT8_C(0))
#define SEM_ID_MAX (({{prefix_type}}SemId) UINT8_C({{semaphores.length}}))
#define SEM_VALUE_ZERO ((SemValue) UINT{{semvalue_size}}_C(0))
{{#sem_max_init}}
#define SEM_VALUE_MAX ((SemValue) UINT{{semvalue_size}}_MAX)
{{/sem_max_init}}


/*| type_definitions |*/
typedef uint{{semvalue_size}}_t SemValue;
typedef {{prefix_type}}SemId SemIdOption;

/*| structure_definitions |*/

struct semaphore {
    SemValue value;
{{#sem_max_init}}
    SemValue max;
{{/sem_max_init}}
};

/*| extern_definitions |*/

/*| function_definitions |*/
static bool internal_sem_try_wait(const {{prefix_type}}SemId s);

/*| state |*/
static struct semaphore semaphores[{{semaphores.length}}];
static SemIdOption waiters[{{tasks.length}}];

/*| function_like_macros |*/
#define assert_sem_valid(sem) api_assert(sem < {{semaphores.length}}, ERROR_ID_INVALID_ID)

/*| functions |*/
static void
sem_init(void)
{
    {{prefix_type}}TaskId t;

    for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
    {
        waiters[t] = SEM_ID_NONE;
    }
}

static bool
internal_sem_try_wait(const {{prefix_type}}SemId s)
{
    /* Must be called with pre-emption disabled */
    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        return false;
    }
    else
    {
        semaphores[s].value--;
        return true;
    }
}

/*| public_functions |*/
void
{{prefix_func}}sem_wait(const {{prefix_type}}SemId s) {{prefix_const}}REENTRANT
{
    assert_sem_valid(s);

    preempt_disable();

    while (!internal_sem_try_wait(s))
    {
        waiters[get_current_task()] = s;
        block();
    }

    preempt_enable();
}

void
{{prefix_func}}sem_post(const {{prefix_type}}SemId s)
{
    {{prefix_type}}TaskId t;

    assert_sem_valid(s);

    preempt_disable();

{{#sem_max_init}}
    if (semaphores[s].value >= semaphores[s].max) {
        {{fatal_error}}(ERROR_ID_SEMAPHORE_MAX_EXCEEDED);
    }
{{/sem_max_init}}

    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
        {
            if (waiters[t] == s)
            {
                waiters[t] = SEM_ID_NONE;
                unblock(t);
            }
        }
    }

    semaphores[s].value++;

    preempt_enable();
}

bool
{{prefix_func}}sem_try_wait(const {{prefix_type}}SemId s)
{
    bool r;

    assert_sem_valid(s);

    preempt_disable();
    r = internal_sem_try_wait(s);
    preempt_enable();

    return r;
}

{{#sem_max_init}}
void
{{prefix_func}}sem_max_init(const {{prefix_type}}SemId s, const int max)
{
    if (semaphores[s].max != SEM_VALUE_ZERO) {
        {{fatal_error}}(ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT);
    }

    if (max > SEM_VALUE_MAX) {
        {{fatal_error}}(ERROR_ID_SEMAPHORE_MAX_EXCEEDED);
    }

    semaphores[s].max = (SemValue)max;
}
{{/sem_max_init}}
