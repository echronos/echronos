Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Travis support with public test runs is a good idea!

Location: test.sh:32
Comment: 'echo -n' is not available everywhere. Plain sh for example does not
         have it. printf would be better here I think.

[stg: resolved as suggested]

Location: Travis test output: https://travis-ci.org/echronos/echronos#L2530
Comment: It fails with a "Python Exception <type 'exceptions.ImportError'>
         No module named gdb". Something went wrong here, and it does not seem
         to pick it up as a failure unfortunately.

[stg: the system tests do not actually fail in this case.
However, the message still indicates an issue in the GDB setup.
.travis.yml instructs Travis CI to cache the GDB build because building GDB from source takes a long time.
It was incorrectly set up to only cache the contents of $HOME/local/bin, but the GDB installation puts files in several other directories in $HOME/local that are necessary for GDB to execute without warnings.
This issue is resolved by caching the complete $HOME/local directory.]

Location: test.sh:39
          test_setup.sh:40
Comment: The definition for $USAGE is missing, right?

[stg: indeed; defined the USAGE variable in both files]

Location: test.sh:44
Comment: "sed -r 's/.*packages\///; s/.prx//; s/\//./g'"
         Is OS X supported as a platform? Not officially, right? In any case,
         there the '-r' option for sed does not exist (it is -E instead).
         We are not using an extended regex, so would it be worth simply not
         using the option here?

[stg: resolved as suggested]
