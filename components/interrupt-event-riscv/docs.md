/*| provides |*/
interrupt-event-riscv
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
Raising an interrupt event causes the signal set associated with the interrupt event to be sent to the task associated with the interrupt event.
This and [<span class="api">timer_tick</span>] are the only RTOS API functions that an interrupt service routine may call.

/*| doc_configuration |*/
/*| doc_footer |*/
