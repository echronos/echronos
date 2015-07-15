/*| headers |*/

/*| object_like_macros |*/
#define SEM_ID_NONE ((SemIdOption) UINT8_MAX)
#define SEM_VALUE_ZERO (({{prefix_type}}SemValue) UINT{{semaphore_value_size}}_C(0))
{{#semaphore_enable_max}}
#define SEM_VALUE_MAX (({{prefix_type}}SemValue) UINT{{semaphore_value_size}}_MAX)
{{/semaphore_enable_max}}


/*| types |*/
typedef {{prefix_type}}SemId SemIdOption;

/*| structures |*/
struct semaphore {
    {{prefix_type}}SemValue value;
{{#semaphore_enable_max}}
    {{prefix_type}}SemValue max;
{{/semaphore_enable_max}}
};

/*| extern_declarations |*/

/*| function_declarations |*/
static bool internal_sem_try_wait(const {{prefix_type}}SemId s);

/*| state |*/
static struct semaphore semaphores[{{semaphores.length}}];
static SemIdOption sem_waiters[{{tasks.length}}];

/*| function_like_macros |*/
#define assert_sem_valid(sem) api_assert(sem < {{semaphores.length}}, ERROR_ID_INVALID_ID)

/*| functions |*/
static void
sem_init(void)
{
    {{prefix_type}}TaskId t;

    for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
    {
        sem_waiters[t] = SEM_ID_NONE;
    }
}

static bool
internal_sem_try_wait(const {{prefix_type}}SemId s)
{
    precondition_preemption_disabled();

    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        return false;
    }
    else
    {
        semaphores[s].value--;
        return true;
    }

    postcondition_preemption_disabled();
}

/*| public_functions |*/
void
{{prefix_func}}sem_wait(const {{prefix_type}}SemId s) {{prefix_const}}REENTRANT
{
    assert_sem_valid(s);

    preempt_disable();

    while (!internal_sem_try_wait(s))
    {
        sem_waiters[get_current_task()] = s;
        sem_core_block();
    }

    preempt_enable();
}

[[#timeouts]]
bool
{{prefix_func}}sem_wait_timeout(const {{prefix_type}}SemId s, const {{prefix_type}}TicksRelative timeout)
        {{prefix_const}}REENTRANT
{
    bool ret;
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    assert_sem_valid(s);

    preempt_disable();

    while (!(ret = internal_sem_try_wait(s)) && absolute_timeout > {{prefix_func}}timer_current_ticks) {
        sem_waiters[get_current_task()] = s;
        sem_core_block_timeout(absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    preempt_enable();

    return ret;
}
[[/timeouts]]

void
{{prefix_func}}sem_post(const {{prefix_type}}SemId s)
{
    {{prefix_type}}TaskId t;

    assert_sem_valid(s);

    preempt_disable();

{{#semaphore_enable_max}}
    api_assert(semaphores[s].max != SEM_VALUE_ZERO, ERROR_ID_SEMAPHORE_MAX_USE_BEFORE_INIT);

    if (semaphores[s].value >= semaphores[s].max) {
        {{fatal_error}}(ERROR_ID_SEMAPHORE_MAX_EXCEEDED);
    }
{{/semaphore_enable_max}}

    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
        {
            if (sem_waiters[t] == s)
            {
                sem_waiters[t] = SEM_ID_NONE;
                sem_core_unblock(t);
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

{{#semaphore_enable_max}}
void
{{prefix_func}}sem_max_init(const {{prefix_type}}SemId s, const {{prefix_type}}SemValue max)
{
    assert_sem_valid(s);

    api_assert(max != SEM_VALUE_ZERO, ERROR_ID_SEMAPHORE_MAX_INVALID);
    api_assert(semaphores[s].max == SEM_VALUE_ZERO, ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT);

    semaphores[s].max = max;
}
{{/semaphore_enable_max}}
