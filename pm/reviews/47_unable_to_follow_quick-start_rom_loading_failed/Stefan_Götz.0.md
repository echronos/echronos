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

Location: README.md:110 & 118
Comment 3: Rework
At least on Ubuntu 17.04, the package is called `gdb-arm-none-eabi` instead of `arm-none-eabi-gdb`.
If older versions use the other name, it's worth mentioning.

Location: README.md:63
Comment 4: Rework
This fails on systems without a GUI, e.g., on default Ubuntu server installations.
I think it is worth either documenting that dependency or removing it.

Location: README.md:95
Comment 5: Rework
When I ran the Kochab system, the GDB output look quite different from what is documented in the README.
It seems that the Kochab test system behaves quite differently than what is documented at the moment.
Can you reproduce that?
