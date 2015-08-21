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

%title Generic package: user manual
%version 1-draft
%docid SEekKp

Introduction
-------------

The generic package provides the following modules:

<dl>
  <dt>`debug`</dt>
  <dd>A module providing basic debug functionality.</dd>

  <dt>`hello`</dt>
  <dd>A module that does the standard *Hello, world*.</dd>
</dl>


`generic/debug`
==============

The debug module supports the *low-level debug* interface.
It provides the `debug_println` function.
It depend on a module that provides the *low-level debug* interface.

### `debug_println`

    void debug_println(char *msg)

The function outputs an single ASCII string followed by a newline character to the machine's debug console.
The function should act in a synchronous manner; when the function returns the message should be visible, and not buffered.
If the function can not successfully perform the operation it should abort execution.


`generic/hello`
==============

A very simple module that simply prints "Hello, world" to the console.
