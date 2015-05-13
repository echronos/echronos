#
# eChronos Real-Time Operating System
# Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, version 3, provided that no right, title
# or interest in or to any trade mark, service mark, logo or trade name
# of NICTA or its licensors is granted.
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

from types import ModuleType, FunctionType
from functools import wraps


class Arg:
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs


class subcmd:
    def __init__(self, name=None, cmd=None, help=None, args=()):
        self.name = name
        self.cmd = cmd
        self.help = help
        self.args = args

    def __call__(self, f):
        @wraps(f)
        def wrapper(*args, **kwds):
            return f(*args, **kwds)
        if self.name is None:
            self.name = f.__name__
        if self.cmd is None:
            self.cmd = f.__module__.split('.')[-1]
        self.execute = f
        wrapper.subcmd = self
        return wrapper


def add_cmds_in_globals_to_parser(global_attributes, parser):
    cmd_tree = _get_cmd_tree(_get_subcmds(global_attributes))
    _add_cmd_tree_to_parser(cmd_tree, parser)


def _get_subcmds(global_attributes):
    for module in global_attributes.values():
        if isinstance(module, ModuleType):
            yield from [func.subcmd for func in vars(module).values()
                        if isinstance(func, FunctionType) and hasattr(func, 'subcmd')]


def _get_cmd_tree(subcmds):
    # produce dict {'command name': {'subcommand name 1': subcmd1, 'subcommand name 2': subcmd2}}
    cmds = {}
    for subcmd in subcmds:
        cmd_dict = cmds.setdefault(subcmd.cmd, {})
        cmd_dict[subcmd.name] = subcmd
    return cmds


def _add_cmd_tree_to_parser(cmd_tree, parser):
    cmds_parsers = parser.add_subparsers(title='commands', dest='command')
    for cmd in sorted(cmd_tree.keys()):
        subcmds_parsers = cmds_parsers.add_parser(cmd).add_subparsers(dest="subcommand")
        for subcmd_name in sorted(cmd_tree[cmd].keys()):
            subcmd = cmd_tree[cmd][subcmd_name]
            subcmd_parser = subcmds_parsers.add_parser(subcmd_name, help=subcmd.help)
            for arg in subcmd.args:
                subcmd_parser.add_argument(*arg.args, **arg.kwargs)
            subcmd_parser.set_defaults(execute=subcmd.execute)
