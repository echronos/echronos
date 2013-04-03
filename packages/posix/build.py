from prj import execute, SystemBuildError


def run(system, configuration=None):
    return system_build(system)


def system_build(system):
    inc_path_args = ['-I%s' % i for i in system.include_paths]

    if len(system.c_files) == 0:
        raise SystemBuildError("Zero C files in system definition")

    execute(['gcc', '-o', system.output_file, '-Wall', '-Werror'] + inc_path_args + system.c_files)
