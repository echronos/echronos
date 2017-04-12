/*| headers |*/
#include <signal.h>
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static uint8_t timer_pending_ticks_get_and_clear_atomically(void);

/*| state |*/
volatile unsigned int timer_ticks_pending;

/*| function_like_macros |*/
#define timer_pending_ticks_check() (timer_ticks_pending != 0)

/*| functions |*/
static uint8_t
timer_pending_ticks_get_and_clear_atomically(void)
{
    uint8_t result;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    (void)sigprocmask(SIG_BLOCK, &set, NULL);
    result = timer_ticks_pending;
    timer_ticks_pending = 0;
    (void)sigprocmask(SIG_UNBLOCK, &set, NULL);

    return result;
}

/*| public_functions |*/
void
{{prefix_func}}timer_tick(void)
{
    /* This function is called from a POSIX signal handler.
     * Assume that it is always the same handler, that a handler cannot interrupt itself, and therefore that there is
     * no concurrency issue in the following code. */
    if (timer_ticks_pending < 2)
    {
        timer_ticks_pending += 1;
    }
}

/*| public_privileged_functions |*/
