/*| provides |*/
reentrant

/*| requires |*/
task

/*| doc_header |*/
/*| doc_concepts |*/
## Reentrancy

Some but not all RTOS APIs are *reentrant*.

RTOS APIs that are reentrant are ones that may (but do not necessarily) switch to another task before returning to the caller, such as [<span class="api">signal_wait</span>].

If a function is reentrant, it means that there may exist execution paths that enter the function, and then, before leaving the function, enter the same function again, whether in the same or different tasks.

[[^preemptive]]
Since the RTOS is non-preemptive, this implies the following:

- a non-reentrant function is executed synchronously, i.e., it is guaranteed that the task calling the function remains active until the function returns;

- a reentrant function may or may not return directly;
  0 or more context switches to other tasks may occur before the calling task becomes the current task again and the function returns.
[[/preemptive]]

In the RTOS header file, all reentrant and only reentrant API declarations are marked with the <span class="api">REENTRANT</span> preprocessor macro.

/*| doc_api |*/
/*| doc_configuration |*/
/*| doc_footer |*/
