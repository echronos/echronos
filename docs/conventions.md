<!--
     eChronos Real-Time Operating System
     Copyright (c) 2017, Commonwealth Scientific and Industrial Research
     Organisation (CSIRO) ABN 41 687 119 230.

     All rights reserved. CSIRO is willing to grant you a licence to the eChronos
     real-time operating system under the terms of the CSIRO_BSD_MIT license. See
     the file "LICENSE_CSIRO_BSD_MIT.txt" for details.

     @TAG(CSIRO_BSD_MIT)
-->

lupHw1: All project (i.e., non-third-party) Python files shall be PEP8 compliant.
Rationale: consistent code style and improved readability.

u1wSS9: The command 'x.py test style' shall check for compliance of all project Python files.
Rationale: an automated check allows to detect and resolve non-compliance efficiently.

05K0tk: In plain-text and markdown files, each sentence shall occupy exactly one line.
Rationale: simplifies file handling with line-oriented tools, such as diff and patch.

TZb0Uv: The maximum line length in project Python files is 118 characters.
Rationale: a maximum line length allows for convenient source code handling with standard text management tools on the command line.
Since an 80-character limit is considered overly restrictive, 120 characters are a viable compromise on modern displays.
A reduction by 2 to 118 characters allows to manage indented source code (e.g., by diff) on 120-character wide displays.

tGT1n0: In docstrings and comments of project Python files, each sentence shall start on a new line.
Rationale: simplifies file handling with line-oriented tools, such as diff (same rationale as for plain text files).

g0O2QN: RTOS changes visible to application developers, in particular changes to RTOS concepts, APIs, or configuration items, always need to be accompanied by the corresponding updates to their documentation.
Rationale: helps to keep the documentation up-to-date with the implementation.

eIhEVe: All text in the repository shall use American English spelling and grammar (as opposed to British or Australian spelling or grammar).

29g3DU: The following convention should be used for the naming of symbols in component-specific RTOS code:
1. Symbols in component-specific code implemented in the component itself: <component-name>_<functionality>.
2. Symbols in component-specific code required by the component and implemented in the variant: <component-name>_core_<functionality>.

2g8PAE: Specific API and internal assertions are not considered API features.
Instead, they are considered implementation details that may change any time.
Specific API assertions and internal assertions are therefore not to be documented in the RTOS manual.

SKcASp: All documentation uses present tense.
Other tenses are only acceptable where they are grammatically necessary for clarity.

BXCwte: All C code must observe const correctness.
Const correctness is described in more detail online in Wikipedia and in the C++ FQA lite.
In the RTOS C code, all function arguments not modified in the scope of the function must be marked with the const keyword in the function definition.
Function arguments that are modified within the scope of the function must not be marked as const.
By-value arguments must not be marked as const in the function declaration.
For example for the function definition `int foo(const int *const bar) { return *bar; }`, the declaration `int foo(const int *const bar);` is invalid whereas `int foo(const int *bar);` is the correct declaration.

r62xUL: All C code in architecture-independent components must be compatible with the ISO9899:1990 / C90 / ANSI C standard.
This is necessary so that the RTOS can be built with C compilers that do not support later C standards.
Architecture specific code, such as the interrupt or timer components and modules for specific target platforms, may use features of later C standards and compiler extensions.

3zAfAI: The term 'eChronos' shall not be used in documentation and code unless necessary for disambiguation or marketing purposes.
To avoid any misuse of the term, it shall not be used at all in documentation and code (instead, 'the RTOS' or a similar term may be used).
Rationale: to use eChronos as an unregistered trademark, it shall be used as an adjective, not a noun (see task Z2CK2B-echronos_as_unregistered_trademark).

09aou0: The Python code of the project shall be compatible with the two most recent stable versions of Python 3.
Rationale: The Python project officially supports the two most recent stable versions of Python 3, in particular by providing security updates.
Users of the eChronos RTOS therefore have a vested interest in using these Python versions, so the project shall support them.
Not needing to support older Python versions reduces the maintenance burden on the eChronos RTOS.
Regression tests may additionally target older Python versions to make incompatibilities visible, but only if this can be achieved with little overhead on the regression test setups.
