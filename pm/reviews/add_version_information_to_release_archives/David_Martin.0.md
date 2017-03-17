Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Location: release\_cfg.py:35
Comment: The version number needs to be bumped again as we have since
         had an update on master.

[stg: As long as it is only a change in the patch number and not a minor or major change, I would prefer to leave it as is.
Otherwise, every single task on review needs to have their version numbers updated when another task is integrated.
It is then the responsibility of the integrator to update the version number when integrating a completed task.
Is that acceptable?
Note that the task manage_release_version_numbers will address this whole topic more strategically.]
