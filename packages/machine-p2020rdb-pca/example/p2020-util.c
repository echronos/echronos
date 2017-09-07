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
#include <stdint.h>
#include <p2020-util.h>
#include "p2020-duart.h"

#define CCSRBAR_PIC_OFFSET 0x40000
#define PIC_REGISTER_BASE (CCSRBAR + CCSRBAR_PIC_OFFSET)
#define PIC_REGISTER_GCR_OFFSET 0x1020

/* PIC registers are 32 bits wide */
#define PIC_GCR (volatile uint32_t *)(PIC_REGISTER_BASE + PIC_REGISTER_GCR_OFFSET)
#define GCR_MIXED_MODE_SET 0x20000000

/* Switch the PIC (Programmable Interrupt Controller) on the P2020 to "Mixed mode".
 * By default the PIC is in "Pass-through mode", but that routes the PCI Express 1's IRQA signal directly to the
 * external interrupt line of CPU0, causing a storm of spurious external input interrupts.
 * With the PIC switched to "mixed mode", its default mask settings kick in (where everything is masked).
 * When booting from U-Boot, this is not necessary. */
void
machine_pic_init(void)
{
    *PIC_GCR = GCR_MIXED_MODE_SET;
}

/* This is deliberately a busy-waiting use of DUART1 tx so that it doesn't rely on or generate any IRQs */
static void
duart_putc(const char c)
{
    static int initted = 0;

    while (!duart1_tx_ready());

    if (!initted) {
        duart1_init();
        initted = 1;
    }

    duart1_tx_put(c);
}

void
debug_putc(const char c)
{
    /* Purely to be overly safe about the readability of debugging output, ensure that every CR is preceded by a LF,
     * and every LF is followed by a CR. */
    if (c == '\r' || c == '\n') {
        duart_putc('\n');
        duart_putc('\r');
    } else {
        duart_putc(c);
    }
}

void
debug_puts(const char *s)
{
    while (*s != '\x00')
    {
        debug_putc(*s);
        s++;
    }
}
