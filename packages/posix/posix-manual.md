<!--
     eChronos Real-Time Operating System
     Copyright (c) 2017, Commonwealth Scientific and Industrial Research
     Organisation (CSIRO) ABN 41 687 119 230.

     All rights reserved. CSIRO is willing to grant you a licence to the eChronos
     real-time operating system under the terms of the CSIRO_BSD_MIT license. See
     the file "LICENSE_CSIRO_BSD_MIT.txt" for details.

     @TAG(CSIRO_BSD_MIT)
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
