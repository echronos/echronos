.syntax unified
.section .text

/* The Base Priority Mask Register (BASEPRI) is used to define the minimum priority for exception processing.
 * When set to a nonzero value, it prevents the activation of all exceptions with the same or lower priority level.
 * Note that lower priority levels are expressed by *higher* priority values.
 *
 * Setting BASEPRI to 0 enables all configurable-priority exceptions. */
.macro asm_preempt_enable scratch
        ldr \scratch, =#0
        msr basepri, \scratch
.endm

/* Setting BASEPRI to the same priority value as that of a configurable-priority interrupt disables the interrupt. */
.macro asm_preempt_disable scratch
        /* This hardcoded value must match the PendSV priority set by the ARM-specific preempt_init. */
        ldr \scratch, =#240
        msr basepri, \scratch
.endm

/* This macro is used by the preemption handler to clear the 'preemption pending' status. */
.macro asm_preempt_clear scratch0 scratch1
        /* Clear the PendSV bit in the ICSR (Interrupt Control and State Register) */
        ldr \scratch0, =0xE000ED04
        ldr \scratch1, =0x08000000
        str \scratch1, [\scratch0]
.endm

/* This macro invokes the scheduler in order to determine the TaskId of the next task to be switched to.
 * If that task is the same task as the current one, the macro will jump to the 'return_label' location.
 * The TaskId of the task to be switched to ends up in r0.
 *
 * Side-effects of this macro include that the pointer to the 'rtos_internal_current_task' variable is placed into the
 * 'current_task_p' register, and its value is placed into the 'old_task' register.
 * This may be helpful in order to avoid having to reload those values later. */
.macro asm_invoke_scheduler current_task_p old_task return_label
        /* new_task<r0> = interrupt_event_get_next() */
        push {ip, lr}
        bl rtos_internal_interrupt_event_get_next
        pop {ip, lr}

        /* old_task = rtos_internal_current_task */
        ldr \current_task_p, =rtos_internal_current_task
        ldrb \old_task, [\current_task_p]

        /* if (old_task == new_task<r0>) return */
        cmp \old_task, r0
        beq \return_label
.endm

/* This macro switches context from the TaskId given in register 'old_task_idx' to the one given in 'new_task_idx'.
 * It also expects to be given the pointer to the 'rtos_internal_current_task' variable in the 'current_task_p'
 * register, and will corrupt the given 'scratch' register. */
