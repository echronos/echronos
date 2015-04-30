/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_types |*/
typedef uint{{signalset_size}}_t {{prefix_type}}SignalSet;
typedef {{prefix_type}}SignalSet {{prefix_type}}SignalId;

/*| public_structures |*/

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

/*| public_state |*/

/*| public_function_declarations |*/
{{prefix_type}}SignalSet {{prefix_func}}signal_wait_set({{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT;
{{prefix_type}}SignalSet {{prefix_func}}signal_poll_set({{prefix_type}}SignalSet requested_signals);
{{prefix_type}}SignalSet {{prefix_func}}signal_peek_set({{prefix_type}}SignalSet requested_signals);
void {{prefix_func}}signal_send_set({{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signals);
