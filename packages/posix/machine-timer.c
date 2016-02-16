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

/*<module>
    <code_gen>template</code_gen>
    <headers>
        <header path="../rtos-example/machine-timer.h" code_gen="template" />
    </headers>
</module>*/

#include <signal.h>
#include <unistd.h>
#include "machine-timer.h"

#define UALARM_MILLISECOND (1000U)
#define TICK_DURATION (100U * UALARM_MILLISECOND)

static void sigalrm_handler(int sig);

static void
sigalrm_handler(__attribute__((unused)) const int sig)
{
    machine_timer_tick_isr();
}

void
machine_timer_start(void)
{
    signal(SIGALRM, sigalrm_handler);
    ualarm(TICK_DURATION, TICK_DURATION);
}

void
machine_timer_stop(void)
{
    ualarm(0, 0);
}

void
machine_timer_tick_isr(void)
{
    application_tick_isr();
}
