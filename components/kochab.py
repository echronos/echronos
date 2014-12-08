import os.path
from prj import Module
from operator import itemgetter


class KochabModule(Module):
    xml_schema_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema.xml')
    files = [
        {'input': 'rtos-kochab.h', 'render': True},
        {'input': 'rtos-kochab.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

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
        for idx, t in enumerate(tasks):
            t['idx'] = idx
            # Create a timer for each task
            timer = {'name': '_task_' + t['name'],
                     'error': 0,
                     'reload': 0,
                     'task': t,
                     'idx': len(config['timers']),
                     'enabled': False,
                     'sig_set': '_task_timer'}
            t['timer'] = timer
            config['timers'].append(timer)
        return config

module = KochabModule()
