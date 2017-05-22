Reviewer: Stefan Götz (stg@brkawy.com)
Conclusion: Rework

Location: README.md:49
Comment 1: Accepted
Cloning the full repository takes fairly long and is not necessary for non-developers.
How about adding the option `--depth=1` so that git only retrieves the latest revision of the master branch?

[sebastian.holzapfel: Good point, done.]

Location: README.md:55
Comment 2: Accepted
The _prerequisites_ section itself and it being referenced from the Quick-Start guide is somewhat confusing.
In particular, it is unclear which prerequisites are required for what.
Although this is unrelated to your task, would it make sense to just merge the two sections and drop pandoc and wkhtmltopdf as a prerequisites?

[sebastian.holzapfel: I agree.
I've moved the _prerequisites_ sections into subsections closer to their examples and labelled which are required to run which examples.
Also updated the task description and moved the documentation building prerequisites into the _documentation_ section. ]

Location: README.md:110 & 118
Comment 3: Rework
At least on Ubuntu 17.04, the package is called `gdb-arm-none-eabi` instead of `arm-none-eabi-gdb`.
If older versions use the other name, it's worth mentioning.

[sebastian.holzapfel: Good catch.
I have fixed the wrong package name and added a sentence about the package naming.
I have not altered the package name in the dpkg redirection as it needs to remain-as is for the diversion to work]

Location: README.md:63
Comment 4: Rework
This fails on systems without a GUI, e.g., on default Ubuntu server installations.
I think it is worth either documenting that dependency or removing it.

[sebastian.holzapfel: This is a good point, however it will unfortunately be nontrivial to remove the dependency.
Qemu does have a `--no-sdl` flag, however this cortex-M4 fork has lots of additional SDL code that isn't contained in configuration #ifdefs for visual board emulation.
I will work toward making this dependency a compiler flag, and update the QEMU fork with instructions in due course.
For this task I have documented the dependency in the QEMU fork readme, _prerequisites_ section.]

Location: README.md:95
Comment 5: Rework
When I ran the Kochab system, the GDB output look quite different from what is documented in the README.
It seems that the Kochab test system behaves quite differently than what is documented at the moment.
Can you reproduce that?

[sebastian.holzapfel: As we discussed, PPC QEMU behaviour is out of scope for this task.
However, testing Kochab on ARM QEMU did surface a probable bug in QEMU's implementation of the ARM NVIC.
I have spent a few hours trying to iron out this bug to no avail.
For now, I have documented that preemptive RTOS variants are unsupported in ARM QEMU in README.md.
I believe this is okay because:
    - Preemptive RTOS variants were not working on ARM QEMU before this patch.
    - Fixing this bug will mostly involve changes in the QEMU repository (with a minor README update here)
So, support for preemptive RTOS variants will be added in a future patch.
UPDATE: The bug was found. Section stating preemptive variants are unsupported has been removed from the README.]
