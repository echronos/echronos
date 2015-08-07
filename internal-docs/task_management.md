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

Overview
========

This document explains the work flows of creating, reviewing, integrating, and abandoning tasks.

Once created, a task is always associated with a git branch of the same name.
Typically, the terms *task*, *branch*, and *task branch* are used interchangeably.
Therefore, task management is very closely related to branch management.


Tasks
=====

The name of a task always consists of a unique ID and a brief summary of its purpose or goal.
The ID is generated automatically, while the developer creating a task supplies the rest of the name.

A task typically goes through the following work flow:

1. *create* action -> *under development* state
2. *review* action -> *under review* state
3. *integrate* action -> *integrated* state or
4. *abandon* action -> *abandoned* state

A developer uses the `x` tool to manage tasks and their work flows.
The following sections describe these states and work flows in more detail.


Development
===========

The command

    #> x.py task new SUMMARY

creates a new task.
The `SUMMARY` may not contain spaces.
`x` prints instructions about what further actions are necessary to fully create the task.

Once created, a task is considered under development until it is first put up for review.
While under development, the git history of a task branch may be modified and rewritten.
In particular, it is strongly recommended to keep a task branch up-to-date with the development branch by re-basing it onto development instead of merging the development branch into the task branch.
At that stage, re-basing keeps the git history cleaner than merging.


Review
======

This section describes the review process first from the perspective of a developer and then from the perspective of a reviewer.
Note that once the first round of reviews has started, the git history must no longer be modified under any circumstances.
This is necessary to keep the audit trail intact that the review process establishes.

Soliciting Reviews
------------------

1. A developer puts a task on review when they consider it fully ready for integration.
Pre-requisites are:
    1. Task branch is based on the latest revision of the development branch (via re-basing before the first review and via merging afterwards)
    2. All regression tests pass
2. The developer puts the task on review by running the command `x.py task review REVIEWERS`.
`REVIEWERS` is a space separated list of reviewer names or short-hands.
By convention, we use the user name part of a reviewer's e-mail address as their short-hand, e.g., *benno* for benno@brkawy.com.
The developer does not qualify as a reviewer.
The `x` command creates a review template file for each reviewer which is for the reviewer to edit.
The developer needs to manually notify reviewers of the new reviews, e.g., by e-mail.
3. Reviewers provide their feedback as described under [Providing Reviews] below, resulting in the review conclusion *Accept* or *Rework*.
4. When a reviewer asks a task to be reworked, the developer needs to resolve all concerns raised in the review and initiate another review round (i.e., go to 1.) until that same reviewer accepts the task branch.
A developer is free to immediately address review feedback and start another round of reviews or to wait for more reviews to come in and address them collectively.

The above review process continues until:

1. every reviewer who had arrived at the conclusion *Rework* later accepted the task AND
2. at least two reviewers accept the task.

Once these criteria are met, the task is ready for integration.


Providing Reviews
-----------------

When asked for a review, the reviewer should find a file with the pattern pm/reviews/TASKNAME/review-N.REVIEWERNAME in the repository.
This is where the review feedback goes.

A review consists of:

- reviewing the task description in pm/tasks/TASKNAME and checking whether the task motivation, goal, and test plan are necessary and sound
- reviewing the regression test results and checking whether they pass
- reviewing the differences to the branch point from the development branch and whether they:
    * fulfill the requirements of the task description
    * are correct, complete, minimal, efficient, clean, elegant, and meet the conventions documented in internal-docs/conventions.
    * are covered adequately by tests (if applicable)
    * pass the test plan documented in the task description

The RTOS code is expected to meet very high quality standards.
In a review, polite and constructive thoroughness is expected and encouraged.
At the same time, it is not unheard of that review feedback can be misguided, incomplete, off topic, out of scope, or based on misunderstandings.
Reviewers and developers are expected to resolve such cases constructively and in good faith.

Based on these criteria, the reviewer documents their feedback, including the overall verdict (accept/rework), in the review file mentioned above.
The review file is simply committed and pushed to the main repository.



Integration
===========

Before a task may be integrated into the development branch, it needs to meet the following criteria:

1. it has passed the review process as described above
2. it is up-to-date with the latest revision of the development branch in the main repository
    * if merging the development branch into the task branch is not trivial and requires (or leads to) functional changes, it is recommended to re-open the review process to receive feedback on the result of the merge
3. it needs to pass the regression tests and the task's test plan

When these conditions are met, the developer may integrate the task into the development branch by using the command `x.py task integrate`.



Abandoning Tasks
================

Sometimes, development on a task ceases.
This may be caused, for example, by the task motivation or goals becoming obsolete or the approach being considered unsuitable.
In such a case, the task should be abandoned rather than deleted.

To abandon a task, manually add the prefix `abandoned/` to its name and delete the original branch in the server repository.
This can be accomplished with a command similar to `git push origin TASKNAME:abandoned/TASKNAME :TASKNAME`
