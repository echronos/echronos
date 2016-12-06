Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

General comments:
=================

- Overall, I think these are some important changes that will really streamline our workflow.

- The changes as they are presented here are very thorough and improve consistency in many areas of task management, which is great.
    - I have tested the changes by cloning the repository across to a different provider (bitbucket) and then performing various task management activities.

- Chicken & egg situation of not using the established review process to review workflow changes seems a bit off but it is unavoidable.

- There are a few changes that seem to be out of scope for this task, for example in x.py there is some reordering, some general cleanup & refactoring.
    - This is great, and definitely related to the task, but perhaps the task description should be updated to better encompass the general cleanup and such.
    - Examples of this: commit dc09f0, f9ea, d1720, bd951..

[stg: added general, but related clean-up to the goals of this task]

- Not all of the completed task descriptions have been moved out of the "pm/tasks/completed" folder - not sure why this is.

[stg: caused by me having merged completed tasks from the development branch after having moved all other task descriptions.
Resolved by moving task descriptions from `pm/tasks/completed/` to `pm/tasks/`.]

- It may be worth documenting more clearly that one's .gitconfig name determines review increments, and not their github account or email.
    - On some of my machines for example, I have 'Seb Holzapfel' instead of 'Sebastian Holzapfel' as my commit name.
    - Easy to fix (on my machines), but this was not an issue before due to the fixed review templates.
    - As one can associate multiple emails with their github account, one could argue that discriminating review increments based on someone's github account might be better
        - This would introduce too much complexity in my opinion, requiring calls to the github API to fetch all associated emails to an account.
        - So emphasizing the name dependency in the documentation is probably sufficient

[stg: resolved by checking for similar reviewer names and double checking with the user as part of the `task.py review` command]

- It may be worth documenting the task naming restrictions, including our convention of using underscores rather than hyphens to separate words in task branches.

[stg: resolved by documenting it in `internal-docs/task_management.md`;
it is also "documented" in the user-visible error raised when an invalid name is encountered.]

Specific comments:
==================

pylib/utils.py:387
- There is a typo in the docstring - "specificed"

[stg: resolved by replacing 'specificed' with 'specified']

pylib/task_commands.py:69, 78
- I think an 'alias' has to have an obvious 'target'.
- Both commands are listed as aliases of each other - perhaps it makes more sense to remove the 'alias' statement from  the first command to correctly indicate that the control flow from the accept alias falls through to the review command, but not the other way around.

[stg: resolved by removing 'alias' documentation from the '--accept' option.]

pylib/task.py:80
- This filename seems excessively long. Maybe '.keep'? Obviously a matter of preference though.

[stg: I do prefer the long, but self-explanatory file name.
Unless other reviewers also prefer a shorter name, I leave it as is.]

pylib/task.py:102, 211
- `git commit -a` scares me a little as it discourages checking for unintentionally committing an unrelated modification, but this is probably fine and unlikely to happen if someone is performing a review.

[stg: resolved by making the instructions more specific and limiting the commits to the files that can be expected to have changed.]
