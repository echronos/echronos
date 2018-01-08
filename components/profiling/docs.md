/*| provides |*/
profiling

/*| requires |*/
task

/*| doc_header |*/

/*| doc_concepts |*/
## Profiling

The `profiling` component offers optional configuration items to enable tasks monitoring and instrumentation.
Configuration items provided are  [`profiling/task_uptime`] and [`profiling/hook_for_task_switch`].
An API function `profiling_record_sample` is available when `task_uptime` is enabled.
Note that the usage of these features is not recommended in production as it impacts the system execution time.

### Task Uptime

This feature is controlled via the `task_uptime` configuration item.
It enables the API function `profiling_record_sample()` and the internal array `profiling_uptimes`.

`profiling_uptimes` is an array of type `uint32_t[]` with as many entries as there are tasks in the system.
When the application calls `profiling_record_sample()`, the RTOS increments by one the array entry that corresponds to the currently active task.
It is expressly supported to call `profiling_record_sample()` from an interrupt service routine.

This effectively allows applications to keep track of relative task uptime through random sampling:
By calling `profiling_record_sample()` from a regular timer interrupt, the `task_uptime` array over time accumulates counter values that reflect how much time is spent in each task.

### Hook for Task Switch

This configuration item instructs the RTOS to call an application-provided hook function when switching tasks.

If specified, the RTOS expects the hook to have the signature `void hook(const TaskId from, const TaskId to)`.
If the hook function uses any RTOS API, the system behavior becomes undefined.
Therefore, an application shall not call any RTOS API function from the hook function.

It is worth noting that the `from` and `to` arguments may have the same value.
This case arises, for example, when the only runnable task in the system calls the [<span class="api">yield</span>] API function.

/*| doc_api |*/
## Profiling API

### `profiling_record_sample`

<div class="codebox">void profiling_record_sample(void);</div>

/*| doc_configuration |*/
## Profiling Configuration

### `profiling`

Example:

```xml
<profiling>
    <task_uptime>false</task_uptime>
    <hook_for_task_switch>app_task_switch_notify</hook_for_task_switch>
</profiling>
```

### `profiling/task_uptime`

This configuration item enables the application to periodically record tasks uptime (also see [Task Uptime]).
This is item is optional and is of a boolean type, which defaults to true.

### `profiling/hook_for_task_switch`

This configuration item specifies the function called when the RTOS switches tasks (see [Hook for Task Switch]).
It must be the name of a function that the application implements so that it is available at link time.
This is an optional configuration item.

/*| doc_footer |*/
