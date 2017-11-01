/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2017, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */

/**
 * Start a platform-specific timer (typically some hardware timer) that produces periodic timer ticks at about 100ms
 * intervals.
 *
 * @param application_timer_isr an application-defined function that implements the application-specific part of handling a
 *  tick service routine.
 *  This parameter is necessary because on some target platforms, the machine-timer implementation sets up the ISR
 *  handler internally (as opposed to the application associating the handler with some well-known, timer-related
 *  interrupt vector, for example).
 */
void machine_timer_start(void (*application_timer_isr)(void));

/**
 * Stop platform-specific timers.
 * Typically, this function only makes sense to be called after having previously called machine_timer_start().
 * However, on some platforms this function may generally stop all hardware based timers which may be useful if, e.g.,
 * the boot loader enables such timers by default.
 */
void machine_timer_stop(void);

/**
 * Handle a timer tick inside the platform-specific timer implementation.
 *
 * This function ensures that for every timer tick, the timer implementation continues to work as expected and continues
 * to deliver timer ticks.
 * For some target platforms and timer implementations, this function might be empty.
 * For others, per-tick platform-timer handling may be necessary.
 *
 * The application must call this function from its timer tick ISR if it expects the timer implementation to work as
 * expected.
 * Note that this function must only be called in interrupt mode.
 * Also note that this function only deals with the platform's timer implementation itself.
 * That is to say that this function does not call rtos_timer_tick() or any other RTOS APIs.
 */
void machine_timer_tick_isr(void);
