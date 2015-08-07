<!---
eChronos Real-Time Operating System
Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, version 3, provided that these additional
terms apply under section 7:

  No right, title or interest in or to any trade mark, service mark, logo
  or trade name of of National ICT Australia Limited, ABN 62 102 206 173
  ("NICTA") or its licensors is granted. Modified versions of the Program
  must be plainly marked as such, and must not be distributed using
  "eChronos" as a trade mark or product name, or misrepresented as being
  the original Program.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@TAG(NICTA_DOC_AGPL)
  -->

%title RTOS Example package: user manual
%version 1-draft
%docid ApE1iw

Introduction
-------------

The RTOS Example package contains the code for a number of RTOS example programs:

<dl>
  <dt>`signal-demo`</dt>
  <dd>An example C program demonstrating signal functionality on variants that support it.</dd>

  <dt>`kochab-mutex-demo`</dt>
  <dd>An example C program demonstrating mutex functionality on the Kochab variant.</dd>

  <dt>`phact-mutex-demo`</dt>
  <dd>An example C program demonstrating mutex functionality on the Phact variant.</dd>

  <dt>`sem-demo`</dt>
  <dd>An example C program demonstrating semaphore functionality on variants that support it.</dd>

  <dt>`sched-demo`</dt>
  <dd>An example C program demonstrates scheduler behavior on variants that support priority scheduling.</dd>

  <dt>`timer-test`</dt>
  <dd>An example C program that tests runtime timer APIs on variants that support it.</dd>

  <dt>`kochab-test`</dt>
  <dd>An example C program demonstrating task preemption functionality on the Kochab variant, driven by timer interrupt events.</dd>
</dl>

RTOS variant-agnostic program modules in this package take a non-optional `variant` configuration element that must be supplied to them by the system `.prx` file, so that they can include the correct RTOS variant header.

For example, when building `rtos-example.timer-test` for the Kochab variant:

    <module name="rtos-example.timer-test">
      <variant>kochab</variant>
    </module>


`signal-demo`
====================

This system demonstrates signal functionality on variants that support priority scheduling:

  Part 0 has one task (A) demonstrate peeking and polling of signals, as well as waits returning immediately if a signal in the set is already available.

  Part 1 shows a higher priority task (A) block waiting on a signal set, and being unblocked by a lower priority task (B) sending it signals in the set.
  Signals not in the set will not wake task (A), but will remain pending such that (A) will return immediately if it tries to wait on them subsequently.

There is no LED activity in this system, only debug prints via GDB.
The following is the expected output of the signal demo, continuing from a breakpoint set at `rtos_start`:

    (gdb) c
    Continuing.

    Part 0: Solo

    a: setting all signals
    a: peeking/polling specific signals
    a: checking for stray signals
    a: re-setting all signals
    a: peeking/polling specific signals in reverse order
    a: checking for stray signals
    a: re-setting all signals
    a: peeking/polling whole signal set
    a: checking for stray signals
    a: re-setting all signals
    a: waiting on specific signals (should not block)
    a: checking for stray signals
    a: re-setting all signals
    a: waiting on specific signals in reverse order (should not block)
    a: checking for stray signals
    a: re-setting all signals
    a: waiting on signal set (should not block)
    a: checking for stray signals

    Part 1: B unblocks A

    a: waiting on signal set
    b: sending specific signal to a
    a: checking for stray signals
    a: waiting on specific signal
    b: sending whole signal set to a
    a: got specific signal
    a: gathering up all other signals b sent (should not block)
    a: checking for stray signals
    a: waiting on specific signal
    b: sending signals that shouldn't wake up a
    b: sending signal to wake up a
    a: got specific signal
    a: gathering up all other signals b sent (should not block)
    a: checking for stray signals

    Done.


`kochab-mutex-demo`
===================

