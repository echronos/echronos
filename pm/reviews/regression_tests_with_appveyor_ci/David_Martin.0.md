Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Looks good to me! Though reading batch files is a whole new thing for me.

Location: test.bat:57
Comment: Is there a reason the unit tests (?) are commented out?
         Should we reenable them or delete the commented out bit?

[stg: They are disabled because they are known to not work on Windows.
I have removed the commented out commands and replaced them with a comment that explains why they are not present.]
