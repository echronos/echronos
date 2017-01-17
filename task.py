#!/usr/bin/env python3
#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that these additional
# terms apply under section 7:
#
#   No right, title or interest in or to any trade mark, service mark, logo or
#   trade name of of National ICT Australia Limited, ABN 62 102 206 173
#   ("NICTA") or its licensors is granted. Modified versions of the Program
#   must be plainly marked as such, and must not be distributed using
#   "eChronos" as a trade mark or product name, or misrepresented as being the
#   original Program.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# @TAG(NICTA_AGPL)
#
import argparse
import os.path
import sys

# Make pylib available for importing - this is necessary for x.py wrappers in client repositories to work
sys.path.append(os.path.dirname(__file__))

# pylint: disable=wrong-import-position
from pylib import task_commands  # pylint: disable=unused-import
from pylib.cmdline import add_commands_to_parser


def main():
    parser = argparse.ArgumentParser(prog='task.py')
    add_commands_to_parser(globals(), parser)
    args = parser.parse_args()

    if not args.command:
        # argparse does not support required subparsers so it does not itself reject a command line that lacks a
        # command or subcommand
        parser.print_help()
    else:
        args.execute(args)


if __name__ == "__main__":
    main()
