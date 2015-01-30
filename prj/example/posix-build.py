# @LICENSE(NICTA)

from prj import execute, SystemBuildError


def system_build(output_file, modules, include_paths=None):
    if include_paths is None:
        include_paths = []

    inc_path_args = ['-I%s' % i for i in include_paths]

    c_files = []
    for m in modules:
        c_files.extend(m.c_files())

    if len(c_files) == 0:
        raise SystemBuildError("Zero C files in system definition")

    execute(['gcc', '-o', output_file, '-Wall'] + inc_path_args + c_files)
