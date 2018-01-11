Reviewer: akroh (alex.kroh@unsw.edu.au)
Conclusion: Rework

General comments:

The MPU feature implementation is very elegant and well written.  
Besides a few nitpicks, the documentation is also quite clear and complete.  
The tests are a great demonstration of the new features, but it would be nice to show the result of protection
policy violations. Unfortunately, I acknowledge that the violation would be a reset event and is
hence difficult to demonstrate.

[sebastian.holzapfel: I agree that 'hard reset' violation behaviour is hard to demonstrate.
I could be misunderstanding, but I believe the tests already show the result of protection policy violations.
The acamar test uses a special debugging switch `mpu_skip_faulting_instructions`, which is implemented at:
    - `components/mpu-armv7m/implementation.c:382`
This injects a different implementation of the fault handling IRQ.
The replaced IRQ, instead of locking up with the RTOS `fatal` handler, will display debugging information indicating a protection fault occurred.
It will alter the stacked registers populated by the `memmanage` IRQ such that when the IRQ completes, execution is resumed and the faulting instruction is skipped.
It is easy to imagine situations where simply skipping a faulting instruction would cause some pretty nasty things to happen (in real applications) - so there are warnings about this flag in the documentation.]

The per-task protection domain limit of 8/16 could be extended by detecting memmanage faults and swapping
regions as needed (much like a TLB). I am curious if there is a demand for this, and how this would impact the
real-time properties of the RTOS. Perhaps this could be considered future work to avoid prolonging this review.

[sebastian.holzapfel: I am glad that you brought this up, as it is something that was discussed at the start of this year and is buried in my notes somewhere (nowhere official until now).
Such a feature would probably be useful, but concerns we had (some of which you have already mentioned) were:
    - Depending on the domain working set, there is potential for large performance overhead
    - It may prove difficult to reason about how this would affect the real-time properties of the RTOS
        - A pathological example would easily blow up the WCET between yields by an order of magnitude.
    - It was thought that an RTOS application (that consists of many protection domains & tasks) generally gains more from memory protection IF tasks are loosely coupled (as far as shared memory is concerned), and each task accesses a minimal amount of peripherals.
        - It becomes less likely for an invalid access (in a task that is overassociated with protection domains) to occur without error.
        - Allowing users to 'ignore' this practice more easily may not be a good idea.
        - Of course the more complex a task network becomes, the slower an application will be as well.
This is definitely worth investigating further.
For now, I would prefer we consider this future work.]

Location: various  
Comment 1: Rework  
The diff with master contains changes that do not seem relevant to this task.  
 - components/acamar/docs.md
 - components/preempt-null/docs.md
 - components/profiling/docs.md
 - components/task/docs.md
 - x.py

[sebastian.holzapfel: True, these changes are not directly referenced in the task description.
I have added scope for the documentation changes in the task description.
Justification:
    - Acamar had some holes in it's documentation support, which I filled in so that the documentation for this variant was able to be built. (Most of the 'new' file is just copied from another RTOS variant)
        - This was necessary as only this variant has memory protection support at this time, and documentation builds needed to be tested.
    - The rest of the changes in the files you describe are also victims of the above.
If this isn't satisfactory, let me know and I will split this into another task.]

Location: components/mpu-armv7m/docs.md:26  
Comment 2: Rework  
"Tasks may only read/write to their own stack."  
Is this sensible behaviour? Should this case result in a build error?  
What could such a task possibly execute?  
Perhaps the motivation for this design decision should be briefly provided.


Location: components/mpu-armv7m/docs.md:31  
Comment 3: Rework  
usermode -> user mode

[sebastian.holzapfel: fixed.]

Location: components/mpu-armv7m/docs.md:35  
Comment 4: Rework  
* The qualifier 'such' is unnecessary and can be removed.  
* This statement is quite ambiguous. Why is it standard practice? In what way is the device limited?  
* Regarding the functionality trade-off: what functionality and what is it being traded for?  
From the Cortex-M reference manual:  
 "Instructions must have read access as defined by the AP bits and XN clear for correct execution"  
This is also true for high-end Cortex-A processors, but I am not sure about other vendors.
In any case, you could argue that providing read permissions on execute segments by default will
ensure consistent default permissions across devices.


Location: components/mpu-armv7m/docs.md:41  
Comment 5: Rework  
* Can "pieces of data" be phrased more concretely? Variables? Arrays?  
* like a peripheral ->  such as an MMIO peripheral region


Location: components/mpu-armv7m/docs.md:65  
Comment 6: Rework  
* 'essentially' can be removed.


Location: components/mpu-armv7m/docs.md:84  
Comment 7: Rework  
* uart -> UART


Location: components/mpu-armv7m/docs.md:90  
Comment 8: Rework  
The "verbose" in mpu\_verbose\_faults sounds like it is only used for debugging. Is it required or optional when
using the MPU feature?  
 - If optional, please rephrase.
 - If required, is it possible to rename this feature, or separate debug code from the required code?


Location: components/mpu-armv7m/docs.md:91  
Comment 9: Rework  
Though I am unaware of the standard practice for this project, I feel that <machine>\_mpu.build is more
appropriate than <machine>.build\_mpu

Location: components/mpu-armv7m/docs.md:107  
Comment 10: Rework  
I recall that you personally had problems with third-party drivers. Are these problems limited to
driver-specific code?  
Perhaps you could s/drivers/software' to extend the scope of this statement.

Location: components/mpu-armv7m/docs.md:108  
Comment 11: Rework  
"is your friend" is very informal. What about "are useful tools for..."

Location: components/mpu-armv7m/docs.md:129  
Comment 12: Rework  
"by the system to avoid strange error messages when memory protection is enabled."  
It is unclear what a strange error message is. Please provide a specific example, or simply remove this clause.  
Also, I feel that developers may overlook this sentence and invest a significant amount of effort in trying to
resolve the error message. Perhaps this sentence should be in its own paragraph with keywords highlighted?


Location: components/stack-armv7m/implementation.c:17  
Comment 13: Rework  
on their size -> by stack size


Location: components/stack-armv7m/implementation.c:24  
Comment 14: Rework  
The argument to aligned() should be in bytes. Is stack\_size measured in words?


Location: packages/armv7m/ctxt-switch.s:13  
Comment 15: Rework  
This code seem unrelated to this task, is commented out, and should be removed.

Location: packages/armv7m/ctxt-switch.s:55  
Comment 16: Rework  
we must ensure that we -> we must  
The second part of this comment is a little confusing:  
 How does one context switch into a function?  
 First context switch with respect to? System boot?  
 "as we do here" suggests to me that the previous clause is not relevant here.  


Location: packages/armv7m/ctxt-switch.s:74  
Comment 17: Rework  
 The magic number "#1" should be a #define.  
 Ideally, this would be a list in a well-defined place to avoid aliasing... (out of scope?)


Location: packages/armv7m/vectable.py:28 and packages/armv7m/vectable.s:26  
Comment 18: Rework  
Is it possible to continue to use a default value for memmanage? Would this clean up the vector table code?


Location: packages/machine-stm32f4-discovery/build_mpu.py  
Comment 19: Rework  
This file is very similar to the corresponding build.py. Would it be much effort to factor out the common parts?


Location: packages/rtos-example/acamar-mpu-test.c:116/126  
Comment 20: Rework  
Is it possible to use task labels rather than magic numbers here?

Location: packages/rtos-example/acamar-mpu-test.c  
Comment 20: Rework  
Could the tests be extended to demonstrate a protection policy violation?

[sebastian.holzapfel: See response to first general comment above]
