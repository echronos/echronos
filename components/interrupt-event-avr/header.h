/*| public_headers |*/

/*| public_types |*/

/*| public_structures |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/
{{#interrupt_events.length}}
#define {{prefix_func}}interrupt_event_raise(interrupt_event_id) do { {{prefix_func}}_internal_pending_interrupt_events |= (1U << (interrupt_event_id)); } while (0)
{{/interrupt_events.length}}

/*| public_state |*/
{{#interrupt_events.length}}
extern volatile uint8_t {{prefix_func}}_internal_pending_interrupt_events;
{{/interrupt_events.length}}

/*| public_function_declarations |*/

/*| public_privileged_function_declarations |*/