This system demonstrates the Kochab variant's mutex functionality, whose behavior is subject to priority scheduling with priority inheritance:

  Part 0 has one task (A) demonstrate trying and releasing of a mutex, as well as taking of a mutex when the mutex is available.

  Part 1 shows a lower priority task (B) unblocking a higher priority task (A) by releasing a mutex it is blocked waiting on.

  Part 2 demonstrates the lowest-priority task (Z) inheriting the priority of the highest priority task (A), due to task (Z) holding a mutex that task (A) is blocked waiting on.
  Thus, task (Z) is scheduled over tasks (B) and (Y), despite them normally having a priority higher than task (Z).

  Part 3 demonstrates that priority inheritance is transitive.
  Task (A) is blocked by one mutex held by task (Y), and task (Y) is blocked by another mutex held by task (Z).
  Therefore, task (Z) inherits the priority of task (A) via task (Y), and is thus scheduled ahead of task (B), even though task (B) normally has a higher priority than both (Y) and (Z).

  Part 4 demonstrates that if two tasks (B) and (Y) are both waiting to acquire a mutex, the higher-priority task (B) will be given ownership of the mutex in preference to the lower-priority task (Y).

  Part 5 demonstrates that the higher-priority task (B) will be given preference over the lower priority task (Y) regardless of which of the two tasks attempted to acquire the mutex first.

  Part 6 demonstrates a lock attempt on a mutex by task (B) timing out due to task (A) not unlocking the mutex until after the requested timeout has expired.

  Part 7 demonstrates a lock attempt on a mutex by task (B) succeeding before its timeout due to task (A) unlocking the mutex within the requested time.

There is no LED activity in this system, only debug prints via GDB.
The following is the expected output of the mutex demo, continuing from a breakpoint set at `rtos_start`:

    (gdb) c
    Continuing.

    Part 0: Solo

    a: taking lock
    a: trying held lock
    a: releasing lock
    a: trying unheld lock
    a: releasing lock

    Part 1: B unblocks A

    a: waiting on signal
    b: taking lock
    b: sending signal to a
    a: got signal, waiting on lock
    b: now runnable. releasing lock
    a: got lock, releasing lock

    Part 2: Z inherits from A, over B and Y

    a: waiting on signal
    b: waiting on signal
    y: waiting on signal
    z: taking lock
    z: sending signal to y
    y: got signal, now y is runnable. sending signal to b
    b: got signal, now b is runnable. sending signal to a
    a: got signal, waiting on lock
    z: inherited priority from a, over b and y. releasing lock
    a: got lock, releasing lock

    Part 3: Z inherits from A via Y, over B

    a: waiting on signal
    b: waiting on signal
    y: waiting on signal
    z: taking 2nd lock
    z: sending signal to y
    y: got signal, grabbing 1st lock
    y: sending signal to b
    b: got signal, now b is runnable. sending signal to a
    a: got signal, waiting on 1st lock
    y: inherited priority from a, over b. now waiting on 2nd lock
    z: inherited priority from a via y, over b. releasing 2nd lock
    y: got 2nd lock, releasing it
    y: releasing 1st lock
    a: got 1st lock, releasing it

    Part 4: B and Y compete. B tries to acquire first

    a: taking the lock
    a: waiting on signal
    b: blocking on the lock
    y: blocking on the lock
    z: sending signal to a
    a: releasing the lock
    a: taking the lock again
    a: should still be running, with the lock. releasing it
    a: waiting on signal to let new lock holder run
    b: should get the lock before y does. releasing it
    b: blocking again on the lock
    b: should still be running, with the lock. releasing it
    b: waiting on signal to let lock holder run
    y: should be the last task to get the lock. releasing it
    y: sending signal to b
    b: sending signal to a

    Part 5: B and Y compete. Y tries to acquire first

    a: taking the lock
    a: waiting on signal
    b: waiting on signal to let y go first
    y: blocking on the lock
    z: sending signal to b
    b: blocking on the lock
    z: sending signal to a
    a: releasing the lock
    a: waiting on signal to let new lock holder run
    b: should still get the lock before y does. releasing it
    b: waiting on signal to let lock holder run
    y: should be the last task to get the lock. releasing it
    y: sending signal to b
    b: sending signal to a

    Part 6: B's lock attempt times out

    a: taking the lock
    a: sleeping
    b: blocking on the lock, should time out
    y: sleeping
    z: sleeping
    b: waiting for a to signal the lock's free
    a: releasing the lock
    a: signalling b
    a: waiting until b has the lock
    b: taking the lock
    b: waking up a

    Part 7: B gets lock before timeout

    a: taking the lock
    a: sleeping
    b: blocking on the lock, should succeed
    y: sleeping
    z: sleeping
    a: releasing the lock
    a: waiting until b has the lock
    b: waking up a
    a: blocking on the lock
    b: releasing the lock

    Done.


`phact-mutex-demo`
===================

