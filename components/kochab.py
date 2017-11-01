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
from operator import itemgetter
from prj import Module


class KochabModule(Module):
    xml_schema_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema.xml')
    files = [
        {'input': 'rtos-kochab.h', 'render': True},
        {'input': 'rtos-kochab.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        # This variant does not support memory protection at this time
        config['mpu_enabled'] = False

        # Create builtin signals
        config['signal_labels'].append({'name': '_task_timer', 'idx': len(config['signal_labels'])})

        # Create signal_set definitions from signal definitions:
        config['signal_sets'] = [{'name': sig['name'], 'value': 1 << sig['idx'], 'singleton': True}
                                 for sig in config['signal_labels']]

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''
        config['prefix_type'] = config['prefix'].capitalize() if config['prefix'] is not None else ''
        config['prefix_const'] = config['prefix'].upper() + '_' if config['prefix'] is not None else ''

        tasks = config['tasks']
        tasks.sort(key=itemgetter('priority'), reverse=True)
        for idx, task in enumerate(tasks):
            task['idx'] = idx
            # Create a timer for each task
            timer = {'name': '_task_' + task['name'],
                     'error': 0,
                     'reload': 0,
                     'task': task,
                     'idx': len(config['timers']),
                     'enabled': False,
                     'sig_set': '_task_timer'}
            task['timer'] = timer
            config['timers'].append(timer)
        return config

module = KochabModule()  # pylint: disable=invalid-name
