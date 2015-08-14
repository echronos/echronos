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
"""Tools for managing command line commands of x.py.

x.py's command line is structured by high-level commands, such as *test*, each of which have multiple sub-commands,
such as *systems*.
Each sub-command can have one or more arguments/options.
With the tools in this module, a function that implements a sub-command can be annotated with attributes that describe
its command-line command, sub-command, help string, and arguments:

    @subcmd(name='systems', cmd='test', help='Run system tests',
            args=(Arg('--verbose', action='store_true', default=False),))
    def system_tests():
        pass

"""

from types import ModuleType, FunctionType
from functools import wraps


class Arg:
    """Wrap arbitrary function arguments.

    With this class, one can store a collection of arbitrary unnamed and named function arguments with a minimum
    amount of code.
    The arguments can then, e.g., be passed on to another function call.

    In the case of command line parsing, Arg objects collect the arguments that go into the add_argument() method of
    command line parsers.

    """
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs


class subcmd:
    """The @subcmd() function attribute marks a function as the implementation of an x.py command-line sub-command."""
    def __init__(self, name=None, cmd=None, help=None, args=()):
        """Create a function attribute and wrapper for a function implementing a command-line sub-command.

        The arguments of this constructor allow to describe the command-line properties of the sub-command in more
        detail.

        `name`: name of the sub-command the user has to specify to invoke the sub-command, e.g., "systems".
        It defaults to the name of the wrapped function.
        For example, when adding the @subcmd attribute to the function `def systems()`, the default for the
        sub-command's name is "systems".

        `cmd`: name of the parent command of the sub-command as it appears on the command line, e.g., "test".
        It defaults to the name of the module containing the wrapped function.
        For example, when adding the @subcmd attribute to the function `def systems()` implemented in the module
        `test.py`, the default value for `cmd` is "test".

        `help`: the help string for the sub-command as it appears in the command-line help of x.py.

        `args`: the arguments or options for the sub-command, e.g., "--verbose", as an iterable of Arg objects.
        For each Arg object, its contents are passed to the add_argument() function of the parser object for the
        sub-command.

        """
        self.name = name
        self.cmd = cmd
        self.help = help
        self.args = args

    def __call__(self, f):
        """Implementation of Python magic for wrapping functions."""
        # See functools.wraps() documentation
        @wraps(f)
        def wrapper(*args, **kwds):
            return f(*args, **kwds)
        # Let sub-command name default to name of wrapped function
        if self.name is None:
            self.name = f.__name__
        # Set sub-command parent cmd default to name of module implementing wrapped function
        if self.cmd is None:
            self.cmd = f.__module__.split('.')[-1]
        # Set function wrapper as handler for sub-command
        self.execute = f
        # Make subcmd object and its properties accessible as attribute of function wrapper
        wrapper.subcmd = self
        return wrapper


def add_cmds_in_globals_to_parser(global_attributes, parser):
    """Search global attributes for functions marked with @subcmd attributes and construct command-line parser"""
    cmd_tree = _get_cmd_tree(_get_subcmds(global_attributes))
    _add_cmd_tree_to_parser(cmd_tree, parser)


def _get_subcmds(global_attributes):
    """From x.py's globals() dictionary, retrieve the subcmd objects of all functions with the @subcmd attribute"""
    for module in global_attributes.values():
        if isinstance(module, ModuleType):
            yield from [func.subcmd for func in vars(module).values()
                        if isinstance(func, FunctionType) and hasattr(func, 'subcmd')]


def _get_cmd_tree(subcmds):
    """Convert flat list of subcmd objects into hierarchical dictionary
    {'command name': {'subcommand name 1': subcmd1, 'subcommand name 2': subcmd2}}"""
    cmds = {}
    for subcmd in subcmds:
        cmd_dict = cmds.setdefault(subcmd.cmd, {})
        cmd_dict[subcmd.name] = subcmd
    return cmds


def _add_cmd_tree_to_parser(cmd_tree, parser):
    """Create command-line parser from hierarchical subcmd objects."""
    cmds_parsers = parser.add_subparsers(title='commands', dest='command')
    for cmd in sorted(cmd_tree.keys()):
        subcmds_parsers = cmds_parsers.add_parser(cmd).add_subparsers(dest="subcommand")
        for subcmd_name in sorted(cmd_tree[cmd].keys()):
            subcmd = cmd_tree[cmd][subcmd_name]
            subcmd_parser = subcmds_parsers.add_parser(subcmd_name, help=subcmd.help)
            for arg in subcmd.args:
                subcmd_parser.add_argument(*arg.args, **arg.kwargs)
            subcmd_parser.set_defaults(execute=subcmd.execute)