This system demonstrates the Phact variant's mutex functionality, whose behavior is subject to priority ceiling protocol scheduling:

  Part 0 has one task (A) demonstrate trying and releasing of a mutex, as well as taking of a mutex when the mutex is available.

  Part 1 has four tasks (A, B, Y, and Z) taking turns to lock a mutex whose priority ceiling is higher than all of their task priorities.
  It also shows that upon releasing such a mutex, a task will immediately revert to its original priority, implicitly yielding to the highest-priority runnable task if necessary.

  Part 2 has three tasks (B, Y, and Z) taking turns to lock a mutex whose priority ceiling is higher than all of theirs, but lower than that of the highest-priority task (A).

  Part 3 has two tasks (B, Y) competing on a mutex whose priority ceiling is higher than both of theirs, in the midst of which a third task (Z) interacts with a mutex whose priority ceiling lies between the task priorities of (B) and (Y).
  Both mutexes' priority ceilings are lower than that of the highest-priority task (A).
  All three tasks (B, Y, Z) intermittently wake the highest-priority task (A) to show that it is prioritized above all others regardless of other tasks' acquisition of either of the two mutexes.

  Part 4 demonstrates a lock attempt on a mutex by task (B) timing out due to task (A) not unlocking the mutex until after the requested timeout has expired.

  Part 5 demonstrates a lock attempt on a mutex by task (B) succeeding before its timeout due to task (A) unlocking the mutex within the requested time.

  Part 6 has one task (A) lock a mutex whose priority ceiling is lower than its task priority, which is illegal.
  This triggers a fatal error.

The following is the expected output of the Phact mutex demo:

    Part 0: Solo

    a: taking lock
    a: trying held lock
    a: releasing lock
    a: trying unheld lock
    a: releasing lock

    Part 1: Mutex with ceiling higher than all tasks

    a: waiting on signal
    b: waiting on signal
    y: waiting on signal
    z: taking lock
    z: sending signal to y
    z: sending signal to b
    z: sending signal to a
    z: still running at greater priority than a, b and y. releasing lock
    a: should get signal 1st, waiting again
    b: should get signal 2nd, waiting again
    y: should get signal last, taking lock
    y: sending signal to b
    y: sending signal to a
    y: still running at greater priority than a and b. releasing lock
    a: should get signal first, waiting again
    b: should get signal last, taking lock
    b: sending signal to a
    b: still running at greater priority than a. releasing lock
    a: got signal, taking lock
    a: releasing lock

    Part 2: Mutex with ceiling lower than A's

    a: waiting on signal
    b: waiting on signal
    y: waiting on signal
    z: taking lock
    z: sending signal to y
    z: sending signal to b
    z: sending signal to a
    a: got signal, waiting again
    z: still running at greater priority than b and y. releasing lock
    b: got signal, waiting again
    y: should get signal last, taking lock
    y: sending signal to b
    y: sending signal to a
    a: got signal, waiting again
    y: still running at greater priority than b. releasing lock
    b: got signal, taking lock
    b: sending signal to a
    a: got signal, waiting again
    b: releasing lock
    b: sending signal to a
    a: got signal, done

    Part 3: Two mutexes with differing ceilings

    a: waiting on signal
    b: waiting on signal
    y: taking lock H (> b)
    y: sending signal to b
    y: should still run (> b), sending signal to a
    a: got signal, waiting again
    y: waiting on signal
    b: got signal, waiting on lock H
    z: taking lock L (> y)
    z: sending signal to y
    y: got signal, releasing lock H
    b: got lock H, sending signal to a
    a: got signal, waiting again
    b: releasing lock H
    b: waiting on signal
    z: should run (> y), sending signal to a
    a: got signal, waiting again
    z: sending signal to b
    b: got signal, waiting again
    z: releasing lock L
    y: sending signal to b
    b: sending signal to a
    a: got signal, done

    Part 4: B's lock attempt times out

    a: taking the lock
    a: sleeping
    b: blocking on the lock, should time out
    y: sleeping
    z: sleeping
    b: waiting for a to signal the lock's free
    a: releasing the lock
    a: signalling b
    a: waiting until b has the lock
    b: taking the lock
    b: waking up a
    b: releasing the lock

    Part 5: B gets lock before timeout

    a: taking the lock
    a: sleeping
    b: blocking on the lock, should succeed
    y: sleeping
    z: sleeping
    a: releasing the lock
    a: waiting until b has the lock
    b: waking up a
    b: releasing the lock

    Part 6: Task takes mutex with lower priority ceiling

    a: taking lock (should trigger fatal error)
    FATAL ERROR: <hexadecimal error code for ERROR_ID_SCHED_PRIO_PCP_TASK_LOCKING_LOWER_PRIORITY_MUTEX - see rtos-variant.h>


