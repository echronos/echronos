void
machine_timer_clear(void) {
    asm volatile(
        /* Write-1-to-clear:
         *   In TSR (timer status register)
         *     TSR[FIS] (fixed-interval timer interrupt status) */
        "lis %%r3,0x400\n"
        "mttsr %%r3\n"
        ::: "r3");
}

void
machine_timer_init(void) {
    /*
     * Configure a fixed interval timer
     * Enable:
     *  In TCR (timer control register)
     *    TCR[FIE] (fixed-interval interrupt enable)
     *  In MSR (machine state register)
     *    MSR[EE] (external enable) -> Note: this also enables other async interrupts
     *
     * Set period: TCR[FPEXT] || TCR[FP]
     */
    asm volatile(
        "mftcr %%r3\n"
        "oris %%r3,%%r3,0x380\n" /* 0x300 = TCR[FP], 0x80 = TCR[FIE] */
        "mttcr %%r3"
        ::: "r3");
}
