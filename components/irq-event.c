/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| state |*/

/*| function_like_macros |*/

/*| functions |*/
static TaskId
irq_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        irq_event_process();
        next = sched_get_next();
        if (next == TASK_ID_NONE)
        {
            irq_event_wait();
        }
        else
        {
            break;
        }
    }

    return next;
}

/*| public_functions |*/
