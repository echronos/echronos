/*| provides |*/
acamar
rtos

/*| requires |*/
docs

/*| doc_header |*/
<!-- %title eChronos RTOS Manual: Acamar Variant -->
<!-- %version 0.2 -->
<!-- %docid 2SmYxA -->


# Introduction

This document provides the information that system designers and application developers require to successfully create reliable and efficient embedded applications with the eChronos real-time operating system.

The [Concepts] chapter presents the fundamental ideas and functionalities realized by the RTOS and how developers can harness them to successfully construct systems.

The [API Reference] chapter documents the details of the run-time programming interface that applications use to interact with the RTOS.

The [Configuration Reference] chapter details the interface to the build-time configuration of the RTOS that system designers use to tailor the RTOS to their applications.

Throughout this document, *eChronos RTOS* or *the RTOS* will refer specifically to the *Acamar* variant of the eChronos RTOS.

/*| doc_concepts |*/
## Overview

The eChronos RTOS facilitates the rapid development of reliable, high-performance embedded applications.
It allows developers to focus on the application logic by wrapping the complexities of low-level platform and system code in a comprehensive, easy-to-use operating-system API.
Since each application configures the RTOS to its specific requirements, this document refers to the combination of RTOS and application code simply as the *system*.

In terms of its functionality, the RTOS is a task-based operating system that multiplexes the available CPU time between tasks.
Since it is non-preemptive, tasks execute on the CPU until they voluntarily relinquish the CPU by calling an appropriate RTOS API function.
The RTOS API (see [API Reference]) gives tasks access to the objects that the RTOS provides.

A distinctive feature of the RTOS is that these objects, including tasks, are defined and configured at build time (see [Configuration Reference]), not at run time.
This configuration defines, for example, the tasks that exist in a system at compile and run time.
Static system configuration like this is typical for small embedded systems.
It avoids the need for dynamic memory allocation and permits a much higher degree of code optimization.
The [Configuration Reference] chapter describes the available configuration options for each type of object in the RTOS.


## The Acamar Variant

The acamar variant of the RTOS is a bare-bones RTOS with a single feature: context switching between multiple tasks.
This narrow feature set makes it a good candidate to start experimenting with the RTOS, but is unlikely to provide much benefit to real-world applications.

An application built on top of this RTOS variant barely differs from an application that does not use any operating system at all.
The main difference is that the application logic can be split into multiple [Tasks].
For example, periodic sensor reads and user interaction could be performed by two separate tasks.

Since there is no scheduler in the RTOS variant, the tasks need to explicitly switch between each other.
To do so, they call the [<span class="api">yield_to</span>] API function, specifying the other task, respectively, as the target of the context switch.

For more complex application behavior, the RTOS provides other variants with more features.


## Startup

The RTOS does not start automatically when a system boots.
Instead, the system is expected to start normally, as per the platform's conventions and C runtime environment.
The C runtime environment invokes the canonical `main` function without any involvement of the RTOS.
This allows the user to customize how the system is initialized before starting the RTOS.

The RTOS provides a [<span class="api">start</span>] API that needs to be called to initialize the RTOS and begin its execution.
The [<span class="api">start</span>] API never returns because it transfers control to the RTOS and its [Tasks].
From the application's point of view, calling the [<span class="api">start</span>] API function makes the first task in the system execute its task function (see [Task Functions]).
From this point onwards, task implementations must explicitly [<span class="api">yield_to</span>] other tasks to allow all tasks to perform their respective functions.

There is no API to shut down or stop the RTOS once it has started.

/*| doc_api |*/
## Functions vs. Macros

Some compilers do not support function inlining.
For performance or code space considerations, some APIs described in this chapter are implemented as function-like macros.
This is an implementation detail and the use of all APIs must conform to the formal function definitions provided in this chapter.


## Core API

### <span class="api">start</span>

<div class="codebox">void start(void);</div>

The [<span class="api">start</span>] API initializes the RTOS and starts the first task in its task function (see [Task Functions]).
That first task is the first task defined in the system configuration file (see [Configuration Reference]), which also specifies the task function.

This function must be called from the system's main function.
This function does not return.

### <span class="api">yield_to</span>

<div class="codebox">void yield_to(TaskId task_id);</div>

The [<span class="api">yield_to</span>] API causes a context switch to the task with the specified task ID.
If the specified task ID identifies the current task, [<span class="api">yield_to</span>] returns without a context switch and has no application-visible effect.


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
