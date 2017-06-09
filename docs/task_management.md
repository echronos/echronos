<!---
eChronos Real-Time Operating System
Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, version 3, provided that these additional
terms apply under section 7:

  No right, title or interest in or to any trade mark, service mark, logo
  or trade name of of National ICT Australia Limited, ABN 62 102 206 173
  ("NICTA") or its licensors is granted. Modified versions of the Program
  must be plainly marked as such, and must not be distributed using
  "eChronos" as a trade mark or product name, or misrepresented as being
  the original Program.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@TAG(NICTA_DOC_AGPL)
  -->

# Overview

This document explains the work flows of creating, reviewing, integrating, and abandoning tasks.
A task encapsulates the development and adoption of a feature into the project.

The lifecycle of a task knows the following actions and states, which are described in more detail below.

1. *create*: create a task branch for working on a specific change or improvement without impacting the mainline branch
2. *update*: while work is ongoing in a task, keep it up-to-date with any changes in the mainline branch
3. *review*: signal that work in a task is complete and ready for review and integration
4. *integrate*: once reviewed and accepted, integrate a task into the mainline branch
5. *abandon*: when work on a task ceases without it having been integrated, abandon it to keep it accessible but not clutter the list of active tasks


## Tasks

A task tracks a development effort for a certain feature or improvement.
Every change to the repository needs to go through a task.

A task also tracks the corresponding set of repository changes as a git branch.
Typically, the terms *task*, *branch*, and *task branch* are used interchangeably.
Therefore, managing a task effectively means managing a branch.


## Create

The command `task.py create TASKNAME` creates a new task with the given name.
This also creates the corresponding branch.

The name is a brief summary of the purpose or goal of the branch.
By convention, the name must only contain characters, digits, hyphens, and underscores.
Also, task names need to be unique.
`task.py` checks for this and fails if the task to create has the same name as another task.

From that point on, the developer of a task commits their changes to that branch.


## Update

While a developer works on a task, the mainline branch may integrate other tasks in the meantime.
It is best practice to frequently bring a task up-to-date with changes on the mainline branch.

As long as a task has not been put up for review, it is ok to rewrite its git history and to rebase it onto the mainline branch.
After a task has been put up for review, it is important to maintain its git history to maintain an audit trail.
Therefore, the mainline branch needs to be merged into the task branch.
The command `task.py update` takes care of this.


## Review

This section describes the review process first from the perspective of a developer and then from the perspective of a reviewer.
Note that once the first round of reviews has started, the git history must no longer be modified under any circumstances.
This is necessary to keep the audit trail intact that the review process establishes.

### Request Reviews

1. When a developer considers a task ready for integration, they run `task.py request_reviews`.
Make sure that all regression tests pass and notify reviewers.
2. Reviewers provide their feedback as described under [Providing Reviews] below, resulting in the review conclusion *accepted* or *rework*.
3. When a reviewer asks for a task to be reworked, the developer needs to resolve all concerns raised in the review until that same reviewer provides another review in which they accept the updates and the task branch as a whole.

Steps 2 and 3 of the review process continue until there are at least two *accepted* conclusions *and* no more *rework* conclusions.

### Provide Reviews

A reviewer starts a review by running the command `task.py review`.
This creates the file `pm/reviews/TASKNAME/_REVIEWERNAME_._INDEX_.md` where the review feedback goes.

Changes are expected to meet very high quality standards.
In a review, polite and constructive thoroughness is expected and encouraged.
A review consists of:

- reviewing the task description in `pm/tasks/TASKNAME` and checking whether the task motivation, goal, and test plan are necessary and sound
- reviewing the regression test results and checking whether they pass
- reviewing the differences to the branch point from the mainline branch and whether they:
    * fulfill the requirements of the task description
    * are correct, complete, minimal, efficient, clean, elegant, and meet the conventions documented in internal-docs/conventions.
    * are covered adequately by tests (if applicable)
    * pass the test plan documented in the task description

If the reviewer's conclusion is *accepted*, that review is complete and neither the developer nor the reviewer would typically need to take any further action.

If the reviewer concludes a review with a *rework* verdict, the developer and reviewer need to resolve the reviewer's concerns.
In some cases a reviewer's feedback can be accidentally incomplete, off topic, out of scope, or result from misunderstandings.
Reviewers and developers are expected to resolve such cases constructively and in good faith.

Developers have two main avenues to resolve a *rework* verdict:
1. They make the suggested changes and commit them to the task branch like any other change.
2. They reply to the reviewer by leaving additional comments in the review file, explaining why they think that no changes are necessary.
Again, they commit these changes to the review file on the task branch like any other change.
The reviewer then replies by creating another review via the `task.py review` command.


# Integrate

Before a task may be integrated into the mainline branch, it needs to meet the following criteria:

1. it has passed the review process as described above
2. it is up-to-date with the latest revision of the mainline branch
    * if merging the mainline branch into the task branch is not trivial and requires (or leads to) functional changes, it is recommended to re-open the review process to receive feedback on the result of the merge
3. it passes the regression tests and the task's test plan

When these conditions are met, the command `task.py integrate` merges the task into the mainline branch.


## Abandon

To abandon a task that is no longer being developed, manually add the prefix `abandoned/` to its name and delete the original branch in the remote repository.
This can be accomplished with a command similar to `git push origin TASKNAME:abandoned/TASKNAME :TASKNAME`


## Checking Integration Status

There are several ways to check whether a task has been integrated into the mainline branch.

1. Run the command `git merge-base --is-ancestor TASKNAME master`.
   If the task branch `TASKNAME` has been integrated into the mainline branch, this command exits with exit code 0.
   If the task branch is not yet integrated, the command exits with a non-zero exit code.
2. Run the command `git log --grep TASKNAME master`.
   If the task is integrated, this command typically lists a few changes.
   If the task branch is not yet integrated, the command usually lists no changes.
   This approach depends on the task name being very different from any other part of unrelated commit messages.
