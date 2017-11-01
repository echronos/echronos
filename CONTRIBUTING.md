<!--
     eChronos Real-Time Operating System
     Copyright (c) 2017, Commonwealth Scientific and Industrial Research
     Organisation (CSIRO) ABN 41 687 119 230.

     All rights reserved. CSIRO is willing to grant you a licence to the eChronos
     real-time operating system under the terms of the CSIRO_BSD_MIT license. See
     the file "LICENSE_CSIRO_BSD_MIT.txt" for details.

     @TAG(CSIRO_BSD_MIT)
-->

# Welcome!

You are new around here?
Welcome to the project - we are glad to have you!

If you have any questions, send us an e-mail to echronos@trustworthy.systems or tweet at us [@echronosrtos](https://twitter.com/echronosrtos).

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

Currently, the project does not use github pull requests and reviews to handle changes.
We have a purely git-based work flow in place that is supported by custom tools.
It revolves around task branches (similar to branches and pull requests on github) that have the following, common-sense life cycle:

- create a task branch with a description of the work planned
- commit changes to task branch
- submit task branch for review until all review comments have been addressed
- integrate task branch into mainline branch

A core goal of this approach is to create an audit trail within git, so all task descriptions, reviews, and comments are files in the git repository.

[The _Task Management_ section of the README](README.md#task-management) documents the details of this workflow and how our tools help with it.

## Contributor Agreements

All contributions to the repository need to be accompanied by a scan of a signed copy of the eChronos Contributor Assignment Agreement.
There is a version of the agreement for [individuals](https://ts.data61.csiro.au/projects/TS/echronos/CAA-individual.pdf) and a version for [companies](https://ts.data61.csiro.au/projects/TS/echronos/CAA-entity.pdf).
