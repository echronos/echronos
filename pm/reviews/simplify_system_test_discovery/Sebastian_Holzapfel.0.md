Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

Generally looks good, a welcome improvement to the test system.

Location: packages/machine-qemu-ppce500/example/test.py:28
          packages/posix/test.py:28
Comment: Very minor nit. The comment:

     # importing <>TestCase would make the unittest framework to pick it up as a test case

I'm not sure exactly what this is supposed to mean, looks like a typo.

[stg: rephrased the comment to hopefully make more sense.]
