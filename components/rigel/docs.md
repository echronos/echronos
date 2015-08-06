/*| provides |*/
rigel
rtos

/*| requires |*/
docs

/*| doc_header |*/
<!-- %title eChronos RTOS Manual: Rigel Variant -->
<!-- %version 0.2 -->
<!-- %docid Wq8tAN -->


# Introduction

This document provides the information that system designers and application developers require to successfully create reliable and efficient embedded applications with the eChronos real-time operating system.

The [Concepts] chapter presents the fundamental ideas and functionalities realized by the RTOS and how developers can harness them to successfully construct systems.

The [API Reference] chapter documents the details of the run-time programming interface that applications use to interact with the RTOS.

The [Configuration Reference] chapter details the interface to the build-time configuration of the RTOS that system designers use to tailor the RTOS to their applications.

Throughout this document, *eChronos RTOS* or *the RTOS* will refer specifically to the *Rigel* variant of the eChronos RTOS.

/*| doc_concepts |*/
## Overview

The eChronos RTOS facilitates the rapid development of reliable, high-performance embedded applications.
It allows developers to focus on the application logic by wrapping the complexities of low-level platform and system code in a comprehensive, easy-to-use operating-system API.
Since each application configures the RTOS to its specific requirements, this document refers to the combination of RTOS and application code simply as the *system*.

In terms of its functionality, the RTOS is a task-based operating system that multiplexes the available CPU time between tasks.
Since it is non-preemptive, tasks execute on the CPU until they voluntarily relinquish the CPU by calling an appropriate RTOS API function.
The RTOS API (see [API Reference]) gives tasks access to the objects that the RTOS provides.
They include [Interrupt Service Routines], [Signals], [Time and Timers], [Mutexes], and [Message Queues].

A distinctive feature of the RTOS is that these objects, including tasks, are defined and configured at build time (see [Configuration Reference]), not at run time.
This configuration defines, for example, the tasks and mutexes that exist in a system at compile and run time.
Static system configuration like this is typical for small embedded systems.
It avoids the need for dynamic memory allocation and permits a much higher degree of code optimization.
The [Configuration Reference] chapter describes the available configuration options for each type of object in the RTOS.


## Startup

The RTOS does not start automatically when a system boots.
Instead, the system is expected to start normally, as per the platform's conventions and C runtime environment.
The C runtime environment invokes the canonical `main` function without any involvement of the RTOS.
This allows the user to customize how the system is initialized before starting the RTOS.

The RTOS provides a [<span class="api">start</span>] API that needs to be called to initialize the RTOS and begin its execution.
The [<span class="api">start</span>] API never returns.
Any tasks that are marked as auto-start are automatically started by the RTOS.
All other tasks are initially in the blocked state, waiting to receive a start signal.
A start signal can be sent to a task via the [<span class="api">task_start</span>] API.

There is no API to shut down or stop the RTOS once it has started.

/*| doc_api |*/
## Functions vs. Macros

Some compilers do not support function inlining.
For performance or code space considerations, some APIs described in this chapter are implemented as function-like macros.
This is an implementation detail and the use of all APIs must conform to the formal function definitions provided in this chapter.


## Core API

### <span class="api">start</span>

<div class="codebox">void start(void);</div>

The [<span class="api">start</span>] API initializes the RTOS, makes all tasks configured as auto-start runnable and then, based on the scheduling algorithm, chooses and starts executing the current task.
This function must be called from the system's main function.
This function does not return.

### <span class="api">task_start</span>

<div class="codebox">void task_start(TaskId task);</div>

The [<span class="api">task_start</span>] API starts the specified task.
This API must be called only once for each task that is not automatically started by the RTOS.
This function is merely a convenience function for sending `SIGNAL_SET_START` to the function.

### <span class="api">yield</span>

<div class="codebox">void yield(void);</div>

The [<span class="api">yield</span>] API causes a context switch to a runnable task in the system.
That task is determined by the scheduler.
If the current task is the only runnable task in the system, [<span class="api">yield</span>] returns without a context switch.
Each task must yield (or block) at least once per timer period to ensure that ticks are correctly processed and that all tasks receive a share of execution time on the CPU.
When yielding (or blocking), any raised interrupt events are processed.

### <span class="api">sleep</span>

<div class="codebox">void sleep(TicksRelative ticks);</div>

The [<span class="api">sleep</span>] API blocks the current task for the specified number of timer ticks.
After `ticks` timer ticks, the task becomes runnable again.
Note that that does not immediately make it the current task again.
See the [Time and Timers] section for further information on timing and scheduling considerations.


/*| doc_configuration |*/
## RTOS Configuration

### `prefix`

In some cases the RTOS APIs may conflict with existing symbol or pre-processor macro names used in a system.
Therefore, the RTOS gives system designers the option to prefix all RTOS APIs to help avoid name-space conflicts.
The prefix must be an all lower-case, legal C identifier.
This is an optional configuration item that defaults to the empty string, i.e., no prefix.

The following examples are based on `prefix` having the value `rtos`.

* functions and variables: lower-case version of `prefix` plus an underscore, so [<span class="api">start</span>] becomes `rtos_start` and [<span class="api">task_current</span>] becomes `rtos_task_current`.

* types: CamelCase version of `prefix`, so [<span class="api">TaskId</span>] becomes `RtosTaskId`.

* constants: upper-case version of `prefix` plus an underscore, so [<span class="api">TASK_ID_ZERO</span>] becomes `RTOS_TASK_ID_ZERO`.

/*| doc_footer |*/
