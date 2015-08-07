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

lupHw1: All project (i.e., non-third-party) Python files shall be PEP8 compliant.
Rationale: consistent code style and improved readability.

u1wSS9: The command 'x.py test style' shall check for compliance of all project Python files.
Rationale: an automated check allows to detect and resolve non-compliance efficiently.

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
Other tenses are only acceptable where they are necessary grammatically for clarity.

BXCwte: All C code must observe const correctness.
Const correctness is described in more detail online in Wikipedia and in the C++ FQA lite.
In the RTOS C code, all function arguments not modified in the scope of the function must be marked with the const keyword in the function definition.
Function arguments that are modified within the scope of the function must not be marked as const.
By-value arguments must not be marked as const in the function declaration.
For example for the function definition `int foo(const int *const bar) { return *bar; }`, the declaration `int foo(const int *const bar);` is invalid whereas `int foo(const int *bar);` is the correct declaration.

r62xUL: All C code in architecture-independent components must be compatible with the ISO9899:1990 / C90 / ANSI C standard.
This is necessary so that the RTOS can be built with C compilers that do not support later C standards.
Architecture specific code, such as the interrupt or timer components and modules for specific target platforms, may use features of later C standards and compiler extensions.
