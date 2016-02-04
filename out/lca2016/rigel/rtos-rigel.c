

#include <stddef.h>
#include <stdint.h>

#include <stdbool.h>

#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>


#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include "rtos-rigel.h"


#define CONTEXT_SIZE (sizeof(struct context))




#define TIMER_ID_ZERO ((RtosTimerId) UINT8_C(0))
#define TIMER_ID_MAX ((RtosTimerId) UINT8_C(7 - 1U))



#define MUTEX_ID_NONE ((MutexIdOption) UINT8_MAX)

#define MESSAGE_QUEUE_ID_NONE ((MessageQueueIdOption)UINT8_C(255))
#define ERROR_ID_NONE ((RtosErrorId) UINT8_C(0))
#define ERROR_ID_TICK_OVERFLOW ((RtosErrorId) UINT8_C(1))
#define ERROR_ID_INVALID_ID ((RtosErrorId) UINT8_C(2))
#define ERROR_ID_NOT_HOLDING_MUTEX ((RtosErrorId) UINT8_C(3))
#define ERROR_ID_DEADLOCK ((RtosErrorId) UINT8_C(4))
#define ERROR_ID_TASK_FUNCTION_RETURNS ((RtosErrorId) UINT8_C(5))
#define ERROR_ID_INTERNAL_CURRENT_TASK_INVALID ((RtosErrorId) UINT8_C(6))
#define ERROR_ID_INTERNAL_INVALID_ID ((RtosErrorId) UINT8_C(7))
#define ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP ((RtosErrorId) UINT8_C(8))
#define ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT ((RtosErrorId) UINT8_C(9))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT ((RtosErrorId) UINT8_C(10))
#define ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER ((RtosErrorId) UINT8_C(11))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW ((RtosErrorId) UINT8_C(12))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION ((RtosErrorId) UINT8_C(13))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION ((RtosErrorId) UINT8_C(14))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD ((RtosErrorId) UINT8_C(15))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE ((RtosErrorId) UINT8_C(16))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS ((RtosErrorId) UINT8_C(17))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES ((RtosErrorId) UINT8_C(18))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED ((RtosErrorId) UINT8_C(19))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER ((RtosErrorId) UINT8_C(20))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE ((RtosErrorId) UINT8_C(21))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH ((RtosErrorId) UINT8_C(22))
#define ERROR_ID_INTERNAL_PRECONDITION_VIOLATED ((RtosErrorId) UINT8_C(23))
#define ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED ((RtosErrorId) UINT8_C(24))
#define ERROR_ID_SEMAPHORE_MAX_INVALID ((RtosErrorId) UINT8_C(25))
#define ERROR_ID_SEMAPHORE_MAX_USE_BEFORE_INIT ((RtosErrorId) UINT8_C(26))
#define ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT ((RtosErrorId) UINT8_C(27))
#define ERROR_ID_SEMAPHORE_MAX_EXCEEDED ((RtosErrorId) UINT8_C(28))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED ((RtosErrorId) UINT8_C(29))
#define ERROR_ID_SCHED_PRIO_CEILING_TASK_LOCKING_LOWER_PRIORITY_MUTEX ((RtosErrorId) UINT8_C(30))
#define ERROR_ID_SCHED_PRIO_CEILING_MUTEX_ALREADY_LOCKED ((RtosErrorId) UINT8_C(31))
/* The TASK_ID_NONE and TASK_ID_END macros require some care:
 * - TASK_ID_NONE is a valid integer within the value range of the TaskIdOption/TaskId types.
 *   There is no fundamental safeguard against the application defining TASK_ID_NONE+1 tasks so that the last task
 *   receives a task ID that is numerically equal to TASK_ID_NONE.
 * - TASK_ID_END is of type integer, not TaskIdOption/TaskId.
 *   It may hold the value TASK_ID_MAX + 1 which potentially exceeds the valid value range of TaskIdOption/TaskId.
 *   It can therefore not necessarily be safely assigned to or cast to type TaskIdOption/TaskId. */
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)
#define TASK_ID_END (5)
#define current_task rtos_internal_current_task
#define tasks rtos_internal_tasks

typedef struct context* context_t;
typedef RtosTaskId SchedIndex;
typedef uint16_t TicksTimeout;
typedef RtosMutexId MutexIdOption;
typedef uint8_t MessageQueueIdOption;
typedef RtosTaskId TaskIdOption;


/**
 * A C representation of the data stored on the stack by the x86 context-switch implementation.
 * Note that this data structure and the context-switch implementation need to be consistent.
 * Also note that the RTOS does not store this data structure as the per-task context information.
 * Instead, it just stores a stack pointer for each inactive task.
 * This data structure describes the data that resides at such a stored stack pointer of an inactive task.
 *
 * The RTOS core uses the type `context_t` for storing the execution context of an inactive task.
 * The x86 context switch implementation stores all task execution context on the task's stack before a context
 * switch.
 * Therefore, the only task execution context the RTOS core needs to handle is a task's stack pointer.
 * `context_t` is therefore a pointer type and `struct context` describes the data that can be found at such a
 * pointer address.
 */
