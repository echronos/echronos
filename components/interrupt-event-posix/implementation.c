/*| headers |*/
#include <signal.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void interrupt_event_process(void);
static void interrupt_event_wait(void);
static sigset_t interrupt_event_set_default(void);

/*| state |*/
static unsigned int pending_interrupt_events;

/*| function_like_macros |*/
#define interrupt_application_event_check() (pending_interrupt_events != 0)

/*| functions |*/
static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    while (pending_interrupt_events != 0)
    {
        unsigned int current;
        for (current = 0; current < {{interrupt_events.length}} && pending_interrupt_events != 0; current += 1)
        {
            unsigned int mask = 1 << current;
            if (pending_interrupt_events & mask)
            {
                sigset_t set = interrupt_event_set_default();

                (void)sigprocmask(SIG_BLOCK, &set, NULL);
                pending_interrupt_events = pending_interrupt_events & (~mask);
                (void)sigprocmask(SIG_UNBLOCK, &set, NULL);

                interrupt_event_handle(current);
            }
        }
    }
{{/interrupt_events.length}}
}

static void
interrupt_event_wait(void)
{
    sigset_t set = interrupt_event_set_default();
    int sig;

    /* Atomically check for pending interrupt events and wait for signals.
     * 1. It is necessary to only wait for POSIX signals if there are no pending interrupt events.
     *    If sigwait() was called while there are pending interrupt events, those interrupt events would remain
     *    unprocessed.
     *    This would violate the system behavior guarantee that a system only stops scheduling tasks and processing
     *    interrupt events when no tasks are runnable and no interrupt events are pending.
     * 2. The check and the sigwait() call need to occur atomically.
     *    Otherwise, a POSIX signal could arrive and raise an interrupt event after the check and before the sigwait()
     *    call.
     *    This would violate item 1. above. */
    (void)sigprocmask(SIG_BLOCK, &set, NULL);
    if (!interrupt_event_check())
    {
        (void)sigwait(&set, &sig);
    }
    (void)sigprocmask(SIG_UNBLOCK, &set, NULL);

    /* sigwait() removes the POSIX signal 'sig' from the POSIX process's pending signal set.
     * Therefore, the host OS does not run the POSIX signal handler for this POSIX signal 'sig'.
     * However, system correctness requires that POSIX signals are not "lost" in this manner.
     * Therefore, re-raise the POSIX signal 'sig' to make it pending again and to trigger its handler. */
    raise(sig);
}

static sigset_t
interrupt_event_set_default(void)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGVTALRM);
    sigaddset(&set, SIGPROF);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGRTMIN);
    sigaddset(&set, SIGRTMAX);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    return set;
}

/*| public_functions |*/
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_raise(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    sigset_t set = interrupt_event_set_default();

    /* POSIX signal handlers can interrupt each other.
     * Therefore, manipulate 'pending_interrupt_events' atomically. */
    (void)sigprocmask(SIG_BLOCK, &set, NULL);
    pending_interrupt_events = pending_interrupt_events | (1 << interrupt_event_id);
    (void)sigprocmask(SIG_UNBLOCK, &set, NULL);
}
{{/interrupt_events.length}}

/*| public_privileged_functions |*/
