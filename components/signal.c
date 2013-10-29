/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{signalset_size}}_t SignalSet;
typedef SignalSet SignalId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define SIGNAL_SET_EMPTY ((SignalSet) UINT{{signalset_size}}_C(0))
#define SIGNAL_SET_ALL ((SignalSet) UINT{{signalset_size}}_MAX)
{{#signal_sets}}
#define SIGNAL_SET_{{name|u}} ((SignalSet) UINT{{signalset_size}}_C({{value}}))
{{#singleton}}#define SIGNAL_ID_{{name|u}} ((SignalId) SIGNAL_SET_{{name|u}}){{/singleton}}
{{/signal_sets}}

/*| public_function_like_macros |*/
#define {{prefix}}signal_wait(requested_signal) \
    (void) {{prefix}}signal_wait_set(requested_signal)

#define {{prefix}}signal_poll(requested_signal) \
    ({{prefix}}signal_poll_set(requested_signal) != SIGNAL_SET_EMPTY)

#define {{prefix}}signal_peek(requested_signal) \
    ({{prefix}}signal_peek_set(requested_signal) != SIGNAL_SET_EMPTY)

#define {{prefix}}signal_send(task_id, signal) \
    {{prefix}}signal_send_set(task_id, signal)

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
SignalSet {{prefix}}signal_wait_set(SignalSet requested_signals);
SignalSet {{prefix}}signal_poll_set(SignalSet requested_signals);
SignalSet {{prefix}}signal_peek_set(SignalSet requested_signals);
void {{prefix}}signal_send_set(TaskId task_id, SignalSet signals);

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/
struct signal_task {
    SignalSet signals;
};

struct signal {
    struct signal_task tasks[{{tasks.length}}];
};

/*| extern_definitions |*/

/*| function_definitions |*/
static SignalSet _signal_recv(SignalSet *const cur_task_signals, const SignalSet mask);

/*| state |*/
static struct signal signal_tasks;

/*| function_like_macros |*/
#define _signal_peek(pending_signals, requested_signals) (((pending_signals) & (requested_signals)) != SIGNAL_SET_EMPTY)
#define _signal_pending(task_id, mask) ((PENDING_SIGNALS(task_id) & mask) == mask)
#define PENDING_SIGNALS(task_id) signal_tasks.tasks[task_id].signals

/*| functions |*/
static SignalSet
_signal_recv(SignalSet *const pending_signals, const SignalSet requested_signals)
{
    const SignalSet received_signals = *pending_signals & requested_signals;
    *pending_signals &= ~received_signals;

    return received_signals;
}

/*| public_functions |*/
SignalSet
{{prefix}}signal_wait_set(const SignalSet requested_signals)
{
    SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    SignalSet received_signals;

    preempt_disable();

    if (_signal_peek(*pending_signals, requested_signals))
    {
        {{prefix}}yield();
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

SignalSet
{{prefix}}signal_poll_set(const SignalSet requested_signals)
{
    SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    SignalSet received_signals;

    preempt_disable();

    received_signals = _signal_recv(pending_signals, requested_signals);

    preempt_enable();

    return received_signals;
}

SignalSet
{{prefix}}signal_peek_set(const SignalSet requested_signals)
{
    return _signal_peek(PENDING_SIGNALS(get_current_task()), requested_signals);
}

void
{{prefix}}signal_send_set(const TaskId task_id, const SignalSet signals)
{
    preempt_disable();

    PENDING_SIGNALS(task_id) |= signals;
    _unblock(task_id);

    preempt_enable();
}