.macro asm_context_switch current_task_p scratch old_task_idx new_task_idx
        ldr \scratch, =rtos_internal_tasks
        /* The 2-bit logical left-shift here assumes each element of rtos_internal_tasks[] is a 32-bit pointer.
         * If the structure of 'struct task' is ever changed, this assumption will break. */
        str.w sp, [\scratch, \old_task_idx, lsl #2] /* tasks[old_task_idx] = current_ctx<sp> */
        ldr.w sp, [\scratch, \new_task_idx, lsl #2] /* new_ctx<sp> = tasks[new_task_idx] */
        strb \new_task_idx, [\current_task_p] /* current_task = new_task_idx */
.endm

/**
 * Enable preemption, and in doing so, cause any pending preemption to happen immediately.
 */
.global rtos_internal_preempt_enable
.type rtos_internal_preempt_enable,#function
/* void rtos_internal_preempt_enable(void); */
rtos_internal_preempt_enable:
        asm_preempt_enable r0
        bx lr
.size rtos_internal_preempt_enable, .-rtos_internal_preempt_enable

/**
 * Disable preemption.
 */
.global rtos_internal_preempt_disable
.type rtos_internal_preempt_disable,#function
/* void rtos_internal_preempt_disable(void); */
rtos_internal_preempt_disable:
        asm_preempt_disable r0
        bx lr
.size rtos_internal_preempt_disable, .-rtos_internal_preempt_disable

/* This function returns 1 if preemption is disabled, and 0 otherwise.
 * We consider preemption 'disabled' if either of two things currently hold:
 * 1. We are currently in *any* handler (i.e. the IPSR, or equivalently PSR[8:0] or ICSR[8:0], is non-zero).
 *    This implies preemption is disabled, assuming we set PendSV to be the lowest priority level (i.e. highest
 *    priority value) interrupt handler, meaning that PendSV can preempt no other handler.
 * 2. The BASEPRI is raised to the priority of PendSV, which prevents PendSV from activating. */
.global rtos_internal_check_preempt_disabled
.type rtos_internal_check_preempt_disabled,#function
/* bool rtos_internal_check_preempt_disabled(void); */
rtos_internal_check_preempt_disabled:
        /* If the IPSR is non-zero, we're in another handler and so PendSV cannot preempt it */
        mrs r0, ipsr
        cbnz r0, 1f
        /* If the BASEPRI is non-zero, we've set it to PendSV's priority and so PendSV cannot occur */
        mrs r0, basepri
        cbz r0, 2f
1:
        mov r0, #1
2:
        bx lr
.size rtos_internal_check_preempt_disabled, .-rtos_internal_check_preempt_disabled

/**
 * Trigger a context-switch to the next task runnable as determined by the scheduler.
 * Intended to be used by the RTOS internally after taking actions that change the set of schedulable tasks.
 */
.global rtos_internal_yield
.type rtos_internal_yield,#function
/* void rtos_internal_yield(void); */
rtos_internal_yield:
        /* We implement manual context switch using ARM's SVC (supervisor call) exception.
         * Upon executing the 'svc' instruction, the CPU immediately takes a SVC exception and jumps to
         * 'rtos_internal_svc_handler'. */
        svc #0
        bx lr
.size rtos_internal_yield, .-rtos_internal_yield

.global rtos_internal_svc_handler
.type rtos_internal_svc_handler,#function
/* Implements the functionality of rtos_internal_yield. */
rtos_internal_svc_handler:
        /* This will jump to the end if no context switch is necessary, else put the new task id in r0 */
        asm_invoke_scheduler r12 r1 1f

        /* Perform the context switch */
        ldr r3, =1 /* Assuming SVC is only ever called with preemption disabled, set CONTEXT_PREEMPT_DISABLED = 1 */
        push {r3, r4-r11, lr}
        /* The values in r12, r1 and r0 are retained from asm_invoke_scheduler */
        asm_context_switch r12 r2 r1 r0

        /* Pop CONTEXT_PREEMPT_DISABLED alone. */
        pop {r3}
        /* We assume that we could've only entered SVC with preemption disabled.
         * If CONTEXT_PREEMPT_DISABLED is set, skip enabling preemption and just leave it disabled. */
        cbnz r3, 2f
        asm_preempt_enable r0
2:
        pop {r4-r11, lr}
1:
        bx lr
.size rtos_internal_svc_handler, .-rtos_internal_svc_handler

.global rtos_internal_pendsv_handler
.type rtos_internal_pendsv_handler,#function
/* The PendSV exception implements preemption and is triggered when preemption is both enabled and pending. */
rtos_internal_pendsv_handler:
        asm_preempt_clear r0 r1

        /* This will jump to the end if no context switch is necessary, else put the new task id in r0 */
        asm_invoke_scheduler r12 r1 1f

        /* Perform the context switch */
        ldr r3, =0 /* We only got here because preemption was enabled, so set CONTEXT_PREEMPT_DISABLED = 0 */
        push {r3, r4-r11, lr}
        /* The values in r12, r1 and r0 are retained from asm_invoke_scheduler */
        asm_context_switch r12 r2 r1 r0

        /* Pop CONTEXT_PREEMPT_DISABLED alone. */
        pop {r3}
        /* We assume that we could've only entered PendSV with preemption enabled.
         * If CONTEXT_PREEMPT_DISABLED is unset, skip disabling preemption and just leave it enabled. */
        cbz r3, 2f
        asm_preempt_disable r0
2:
        pop {r4-r11, lr}
1:
        bx lr
.size rtos_internal_pendsv_handler, .-rtos_internal_pendsv_handler

/**
 * Initial context switch to a task.
 * Does not return.
 *
 * @param to A pointer to the context stack frame of the task to switch to.
 */
.global rtos_internal_context_switch_first
.type rtos_internal_context_switch_first,#function
/* void rtos_internal_context_switch_first(context_t *to); */
rtos_internal_context_switch_first:
        ldr sp, [r0]
        /* Cherrypick just those register contents from the initial stack frame relevant to the task entry trampoline.
         * These hardcoded context stack frame offsets must match those used by context_init.
         * We initialized context[CONTEXT_R4_IDX] to hold the task's start function pointer. */
        ldr r4, [sp, #4]
        /* We initialized context[CONTEXT_PC_IDX] to the address of the task entry trampoline function. */
        ldr r0, [sp, #(16 * 4)]
        /* Increment the stack pointer by context[CONTEXT_SIZE] to tear down the context stack frame.  */
        add sp, sp, #(18 * 4)
        /* All instructions on the Cortex-M4 are Thumb, so the LSB of the PC must be 1 when setting it directly. */
        orr r0, r0, #1
        mov pc, r0
.size rtos_internal_context_switch_first, .-rtos_internal_context_switch_first

.global rtos_internal_task_entry_trampoline
.type rtos_internal_task_entry_trampoline,#function
/*
 * This function does not really obey a standard C ABI.
 * It is designed to be used in conjunction with the context switch code for the initial switch to a particular task.
 * The task's entry point is stored in 'r4'.
 */
rtos_internal_task_entry_trampoline:
        blx r4
.size rtos_internal_task_entry_trampoline, .-rtos_internal_task_entry_trampoline
