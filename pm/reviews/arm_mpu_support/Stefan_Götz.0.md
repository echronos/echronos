Reviewer: Stefan Götz (stg@brkawy.com)
Conclusion: Rework

This is a non-technical review for two reasons:
1. From all I can tell, the technical realization of the feature is excellent.
2. My knowledge about ARM platforms and the MP support in particular is very limited.
So despite all the nit picking, the changes look fantastic overall.

Location: components/mpu-armv7m/docs.md:20 and following
Comment 1: Rework
The document uses the future tense in a number of cases.
For consistency with the rest of the documentation and grammatical correctness, please generally use present tense.
Future tense should only be used where necessary for clarity or correctness.

[sebastian.holzapfel: Fixed up tenses for consistency]

Location: components/mpu-armv7m/docs.md:40
Comment 2: Rework
I believe that the phrase "as a way of bookkeeping memory regions" is not quite correct.
Since "bookkeep" is not a verb, how about "as a way of bookkeeping for memory regions"?

[sebastian.holzapfel: rephrased as required]

Location: components/mpu-armv7m/docs.md:41 and following
Comment 3: Rework
I find the use of italics in this section a bit excessive.
For example, I don't understand the benefit of putting "or" and "different permissions" in italics.
Although clearly a matter of preference, how about limiting italic text to quotes and newly introduced or unusual terms?

[sebastian.holzapfel: reduced excessive italics usage in some areas]

Location: components/mpu-armv7m/docs.md:51
Comment 4: Rework
Does "it's" need to be "its" instead?

[sebastian.holzapfel: fixed]

Location: components/mpu-armv7m/docs.md:57
Comment 5: Rework
This sentence contains two main clauses.
Please separate them with a full stop.

[sebastian.holzapfel: fixed]

Location: components/mpu-armv7m/docs.md:69
Comment 6: Rework
How about replacing "there is a limitation on the number of protection regions" with "the number of protection regions is limited"?

[sebastian.holzapfel: good point, fixed]

Location: components/mpu-armv7m/docs.md:70
Comment 7: Rework
Something appears to be missing on this line and the following list to make this a complete sentence.

[sebastian.holzapfel: rephrased as required]

Location: components/mpu-armv7m/docs.md:94
Comment 8: Rework
"its" instead of "it's"?

[sebastian.holzapfel: fixed]

Location: components/mpu-armv7m/docs.md:112
Comment 9: Rework
Missing full stop at end of line.

[sebastian.holzapfel: fixed]

Location: components/mpu-armv7m/docs.md:132
Comment 10: Rework
The sentence on this line contains no main clause.

[sebastian.holzapfel: fixed]

Location: components/mpu-armv7m/docs.md:132 and following
Comment 11: Rework
To improve consistency with the rest of the documentation, please avoid the use of 'we'.
This often improves clarity because the identify of 'we' can be ambiguous.

[sebastian.holzapfel: this was fixed when switching to present tense earlier]

Location: components/mpu-armv7m/docs.md:161
Comment 12: Rework
How about "This is an optional configuration item" instead of "This is not a mandatory configuration item"?

[sebastian.holzapfel: good point, fixed]

Location: components/mpu-armv7m/docs.md:166
Comment 13: Rework
Full stop missing at end of line.

[sebastian.holzapfel: fixed]

Location: components/mpu-armv7m/implementation.c:13
Comment 14: Rework
Please surround all macro values that are compound expressions with parentheses.
This avoids surprises in operator precedence when using such macros and is a convention aimed for by the rest of the code base.

[sebastian.holzapfel: good point, fixed in multiple places]

Location: components/mpu-armv7m/implementation.c:25 and 138-140
Comment 15: Rework
Please prefix macro names, where sensible, with "MPU_" to indicate that they originate from the MPU component.
Although not a rigid convention, many components follow this pattern.

