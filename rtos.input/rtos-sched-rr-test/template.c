/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="num_tasks" type="int"/>
  </schema>
</module>*/

#include <stdbool.h>
#include <stdint.h>

#define TASK_ID_ZERO ((TaskId) 0u)
#define TASK_ID_NONE ((TaskIdOption) 0xffU)

typedef uint8_t TaskId;
typedef TaskId TaskIdOption;

[[sched.headers]]

[[sched.object_like_macros]]

[[sched.type_definitions]]

[[sched.structure_definitions]]

[[sched.state]]

[[sched.function_like_macros]]

[[sched.functions]]

void
pub_sched_set_runnable(const TaskId task_id)
{
    sched_set_runnable(task_id);
}

void
pub_sched_set_blocked(const TaskId task_id)
{
    sched_set_blocked(task_id);
}

TaskIdOption
pub_sched_get_next(void)
{
    return sched_get_next();
}

struct sched * pub_sched_tasks = &sched_tasks;
