Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Looks good to me! I only tried it on my Windows VM and did not check gdb.

In my opinion one problem with the current README is that it is very long and
the structure is not very clear. A table of contents with links to the individual
sections might help with this. Even if it is just the toplevel sections as in

* Overview
* POSIX Quick-Start
* ARMv7m Quick-Start
* PowerPC e500 Quick-Start
* Documentation
* Software Model
* Repository Contents
* Common Development Tasks

Not sure if this is out of scope for this task though.

[stg: split README.md into multiple files]


Location: README.md:91
Comment: The command should be './bin/prj.sh build posix.acamar'

[stg: resolved as suggested]
