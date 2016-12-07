Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

General comments:
Changes look good except small bug and a typo.

Location: pylib/task.py:215-217
Comment:
There is inconsistent use of manual and automatic field specification which results in the following error if one attempts to review using `task review`:

```
File "/home/seb/dev/echronos-workflow-sandbox/pylib/task.py", line 217, in review
    git push'.format(review_path, ' '.join(files_to_commit)))
ValueError: cannot switch from manual field specification to automatic field numbering
```

Possible fix:
```
-            print('To complete the review, edit the file "{0}" and commit and push it with the commands\n\
+            print('To complete the review, edit the file "{}" and commit and push it with the commands\n\
```

[stg: resolved as suggested]

Location: pylib/task.py:234,345
Comment:
`InconsistentUSerName` should be `InconsistentUserName`

[stg: resolved as suggested]
