/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{signalset_size}}_t {{prefix_type}}SignalSet;
typedef {{prefix_type}}SignalSet {{prefix_type}}SignalId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}SIGNAL_SET_EMPTY (({{prefix_type}}SignalSet) UINT{{signalset_size}}_C(0))
#define {{prefix_const}}SIGNAL_SET_ALL (({{prefix_type}}SignalSet) UINT{{signalset_size}}_MAX)
{{#signal_sets}}
#define {{prefix_const}}SIGNAL_SET_{{name|u}} (({{prefix_type}}SignalSet) UINT{{signalset_size}}_C({{value}}))
{{#singleton}}#define {{prefix_const}}SIGNAL_ID_{{name|u}} (({{prefix_type}}SignalId) {{prefix_const}}SIGNAL_SET_{{name|u}}){{/singleton}}
{{/signal_sets}}

/*| public_function_like_macros |*/
#define {{prefix_func}}signal_wait(requested_signal) \
    (void) {{prefix_func}}signal_wait_set(requested_signal)

#define {{prefix_func}}signal_poll(requested_signal) \
    ({{prefix_func}}signal_poll_set(requested_signal) != {{prefix_const}}SIGNAL_SET_EMPTY)

#define {{prefix_func}}signal_peek(requested_signal) \
    ({{prefix_func}}signal_peek_set(requested_signal) != {{prefix_const}}SIGNAL_SET_EMPTY)

#define {{prefix_func}}signal_send(task_id, signal) \
    {{prefix_func}}signal_send_set(task_id, signal)

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
{{prefix_type}}SignalSet {{prefix_func}}signal_wait_set({{prefix_type}}SignalSet requested_signals);
{{prefix_type}}SignalSet {{prefix_func}}signal_poll_set({{prefix_type}}SignalSet requested_signals);
{{prefix_type}}SignalSet {{prefix_func}}signal_peek_set({{prefix_type}}SignalSet requested_signals);
void {{prefix_func}}signal_send_set({{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signals);

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/
struct signal_task {
    {{prefix_type}}SignalSet signals;
};

struct signal {
    struct signal_task tasks[{{tasks.length}}];
};

/*| extern_definitions |*/

/*| function_definitions |*/
static {{prefix_type}}SignalSet _signal_recv({{prefix_type}}SignalSet *const cur_task_signals, const {{prefix_type}}SignalSet mask);

/*| state |*/
static struct signal signal_tasks;

/*| function_like_macros |*/
#define _signal_peek(pending_signals, requested_signals) (((pending_signals) & (requested_signals)) != {{prefix_const}}SIGNAL_SET_EMPTY)
#define _signal_pending(task_id, mask) ((PENDING_SIGNALS(task_id) & mask) == mask)
#define PENDING_SIGNALS(task_id) signal_tasks.tasks[task_id].signals

/*| functions |*/
static {{prefix_type}}SignalSet
_signal_recv({{prefix_type}}SignalSet *const pending_signals, const {{prefix_type}}SignalSet requested_signals)
{
    const {{prefix_type}}SignalSet received_signals = *pending_signals & requested_signals;
    *pending_signals &= ~received_signals;

    return received_signals;
}

/*| public_functions |*/
{{prefix_type}}SignalSet
{{prefix_func}}signal_wait_set(const {{prefix_type}}SignalSet requested_signals)
{
    {{prefix_type}}SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    {{prefix_type}}SignalSet received_signals;

    preempt_disable();

    if (_signal_peek(*pending_signals, requested_signals))
    {
        {{prefix_func}}yield();
    }
    else
    {
        do
        {
            _block();
        } while (!_signal_peek(*pending_signals, requested_signals));
    }

    received_signals = _signal_recv(pending_signals, requested_signals);

    preempt_enable();

    return received_signals;
}

{{prefix_type}}SignalSet
{{prefix_func}}signal_poll_set(const {{prefix_type}}SignalSet requested_signals)
{
    {{prefix_type}}SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    {{prefix_type}}SignalSet received_signals;

    preempt_disable();

    received_signals = _signal_recv(pending_signals, requested_signals);

    preempt_enable();

    return received_signals;
}

{{prefix_type}}SignalSet
{{prefix_func}}signal_peek_set(const {{prefix_type}}SignalSet requested_signals)
{
    return _signal_peek(PENDING_SIGNALS(get_current_task()), requested_signals);
}

void
{{prefix_func}}signal_send_set(const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signals)
{
    preempt_disable();

    PENDING_SIGNALS(task_id) |= signals;
    _unblock(task_id);

    preempt_enable();
}
