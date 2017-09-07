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
import os.path
import sys
from prj import execute, SystemBuildError


# pylint: disable=invalid-name
schema = {
    'type': 'dict',
    'name': 'module',
    'dict_type': ([{'type': 'string', 'name': 'output_type', 'default': 'executable'}], [])
}


def run(system, configuration=None):
    return system_build(system, configuration)


def system_build(system, configuration):
    inc_path_args = ['-I%s' % i for i in system.include_paths + [os.path.dirname(os.path.abspath(__file__))]]

    if not system.c_files:
        raise SystemBuildError("Zero C files in system definition")

    if configuration['output_type'] == 'shared-library':
        shared_args = ['-shared']
        if sys.platform != 'win32':
            shared_args.append('-fPIC')
    else:
        shared_args = []

    execute('gcc -std=c90 -Werror -Wall --all-warnings -Wpedantic -pedantic -Wextra -O -Winit-self -Wswitch-default \
-Wswitch-enum -fstrict-aliasing -fstrict-overflow -Wstrict-overflow=5 -Wundef -Wbad-function-cast -Wcast-qual \
-Wcast-align -Wwrite-strings -Wjump-misses-init -Wlogical-op -Waggregate-return -Wstrict-prototypes \
-Wmissing-prototypes -Wmissing-declarations -Wpacked -Wredundant-decls -o'.split() +
            [system.output_file] + shared_args + inc_path_args + system.c_files)
