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

# Welcome!

You are new around here?
Welcome to the project - we are glad to have you!

If you have any questions, send us an e-mail to echronos@trustworthy.systems or tweet at us @echronosrtos.

If there is something in the project that you think is worth improving, please create a [github issue](https://github.com/echronos/echronos/issues).

# Contributing

We have a few basic rules for how contributing works exactly:

- [your first contribution](#your-first-contribution)
- [further contributions](#further-contributions)
- [contributor agreements](#contributor-agreements)

Let's walk you through that fine print briefly.

## Your First Contribution

If this is your first contribution to the eChronos RTOS, we will try and make this painless for you.
Assuming you already have a clone/branch of the repository with your changes sitting somewhere, simply submit a github pull request.

The only snag is the [contributor agreement](#contributor-agreements) which we need to be able to integrate your changes.
We will sort out and walk you through the rest to make this easy for you.

_The rest_ includes our workflows that help to make every contribution the best it can be (reviews, tests, the works).
More on that below.

## Further Contributions

You picked the red pill.
Good on you!

Currently, the project does not use github to handle changes.
We have a purely git-based work flow in place that is supported by custom tools.
It revolves around task branches (similar to branches and pull requests on github) that have the following, common-sense life cycle:

- create a task branch with a description of the work planned
- commit changes to task branch
- submit task branch for review until all review comments have been addressed
- integrate task branch into mainline branch

A core goal of this approach is to create an audit trail within git, so all task descriptions, reviews, and comments are files in the git repository.

[`internal-docs/task_management.md`](internal-docs/task_management.md) documents the details of this workflow and how our tools help with it.

## Contributor Agreements

All contributions to the repository need to be accompanied by a scan of a signed copy of the eChronos Contributor Assignment Agreement.
There is a version of the agreement for [individuals](https://ssrg.nicta.com.au/projects/TS/echronos/CAA-individual.pdf) and a version for [companies](https://ssrg.nicta.com.au/projects/TS/echronos/CAA-entity.pdf).
