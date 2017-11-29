Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Nice one! looks good to me as far as I can tell. Just two minor comments from me.

Location:
Comment:
    The output of the new tests is currently very verbose in the logs [0]. Is
    there a way of reducing it or not having it print those?

[sebh: 100% agree that it is annoyingly verbose.
Unfortunately despite spending a few hours on this I have not found a reliable way to quench it.
Strangely, using a `&> /dev/null` works as intended in a shell.
However, I attempted a stderr/stdout redirect to `/dev/null` using 3 different methods with Popen, and either the output always appears in the logs, or QEMU eats CPU and the test never completes.
If you consider this a dealbreaker, submit another rework I'll spend some more time on it]

Location: packages/machine-qemu-simple/example/test.py:linenum
Comment:
    ```
    class Acamar(tests.Armv7mQemuTestCase):
        prx_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'acamar-system.prx')
    ```

    Nitpick: Instead of determining the current directory for every test it would
             be more concise to call `os.path.dirname(os.path.abspath(__file__))`
             once and then refer back to it.

[sebh: I agree, however all the existing `test.py` scripts (i.e `machine-qemu-ppce500/example/test.py`) use the verbose form; and to change all of them would probably be out of the task scope.
Keeping as-is for consistency.]

[0] https://travis-ci.org/echronos/echronos/jobs/306662705
    ```
    task b
    task a
    task b
    task a
    task b
    task a
    task b
    task a
    task b
    task a
    task b
    ```

    and so on
