/*| provides |*/
preempt-null
preempt

/*| requires |*/
reentrant
task

/*| doc_header |*/
/*| doc_concepts |*/
## Preemption

The RTOS is non-preemptive, which means that any context switch (see [Task Switching]) is actively caused by code that the current task executes.
In other words, once a task becomes the current task, it is in full control of the CPU, which it can relinquish only by actively causing a task switch.
While [Interrupt Service Routines] interrupt the current task, they immediately resume the same task and do not lead to task switches.

This also means that the system never performs a context switch asynchronously on its own or without an explicit action from the current task itself.
To put it another way, only tasks are active entities in a system, whereas the RTOS merely provides passive library code invoked by tasks.

For example, if any task enters an infinite loop such as `for (;;) ;`, the system effectively stops.
Once inside the loop, there is no call to an RTOS API that would lead to a task switch.
Therefore, to ensure that the system as a whole operates correctly, it is important that tasks perform context switches on a regular basis[^context_switch_regularity].

[^context_switch_regularity]: The exact definition of regular in this context depends on the system timing attributes.

For example, if there are any runnable tasks, it is important that they have the opportunity to become the current task.
When tasks are frequently interacting with other entities this is usually not a problem, as the task regularly block, providing the other runnable tasks with an opportunity to become the current task.
There are cases, however, when a task may wish to perform a long running operation without blocking.
In these cases, a task can perform a [<span class="api">yield</span>] operation.
It makes the current task relinquish the CPU by performing a context switch, allowing another runnable task (selected by the [Scheduling Algorithm]) to become the current task.

/*| doc_api |*/
/*| doc_configuration |*/
/*| doc_footer |*/
