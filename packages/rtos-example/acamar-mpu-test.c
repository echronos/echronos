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

#include <stddef.h>
#include <stdint.h>

#include "rtos-acamar.h"
#include "debug.h"

extern void debug_println(const char *msg);

void fn_a(void);
void fn_b(void);
void fatal(RtosErrorId error_id);

uint32_t dom1_variable = 0;
uint32_t dom2_variable = 0;
uint32_t *dom1_stack_variable_addr = NULL;
uint32_t un_owned_global = 0;

extern uint8_t rtos_internal_current_task;

#define REGISTER(x) (*(uint32_t*)(x))

void nmi() { for(;;); }
void hardfault() { for(;;); }
void busfault() { for(;;); }
void usagefault() { for(;;); }

void
fatal(const RtosErrorId error_id)
{
    debug_print("FATAL ERROR: ");
    debug_printhex32(error_id);
    debug_println("");
    for (;;)
    {
    }
}

void
iteration() {
    debug_print("current task (API): ");
    debug_printhex32(rtos_task_current());
    debug_println("");

    /* One of these increments should fault depending on
     * which task is currently executing */

    debug_println("increment dom1_variable...");
    ++dom1_variable;

    debug_println("increment dom2_variable...");
    ++dom2_variable;

    /* Both of these reads should succeed, both tasks have read access */

    debug_print("dom1_variable=");
    debug_printhex32(dom1_variable);
    debug_println("");

    debug_print("dom2_variable=");
    debug_printhex32(dom2_variable);
    debug_println("");

    uint32_t x = 0;

    debug_println("increment x (on my stack)...");
    ++x;

    /* Next 2 tests should fault in task 1, but not task 0 */

    debug_println("increment variable on stack of task 0...");
    ++(*dom1_stack_variable_addr);

    debug_print("*dom1_stack_variable_addr=");
    debug_printhex32(*dom1_stack_variable_addr);
    debug_println("");

    /* These should all fault */

    debug_println("access un_owned_global...");
    x = un_owned_global;

    debug_println("access kernel data...");
    x = rtos_internal_current_task;

    debug_println("write to peripheral address...");
    REGISTER(0x400000ff) = 42;
}

void
fn_a(void)
{
    /* store the address of a variable on stack of task 0 */
    uint32_t x = 0;
    dom1_stack_variable_addr = &x;

    debug_print("dom1_stack_variable_addr=");
    debug_printhex32((uint32_t)dom1_stack_variable_addr);
    debug_println("");

    for (;;)
    {
        iteration();
        rtos_yield_to(1);
    }
}

void
fn_b(void)
{
    for (;;)
    {
        iteration();
        rtos_yield_to(0);
    }
}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
