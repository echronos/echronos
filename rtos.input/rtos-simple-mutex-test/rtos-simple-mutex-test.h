typedef uint8_t MutexId;

void {{prefix}}yield(void);
void {{prefix}}mutex_lock(MutexId);
void {{prefix}}mutex_unlock(MutexId);
bool {{prefix}}mutex_try_lock(MutexId);