struct context
{
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp_stack_frame;
    void (*return_address)(void);
};

struct sched_task {
    bool runnable;
};

struct sched {
    SchedIndex cur; /* The index of the currently scheduled task */
    struct sched_task tasks[5];
};
struct signal_task {
    RtosSignalSet signals;
};

struct signal {
    struct signal_task tasks[5];
};

struct timer
{
    bool enabled;
    bool overflow;
    TicksTimeout expiry;
    RtosTicksRelative reload;

    /*
     * when error_id is not ERROR_ID_NONE, the timer calls
     * the application error function with this error_id.
     */
    RtosErrorId error_id;

    RtosTaskId task_id;
    RtosSignalSet signal_set;
};


struct interrupt_event_handler {
    RtosTaskId task;
    RtosSignalSet sig_set;
};

struct mutex {
    TaskIdOption holder;
};

/* representation of a message queue instance
 * sorted by size of fields */
struct message_queue
{
    /* pointer to the array holding the message data
     * the array contains message_size * queue_length bytes */
    /*@shared@*/ uint8_t *messages; /* "shared" tells splint that message_queue_{name}_messages may be accessed directly, not only through this field */
    /* size of each message in bytes */
    const uint8_t message_size;
    /* maximum number of messages this queue can hold */
    const uint8_t queue_length;
    /* index of the oldest message that has been put into the queue but not yet been retrieved
     * 0 <= head < queue_length */
    uint8_t head;
    /* number of messages that have been put into the queue but not yet been retrieved
     * 0 <= available < queue_length */
    uint8_t available;
};

struct task
{
    context_t ctx;
};



/** C declaration of the context switch implementation in assembly in packages/x86/ctxt-switch.s */
extern void rtos_internal_context_switch_x86(context_t *from, context_t to);











extern /*@noreturn@*/ void fatal(RtosErrorId error_id);
extern void task_one(void);
extern void task_two(void);
extern void task_three(void);
extern void task_four(void);
extern void task_five(void);



static void context_init(context_t *ctx, void (*task_function)(void), uint8_t *stack_base, size_t stack_size);

static void sched_set_runnable(const RtosTaskId task_id);
static void sched_set_blocked(const RtosTaskId task_id);
static TaskIdOption sched_get_next(void);
static RtosSignalSet signal_recv(RtosSignalSet *pending_signals, RtosSignalSet requested_signals);
static void signal_send_set(RtosTaskId task_id, RtosSignalSet signals);
static RtosSignalSet signal_wait_set(RtosSignalSet requested_signals) RTOS_REENTRANT;

static uint8_t timer_pending_ticks_get_and_clear_atomically(void);
static void timer_process_one(struct timer *timer);
static void timer_enable(RtosTimerId timer_id);
static void timer_oneshot(RtosTimerId timer_id, RtosTicksRelative timeout);
static void timer_tick_process(void);
static void interrupt_event_process(void);
static void interrupt_event_wait(void);
RtosTaskId rtos_internal_interrupt_event_get_next(void);
static void interrupt_event_handle(RtosInterruptEventId interrupt_event_id);




static RtosTaskId get_current_task_check(void);
static void yield_to(RtosTaskId to) RTOS_REENTRANT;
static void block(void) RTOS_REENTRANT;
static void unblock(RtosTaskId task);

static uint8_t stack_0[8192];
static uint8_t stack_1[8192];
static uint8_t stack_2[8192];
static uint8_t stack_3[8192];
static uint8_t stack_4[8192];


