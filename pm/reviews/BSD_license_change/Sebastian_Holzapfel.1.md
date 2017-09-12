Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

Location: test_setup.bat
Comment:
This file fails the license sentinel regression test.
It also has a duplicate license header, but I don't think that's the cause of the test failure.

    $ /x.py test licenses
    ERROR:root:License check found files without a license header:
    ERROR:root:    echronos_root/test_setup.bat

Location: many files, i.e: packages/armv7m/default.ld
Comment:
Most of these files previously had a newline after the end of the license header before the code started.
I would prefer that we keep this for consistency.