[sebastian.holzapfel: the macros I did not prefix with mpu_ have potential uses outside the mpu implementation, but I agree that keeping the naming convention is more important so it's obvious where things reside.
Fixed by prepending the offending macros with MPU_ or mpu_ accordingly]

Location: components/mpu-armv7m/implementation.c:77, 83 and following
Comment 16: Rework
For readability, please use a consistent line-break and indentation style.
Most RTOS code places an opening brace on a separate line, starting at no indentation.

[sebastian.holzapfel: good point, missed that.
I also fixed up the bracing style used by inline assembly statements.]

Location: components/mpu-armv7m/implementation.c:126 following
Comment 17: Rework
For consistency with the rest of the code base, by-value arguments in function declarations shall have no const qualifiers, even if they do in the corresponding function definition.

[sebastian.holzapfel: missed that, fixed.
also added some prototypes that I somehow missed]

Location: components/mpu-armv7m/implementation.c:188 and following
Comment 18: Rework
Feel free to use the full line length of 118 characters where possible.

[sebastian.holzapfel: good point.
Fixed by collapsing unnecessarily sparse lines onto a single line where appropriate.]

Location: components/mpu-armv7m/implementation.c:339 following
Comment 19: Rework
For consistency, how about prefixing this and other public non-API functions with `rtos_internal_`?

[sebastian.holzapfel: good point, done.
Also fixed the naming consistency of mpu_handle_fault while I was here.]

Location: components/mpu-armv7m/implementation.c:368
Comment 20: Rework
Remove extra space before assignment operator.
Maybe check for other extra whitespace in this and other added files.

[sebastian.holzapfel: fixed.
Also searched for excessive spaces before operators in other added files.]

Location: components/mpu-armv7m/implementation.c:388
Comment 21: Rework
Remove empty line.
Maybe check for similar patterns in this and other added files.

[sebastian.holzapfel: removed extra empty lines, looked at all added files.]

Location: components/mpu-armv7m/schema.xml
Comment 22: Accepted
How about giving at least the top-level configuration items a common prefix?
For example: mpu_enabled, mpu_verbose_faults, mpu_skip_faulting_instructions, etc.

[sebastian.holzapfel: I agree that this is a worthwhile improvement.
I have given all top-level configuration items a common prefix.]

Location: packages/machine-stm32f4-discovery/build_mpu.py:2 and other files
Comment 23: Rework
For all new files, please update the copyright year to 2017.

[sebastian.holzapfel: Done.
Should probably fix all the project licenses to refer to Data61 instead of NICTA as well.
I'll leave that to a future task]

Location: README.md:63
Comment 24: Rework
Please update the README so it no longer states that the RTOS is intended for devices without memory protection.
Maybe "without memory management units and virtual memory support" or something along those lines would be more appropriate?

[sebastian.holzapfel: fixed.]

Location: components/mpu-armv7m/docs.md or other location
Comment 25: Rework
Could you add documentation for system designers that provides basic instructions on how to add MPU support to a system?
Something along the lines of how to upgrade a regular acamar system to an MPU-enabled acamar system?
So this would be not so much about the details of the hardware, concepts, configuration options, API, etc. but how to tie them all together.

[sebastian.holzapfel: Added a section to the documentation on upgrading an existing system]

Location: N/A
Comment 26: Rework
How about adding a system test for the MPU support for regression testing?
Would that be possible with reasonable effort?

[sebastian.holzapfel: I would like to, but this is not possible with reasonable effort at the moment.
When I created this patch, mainline qemu didn't have MPU support yet.
As of a couple of weeks ago it now does, BUT:
- It will take another few months for these changes to propogate to the stm32-qemu fork we're using.
    - The entire VIC was rewritten so it's nontrivial to just cherry pick the changes on top.
- The cortex-M MPU support is currently mostly copying the cortex-R support.
    - This means that the minimum region size is 1K, which is incorrect and makes it almost impossible to run a hardware-accurate system.
Thus, I would prefer delaying this to a future task as it will be much easier in a few months.]
