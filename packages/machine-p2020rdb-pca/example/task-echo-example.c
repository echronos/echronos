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

#define RX_BUF_OVERRUN_CHAR '#'

#define SPURIOUS_LIMIT 10
#define EXAMPLE_ERROR_ID_BUFFER_COUNT_OOB 0xfe
#define EXAMPLE_ERROR_ID_RX_FIFO_OVERRUN 0xff

/* 16 bytes is the size of the DUART FIFOs.
 * But we can pick a totally arbitrary size for the buffer we use to pass bytes to Task A. */
extern uint8_t rx_buf[BUF_CAPACITY];
extern volatile unsigned int rx_count;

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;) ;
}

static void
tx_put_when_ready(const char c)
{
    while (!duart2_tx_ready()) {
        rtos_signal_wait(RTOS_SIGNAL_ID_TX);
    }
    duart2_tx_put(c);
}

/* This task simply functions as an echo server. */
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
            /* For demo purposes, manually insert a newline before every carriage return - for readability. */
            if (p_buf[i] == '\r') {
                tx_put_when_ready('\n');
            }
            tx_put_when_ready(p_buf[i]);
        }
    }
}

int
main(void)
{
    /* This example system uses DUART2 tx for the actual program output, and debug prints for error cases. */
    debug_println("Interrupt buffering example");

    /* We won't be using any CPU-based timer interrupt sources - disable any the bootloader may have set up. */
    machine_timer_deinit();

    interrupt_buffering_example_init();

    rtos_start();

    for (;;) ;
}
