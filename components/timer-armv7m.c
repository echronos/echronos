/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}timer_tick(void);

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static bool timer_tick_interrupt_check(void);

/*| state |*/
static bool timer_tick_interrupt_flag;
static bool timer_tick_pending;
static bool timer_tick_overflow;

/*| function_like_macros |*/
/* Return whether a timer tick is pending and needs to be processed by the timer component.
 *
 * A timer tick is marked as pending by timer_tick_interrupt_check().
 * A timer tick is cleared, i.e. marked as not pending, by timer_tick_process().
 *
 * Pre & post conditions: interrupts enabled */
#define timer_tick_check() (timer_tick_pending)
/* Mark a timer tick as processed and make timer_tick_check() return false.
 *
 * A timer tick is marked as pending by timer_tick_interrupt_check().
 * A timer tick is cleared, i.e. marked as not pending, by timer_tick_process().
 *
 * Pre & post conditions: interrupts enabled */
#define timer_tick_clear() do { timer_tick_pending = false; } while (0)
/* Return whether a tick interrupt occurred while the previous one had not been processed yet.
 * The overflow is usually detected in timer_tick() or timer_tick_interrupt_check().
 *
 * Pre & post conditions: interrupts enabled */
#define timer_tick_overflow_check() (timer_tick_overflow)

/*| functions |*/
/* Check whether a timer tick interrupt has occurred and mark a timer tick as pending and requiring processing.
 *
 * This function interacts with timer_tick() ISR in that
 * - timer_tick() is called in the timer ISR and sets the tick interrupt flag
 * - this function checks and returns whether the tick interrupt flag is set; if yes, this function:
 *   - atomically clears the interrupt flag so that timer_tick() can set it again
 *   - marks a timer tick as pending and requiring processing via timer_tick_process()
 *
 * This function is always called with interrupts disabled.
 * For correct system behavior, it is vital that this function does not enable interrupts.
 * Since this function runs with interrupts disabled, the tick interrupt flag is effectively cleared atomically.
 * The distinction between the tick interrupt flag and the tick pending state allows to cleanly separate interrupt
 * sensitive and interrupt agnostic code paths in the processing of timer ticks and timers.
 *
 * The return value of this function is used to determine whether it is safe to consider the system idle and
 * potentially enter a low-power state.
 * It is deliberately not used to decide whether to call timer_tick_process() because interrupts might need to be
 * enabled between the check and the call which opens room for a race.
 *
 * Pre and post condition and invariant: interrupts disabled
 */
static bool
timer_tick_interrupt_check(void)
{
    if (timer_tick_interrupt_flag)
    {
        timer_tick_pending = true;
        timer_tick_interrupt_flag = false;
    }

    return timer_tick_pending;
}

/*| public_functions |*/
void
{{prefix_func}}timer_tick(void)
{
    if (timer_tick_pending)
    {
        timer_tick_overflow = true;
    }
    timer_tick_interrupt_flag = true;
}
