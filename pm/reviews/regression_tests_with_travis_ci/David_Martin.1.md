Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

This probably counts as nitpicking, so feel free to ignore!

Location: Travis log output:678
          https://travis-ci.org/echronos/echronos/jobs/208864888#L687
Comment: I feel like every command run as part of the test suite should report
         whether it passed or failed.
         For example 'x.py test licences' does not print anything and does not
         give any indication on whether it actually did what it was supposed to.
         Especially for external contributors this makes it less clear whether
         their changes actually pass everything.

         Even if it is just a simple print that says 'OK' or 'FAIL' it would
         already help in my opinion.

[stg: This is a very valid point.
I have updated the test script to print the test outcome for each test as well as a summary.]
