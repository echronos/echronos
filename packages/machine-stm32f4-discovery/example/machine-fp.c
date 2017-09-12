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

void
machine_fp_init(void)
{
    /* This is the example code given in the Cortex-M4 Devices Generic User Guide */
    __asm volatile(
        "ldr.w r0, =0xe000ed88\n"
        "ldr r1, [r0]\n"
        "orr r1, r1, #(0xf << 20)\n"
        "str r1, [r0]\n"
        "dsb\n"
        "isb\n");
}

void
machine_fp_test_zero_nonfp_regs(void)
{
    /* Some of these are nonvolatile, and zeroing them is rather pointless because the compiler is supposed to
     * preserve them due to their being in the clobber list, but do it anyway to make sure the compiler's also sane. */
    asm volatile(
            "mov r0, #0\n"
            "mov r1, #0\n"
            "mov r2, #0\n"
            "mov r3, #0\n"
            "mov r4, #0\n"
            "mov r5, #0\n"
            "mov r6, #0\n"
            "mov r7, #0\n"
            "mov r8, #0\n"
            "mov r9, #0\n"
            "mov r10, #0\n"
            "mov r11, #0\n"
            "mov r12, #0"
            ::: "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12");
}
