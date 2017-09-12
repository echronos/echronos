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

from prj import execute, SystemBuildError


def system_build(output_file, modules, include_paths=None):
    if include_paths is None:
        include_paths = []

    inc_path_args = ['-I%s' % i for i in include_paths]

    c_files = []
    for mod in modules:
        c_files.extend(mod.c_files())

    if not c_files:
        raise SystemBuildError("Zero C files in system definition")

    execute(['gcc', '-o', output_file, '-Wall'] + inc_path_args + c_files)
