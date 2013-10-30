from prj import SystemParseError, Module


class RigelModule(Module):
    xml_schema = """
  <schema>
   <entry name="signalset_size" type="int" default="8"/>
   <entry name="prefix" type="ident" optional="true" />
   <entry name="fatal_error" type="c_ident" />
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="function" type="c_ident" />
      <entry name="name" type="ident" />
      <entry name="start" type="bool" default="false" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="signal_labels" type="list">
     <entry name="signal_label" type="dict">
       <entry name="name" type="ident" />
       <entry name="global" type="bool" optional="true" />
       <entry name="tasks" type="list" optional="true">
         <entry name="task" type="object" group="tasks" />
       </entry>
       <constraint type="one_of">
         <entry>global</entry>
         <entry>tasks</entry>
       </constraint>
     </entry>
   </entry>
   <entry name="interrupt_events" type="list" auto_index_field="idx">
     <entry name="interrupt_event" type="dict">
      <entry name="name" type="ident" />
      <entry name="task" type="object" group="tasks" />
      <entry name="sig_set" type="ident" />
     </entry>
   </entry>
   <entry name="mutexes" type="list" auto_index_field="idx">
     <entry name="mutex" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="timers" type="list" auto_index_field="idx">
     <entry name="timer" type="dict">
      <entry name="name" type="ident" />
      <entry name="enabled" type="bool" />
      <entry name="reload" type="int" />
      <entry name="error" type="int" default="0" />
      <entry name="task" type="object" group="tasks" optional="true" />
      <entry name="sig_set" type="ident" optional="true" />
     </entry>
   </entry>
   <entry name="profiling" type="dict" optional="true">
    <entry name="task_uptime" type="bool" optional="true" default="true"/>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-rigel.h', 'render': True},
        {'input': 'rtos-rigel.c', 'render': True, 'type': 'c'},
    ]

    def configure(self, xml_config):
        config = super().configure(xml_config)

        config['prefix_func'] = config['prefix'] + '_' if config['prefix'] is not None else ''
        config['prefix_type'] = config['prefix'].capitalize() if config['prefix'] is not None else ''
        config['prefix_const'] = config['prefix'].upper() + '_' if config['prefix'] is not None else ''

        # Ensure that at least one task is runnable.
        if not any(task['start'] for task in config['tasks']):
            raise SystemParseError("At least one task must be configured to start.")

        # Semi-configurable items
        # These are configurable in the code, but for simplicitly they are not supported as
        # user configuration at this stage.
        config['interrupteventid_size'] = 8
        config['taskid_size'] = 8

        # Create builtin signals
        config['signal_labels'].append({'name': '_task_timer', 'global': True})

        # The RTOS utility signal is used in the following conditions:
        #   1. To start the task.
        #   2. To notify the task when a mutex is unlocked.
        #   3. To notify the task when a message queue has available messages / space
        #
        # The same signal is re-used to avoid excessive allocation of signals.
        # This is safe as a task can not be simultanesouly waiting to start,
        # waiting for a mutex, and waiting on a message queue.
        config['signal_labels'].append({'name': '_rtos_util', 'global': True})

        # Assign signal ids
        sig_sets = []
        for task in config['tasks']:
            sig_set = []
            for sig in config['signal_labels']:
                if sig.get('global', False) or task['name'] in [t['name'] for t in sig['tasks']]:
                    sig_set.append(sig['name'])
            sig_sets.append(sig_set)

        label_ids = assign_signal_vals(sig_sets)
        for sig in config['signal_labels']:
            sig['idx'] = label_ids[sig['name']]

        # Create signal_set definitions from signal definitions:
        config['signal_sets'] = [{'name': sig['name'], 'value': 1 << sig['idx'], 'singleton': True}
                                 for sig in config['signal_labels']]

        signal_set_names = [sigset['name'] for sigset in config['signal_sets']]

        for interrupt_event in config['interrupt_events']:
            if interrupt_event['sig_set'] not in signal_set_names:
                msg = "Unknown signal-set '{}' in interrupt_event '{}'"
                raise SystemParseError(msg.format(interrupt_event['sig_set'], interrupt_event['name']))

        for timer in config['timers']:
            if timer['sig_set'] is not None and timer['sig_set'] not in signal_set_names:
                msg = "Unknown signal-set '{}' in timer '{}'"
                raise SystemParseError(msg.format(timer['sig_set'], timer['name']))

        # Create a timer for each task
        for task in config['tasks']:
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


def assign_signal_vals(sig_sets):
    """Assign values to each signal in a list of signal sets.

    Values are assigned so that the values in each set are unique.

    A greedy algorithm is used to minimise the signal values used.

    A dictionary of signal values index by signal is returned.

    """
    signals = set().union(*sig_sets)
    possible_vals = set(range(len(signals)))

    assigned = {}
    for sig in signals:
        used_vals = [{assigned.get(ss) for ss in sig_set} for sig_set in sig_sets if sig in sig_set]
        assigned[sig] = min(possible_vals.difference(*used_vals))

    assert all(len({assigned[x] for x in sig_set}) == len(sig_set) for sig_set in sig_sets)

    return assigned


module = RigelModule()
