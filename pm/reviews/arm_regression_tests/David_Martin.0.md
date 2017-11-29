Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Nice one! looks good to me as far as I can tell. Just two minor comments from me.

Location:
Comment:
    The output of the new tests is currently very verbose in the logs [0]. Is
    there a way of reducing it or not having it print those?

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
