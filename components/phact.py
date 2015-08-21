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

import os.path
from prj import Module
from operator import itemgetter


class PhactModule(Module):
    xml_schema_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema.xml')
    files = [
        {'input': 'rtos-phact.h', 'render': True},
        {'input': 'rtos-phact.c', 'render': True, 'type': 'c'},
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

        # get merged task and mutex lists to figure out priorities
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

        mutexes = config['mutexes']
        mutexes.sort(key=itemgetter('priority'), reverse=True)
        for idx, m in enumerate(mutexes):
            m['idx'] = idx
        mutex_tasks = tasks + mutexes

        config['mutex_tasks_length'] = len(mutex_tasks)
        mutex_tasks.sort(key=itemgetter('priority'), reverse=True)
        for idx, mt in enumerate(mutex_tasks):
            mt['sched_idx'] = idx
        config['mutex_tasks'] = mutex_tasks

        # determine scheduling queue index type size
        if len(mutex_tasks) >= 65536:
            config['schedindex_size'] = 32
        elif len(mutex_tasks) >= 256:
            config['schedindex_size'] = 16
        else:
            config['schedindex_size'] = 8

        return config

module = PhactModule()
