/*| provides |*/
interrupt-event-avr
interrupt-event-arch

/*| requires |*/
interrupt-event

/*| doc_header |*/
/*| doc_concepts |*/
/*| doc_api |*/
## Platform Interrupt Event API

### <span class="api">interrupt_event_raise</span>

<div class="codebox">void interrupt_event_raise(InterruptEventId event);</div>

The `interrupt_event_raise` API raises the specified interrupt event.
This API must be called only from an interrupt service routine (not a task).
On the AVR target platform, the RTOS supports at most 8 different interrupt events.
Raising an interrupt event sends one or more signals to a task.
The configuration item [`interrupt_events/interrupt_event/name`] specifies that specific signal set and the destination task for each interrupt event in the system.

This API function and [<span class="api">timer_tick</span>] are the only RTOS API functions that an interrupt service routine may call.

/*| doc_configuration |*/
/*| doc_footer |*/
