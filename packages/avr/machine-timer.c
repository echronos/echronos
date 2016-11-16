/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2018, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
 */

/*<module>
    <code_gen>template</code_gen>
    <headers>
        <header path="../rtos-example/machine-timer.h" code_gen="template" />
    </headers>
</module>*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "machine-timer.h"

static void (*application_timer_isr_pointer)(void);

void
machine_timer_start(void (*application_timer_isr)(void))
{
    application_timer_isr_pointer = application_timer_isr;

#if defined (__AVR_ATmega128__)
    TCCR0 = 0; /* stop counter */
    TCNT0 = 0; /* reset timer counter */
    TIFR = (1 << TOV0); /* reset overflow flag */
    TIMSK |= (1 << TOIE0); /* enable interrupt on overflow */
    sei(); /* enable interrupts globally */
    TCCR0 = (1 << CS02) | (1 << CS00); /* start timer on max prescaler */
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
    TCCR1B = (1 << WGM12) | (1 << CS12); /* enable CTC mode with prescaler of 256 */
    OCR1A = 0xFFFF; /* set counter value */
    TIMSK1 = (1 << OCIE1A); /* enable timer interrupts */
    sei(); /* enable interrupts globally */
#else
#error unsupported hardware
#endif
}

void
machine_timer_stop(void)
{
#if defined (__AVR_ATmega128__)
    TIMSK &= ~(1 << TOIE0); /* disable interrupt on overflow */
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
    TIMSK0 &= ~(1 << TOIE0); /* disable interrupt on overflow */
#else
#error unsupported hardware
#endif
}

void
machine_timer_tick_isr(void)
{
}

#if defined (__AVR_ATmega128__)
ISR(TIMER0_OVF_vect)
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
ISR(TIMER1_COMPA_vect)
#else
#error unsupported hardware
#endif
{
   application_timer_isr_pointer();
}
