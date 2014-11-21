/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT

/* To support preemption of tasks by exception handlers, we define a context stack frame that includes the exception
 * frame automatically stored onto the stack by the CPU when it takes an exception.
 *
 * The ARM stack grows downwards, from high to low addresses.
 * Note that the ARM ABI's Procedure Call Standard says that at a public interface, the stack must be double-word
 * (8-byte) aligned.
 *
 * The lower-address half of the context stack frame is for register values and task state that the RTOS must manually
 * push onto the stack before it switches context. */

#define CONTEXT_PREEMPT_DISABLED 0
#define CONTEXT_R4_IDX 1
#define CONTEXT_R5_IDX 2
#define CONTEXT_R6_IDX 3
#define CONTEXT_R7_IDX 4
#define CONTEXT_R8_IDX 5
#define CONTEXT_R9_IDX 6
#define CONTEXT_R10_IDX 7
#define CONTEXT_R11_IDX 8
#define CONTEXT_EXCEPTION_RETURN_IDX 9

/* The higher-address half of the context stack frame is the exception frame automatically pushed by the CPU. */

#define CONTEXT_R0_IDX 10
#define CONTEXT_R1_IDX 11
#define CONTEXT_R2_IDX 12
#define CONTEXT_R3_IDX 13
#define CONTEXT_IP_IDX 14
#define CONTEXT_LR_IDX 15
#define CONTEXT_PC_IDX 16
#define CONTEXT_PSR_IDX 17

#define CONTEXT_SIZE 18

/* Definitions for ARM-specific initialization.
 *
 * We define the SVCALL priority to be a higher level (i.e. lower numerical value) than PENDSV so that when we disable
 * preemption by setting BASEPRI to PENDSV's priority, PENDSV is disabled but SVCALL remains enabled.
 * This reflects our desire for the RTOS to be able to manually yield (via svc) when preemption is disabled.
 *
 * Some platforms do not implement the low 4 bits of priority.
 * Thus, we choose priority values that are distinct regardless of the lower 4 bits. */

#define SVCALL_PRIORITY 224u
#define PENDSV_PRIORITY 240u

/* System Handler Priority Registers (SHPR) */
#define SHPR2_PHYSADDR 0xE000ED1C
#define SHPR3_PHYSADDR 0xE000ED20

#define SHPR2_SVCALL_PRIO_MASK 0x00ffffff
#define SHPR2_SVCALL_PRIO_OFFSET 24

#define SHPR3_PENDSV_PRIO_MASK 0xff00ffff
#define SHPR3_PENDSV_PRIO_OFFSET 16

/* Execution Program Status Register (EPSR)
 * T-bit is the Thumb state bit, which must be 1 for the Cortex-M4 because it only supports Thumb instructions. */
#define EPSR_THUMB_BIT_OFFSET 24

/* For EXC_RETURN of the first task, we choose this value, which means "Return to Thread mode, exception return uses
 * non-floating-point state from MSP and execution uses MSP after return" according to the Cortex-M4 manual. */
#define EXC_RETURN_INITIAL_TASK 0xfffffff9

/*| type_definitions |*/
typedef uint32_t* context_t;

/*| structure_definitions |*/

/*| extern_definitions |*/
extern void rtos_internal_context_switch_first(context_t *);
extern void rtos_internal_task_entry_trampoline(void);
extern bool rtos_internal_check_preempt_disabled(void);
extern void rtos_internal_yield(void);
extern void rtos_internal_preempt_enable(void);
extern void rtos_internal_preempt_disable(void);
extern void rtos_internal_preempt_pend(void);

/*| function_definitions |*/
/**
 * Platform-specific initialization for the preemption implementation called once at RTOS start time.
 */
static void preempt_init(void);

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

/*| function_like_macros |*/
#define context_switch_first(to) rtos_internal_context_switch_first(get_task_context(to))
#define yield() rtos_internal_yield()
#define preempt_disable() rtos_internal_preempt_disable()
#define preempt_enable() rtos_internal_preempt_enable()
#define preempt_pend() rtos_internal_preempt_pend()
#define precondition_preemption_disabled() internal_assert(rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_preemption_disabled() internal_assert(rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_preemption_enabled() internal_assert(!rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)

/*| functions |*/

/* ARM specific initialization */
static void
preempt_init(void)
{
    /* Set the priority of handlers */
    volatile uint32_t *shpr2 = (uint32_t *) SHPR2_PHYSADDR;
    volatile uint32_t *shpr3 = (uint32_t *) SHPR3_PHYSADDR;
    *shpr2 = (SVCALL_PRIORITY << SHPR2_SVCALL_PRIO_OFFSET) | (SHPR2_SVCALL_PRIO_MASK & *shpr2);
    *shpr3 = (PENDSV_PRIORITY << SHPR3_PENDSV_PRIO_OFFSET) | (SHPR3_PENDSV_PRIO_MASK & *shpr3);

    preempt_disable();
}

static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *const context = stack_base + stack_size - CONTEXT_SIZE;

    /* Start all tasks with preemption disabled by setting this field to a non-zero value. */
    context[CONTEXT_PREEMPT_DISABLED] = true;
    /* Set R4 to the task entry point, for the task entry trampoline to bounce to. */
    context[CONTEXT_R4_IDX] = (uint32_t) fn;
    context[CONTEXT_PC_IDX] = (uint32_t) rtos_internal_task_entry_trampoline;
    context[CONTEXT_PSR_IDX] = (uint32_t) (1u << EPSR_THUMB_BIT_OFFSET); /* make sure the T-bit is set! */
    context[CONTEXT_EXCEPTION_RETURN_IDX] = (uint32_t) EXC_RETURN_INITIAL_TASK;

    *ctx = context;
}

/*| public_functions |*/
