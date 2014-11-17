#include "rtos-kochab.h"
#include "debug.h"

#define DEMO_PRODUCTION_LIMIT 10

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;)
    {
    }
}

void
fn_a(void)
{
    int i;

    /* Part 0: Solo */
    debug_println("");
    debug_println("Part 0: Solo");
    debug_println("");

    debug_println("a: initializing maximum");
    rtos_sem_max_init(RTOS_SEM_ID_sem0, DEMO_PRODUCTION_LIMIT);

    debug_println("a: V");
    rtos_sem_post(RTOS_SEM_ID_sem0);

    debug_println("a: P (should succeed)");
    rtos_sem_wait(RTOS_SEM_ID_sem0);
    debug_println("a: trying P (should fail)");
    if (rtos_sem_try_wait(RTOS_SEM_ID_sem0))
    {
        debug_println("a: P try unexpectedly succeeded!");
    }
    debug_println("a: V");
    rtos_sem_post(RTOS_SEM_ID_sem0);

    debug_println("a: trying P (should succeed)");
    if (!rtos_sem_try_wait(RTOS_SEM_ID_sem0))
    {
        debug_println("a: P try unexpectedly failed!");
    }

    /* Part 1: B unblocks A */
    debug_println("");
    debug_println("Part 1: B unblocks A");
    debug_println("");

    debug_println("a: P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_sem0);
    debug_println("a: now runnable");

    debug_println("a: consuming...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("a: P");
        rtos_sem_wait(RTOS_SEM_ID_sem0);
    }
    debug_println("a: trying P (should fail)");
    if (rtos_sem_try_wait(RTOS_SEM_ID_sem0))
    {
        debug_println("a: P try unexpectedly succeeded!");
    }

    debug_println("a: waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_ID_DEMO_HELPER);

    debug_println("a: consuming...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("a: P");
        rtos_sem_wait(RTOS_SEM_ID_sem0);
    }
    debug_println("a: trying P (should fail)");
    if (rtos_sem_try_wait(RTOS_SEM_ID_sem0))
    {
        debug_println("a: P try unexpectedly succeeded!");
    }

    /* Part 2: A and B compete */
    debug_println("");
    debug_println("Part 2: A and B compete");
    debug_println("");

    debug_println("a: P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_sem0);

    debug_println("a: should wake up before b. P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_sem0);

    debug_println("a: should again wake up before b. waiting on signal");
    rtos_signal_wait_set(RTOS_SIGNAL_ID_DEMO_HELPER);

    /* Part 3: A posts past maximum and triggers fatal error */
    debug_println("");
    debug_println("Part 3: A posts past maximum and triggers fatal error");
    debug_println("");

    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("a: P");
        rtos_sem_post(RTOS_SEM_ID_sem0);
    }

    debug_println("a: trying P (should trigger fatal error)");
    rtos_sem_post(RTOS_SEM_ID_sem0);

    debug_println("a: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_b(void)
{
    int i;

    /* Part 1: B unblocks A */
    debug_println("b: V (should unblock a)");
    rtos_sem_post(RTOS_SEM_ID_sem0);

    debug_println("b: producing while a consumes...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("b: V");
        rtos_sem_post(RTOS_SEM_ID_sem0);
    }

    debug_println("b: producing while a is blocked...");
    for (i = 0; i < DEMO_PRODUCTION_LIMIT; i++) {
        debug_println("b: V");
        rtos_sem_post(RTOS_SEM_ID_sem0);
    }

    debug_println("b: sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_ID_DEMO_HELPER);

    /* Part 2: A and B compete */
    debug_println("b: P (should block)");
    rtos_sem_wait(RTOS_SEM_ID_sem0);

    debug_println("b: finally awake. sending signal to a");
    rtos_signal_send_set(RTOS_TASK_ID_A, RTOS_SIGNAL_ID_DEMO_HELPER);

    debug_println("b: shouldn't be here!");
    for (;;)
    {
    }
}

void
fn_z(void)
{
    /* Part 2: A and B compete */
    debug_println("z: V");
    rtos_sem_post(RTOS_SEM_ID_sem0);
    debug_println("z: V");
    rtos_sem_post(RTOS_SEM_ID_sem0);
    debug_println("z: V");
    rtos_sem_post(RTOS_SEM_ID_sem0);

    debug_println("z: shouldn't be here!");
    for (;;)
    {
    }
}

int
main(void)
{
    rtos_start();
    /* Should never reach here, but if we do, an infinite loop is
       easier to debug than returning somewhere random. */
    for (;;) ;
}