`sem-demo`
=================

This system demonstrates semaphore functionality on variants that support priority scheduling:

  Part 0 has one task (A) demonstrate posting (denoted `V`) and trying to wait (denoted `P`) on a semaphore, as well as returning immediately from waiting on a semaphore that has already been posted.

  Part 1 has a lower-priority task (B) unblock a higher-priority task (A) by posting to a semaphore that task (A) is blocked waiting on.
  It also shows the basic property of semaphores that (A) can only wait the same number of times (B) has posted to the semaphore, before a wait attempt would block (A).

  Part 2 shows that if two tasks are both blocked waiting on a semaphore, the higher priority task (A) will be woken in preference to the lower priority task (B) when some other task (Z) posts to the semaphore, regardless of whether task (A) or task (B) attempted to wait on the semaphore first.

  Part 3 demonstrates a wait attempt on a semaphore by task (A) timing out due to task (B) not posting to the semaphore until after the requested timeout has expired.

  Part 4 demonstrates a wait attempt on a semaphore by task (A) succeeding before its timeout due to task (B) posting to the semaphore within the requested time.

  Part 5 demonstrates that if the user posts to the semaphore more times than the runtime-initialized maximum value, the RTOS will trigger a fatal error.

For RTOS variants that do not support semaphore timeouts, parts 3 and 4 can be disabled by setting the optional `timeout_tests` option to `false` when configuring this module in the system `.prx` file.

There is no LED activity in this system, only debug prints via GDB.
The following is the expected output of the semaphore demo, continuing from a breakpoint set at `rtos_start`:

    (gdb) c
    Continuing.

    Part 0: Solo

    a: initializing maximum
    a: V
    a: P (should succeed)
    a: trying P (should fail)
    a: V
    a: trying P (should succeed)

    Part 1: B unblocks A

    a: initializing maximum
    a: P (should block)
    b: V (should unblock a)
    a: now runnable
    a: consuming...
    a: P
    b: producing while a consumes...
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: P
    b: V
    a: trying P (should fail)
    a: waiting on signal
    b: producing while a is blocked...
    b: V
    b: V
    b: V
    b: V
    b: V
    b: V
    b: V
    b: V
    b: V
    b: V
    b: sending signal to a
    a: consuming...
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: trying P (should fail)

    Part 2: A and B compete

    a: initializing maximum
    a: P (should block)
    b: P (should block)
    z: V
    a: should wake up before b. P (should block)
    z: V
    a: should again wake up before b. waiting on signal
    z: V
    b: finally awake. sending signal to a

    Part 3: A's wait attempt times out

    a: initializing maximum
    a: P (should time out)
    b: sleeping
    z: sleeping
    a: waiting on signal
    b: V (should not unblock a)
    b: sending signal to a
    a: trying P (should succeed)

    Part 4: A's wait returns before timeout

    a: initializing maximum
    a: P (should succeed before timeout)
    b: sleeping
    z: sleeping
    b: V (should unblock a)

    Part 5: A posts past maximum and triggers fatal error

    a: initializing maximum
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: P
    a: trying P (should trigger fatal error)
    FATAL ERROR: <hexadecimal error code for ERROR_ID_SEMAPHORE_MAX_EXCEEDED - see rtos-variant.h>


`sched-demo`
============

This program demonstrates scheduler behavior on variants that support strict priority scheduling.
For more information on this program's test cases, please see `sched-demo.c`.

