<!---
eChronos Real-Time Operating System
Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, version 3, provided that these additional
terms apply under section 7:

  No right, title or interest in or to any trade mark, service mark, logo
  or trade name of of National ICT Australia Limited, ABN 62 102 206 173
  ("NICTA") or its licensors is granted. Modified versions of the Program
  must be plainly marked as such, and must not be distributed using
  "eChronos" as a trade mark or product name, or misrepresented as being
  the original Program.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@TAG(NICTA_DOC_AGPL)
  -->

*Does the AGPL license also apply to programs that use the eChronos RTOS kernel?*

Yes.
The license also covers user programs that use the eChronos RTOS services through API calls.
Linking the eChronos RTOS statically or dynamically with other modules is considered making a combined work based on the eChronos RTOS.
Thus, the terms and  conditions of the AGPLv3 cover the whole combination (see: http://www.gnu.org/licenses/gpl-faq.html#GPLStaticVsDynamic).

We specifically do not provide a FreeRTOS-like exception to the license.
