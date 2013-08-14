/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t SignalId;
typedef uint{{signalset_size}}_t SignalSet;
typedef SignalId SignalIdOption;

/*| public_object_like_macros |*/
#define SIGNAL_ID_C(x) ((SignalId) UINT8_C(x))
#define SIGNAL_ID_NONE ((SignalIdOption) SIGNAL_ID_C(0xff))

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
SignalId {{prefix}}signal_wait_set(SignalSet signal_set);
void {{prefix}}signal_send_set(TaskId task_id, SignalId signal_id);
SignalIdOption {{prefix}}signal_poll_set(SignalSet signal_set);
bool {{prefix}}signal_peek_set(SignalSet signal_set);

/*| headers |*/

/*| object_like_macros |*/
/* Sanity check; should be impossible (since there is no uint256_t!) */
#if {{signalset_size}} > UINT8_MAX
#  error "signalset_size ({{signalset_size}}) is greater than UINT8_MAX"
#endif

/*| type_definitions |*/

/*| structure_definitions |*/
struct signal_task {
    SignalSet signals;
};

struct signal {
    struct signal_task tasks[{{num_tasks}}];
};

/*| extern_definitions |*/

/*| function_definitions |*/
static SignalId _signal_recv(SignalSet *const cur_task_signals, const SignalSet mask);

/*| state |*/
static struct signal signal_tasks;

/*| function_like_macros |*/
#define _signal_peek(cur_task_signals, mask) ((*(cur_task_signals) & (mask)) != 0)
#define SIGNAL_OBJ(task_id) signal_tasks.tasks[task_id]

/*| functions |*/
static SignalId
_signal_recv(SignalSet *const cur_task_signals, const SignalSet mask)
{
    SignalSet signal_mask, signal_inverse;
    SignalId signal;

    signal_mask = *cur_task_signals & mask;

    signal_inverse = 1U;
    signal = 0;

    while ((signal_mask & 1U) == 0)
    {
        signal_mask >>= 1;
        signal_inverse <<= 1;
        signal++;
    }

    *cur_task_signals = *cur_task_signals & ~signal_inverse;

    return signal;
}

/*| public_functions |*/
SignalId
{{prefix}}signal_wait_set(const SignalSet sig_set)
{
    SignalSet *const cur_task_signals = &SIGNAL_OBJ(get_current_task()).signals;

    if (_signal_peek(cur_task_signals, sig_set))
    {
        {{prefix}}yield();
    }
    else
    {
        do
        {
            _block();
        } while (!_signal_peek(cur_task_signals, sig_set));
    }

    return _signal_recv(cur_task_signals, sig_set);
}

void
{{prefix}}signal_send_set(const TaskId task_id, const SignalSet sig_set)
{
    SIGNAL_OBJ(task_id).signals |= sig_set;
    _unblock(task_id);
}

bool
{{prefix}}signal_peek_set(SignalSet sig_set)
{
    SignalSet *const cur_task_signals = &SIGNAL_OBJ(get_current_task()).signals;

    return _signal_peek(cur_task_signals, sig_set);
}

SignalIdOption
{{prefix}}signal_poll_set(SignalSet sig_set)
{
    SignalSet *const cur_task_signals = &SIGNAL_OBJ(get_current_task()).signals;

    if (_signal_peek(cur_task_signals, sig_set))
    {
        return _signal_recv(cur_task_signals, sig_set);
    }
    else
    {
        return SIGNAL_ID_NONE;
    }
}
