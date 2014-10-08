/*| headers |*/
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/
#define PREEMPT_RESTORE_DISABLED 1
#define PREEMPT_RESTORE_VOLATILES 2

/*
 * The unified stack frame structure used here both to preserve interrupted contexts and to implement context switch
 * on RTOS variants that support task preemption by interrupts is defined in ppce500-manual under
 * 'ppce500/vectable-preempt'.
 * All magic numbers used in this file should match the stack frame structure documented in ppce500-manual.
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

/*| type_definitions |*/
typedef uint32_t* context_t;

/*| structure_definitions |*/

/*| extern_definitions |*/
extern void rtos_internal_yield_syscall({{prefix_type}}TaskId to, bool return_with_preempt_disabled);
extern void rtos_internal_restore_preempted_context(bool restore_volatiles, context_t ctxt_to);
extern bool rtos_internal_check_interrupts_enabled(void);

/*| function_definitions |*/
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
static void context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size);

static void ppce500_context_preempt_first({{prefix_type}}TaskId to);
static void ppce500_yield(void);
static void preempt_enable(void);

/*| state |*/
static volatile bool preempt_disabled = true;
static volatile bool preempt_pending;

/*| function_like_macros |*/
#define _yield ppce500_yield
#define preempt_disable() preempt_disabled = true
#define preempt_pend() preempt_pending = true
#define preempt_clear() preempt_pending = false
#define context_preempt_first ppce500_context_preempt_first
#define irqs_enabled() rtos_internal_check_interrupts_enabled()
#define precondition_interrupts_disabled() internal_assert(!irqs_enabled(), ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define precondition_interrupts_enabled() internal_assert(irqs_enabled(), ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_interrupts_disabled() internal_assert(!irqs_enabled(), ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_interrupts_enabled() internal_assert(irqs_enabled(), ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define precondition_preemption_disabled() internal_assert(preempt_disabled, ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_preemption_disabled() internal_assert(preempt_disabled, ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_preemption_enabled() internal_assert(!preempt_disabled, ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)

/*| functions |*/
static void
ppce500_yield_common(bool return_with_preempt_disabled)
{
    precondition_interrupts_enabled();
    precondition_preemption_disabled();

    {
        const {{prefix_type}}TaskId from = get_current_task();
        {{prefix_type}}TaskId to;

        while (true) {
            /* this unsets preempt_pending */
            to = interrupt_event_get_next();

            interrupts_disable();

            if (preempt_pending) {
                interrupts_enable();
                continue;
            }

            if (from != to) {
                /* This enables interrupts */
                rtos_internal_yield_syscall(to, return_with_preempt_disabled);
            } else {
                preempt_disabled = return_with_preempt_disabled;
                interrupts_enable();
            }

            break;
        }
    }

    if (return_with_preempt_disabled) {
        postcondition_preemption_disabled();
    } else {
        postcondition_preemption_enabled();
    }
    postcondition_interrupts_enabled();
}

static void
ppce500_yield(void)
{
    precondition_preemption_disabled();

    ppce500_yield_common(true);

    postcondition_preemption_disabled();
}

static void
preempt_enable(void)
{
    precondition_interrupts_enabled();
    precondition_preemption_disabled();

    /* Avoid invoking the scheduler if there's no preemption pending */
    interrupts_disable();
    if (preempt_pending) {
        interrupts_enable();
        ppce500_yield_common(false);
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

    precondition_interrupts_disabled();
    precondition_preemption_disabled();

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

/* This function returns the (context_t *) to switch to, or TASK_ID_NONE if no context switch is required.
 * It is NOT responsible for setting current_task = to! */
{{prefix_type}}TaskId
preempt_irq_handler_wrapper(bool (*handler)(void))
{
    bool initial_preempt_disabled = preempt_disabled;
    {{prefix_type}}TaskId to = TASK_ID_NONE;

    precondition_interrupts_disabled();
    /* Note: preemption can be either enabled or disabled */

    if ((*handler)() == false) {
        goto end;
    }

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
    if (to == TASK_ID_NONE) {
        internal_assert(initial_preempt_disabled == preempt_disabled, ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED);
    } else {
        postcondition_preemption_disabled();
    }
    postcondition_interrupts_disabled();

    return to;
}

void
ppce500_context_preempt({{prefix_type}}TaskId to, context_t sp, bool restore_preempt_disabled, bool restore_volatiles)
{
    precondition_interrupts_disabled();
    precondition_preemption_disabled();
    {
        context_t *from = get_task_context(get_current_task());
        uint32_t preempt_status = 0;

        if (restore_preempt_disabled) {
            preempt_status |= PREEMPT_RESTORE_DISABLED;
        }

        if (restore_volatiles) {
            preempt_status |= PREEMPT_RESTORE_VOLATILES;
        }

        sp[CONTEXT_PREEMPT_RESTORE_STATUS] = preempt_status;

        /* Commit the constructed stack frame of the outgoing task */
        *from = sp;
    }

    ppce500_context_preempt_first(to);
}

void
ppce500_context_preempt_first({{prefix_type}}TaskId to)
{
    bool restore_volatiles;

    precondition_interrupts_disabled();
    precondition_preemption_disabled();
    {
        context_t ctxt_to = *(get_task_context(to));

        /* Set the current task id to the incoming one */
        current_task = to;

        if (!(ctxt_to[CONTEXT_PREEMPT_RESTORE_STATUS] & PREEMPT_RESTORE_DISABLED)) {
            /* only need the enable case because we entered with preemption disabled */
            preempt_disabled = false;
        }

        if (ctxt_to[CONTEXT_PREEMPT_RESTORE_STATUS] & PREEMPT_RESTORE_VOLATILES) {
            restore_volatiles = true;
        } else {
            restore_volatiles = false;
        }

        /* This never returns */
        rtos_internal_restore_preempted_context(restore_volatiles, ctxt_to);
    }
}

static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *init_context, *context;

    init_context = stack_base + stack_size - CONTEXT_HEADER_SIZE;
    /**
     * Set up an initial stack frame header containing just the back chain word and the LR save word.
     * The EABI specification requires that the back chain be NULL-terminated.
     */
    init_context[CONTEXT_BC_IDX] = 0;

    /**
     * Immediately below the dummy stack frame header, create a full-size stack frame containing register values for
     * the initial context.
     * The EABI spec requires that the back chain word always points to the previous frame's back chain word field.
     */
    context = init_context - CONTEXT_FRAME_SIZE;
    /* Set SRR0 to the task entry point */
    context[CONTEXT_SRR0_IDX] = (uint32_t) fn;
    /* Set MSR[EE] = 1 (interrupts enabled) initially for all tasks */
    context[CONTEXT_SRR1_IDX] = 0x8000;
    context[CONTEXT_PREEMPT_RESTORE_STATUS] = PREEMPT_RESTORE_DISABLED | PREEMPT_RESTORE_VOLATILES;
    context[CONTEXT_BC_IDX] = (uint32_t) &init_context[CONTEXT_BC_IDX];
    *ctx = context;
}

/*| public_functions |*/
