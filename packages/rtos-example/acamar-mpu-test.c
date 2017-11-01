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
iteration()
{
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
