/*| provides |*/
profiling

/*| requires |*/
task
sched

/*| doc_header |*/

/*| doc_concepts |*/
## Profiling

The `profiling` component offers optional configuration items to enable tasks monitoring and instrumentation.
Configuration items provided are  `task_uptime` and `hook_for_task_switch`.
An API function `profiling_record_sample` is available when `task_uptime` is enabled.
Note that the usage of these features is not recommended in production as it will impact the system execution time.

### Task Uptime

This feature is enabled via `task_uptime`. As a result the API function `profiling_record_sample` is available to the application.
An internal state variable `profiling_uptimes` is an array of the same sizes as the number of tasks in the system.
Calling `profiling_record_sample` essentially results in an update to the array uptime count at the index of the currently active task.

### Hook for Task Switch

This configuration item allows the application to specify a function  which RTOS calls when task switching occurs.

The function defined by the application must have the type signature `void fn(TaskId from, TaskId to)`.

This function mustn't call any RTOS specific functions which might invoke tasks switching such as `yield` or `sleep`.
Doing so is bound to lock up your application, resulting in unexpected behaviour, potentially an infinite loop, or a system timeout.

It is worth noting that `from` and `to` arguments can have the same value.

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

This configuration item enables the applcaton to periodically record tasks uptime (also see [Task Uptime](#task_uptime).
This is item is option and is of a boolean type, which defaults to true.

### `profiling/hook_for_task_switch`

This configuration item specifies the function called when RTOS switches tasks see [Hook for Task Switch](#hook_for_task_switch).
It must be the name of a function that the application implements so that it is available at link time.
This is an optional configuration item.

/*| doc_footer |*/
