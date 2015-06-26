/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that no right, title
 * or interest in or to any trade mark, service mark, logo or trade name
 * of NICTA or its licensors is granted.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @TAG(NICTA_AGPL)
 */

#include <stdint.h>
#include <stdbool.h>
#include <p2020-util.h>

#include "machine-timer.h"
#include "p2020-duart.h"
#include "p2020-pic.h"

#include "rtos-{{variant}}.h"
#include "debug.h"

#define PIC_IIV_DUART_EXAMPLE_PRIORITY 2
#define PIC_IIV_DUART_EXAMPLE_VECTOR 0xbeef

#define PIC_GT_EXAMPLE_BASE_COUNT 0x4000000
#define PIC_GT_EXAMPLE_PRIORITY 0xa
#define PIC_GT_EXAMPLE_VECTOR(x) (0xf000 + ((x) & 1) + (((x) & 2) ? 0x10 : 0) + (((x) & 4) ? 0x100 : 0))

bool
exti_irq(void)
{
    uint32_t inc_vector;

    debug_print("exti_irq: ");

    inc_vector = pic_iack_get();

    debug_printhex32(inc_vector);
    debug_print(": ");

    switch (inc_vector) {
    case PIC_IIV_DUART_EXAMPLE_VECTOR:
        /* The incoming character must be read to clear the interrupt condition */
        if (duart1_rx_ready()) {
            debug_print("DUART1: ");
            debug_putc(duart1_rx_get());
            rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_J);
            if (duart2_rx_ready()) {
                debug_print(", DUART2: ");
                debug_putc(duart2_rx_get());
                rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_K);
            }
        } else if (duart2_rx_ready()) {
            debug_print("DUART2: ");
            debug_putc(duart2_rx_get());
            rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_L);
        }
        debug_println("");
        break;
    case PIC_GT_EXAMPLE_VECTOR(0):
        debug_println("PIC timer A0");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_A);
        break;
    case PIC_GT_EXAMPLE_VECTOR(1):
        debug_println("PIC timer A1");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_B);
        break;
    case PIC_GT_EXAMPLE_VECTOR(2):
        debug_println("PIC timer A2");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_C);
        break;
    case PIC_GT_EXAMPLE_VECTOR(3):
        debug_println("PIC timer A3");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_D);
        break;
    case PIC_GT_EXAMPLE_VECTOR(4):
        debug_println("PIC timer B0");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_E);
        break;
    case PIC_GT_EXAMPLE_VECTOR(5):
        debug_println("PIC timer B1");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_F);
        break;
    case PIC_GT_EXAMPLE_VECTOR(6):
        debug_println("PIC timer B2");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_G);
        break;
    case PIC_GT_EXAMPLE_VECTOR(7):
        debug_println("PIC timer B3");
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_H);
        break;
    case PIC_SPURIOUS_VECTOR_CODE:
        debug_println("spurious vector!");
        break;
    default:
        debug_println("unknown vector!");
        break;
    }

    pic_eoi_put();

    return true;
}

bool
tick_irq(void)
{
    machine_timer_clear();

    debug_println("tick_irq");

    rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_EVT_I);

    return true;
}

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;) ;
}

{{#rtos.tasks}}
void
{{function}}(void)
{
    debug_println("Task {{name}}");
    for (;;) {
        rtos_signal_wait(RTOS_SIGNAL_ID_DEMO_SIG);
        debug_println("{{name}}");
    }
}
{{/rtos.tasks}}

int
main(void)
{
    int i;

    /* This example system actively relies on debug prints for demonstration purposes.
     * We're going to assume that the p2020-util library's DUART1 debug print implementation is in use, and that this
     * initial invocation of debug_println calls duart1_init. */
    debug_println("Interrupt demux example");

    /* Set up a CPU-based timer interrupt source - these do not go through the PIC. */
    machine_timer_init();

    /* This code assumes the PIC init invocation has already been done by vectable.s, if it was needed.
     * Configure the PIC to deliver the DUART interrupt with the given priority and vector number. */
    pic_iiv_duart_init(PIC_IIV_DUART_EXAMPLE_PRIORITY, PIC_IIV_DUART_EXAMPLE_VECTOR);

    /* The p2020-util library uses DUART1 tx for debug output.
     * Use DUART1 and DUART2 rx as distinct interrupt sources. */
    duart2_init();
    duart2_rx_irq_init();
    /* Assume here that duart1_init has already been called by the initial debug print. */
    duart1_rx_irq_init();

    /* Use all 8 of the PIC's global timers as distinct interrupt sources. */
    for (i = 0; i < 8; i++) {
        pic_global_timer_init(i, PIC_GT_EXAMPLE_PRIORITY, PIC_GT_EXAMPLE_VECTOR(i),
                PIC_GT_EXAMPLE_BASE_COUNT * (i + 1));
    }

    rtos_start();

    for (;;) ;
}
