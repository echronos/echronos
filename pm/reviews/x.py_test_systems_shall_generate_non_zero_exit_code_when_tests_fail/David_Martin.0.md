Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Location: x.py:281
Comment: I appreciate the sanity check against actually receiving an int return
         value!

Location: x.py:270
Comment: This line should return an integer value as well.
         For example just calling 'x.py build' throws an exception as below:

         > p3 x.py build
         > usage: x.py [-h] {build,test} ...
         >
         > optional arguments:
         >   -h, --help    show this help message and exit
         >
         > commands:
         >   {build,test}
         > Traceback (most recent call last):
         >   File "x.py", line 288, in <module>
         >     .format(type(result)))
         > TypeError: The main() function shall return an integer, but returned a value of type <class 'NoneType'> instead.

[stg: resolved as suggested]

         Should all possible calls return an int or should we only check for
         calls to 'test systems'?

[stg: Since this task introduces the generic check for integers in the __main__ context of x.py, it makes sense to ensure that all code paths return integers.
This task certainly attempts to achieve that, so please feel free to double-check this.]

         Note also that calling 'x.py build' prints the help dialog for 'x.py'
         rather than 'x.py build' which would be more appropriate and actually
         convey information on what the correct build command would be. It should
         print the output of 'x.py build --help'. This applies to the other commands
         as well. It is unrelated to the current task though, so feel free to
         ignore.

[stg: unfortunately, the argparse API does not support that directly.
Since such a change would therefore require significant refactoring, I will leave the current behavior as is.]
