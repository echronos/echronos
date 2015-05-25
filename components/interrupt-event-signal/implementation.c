/*| headers |*/

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/
struct interrupt_event_handler {
    {{prefix_type}}TaskId task;
    {{prefix_type}}SignalSet sig_set;
};

/*| extern_declarations |*/

/*| function_declarations |*/
{{#interrupt_events.length}}
static void interrupt_event_handle({{prefix_type}}InterruptEventId interrupt_event_id);
{{/interrupt_events.length}}

/*| state |*/
{{#interrupt_events.length}}
struct interrupt_event_handler interrupt_events[{{interrupt_events.length}}] = {
{{#interrupt_events}}
    { {{prefix_const}}TASK_ID_{{task.name|u}}, {{prefix_const}}SIGNAL_SET_{{sig_set|u}} },
{{/interrupt_events}}
};
{{/interrupt_events.length}}

/*| function_like_macros |*/

/*| functions |*/
{{#interrupt_events.length}}
static void
interrupt_event_handle(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    precondition_preemption_disabled();

    internal_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INTERNAL_INVALID_ID);

#ifdef PREEMPTION_SUPPORT
    signal_send_set(interrupt_events[interrupt_event_id].task, interrupt_events[interrupt_event_id].sig_set);
#else
    {{prefix_func}}signal_send_set(interrupt_events[interrupt_event_id].task,
            interrupt_events[interrupt_event_id].sig_set);
#endif

    postcondition_preemption_disabled();
}
{{/interrupt_events.length}}

/*| public_functions |*/
[[#task_set]]
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_task_set(const {{prefix_type}}InterruptEventId interrupt_event_id, const {{prefix_type}}TaskId task_id)
{
    api_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INVALID_ID);
    api_assert(task_id < {{tasks.length}}, ERROR_ID_INVALID_ID);
    interrupt_events[interrupt_event_id].task = task_id;
}
{{/interrupt_events.length}}
[[/task_set]]
