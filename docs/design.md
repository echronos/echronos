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

The aim of this document is to capture any significant design decisions.

Note: Currently there is little structure.
Each decision should be tagged with the help of the `gen_tag.py` script from the QMS repository, and this tag can then be referred to in source code or commit messages.

`QtW5qV`: During the development process it has become clear that many design decisions are being captured within the commit messages.
While it is certainly possible to extract and use these it is not necessarily clear which design decision are currently relevant, and which may have been superseded.
Additionally there are times when the source code is delivered to third parties which will not necessarily have access to the commit log.
Thus, developers are strongly discouraged from following this practice.
Instead, design decisions should be documented in this document, from where they can be referenced via their tags in other documents, such as source code, commit message, or reviews.

`NQ50QD`: On the ARM platform it is possible to setup tasks so that no trampoline is required.
This was the first implementation approach used, and worked well for normal operation.
However, when debugging, this caused difficulties as a task's stack would continue to unwind in to other tasks.
By using this trampoline approach the over-zealous stack unwinding can be avoided.
With this approach the stack unwinds to the trampoline function, and then bottoms out correctly (although slightly inelegantly).
The trade-off is adding a single additional instruction vs. improved debugging experience.
In this case a choice is made in favour of improved debugging.

`nEAfHr`: On the ARMv7M platform we can not rely on a bootloader correctly loading an ELF file image in to memory.
As a consequence, when a system starts executing the state of DRAM is unknown.
A C program depends on the .bss region of DRAM being zero, and the .data region of DRAM holding the initialized values.
As there is no bootloader, the system should ensure that DRAM is zero, and .data is initialized on startup.
To achieve this, the design includes two parts; firstly the linker script is updated to provide relevant symbols: _bss_virt_addr,
_bss_size, _data_load_addr, _data_virt_addr, _data_size.
Additionally the linker script must ensure that the .data section is physically located within flash memory (by appropriately setting the load address differently to the virtual/run address).
On start up rather than jumping directly to `main`, an `_entry` stub is used which copies the .data section from the load address to the virtual address and then directly zeros the .bss section.
The current design implements this in assembly primarily due to the simplicity of the code.
It may be possible to consider implementing this in C as an alternative.

`YBys6d`: When a pystache template fails to render it can be difficult to debug.
Ideally an error is printed that references the location in the template at which the error occurred.
To achieve this it is necessary to attach some location meta-data to each part of the template that can cause an error.
Additionally, it is necessary to associate templates with their original filename.

`m8Oowb`: The debug.c module is designed to help system developers perform printf-style debugging.
A full `printf` implementation has an excessive code foot-print.
As an alternative some small, simple to use, functions are provided.
Currently the supported interfaces are debug_print, debug_println, and debug_printhex32.
These have been chosen primarily based on the need of internal developers.
A general purpose debug printing library may be considered in the future.

`Z6Wkvg`: On the ARMv7M platform the *-Os* compiler flag provides better code-generation that others such as *-O2* or *-O3*.
In general for the targeted systems code-size is generally more important than performance.
Although extra cycles may degrade battery life which is also important, this is generally mitigated by the fact that smaller code size has improved cache locality, so should avoid unnecessary loads from memory to cache.
Most ARMv7M platforms have some form of I-cache and/or flash memory accelerator.
Additionally, the code generated by -Os is normally easier to read, understand and debug.
For a specific system, the developer should benchmark and review the most appropriate flag, however at this stage -Os seems the best default.

`HbAcDx`: When a module provides header files it is likely that the header file may depend on the specific configuration of the module.
To support this `prj` is able to treat header files as templates which are then generated in a similar manner to how modules are rendered via templates (see `s3Txvx`).
A module can declare a header file as being a template by setting the `code_gen` attribute of the `header` element.
Currently only the `template` code generation method is supported.

`s3Txvc`: A module should be configurable during build.
To achieve the level of efficiency and flexibility desired, modules should be able to configured appropriately at system generation time.
This configuration could be done at either a binary level, or a source code level.
The approach taken is to perform this at a source level before a module is compiled.
This approach is taken to provide maximum flexibility and efficiency of the generated code.
The drawback is that it exposes the source code to the end customer which has some obvious drawbacks.
Two configuration approaches are used.
The first is using a templating approach.
The source code is marked up with suitable variable replacement regions.
The system designer provides the suitable replacements as part of the system configuration file.
Currently a simpler C preprocessor based approach is also supported, however that may be deprecated.