static struct sched sched_tasks;
static struct signal signal_tasks;
volatile unsigned int timer_ticks_pending;
RtosTicksAbsolute rtos_timer_current_ticks;
static struct timer timers[7] = {
    {
        false,
        false,
        0,
        0,
        0,
        TASK_ID_NONE,
        RTOS_SIGNAL_SET_EMPTY
    },
    {
        false,
        false,
        0,
        0,
        0,
        TASK_ID_NONE,
        RTOS_SIGNAL_SET_EMPTY
    },
    {
        false,
        false,
        0,
        0,
        0,
        RTOS_TASK_ID_ONE,
        RTOS_SIGNAL_SET__TASK_TIMER
    },
    {
        false,
        false,
        0,
        0,
        0,
        RTOS_TASK_ID_TWO,
        RTOS_SIGNAL_SET__TASK_TIMER
    },
    {
        false,
        false,
        0,
        0,
        0,
        RTOS_TASK_ID_THREE,
        RTOS_SIGNAL_SET__TASK_TIMER
    },
    {
        false,
        false,
        0,
        0,
        0,
        RTOS_TASK_ID_FOUR,
        RTOS_SIGNAL_SET__TASK_TIMER
    },
    {
        false,
        false,
        0,
        0,
        0,
        RTOS_TASK_ID_FIVE,
        RTOS_SIGNAL_SET__TASK_TIMER
    },
};
static uint32_t pending_interrupt_events;
static bool system_is_idle;
static struct interrupt_event_handler interrupt_events[5] = {
    { RTOS_TASK_ID_ONE, RTOS_SIGNAL_SET_ONE },
    { RTOS_TASK_ID_TWO, RTOS_SIGNAL_SET_TWO },
    { RTOS_TASK_ID_THREE, RTOS_SIGNAL_SET_THREE },
    { RTOS_TASK_ID_FOUR, RTOS_SIGNAL_SET_FOUR },
    { RTOS_TASK_ID_FIVE, RTOS_SIGNAL_SET_FIVE },
};
static struct mutex mutexes[5] = {
    {TASK_ID_NONE},
    {TASK_ID_NONE},
    {TASK_ID_NONE},
    {TASK_ID_NONE},
    {TASK_ID_NONE},
};
static MutexIdOption mutex_waiters[5] = {
    MUTEX_ID_NONE,
    MUTEX_ID_NONE,
    MUTEX_ID_NONE,
    MUTEX_ID_NONE,
    MUTEX_ID_NONE,
};
static uint8_t message_queue_one_messages[10][100];
static struct message_queue message_queues[] =
{
    {
        (uint8_t*)message_queue_one_messages,
        100,
        10,
        0,
        0,
    },
};
static MessageQueueIdOption message_queue_waiters[] =
{
    MESSAGE_QUEUE_ID_NONE,
    MESSAGE_QUEUE_ID_NONE,
    MESSAGE_QUEUE_ID_NONE,
    MESSAGE_QUEUE_ID_NONE,
    MESSAGE_QUEUE_ID_NONE,
};


/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
RtosTaskId rtos_internal_current_task;
/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
struct task rtos_internal_tasks[5];
static RtosTimerId task_timers[5] = {
    RTOS_TIMER_ID__TASK_ONE,
    RTOS_TIMER_ID__TASK_TWO,
    RTOS_TIMER_ID__TASK_THREE,
    RTOS_TIMER_ID__TASK_FOUR,
    RTOS_TIMER_ID__TASK_FIVE,
};

#define stack_init()
/* context_switch(context_t *from, context_t *to) is a component API; translate it to the implementation; */
#define context_switch(from, to)\
    do\
    {\
        if (from != to)\
        {\
            rtos_internal_context_switch_x86(from, *to);\
        }\
    } while(0);

/* context_switch_first(context_t *to) is a component API; translate it to the implementation; */
#define context_switch_first(to)\
    do\
    {\
        context_t unused_context;\
        context_switch(&unused_context, to);\
    } while(0);
#define preempt_disable()
#define preempt_enable()
#define preempt_clear()
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_next_index(cur) (((cur) == sched_max_index()) ? 0 : ((cur) + 1))
#define sched_get_cur_index() (sched_tasks.cur)
#define sched_set_cur_index(idx) sched_tasks.cur = (idx)
#define sched_max_index() (SchedIndex)(5 - 1U)
#define sched_index_to_taskid(sched_index) (RtosTaskId)(sched_index)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]

#define signal_wait(requested_signals) (void)signal_wait_set(requested_signals)
#define signal_peek(pending_signals, requested_signals) (((pending_signals) & (requested_signals)) != RTOS_SIGNAL_SET_EMPTY)
#define signal_pending(task_id, mask) ((PENDING_SIGNALS(task_id) & mask) == mask)
#define PENDING_SIGNALS(task_id) signal_tasks.tasks[task_id].signals
#define timer_pending_ticks_check() (timer_ticks_pending != 0)
#define timer_expired(timer, timeout) ((timer)->enabled && (timer)->expiry == timeout)
#define timer_is_periodic(timer) ((timer)->reload > 0)
#define timer_reload_set(timer_id, ticks) timers[timer_id].reload = ticks
#define timer_disable(timer_id) timers[timer_id].enabled = false
#define current_timeout() ((TicksTimeout) rtos_timer_current_ticks)
#define TIMER_PTR(timer_id) (&timers[timer_id])
#define assert_timer_valid(timer) api_assert(timer_id < 7, ERROR_ID_INVALID_ID)
#define interrupt_application_event_check() (pending_interrupt_events != 0)
#define interrupt_event_check() (interrupt_application_event_check() || interrupt_system_event_check())
#define interrupt_system_event_check() timer_pending_ticks_check()
#define interrupt_event_get_next() rtos_internal_interrupt_event_get_next()

#define assert_mutex_valid(mutex) api_assert(mutex < 5, ERROR_ID_INVALID_ID)

#define message_queue_api_assert_valid(message_queue) api_assert(message_queue < 1,\
                                                                 ERROR_ID_INVALID_ID)
#define message_queue_internal_assert_valid(message_queue) internal_assert(message_queue < 1,\
                                                                           ERROR_ID_INVALID_ID)

#define api_error(error_id) fatal(error_id)
#define api_assert(expression, error_id) do { if (!(expression)) { api_error(error_id); } } while(0)