The following is the expected output of the scheduler demo, running on the Phact variant:

    fn_a starting
    fn_a test: priority inversion
    fn_b starting
    fn_b test: priority inversion
    fn_c starting
    fn_c test: priority inversion
    fn_c: got m0
    fn_c: sending sig A
    fn_c: releasing m0
    fn_a: go
    fn_a: sending sig B
    fn_a: locking M0
    fn_a: got M0
    fn_a: releasing m0
    fn_a test: priority inversion: completed
    fn_b: go
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b: looping
    fn_b test: priority inversion: completed
    fn_c test: priority inversion: completed
    fn_a test: chain blocking
    fn_b test: chain blocking
    fn_c test: chain blocking
    fn_c: locking M2
    fn_c: got m2
    fn_c: sending sig B
    fn_c: releasing m2
    fn_b: go
    fn_b: locking M1
    fn_b: got m1
    fn_b: sending sig A
    fn_b: locking M2
    fn_b: got m2
    fn_b: releasing m2
    fn_b: releasing m1
    fn_a: go
    fn_a: locking M0
    fn_a: got m0
    fn_a: locking M1
    fn_a: got m1
    fn_a: releasing m1
    fn_a: releasing m0
    fn_a test: chain blocking: completed
    fn_b test: chain blocking: completed
    fn_c test: chain blocking: completed
    fn_a test: deadlock
    fn_b test: deadlock
    fn_b: locking M0
    fn_b: got M0
    fn_b: sending sig A
    fn_b: locking M1
    fn_b: got M1
    fn_b: releasing M1
    fn_b: releasing M0
    fn_a: go
    fn_a: locking M1
    fn_a: got M1
    fn_a: locking M0
    fn_a: got M0
    fn_a: releasing M0
    fn_a: releasing M1
    fn_a test: deadlock: completed
    fn_b test: deadlock: completed
    fn_c test: deadlock
    fn_c test: deadlock: completed
    fn_a done
    fn_b done
    fn_c done

When run on the Kochab variant, the `deadlock` section of the test is expected to deadlock.
It can be disabled by setting the optional `deadlock_test` option to `false` when configuring this module in the system `.prx` file.


`timer-test`
============

This system features two tasks concurrently exercising the RTOS' runtime timer APIs to configure and make use of timers.

Task A uses two timers: The first, WATCHDOG_A, is configured to cause a fatal error exactly one tick after the expected duration of the main body of the test.
The second timer, WAKE_A, is configured to reload itself periodically on expiry, and on each expiry to send task A the WAKE signal.

For a fixed number of iterations, task A alternates between waiting for the WAKE signal to arrive, and sleeping for longer than the WAKE_A timer period so that an overflow occurs.
On each "sleeping" iteration, task A calls the `timer_check_overflow` API on the WAKE_A timer to ensure that an overflow indeed occurred, and to clear the overflow state of the timer.

Finally, task A will issue a 1 second sleep, but before the sleep ever completes, the WATCHDOG_A timer should expire and cause a fatal error.

Task B uses one timer, WATCHDOG_B, which is configured to cause a fatal error if it ever expires.
However, task B just sits in a while-forever loop periodically sleeping for a time period just one second short of the expiry, and on each iteration resetting its watchdog, so that it never expires.

The test ends successfully once the fatal error at the end of task A's main body occurs, as long as this happens before the call to `sleep` returns at the end of task A.
After this point the test is over, although the `tick_irq` will continue to come in once every tick period.