`EVIC4b`: RTOS should provide a signal module.
The signal module is an important low-level building block.
There are two primary operations signal_wait_set and signal_send_set.
`wait` blocks the calling task until any of the requested signals are available.
`send` sends one (or more) signals to another task.

`ECfEoV`: signal_wait_set should yield, even if there is a signal available.
When operating in a cooperative scheduler environment it is important for the application programmer to yield at appropriate times.
This can generally be achieved by either `yield` or `block`.
`wait` is effectively a mechanism for blocking on 1 or more inputs, if a signal is already available the function can return immediately.
A common pattern for an RTOS task will be `for (;;) { sig = signal_wait_set(); /* do stuff */ }`.
In this case, the task ideally wants to yield on each loop iteration.
Inserting an explicit yield is unnecessary if the signal wait blocked, so the task would have to do extra work.
Also, the task can not assume that blocking occurred, since the signal may have been sent before the wait.
To deal with this problem, the wait function itself should perform the yield in the case where blocking is not required.

`EnJmW2`: Waiting signal set stored on the stack during `signal_wait_set`
During a signal wait operation the signal set being waited must be stored.
This could be stored explicitly in a task control block, or it can simply reside on the stack.
Making this explicit has some advantages in terms of debugging as it is simpler to determine what signals a given task is waiting on.
Simply leaving the value on the stack results in simpler code, and reduces the data size.
Additionally, it enforces information hiding.


`mflnkA`: An RTOS needs some way for the *task world* and the *interrupt world* to interact.
Ideally the interface of interaction should be as narrow as possible to avoid the general problems associated with concurrent execution.
Specifically, shared data can lead to corruption if not carefully managed; locking can lead to dead-lock if not carefully managed; and locking (disabling interrupts) can have a negative impact on interrupt latency performance if not carefully managed.
The chosen mechanism is an *irq event* mechanism.
In simplest terms an interrupt handler is able to *raise* an irq event and the RTOS is able to *handle* an irq event in some manner.
The irq event component provides an `irq_event_get_next` function, which should be used by an RTOS in preference to `sched_get_next`.
`irq_event_get_next` should *process* any raised irq events, before calling the scheduler.
If the scheduler returns TASK_ID_NONE, then `irq_event_get_next` will (logically) wait until an irq event is raised by an interrupt at which
time irq events will be processed and the scheduler examined again.
Processing a irq event involves marking it as unraised, and then calling an appropriate handler function.
It is critical to the design correctness that the raise and the unraise operations are safe with respect to each other.
This can be achieved as long as the platform provides an atomic write and atomic read.
Note that atomic test-and-set is *not* required.
As the amount of data stored per irq event is 1 bit (raised/not raised), ideally the implementation will not use more than 1 bit per irq event.
This depends on architecture specific mechanisms for atomic bit set and clear.
The other critical aspect for correctness is that of waiting for an interrupt.
The architecture must provide a mechanism for atomically determining if any irq event has been raised, and sleeping.
A race could occur between the RTOS checking if an irq event has been raised, and waiting for an interrupt.
If an interrupt is fired and an irq event raised in between the check then the system could wait indefinitely before another interrupt is fired.
As much of the detail required for irq events in general is architecture specific, the main primary irq-event.c component only provides the main `irq_event_get_next` function and relies on the architecture specific implementation to provide `irq_event_process`, `irq_event_wait` and `irq_event_raise`.

`9jezHc`: The ARMv7M irq event module takes advantage of the ARMv7M bitband functionality to enable atomic bit manipulation.
An alternative approach to this would be to use a word-per-irq event, however this would increase data size significantly, and also increase the time for the irq_event_check.
To simplify the code a maximum of 32 irq events is supported.
This could be increased without too much rewrite, and the core design does not depend on this limitation.
By containing the irq events in a single word it is possible to determine if any irq events are pending with a single memory read.
There is no atomic mechanism for performing a memory read and check combined with a wait-for-interrupt.
Two options were considered; disabling interrupts combined with a wfi instruction, and avoiding interrupt disable by using the wfe instruction.
An approach using the wfi was chosen as the exact semantics of the wfe instruction were unclear from the documentation, and did not appear to be implemented on QEMU.
It may be possible to examine the possibility of using wfe as an optimisation in the future.
