/*| provides |*/
interrupt-event-make-runnable

/*| requires |*/
task
preempt
interrupt-event

/*| doc_header |*/

/*| doc_concepts |*/
## Interrupt Events

Interrupt events provide the bridge between [Interrupt Service Routines] and [Tasks].
In this RTOS variant, their implementation is trivially simple: raising an interrupt event makes a task runnable (see [Task States]).

To do so, an interrupt service routine calls the [<span class="api">interrupt_event_raise</span>] API with the task ID of the task to make runnable.
When the specified task is already runnable at the time of the call, the call has no application-visible effect.
When there are runnable tasks in the system at the time of the call, the scheduler considers the specified task for execution in the course of its [Scheduling Algorithm].
When there is no runnable task in the system at the time of the call, the scheduler immediately schedules the specified task.

/*| doc_api |*/

/*| doc_configuration |*/

/*| doc_footer |*/