#define internal_error(error_id) fatal(error_id)
#define internal_assert(expression, error_id) do { if (!(expression)) { internal_error(error_id); } } while(0)
#define get_current_task() get_current_task_check()
#define get_task_context(task_id) &tasks[task_id].ctx
#define internal_assert_task_valid(task) internal_assert(task < 5, ERROR_ID_INTERNAL_INVALID_ID)
#define assert_task_valid(task) api_assert(task < 5, ERROR_ID_INVALID_ID)
#define yield() rtos_yield()
#define interrupt_event_id_to_taskid(interrupt_event_id) ((RtosTaskId)(interrupt_event_id))
#define mutex_core_block_on(unused_task) rtos_signal_wait(RTOS_SIGNAL_ID__TASK_TIMER)
#define mutex_core_unblock(task) rtos_signal_send(task, RTOS_SIGNAL_ID__TASK_TIMER)
#define message_queue_core_block() rtos_signal_wait(RTOS_SIGNAL_ID__TASK_TIMER)
/* sleep() may return before the timeout occurs because another task may send the timeout signal to indicate that the
 * state of the message queue has changed.
 * Therefore, disable the timer whenever sleep() returns to make sure the timer is no longer active.
 * Note that in the current message-queue implementation, this is not necessary for correctness.
 * The message-queue implementation handles spurious timer signals gracefully.
 * However, disabling the timer avoids confusion and provides a minor benefit in run-time efficiency. */
#define message_queue_core_block_timeout(timeout)\
do\
{\
    rtos_sleep((timeout));\
    rtos_timer_disable(task_timers[get_current_task()]);\
}\
while (0)
#define message_queue_core_unblock(task) rtos_signal_send((task), RTOS_SIGNAL_ID__TASK_TIMER)
#define message_queue_core_is_unblocked(task) sched_runnable((task))


/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
 *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The concept of a context and of the context_t type is abstract and may have different implementations on
 *  different platforms.
 *  It can be, e.g., a stack pointer or a data structure for user-level task switching as on POSIX.
 *  This function is expected to set ctx to a value that the RTOS can pass to this platform's implementation of
 *  context_switch() and context_switch_first().
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 *  For hardware platforms, this typically requires the following set up steps:
 *  - The value of ctx points to either the beginning or the end of the stack area.
 *  - The stack area contains fn so that the context-switch functions can pop it off the stack as a return address to
 *    begin execution at.
 *  - Optionally reserve additional stack space if the context-switch functions depend on it.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void
context_init(context_t *const ctx, void (*const task_function)(void), uint8_t *const stack_base, const size_t stack_size)
{
    /* x86 uses a pre-decrement stack.
     * The initial stack pointer is set up to point to the high-address end of the task's stack area minus the size of
     * the initial context set up in this function.
     * The initial stack pointer is set so that, upon entering the task function, the stack pointer is aligned at a
     * 16-byte boundary.
     * This ensures compliance with x86 gcc calling conventions. */
    uint32_t stack_top_address = (uint32_t)stack_base + stack_size;
    *ctx = (context_t)((stack_top_address & 0xFFFFFFF0UL) - CONTEXT_SIZE);
    /* When the context-switch implementation switches the first time to a task, it will find the data on the stack
     * that is set up here.
     * First, it pops context->ebx,esi,edi, and ebp_stack_frame from the stack into the corresponding registers.
     * Second, it pops context->return_address from the stack into the instruction pointer register, effectively
     * causing a jump.
     * The return address is set up to be the task function of the respective task.
     * As per the x86 calling convention, the first action of that function is to push the value of the ebp register
     * onto the stack and move the stack pointer (from the esp register) into the ebp register.
     * Therefore, the value of context->ebp_stack_frame is never evaluated and is therefore irrelevant.
     * Also, the values for the ebx, esi, and edi fields in context are irrelevant for the first context switch. */
    (*ctx)->return_address = task_function;
}

static void
sched_set_runnable(const RtosTaskId task_id)
{
    SCHED_OBJ(task_id).runnable = true;
}

static void
sched_set_blocked(const RtosTaskId task_id)
{
    SCHED_OBJ(task_id).runnable = false;
}

static TaskIdOption
sched_get_next(void)
{
    TaskIdOption task;
    SchedIndex next = sched_get_cur_index();
    bool found = false;

    do
    {
        next = sched_next_index(next);
        found = sched_runnable(sched_index_to_taskid(next));
    } while (
        (!found)
        && (next != (sched_get_cur_index()))
        );

    if (found)
    {
        task = sched_index_to_taskid(next);
    }
    else
    {
        next = sched_max_index();
        task = TASK_ID_NONE;
    }

    sched_set_cur_index(next);

    return task;
}
static RtosSignalSet
signal_recv(RtosSignalSet *const pending_signals, const RtosSignalSet requested_signals)
{
    const RtosSignalSet received_signals = *pending_signals & requested_signals;

    precondition_preemption_disabled();

    *pending_signals &= ~received_signals;

    postcondition_preemption_disabled();

    return received_signals;
}

static void
signal_send_set(const RtosTaskId task_id, const RtosSignalSet signals)
{
    precondition_preemption_disabled();

    PENDING_SIGNALS(task_id) |= signals;
    unblock(task_id);

    postcondition_preemption_disabled();
}

