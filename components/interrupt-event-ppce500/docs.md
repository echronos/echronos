/*| provides |*/
interrupt-event-ppce500
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
# Acknowledgements

This material is based on research sponsored by the Air Force Research Laboratory and the Defense Advanced Research Projects Agency (DARPA) under agreement number FA8750-12-9-0179.
The U.S. Government is authorised to reproduce and distribute reprints for Governmental purposes notwithstanding any copyright notation thereon.
The views and conclusions contained herein are those of the authors and should not be interpreted as necessarily representing the official policies or endorsements, either express or implied, of Air Force Research Laboratory, the Defense Advanced Research Projects Agency or the U.S. Government.

# Notes
