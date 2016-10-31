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
#include <stdbool.h>
#include <avr/io.h>
#if defined (__AVR_ATmega128__)
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
#include <util/setbaud.h>
#else
#error unsupported hardware
#endif

static void uart_init(void);

void
debug_puts(const char *s)
{
    static bool uart_is_initialized;

    if (!uart_is_initialized)
    {
        uart_init();
        uart_is_initialized = true;
    }

    while (*s)
    {
#if defined (__AVR_ATmega128__)
        /* unfortunately, this breaks older versions of simulavr which we rely on for system tests */
        /* loop_until_bit_is_set(UCSR0A, UDRE); */
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
        loop_until_bit_is_set(UCSR0A, UDRE0);
#else
#error unsupported hardware
#endif
        UDR0 = *s;
        s += 1;
    }
}

static void uart_init(void)
{
#if defined (__AVR_ATmega128__)
    UCSR0B = (1 << RXEN) | (1 << TXEN);
    UCSR0C = (1 << UCSZ0) | (1 << UCSZ1);
#elif defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0A &= ~(_BV(U2X0));
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
#else
#error unsupported hardware
#endif
}