static RtosSignalSet
signal_wait_set(const RtosSignalSet requested_signals) RTOS_REENTRANT
{
    RtosSignalSet received_signals;

    precondition_preemption_disabled();
    {
        RtosSignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());

        if (signal_peek(*pending_signals, requested_signals))
        {
            yield();
        }
        else
        {
            do
            {
                block();
            } while (!signal_peek(*pending_signals, requested_signals));
        }

        received_signals = signal_recv(pending_signals, requested_signals);
    }
    postcondition_preemption_disabled();

    return received_signals;
}
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
static void
timer_process_one(struct timer *const timer)
{
    precondition_preemption_disabled();

    if (timer_is_periodic(timer))
    {
        timer->expiry += timer->reload;
    }
    else
    {
        timer->enabled = false;
    }

    if (timer->error_id != ERROR_ID_NONE)
    {
        fatal(timer->error_id);
    }
    else
    {
        if (signal_pending(timer->task_id, timer->signal_set))
        {
            timer->overflow = true;
        }
        signal_send_set(timer->task_id, timer->signal_set);
    }

    postcondition_preemption_disabled();
}

static void
timer_enable(const RtosTimerId timer_id)
{
    precondition_preemption_disabled();

    if (timers[timer_id].reload == 0)
    {
        timer_process_one(&timers[timer_id]);
    }
    else
    {
        timers[timer_id].expiry = current_timeout() + timers[timer_id].reload;
        timers[timer_id].enabled = true;
    }

    postcondition_preemption_disabled();
}

static void
timer_oneshot(const RtosTimerId timer_id, const RtosTicksRelative timeout)
{
    precondition_preemption_disabled();

    timer_reload_set(timer_id, timeout);
    timer_enable(timer_id);
    timer_reload_set(timer_id, 0);

    postcondition_preemption_disabled();
}

static void
timer_tick_process(void)
{
    precondition_preemption_disabled();
    {
        const uint8_t pending_ticks = timer_pending_ticks_get_and_clear_atomically();

        if (pending_ticks > 1)
        {
            fatal(ERROR_ID_TICK_OVERFLOW);
        }

        if (pending_ticks != 0)
        {
            RtosTimerId timer_id;
            struct timer *timer;
            TicksTimeout timeout;

            rtos_timer_current_ticks++;

            timeout = current_timeout();

            for (timer_id = TIMER_ID_ZERO; timer_id <= TIMER_ID_MAX; timer_id++)
            {
                timer = TIMER_PTR(timer_id);
                if (timer_expired(timer, timeout))
                {
                    timer_process_one(timer);
                }
            }
        }
    }
    postcondition_preemption_disabled();
}
static void
interrupt_event_process(void)
{
    while (pending_interrupt_events != 0)
    {
        uint8_t current;
        for (current = 0; current < 5 && pending_interrupt_events != 0; current += 1)
        {
            uint32_t mask = 1 << current;
            if (pending_interrupt_events & mask)
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

                (void)sigprocmask(SIG_BLOCK, &set, NULL);
                pending_interrupt_events = pending_interrupt_events & (~mask);
                (void)sigprocmask(SIG_UNBLOCK, &set, NULL);

                interrupt_event_handle(current);
            }
        }
    }
}

static void
interrupt_event_wait(void)
{
    sigset_t set;
    int sig;

    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGVTALRM);
    sigaddset(&set, SIGPROF);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGRTMIN);
    sigaddset(&set, SIGRTMAX);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

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
/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
RtosTaskId
rtos_internal_interrupt_event_get_next(void)
{
    TaskIdOption next = TASK_ID_NONE;

    for (;;)
    {
        interrupt_event_process();
        timer_tick_process();
        next = sched_get_next();

        if (next == TASK_ID_NONE)
        {
            system_is_idle = true;
            interrupt_event_wait();
        }
        else
        {
            system_is_idle = false;
            break;
        }
    }

    internal_assert_task_valid(next);

    return next;
}
static void
interrupt_event_handle(const RtosInterruptEventId interrupt_event_id)
{
    precondition_preemption_disabled();

    internal_assert(interrupt_event_id < 5, ERROR_ID_INTERNAL_INVALID_ID);

#ifdef PREEMPTION_SUPPORT
    signal_send_set(interrupt_events[interrupt_event_id].task, interrupt_events[interrupt_event_id].sig_set);
#else
    rtos_signal_send_set(interrupt_events[interrupt_event_id].task,
            interrupt_events[interrupt_event_id].sig_set);
#endif

    postcondition_preemption_disabled();
}
static bool
mutex_try_lock(const RtosMutexId m)
{
    const bool r = mutexes[m].holder == TASK_ID_NONE;

    precondition_preemption_disabled();

    if (r)
    {
        mutexes[m].holder = get_current_task();
    }

    postcondition_preemption_disabled();

    return r;
}

