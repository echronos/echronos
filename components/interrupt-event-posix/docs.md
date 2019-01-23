/*| provides |*/
interrupt-event-posix
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
This API must only be called from [Interrupt Service Routines], not [Tasks].
The result of calling this API from a task instead of an ISR is undefined.

See [Interrupt Events] for further information on the effect of raising an interrupt event.

/*| doc_configuration |*/
/*| doc_footer |*/
