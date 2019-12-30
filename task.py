#!/usr/bin/env python3
#
# eChronos Real-Time Operating System
# Copyright (c) 2017, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#

import argparse
import os.path
import sys

# Make pylib available for importing - this is necessary for x.py wrappers in client repositories to work
sys.path.append(os.path.dirname(__file__))

# pylint: disable=wrong-import-position
from pylib import task_commands  # pylint: disable=unused-import
from pylib.cmdline import add_commands_to_parser
from pylib.task import TaskConfiguration
from pylib.utils import TOP_DIR, BASE_DIR

TASK_CFG = TaskConfiguration(repo_path=TOP_DIR,
                             tasks_path=os.path.join('pm', 'tasks'),
                             description_template_path=os.path.join(BASE_DIR, '.github', 'PULL_REQUEST_TEMPLATE.md'),
                             reviews_path=os.path.join('pm', 'reviews'),
                             mainline_branch='master',
                             manage_release_version=True)


def main(task_cfg=TASK_CFG):
    parser = argparse.ArgumentParser(prog='task.py')
    add_commands_to_parser(globals(), parser)
    args = parser.parse_args()

    if not args.command:
        # argparse does not support required subparsers so it does not itself reject a command line that lacks a
        # command or subcommand
        parser.print_help()
    else:
        args.execute(task_cfg, args)


if __name__ == "__main__":
    main()
