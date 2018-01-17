Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Location: pjr/app/prj.py:1236
Comment:
    ```
    prefix = os.path.commonprefix(paths)
    if not os.path.exists(prefix):
        return os.path.dirname(prefix)
    assert False
    return None
    ```

I had a look online and I gather the goal is to support several Python versions.
Depending on the Python version the commonpath function is not available. The
alternative is commonprefix which kind of does the same, but is somewhat unhelpful
as it does a string comparison and may not actually return a valid path [0].

In the case that it does return a valid path though, should we not return it
instead of raising an assertion error? Because that's the whole point, right?


[0] https://docs.python.org/3.5/library/os.path.html#os.path.commonprefix

[stg: you are right, and I changed the code to address that.
However, I believe there was another bug here in that commonpath() calls os.path.commonprefix() and completely ignores the commonprefix() function in the prj module.
I have therefore also changed commonpath() to call commonprefix() instead.
Please review.]
