/*| doc_header |*/
<!-- %title eChronos Manual: Rigel Variant -->
<!-- %version 0.2 -->
<!-- %docid Wq8tAN -->


# Introduction

This document provides the information that system designers and application developers require to successfully create reliable and efficient embedded applications with eChronos.

The [Concepts] chapter presents the fundamental ideas and functionalities realized by the RTOS and how developers can harness them to successfully construct systems.

The [API Reference] chapter documents the details of the run-time programming interface that applications use to interact with the RTOS.

The [Configuration Reference] chapter details the interface to the build-time configuration of the RTOS that system designers use to tailor the RTOS to their applications.

Throughout this document, *eChronos RTOS* or *the RTOS* will refer specifically to the *Rigel* variant of eChronos.

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


## Signal Scopes

The RTOS represents signals and signal sets as bit masks of the [<span class="api">SignalSet</span>] type.
With a growing number of signals, the [<span class="api">SignalSet</span>] type may become too narrow to allocate distinct bits for all signals.
To mitigate this issue, the RTOS distinguishes between *global signals* and *task signals* which allows for optimizing the bit allocation in [<span class="api">SignalSet</span>].

### Global vs. Task Signals

*Global signals* are signals which are handled in multiple different tasks whereas a *task signal* is handled only by a small fixed set of tasks known at configuration time.

A typical example of a global signal is one that a driver task sends to client tasks to inform them that the driver has new data available for those clients.
Since those clients can be any number of tasks that are not necessarily known at configuration time, the signal in question is best configured as a global one.

Conversely, a common instance of a task signal is a signal that client tasks send to a driver task to notify the driver of client data being available.
In this case, only a single fixed task is known to handle that signal.
Since that makes for a small fixed set of handler tasks (one in this case) known at configuration time, that signal is best [configured](#task-signal-configuration) as a task signal.

### Impact on Signal IDs

Task signals allow the RTOS to optimize the allocation of bits for signal IDs and sets.
In particular, it can use fewer bits for signals that are sent to disjoint task sets.
This may allow the application to configure and use a smaller, more efficient type for signal IDs and sets.

### Impact on System Configuration and Implementation

For application correctness, it is crucial that task signals are [configured](#task-signal-configuration) and used correctly.
The system configuration needs to be correct in the sense that each task signal is configured for the correct set of tasks, i.e., all tasks that receive and handle that signal.
The application implementation needs to be correct in the sense that each task signal is sent only to and handled only in the set of tasks configured in the system configuration.

For correctness, it is necessary and sufficient that the implementation does not send a task signal to a task outside the configured task set.
For efficiency, it is recommended to configure signals as task signals with as small task sets as possible.
It is always safe but potentially inefficient to configure all signals as global signals or to configure task signals with unnecessarily large task sets.

/*| doc_api |*/
## Functions vs. Macros

Some compilers do not support function inlining.
For performance or code space considerations, some APIs described in this chapter are implemented as function-like macros.
This is an implementation detail and the use of all APIs must conform to the formal function definitions provided in this chapter.

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


## Task Signal Configuration

### `signal_labels/signal_label/global`

This optional boolean configuration item defaults to true.
If true, the given signal label is considered to refer a [global signal](#signal-scopes).
If true, the [`signal_labels/signal_label/tasks`] configuration item may not be used for the same signal label.
If false, the [`signal_labels/signal_label/tasks`] configuration item is required.

### `signal_labels/signal_label/tasks`

This optional list configuration item makes a signal a [task signal](#signal-scopes).
It defaults to the empty list.
If present, it needs to contain a non-empty set of `signal_labels/signal_label/tasks/task` configuration items that identify the tasks that the given signal is sent to.
If this list is present, the [`signal_labels/signal_label/global`] configuration item may not be set to true and is implicitly set to false.
If this configuration item is not present, the [`signal_labels/signal_label/global`] configuration item needs to be left at its default or explicitly set to true.

### `signal_labels/signal_label/tasks/task`

Each of these identifier configuration items needs to contain the name of a task that the signal `signal_label` is sent to and handled by.
See [Signal Scopes] for more information on global and task signals.

/*| doc_footer |*/