static void
message_queue_init(void)
{
    RtosMessageQueueId message_queue = 1 - 1;
    RtosTaskId task;

    /* do not use for loop to work around buggy compiler optimization when there is only one message queue */
    do
    {
        struct message_queue *mq = &message_queues[message_queue];

        internal_assert(mq->messages != NULL &&
                        mq->message_size != 0 &&
                        mq->queue_length != 0 &&
                        mq->head == 0 &&
                        mq->available == 0, ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    } while (message_queue-- != 0);

    for (task = 0; task <= RTOS_TASK_ID_MAX; task += 1)
    {
        internal_assert(message_queue_waiters[task] == MESSAGE_QUEUE_ID_NONE,\
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    }
}

static void
message_queue_invariants_check(void)
{
    RtosMessageQueueId message_queue;
    RtosTaskId task;

    internal_assert(message_queues[0].messages == (uint8_t*)message_queue_one_messages,
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER);
    internal_assert(message_queues[0].message_size == 100,
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE);
    internal_assert(message_queues[0].queue_length == 10,
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH);

    for (message_queue = 0; message_queue < 1; message_queue += 1)
    {
        const struct message_queue *const mq = &message_queues[message_queue];

        internal_assert(mq->messages != NULL && mq->message_size != 0 && mq->queue_length != 0,
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION);
        internal_assert(mq->head < mq->queue_length, ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD);
        internal_assert(mq->available <= mq->queue_length,
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE);
    }

    for (task = 0; task <= RTOS_TASK_ID_MAX; task += 1)
    {
        message_queue = message_queue_waiters[task];

        internal_assert((message_queue < 1) || (message_queue == MESSAGE_QUEUE_ID_NONE),\
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS);

        if (message_queue != MESSAGE_QUEUE_ID_NONE)
        {
            const struct message_queue *const mq = &message_queues[message_queue];

            internal_assert((mq->available == 0) || (mq->available == mq->queue_length),\
                ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES);
            internal_assert(!message_queue_core_is_unblocked(task),\
                            ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED);
        }
    }

    /* The timer of the current task is expected to be disabled.
     * It is expected to be only enabled while the current task is blocked in message_queue_wait_timeout().
     * Unfortunately, we cannot make any assumptions about the relationship between the states of message queues and
     * other timers.
     * The timers of tasks depends not only on the message queue implementation but also on how other components use
     * those task timers. */
    internal_assert(!timers[task_timers[get_current_task()]].enabled,\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED);
}

static void
message_queue_waiters_wakeup(const RtosMessageQueueId message_queue)
{
    RtosTaskId task;

    message_queue_internal_assert_valid(message_queue);

    for (task = RTOS_TASK_ID_ZERO; task <= RTOS_TASK_ID_MAX; task += 1)
    {
        if (message_queue_waiters[task] == message_queue)
        {
            message_queue_core_unblock(task);
            message_queue_waiters[task] = MESSAGE_QUEUE_ID_NONE;
        }
    }
}

static void
message_queue_wait(const RtosMessageQueueId message_queue) RTOS_REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    message_queue_invariants_check();

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block();

    message_queue_invariants_check();
}

static void
message_queue_wait_timeout(const RtosMessageQueueId message_queue,
                           const RtosTicksRelative timeout) RTOS_REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    internal_assert(timeout != 0, ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT);
    message_queue_invariants_check();

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block_timeout(timeout);
    message_queue_waiters[get_current_task()] = MESSAGE_QUEUE_ID_NONE;

    message_queue_invariants_check();
}

/* assumptions: max length 255, no overlap of dst & src */
/* called memcopy instead of memcpy to not conflict with gcc's built-in memcpy declaration on unit test targets */
static void
memcopy(uint8_t *dst, const uint8_t *src, const uint8_t length)
{
    uint8_t *const dst_end = dst + length;

    api_assert((dst < src) || (dst >= (src + length)), ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP);

    while (dst < dst_end)
    {
        *dst++ = *src++;
    }
}