The following is the expected output of the timer test, continuing from a breakpoint set at `rtos_start`:

    (gdb) c
    Continuing.
    a: enabling watchdog timer
    a: enabling periodic wake timer
    a: waiting for wake signal
    b: starting secondary watchdog
    tick_irq: 0x00000000
    tick_irq: 0x00000001
    b: restarting secondary watchdog
    tick_irq: 0x00000002
    a: sleeping to overflow wake timer
    tick_irq: 0x00000003
    b: restarting secondary watchdog
    tick_irq: 0x00000004
    tick_irq: 0x00000005
    b: restarting secondary watchdog
    tick_irq: 0x00000006
    tick_irq: 0x00000007
    b: restarting secondary watchdog
    tick_irq: 0x00000008
    a: polled pending wake signal
    a: waiting for wake signal
    tick_irq: 0x00000009
    b: restarting secondary watchdog
    tick_irq: 0x0000000a
    tick_irq: 0x0000000b
    a: sleeping to overflow wake timer
    b: restarting secondary watchdog
    tick_irq: 0x0000000c
    tick_irq: 0x0000000d
    b: restarting secondary watchdog
    tick_irq: 0x0000000e
    tick_irq: 0x0000000f
    b: restarting secondary watchdog
    tick_irq: 0x00000010
    tick_irq: 0x00000011
    a: polled pending wake signal
    a: waiting for wake signal
    b: restarting secondary watchdog
    tick_irq: 0x00000012
    tick_irq: 0x00000013
    b: restarting secondary watchdog
    tick_irq: 0x00000014
    a: sleeping to overflow wake timer
    tick_irq: 0x00000015
    b: restarting secondary watchdog
    tick_irq: 0x00000016
    tick_irq: 0x00000017
    b: restarting secondary watchdog
    tick_irq: 0x00000018
    tick_irq: 0x00000019
    b: restarting secondary watchdog
    tick_irq: 0x0000001a
    a: polled pending wake signal
    a: waiting for wake signal
    tick_irq: 0x0000001b
    b: restarting secondary watchdog
    tick_irq: 0x0000001c
    tick_irq: 0x0000001d
    a: sleeping to overflow wake timer
    b: restarting secondary watchdog
    tick_irq: 0x0000001e
    tick_irq: 0x0000001f
    b: restarting secondary watchdog
    tick_irq: 0x00000020
    tick_irq: 0x00000021
    b: restarting secondary watchdog
    tick_irq: 0x00000022
    tick_irq: 0x00000023
    a: polled pending wake signal
    a: waiting for wake signal
    b: restarting secondary watchdog
    tick_irq: 0x00000024
    tick_irq: 0x00000025
    b: restarting secondary watchdog
    tick_irq: 0x00000026
    a: sleeping to overflow wake timer
    tick_irq: 0x00000027
    b: restarting secondary watchdog
    tick_irq: 0x00000028
    tick_irq: 0x00000029
    b: restarting secondary watchdog
    tick_irq: 0x0000002a
    tick_irq: 0x0000002b
    b: restarting secondary watchdog
    tick_irq: 0x0000002c
    a: polled pending wake signal
    a: disabling periodic wake timer
    a: the watchdog should fatal error before this sleep completes
    tick_irq: 0x0000002d
    FATAL ERROR: <hexadecimal error code for DEMO_ERROR_ID_WATCHDOG_A - see timer-test.c>
    tick_irq: 0x0000002e
    tick_irq: 0x0000002f
    tick_irq: 0x00000030
    < and so on ... >

This module depends on an external code module to implement `machine_timer_init()` and `machine_timer_clear()` for the platform the test system is to be run on.


`kochab-test`
===============

This system demonstrates the Kochab variant's task preemption functionality.
It features two tasks, A and B, where task A is assigned a higher priority than task B.

The test program also configures a fixed-interval timer interrupt to occur periodically, and supplies an interrupt handler function, `tick_irq`, that fulfils three responsibilities:

1. Firstly, it takes platform-specific action to clear the interrupt pending status with the hardware.
2. Secondly, it raises an interrupt event (which fulfils its role in the test system in waking up A).
3. Finally, it returns true, to indicate that the handler potentially caused a preemption.

Note that in general, a user-supplied interrupt handler function supplied in the system `.prx` configuration MUST be responsible for clearing the hardware condition that caused its interrupt.
Furthermore, if marked `preempting` in the `.prx`, it MUST return a true boolean value if the handler on that run made an action that has the potential to cause a preemption, such as raising an irq event.

Kochab's expected behaviour is to *preempt* the currently running task if an interrupt handler raises an event that causes a higher-priority task to become runnable.
In other words, the RTOS should immediately context switch to the higher-priority task.
This should occur regardless of whether any lower-priority task is currently running, or blocked.

In the first section, task A sends a number of signals to B that are meant to unblock task B, but A should continue to run because it is of a higher priority than B.

After printing "A now waiting for ticks", A goes into a loop where it waits on a signal set that is only delivered whenever an interrupt event is raised by the timer interrupt event handler.
Whenever it receives this signal, it prints "tick" and sends a signal to wake up B.

With task A waiting on interrupt events, lower-priority task B now comes alive, and goes into a loop where it takes turns busy-waiting (via a loop that calls `signal_poll_set`), and waiting (via `signal_wait_set`) on the signal sent by A.

The `kochab-test` program's expected output is as follows:

    Starting RTOS
    task a: taking lock
    task a: releasing lock
    task a
    unblocking b
    task a
    task a
    task a
    task a
    task a
    unblocking b
    task a
    task a
    task a
    task a
    A now waiting for ticks
    tick
    task b: attempting lock
    task b: got lock
    task b blocking
    tick
    task b unblocked
    tick
    (...)
