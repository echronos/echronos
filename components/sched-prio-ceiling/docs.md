/*| provides |*/
sched-prio-ceiling
sched
preempt

/*| requires |*/
task

/*| doc_header |*/

/*| doc_concepts |*/
## Scheduling Algorithm

The scheduling algorithm is an important component of the RTOS because it determines which one of the runnable tasks is selected for active execution.
For this purpose, the RTOS uses a *strict priority* algorithm with a *priority ceiling protocol* (see [Priority Ceiling Protocol]).

Each task in the system is statically assigned a priority (which is a positive, integral value, with higher values meaning higher priority).
Priorities must be unique, that is, no two tasks may have the same priority.

The general rule of this scheduling algorithm is to pick the task with the highest effective priority from the set of runnable tasks.

The algorithm is *strict* in the sense that a task is never permitted to be the current task when one or more higher priority ones are runnable.
The RTOS achieves this by triggering task preemption whenever necessary (see [Preemption]).

### Priority Ceiling Protocol

To prevent the occurrence of unbounded priority inversion and mutual deadlock, the scheduler implements a priority ceiling protocol.
Specifically, it implements the *immediate ceiling* (also known as *highest locker's*) *priority protocol*.

Normally, a task's effective priority is the priority it has been explicitly assigned, however in some cases a task may be assigned a higher priority based on the *ceiling priority* of any mutexes it currently holds.

The ceiling priority of each mutex is a priority value statically assigned to each mutex in the system, that must be calculated to be just higher than the highest statically assigned priority of any task that will lock that mutex.
Like task priorities, the ceiling priority value of each mutex must also be unique, with respect to the assigned priorities of any other mutexes and tasks.

When a task locks a mutex, the task's effective priority will immediately be raised to the ceiling priority of that mutex (if higher).
Therefore at any given time, a task's effective priority is the higher one of:

1. the task's own explicitly assigned priority, or
2. the highest of the ceiling priorities of all mutexes that the task currently holds.

If a task tries to lock a mutex that is already locked, the task is blocked.
When a task unlocks a mutex, the highest priority task blocked on it is unblocked.

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
