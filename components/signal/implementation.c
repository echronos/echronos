/*| headers |*/

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/
struct signal_task {
    {{prefix_type}}SignalSet signals;
};

struct signal {
    struct signal_task tasks[{{tasks.length}}];
};

/*| extern_declarations |*/

/*| function_declarations |*/
static {{prefix_type}}SignalSet signal_recv({{prefix_type}}SignalSet *pending_signals, {{prefix_type}}SignalSet requested_signals);
static void signal_send_set({{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signals);
[[^prio_inherit]]
static {{prefix_type}}SignalSet signal_wait_set({{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT;
[[/prio_inherit]]
[[#prio_inherit]]
static {{prefix_type}}SignalSet signal_wait_set_blocked_on({{prefix_type}}SignalSet requested_signals,
        {{prefix_type}}TaskId blocker) {{prefix_const}}REENTRANT;
[[/prio_inherit]]

/*| state |*/
static struct signal signal_tasks;

/*| function_like_macros |*/

[[#prio_inherit]]
#define signal_wait_blocked_on(requested_signals, blocker) signal_wait_set_blocked_on(requested_signals, blocker)
#define signal_wait_set(requested_signals) signal_wait_set_blocked_on(requested_signals, TASK_ID_NONE)
[[/prio_inherit]]
#define signal_wait(requested_signals) signal_wait_set(requested_signals)
#define signal_peek(pending_signals, requested_signals) (((pending_signals) & (requested_signals)) != {{prefix_const}}SIGNAL_SET_EMPTY)
#define signal_pending(task_id, mask) ((PENDING_SIGNALS(task_id) & mask) == mask)
#define PENDING_SIGNALS(task_id) signal_tasks.tasks[task_id].signals

/*| functions |*/
static {{prefix_type}}SignalSet
signal_recv({{prefix_type}}SignalSet *const pending_signals, const {{prefix_type}}SignalSet requested_signals)
{
    const {{prefix_type}}SignalSet received_signals = *pending_signals & requested_signals;

    precondition_preemption_disabled();

    *pending_signals &= ~received_signals;

    postcondition_preemption_disabled();

    return received_signals;
}

static void
signal_send_set(const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signals)
{
    precondition_preemption_disabled();

    PENDING_SIGNALS(task_id) |= signals;
    unblock(task_id);

    postcondition_preemption_disabled();
}

static {{prefix_type}}SignalSet
[[^prio_inherit]]
signal_wait_set(const {{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT
[[/prio_inherit]]
[[#prio_inherit]]
signal_wait_set_blocked_on(const {{prefix_type}}SignalSet requested_signals, const {{prefix_type}}TaskId blocker)
        {{prefix_const}}REENTRANT
[[/prio_inherit]]
{
    {{prefix_type}}SignalSet received_signals;

    precondition_preemption_disabled();
    {
        {{prefix_type}}SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());

        if (signal_peek(*pending_signals, requested_signals))
        {
            yield();
        }
        else
        {
            do
            {
[[^prio_inherit]]
                block();
[[/prio_inherit]]
[[#prio_inherit]]
                block_on(blocker);
[[/prio_inherit]]
            } while (!signal_peek(*pending_signals, requested_signals));
        }

        received_signals = signal_recv(pending_signals, requested_signals);
    }
    postcondition_preemption_disabled();

    return received_signals;
}

/*| public_functions |*/
{{prefix_type}}SignalSet
{{prefix_func}}signal_wait_set(const {{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT
{
    {{prefix_type}}SignalSet received_signals;

    preempt_disable();

    received_signals = signal_wait_set(requested_signals);

    preempt_enable();

    return received_signals;
}

{{prefix_type}}SignalSet
{{prefix_func}}signal_poll_set(const {{prefix_type}}SignalSet requested_signals)
{
    {{prefix_type}}SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    {{prefix_type}}SignalSet received_signals;

    preempt_disable();

    received_signals = signal_recv(pending_signals, requested_signals);

    preempt_enable();

    return received_signals;
}

{{prefix_type}}SignalSet
{{prefix_func}}signal_peek_set(const {{prefix_type}}SignalSet requested_signals)
{
    const {{prefix_type}}SignalSet pending_signals = PENDING_SIGNALS(get_current_task());
    return pending_signals & requested_signals;
}

void
{{prefix_func}}signal_send_set(const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signals)
{
    assert_task_valid(task_id);

    preempt_disable();

    signal_send_set(task_id, signals);

    preempt_enable();
}
