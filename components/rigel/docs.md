/*| doc_header |*/
<!-- %title eChronos Manual: NEO-Dig Rigel Variant -->
<!-- %version 0.2 -->
<!-- %docid Wq8tAN -->

<!--
To do:
* potentially implement pre-processing, e.g., to mark API elements with ``func``
* reduce duplication in function API chapter
* double-check consistency with implementation
* consistently document whether function APIs may incur a context switch or not
* ensure consistency between release version and manual version
-->


# Introduction

The eChronos real time operating system (RTOS) provides multiple RTOS variants.
Each RTOS variant has an API targeted towards a specific set of use cases and requirements.
This manual describes the Rigel variant.
Throughout this document the term RTOS should be taken to refer to the Rigel RTOS variant.

The primary audience for this manual is firmware developers that are using the RTOS to develop an embedded system.

The chapter ([Concepts]) introduces the RTOS concepts, and provide a foundation for understanding how the RTOS works.

The chapter ([API Reference]) details the run-time programming interface.

The chapter ([Configuration Reference]) details the configuration interface for the RTOS.

/*| doc_concepts |*/
## Compiler Support

The RTOS has been tested with and optimised for the Keil C251[^c251_version] compiler and L251 linker.

[^c251_version]: Version 4.61a


## Startup

The RTOS does not start automatically when a system boots.
Instead, the system is expected to start normally, as per the platform's conventions and C runtime environment.
This typically means that at some point, C runtime environment invokes the canonical `main` function without any involvement of the RTOS.
This enables the system to perform an initial startup that may be required before starting the RTOS.

The RTOS provides a [<span class="api">start</span>] API that should be called to initialise the RTOS and start execution.
The [<span class="api">start</span>] API never returns.
Any tasks that are marked as auto-start are automatically started by the RTOS.
All other tasks start in the blocked state waiting to receive a start signal.
A task can be started via the [<span class="api">task_start</span>] API.

There is no API to shutdown or stop the RTOS once it has started.

/*| doc_api |*/
## Functions vs. Macros

The C251 compiler does not support static inline functions so, for performance reasons, some APIs described in this chapter are implemented as function-like macros.
This is an implementation detail and the use of all APIs must conform to the formal function definitions provided in this chapter.

### <span class="api">start</span>

<div class="codebox">void start(void);</div>

The [<span class="api">start</span>] API initialises the RTOS, makes all tasks configured as auto-start runnable and then, based on the scheduling algorithm, chooses and starts executing the current task.
This function must be called from the system's main function.
This function does not return.

### <span class="api">task_start</span>

<div class="codebox">void task_start(TaskId task);</div>

The [<span class="api">task_start</span>] API starts the specified task.
This API must only be called once for each task that is not automatically started by the RTOS.
This function is merely a convenience function for sending `SIGNAL_SET_START` to the function.

### <span class="api">task_current</span>

<div class="codebox">TaskId task_current(void);</div>

The [<span class="api">task_current</span>] API returns the task id for the current task.

### <span class="api">yield</span>

<div class="codebox">void yield(void);</div>

This [<span class="api">yield</span>] API yields to another runnable task.
Each task must yield (or block) at least once per timer period to ensure that ticks are correctly processed and that all tasks have a share of execution.
When yielding (or blocking), any raised interrupt events are processed.

/*| doc_configuration |*/
## RTOS Configuration

### `prefix`

In some cases the RTOS APIs may conflict with existing names used in a system.
A system designer may choose to prefix all RTOS APIs to help avoid namespace conflicts.
The prefix should be an all lower-case, legal C identifier.
This is an optional configuration item that defaults to no prefix.

The following examples are based on the prefix being specified as `rtos`.

* Function APIs are pre-pended with `rtos_`, for example [<span class="api">start</span>] becomes `rtos_start`.

* Types are pre-pended with `Rtos`, for example [<span class="api">TaskId</span>] becomes `RtosTaskId`.

* Constant definitions are prefixed by `RTOS_`, for example [<span class="api">TASK_ID_ZERO</span>] becomes `RTOS_TASK_ID_ZERO`.

### `fatal_error`

The `fatal_error` configuration allows the system designer to specify the function to be called in the case of a fatal error.
The specified function must be available at link time and should have the `type void fatal_error(ErrorCode)`.
The function should not return or use any RTOS interfaces.
This is a mandatory configuration item.

/*| doc_footer |*/
