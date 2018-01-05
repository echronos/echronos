Reviewer: Stefan Götz (github.nooneelse@spamgourmet.com)
Conclusion: Rework

Comment: Accept
That's a really good functionality to have for applications!
Nice!

Location: pm/tasks/add_task_profiling_hooks:15
Comment: Rework
This is nit picking, but please put each sentence on a separate line (see convention 05K0tk).

Location: components/profiling/schema.xml:3
Comment: Rework
Although somewhat a matter of taste, the name `extern_task_profiling_hook` of the configuration item has the following issues in my opinion:
- why the 'extern_' prefix?
  If it is to indicate that it is a symbol to implemented by the application instead of the RTOS, how about using `application_` instead?
- 'task_profiling_hook' is very generic and does not actually describe the event this hook instruments.
  How about a more descriptive name, such as `hook_for_task_switch`?

Location: components/rigel/implementation.c:53
Comment: Rework
The current implementation works when the hook is a globally defined pre-processor macro.
However, when the application implements the hook as a function, the compiler expects that function to be declared, but cannot find such a declaration.
Building a system with a configured hook therefore fails with an error such as ```out\posix\rigel\posix\rtos-rigel.c: In function ‘yield_to’:
out\posix\rigel\posix\rtos-rigel.c:885:5: error: implicit declaration of function ‘app_hook’ [-Werror=implicit-function-declaration]```
The hook being provided as a function seems like a rather common use case to me and I think it would be worth supporting it.

Comment: Rework
Add or extend any kind of test that checks whether and shows that this functionality works.

Comment: Rework
I agree with David already having pointed out that this feature needs documentation.
It needs to document the name of the configuration item, the function signature of the hook, the event it instruments, and the fact that the from and to arguments can have the same value.
I would consider it important that this basic level of documentation is part of this task, not some other task.

Comment: Accept
Is it possible to integrate this functionality into the other major variants, kochab and phact?
Superficially, it does not look like a lot of effort, but maybe there are some more fundamental blockers I might be overlooking?
