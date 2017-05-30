Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Looks good to me! That is a fair chunk of code being obsoleted.

Location: unit_tests/test_blocking_mutex.py:45
Comment: Cosmetics: would it be worth splitting up this line into several? It
         is borderline unreadable here on my setup. :)
         > result = os.system("{} {} build posix.unittest.blocking-mutex".format(sys.executable,
         >                                                                       base_path('prj', 'app', 'prj.py')))

         Maybe something like
         prj_path = base_path('prj', 'app', 'prj.py')
         command = "build posix.unittest.blocking-mutex"
         result = os.system("{} {} {}".format(sys.executable, command, prj_path)

[stg: good point.
Resolved as suggested.]
