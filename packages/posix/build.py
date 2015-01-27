from prj import execute, SystemBuildError


schema = {
    'type': 'dict',
    'name': 'module',
    'dict_type': ([{'type': 'string', 'name': 'output_type', 'default': 'executable'}], [])
}


def run(system, configuration=None):
    return system_build(system, configuration)


def system_build(system, configuration):
    inc_path_args = ['-I%s' % i for i in system.include_paths]

    if len(system.c_files) == 0:
        raise SystemBuildError("Zero C files in system definition")

    shared_args = ['-shared', '-fPIC'] if configuration['output_type'] == 'shared-library' else []

    execute(['gcc', '-o', system.output_file, '-Wall', '-Werror'] + shared_args + inc_path_args + system.c_files)