static RtosTaskId
get_current_task_check(void)
{
    internal_assert(current_task < 5, ERROR_ID_INTERNAL_CURRENT_TASK_INVALID);
    return current_task;
}
static void
yield_to(const RtosTaskId to) RTOS_REENTRANT
{
    const RtosTaskId from = get_current_task();

    internal_assert(to < 5, ERROR_ID_INTERNAL_INVALID_ID);

    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
block(void) RTOS_REENTRANT
{
    sched_set_blocked(get_current_task());
    rtos_yield();
}

static void
unblock(const RtosTaskId task)
{
    sched_set_runnable(task);
}

/* entry point trampolines */
static void
entry_one(void)
{
    
    task_one();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}

static void
entry_two(void)
{
    rtos_signal_wait(RTOS_SIGNAL_ID__RTOS_UTIL);
    task_two();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}

static void
entry_three(void)
{
    rtos_signal_wait(RTOS_SIGNAL_ID__RTOS_UTIL);
    task_three();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}

static void
entry_four(void)
{
    rtos_signal_wait(RTOS_SIGNAL_ID__RTOS_UTIL);
    task_four();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}

static void
entry_five(void)
{
    rtos_signal_wait(RTOS_SIGNAL_ID__RTOS_UTIL);
    task_five();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}






RtosSignalSet
rtos_signal_wait_set(const RtosSignalSet requested_signals) RTOS_REENTRANT
{
    RtosSignalSet received_signals;

    preempt_disable();

    received_signals = signal_wait_set(requested_signals);

    preempt_enable();

    return received_signals;
}

RtosSignalSet
rtos_signal_poll_set(const RtosSignalSet requested_signals)
{
    RtosSignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    RtosSignalSet received_signals;

    preempt_disable();

    received_signals = signal_recv(pending_signals, requested_signals);

    preempt_enable();

    return received_signals;
}

RtosSignalSet
rtos_signal_peek_set(const RtosSignalSet requested_signals)
{
    const RtosSignalSet pending_signals = PENDING_SIGNALS(get_current_task());
    return pending_signals & requested_signals;
}

void
rtos_signal_send_set(const RtosTaskId task_id, const RtosSignalSet signals)
{
    assert_task_valid(task_id);

    preempt_disable();

    signal_send_set(task_id, signals);

    preempt_enable();
}
void
rtos_timer_tick(void)
{
    /* This function is called from a POSIX signal handler.
     * Assume that it is always the same handler, that a handler cannot interrupt itself, and therefore that there is
     * no concurrency issue in the following code. */
    if (timer_ticks_pending < 2)
    {
        timer_ticks_pending += 1;
    }
}
void
rtos_sleep(const RtosTicksRelative ticks) RTOS_REENTRANT
{
    preempt_disable();

    timer_oneshot(task_timers[get_current_task()], ticks);
    signal_wait(RTOS_SIGNAL_ID__TASK_TIMER);

    preempt_enable();
}

void
rtos_timer_enable(const RtosTimerId timer_id)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timer_enable(timer_id);

    preempt_enable();
}

void
rtos_timer_disable(const RtosTimerId timer_id)
{
    assert_timer_valid(timer_id);

    timer_disable(timer_id);
}

void
rtos_timer_oneshot(const RtosTimerId timer_id, const RtosTicksRelative timeout)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timer_oneshot(timer_id, timeout);

    preempt_enable();
}

bool
rtos_timer_check_overflow(const RtosTimerId timer_id)
{
    bool r;

    assert_timer_valid(timer_id);

    preempt_disable();

    r = timers[timer_id].overflow;
    timers[timer_id].overflow = false;

    preempt_enable();

    return r;
}

RtosTicksRelative
rtos_timer_remaining(const RtosTimerId timer_id)
{
    RtosTicksRelative remaining;

    assert_timer_valid(timer_id);

    preempt_disable();

    remaining = timers[timer_id].enabled ? timers[timer_id].expiry - current_timeout() : 0;

    preempt_enable();

    return remaining;
}

/* Configuration functions */
void
rtos_timer_reload_set(const RtosTimerId timer_id, const RtosTicksRelative reload)
{
    assert_timer_valid(timer_id);

    timer_reload_set(timer_id, reload);
}

void
rtos_timer_signal_set(const RtosTimerId timer_id, const RtosTaskId task_id, const RtosSignalSet signal_set)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timers[timer_id].error_id = ERROR_ID_NONE;
    timers[timer_id].task_id = task_id;
    timers[timer_id].signal_set = signal_set;

    preempt_enable();
}

void
rtos_timer_error_set(const RtosTimerId timer_id, const RtosErrorId error_id)
{
    assert_timer_valid(timer_id);

    timers[timer_id].error_id = error_id;
}
void
rtos_interrupt_event_raise(const RtosInterruptEventId interrupt_event_id)
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

    /* POSIX signal handlers can interrupt each other.
     * Therefore, manipulate 'pending_interrupt_events' atomically. */
    (void)sigprocmask(SIG_BLOCK, &set, NULL);
    pending_interrupt_events = pending_interrupt_events | (1 << interrupt_event_id);
    (void)sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void
rtos_interrupt_event_task_set(const RtosInterruptEventId interrupt_event_id, const RtosTaskId task_id)
{
    api_assert(interrupt_event_id < 5, ERROR_ID_INVALID_ID);
    api_assert(task_id < 5, ERROR_ID_INVALID_ID);
    interrupt_events[interrupt_event_id].task = task_id;
}

void
rtos_mutex_lock(const RtosMutexId m) RTOS_REENTRANT
{
    assert_mutex_valid(m);
    api_assert(mutexes[m].holder != get_current_task(), ERROR_ID_DEADLOCK);

    preempt_disable();

    while (!mutex_try_lock(m))
    {
        mutex_waiters[get_current_task()] = m;
        mutex_core_block_on(mutexes[m].holder);
    }

    preempt_enable();

}


void
rtos_mutex_unlock(const RtosMutexId m)
{
    RtosTaskId t;

    assert_mutex_valid(m);
    api_assert(mutexes[m].holder == get_current_task(), ERROR_ID_NOT_HOLDING_MUTEX);

    preempt_disable();


    for (t = RTOS_TASK_ID_ZERO; t < TASK_ID_END; t++)
    {
        if (mutex_waiters[t] == m)
        {
            mutex_waiters[t] = MUTEX_ID_NONE;
            mutex_core_unblock(t);
        }
    }

    mutexes[m].holder = TASK_ID_NONE;

    preempt_enable();
}

