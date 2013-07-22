typedef uint{{taskid_size}}_t TaskId;
typedef TaskId IrqEventId;
typedef uint8_t MutexId;

void {{prefix}}yield_to(TaskId);
void {{prefix}}yield(void);
void {{prefix}}block(void);
void {{prefix}}unblock(TaskId);
void {{prefix}}start(void);
void {{prefix}}irq_event_raise(IrqEventId);

void {{prefix}}mutex_lock(MutexId);
bool {{prefix}}mutex_try_lock(MutexId);
void {{prefix}}mutex_unlock(MutexId);
