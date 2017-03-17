Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Nice find!

Location: pylib/tests.py:221
Comment: Would it make sense to have a comment for the error code? Otherwise it
         is a somewhat magic string that does not explain itself.

stg@brkawy.com: That is a very valid point.
I have added a comment as suggested.
Note that resolving the pycodestyle error (instead of ignoring it) is out of scope for this task, but is addressed by the task integrate_pylint_package_and_use_for_regression_tests.