bool
rtos_mutex_try_lock(const RtosMutexId m)
{
    bool r;

    assert_mutex_valid(m);

    preempt_disable();

    r = mutex_try_lock(m);

    preempt_enable();

    return r;
}

/* A macro implementation would be preferable to eliminate function call overhead when compilers don't support implicit
 * inlining, but at present this would involve exposing too many implementation internals in the public API header. */
bool
rtos_mutex_holder_is_current(const RtosMutexId m)
{
    assert_mutex_valid(m);
    return mutexes[m].holder == get_current_task();
}

void
rtos_message_queue_put(const RtosMessageQueueId message_queue, const void *const message)
        RTOS_REENTRANT
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);

    while (!rtos_message_queue_try_put(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
rtos_message_queue_try_put(const RtosMessageQueueId message_queue, const void *const message)
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    message_queue_invariants_check();

    {
        struct message_queue *const mq = &message_queues[message_queue];

        if (mq->available == mq->queue_length)
        {
            return false;
        }
        else
        {
            const uint8_t buffer_index = (mq->head + mq->available) % mq->queue_length;
            const uint16_t buffer_offset = buffer_index * mq->message_size;
            memcopy(&mq->messages[buffer_offset], message, mq->message_size);
            mq->available += 1;

            if (mq->available == 1)
            {
                message_queue_waiters_wakeup(message_queue);
            }

            message_queue_invariants_check();
            return true;
        }
    }
}

bool
rtos_message_queue_put_timeout(const RtosMessageQueueId message_queue, const void *const message,
                                         const RtosTicksRelative timeout) RTOS_REENTRANT
{
    const RtosTicksAbsolute absolute_timeout = rtos_timer_current_ticks + timeout;

    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    api_assert(timeout != 0, ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT);
    internal_assert(rtos_timer_current_ticks < (UINT32_MAX - timeout),\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);
    message_queue_invariants_check();

    while ((message_queues[message_queue].available == message_queues[message_queue].queue_length) &&
            (absolute_timeout > rtos_timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - rtos_timer_current_ticks);
    }

    return rtos_message_queue_try_put(message_queue, message);
}

void
rtos_message_queue_get(const RtosMessageQueueId message_queue, void *const message)
        RTOS_REENTRANT
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);

    while (!rtos_message_queue_try_get(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
rtos_message_queue_try_get(const RtosMessageQueueId message_queue, void *const message)
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    message_queue_invariants_check();

    {
        struct message_queue *const mq = &message_queues[message_queue];

        if (mq->available == 0)
        {
            return false;
        }
        else
        {
            const uint16_t buffer_offset = mq->head * mq->message_size;
            memcopy((uint8_t *const)message, &mq->messages[buffer_offset], mq->message_size);
            mq->head = (mq->head + 1) % mq->queue_length;
            mq->available -= 1;

            if (mq->available == (1 - 1))
            {
                message_queue_waiters_wakeup(message_queue);
            }

            message_queue_invariants_check();
            return true;
        }
    }
}

bool
rtos_message_queue_get_timeout(const RtosMessageQueueId message_queue, void *const message,
                                         const RtosTicksRelative timeout) RTOS_REENTRANT
{
    const RtosTicksAbsolute absolute_timeout = rtos_timer_current_ticks + timeout;

    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    api_assert(timeout != 0, ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT);
    internal_assert(rtos_timer_current_ticks < (UINT32_MAX - timeout),\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);
    message_queue_invariants_check();

    while ((message_queues[message_queue].available == 0) &&
            (absolute_timeout > rtos_timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - rtos_timer_current_ticks);
    }

    return rtos_message_queue_try_get(message_queue, message);
}


RtosTaskId
rtos_task_current(void)
{
    return get_current_task();
}
void
rtos_task_start(const RtosTaskId task)
{
    assert_task_valid(task);
    rtos_signal_send(task, RTOS_SIGNAL_ID__RTOS_UTIL);
}

void
rtos_yield(void) RTOS_REENTRANT
{
    RtosTaskId to = interrupt_event_get_next();
    yield_to(to);
}

void
rtos_start(void)
{
    /* stack_init() must be called at the top of the start function so that it can declare variables */
    stack_init();

    message_queue_init();

    context_init(get_task_context(0), entry_one, stack_0, 8192);
    sched_set_runnable(0);
    context_init(get_task_context(1), entry_two, stack_1, 8192);
    sched_set_runnable(1);
    context_init(get_task_context(2), entry_three, stack_2, 8192);
    sched_set_runnable(2);
    context_init(get_task_context(3), entry_four, stack_3, 8192);
    sched_set_runnable(3);
    context_init(get_task_context(4), entry_five, stack_4, 8192);
    sched_set_runnable(4);

    context_switch_first(get_task_context(RTOS_TASK_ID_ZERO));
}
