/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT
#define PREEMPT_RESTORE_DISABLED 1
#define PREEMPT_RESTORE_VOLATILES 2

/* The 'external interrupts enable' (EE) bit of the PowerPC e500 machine state register (MSR) */
#define PPCE500_MSR_EE_SET 0x8000

/*
 * The unified stack frame structure used here both to preserve interrupted contexts and to implement context switch
 * on RTOS variants that support task preemption by interrupts is defined in vectable.s, and all magic numbers used in
 * this file should match the stack frame structure documented there.
 */
#define CONTEXT_BC_IDX 0
#define CONTEXT_LR_IDX 1
#define CONTEXT_HEADER_SIZE (CONTEXT_LR_IDX + 1)
#define CONTEXT_INTERRUPTED_LR_IDX 2
#define CONTEXT_SRR0_IDX 3
#define CONTEXT_SRR1_IDX 4
#define CONTEXT_XER_IDX 5
#define CONTEXT_CTR_IDX 6
#define CONTEXT_CR_IDX 7
#define CONTEXT_GPR0_IDX 8
#define CONTEXT_GPR3_IDX 9
#define CONTEXT_GPR4_IDX 10
#define CONTEXT_GPR5_IDX 11
#define CONTEXT_GPR6_IDX 12
#define CONTEXT_GPR7_IDX 13
#define CONTEXT_GPR8_IDX 14
#define CONTEXT_GPR9_IDX 15
#define CONTEXT_GPR10_IDX 16
#define CONTEXT_GPR11_IDX 17
#define CONTEXT_GPR12_IDX 18
#define CONTEXT_GPR14_IDX 19
#define CONTEXT_GPR15_IDX 20
#define CONTEXT_GPR16_IDX 21
#define CONTEXT_GPR17_IDX 22
#define CONTEXT_GPR18_IDX 23
#define CONTEXT_GPR19_IDX 24
#define CONTEXT_GPR20_IDX 25
#define CONTEXT_GPR21_IDX 26
#define CONTEXT_GPR22_IDX 27
#define CONTEXT_GPR23_IDX 28
#define CONTEXT_GPR24_IDX 29
#define CONTEXT_GPR25_IDX 30
#define CONTEXT_GPR26_IDX 31
#define CONTEXT_GPR27_IDX 32
#define CONTEXT_GPR28_IDX 33
#define CONTEXT_GPR29_IDX 34
#define CONTEXT_GPR30_IDX 35
#define CONTEXT_GPR31_IDX 36
#define CONTEXT_PREEMPT_RESTORE_STATUS 37
#define CONTEXT_FRAME_SIZE (CONTEXT_PREEMPT_RESTORE_STATUS + 1)

/*| types |*/
typedef uint32_t* context_t;

/*| structures |*/

/*| extern_declarations |*/
extern void rtos_internal_yield_syscall({{prefix_type}}TaskId to, bool return_with_preempt_disabled);
extern void rtos_internal_restore_preempted_context(bool restore_volatiles, context_t ctxt_to);

/*| function_declarations |*/
/**
 * Common helper for context-switching due to an internal yield or preempt_enable.
 *
 * @param return_with_preempt_disabled A bool indicating whether this function should return with preemption disabled.
 */
static void yield_common(bool return_with_preempt_disabled);

/**
 * Trigger a context-switch to the next task runnable as determined by the scheduler.
 * Intended to be used by the RTOS internally after taking actions that change the set of schedulable tasks.
 */
static void yield(void);

/**
 * Enable preemption, and in doing so, cause any pending preemption to happen immediately.
 */
static void preempt_enable(void);

/**
 * Invoke the scheduler repeatedly until no preemptions are pending, to determine which task should currently be
 * running.
 * Each scheduler invocation occurs with interrupts enabled, during which time new preemptions can become pending.
 *
 * @return The identifier of the task that should currently be running.
 */
static {{prefix_type}}TaskId preempt_irq_invoke_scheduler(void);

/**
 * Modify preemption state and invoke the scheduler to determine if preemption is necessary.
 *
 * @return The identifier of the task to switch to, or TASK_ID_NONE if no context switch is necessary.
 */
{{prefix_type}}TaskId rtos_internal_preempt_irq_scheduler_wrapper(void);

