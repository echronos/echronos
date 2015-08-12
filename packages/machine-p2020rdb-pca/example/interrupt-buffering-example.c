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

#include <stdbool.h>
#include <string.h>
#include <p2020-util.h>

#include "p2020-duart.h"
#include "p2020-pic.h"
#include "machine-timer.h"

#include "rtos-{{variant}}.h"
#include "interrupt-buffering-example.h"
#include "debug.h"

#define PIC_IIV_DUART_EXAMPLE_PRIORITY 2
#define PIC_IIV_DUART_EXAMPLE_VECTOR 0xbeef

#define RX_BUF_OVERRUN_CHAR '#'

#define SPURIOUS_LIMIT 10
#define EXAMPLE_ERROR_ID_BUFFER_COUNT_OOB 0xfe
#define EXAMPLE_ERROR_ID_RX_FIFO_OVERRUN 0xff

extern void fatal(RtosErrorId error_id);
uint8_t rx_buf[BUF_CAPACITY];
volatile int rx_count;

bool
exti_duart_irq_handle(uint8_t iid)
{
    int original_rx_count;
    static int spurious_count;

    switch (iid) {
    /* THRE: Transmitter holding register empty */
    case DUART_IID_THRE:
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_TX);
        return true;

    /* CTO: Character time-out
     * RDA: Received data available */
    case DUART_IID_CTO:
    case DUART_IID_RDA:
        original_rx_count = rx_count;

        while (rx_count < BUF_CAPACITY && duart2_rx_ready()) {
            /* Treat DUART rx FIFO (hardware) capacity overrun as a fatal error.
             * It's not necessarily typical to do this in a production system, but if interrupts are arriving much
             * faster than even this interrupt handler can service them, then something is seriously wrong. */
            if (duart2_rx_overrun()) {
                debug_println("overrun! dumping FIFO for debug");
                while (duart2_rx_ready()) {
                    debug_putc(duart2_rx_get());
                }
                fatal(EXAMPLE_ERROR_ID_RX_FIFO_OVERRUN);
            }

            /* While there are bytes in the DUART rx FIFO, copy them to rx_buf[], the rx buffer shared between the
             * interrupt handler and Task A. */
            rx_buf[rx_count] = duart2_rx_get();
            rx_count++;
        }

        if (rx_count == BUF_CAPACITY) {
            /* Indicate rx_buf[] rx buffer capacity overrun with a special character. */
            rx_buf[BUF_CAPACITY - 1] = RX_BUF_OVERRUN_CHAR;
            /* Reset the FIFO and clear the receiver shift register. */
            duart2_rx_fifo_reset();
            duart2_rx_get();
        } else if (rx_count == original_rx_count) {
            /* No new bytes were buffered despite receiving the interrupt - this is pretty much unexpected. */
            debug_println("Spurious DUART RDA interrupt?");
            spurious_count++;
            if (spurious_count == SPURIOUS_LIMIT) {
                /* Disable the interrupt. */
                debug_println("Disabling DUART.");
                duart2_irq_disable();
            }
            return false;
        }
        spurious_count = 0;

        /* Wake up Task A */
        rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_RX);
        return true;

    default:
        debug_print("unexpected DUART iid! ");
        debug_printhex8(iid);
        debug_println("");
        break;
    }

    return false;
}

bool
exti_irq(void)
{
    uint32_t inc_vector;
    uint8_t iid;
    bool ret = false;

    /* This handler is responsible for clearing all external interrupt conditions before it returns */
    while ((inc_vector = pic_iack_get()) != PIC_SPURIOUS_VECTOR_CODE) {
        switch (inc_vector) {
        case PIC_IIV_DUART_EXAMPLE_VECTOR:
            /* There's no point returning unless all of the DUART interrupts have been cleared anyway */
            while (true) {
                iid = duart2_iid_get();
                if (iid == DUART_IID_NONE) {
                    break;
                }
                if (ret) {
                    exti_duart_irq_handle(iid);
                } else {
                    ret = exti_duart_irq_handle(iid);
                }
            }
            break;
        default:
            debug_print("unknown vector! ");
            debug_printhex32(inc_vector);
            debug_println("");
            break;
        }

        pic_eoi_put();
    }

    return ret;
}


void
interrupt_buffering_example_init(void)
{
    /* This code assumes the PIC init invocation has already been done by vectable.s, if it was needed.
     * Configure the PIC to deliver the DUART interrupt with the given priority and vector number. */
    pic_iiv_duart_init(PIC_IIV_DUART_EXAMPLE_PRIORITY, PIC_IIV_DUART_EXAMPLE_VECTOR);

    /* Set up DUART2 to be a source of input characters. */
    duart2_init();
    duart2_rx_irq_init();
}
