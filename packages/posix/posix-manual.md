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

%title POSIX package: user manual
%version 1-draft
%docid H8LKmc

Introduction
-------------

The POSIX package provides the following modules:

<dl>
  <dt>`build`</dt>
  <dd>A build module.</dd>

  <dt>`debug`</dt>
  <dd>A module that provides the *low-level debug* interface.</dd>
</dl>

Additionally an example system `hello` is provided, which is a standard *Hello, world* program.

The build module allows systems that are targeted at a POSIX environment.
Systems here run as a process on a POSIX operating system, rather than on a real machine.
Such an environment can enable prototyping and debugging of some system aspects without requiring a real machine.
As the environment does not require any upload or flashing step development can progress much more quickly.

`posix/build`
==============

The build module supports the *system build* interface.
The module does not support any options.

`posix/debug`
==============

The debug module supports the *low-level debug* interface.
It provides the `debug_putc` function.

### `debug_putc`

    void debug_putc(char c)

The function outputs a single ASCII character to the machine's debug console.
The function should act in a synchronous manner; when the function returns the character should be visible, and not buffered.
If the function can not successfully perform the operation it should abort execution.

The debug console for the POSIX environment is the *standard output*, which is normally the current terminal.
