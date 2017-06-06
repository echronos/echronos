Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Nice work!

Location: pylib/task.py:190
Comment: > self._git.commit('{}\nUpdate {} number of release version .'.format(...)
         Nitpick: This adds an unnecessary whitespace between version and the
         full stop. When seeing it in the commit message it reads like there is
         something missing.

[stg: indeed - removed whitespace as suggested.]

Location: pm/tasks/manage_release_version_numbers:49
Comment: Would it be worth mentioning that two accepting reviews have to be created
         to run `task.py integrate --offline`? Might be obvious. But, if we update
         this, the `git reset --hard HEAD~1` would have to be updated as well
         though.

Location: pm/tasks/manage_release_version_numbers:54
Comment: > Run `git checkout manage_release_version_numbers` to switch back to this task.
         Not an actual review comment but an observation. This is one of the git
         niceties: `git checkout -` switches to the previous branch. :)
