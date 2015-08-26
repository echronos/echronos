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

/* This file defines a pair of tasks for the `task-sync-example` system.
 * The main purpose of this code is to demonstrate one possible way of synchronizing access to a data structure shared
 * between two tasks, using some choice of API offered by the RTOS.
 * Please see `machine-p2020rdb-pca-manual.md` for more information. */

#define MSG_SIZE 42

#define EXAMPLE_ERROR_ID_RX_BUF_OVERRUN 0xfc
#define EXAMPLE_ERROR_ID_BUFFER_COUNT_OOB 0xfe

/* 16 bytes is the size of the DUART FIFOs.
 * But we can pick a totally arbitrary size for each of the buffers we use:
 * - to pass bytes from the interrupt handler to Task A. (BUF_CAPACITY)
 * - to pass bytes from Task A to Task B. (MSG_SIZE) */
extern uint8_t rx_buf[BUF_CAPACITY];
extern volatile unsigned int rx_count;
uint8_t msg_buf[MSG_SIZE];
unsigned int msg_len;

/* Fatal error function provided for debugging purposes. */
void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    /* Disable interrupts */
    asm volatile("wrteei 0");
    for (;;) ;
}

/* Helper function that just waits until DUART2 is ready to transmit, then transmits the given character. */
static void
tx_put_when_ready(const char c)
{
    while (!duart2_tx_ready()) {
        rtos_signal_wait(RTOS_SIGNAL_ID_TX);
    }
    duart2_tx_put(c);
}

/* This task waits for a "message"-sized chunk of bytes from DUART2 to accumulate, then forwards it to Task B. */
void
fn_a(void)
{
    debug_println("Task A");

    duart2_tx_interrupt_init();

    for (;;) {
        unsigned int i, p_count;
        uint8_t p_buf[BUF_CAPACITY];

        rtos_signal_wait(RTOS_SIGNAL_ID_RX);

        /* Tasks accessing data concurrently with interrupt handlers are responsible for synchronizing access to those
         * data structures in a platform-specific way.
         * In this example system, we choose to synchronize access to rx_buf[] by disabling interrupts by unsetting
         * MSR[EE] with the "wrteei" instruction.
         * We limit this window to as short a time as possible by using it only to clear rx_buf[] by copying its
         * contents to an intermediate buffer p_buf[]. */

        /* Disable interrupts */
        asm volatile("wrteei 0");

        if (rx_count > BUF_CAPACITY) {
            fatal(EXAMPLE_ERROR_ID_BUFFER_COUNT_OOB);
        }
        for (i = 0; i < rx_count; i++) {
            p_buf[i] = rx_buf[i];
        }
        p_count = rx_count;
        rx_count = 0;

        /* Enable interrupts */
        asm volatile("wrteei 1");

        for (i = 0; i < p_count; i++) {
            /* Drop newline characters, expect carriage-returns to delimit non-zero-length messages */
            if (p_buf[i] != '\n' && p_buf[i] != '\r') {
                msg_buf[msg_len] = p_buf[i];
                msg_len++;
            }
            if (msg_len == MSG_SIZE || (p_buf[i] == '\r' && msg_len != 0)) {
                rtos_signal_send(RTOS_TASK_ID_B, RTOS_SIGNAL_ID_RX);
                rtos_signal_wait(RTOS_SIGNAL_ID_TX);
                msg_len = 0;
            }
        }

        /* We'll choose here to panic in the case of a rx buffer capacity overrun. */
        if (rtos_signal_poll(RTOS_SIGNAL_ID_RX_OVERRUN) != RTOS_SIGNAL_SET_EMPTY) {
            debug_println("rx overrun!");
            fatal(EXAMPLE_ERROR_ID_RX_BUF_OVERRUN);
        }
    }
}

/* This task takes a message-sized "chunk" of bytes and transmits it via DUART2 in reverse order. */
void
fn_b(void)
{
    unsigned int i;

    debug_println("Task B");

    for (;;) {
        rtos_signal_wait(RTOS_SIGNAL_ID_RX);

        for (i = 0; i < msg_len; i++) {
            tx_put_when_ready(msg_buf[(msg_len - 1) - i]);
        }

        rtos_signal_send(RTOS_TASK_ID_A, RTOS_SIGNAL_ID_TX);

        tx_put_when_ready('\n');
        tx_put_when_ready('\r');
    }
}

/* We invoke a helper to initialize the P2020 DUART and PIC before starting the RTOS. */
int
main(void)
{
    /* This example system uses DUART2 tx for the actual program output, and debug prints for error cases. */
    debug_println("Task sync example");

    /* We won't be using any CPU-based timer interrupt sources - disable any the bootloader may have set up. */
    machine_timer_deinit();

    /* Invoke helpers to set up the buffering interrupt handler for DUART rx. */
    interrupt_buffering_example_init();

    rtos_start();

    for (;;) ;
}