/**
 * Context switch from one task to another.
 * Does not return.
 *
 * @param to The identifier of the task to switch to.
 * @param ctxt The partially constructed context of the task to be switched away from.
 * @param restore_preempt_disabled A boolean indicating whether the task we are switching away from should be restored
 *  with preemption disabled.
 * @param restore_volatiles A boolean indicating whether the task we are switching away from needs to have its
 *  volatile registers restored from the stored context.
 */
void rtos_internal_context_switch({{prefix_type}}TaskId to, context_t ctxt, bool restore_preempt_disabled,
        bool restore_volatiles);

/**
 * Initial context switch to a task.
 * Does not return.
 *
 * @param to The identifier of the task to switch to.
 */
static void context_switch_first({{prefix_type}}TaskId to);

/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
 *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void context_init(context_t *ctx, void (*fn)(void), uint32_t *stack_base, size_t stack_size);

/*| state |*/
static volatile bool preempt_disabled = true;
static volatile bool preempt_pending;

/*| function_like_macros |*/
#define preempt_init()
#define preempt_disable() preempt_disabled = true
#define preempt_pend() preempt_pending = true
#define preempt_clear() preempt_pending = false
#define precondition_preemption_disabled() internal_assert(preempt_disabled, ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_preemption_disabled() internal_assert(preempt_disabled, ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_preemption_enabled() internal_assert(!preempt_disabled, ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_conditional_preempt_disabled(cond, target) \
        if (cond) { \
            internal_assert(preempt_disabled == target, ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED); \
        } else { \
            postcondition_preemption_disabled(); \
        }

/*| functions |*/
static void
yield_common(const bool return_with_preempt_disabled)
{
    precondition_interrupts_enabled();
    precondition_preemption_disabled();

    {
        const {{prefix_type}}TaskId from = get_current_task();
        /* The scheduler invocation returns with interrupts disabled.
         * This is to ensure that no interrupts come in and change the scheduler state between this call and the
         * context switch, or at least, the reenabling of preemption. */
        const {{prefix_type}}TaskId to = preempt_irq_invoke_scheduler();

        if (from != to) {
            rtos_internal_yield_syscall(to, return_with_preempt_disabled);
        } else {
            preempt_disabled = return_with_preempt_disabled;
        }

        interrupts_enable();
    }

    postcondition_conditional_preempt_disabled(true, return_with_preempt_disabled);
    postcondition_interrupts_enabled();
}

static void
yield(void)
{
    precondition_preemption_disabled();

    yield_common(true);

    postcondition_preemption_disabled();
}

/* Enabling preemption means checking immediately if a preemption needs to occur, because simply continuing to run the
 * current task would violate the scheduler requirement if a higher-priority task is meant to preempt it. */
static void
preempt_enable(void)
{
    precondition_interrupts_enabled();
    precondition_preemption_disabled();

    /* Avoid invoking the scheduler if there's no preemption pending */
    interrupts_disable();
    if (preempt_pending) {
        interrupts_enable();
        yield_common(false);
    } else {
        preempt_disabled = false;
        interrupts_enable();
    }

    postcondition_preemption_enabled();
    postcondition_interrupts_enabled();
}

static {{prefix_type}}TaskId
preempt_irq_invoke_scheduler(void)
{
    {{prefix_type}}TaskId next;

    /* Precondition: interrupts are allowed to be either enabled or disabled */
    precondition_preemption_disabled();

    /* While interrupts are enabled in the following do-loop, another interrupt may set preempt_pending back to true,
     * but the precondition that preemption is disabled ensures we don't get runaway interrupt recursion. */
    do {
        interrupts_enable();

        /* this unsets preempt_pending */
        next = interrupt_event_get_next();

        interrupts_disable();
    } while (preempt_pending);

    postcondition_preemption_disabled();
    postcondition_interrupts_disabled();

    return next;
}

