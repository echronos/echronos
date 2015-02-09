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
