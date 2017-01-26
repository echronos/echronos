/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
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

/* This file defines the interrupt handler for the `interrupt-buffering-example` and `task-sync-example` systems.
 * The main purpose of this code is to demonstrate one possible way to buffer data from an interrupt handler to a task.
 * Please see `machine-p2020rdb-pca-manual.md` for more information. */

#define PIC_IIV_DUART_EXAMPLE_PRIORITY 2
#define PIC_IIV_DUART_EXAMPLE_VECTOR 0xbeef

#define RX_BUF_OVERRUN_CHAR '#'

#define EXAMPLE_ERROR_ID_RX_SPURIOUS_RDA 0xfd
#define EXAMPLE_ERROR_ID_RX_FIFO_OVERRUN 0xff

extern void fatal(RtosErrorId error_id);
uint8_t rx_buf[BUF_CAPACITY];
volatile unsigned int rx_count;

/* Helper function that handles just P2020 DUART interrupts.
 * For demo purposes, we append incoming data to a buffer, and raise an interrupt event to a task to indicate that
 * data is ready for collection from the buffer.
 * Note that the task is responsible for using some platform-specific method for synchronizing its access to this
 * buffer with that of this interrupt handler.
 * This function also raises a distinct interrupt event to indicate when the DUART is ready to transmit data. */
bool
exti_duart_interrupt_handle(const uint8_t iid)
{
    unsigned int original_rx_count;

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
             * interrupt handler and the task. */
            rx_buf[rx_count] = duart2_rx_get();
            rx_count++;
        }

        if (rx_count == BUF_CAPACITY) {
            /* Indicate rx_buf[] capacity overrun with its own signal and let the task decide what to do with it. */
            rtos_interrupt_event_raise(RTOS_INTERRUPT_EVENT_ID_RX_OVERRUN);

            /* Reset the FIFO and clear the receiver shift register. */
            duart2_rx_fifo_reset();
            duart2_rx_get();
        } else if (rx_count == original_rx_count) {
            /* No new bytes were buffered despite receiving the interrupt - this is pretty much unexpected. */
            debug_println("Spurious DUART RDA interrupt?");

            /* Disable the interrupt. */
            debug_println("Disabling DUART.");
            duart2_interrupt_disable();

            fatal(EXAMPLE_ERROR_ID_RX_SPURIOUS_RDA);
        }

        /* Wake up the task */
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

/* Handler for external interrupts.
 * On the P2020, the various external interrupt sources are multiplexed onto the one external interrupt vector by a
 * Programmable Interrupt Controller (PIC).
 * For demo purposes, this particular handler only cares about DUART interrupts - it identifies them as such by
 * querying the P2020 PIC and calls a helper function to handle them. */
bool
exti_interrupt(void)
{
    uint32_t inc_vector;
    uint8_t iid;
    bool ret = false;

    /* This handler is responsible for clearing all external interrupt conditions before it returns */
    while ((inc_vector = pic_iack_get()) != PIC_SPURIOUS_VECTOR_CODE) {
        switch (inc_vector) {
        case PIC_IIV_DUART_EXAMPLE_VECTOR:
            /* There's no point returning unless all of the DUART interrupts have been cleared anyway */
            while ((iid = duart2_iid_get()) != DUART_IID_NONE) {
                if (ret) {
                    exti_duart_interrupt_handle(iid);
                } else {
                    ret = exti_duart_interrupt_handle(iid);
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

/* Invoke library code to initialize the P2020 for interrupt-driven receipt of data from DUART2 via the PIC. */
void
interrupt_buffering_example_init(void)
{
    /* This code assumes the PIC init invocation has already been done by vectable.s, if it was needed.
     * Configure the PIC to deliver the DUART interrupt with the given priority and vector number. */
    pic_iiv_duart_init(PIC_IIV_DUART_EXAMPLE_PRIORITY, PIC_IIV_DUART_EXAMPLE_VECTOR);

    /* Set up DUART2 to be a source of input characters. */
    duart2_init();
    duart2_rx_interrupt_init();
}
