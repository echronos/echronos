#include "mutex.h"

bool
_mutex_lock({{prefix}}mutex *mutex, uint16_t timeout
[[#has_signal]]
, SignalSet signal, TaskId taskToSignal
[[/has_signal]]
)
{
[[#has_clock]]
    clock_t end = clock() + timeout / (CLOCKS_PER_SEC / 1000);
[[/has_clock]]
    while (mutex->locked)
    {
[[#has_clock]]
         if(clock() >= end)
         {
[[/has_clock]]
[[#has_signal]]
             if (signal)
             {
                 {{prefix}}signal_send_set(taskToSignal, signal);
             }
[[/has_signal]]
[[#has_clock]]
             return false;
         }
[[/has_clock]]
        yield();
    }
    mutex->locked = true;
    return true;
}