#if ({{taskid_size}} != 8)
#error "Assembly implementation assumes a taskid_size of 8. See vectable.s"
#endif
/* This function, called directly from the common assembly code for preemption-supporting interrupts (see vectable.s),
 * is responsible for modifying preemption state, and invoking the scheduler to determine if preemption is necessary.
 * It returns the TaskId of the task to switch to, or TASK_ID_NONE if no context switch is required.
 * It is NOT responsible for setting current_task = to!
 * Note also that although we impose a pre and postcondition that interrupts are disabled, the call to
 * preempt_irq_invoke_scheduler will enable interrupts one or more times during its execution. */
{{prefix_type}}TaskId
rtos_internal_preempt_irq_scheduler_wrapper(void)
{
    {{prefix_type}}TaskId to = TASK_ID_NONE;

    precondition_interrupts_disabled();
    {
        /* Precondition: preemption is allowed to be either enabled or disabled */
        const bool initial_preempt_disabled = preempt_disabled;

        preempt_pending = true;

        if (preempt_disabled) {
            goto end;
        }

        preempt_disabled = true;

        to = preempt_irq_invoke_scheduler();
        if (to == get_current_task()) {
            to = TASK_ID_NONE;
            preempt_disabled = initial_preempt_disabled;
        }
end:
        postcondition_conditional_preempt_disabled(to == TASK_ID_NONE, initial_preempt_disabled);
    }
    postcondition_interrupts_disabled();

    return to;
}

/* This context switch routine is invoked directly from the assembly wrappers for preemption-supporting interrupt
 * vector code (see vectable.s).
 * At the end it jumps directly to context_switch_first which implements its 2nd half, and never returns. */
void
rtos_internal_context_switch(const {{prefix_type}}TaskId to, const context_t ctxt, const bool restore_preempt_disabled,
        const bool restore_volatiles)
{
    precondition_interrupts_disabled();
    precondition_preemption_disabled();
    {
        context_t *const ctxt_from = get_task_context(get_current_task());
        uint32_t preempt_status = 0;

        if (restore_preempt_disabled) {
            preempt_status |= PREEMPT_RESTORE_DISABLED;
        }

        if (restore_volatiles) {
            preempt_status |= PREEMPT_RESTORE_VOLATILES;
        }

        ctxt[CONTEXT_PREEMPT_RESTORE_STATUS] = preempt_status;

        /* Commit the constructed stack frame of the outgoing task */
        *ctxt_from = ctxt;
    }

    /* This never returns */
    context_switch_first(to);
}

/* This is used by the RTOS start function to start the very first task, but is also identical with the latter half of
 * the context switch process.
 * At the end it jumps directly to task context restoration assembly routines (see vectable.s), and never returns. */
static void
context_switch_first(const {{prefix_type}}TaskId to)
{
    precondition_interrupts_disabled();
    precondition_preemption_disabled();
    {
        const context_t ctxt_to = *(get_task_context(to));
        bool restore_volatiles;

        /* Set the current task id to the incoming one */
        current_task = to;

        if (!(ctxt_to[CONTEXT_PREEMPT_RESTORE_STATUS] & PREEMPT_RESTORE_DISABLED)) {
            /* only need the enable case because we entered with preemption disabled */
            preempt_disabled = false;
        }

        restore_volatiles = (ctxt_to[CONTEXT_PREEMPT_RESTORE_STATUS] & PREEMPT_RESTORE_VOLATILES);

        /* This never returns */
        rtos_internal_restore_preempted_context(restore_volatiles, ctxt_to);
    }
}

static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *const init_context = stack_base + stack_size - CONTEXT_HEADER_SIZE;
    uint32_t *const context = init_context - CONTEXT_FRAME_SIZE;
    uint32_t current_msr;

    /**
     * Set up an initial stack frame header containing just the back chain word and the LR save word.
     * The EABI specification requires that the back chain be NULL-terminated.
     */
    init_context[CONTEXT_BC_IDX] = 0;

    /**
     * Immediately below the initial stack frame header, create a full-size stack frame containing register values for
     * the initial context.
     * The EABI spec requires that the back chain word always points to the previous frame's back chain word field.
     */
    /* Set SRR0 to the task entry point */
    context[CONTEXT_SRR0_IDX] = (uint32_t) fn;
    /* Set MSR[EE] = 1 (interrupts enabled) initially for all tasks */
    asm volatile("mfmsr %0":"=r"(current_msr)::);
    context[CONTEXT_SRR1_IDX] = current_msr | PPCE500_MSR_EE_SET;
    context[CONTEXT_PREEMPT_RESTORE_STATUS] = PREEMPT_RESTORE_DISABLED | PREEMPT_RESTORE_VOLATILES;
    context[CONTEXT_BC_IDX] = (uint32_t) &init_context[CONTEXT_BC_IDX];
    *ctx = context;
}

/*| public_functions |*/
