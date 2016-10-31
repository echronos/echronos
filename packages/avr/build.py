#
# eChronos Real-Time Operating System
# Copyright (c) 2018, Commonwealth Scientific and Industrial Research
# Organisation (CSIRO) ABN 41 687 119 230.
#
# All rights reserved. CSIRO is willing to grant you a licence to the eChronos
# real-time operating system under the terms of the CSIRO_BSD_MIT license. See
# the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
#
# @TAG(CSIRO_BSD_MIT)
#
import os
from prj import execute


# pylint: disable=invalid-name
schema = {
    'type': 'dict',
    'name': 'module',
    'dict_type': ([{'type': 'bool', 'name': 'verbose', 'default': 'True'},
                   {'type': 'string', 'name': 'mmcu', 'default': 'atmega328p'},
                   {'type': 'int', 'name': 'cpu_frequency', 'default': '16000000'},
                   {'type': 'int', 'name': 'baud', 'default': '9600'}], [])
}


def run(system, configuration=None):
    return system_build(system, configuration)


def system_build(system, configuration):
    inc_path_args = ['-I%s' % i for i in system.include_paths]
    common_flags = ['-mmcu=' + configuration['mmcu']]
    a_flags = common_flags + ['-x', 'assembler-with-cpp', '-c']
    c_flags = common_flags + ['-c', '-Wall', '-Werror', '-Os', '-DF_CPU={}UL'.format(configuration['cpu_frequency']),
                              '-DBAUD={}'.format(configuration['baud'])]

    c_and_asm_files = [path for path in system.asm_files + system.c_files]
    base_names = set(os.path.splitext(os.path.basename(path))[0] for path in c_and_asm_files)
    if len(c_and_asm_files) != len(base_names):
        raise RuntimeError("Two or more input files share the same base name (e.g. 'bar/foo.c' and 'baz/foo.S'). "
                           "This is not supported by the build system. "
                           "All input files need to have distinct base names: {}".format(c_and_asm_files))

    asm_obj_files = [os.path.join(system.output, os.path.basename(asm_file.replace('.S', '.o').replace('.s', '.o')))
                     for asm_file in system.asm_files]
    for asm_file, obj_file in zip(system.asm_files, asm_obj_files):
        os.makedirs(os.path.dirname(obj_file), exist_ok=True)
        execute(['avr-gcc'] + a_flags + inc_path_args + ['-o', obj_file, asm_file])

    c_obj_files = [os.path.join(system.output, os.path.basename(c_file.replace('.c', '.o')))
                   for c_file in system.c_files]
    for c_file, obj_file in zip(system.c_files, c_obj_files):
        os.makedirs(os.path.dirname(obj_file), exist_ok=True)
        execute(['avr-gcc'] + c_flags + inc_path_args + ['-o', obj_file, c_file])

    obj_files = asm_obj_files + c_obj_files
    execute(['avr-gcc'] + common_flags + ['-o', system.output_file] + obj_files)

    hex_file = os.path.splitext(system.output_file)[0] + '.hex'
    execute(['avr-objcopy', '-O', 'ihex', '-R', '.eeprom', system.output_file, hex_file])

    if configuration["verbose"]:
        print('''Default command for programming Arduino device:
 avrdude -c arduino -p {} -b 115200 -U flash:w:{} -P COM? / /dev/ttyS?'''.format(configuration['mmcu'].upper(),
                                                                                 hex_file))
        print('''Default commands for debugging with avr-gdb and simulavr:
 simulavr -d {} -g
 avr-gdb -ex "target remote localhost:1212" -ex load {}'''.format(configuration['mmcu'].lower(), system.output_file))
