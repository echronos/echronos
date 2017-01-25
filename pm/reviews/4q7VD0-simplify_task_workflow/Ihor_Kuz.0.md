Reviewer: Ihor Kuz (ihor.kuz@nicta.com.au)
Conclusion: Rework

General Comments:
This is quite a big task (90 commits), so my review is not particularly in depth
(I read the code, but not in as much detail as I might have a smaller review).
Most of my comments relate to improving the documentation of the new workflow.
If you need a more thorough code review let me know.

Location: internal-docs/task_management.md:33
Comment:
commit 078f148b removes an overview list of the steps involved in the task creation/review process.
I think some sort of 'quick guide' overview is useful to remind casual committers/reviewers of the process without having to re-read the whole document every time.

[stg: That is a good point.
I have re-created such an overview at the top of the file.]

Location: pylib/task.py:89
Comment:
commit 7956dffa removes the code to add a randomized 6-digit prefix because "it is not necessary to make task names unique", however, task branches do have to be unique, and there is code that checks that when creating a task.
This uniqueness requirement for task names should be stated in the documentation.

[stg: Very true.
I updated the documentation accordingly.]

Location: commit c6c983d3ee
Comment:
commit c6c983d3ee simplify task handling by not moving task description files from pm/tasks/ into pm/tasks/completed/ it says that "to check whether a branch has been completed, one can simply check whether it has been integrated into the mainline branch".
Please include this information and instructions for how to do this check in the documentation.

[stg: I added this information to the 'Integrate' section of the documentation.]
