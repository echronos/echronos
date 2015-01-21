/*| provides |*/
sched-prio-inherit
sched
preempt

/*| requires |*/
task

/*| doc_header |*/

/*| doc_concepts |*/
## Scheduling Algorithm

The scheduling algorithm is an important component of the RTOS because it determines which one of the runnable tasks is selected for active execution.
For this purpose, the RTOS uses a *strict priority with inheritance* algorithm.

Each task in the system is assigned a priority (which is a positive, integral value, with higher values meaning higher priority).
Priorities must be unique, that is, no two tasks may have the same priority.

The general rule of this scheduling algorithm is to pick the task with the highest effective priority from the set of runnable tasks.

The algorithm is *strict* in the sense that a task is never permitted to be the current task when one or more higher priority ones are runnable.
The RTOS achieves this by triggering task preemption whenever necessary (see [Preemption]).

### Priority Inheritance

Normally, a task's effective priority is the priority it has been explicitly assigned, however in some cases a task may be assigned a different priority based on *priority inheritance*.

When a task in the system is not runnable (i.e.: it is blocked), it may be blocked waiting for a specific task, or alternatively, it may be blocked waiting on an external event or no specific task.
To reduce the occurrence of priority inversion, the scheduler implements priority inheritance for the case where a task is blocked on another specific task.
A task's effective priority is the higher one of:

1. the task's own explicitly assigned priority, or
2. the highest of the effective priorities of all tasks that are blocked on the task.

Consider three tasks, A, B and C with priorities 20, 10, and 5.
If task A is blocked on task C, then C's effective priority is 20, rather than 5.
In this case, assuming C is runnable, it would be selected.
It is important to note that this inheritance relationship is transitive, so if C blocked on a task D with priority 1, then D's effective priority would be 20.

## Preemption

The RTOS is *preemptive*, which means that [Task Switching] can be triggered in either of two ways:

1. voluntarily, by RTOS code the current task chooses to execute causing the current task to become blocked (see [Task
States]), or
2. involuntarily (as far as the current task is concerned), by the RTOS due to an ISR (see [Interrupt Service Routines]) changing the set of runnable tasks.

The second case is known as *task preemption*, or just *preemption*.

When an interrupt occurs, first the ISR runs.
Then, depending on the platform and RTOS variant, the RTOS may either:

1. resume the currently executing task, provided the ISR could not have possibly changed the set of runnable tasks, or
2. use the [Scheduling Algorithm] to determine the next task to run, which may or may not be the currently executing task.

Note that at this point, the only reason why the scheduler would choose a different task to run is that the ISR changed the set of runnable tasks via an interrupt event (see [Interrupt Events]).

Finally, the RTOS performs a task switch to the new task chosen by the scheduler if it differs from the current one.
Otherwise, it resumes the current task.

/*| doc_api |*/

/*| doc_configuration |*/

/*| doc_footer |*/
